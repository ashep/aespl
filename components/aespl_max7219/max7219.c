/**
 * MAX7219 driver for ESP8266
 * 
 * Author: Alexander Shepetko <a@shepetko.com>
 * License: MIT
 */

#include "stdlib.h"
#include "string.h"
#include "strings.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp8266/eagle_soc.h"
#include "driver/gpio.h"
#include "aespl_max7219.h"

esp_err_t aespl_max7219_init(const aespl_max7219_config_t *cfg) {
    gpio_config_t gpio_cfg = {
        .pin_bit_mask = BIT(cfg->pin_cs) | BIT(cfg->pin_clk) | BIT(cfg->pin_data),
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .intr_type = GPIO_INTR_DISABLE,
    };

    return gpio_config(&gpio_cfg);
}

esp_err_t aespl_max7219_send(const aespl_max7219_config_t *cfg, aespl_max7219_addr_t addr, uint8_t data) {
    esp_err_t err;

    // Setup pins
    err = gpio_set_level(cfg->pin_cs, 0);
    if (err) {
        return err;
    }
    err = gpio_set_level(cfg->pin_clk, 0);
    if (err) {
        return err;
    }
    err = gpio_set_level(cfg->pin_data, 0);
    if (err) {
        return err;
    }

    // Prepare data frame
    uint16_t frame = ((uint16_t)addr) << 8 | data;
    for (int8_t i = 15; i >= 0; i--) {
        // Set data
        gpio_set_level(cfg->pin_data, 1 & frame >> i);

        // Load data on rising edge
        gpio_set_level(cfg->pin_clk, 1);
        gpio_set_level(cfg->pin_clk, 0);
    }

    return ESP_OK;
}

esp_err_t aespl_max7219_send_all(const aespl_max7219_config_t *cfg, aespl_max7219_addr_t addr, uint8_t data) {
    esp_err_t err = ESP_OK;

    for (uint8_t i = 0; i < cfg->n_displays; i++) {
        err = aespl_max7219_send(cfg, addr, data);
        if (err) {
            return err;
        }
    }

    return err;
}

esp_err_t aespl_max7219_latch(const aespl_max7219_config_t *cfg) {
    esp_err_t err;

    err = gpio_set_level(cfg->pin_cs, 1);
    if (err) {
        return err;
    }

    return gpio_set_level(cfg->pin_cs, 0);
}
