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

typedef struct {
    uint16_t x;
    uint16_t y;
} aespl_gfx_point_t;

typedef struct {
    aespl_gfx_point_t p1;
    aespl_gfx_point_t p2;
} aespl_gfx_line_t;

typedef struct {
    uint8_t n_corners;
    aespl_gfx_point_t *corners;
} aespl_gfx_poly_t;

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
esp_err_t aespl_gfx_init(aespl_gfx_buf_t *buf, uint16_t width, uint16_t height, aespl_gfx_color_t color);

/**
 * @brief Free resources allocated by aespl_gfx_buf_init()
 *
 * @param buf Buffer
 */
void aespl_gfx_free(aespl_gfx_buf_t *buf);

/**
 * @brief Clear a buffer
 *
 * @param buf Buffer
 */
void aespl_gfx_clear(aespl_gfx_buf_t *buf);

/**
 * @brief Print buffer's content
 *
 * @param buf Buffer
 */
void aespl_gfx_print_buf(const aespl_gfx_buf_t *buf);

/**
 * @brief Set a pixel
 *
 * @param buf[out]  Buffer
 * @param x[in]     X position
 * @param y[in]     Y position
 * @param value[in] Color value
 * @return
 *      - ESP_OK
 *      - ESP_ERR_INVALID_ARG
 */
esp_err_t aespl_gfx_set_px(aespl_gfx_buf_t *buf, uint16_t x, uint16_t y, uint32_t value);

/**
 * @brief Get a pixel
 *
 * @param buf[in]    Buffer
 * @param x[in]      X position
 * @param y[in]      Y position
 * @param value[out] Color value
 * @return
 *      - ESP_OK
 *      - ESP_ERR_INVALID_ARG
 */
esp_err_t aespl_gfx_get_px(const aespl_gfx_buf_t *buf, uint16_t x, uint16_t y, uint32_t *value);

/**
 * @brief Draw a line
 *
 * @param buf[out]  Buffer
 * @param line[in]  Line
 * @param color[in] Color
 * @return
 *      - ESP_OK
 *      - ESP_ERR_INVALID_ARG
 */
esp_err_t aespl_gfx_line(aespl_gfx_buf_t *buf, const aespl_gfx_line_t *line, uint32_t color);

/**
 * @brief Draw a polygon
 *
 * @param buf[out]  Buffer
 * @param poly[in]  Polygon
 * @param color[in] Color
 * @return
 *      - ESP_OK
 *      - ESP_ERR_INVALID_ARG
 */
esp_err_t aespl_gfx_poly(aespl_gfx_buf_t *buf, const aespl_gfx_poly_t *poly, uint32_t color);
