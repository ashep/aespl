/**
 * MAX7219 matrix driver for ESP8266
 *
 * Author: Alexander Shepetko <a@shepetko.com>
 * License: MIT
 */

#include "esp_err.h"
#include "aespl_max7219.h"
#include "aespl_max7219_matrix.h"

esp_err_t aespl_max7219_matrix_init(aespl_max7219_matrix_config_t *cfg) {
    esp_err_t err;

    cfg->max7219cfg.decode = AESPL_MAX7219_DECODE_NONE;
    cfg->max7219cfg.n_displays = cfg->displays_x * cfg->displays_y;

    err = aespl_max7219_init(&cfg->max7219cfg);

    return err;
}
