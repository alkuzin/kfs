/**
 * Monolithic Unix-like kernel from scratch.
 * Copyright (C) 2024 Alexander (@alkuzin).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <kernel/drivers/vesa.hpp>
#include <kernel/gfx/graphics.hpp>
#include <kernel/kstd/cmath.hpp>


namespace kernel {
namespace gfx {

using namespace driver;

point_t point_t::operator+(const int32_t val) const noexcept
{
    return {x + val, y + val};
}

point_t point_t::operator-(const int32_t val) const noexcept
{
    return {x - val, y - val};
}

void draw_line(point_t p1, point_t p2, rgb_t color) noexcept
{
    int32_t dx, dy, sx, sy, err, e2;

    dx  = kstd::abs(p2.x - p1.x);
    dy  = kstd::abs(p2.y - p1.y);
    err = dx - dy;
    sx  = (p1.x < p2.x) ? 1 : -1;
    sy  = (p1.y < p2.y) ? 1 : -1;

    for (;;) {
        vesa::draw_pixel(p1.x, p1.y, color);

        if (p1.x == p2.x && p1.y == p2.y)
            break;

        e2 = 2 * err;

        if (e2 > -dy) {
            err -= dy;
            p1.x += sx;
        }

        if (e2 < dx) {
            err += dx;
            p1.y += sy;
        }
    }
}

void fill_rectangle(point_t p, int32_t width, int32_t height, rgb_t color) noexcept
{
    for (int32_t i = p.x; i < p.x + width; i++) {
        for (int32_t j = p.y; j < p.y + height; j++)
            vesa::draw_pixel(i, j, color);
    }
}

void draw_rectangle(point_t p, int32_t width, int32_t height, rgb_t color) noexcept
{
    point_t b = {p.x + width, p.y};
    point_t c = {p.x, p.y + height};
    point_t d = {p.x + width, p.y + height};

    draw_line(p, b, color);
    draw_line(p, c, color);
    draw_line(b, d, color);
    draw_line(c, d, color);
}

} // namespace gfx
} // namespace kernel