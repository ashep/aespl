/**
 * @brief aGFXL -- ashep's Graphics Library
 *
 * @author    Alexander Shepetko <a@shepetko.com>
 * @copyright MIT License
 */

#include <stdlib.h>
#include "agfxl_buffer.h"
#include "agfxl_geometry.h"

agfxl_err_t agfxl_line(agfxl_buf_t *buf, const agfxl_line_t *line, uint32_t color) {
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

        agfxl_set_px(buf, abs(x), abs(y), color);

        x += dx;
        y += dy;

        ++i;
    }

    return AGFXL_OK;
}

agfxl_err_t agfxl_poly(agfxl_buf_t *buf, const agfxl_poly_t *poly, uint32_t color) {
    agfxl_err_t err;
    agfxl_line_t l;

    for (uint8_t i = 0; i < poly->n_corners; i++) {
        if (i < poly->n_corners - 1) {
            l = (agfxl_line_t){poly->corners[i], poly->corners[i + 1]};
        } else {
            l = (agfxl_line_t){poly->corners[i], poly->corners[0]};
        }

        err = agfxl_line(buf, &l, color);
        if (err) {
            return err;
        }
    }

    return AGFXL_OK;
}

agfxl_err_t agfxl_rect(agfxl_buf_t *buf, const agfxl_point_t p1, const agfxl_point_t p2, uint32_t color) {
    agfxl_point_t points[4] = {
        p1,
        (agfxl_point_t){p2.x, p1.y},
        p2,
        (agfxl_point_t){p1.x, p2.y},
    };

    return agfxl_poly(buf, &((agfxl_poly_t){4, points}), color);
}

agfxl_err_t agfxl_tri(agfxl_buf_t *buf, const agfxl_point_t p1, const agfxl_point_t p2,
                      const agfxl_point_t p3, uint32_t color) {
    agfxl_point_t points[3] = {p1, p2, p3};
    return agfxl_poly(buf, &((agfxl_poly_t){3, points}), color);
}
