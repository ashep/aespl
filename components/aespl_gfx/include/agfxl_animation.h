/**
 * @brief aGFXL -- ashep's Graphics Library
 *
 * @author    Alexander Shepetko <a@shepetko.com>
 * @copyright MIT License
 */

#ifndef _AGFXL_ANIMATION_H_
#define _AGFXL_ANIMATION_H_

#ifndef INC_FREERTOS_H
#error "include FreeRTOS.h must appear in source files before include this file"
#endif

#include <stdio.h>
#include "agfxl_buffer.h"

/**
 * Animation state
 */
typedef enum {
    AGFXL_ANIM_CONTINUE,
    AGFXL_ANIM_RESTART,
    AGFXL_ANIM_STOP,
} agfxl_anim_state_t;

/**
 * Animation callback
 */
typedef agfxl_anim_state_t (*agfxl_animator_t)(agfxl_buf_t *dst, agfxl_buf_t *src, void *args, uint32_t frame_n);

/**
 * Animation structure
 */
typedef struct {
    agfxl_buf_t *dst;
    agfxl_buf_t *src;
    agfxl_animator_t animator;
    void *args;
    uint8_t fps;
    uint32_t frame_n;
    agfxl_anim_state_t state;

} agfxl_animation_t;

/**
 * @brief Start animation
 *
 * @param dst Destination buffer
 * @param src Source buffer
 * @param fn  Animation callback
 * @param fps Frames per second
 */
agfxl_animation_t *agfxl_animate(agfxl_buf_t *dst, agfxl_buf_t *src, agfxl_animator_t fn, void *args, uint8_t fps);

#endif
