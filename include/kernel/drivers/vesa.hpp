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

#include <kernel/kstd/ctypes.hpp>
#include <kernel/multiboot.hpp>


namespace kernel {
namespace driver {

// TODO: move to gfx directory
struct rgb_t
{
    uint8_t m_red;
    uint8_t m_green;
    uint8_t m_blue;
};

struct vesa_t
{
    uint32_t *m_addr;    // framebuffer address
    uint32_t  m_pitch;   // number of bytes in a single row of the framebuffer
    uint32_t  m_width;   // y-resolution
    uint32_t  m_height;  // x-resolution
    uint8_t   m_bpp;     // bytes per pixel

    /**
     * @brief Set VESA driver.
     * 
     * @param [in] mboot - given multiboot info structure.
     */
    void set(const multiboot_t& mboot) noexcept;

    /**
     * @brief Draw pixel on the screen.
     * 
     * @param [in] x - given pixel x-position.
     * @param [in] y - given pixel y-position.
     * @param [in] color - given pixel RGB color.
     */
    inline void draw_pixel(uint32_t x, uint32_t y, const rgb_t& color) noexcept;

    /**
     * @brief Fill screen with specific color.
     * 
     * @param [in] color - given RGB color.
     */
    void fill_screen(const rgb_t& color) noexcept;
};

extern vesa_t vesa;

} // namespace driver
} // namespace kernel

#endif // _KERNEL_DRIVER_VESA_HPP_