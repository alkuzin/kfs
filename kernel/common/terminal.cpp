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

#include <kernel/kstd/cstring.hpp>
#include <kernel/kstd/cctype.hpp>
#include <kernel/terminal.hpp>
#include <kernel/gfx/font.hpp>


namespace kernel {
namespace tty {

using namespace driver::vesa;


void terminal_t::init(void) noexcept
{
        fb 			= get_framebuffer();
    fg    		= gfx::color::white;
    bg    		= gfx::color::black;
    begin_x_pos	= 0;
    begin_y_pos	= 0;
    x_pos 		= 0;
    y_pos 		= 0;
    width 		= fb.width;
    height 		= fb.height;
}

/** @brief Scroll screen.*/
static void scroll(void) noexcept
{
        u32 size         = terminal.fb.height * terminal.fb.pitch;
        u32 *framebuffer = terminal.fb.addr;
        u32 pos {0};

    for (u32 i = 0; i < size; i++) {
                pos 		   = i + terminal.fb.width * FONT_CHAR_HEIGHT;
        framebuffer[i] = framebuffer[pos];
        }

        pos = size - terminal.fb.width * FONT_CHAR_HEIGHT;
        kstd::memset(&framebuffer[pos], 0, size - pos);
}

void clear(void) noexcept
{
    terminal.x_pos = terminal.begin_x_pos;
    terminal.y_pos = terminal.begin_y_pos;
    fill_screen(terminal.bg);
}

void putc(char c, rgb_t fg, rgb_t bg) noexcept
{
        if(terminal.x_pos >= terminal.begin_x_pos + s32(terminal.width)) {
                terminal.x_pos = terminal.begin_x_pos;
                terminal.y_pos += FONT_CHAR_HEIGHT;
        }

        switch(c) {
                case '\n':
                        terminal.y_pos += FONT_CHAR_HEIGHT;
                        terminal.x_pos = terminal.begin_x_pos;
                        break;

                case '\t':
                        for (s32 i = 0; i < TAB_WIDTH; i++) {
                    draw_char(' ', terminal.x_pos, terminal.y_pos,
                                fg, bg, true);
                                terminal.x_pos += FONT_CHAR_WIDTH;
                        }
                        break;

                case '\b':
            terminal.x_pos -= FONT_CHAR_WIDTH;

            if(!terminal.x_pos && terminal.y_pos) {
                            terminal.y_pos -= FONT_CHAR_HEIGHT;
                terminal.x_pos = terminal.width;
            }

            draw_char(' ', terminal.x_pos, terminal.y_pos, fg, bg, true);
                        break;

                default:
            if(kstd::isprint(c)) {
                draw_char(c, terminal.x_pos, terminal.y_pos,
                                fg, bg, true);
                            terminal.x_pos += FONT_CHAR_WIDTH;
            }
                        break;
        };

        if (terminal.y_pos >= s32(terminal.fb.height)) {
                s32 tmp				= (terminal.y_pos - terminal.fb.height);
        s32 rows_to_scroll	= tmp / FONT_CHAR_HEIGHT + 1;
        scroll();
        terminal.y_pos -= rows_to_scroll * FONT_CHAR_HEIGHT;
    }
}

terminal_t terminal;

} // namespace tty
} // namespace kernel