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

#include <kernel/types.hpp>


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


/**
 * @brief Compares the two strings s1 and s2.
 *
 * @param [in] s1 - first given null terminated string.
 * @param [in] s2 - second given null terminated string.
 * @param [in] n - given number of symbols for comparison.
 * @return 0, if s1 and s2 are equal;
 * @return a negative value if s1 is less than s2;
 * @return a positive value if s1 is greater than s2.
 */
int32_t strncmp(const char *s1, const char *s2, size_t n) noexcept;

} // namespace kstd
} // namespace kernel

#endif // _KERNEL_KSTD_CSTRING_HPP_