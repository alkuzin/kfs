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
 * @file  cstring.hpp
 * @brief Defines several strings and memory management functions.
 *
 * @details This header file provides functions for manipulating strings,
 * such as copying, concatenating and comparing.
 * 
 * @author Alexander Kuzin (<a href="https://github.com/alkuzin">alkuzin</a>)
 * @date   25.09.2024
 */

#ifndef _KERNEL_KSTD_CSTRING_HPP_
#define _KERNEL_KSTD_CSTRING_HPP_

#include <kernel/kstd/ctypes.hpp>


namespace kernel {
namespace kstd {

/**
 * @brief Fills the first n bytes of the memory of the area pointed to by s 
 * with the constant byte c.
 * 
 * @param [out] s - given buffer pointer.
 * @param [in] c - given byte for filling buffer.
 * @param [in] n - given number of buffer bytes to fill.
 * @return filled buffer pointer.
 */
void *memset(void *s, int32_t c, size_t n) noexcept;

} // namespace kstd
} // namespace kernel

#endif // _KERNEL_KSTD_CSTRING_HPP_