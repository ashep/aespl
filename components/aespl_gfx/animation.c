/**
 * @brief aGFXL -- ashep's Graphics Library
 *
 * @author    Alexander Shepetko <a@shepetko.com>
 * @copyright MIT License
 */

#include <stdio.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "agfxl.h"
#include "agfxl_animation.h"

static void animate_task(void *args) {
    agfxl_anim_state_t res = AGFXL_ANIM_CONTINUE;
    agfxl_animation_t *anim = (agfxl_animation_t *)args;
    TickType_t delay = pdMS_TO_TICKS(1000 / anim->fps);

    for (;;) {
        if (anim->state == AGFXL_ANIM_RESTART) {
            anim->frame_n = 0;
            anim->state = AGFXL_ANIM_CONTINUE;
        }

        if (anim->state == AGFXL_ANIM_CONTINUE) {
            anim->state = anim->animator(anim->dst, anim->src, anim->args, anim->frame_n++);
            vTaskDelay(delay);
        }

        if (anim->state == AGFXL_ANIM_STOP) {
            free(anim);
            break;
        }
    }

    vTaskDelete(NULL);
}

agfxl_animation_t *agfxl_animate(agfxl_buf_t *dst, agfxl_buf_t *src, agfxl_animator_t fn, void *args, uint8_t fps) {
    agfxl_animation_t *anim = malloc(sizeof(agfxl_animation_t));
    if (!anim) {
        return NULL;
    }

    anim->dst = dst;
    anim->src = src;
    anim->animator = fn;
    anim->args = args;
    anim->fps = fps;
    anim->frame_n = 0;
    anim->state = AGFXL_ANIM_CONTINUE;

    xTaskCreate(animate_task, "animation", 4096, (void *)anim, 0, NULL);

    return anim;
}
