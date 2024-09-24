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


namespace kernel {
namespace driver {


void vesa_t::set(const multiboot_t& mboot) noexcept
{
    m_addr   = reinterpret_cast<uint32_t*>(mboot.framebuffer_addr);
    m_pitch  = mboot.framebuffer_pitch;
    m_width  = mboot.framebuffer_width;
    m_height = mboot.framebuffer_height;
    m_bpp    = mboot.framebuffer_bpp;
}

inline void vesa_t::draw_pixel(uint32_t x, uint32_t y, const rgb_t& color) noexcept
{
    if (x < m_width && y < m_height)
        m_addr[y * m_width + x] = (color.m_red << 0x10) | (color.m_green << 0x8) | color.m_blue;
}

void vesa_t::fill_screen(const rgb_t& color) noexcept
{
    for (uint32_t y = 0; y < m_height; y++) {
        for (uint32_t x = 0; x < m_width; x++)
            draw_pixel(x, y, color);
    }
}

vesa_t vesa;

} // namespace driver
} // namespace kernel