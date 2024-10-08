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
 * @file  panic.hpp
 * @brief Contains function to print kernel internal errors.
 *
 * @author Alexander Kuzin (<a href="https://github.com/alkuzin">alkuzin</a>)
 * @date   08.10.2024
 */

#ifndef _KERNEL_PANIC_HPP_
#define _KERNEL_PANIC_HPP_

#include <kernel/kstd/cstdarg.hpp>


namespace kernel {

// kernel panic types:
#define PANIC_FATAL      "<0>"  // fatal error  (stop kernel)
#define PANIC_ERR        "<1>"  // simple error (don't stop kernel)
#define PANIC_DEBUG      "<2>"  // like fatal error, but outputs memory dump
#define PANIC_DEFAULT    "<9>"  // like PANIC_FATAL & PANIC_DEBUG

/**
 * @brief Print kernel internal error.
 *
 * @param [in] fmt - given format string.
 * @param [in] ... - given variable number of arguments.
 */
void panic(const char *fmt, ...) noexcept;

} // namespace kernel

#endif // _KERNEL_PANIC_HPP_