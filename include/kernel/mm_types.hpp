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

#include <kernel/slab.hpp>


namespace kernel {
namespace core {
namespace memory {

inline const usize     PAGE_SIZE   {4_KB};
inline const bool       PAGE_FREE   {0};
inline const bool       PAGE_USED   {1};
inline const u8    PAGE_SHIFT  {0xC};

// page flags enumeration
enum PG : u8 {
    RESERVED = 0b10000000,   // empty pages or pages that do not even exist
    SLAB     = 0b01000000    // page frame is included in a slab
};

/**
 * @brief Convert page frame number to physical address.
 *
 * @param [in] pfn - given page frame number.
 * @return physical address.
 */
constexpr inline phys_addr_t PFN_PHYS(usize pfn) noexcept
{
    return static_cast<phys_addr_t>(pfn << PAGE_SHIFT);
}

/**
 * @brief Convert physical address to page frame number.
 *
 * @param [in] addr - given page physical address.
 * @return page frame number.
 */
constexpr inline usize PHYS_PFN(phys_addr_t addr) noexcept
{
    return addr >> PAGE_SHIFT;
}

struct page_t
{
    kmem::cache_t *cache;   // memory allocator cache (only if PG::SLAB is set)
    kmem::slab_t  *slab;    // memory allocator slab (only if PG::SLAB is set)
    usize         pfn;      // page frame number (position in bitmap/mem map)
    u8            flags;    // page status

    /**
     * @brief Get page memory address.
     *
     * @return page memory address.
     */
    inline void *addr(void) const noexcept;
};

inline void *page_t::addr(void) const noexcept
{
    return reinterpret_cast<void*>(PFN_PHYS(pfn));
}

} // namespace memory
} // namespace core
} // namespace kernel

#endif // _KERNEL_MM_TYPES_HPP_