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
 * @file  bitmap.hpp
 * @brief Declares bitmap structure.
 *
 * @author Alexander Kuzin (<a href="https://github.com/alkuzin">alkuzin</a>)
 * @date   30.09.2024
 */

#ifndef _KERNEL_KSTD_BITMAP_HPP_
#define _KERNEL_KSTD_BITMAP_HPP_

#include <kernel/bitops.hpp>


namespace kernel {
namespace kstd {


template <typename T>
struct bitmap_t
{
    T*     m_data;  // bitmap data pointer
    size_t m_size;  // size of data in bytes
    size_t m_bits;  // total number of bits in data

    /**
     * @brief Initialize bitmap.
     *
     * @param [in] data - given data pointer to set.
     * @param [in] size - given size of data.
     */
    inline void init(T *data = nullptr, size_t size = 0) noexcept;

    /**
     * @brief Get bit value.
     *
     * @param [in] pos - given position of bit.
     * @return true if bit = 1.
     * @return false if bit = 0.
     */
    inline bool get(size_t pos) const noexcept;

    /**
     * @brief Set specific bit.
     *
     * @param [in] pos - given position of bit.
     * @param [in] value - given bit value to set.
     */
    inline void set(size_t pos) noexcept;

    /**
     * @brief Unset specific bit.
     *
     * @param [in] pos - given position of bit.
     * @param [in] value - given bit value to unset.
     */
    inline void unset(size_t pos) noexcept;

    /**
     * @brief Get bits per element.
     *
     * @return bits per element.
     */
    inline size_t bits_per_element(void) const noexcept;

    /**
     * @brief Get number of elements.
     *
     * @return number of elements.
     */
    inline size_t capacity(void) const noexcept;
};

template <typename T>
inline void bitmap_t<T>::init(T *data, size_t size) noexcept
{
    m_data = data;
    m_size = size;
    m_bits = BYTES_TO_BITS(m_size);
}

template <typename T>
inline bool bitmap_t<T>::get(size_t pos) const noexcept
{
    return m_data[pos / BITS_PER_TYPE<T>] & (0x1 << (pos % BITS_PER_TYPE<T>));
}

template <typename T>
inline void bitmap_t<T>::set(size_t pos) noexcept
{
    m_data[pos / BITS_PER_TYPE<T>] |= (0x1 << (pos % BITS_PER_TYPE<T>));
}

template <typename T>
inline void bitmap_t<T>::unset(size_t pos) noexcept
{
    m_data[pos / BITS_PER_TYPE<T>] &= ~(0x1 << (pos % BITS_PER_TYPE<T>));
}

template <typename T>
inline size_t bitmap_t<T>::bits_per_element(void) const noexcept
{
    return BITS_PER_TYPE<T>;
}

template <typename T>
inline size_t bitmap_t<T>::capacity(void) const noexcept
{
    return (m_bits + BITS_PER_TYPE<T> - 1) / BITS_PER_TYPE<T>;
}

} // namespace kstd
} // namespace kernel

#endif // _KERNEL_KSTD_BITMAP_HPP_