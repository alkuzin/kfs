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

/** @brief Set terminal.*/
void init(void) noexcept;

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

/**
 * @brief Get TTY foreground color.
 *
 * @return foreground color.
 */
gfx::rgb_t fgcolor(void) noexcept;

/**
 * @brief Get TTY background color.
 *
 * @return background color.
 */
gfx::rgb_t bgcolor(void) noexcept;

} // namespace tty
} // namespace kernel

#endif // _KERNEL_TERMINAL_HPP_