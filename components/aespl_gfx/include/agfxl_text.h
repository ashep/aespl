/**
 * @brief aGFXL -- ashep's Graphics Library
 *
 * @author    Alexander Shepetko <a@shepetko.com>
 * @copyright MIT License
 */

#ifndef _AGFXL_TEXT_H_
#define _AGFXL_TEXT_H_

#include <stdio.h>
#include "agfxl_buffer.h"

/**
 * Font row's sizes
 */
typedef enum {
    AGFXL_FONT_WIDTH_8 = 8,
    AGFXL_FONT_WIDTH_16 = 16,
} agfxl_font_width_t;

/**
 * Font
 */
typedef struct {
    uint8_t ascii_offset;      // char code offset relative to ASCII table
    uint8_t length;            // number of covered ASCII codes staring from ascii_offset
    agfxl_font_width_t width;  // number of bits per row
    uint8_t height;            // number of rows per character
    union {
        const uint8_t *c8;    // pointer to 1-byte content
        const uint16_t *c16;  // pointer to 2-byte content
    } content;
} agfxl_font_t;

/**
 * @brief Draw a character
 *
 * After drawing `ch_width` contains drawn character's width.
 *
 * @param buf      Buffer
 * @param font     Font
 * @param pos      Coordinates
 * @param ch       Character
 * @param color    Color
 *
 * @return Width of drawn character or `agfxl_err_t` in case of error
 */
int8_t agfxl_putc(agfxl_buf_t *buf, const agfxl_font_t *font, agfxl_point_t pos, uint8_t ch, uint32_t color);

/**
 * @brief Draw a string
 *
 * @param buf   Buffer
 * @param font  Font
 * @param pos   Coordinates
 * @param s     String
 * @param color Color
 * @param space Space between characters
 *
 * @return Position of the end of the drawn string
 */
agfxl_point_t agfxl_puts(agfxl_buf_t *buf, const agfxl_font_t *font, agfxl_point_t pos, const char *s,
                         uint32_t color, uint8_t space);

/**
 * @brief Get width of a character
 *
 * @param font Font
 * @param ch   Character
 *
 * @return Character's width or `agfxl_err_t` in case of error
 */
int8_t agfxl_ch_width(const agfxl_font_t *font, char ch);

/**
 * @brief Get width of a string
 *
 * @param font Font
 * @param str  String
 * @param space  Space between characters
 *
 * @return String's width or `agfxl_err_t` in case of error
 */
int16_t agfxl_str_width(const agfxl_font_t *font, const char *str, uint8_t space);

/**
 * @brief Create a graphics buffer and put a string into it
 *
 * @param c_mode Buffer color mode
 * @param font   Font
 * @param str    String
 * @param color  Text color
 * @param space  Space between characters
 *
 * @return Buffer or NULL in case of error
 */
agfxl_buf_t *agfxl_make_str_buf(agfxl_c_mode_t c_mode, const agfxl_font_t *font, const char *str, uint32_t color,
                                uint8_t space);

#endif
