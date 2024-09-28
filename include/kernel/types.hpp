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
 * @file  types.hpp
 * @brief Provides definitions for various data types used in kernel programming.
 *
 * @author Alexander Kuzin (<a href="https://github.com/alkuzin">alkuzin</a>)
 * @date   24.09.2024
 */

#ifndef _KERNEL_TYPES_HPP_
#define _KERNEL_TYPES_HPP_


namespace kernel {

// unsigned types
using uint64_t = unsigned long long;
using uint32_t = unsigned int;
using uint16_t = unsigned short;
using uint8_t  = unsigned char;

// signed types
using int64_t = long long;
using int32_t = int;
using int16_t = short;
using int8_t  = char;

// floating point types
using float64_t = double;
using float32_t = float;

using size_t  = uint64_t;
using ssize_t = int64_t;

} // namespace kernel

#endif // _KERNEL_TYPES_HPP_