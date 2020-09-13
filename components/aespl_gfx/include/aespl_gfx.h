/**
 * AESPL Graphics
 *
 * Author: Alexander Shepetko <a@shepetko.com>
 * License: MIT
 */

#ifndef _AESPL_GFX_H_
#define _AESPL_GFX_H_

#include "stdint.h"
#include "esp_err.h"

/**
 * Color modes
 */
typedef enum {
    AESPL_GFX_COLOR_MONO,
    AESPL_GFX_COLOR_RGB565,
    AESPL_GFX_COLOR_ARGB888,
} aespl_gfx_color_t;

/**
 * Graphics buffer
 */
typedef struct {
    uint16_t width;           // columns
    uint16_t height;          // rows
    aespl_gfx_color_t color;  // color mode
    uint8_t ppw;              // pixels per 2-byte word
    uint8_t wpr;              // 2-byte words per row
    uint32_t **content;       // pixels
} aespl_gfx_buf_t;

/**
 * Point coordinates
 */
typedef struct {
    uint16_t x;
    uint16_t y;
} aespl_gfx_point_t;

/**
 * Line
 */
typedef struct {
    aespl_gfx_point_t p1;
    aespl_gfx_point_t p2;
} aespl_gfx_line_t;

/**
 * Polygon
 */
typedef struct {
    uint8_t n_corners;
    aespl_gfx_point_t *corners;
} aespl_gfx_poly_t;

/**
 * Font widths
 */
typedef enum {
    AESPL_GFX_FONT_WIDTH_8 = 8,
    AESPL_GFX_FONT_WIDTH_16 = 16,
    AESPL_GFX_FONT_WIDTH_32 = 32,
    AESPL_GFX_FONT_WIDTH_64 = 64,
} aespl_gfx_font_width_t;

/**
 * Font
 */
typedef struct {
    uint8_t ascii_offset;          // char code offset relative to ASCII table
    uint8_t size;                  // number of covered ASCII codes staring from ascii_offset
    aespl_gfx_font_width_t width;  // number of bits per row
    uint8_t height;                // number of rows per character
    union {
        const uint8_t *c8;    // pointer to 1-byte content
        const uint16_t *c16;  // pointer to 2-byte content
        const uint32_t *c32;  // pointer to 4-byte content
        const uint64_t *c64;  // pointer to 8-byte content
    } content;
} aespl_gfx_font_t;

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
esp_err_t aespl_gfx_init_buf(aespl_gfx_buf_t *buf, uint16_t width, uint16_t height, aespl_gfx_color_t color);

/**
 * @brief Free resources allocated by aespl_gfx_buf_init()
 *
 * @param buf Buffer
 */
void aespl_gfx_free_buf(aespl_gfx_buf_t *buf);

/**
 * @brief Clear a buffer
 *
 * @param buf Buffer
 */
void aespl_gfx_clear_buf(aespl_gfx_buf_t *buf);

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
 * @param poly[in]  Points
 * @param color[in] Color
 * @return
 *      - ESP_OK
 *      - ESP_ERR_INVALID_ARG
 */
esp_err_t aespl_gfx_poly(aespl_gfx_buf_t *buf, const aespl_gfx_poly_t *poly, uint32_t color);

/**
 * @brief Draw a rectangle
 *
 * @param buf[out]  Buffer
 * @param p1[in]    Top left point
 * @param p2[in]    Bottom right point
 * @param color[in] Color
 * @return
 *      - ESP_OK
 *      - ESP_ERR_INVALID_ARG
 */
esp_err_t aespl_gfx_rect(aespl_gfx_buf_t *buf, const aespl_gfx_point_t p1, const aespl_gfx_point_t p2, uint32_t color);

/**
 * @brief Draw a triangle
 *
 * @param buf[out]  Buffer
 * @param p1[in]    Point 1
 * @param p2[in]    Point 2
 * @param p3[in]    Point 3
 * @param color[in] Color
 * @return
 *      - ESP_OK
 *      - ESP_ERR_INVALID_ARG
 */
esp_err_t aespl_gfx_tri(aespl_gfx_buf_t *buf, const aespl_gfx_point_t p1, const aespl_gfx_point_t p2,
                        const aespl_gfx_point_t p3, uint32_t color);

/**
 * @brief Draw a character
 *
 * @param buf[out]  Buffer
 * @param font[in]  Font
 * @param point[in] Coordinates
 * @param ch[in]    Character
 * @param color[in] Color
 * @return
 *      - ESP_OK
 *      - ESP_ERR_INVALID_ARG
 */
esp_err_t aespl_gfx_putc(aespl_gfx_buf_t *buf, const aespl_gfx_font_t *font, aespl_gfx_point_t point, char ch,
                         uint32_t color);

#endif
