/**
 * MAX7219 Matrix Driver for ESP8266
 *
 * Author: Alexander Shepetko <a@shepetko.com>
 * License: MIT
 */

#include "stdbool.h"
#include "esp_err.h"
#include "agfxl.h"
#include "aespl_common.h"
#include "aespl_max7219.h"
#include "aespl_max7219_matrix.h"

esp_err_t aespl_max7219_matrix_init(aespl_max7219_matrix_config_t *cfg, const aespl_max7219_config_t *m7219cfg,
                                    uint8_t disp_x, uint8_t disp_y) {
    cfg->max7219 = m7219cfg;
    cfg->disp_x = disp_x;
    cfg->disp_y = disp_y;

    return ESP_OK;
}

esp_err_t aespl_max7219_matrix_draw(const aespl_max7219_matrix_config_t *cfg, agfxl_buf_t *buf) {
    esp_err_t err;
    agfxl_buf_array_t b_arr;
    err = agfxl_split(&b_arr, buf, cfg->disp_x, cfg->disp_y);
    if (err) {
        return err;
    }

    for (uint8_t row_n = 1; row_n <= 8; row_n++) {
        for (int8_t n = b_arr.length - 1; n >= 0; n--) {
            uint8_t row_data = *(b_arr.buffers[n].content[row_n - 1]) >> 24;
            err = aespl_max7219_send(cfg->max7219, row_n, row_data, false);
            if (err) {
                return err;
            }
        }

        err = aespl_max7219_latch(cfg->max7219);
        if (err) {
            return err;
        }
    }

    agfxl_free_buf_array(&b_arr);

    return ESP_OK;
}
