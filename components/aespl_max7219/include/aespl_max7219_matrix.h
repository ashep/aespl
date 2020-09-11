/**
 * MAX7219 driver for ESP8266
 * 
 * Author: Alexander Shepetko <a@shepetko.com>
 * License: MIT
 */

#ifndef _AESPL_MAX7219_MATRIX_H_
#define _AESPL_MAX7219_MATRIX_H_

#include "driver/gpio.h"
#include "aespl_max7219.h"

/**
 * MAX7219 matrix configuration structure
 */
typedef struct {
    aespl_max7219_config_t max7219cfg;
    uint8_t displays_x;
    uint8_t displays_y;
} aespl_max7219_matrix_config_t;

/**
 * Initialize MAX7219 matrix device(s)
 */
esp_err_t aespl_max7219_matrix_init(aespl_max7219_matrix_config_t *cfg);

#endif
