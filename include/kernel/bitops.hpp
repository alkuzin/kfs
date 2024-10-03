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
 * @file  bitops.hpp
 * @brief Contain bit operations declarations.
 *
 * @author Alexander Kuzin (<a href="https://github.com/alkuzin">alkuzin</a>)
 * @date   03.10.2024
 */

#ifndef _KERNEL_BITOPS_HPP_
#define _KERNEL_BITOPS_HPP_

#include <kernel/types.hpp>


namespace kernel {

inline const auto BITS_PER_BYTE {8};

/**
 * @brief Convert bits to bytes.
 *
 * @param [in] n - given number of bits.
 * @return number of bytes needed for containing @a n bits.
 */
constexpr inline size_t BITS_TO_BYTES(size_t n) noexcept
{
    // assuming that a byte contains 8 bits
    return (n + 7) >> 0x3;
}

/**
 * @brief Convert bytes to bits.
 *
 * @param [in] n - given number of bytes.
 * @return number of bits in @a n bytes.
 */
constexpr inline size_t BYTES_TO_BITS(size_t n) noexcept
{
    // assuming that a byte contains 8 bits
    return n << 0x3;
}

/**
 * @brief Get number of bits in value.
 *
 * @param [in] value - given value to count.
 * @return number of bits in value.
 */
template <typename T>
constexpr inline size_t BITS_PER_TYPE = BYTES_TO_BITS(sizeof(T));

} // namespace kernel

#endif // _KERNEL_BITOPS_HPP_