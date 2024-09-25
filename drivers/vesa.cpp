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
#include <kernel/gfx/font.hpp>


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

inline void vesa_t::draw_pixel(uint32_t x, uint32_t y, gfx::rgb_t color) noexcept
{
    if (x < m_width && y < m_height)
        m_addr[y * m_width + x] = color;
}

void vesa_t::fill_screen(gfx::rgb_t color) noexcept
{
    for (uint32_t y = 0; y < m_height; y++) {
        for (uint32_t x = 0; x < m_width; x++)
            draw_pixel(x, y, color);
    }
}

void vesa_t::draw_char(uint8_t c, int32_t x, int32_t y, gfx::rgb_t fg, gfx::rgb_t bg, bool is_bg_on) noexcept
{
    static int32_t mask[8] = { 128, 64, 32, 16, 8, 4, 2, 1 };
    int32_t cx, cy;

    uint8_t *glyph = static_cast<uint8_t*>(gfx::font) + static_cast<int32_t>(c) * 16;

    for (cy = 0; cy < gfx::FONT_CHAR_HEIGHT; cy++) {
        for (cx = 0; cx < gfx::FONT_CHAR_WIDTH; cx++) {
            if (glyph[cy] & mask[cx])
                draw_pixel(x + cx, y + cy, fg);
            else if (is_bg_on)
                draw_pixel(x + cx, y + cy, bg);
        }
    }
}

vesa_t vesa;

} // namespace driver
} // namespace kernel