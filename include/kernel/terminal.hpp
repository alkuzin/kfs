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
 * @file  terminal.hpp
 * @brief Contains kernel terminal declaration.
 *
 * @author Alexander Kuzin (<a href="https://github.com/alkuzin">alkuzin</a>)
 * @date   25.09.2024
 */

#ifndef _KERNEL_TERMINAL_HPP_
#define _KERNEL_TERMINAL_HPP_

#include <kernel/drivers/vesa.hpp>


namespace kernel {
namespace tty {

struct terminal_t
{
    int32_t    m_height;    // Screen height.
    int32_t    m_width;     // Screen width.
    int32_t    m_x_pos;     // X position of the cursor.
    int32_t    m_y_pos;     // Y position of the cursor.
    gfx::rgb_t m_fg;        // Foreground color.
    gfx::rgb_t m_bg;        // Background color.

    private:
        /** @brief Scroll screen.*/
        void scroll(void) noexcept;

    public:
        /** @brief Set terminal.*/
        void set(void) noexcept;

        /** @brief Clear screen.*/
        void clear(void) noexcept;

        /**
         * @brief Print colored character on screen.
         *
         * @param [in] c - given character to print.
         * @param [in] fg - given foreground color.
         * @param [in] bg - given background color.
         */
        void putc(char c, gfx::rgb_t fg, gfx::rgb_t bg) noexcept;
};

extern terminal_t terminal;

} // namespace tty
} // namespace kernel

#endif // _KERNEL_TERMINAL_HPP_