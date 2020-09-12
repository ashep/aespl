/**
 * AESPL Graphics
 *
 * Author: Alexander Shepetko <a@shepetko.com>
 * License: MIT
 */

#include "stdint.h"
#include "esp_err.h"

typedef enum {
    AESPL_GFX_COLOR_MONO,
    AESPL_GFX_COLOR_RGB565,
    AESPL_GFX_COLOR_ARGB888,
} aespl_gfx_color_t;

typedef struct {
    uint16_t width;           // columns
    uint16_t height;          // rows
    aespl_gfx_color_t color;  // color mode
    uint8_t ppw;              // pixels per 2-byte word
    uint8_t wpr;              // 2-byte words per row
    uint32_t **content;       // pixels
} aespl_gfx_buf_t;

/**
 * @brief Initialize a buffer
 *
 * @param buf[out]   Buffer
 * @param width[in]  Width in pixels
 * @param height[in] Height in pixels
 * @param color[in]  Color mode
 * @return
 *      - ESP_OK
 *      - ESP_ERR_NO_MEM
 */
esp_err_t aespl_gfx_buf_init(aespl_gfx_buf_t *buf, uint16_t width, uint16_t height, aespl_gfx_color_t color);

/**
 * @brief Free resources allocated by aespl_gfx_buf_init()
 *
 * @param buf Buffer
 */
void aespl_gfx_buf_free(aespl_gfx_buf_t *buf);

/**
 * @brief Clear a buffer
 *
 * @param buf Buffer
 */
void aespl_gfx_buf_clear(aespl_gfx_buf_t *buf);

/**
 * @brief Print buffer's content
 *
 * @param buf Buffer
 */
void aespl_gfx_buf_print(const aespl_gfx_buf_t *buf);

/**
 * @brief Set a pixel
 *
 * @param buf[out]  Buffer
 * @param x[in]     X position
 * @param y[in]     Y position
 * @param value[in] Pixel value
 * @return
 *      - ESP_OK
 *      - ESP_ERR_INVALID_ARG
 */
esp_err_t aespl_gfx_buf_set_px(aespl_gfx_buf_t *buf, uint16_t x, uint16_t y, uint32_t value);

/**
 * @brief Get a pixel
 *
 * @param buf[in]  Buffer
 * @param x[in]     X position
 * @param y[in]     Y position
 * @param value[out] Pixel value
 * @return
 *      - ESP_OK
 *      - ESP_ERR_INVALID_ARG
 */
esp_err_t aespl_gfx_buf_get_px(const aespl_gfx_buf_t *buf, uint16_t x, uint16_t y, uint32_t *value);
