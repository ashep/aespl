/**
 * AESPL Button
 *
 * Author: Alexander Shepetko <a@shepetko.com>
 * License: MIT
 */

#ifndef AESPL_BUTTON_H
#define AESPL_BUTTON_H

#include "stdbool.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "driver/gpio.h"

#ifndef AESPL_BUTTON_L_PRESS_MS
#define AESPL_BUTTON_L_PRESS_MS 1000
#endif

#ifndef AESPL_BUTTON_L_PRESS_REPEAT_MS
#define AESPL_BUTTON_L_PRESS_REPEAT_MS 150
#endif

typedef void (*aespl_button_callback)();

typedef enum {
    AESPL_BUTTON_PRESS_HI,
    AESPL_BUTTON_PRESS_LOW,
} aespl_button_conn_type_t;

typedef struct {
    gpio_num_t pin;
    aespl_button_conn_type_t conn_type;
    bool is_pressed;
    bool is_l_press;
    TimerHandle_t l_press_timer;
    aespl_button_callback on_press;
    aespl_button_callback on_l_press;
    aespl_button_callback on_release;
} aespl_button_t;

esp_err_t aespl_button_init(aespl_button_t *btn, gpio_num_t pin, aespl_button_conn_type_t conn_type);
esp_err_t aespl_button_on_press(aespl_button_t *btn, aespl_button_callback handler);
esp_err_t aespl_button_on_l_press(aespl_button_t *btn, aespl_button_callback handler);
esp_err_t aespl_button_on_release(aespl_button_t *btn, aespl_button_callback handler);

#endif
