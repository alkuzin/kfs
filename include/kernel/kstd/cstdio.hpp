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
void snprintk(char *buf, usize size, const char *fmt, ...);

/**
 * @brief Formats and prints data to buffer.
 *
 * @param [out] buf - given buffer for containing formated result.
 * @param [in] size - given buffer size.
 * @param [in] fmt - given format string.
 * @param [in] args - given variable list of arguments.
 */
void vsnprintk(char *buf, usize size, const char *fmt, va_list args);

/**
 * @brief Print character to screen.
 *
 * @param [in] c - given character to print.
 */
inline void putchar(char c)
{
    tty::putc(c, tty::terminal.fg, tty::terminal.bg);
}

/**
 * @brief Print colored string and a trailing newline.
 *
 * @param [in] str - given null terminated string
 * @param [in] fg - given foreground color.
 * @param [in] bg - given background color.
 */
inline void putk(const char *str, rgb_t fg = tty::terminal.fg, rgb_t bg = tty::terminal.bg)
{
    u32 i = 0;

    while (str[i]) {
        tty::putc(str[i], fg, bg);
        i++;
    }
}

} // namespace kstd
} // namespace kernel

#endif // _KERNEL_KSTD_CSTDIO_HPP_