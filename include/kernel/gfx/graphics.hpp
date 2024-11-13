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

/**
 * @file  graphics.hpp
 * @brief Contains computer graphics declarations.
 *
 * @author Alexander Kuzin (<a href="https://github.com/alkuzin">alkuzin</a>)
 * @date   31.10.2024
 */

#ifndef _KERNEL_GFX_GRAPHICS_HPP_
#define _KERNEL_GFX_GRAPHICS_HPP_

#include <kernel/gfx/color.hpp>


namespace kernel {
namespace gfx {

struct point_t {
    s32 x;
    s32 y;

    /**
     * @brief Operator + overloading for point + value.
     *
     * @param [in] val - given value.
     * @return new point.
     */
    point_t operator+(const s32 val) const noexcept;

    /**
     * @brief Operator - overloading for point - value.
     *
     * @param [in] val - given value.
     * @return new point.
     */
    point_t operator-(const s32 val) const noexcept;
};

/**
 * @brief Draw line on the screen.
 *
 * @param [in] p1 - given first point.
 * @param [in] p2 - given second point.
 * @param [in] color - given color of the line.
 */
void draw_line(point_t p1, point_t p2, rgb_t color) noexcept;

/**
 * @brief Fill rectangle with specific color.
 *
 * @param [in] p - given rectangle begin point.
 * @param [in] width - given rectangle width.
 * @param [in] height - given rectangle height.
 * @param [in] color - given color to fill with.
 */
void fill_rectangle(point_t p, s32 width, s32 height, rgb_t color) noexcept;

/**
 * @brief Draw rectangle on the screen.
 *
 * @param [in] p - given rectangle begin point.
 * @param [in] width - given rectangle width.
 * @param [in] height - given rectangle height.
 * @param [in] color - given rectangle color.
 */
void draw_rectangle(point_t p, s32 width, s32 height, rgb_t color) noexcept;

} // namespace gfx
} // namespace kernel

#endif // _KERNEL_GFX_GRAPHICS_HPP_