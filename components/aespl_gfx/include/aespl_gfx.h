/**
 * Graphics Library for ESP8266
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
    AESPL_GFX_CMODE_MONO,
    AESPL_GFX_CMODE_RGB565,
    AESPL_GFX_CMODE_ARGB888,
} aespl_gfx_cmode_t;

/**
 * Common colors
 */
typedef enum {
    AESPL_C_MONO_0,
    AESPL_C_MONO_1,
} aespl_gfx_color_mono_t;

/**
 * Graphics buffer
 */
typedef struct {
    uint16_t width;           // columns
    uint16_t height;          // rows
    aespl_gfx_cmode_t color;  // color mode
    uint8_t ppw;              // pixels per 2-byte word
    uint8_t wpr;              // 2-byte words per row
    uint32_t **content;       // pixels
} aespl_gfx_buf_t;

/**
 * Point coordinates
 */
typedef struct {
    int16_t x;
    int16_t y;
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
 * @param buf    Buffer
 * @param width  Width in pixels
 * @param height Height in pixels
 * @param color  Color mode
 * @return
 *      - ESP_OK
 *      - ESP_ERR_NO_MEM
 */
esp_err_t aespl_gfx_init_buf(aespl_gfx_buf_t *buf, uint16_t width, uint16_t height, aespl_gfx_cmode_t color);

/**
 * @brief Free resources allocated by aespl_gfx_init_buf()
 *
 * @param buf Buffer
 */
void aespl_gfx_free_buf(aespl_gfx_buf_t *buf);

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
 * @param buf   Buffer
 * @param x     X position
 * @param y     Y position
 * @param color Color value
 * @return
 *      - ESP_OK
 *      - ESP_ERR_INVALID_ARG
 */
esp_err_t aespl_gfx_set_px(aespl_gfx_buf_t *buf, uint16_t x, uint16_t y, uint32_t color);

/**
 * @brief Get a pixel
 *
 * @param buf   Buffer
 * @param x     X position
 * @param y     Y position
 * @param color Color value
 * @return
 *      - ESP_OK
 *      - ESP_ERR_INVALID_ARG
 */
esp_err_t aespl_gfx_get_px(const aespl_gfx_buf_t *buf, uint16_t x, uint16_t y, uint32_t *color);

/**
 * @brief Merge the `src` buffer into the `dst` buffer
 *
 * @param dst Target buffer
 * @param src Source buffer
 * @param pos Target buffer coordinates
 * @return
 *      - ESP_OK
 *      - ESP_ERR_INVALID_ARG
 */
esp_err_t aespl_gfx_merge(aespl_gfx_buf_t *dst, const aespl_gfx_buf_t *src, aespl_gfx_point_t pos);

/**
 * @brief Move buffer
 *
 * @param buf     Buffer
 * @param rel_pos Relative position
 * @return
 *      - ESP_OK
 *      - ESP_ERR_NO_MEM
 *      - ESP_ERR_INVALID_ARG
 */
esp_err_t aespl_gfx_move(aespl_gfx_buf_t *buf, aespl_gfx_point_t rel_pos);

/**
 * @brief Draw a line
 *
 * @param buf   Buffer
 * @param line  Line
 * @param color Color
 * @return
 *      - ESP_OK
 */
esp_err_t aespl_gfx_line(aespl_gfx_buf_t *buf, const aespl_gfx_line_t *line, uint32_t color);

/**
 * @brief Draw a polygon
 *
 * @param buf   Buffer
 * @param poly  Points
 * @param color Color
 * @return
 *      - ESP_OK
 *      - ESP_ERR_INVALID_ARG
 */
esp_err_t aespl_gfx_poly(aespl_gfx_buf_t *buf, const aespl_gfx_poly_t *poly, uint32_t color);

/**
 * @brief Draw a rectangle
 *
 * @param buf   Buffer
 * @param p1    Top left point
 * @param p2    Bottom right point
 * @param color Color
 * @return
 *      - ESP_OK
 *      - ESP_ERR_INVALID_ARG
 */
esp_err_t aespl_gfx_rect(aespl_gfx_buf_t *buf, const aespl_gfx_point_t p1, const aespl_gfx_point_t p2, uint32_t color);

/**
 * @brief Draw a triangle
 *
 * @param buf   Buffer
 * @param p1    Point 1
 * @param p2    Point 2
 * @param p3    Point 3
 * @param color Color
 * @return
 *      - ESP_OK
 *      - ESP_ERR_INVALID_ARG
 */
esp_err_t aespl_gfx_tri(aespl_gfx_buf_t *buf, const aespl_gfx_point_t p1, const aespl_gfx_point_t p2,
                        const aespl_gfx_point_t p3, uint32_t color);

/**
 * @brief Draw a character
 *
 * @param buf      Buffer
 * @param font     Font
 * @param pos    Coordinates
 * @param ch       Character
 * @param color    Color
 * @param ch_width Drawn character width
 * @return
 *      - ESP_OK
 *      - ESP_ERR_INVALID_ARG
 */
esp_err_t aespl_gfx_putc(aespl_gfx_buf_t *buf, const aespl_gfx_font_t *font, aespl_gfx_point_t pos, char ch,
                         uint32_t color, uint8_t *ch_width);

/**
 * @brief Draw a string
 *
 * @param buf      Buffer
 * @param font     Font
 * @param pos    Coordinates
 * @param ch       Character
 * @param color    Color
 * @param ch_width Drawn character width
 * @return
 *      - ESP_OK
 *      - ESP_ERR_INVALID_ARG
 */
esp_err_t aespl_gfx_puts(aespl_gfx_buf_t *buf, const aespl_gfx_font_t *font, aespl_gfx_point_t *pos, char *s,
                         uint32_t color, uint8_t space);

#endif
