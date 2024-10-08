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
 * @file  printk.hpp
 * @brief Contains function to print kernel messages.
 *
 * @author Alexander Kuzin (<a href="https://github.com/alkuzin">alkuzin</a>)
 * @date   25.09.2024
 */

#ifndef _KERNEL_PRINTK_HPP_
#define _KERNEL_PRINTK_HPP_

#include <kernel/kstd/cstdarg.hpp>
#include <kernel/kstd/cstdio.hpp>


namespace kernel {

// kernel log types:
#define KERN_OK         "<0>"
#define KERN_ERR        "<1>"
#define KERN_DEBUG      "<2>"
#define KERN_EMERG      "<3>"
#define KERN_DEFAULT    "<9>"

/**
 * @brief Formats and prints data.
 *
 * @param [in] fmt - given format string.
 * @param [in] ... - given variable number of arguments.
 */
void printk(const char *fmt, ...) noexcept;

/**
 * @brief Formats and prints colored data.
 *
 * @param [in] fg - given foreground color.
 * @param [in] bg - given background color.
 * @param [in] fmt - given format string.
 * @param [in] ... - given variable number of arguments.
 */
void cprintk(gfx::rgb_t fg, gfx::rgb_t bg, const char *fmt, ...) noexcept;

} // namespace kernel

#endif // _KERNEL_PRINTK_HPP_