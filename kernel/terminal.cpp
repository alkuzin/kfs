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

constexpr int32_t TAB_WIDTH {4};

void terminal_t::set(void) noexcept
{
    m_x_pos  = 0;
    m_y_pos  = 0;
    m_fg     = gfx::color::white;
    m_bg     = gfx::color::black;
    m_height = driver::vesa.m_height;
    m_width  = driver::vesa.m_width;
}
        
void terminal_t::scroll(void) noexcept
{
	uint32_t size		  = m_height * driver::vesa.m_pitch;
    uint32_t *framebuffer = driver::vesa.m_addr;

    for (uint32_t i = 0; i < size; i++) {
        framebuffer[i] = framebuffer[i + m_width * gfx::FONT_CHAR_HEIGHT];
	}

	auto pos = size - m_width * gfx::FONT_CHAR_HEIGHT;
	kstd::memset(&framebuffer[pos], 0, size - pos);
}

void terminal_t::clear(void) noexcept
{
    m_x_pos = 0;
    m_y_pos = 0;
    driver::vesa.fill_screen(m_bg);
}

void terminal_t::putc(char c, gfx::rgb_t fg, gfx::rgb_t bg) noexcept
{
	if(m_x_pos >= m_width) {
		m_x_pos = 0;
		m_y_pos += gfx::FONT_CHAR_HEIGHT;
	}

	switch(c) {
		case '\n':
			m_y_pos += gfx::FONT_CHAR_HEIGHT;
			m_x_pos = 0;
			break;
		
		case '\t':
			for (int32_t i = 0; i < TAB_WIDTH; i++) {
            	driver::vesa.draw_char(' ', m_x_pos, m_y_pos, fg, bg, true);
				m_x_pos += gfx::FONT_CHAR_WIDTH;
			}
			break;
		
		case '\b':
            m_x_pos -= gfx::FONT_CHAR_WIDTH;
            
            if(!m_x_pos && m_y_pos) {
			    m_y_pos -= gfx::FONT_CHAR_HEIGHT;
                m_x_pos = m_width;
            }
			
            driver::vesa.draw_char(' ', m_x_pos, m_y_pos, fg, bg, true);
			break;
		
		default:
            if(kstd::isprint(c)) {
                driver::vesa.draw_char(c, m_x_pos, m_y_pos, fg, bg, true);
			    m_x_pos += gfx::FONT_CHAR_WIDTH;
            }
			break;
	};

	if (m_y_pos >= m_height) {
        auto rows_to_scroll = (m_y_pos - m_height) / gfx::FONT_CHAR_HEIGHT + 1;
        scroll();
        m_y_pos -= rows_to_scroll * gfx::FONT_CHAR_HEIGHT;
    }
}

terminal_t terminal;

} // namespace tty
} // namespace kernel