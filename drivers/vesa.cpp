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
namespace vesa {

static fb_t framebuffer {};

void init(const multiboot_t& mboot) noexcept
{
    framebuffer.addr   = reinterpret_cast<u32*>(mboot.framebuffer_addr);
    framebuffer.pitch  = mboot.framebuffer_pitch;
    framebuffer.width  = mboot.framebuffer_width;
    framebuffer.height = mboot.framebuffer_height;
    framebuffer.bpp    = mboot.framebuffer_bpp;
}

fb_t get_framebuffer(void) noexcept
{
    return framebuffer;
}

void draw_pixel(u32 x, u32 y, gfx::rgb_t color) noexcept
{
    if (x < framebuffer.width && y < framebuffer.height)
        framebuffer.addr[y * framebuffer.width + x] = color;
}

void fill_screen(gfx::rgb_t color) noexcept
{
    for (u32 y = 0; y < framebuffer.height; y++) {
        for (u32 x = 0; x < framebuffer.width; x++)
            draw_pixel(x, y, color);
    }
}

void draw_char(u8 c, s32 x, s32 y, gfx::rgb_t fg, gfx::rgb_t bg, bool is_bg_on) noexcept
{
    static constexpr u8 mask[8] = { 128, 64, 32, 16, 8, 4, 2, 1 };
    s32 cx, cy;

    u8 *glyph = static_cast<u8*>(gfx::font) + s32(c) * 16;

    for (cy = 0; cy < FONT_CHAR_HEIGHT; cy++) {
        for (cx = 0; cx < FONT_CHAR_WIDTH; cx++) {
            if (glyph[cy] & mask[cx])
                draw_pixel(x + cx, y + cy, fg);
            else if (is_bg_on)
                draw_pixel(x + cx, y + cy, bg);
        }
    }
}

} // namespace vesa
} // namespace driver
} // namespace kernel