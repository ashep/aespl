/**
 * AESPL Button
 *
 * Author: Alexander Shepetko <a@shepetko.com>
 * License: MIT
 */

#include "stdio.h"
#include "stdbool.h"
#include "string.h"
#include "freertos/FreeRTOSConfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "rom/ets_sys.h"
#include "driver/hw_timer.h"
#include "driver/gpio.h"
#include "aespl_button.h"

static void gpio_l_press_callback(TimerHandle_t t) {
    aespl_button_t *btn = (aespl_button_t *)pvTimerGetTimerID(t);

    // Decrease timer period
    xTimerChangePeriodFromISR(t, pdMS_TO_TICKS(AESPL_BUTTON_L_PRESS_REPEAT_MS), NULL);

    btn->is_l_press = true;
    btn->on_l_press(btn->pin);
}

static void IRAM_ATTR gpio_isr(void *button) {
    bool is_pressed = false;
    aespl_button_t *btn = (aespl_button_t *)button;

    // Get current state of the button
    uint8_t lvl = gpio_get_level(btn->pin);
    if ((btn->conn_type == AESPL_BUTTON_PRESS_LOW && !lvl) || (btn->conn_type == AESPL_BUTTON_PRESS_HI && lvl)) {
        is_pressed = true;
    } else if (btn->is_pressed) {
        is_pressed = false;
    }

    // Debounce
    if ((is_pressed && btn->is_pressed) || (!is_pressed && !btn->is_pressed)) {
        return;
    }

    btn->is_pressed = is_pressed;

    if (is_pressed) {
        // Call press hook
        if (btn->on_press) {
            btn->on_press(btn->pin);
        }

        // Start long press timer
        if (btn->l_press_timer && xTimerStartFromISR(btn->l_press_timer, NULL) != pdPASS) {
            ets_printf("Error while starting timer on pin %d\n", btn->pin);
        }

    } else {
        // Call release hook
        if (btn->on_release) {
            btn->on_release(btn->pin);
        }

        // Button is not under long press anymore
        if (btn->is_l_press) {
            btn->is_l_press = false;
        }

        // Stop and re-initialize long press timer
        if (btn->l_press_timer && xTimerIsTimerActive(btn->l_press_timer) == pdTRUE) {
            if (xTimerChangePeriodFromISR(btn->l_press_timer, pdMS_TO_TICKS(AESPL_BUTTON_L_PRESS_MS), NULL) != pdPASS) {
                ets_printf("Error while setting timer period on pin %d\n", btn->pin);
            }

            if (xTimerStopFromISR(btn->l_press_timer, NULL) != pdPASS) {
                ets_printf("Error while stopping timer on pin %d\n", btn->pin);
            }
        }
    }
}

esp_err_t aespl_button_init(aespl_button_t *btn, gpio_num_t pin, aespl_button_conn_type_t conn_type) {
    esp_err_t err;

    memset(btn, 0, sizeof(*btn));

    btn->pin = pin;
    btn->conn_type = conn_type;

    gpio_config_t gpio_cfg = {
        1UL << btn->pin,
        GPIO_MODE_INPUT,
        GPIO_PULLUP_ENABLE,
        GPIO_PULLDOWN_DISABLE,
        GPIO_INTR_ANYEDGE,
    };

    if (conn_type == AESPL_BUTTON_PRESS_LOW) {
        gpio_cfg.pull_up_en = GPIO_PULLUP_ENABLE;
        gpio_cfg.pull_down_en = GPIO_PULLDOWN_DISABLE;
    } else {
        gpio_cfg.pull_up_en = GPIO_PULLUP_DISABLE;
        gpio_cfg.pull_down_en = GPIO_PULLDOWN_ENABLE;
    }

    err = gpio_config(&gpio_cfg);
    if (err) {
        return err;
    }

    return gpio_isr_handler_add(btn->pin, gpio_isr, btn);
}

esp_err_t aespl_button_on_press(aespl_button_t *btn, aespl_button_callback handler) {
    btn->on_press = handler;

    return ESP_OK;
}

esp_err_t aespl_button_on_l_press(aespl_button_t *cfg, aespl_button_callback handler) {
    cfg->l_press_timer = xTimerCreate("GPIO", pdMS_TO_TICKS(AESPL_BUTTON_L_PRESS_MS), pdTRUE, cfg, gpio_l_press_callback);
    cfg->on_l_press = handler;

    return ESP_OK;
}

esp_err_t aespl_button_on_release(aespl_button_t *btn, aespl_button_callback handler) {
    btn->on_release = handler;

    return ESP_OK;
}
