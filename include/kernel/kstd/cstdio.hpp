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
 * @file  cstdio.hpp
 * @brief Defines general input/output functions.
 * 
 * @author Alexander Kuzin (<a href="https://github.com/alkuzin">alkuzin</a>)
 * @date   25.09.2024
 */

#ifndef _KERNEL_KSTD_CSTDIO_HPP_
#define _KERNEL_KSTD_CSTDIO_HPP_

#include <kernel/kstd/cstdarg.hpp>
#include <kernel/terminal.hpp>


namespace kernel {
namespace kstd {

/**
 * @brief Formats and prints data to buffer.
 * 
 * @param [out] buf - given buffer for containing formated result.
 * @param [in] size - given buffer size.
 * @param [in] fmt - given format string.
 * @param [in] args - given variable list of arguments.
 */
void vsnprintk(char *buf, size_t size, const char *fmt, va_list args) noexcept;

/**
 * @brief Print character to screen.
 * 
 * @param [in] c - given character to print.
 */
inline void putchar(const int32_t c) noexcept
{
	tty::terminal.putc(c, tty::terminal.m_fg, tty::terminal.m_bg);
}

} // namespace kstd
} // namespace kernel

#endif // _KERNEL_KSTD_CSTDIO_HPP_