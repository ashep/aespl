/**
 * AESPL Button
 *
 * Author: Alexander Shepetko <a@shepetko.com>
 * License: MIT
 *
 * Before using this component `gpio_install_isr_service()` should be called.
 */

#ifndef AESPL_BUTTON_H
#define AESPL_BUTTON_H

#include "stdbool.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "driver/gpio.h"

/**
 * Delay after which a button considered as long-pressed
 */
#ifndef AESPL_BUTTON_L_PRESS_MS
#define AESPL_BUTTON_L_PRESS_MS 1000
#endif

/*
 * Delay bertwen long press callback calls
 */
#ifndef AESPL_BUTTON_L_PRESS_REPEAT_MS
#define AESPL_BUTTON_L_PRESS_REPEAT_MS 150
#endif

/**
 * Button callback signature
 */
typedef void (*aespl_button_callback)(gpio_num_t pin);

/**
 * Button connection types
 * It means where the other pin of the button is connected to:
 *   AESPL_BUTTON_PRESS_HI -- button connected to VCC
 *   AESPL_BUTTON_PRESS_LOW -- button connected to GND
 */
typedef enum {
    AESPL_BUTTON_PRESS_HI,
    AESPL_BUTTON_PRESS_LOW,
} aespl_button_conn_type_t;

/**
 * Button configuration structure
 */
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

/**
 * @brief Initialize a button
 *
 * @param btn       Button's configuration
 * @param pin       GPIO pin where the button is connected
 * @param conn_type Button connection type
 */
esp_err_t aespl_button_init(aespl_button_t *btn, gpio_num_t pin, aespl_button_conn_type_t conn_type);

/**
 * @brief Setup button's press callback
 *
 * @param btn     Button's configuration
 * @param handler Callback function
 */
esp_err_t aespl_button_on_press(aespl_button_t *btn, aespl_button_callback handler);

/**
 * @brief Setup button's long press callback
 *
 * @param btn     Button's configuration
 * @param handler Callback function
 */
esp_err_t aespl_button_on_l_press(aespl_button_t *btn, aespl_button_callback handler);

/**
 * @brief Setup button's release callback
 *
 * @param btn     Button's configuration
 * @param handler Callback function
 */
esp_err_t aespl_button_on_release(aespl_button_t *btn, aespl_button_callback handler);

#endif
