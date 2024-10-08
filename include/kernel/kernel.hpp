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
 * @file  kernel.hpp
 * @brief Contain various definitions, declarations & auxilar functions
 * that are used throughout the kernel.
 *
 * @author Alexander Kuzin (<a href="https://github.com/alkuzin">alkuzin</a>)
 * @date   08.10.2024
 */

#ifndef _KERNEL_KERNEL_HPP_
#define _KERNEL_KERNEL_HPP_

#include <kernel/types.hpp>


namespace kernel {

/**
 * @brief Get the closest power of 2.
 *
 * @param [in] n - given number.
 * @return closest power of 2.
 */
constexpr inline size_t roundup_pow_of_two(size_t n) noexcept
{
    if (n == 0)
        return 1;

    n--;              // handle the case where x is already a power of 2
    n |= n >> 0x1;    // set bits 1 and 2
    n |= n >> 0x2;    // set bits 1-4
    n |= n >> 0x4;    // set bits 1-8
    n |= n >> 0x8;    // set bits 1-16
    n |= n >> 0x10;   // set bits 1-32 (for 32-bit integers)

    return n + 1;
}

} // namespace kernel

#endif // _KERNEL_KERNEL_HPP_