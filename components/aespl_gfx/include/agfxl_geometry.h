/**
 * @brief aGFXL -- ashep's Graphics Library
 *
 * @author    Alexander Shepetko <a@shepetko.com>
 * @copyright MIT License
 */

#ifndef _AGFXL_GEOMETRY_H_
#define _AGFXL_GEOMETRY_H_

#include <stdio.h>
#include "agfxl.h"
#include "agfxl_buffer.h"

/**
 * Line
 */
typedef struct {
    agfxl_point_t p1;
    agfxl_point_t p2;
} agfxl_line_t;

/**
 * Polygon
 */
typedef struct {
    uint8_t n_corners;
    agfxl_point_t *corners;
} agfxl_poly_t;

/**
 * @brief Draw a line
 *
 * @param buf   Buffer
 * @param line  Line
 * @param color Color
 */
agfxl_err_t agfxl_line(agfxl_buf_t *buf, const agfxl_line_t *line, uint32_t color);

/**
 * @brief Draw a polygon
 *
 * @param buf   Buffer
 * @param poly  Points
 * @param color Color
 */
agfxl_err_t agfxl_poly(agfxl_buf_t *buf, const agfxl_poly_t *poly, uint32_t color);

/**
 * @brief Draw a rectangle
 *
 * @param buf   Buffer
 * @param p1    Top left point
 * @param p2    Bottom right point
 * @param color Color
 */
agfxl_err_t agfxl_rect(agfxl_buf_t *buf, const agfxl_point_t p1, const agfxl_point_t p2, uint32_t color);

/**
 * @brief Draw a triangle
 *
 * @param buf   Buffer
 * @param p1    Point 1
 * @param p2    Point 2
 * @param p3    Point 3
 * @param color Color
 */
agfxl_err_t agfxl_tri(agfxl_buf_t *buf, const agfxl_point_t p1, const agfxl_point_t p2,
                      const agfxl_point_t p3, uint32_t color);

#endif
