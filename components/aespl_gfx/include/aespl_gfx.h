/**
 * AESPL Graphics
 *
 * Author: Alexander Shepetko <a@shepetko.com>
 * License: MIT
 */

#include "esp_err.h"

typedef enum {
    AESPL_GFX_COLOR_MONO,
    AESPL_GFX_COLOR_RGB565,
} aespl_gfx_color_mode_t;

typedef struct {
    uint16_t width;                     // columns
    uint16_t height;                    // rows
    aespl_gfx_color_mode_t color_mode;  // color mode
    uint8_t ppw;                        // pixels per 2-byte word
    uint8_t wpr;                        // 2-byte words per row
    uint32_t **content;                 // pixels
} aespl_gfx_buf_t;

/**
 * @brief Initialize a buffer
 *
 * @param buf     Buffer
 * @param width   Width in pixels
 * @param height  Height in pixels
 * @param color   Color mode
 * @return
 *      - ESP_OK
 *      - ESP_ERR_NO_MEM
 */
esp_err_t aespl_gfx_buf_init(aespl_gfx_buf_t *buf, uint16_t width, uint16_t height, aespl_gfx_color_mode_t color);

/**
 * @brief Free resources allocated by aespl_gfx_buf_init()
 *
 * @param buf  Buffer
 */
void aespl_gfx_buf_free(aespl_gfx_buf_t *buf);

/**
 * @brief Clear a buffer
 *
 * @param buf  Buffer
 */
void aespl_gfx_buf_clear(aespl_gfx_buf_t *buf);


/**
 * @brief Print buffer's content
 *
 * @param buf  Buffer
 */
void aespl_gfx_buf_print(aespl_gfx_buf_t *buf);
