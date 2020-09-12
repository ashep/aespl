/**
 * AESPL Graphic
 *
 * Author: Alexander Shepetko <a@shepetko.com>
 * License: MIT
 */

#include "stdint.h"
#include "stdlib.h"
#include "strings.h"
#include "esp_err.h"
#include "esp_err.h"
#include "aespl_gfx.h"
#include "aespl_util.h"

esp_err_t aespl_gfx_buf_init(aespl_gfx_buf_t *buf, uint16_t width, uint16_t height, aespl_gfx_color_t color) {
    buf->width = width;
    buf->height = height;
    buf->color = color;
    buf->content = calloc(height, sizeof(buf->content));  // pointers to rows

    if (!buf->content) {
        return ESP_ERR_NO_MEM;
    }

    // Pixels per row
    switch (color) {
        case AESPL_GFX_COLOR_MONO:
            buf->ppw = sizeof(**buf->content) * 8;  // 8 bits per pixel,
            break;
        case AESPL_GFX_COLOR_RGB565:
            buf->ppw = sizeof(**buf->content) * 8 / 16;  // 16 bits per pixel
            break;
        case AESPL_GFX_COLOR_ARGB888:
            buf->ppw = 1;  // 32 bit per pixel
            break;
        default:
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

void aespl_gfx_buf_print(const aespl_gfx_buf_t *buf) {
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

esp_err_t aespl_gfx_buf_set_px(aespl_gfx_buf_t *buf, uint16_t x, uint16_t y, uint32_t value) {
    if (x >= buf->width || y >= buf->height) {
        return ESP_ERR_INVALID_ARG;
    }

    size_t word_bits = sizeof(**buf->content) * 8;
    uint16_t word_n = buf->wpr - 1 - x / buf->ppw;

    switch (buf->color) {
        case AESPL_GFX_COLOR_MONO:
            buf->content[y][word_n] |= 1 << (word_bits - x - 1 % word_bits);
            break;

        case AESPL_GFX_COLOR_RGB565:
            buf->content[y][word_n] |= value << ((x % 2) ? 0 : 16);
            break;

        case AESPL_GFX_COLOR_ARGB888:
            buf->content[y][word_n] = value;
            break;
    }

    return ESP_OK;
}

esp_err_t aespl_gfx_buf_get_px(const aespl_gfx_buf_t *buf, uint16_t x, uint16_t y, uint32_t *value) {
    if (x >= buf->width || y >= buf->height) {
        return ESP_ERR_INVALID_ARG;
    }

    size_t word_bits = sizeof(**buf->content) * 8;
    uint16_t word_n = buf->wpr - 1 - x / buf->ppw;
    uint32_t w = buf->content[y][word_n];

    switch (buf->color) {
        case AESPL_GFX_COLOR_MONO:
            *value = 1 & (w >> (word_bits - x - 1 % word_bits));
            break;

        case AESPL_GFX_COLOR_RGB565:
            *value = 0xffff & (w >> ((x % 2) ? 0 : 16));
            break;

        case AESPL_GFX_COLOR_ARGB888:
            *value = w;
            break;
    }

    return ESP_OK;
}
