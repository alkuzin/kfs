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
 * @brief Provides definitions for various data types used in kernel.
 *
 * @author Alexander Kuzin (<a href="https://github.com/alkuzin">alkuzin</a>)
 * @date   24.09.2024
 */

#ifndef _KERNEL_TYPES_HPP_
#define _KERNEL_TYPES_HPP_


namespace kernel {

// unsigned types
using u64 = unsigned long long;
using u32 = unsigned int;
using u16 = unsigned short;
using u8  = unsigned char;

// signed types
using s64 = long long;
using s32 = int;
using s16 = short;
using s8  = char;

// floating point types
using f64 = double;
using f32 = float;

using usize = u64;
using ssize = s64;

using phys_addr_t = u32;
using virt_addr_t = u32;

// process identificator
using pid_t = s32;

// representing time with millisecond precision
using ktime_t = u32;

/** @brief KB literal.*/
constexpr inline usize operator"" _KB(usize n) noexcept
{
    return n << 10;
}

/** @brief MB literal.*/
constexpr inline usize operator"" _MB(usize n) noexcept
{
    return n << 20;
}

/** @brief GB literal.*/
constexpr inline usize operator"" _GB(usize n) noexcept
{
    return n << 30;
}

} // namespace kernel

#endif // _KERNEL_TYPES_HPP_