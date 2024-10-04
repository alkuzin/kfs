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
 * @file  mm_types.hpp
 * @brief Provides definitions for various data types used in kernel memory management.
 *
 * @author Alexander Kuzin (<a href="https://github.com/alkuzin">alkuzin</a>)
 * @date   04.10.2024
 */

#ifndef _KERNEL_MM_TYPES_HPP_
#define _KERNEL_MM_TYPES_HPP_

#include <kernel/types.hpp>


namespace kernel {
namespace core {
namespace memory {

inline const size_t     PAGE_SIZE   {4096}; // 4 KB
inline const bool       PAGE_FREE   {0};
inline const bool       PAGE_USED   {1};
inline const uint8_t    PAGE_SHIFT  {0xC};

// page flags enumeration
enum PG : uint8_t {
    RESERVED = 0b10000000   // flag empty pages or pages that do not even exist
};

/**
 * @brief Convert page frame number to physical address.
 *
 * @param [in] pfn - given page frame number.
 * @return physical address.
 */
constexpr inline phys_addr_t PFN_PHYS(size_t pfn) noexcept
{
    return pfn << PAGE_SHIFT;
}

/**
 * @brief Convert physical address to page frame number.
 *
 * @param [in] addr - given page physical address.
 * @return page frame number.
 */
constexpr inline phys_addr_t PHYS_PFN(phys_addr_t addr) noexcept
{
    return addr >> PAGE_SHIFT;
}

struct page_t
{
    size_t  m_pfn;     // page frame number - position in bitmap & memory map
    uint8_t m_flags;   // describes page status

    /**
     * @brief Get page memory address.
     *
     * @return page memory address.
     */
    inline void *addr(void) const noexcept;
};

inline void *page_t::addr(void) const noexcept
{
    return reinterpret_cast<void*>(PFN_PHYS(m_pfn));
}

} // namespace memory
} // namespace core
} // namespace kernel

#endif // _KERNEL_MM_TYPES_HPP_