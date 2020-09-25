/**
 * @brief AESPL Graphics
 *
 * @author    Alexander Shepetko <a@shepetko.com>
 * @copyright MIT License
 */

#include "stdlib.h"
#include "stdbool.h"
#include "stdint.h"
#include "string.h"
#include "math.h"
#include "esp_err.h"
#include "esp_err.h"
#include "aespl_gfx.h"
#include "aespl_common.h"

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

esp_err_t aespl_gfx_init_buf_array(aespl_gfx_buf_array_t *buf_arr, uint8_t length,
                                   uint16_t width, uint16_t height, aespl_gfx_cmode_t color) {
    buf_arr->length = length;
    buf_arr->color = color;
    buf_arr->buffers = calloc(length, sizeof(aespl_gfx_buf_t));

    aespl_gfx_buf_t *p = buf_arr->buffers;

    for (uint8_t i = 0; i < length; i++) {
        aespl_gfx_init_buf(p++, width, height, color);
    }

    return ESP_OK;
}

void aespl_gfx_free_buf_array(aespl_gfx_buf_array_t *buf_arr) {
    aespl_gfx_buf_t *p = buf_arr->buffers;
    for (uint8_t i = 0; i < buf_arr->length; i++) {
        aespl_gfx_free_buf(p++);
    }

    free(buf_arr->buffers);
}

void aespl_gfx_clear(aespl_gfx_buf_t *buf) {
    for (uint16_t r = 0; r < buf->height; r++) {
        memset(buf->content[r], 0, buf->wpr * sizeof(**buf->content));
    }
}

void aespl_gfx_dump(const aespl_gfx_buf_t *buf) {
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

esp_err_t aespl_gfx_set_px(aespl_gfx_buf_t *buf, int16_t x, int16_t y, uint32_t value) {
    // TODO: make negative/operflow values acceptable by "circling",
    // i. e. for 16-pixel wide buffer x == -1 becomes 15, and x == 16 becomes 1.
    if (x < 0 || x >= buf->width || y < 0 || y >= buf->height) {
        return AESPL_ERR_GFX_OOB;
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

esp_err_t aespl_gfx_get_px(const aespl_gfx_buf_t *buf, int16_t x, int16_t y, uint32_t *value) {
    // TODO: make negative/operflow values acceptable by "circling",
    // i. e. for 16-pixel wide buffer x == -1 becomes 15, and x == 16 becomes 1.
    if (x < 0 || x >= buf->width || y < 0 || y >= buf->height) {
        return AESPL_ERR_GFX_OOB;
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

esp_err_t aespl_gfx_merge(aespl_gfx_buf_t *dst, const aespl_gfx_buf_t *src,
                          aespl_gfx_point_t dst_pos, aespl_gfx_point_t src_pos) {
    if (src_pos.x >= src->width || src_pos.y >= src->height || dst_pos.x >= dst->width || dst_pos.y >= dst->height) {
        return AESPL_ERR_GFX_OOB;
    }

    esp_err_t err;
    uint32_t color = 0;
    int16_t dst_x = dst_pos.x;
    for (int16_t src_x = src_pos.x; src_x < src->width && dst_x < dst->width; src_x++, dst_x++) {
        int16_t dst_y = dst_pos.y;
        for (int16_t src_y = src_pos.y; src_y < src->height && dst_y < dst->height; src_y++, dst_y++) {
            err = aespl_gfx_get_px(src, src_x, src_y, &color);
            if (err == AESPL_ERR_GFX_OOB) {
                continue;
            } else if (err != ESP_OK) {
                return err;
            }

            err = aespl_gfx_set_px(dst, dst_x, dst_y, color);
            if (err == AESPL_ERR_GFX_OOB) {
                continue;
            } else if (err != ESP_OK) {
                return err;
            }
        }
    }

    return ESP_OK;
}

esp_err_t aespl_gfx_split(aespl_gfx_buf_array_t *dst, const aespl_gfx_buf_t *src, uint8_t num_x, uint8_t num_y) {
    esp_err_t err;
    uint16_t dst_width = src->width / num_x;
    uint16_t dst_height = src->height / num_y;

    err = aespl_gfx_init_buf_array(dst, num_x * num_y, dst_width, dst_height, src->color);
    if (err) {
        return err;
    }

    aespl_gfx_buf_t *dst_s_buf = dst->buffers;
    for (uint8_t n_y = 0; n_y < num_y; n_y++) {
        for (uint8_t n_x = 0; n_x < num_x; n_x++) {
            aespl_gfx_point_t src_pos = {n_x * dst_width, n_y * dst_height};
            err = aespl_gfx_merge(dst_s_buf, src, (aespl_gfx_point_t){0, 0}, src_pos);
            if (err) {
                return err;
            }
            dst_s_buf++;
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

    err = aespl_gfx_merge(&tmp_buf, buf, rel_p, (aespl_gfx_point_t){0, 0});
    if (err) {
        return err;
    }

    aespl_gfx_clear(buf);

    err = aespl_gfx_merge(buf, &tmp_buf, (aespl_gfx_point_t){0, 0}, (aespl_gfx_point_t){0, 0});
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

esp_err_t aespl_gfx_putc(aespl_gfx_buf_t *buf, const aespl_gfx_font_t *font, aespl_gfx_point_t pos, uint8_t ch,
                         uint32_t color, uint8_t *ch_width) {
    esp_err_t err;

    // If the character is not covered y the font
    if (ch - font->ascii_offset + 1 > font->length) {
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
            if (err == AESPL_ERR_GFX_OOB) {
                continue;
            } else if (err != ESP_OK) {
                return err;
            }
        }
    }

    return ESP_OK;
}

esp_err_t aespl_gfx_puts(aespl_gfx_buf_t *buf, const aespl_gfx_font_t *font, aespl_gfx_point_t *pos, const char *s,
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
