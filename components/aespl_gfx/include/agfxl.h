/**
 * @brief aGFXL -- ashep's Graphics Library
 *
 * @author    Alexander Shepetko <a@shepetko.com>
 * @copyright MIT License
 */

#ifndef _AGFXL_H_
#define _AGFXL_H_

/**
 * Errors
 */
typedef enum {
    AGFXL_OK = 0,
    AGFXL_FAIL = -1,
    AGFXL_BAD_ARG = -2,
    AGFXL_NO_MEM = -3,
    AGFXL_BAD_CHAR = -4,
} agfxl_err_t;

/**
 * Color modes
 */
typedef enum {
    AGFXL_C_MODE_MONO,
    AGFXL_C_MODE_RGB565,
    AGFXL_C_MODE_ARGB888,
} agfxl_c_mode_t;

/**
 * Mono colors
 */
typedef enum {
    AESPL_C_MONO_0,
    AESPL_C_MONO_1,
} agfxl_color_mono_t;

/**
 * Point
 */
typedef struct {
    int16_t x;
    int16_t y;
} agfxl_point_t;

#endif
