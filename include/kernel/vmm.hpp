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
 * @file  vmm.hpp
 * @brief Declares virtual memory manager.
 *
 * @author Alexander Kuzin (<a href="https://github.com/alkuzin">alkuzin</a>)
 * @date   09.10.2024
 */

#ifndef _KERNEL_VMM_HPP_
#define _KERNEL_VMM_HPP_

#include <kernel/linkage.hpp>
#include <kernel/types.hpp>


namespace kernel {
namespace core {
namespace memory {

// TODO: move to mm_types.hpp:
inline const auto PAGE_DIR_SIZE     {1024};
inline const auto PAGE_DIR_SHIFT    {0x0A};
inline const auto PAGE_TABLE_SIZE   {1024};
inline const auto PAGE_TABLE_SHIFT  {0x0A};

// Page directory entry bits
enum class PDE : s32 {
    PRESENT       = 0x001,   // page is in physical memory at the moment
    READ_WRITE    = 0x002,   // page read/write or read-only
    USER          = 0x004,   // page access control based on privilege level
    WRITE_THROUGH = 0x008,   // write-through caching is enabled/disabled
    CACHE_DISABLE = 0x010,   // will the page be cached or not
    ACCESSED      = 0x020,   // PDE was read during virtual address translation
    PAGE_SIZE     = 0x040    // 0 - page is 4 KB, 1 - page is 4 MB
};

// Page table entry bits
enum class PTE : s32 {
    PRESENT       = 0x001,   // page is in physical memory at the moment
    READ_WRITE    = 0x002,   // page read/write or read-only
    USER          = 0x004,   // page access control based on privilege level
    WRITE_THROUGH = 0x008,   // write-through caching is enabled/disabled
    CACHE_DISABLE = 0x010,   // will the page be cached or not
    ACCESSED      = 0x020,   // PTE was read during virtual address translation
    DIRTY         = 0x040,   // determine whether a page has been written to
    PAT           = 0x080,   // page attribute table
    GLOBAL        = 0x100    // not to invalidate the TLB entry
};

using pd_entry = u32;
using pt_entry = u32;

// Page directory
struct page_dir_t
{
    pd_entry entries[PAGE_DIR_SIZE];
} __attribute__((aligned(4096)));

// Page table
struct page_table_t
{
    pt_entry entries[PAGE_TABLE_SIZE];
} __attribute__((aligned(4096)));

namespace vmm {

/** Initialize virtual memory manager.*/
void init(void);

} // namespace vmm
} // namespace memory
} // namespace core
} // namespace kernel

#endif // _KERNEL_VMM_HPP_