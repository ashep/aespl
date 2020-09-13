/**
 * AESPL Graphic
 *
 * Author: Alexander Shepetko <a@shepetko.com>
 * License: MIT
 */

#include "stdlib.h"
#include "stdbool.h"
#include "stdint.h"
#include "string.h"
#include "math.h"
#include "esp_err.h"
#include "esp_err.h"
#include "aespl_gfx.h"
#include "aespl_util.h"

esp_err_t aespl_gfx_init_buf(aespl_gfx_buf_t *buf, uint16_t width, uint16_t height, aespl_gfx_cmode_t color) {
    buf->width = width;
    buf->height = height;
    buf->color = color;
    buf->content = calloc(height, sizeof(buf->content));  // pointers to rows

    if (!buf->content) {
        return ESP_ERR_NO_MEM;
    }

    // Pixels per row
    switch (color) {
        case AESPL_GFX_CMODE_MONO:
            buf->ppw = sizeof(**buf->content) * 8;  // 8 bits per pixel,
            break;
        case AESPL_GFX_CMODE_RGB565:
            buf->ppw = sizeof(**buf->content) * 8 / 16;  // 16 bits per pixel
            break;
        case AESPL_GFX_CMODE_ARGB888:
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
    aespl_gfx_clear(buf);

    return ESP_OK;
}

void aespl_gfx_free_buf(aespl_gfx_buf_t *buf) {
    // Rows
    for (uint16_t r = 0; r < buf->height; r++) {
        free(buf->content[r]);
    }

    // Pointers to rows
    free(buf->content);
}

void aespl_gfx_clear(aespl_gfx_buf_t *buf) {
    for (uint16_t r = 0; r < buf->height; r++) {
        memset(buf->content[r], 0, buf->wpr * sizeof(**buf->content));
    }
}

void aespl_gfx_print_buf(const aespl_gfx_buf_t *buf) {
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

esp_err_t aespl_gfx_set_px(aespl_gfx_buf_t *buf, uint16_t x, uint16_t y, uint32_t value) {
    if (x >= buf->width || y >= buf->height) {
        return ESP_ERR_INVALID_ARG;
    }

    size_t word_bits = sizeof(**buf->content) * 8;
    uint16_t word_n = buf->wpr - 1 - x / buf->ppw;

    switch (buf->color) {
        case AESPL_GFX_CMODE_MONO:
            buf->content[y][word_n] |= value << (word_bits - x - 1 % word_bits);
            break;

        case AESPL_GFX_CMODE_RGB565:
            buf->content[y][word_n] |= value << ((x % 2) ? 0 : 16);
            break;

        case AESPL_GFX_CMODE_ARGB888:
            buf->content[y][word_n] = value;
            break;
    }

    return ESP_OK;
}

esp_err_t aespl_gfx_get_px(const aespl_gfx_buf_t *buf, uint16_t x, uint16_t y, uint32_t *value) {
    if (x >= buf->width || y >= buf->height) {
        return ESP_ERR_INVALID_ARG;
    }

    size_t word_bits = sizeof(**buf->content) * 8;
    uint16_t word_n = buf->wpr - 1 - x / buf->ppw;
    uint32_t w = buf->content[y][word_n];

    switch (buf->color) {
        case AESPL_GFX_CMODE_MONO:
            *value = 1 & (w >> (word_bits - x - 1 % word_bits));
            break;

        case AESPL_GFX_CMODE_RGB565:
            *value = 0xffff & (w >> ((x % 2) ? 0 : 16));
            break;

        case AESPL_GFX_CMODE_ARGB888:
            *value = w;
            break;
    }

    return ESP_OK;
}

esp_err_t aespl_gfx_merge(aespl_gfx_buf_t *dst, const aespl_gfx_buf_t *src, aespl_gfx_point_t p) {
    if (p.x >= dst->width || p.y >= dst->height) {
        return ESP_ERR_INVALID_ARG;
    }

    for (uint16_t kx = 0; kx < src->width; kx++) {
        for (uint16_t ky = 0; ky < src->height; ky++) {
            uint32_t px_val = 0;
            aespl_gfx_get_px(src, kx, ky, &px_val);
            aespl_gfx_set_px(dst, p.x + kx, p.y + ky, px_val);
        }
    }

    return ESP_OK;
}

esp_err_t aespl_gfx_move(aespl_gfx_buf_t *buf, aespl_gfx_point_t rel_p) {
    esp_err_t err;
    aespl_gfx_buf_t tmp_buf;

    err = aespl_gfx_init_buf(&tmp_buf, buf->width, buf->height, buf->color);
    if (err) {
        return err;
    }

    err = aespl_gfx_merge(&tmp_buf, buf, rel_p);
    if (err) {
        return err;
    }

    aespl_gfx_clear(buf);

    err = aespl_gfx_merge(buf, &tmp_buf, (aespl_gfx_point_t){0, 0});
    if (err) {
        return err;
    }

    aespl_gfx_free_buf(&tmp_buf);

    return ESP_OK;
}

esp_err_t aespl_gfx_line(aespl_gfx_buf_t *buf, const aespl_gfx_line_t *line, uint32_t color) {
    uint16_t x1 = line->p1.x, x2 = line->p2.x, y1 = line->p1.y, y2 = line->p2.y;

    if (x2 >= buf->width) {
        x2 = buf->width - 1;
    }
    if (y2 >= buf->height) {
        y2 = buf->height - 1;
    }

    float x = x1, y = y1, dx = x2 - x1, dy = y2 - y1;
    uint16_t step, abs_dx = abs(dx), abs_dy = abs(dy);

    step = abs_dx >= abs_dy ? abs_dx : abs_dy;
    dx /= step;
    dy /= step;

    uint16_t i = 0;
    while (i <= step) {
        if (i == step) {
            if (x < x2) {
                x = x2;
            }
            if (y < y2) {
                y = y2;
            }
        }

        aespl_gfx_set_px(buf, abs(x), abs(y), color);

        x += dx;
        y += dy;

        ++i;
    }

    return ESP_OK;
}

esp_err_t aespl_gfx_poly(aespl_gfx_buf_t *buf, const aespl_gfx_poly_t *poly, uint32_t color) {
    esp_err_t err;
    aespl_gfx_line_t l;

    for (uint8_t i = 0; i < poly->n_corners; i++) {
        if (i < poly->n_corners - 1) {
            l = (aespl_gfx_line_t){poly->corners[i], poly->corners[i + 1]};
        } else {
            l = (aespl_gfx_line_t){poly->corners[i], poly->corners[0]};
        }

        err = aespl_gfx_line(buf, &l, color);
        if (err) {
            return err;
        }
    }

    return ESP_OK;
}

esp_err_t aespl_gfx_rect(aespl_gfx_buf_t *buf, const aespl_gfx_point_t p1, const aespl_gfx_point_t p2, uint32_t color) {
    aespl_gfx_point_t points[4] = {
        p1,
        (aespl_gfx_point_t){p2.x, p1.y},
        p2,
        (aespl_gfx_point_t){p1.x, p2.y},
    };

    return aespl_gfx_poly(buf, &((aespl_gfx_poly_t){4, points}), color);
}

esp_err_t aespl_gfx_tri(aespl_gfx_buf_t *buf, const aespl_gfx_point_t p1, const aespl_gfx_point_t p2,
                        const aespl_gfx_point_t p3, uint32_t color) {
    aespl_gfx_point_t points[3] = {p1, p2, p3};
    return aespl_gfx_poly(buf, &((aespl_gfx_poly_t){3, points}), color);
}

esp_err_t aespl_gfx_putc(aespl_gfx_buf_t *buf, const aespl_gfx_font_t *font, aespl_gfx_point_t pos, char ch,
                         uint32_t color, uint8_t *ch_width) {
    esp_err_t err;

    // If the character is not covered by the font
    if (ch - font->ascii_offset + 1 > font->size) {
        return ESP_ERR_INVALID_ARG;
    }

    // Offset from the beginning of the font content.
    uint16_t offset = (ch - font->ascii_offset) * (font->height + 1);

    void *ch_p = NULL;
    switch (font->width) {
        case AESPL_GFX_FONT_WIDTH_8:
            ch_p = (uint8_t *)&font->content.c8[offset];
            *ch_width = *(uint8_t *)ch_p++;
            break;
        case AESPL_GFX_FONT_WIDTH_16:
            ch_p = (uint16_t *)&font->content.c16[offset];
            *ch_width = *(uint16_t *)ch_p++;
            break;
        case AESPL_GFX_FONT_WIDTH_32:
            ch_p = (uint32_t *)&font->content.c32[offset];
            *ch_width = *(uint32_t *)ch_p++;
            break;
        case AESPL_GFX_FONT_WIDTH_64:
            ch_p = (uint64_t *)&font->content.c64[offset];
            *ch_width = *(uint64_t *)ch_p++;
            break;
    }

    for (uint8_t row_n = 0; row_n < font->height; row_n++) {
        uint64_t row = 0;

        switch (font->width) {
            case AESPL_GFX_FONT_WIDTH_8:
                row = *(uint8_t *)ch_p++;
                break;
            case AESPL_GFX_FONT_WIDTH_16:
                row = *(uint16_t *)ch_p++;
                break;
            case AESPL_GFX_FONT_WIDTH_32:
                row = *(uint32_t *)ch_p++;
                break;
            case AESPL_GFX_FONT_WIDTH_64:
                row = *(uint64_t *)ch_p++;
                break;
            default:
                return ESP_ERR_INVALID_ARG;
        }

        for (int8_t n = 0, col_n = font->width - 1; n < *ch_width; n++, col_n--) {
            uint32_t px_color = 1 & (row >> col_n) ? color : 0;
            err = aespl_gfx_set_px(buf, pos.x + n, pos.y + row_n, px_color);
            if (err) {
                return err;
            }
        }
    }

    return ESP_OK;
}

esp_err_t aespl_gfx_puts(aespl_gfx_buf_t *buf, const aespl_gfx_font_t *font, aespl_gfx_point_t *pos, char *s,
                         uint32_t color, uint8_t space) {
    esp_err_t err;
    uint8_t ch_width = 0;

    while (*s) {
        err = aespl_gfx_putc(buf, font, *pos, *s++, color, &ch_width);
        if (err) {
            return err;
        }

        pos->x += ch_width + space;
    }

    return ESP_OK;
}
