/**
 * @brief     AESPL graphics
 * @author    Alexander Shepetko <a@shepetko.com>
 * @copyright MIT License
 */

#ifndef _AESPL_GFX_H_
#define _AESPL_GFX_H_

/**
 * Errors.
 */
typedef enum {
    AESPL_GFX_OK = 0,
    AESPL_GFX_FAIL = -1,
    AESPL_GFX_BAD_ARG = -2,
    AESPL_GFX_NO_MEM = -3,
    AESPL_GFX_BAD_CHAR = -4,
} aespl_gfx_err_t;

/**
 * Point.
 */
typedef struct {
    int16_t x;
    int16_t y;
} aespl_gfx_point_t;

#endif
