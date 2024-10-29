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

using namespace driver;

inline const auto TAB_WIDTH {4};

struct terminal_t
{
	vesa::fb_t fb;		// VESA framebuffer
    int32_t    x_pos;   // X position of the cursor.
    int32_t    y_pos;   // Y position of the cursor.
    gfx::rgb_t fg;      // Foreground color.
    gfx::rgb_t bg;      // Background color.
};

static terminal_t terminal;

void init(void) noexcept
{
	terminal.fb 	= vesa::get_framebuffer();
    terminal.x_pos 	= 0;
    terminal.y_pos 	= 0;
    terminal.fg    	= gfx::color::white;
    terminal.bg    	= gfx::color::black;
}

/** @brief Scroll screen.*/
static void scroll(void) noexcept
{
	uint32_t size		  = terminal.fb.height * terminal.fb.pitch;
	uint32_t *framebuffer = terminal.fb.addr;
	uint32_t pos {0};

    for (uint32_t i = 0; i < size; i++) {
		pos 		   = i + terminal.fb.width * FONT_CHAR_HEIGHT;
        framebuffer[i] = framebuffer[pos];
	}

	pos = size - terminal.fb.width * FONT_CHAR_HEIGHT;
	kstd::memset(&framebuffer[pos], 0, size - pos);
}

void clear(void) noexcept
{
    terminal.x_pos = 0;
    terminal.y_pos = 0;
    vesa::fill_screen(terminal.bg);
}

void putc(char c, gfx::rgb_t fg, gfx::rgb_t bg) noexcept
{
	if(terminal.x_pos >= int32_t(terminal.fb.width)) {
		terminal.x_pos = 0;
		terminal.y_pos += FONT_CHAR_HEIGHT;
	}

	switch(c) {
		case '\n':
			terminal.y_pos += FONT_CHAR_HEIGHT;
			terminal.x_pos = 0;
			break;

		case '\t':
			for (int32_t i = 0; i < TAB_WIDTH; i++) {
            	vesa::draw_char(' ', terminal.x_pos, terminal.y_pos,
				fg, bg, true);
				terminal.x_pos += FONT_CHAR_WIDTH;
			}
			break;

		case '\b':
            terminal.x_pos -= FONT_CHAR_WIDTH;

            if(!terminal.x_pos && terminal.y_pos) {
			    terminal.y_pos -= FONT_CHAR_HEIGHT;
                terminal.x_pos = terminal.fb.width;
            }

            vesa::draw_char(' ', terminal.x_pos, terminal.y_pos, fg, bg, true);
			break;

		default:
            if(kstd::isprint(c)) {
                vesa::draw_char(c, terminal.x_pos, terminal.y_pos,
				fg, bg, true);
			    terminal.x_pos += FONT_CHAR_WIDTH;
            }
			break;
	};

	if (terminal.y_pos >= int32_t(terminal.fb.height)) {
		int32_t tmp				= (terminal.y_pos - terminal.fb.height);
        int32_t rows_to_scroll	= tmp / FONT_CHAR_HEIGHT + 1;
        scroll();
        terminal.y_pos -= rows_to_scroll * FONT_CHAR_HEIGHT;
    }
}

gfx::rgb_t fgcolor(void) noexcept
{
	return terminal.fg;
}

gfx::rgb_t bgcolor(void) noexcept
{
	return terminal.bg;
}

} // namespace tty
} // namespace kernel