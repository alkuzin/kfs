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
 * @file  vesa.hpp
 * @brief Contains Video Electronics Standards Association (VESA) driver declaration.
 *
 * @author Alexander Kuzin (<a href="https://github.com/alkuzin">alkuzin</a>)
 * @date   24.09.2024
 */

#ifndef _KERNEL_DRIVER_VESA_HPP_
#define _KERNEL_DRIVER_VESA_HPP_

#include <kernel/gfx/color.hpp>
#include <kernel/multiboot.hpp>


namespace kernel {
namespace driver {
namespace vesa {

struct fb_t {
    u32 *addr;    // framebuffer address
    u32  pitch;   // number of bytes in a single row of the framebuffer
    u32  width;   // y-resolution
    u32  height;  // x-resolution
    u8   bpp;     // bytes per pixel
};

/**
 * @brief Set VESA driver.
 *
 * @param [in] mboot - given multiboot info structure.
 */
void init(const multiboot_t& mboot);

/**
 * @brief Get the VESA framebuffer.
 *
 * @return VESA framebuffer.
 */
fb_t get_framebuffer(void);

/**
 * @brief Draw pixel on the screen.
 *
 * @param [in] x - given pixel x-position.
 * @param [in] y - given pixel y-position.
 * @param [in] color - given pixel RGB color.
 */
void draw_pixel(u32 x, u32 y, rgb_t color);

/**
 * @brief Fill screen with specific color.
 *
 * @param [in] color - given RGB color.
 */
void fill_screen(rgb_t color);

/**
 * @brief Draw font character on the screen.
 *
 * @param [in] c - given character to print.
 * @param [in] x - given x pixel position.
 * @param [in] y - given y pixel position.
 * @param [in] fg - given foreground color.
 * @param [in] bg - given background color.
 * @param [in] is_bg_on - given param determine whether to display the @a bg.
 */
void draw_char(u8 c, s32 x, s32 y, rgb_t fg, rgb_t bg, bool is_bg_on);

} // namespace vesa
} // namespace driver
} // namespace kernel

#endif // _KERNEL_DRIVER_VESA_HPP_