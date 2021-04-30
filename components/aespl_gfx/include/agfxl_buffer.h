/**
 * @brief aGFXL -- ashep's Graphics Library
 *
 * @author    Alexander Shepetko <a@shepetko.com>
 * @copyright MIT License
 */

#ifndef _AGFXL_BUFFER_H_
#define _AGFXL_BUFFER_H_

#include <stdint.h>
#include "agfxl.h"

/**
 * Graphics buffer
 */
typedef struct {
    uint16_t width;         // columns
    uint16_t height;        // rows
    agfxl_c_mode_t c_mode;  // color mode
    uint8_t ppw;            // pixels per word
    uint8_t wpr;            // words per row
    uint32_t **content;     // pixels
} agfxl_buf_t;

/**
 * Array of graphic buffers
 */
typedef struct {
    uint16_t length;        // number of buffers
    agfxl_c_mode_t c_mode;  // color mode
    agfxl_buf_t **buffers;  // buffers
} agfxl_buf_array_t;

/**
 * @brief Initialize a buffer
 *
 * @param buf    Buffer structure to initialize
 * @param width  Width in pixels
 * @param height Height in pixels
 * @param c_mode Color mode
 *
 * @return Allocated buffer or NULL in case of error
 */
agfxl_buf_t *agfxl_make_buf(uint16_t width, uint16_t height, agfxl_c_mode_t c_mode);

/**
 * @brief Free resources allocated by `agfxl_make_buf()`
 *
 * @param buf Buffer
 */
void agfxl_free_buf(agfxl_buf_t *buf);

/**
 * @brief Initialize a buffers array
 *
 * @param length Number of buffers
 * @param width  Width of each buffer
 * @param height Height of each buffer
 * @param c_mode Color mode
 * @returns      Buffers array or NULL in case of error
 */
agfxl_buf_array_t *agfxl_make_buf_array(uint8_t length, uint16_t width, uint16_t height, agfxl_c_mode_t c_mode);

/**
 * @brief Free resources allocated by `agfxl_init_buf_array()`
 *
 * @param buf_arr Buffers array
 */
void agfxl_free_buf_array(agfxl_buf_array_t *buf_arr);

/**
 * @brief Clear a buffer
 *
 * @param buf Buffer
 */
void agfxl_clear_buf(agfxl_buf_t *buf);

/**
 * @brief Dump buffer's content to the console
 *
 * @param buf Buffer
 */
void agfxl_dump_buf(const agfxl_buf_t *buf);

/**
 * @brief Set a pixel
 *
 * @param buf   Buffer
 * @param x     X position
 * @param y     Y position
 * @param color Color value
 *
 * @return Set pixel's value or 0 if `color` wasn't 0, but pixel wasn't set
 */
uint32_t agfxl_set_px(agfxl_buf_t *buf, int16_t x, int16_t y, uint32_t color);

/**
 * @brief Get a pixel
 *
 * @param buf   Buffer
 * @param x     X position
 * @param y     Y position
 *
 * @return Pixel's value
 */
uint32_t agfxl_get_px(const agfxl_buf_t *buf, int16_t x, int16_t y);

/**
 * @brief Merge two buffers
 *
 * @param dst     Target buffer
 * @param src     Source buffer
 * @param dst_pos Coordinates on the target buffer
 * @param src_pos Coordinates on the source buffer
 */
agfxl_err_t agfxl_merge(agfxl_buf_t *dst, const agfxl_buf_t *src, agfxl_point_t dst_pos, agfxl_point_t src_pos);

/**
 * @brief Split a buffer
 *
 * @param src   Source buffer
 * @param num_x X parts
 * @param num_y Y parts
 *
 * @return Buffers array or NULL in case of error
 */
agfxl_buf_array_t *agfxl_split_buf(const agfxl_buf_t *src, uint8_t num_x, uint8_t num_y);

/**
 * @brief Move buffer's content to a new position
 *
 * @param buf     Buffer
 * @param rel_pos Relative position
 */
agfxl_err_t agfxl_move_buf(agfxl_buf_t *buf, agfxl_point_t rel_pos);

#endif
