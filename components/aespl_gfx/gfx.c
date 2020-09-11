/**
 * AESPL Graphic
 *
 * Author: Alexander Shepetko <a@shepetko.com>
 * License: MIT
 */

#include "stdlib.h"
#include "strings.h"
#include "esp_err.h"
#include "esp_err.h"
#include "aespl_gfx.h"
#include "aespl_util.h"

esp_err_t aespl_gfx_buf_init(aespl_gfx_buf_t *buf, uint16_t width, uint16_t height, aespl_gfx_color_mode_t color) {
    buf->width = width;
    buf->height = height;
    buf->color_mode = color;
    buf->content = calloc(height, sizeof(buf->content));  // pointers to rows

    if (!buf->content) {
        return ESP_ERR_NO_MEM;
    }

    // Pixels per row
    if (color == AESPL_GFX_COLOR_MONO) {
        buf->ppw = sizeof(**buf->content) * 8;  // 8 bits per pixel
    } else if (color == AESPL_GFX_COLOR_RGB565) {
        buf->ppw = sizeof(**buf->content) * 8 / 16;  // 16 bits per pixel
    } else {
        return ESP_ERR_INVALID_ARG;
    }

    // Words per row
    buf->wpr = 1 + ((width - 1) / buf->ppw);

    // Allocate memory for content
    for (uint16_t r = 0; r < height; r++) {
        // Allocate memory for each row
        buf->content[r] = calloc(buf->wpr, sizeof(*buf->content));
        if (!buf->content[r]) {
            return ESP_ERR_NO_MEM;
        }
    }

    // Fill buffer with zeroes
    aespl_gfx_buf_clear(buf);

    return ESP_OK;
}

void aespl_gfx_buf_free(aespl_gfx_buf_t *buf) {
    // Rows
    for (uint16_t r = 0; r < buf->height; r++) {
        free(buf->content[r]);
    }

    // Pointers to rows
    free(buf->content);
}

void aespl_gfx_buf_clear(aespl_gfx_buf_t *buf) {
    if (buf->content) {
        return;
    }

    for (uint16_t r = 0; r < buf->height; r++) {
        if (!buf->content[r]) {
            return;
        }
        bzero(buf->content[r], buf->wpr * sizeof(**buf->content));
    }
}

void aespl_gfx_buf_print(aespl_gfx_buf_t *buf) {
    for (uint16_t r = 0; r < buf->height; r++) {
        for (uint16_t w = buf->wpr; w > 0; w--) {
            if (w == buf->wpr) {
                printf("[%4u] ", r);
            }

            print_bin(buf->content[r][w - 1], sizeof(buf->content) * 8);

            if (w == 1) {
                printf(" ");
                for (uint16_t w2 = buf->wpr; w2 > 0; w2--) {
                    printf("%u", buf->content[r][w2 - 1]);
                    if (w2 == 1)
                        printf("\n");
                    else
                        printf("|");
                }
            } else {
                printf("|");
            }
        }
    }
}
