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

#include <kernel/kstd/cstring.hpp>
#include <kernel/memlayout.hpp>
#include <kernel/printk.hpp>
#include <kernel/core.hpp>
#include <kernel/pmm.hpp>


namespace kernel {
namespace core {
namespace memory {

void phys_mman_t::detect_memory(void) noexcept
{
    multiboot_entry_t *mmmt;
    size_t i = 0;

    while (i < m_mboot->mmap_length) {
        mmmt = reinterpret_cast<multiboot_entry_t*>(m_mboot->mmap_addr + i);

        if (mmmt->type == MULTIBOOT_MEMORY_AVAILABLE)
            m_mem_available += mmmt->len;

        m_mem_total += mmmt->len;
        i += sizeof(multiboot_entry_t);
    }
}

void phys_mman_t::free_available_memory(void) noexcept
{
    multiboot_entry_t *mmmt;
    size_t i = 0;

    while (i < m_mboot->mmap_length) {
        mmmt = reinterpret_cast<multiboot_entry_t*>(m_mboot->mmap_addr + i);

        if (mmmt->type == MULTIBOOT_MEMORY_AVAILABLE)
            mark_as_free(mmmt->addr, mmmt->len);

        i += sizeof(multiboot_entry_t);
    }
}

void phys_mman_t::init(const multiboot_t& mboot) noexcept
{
    // check that multiboot memory map is set correctly
    if ((mboot.flags & (1 << 6)) == 0) {
        // TODO: replace with panic()
        printk(KERN_ERR "%s\n", "multiboot memory map wasn't set correctly");
        core::khalt();
    }

    m_mboot = &mboot;
    detect_memory();
    m_max_pages = m_mem_total >> PAGE_SHIFT;

    /** @warning There is an issue with overwriting global variables
     * with bitmap data, so I added additional offset to prevent that.*/
    auto bitmap_addr = const_cast<phys_addr_t*>(KERNEL_END_PTR) + STACK_SIZE;
    auto bitmap_size = BITS_TO_BYTES(m_max_pages);

    // physical memory bitmap starts right after the kernel end
    m_bitmap.init(bitmap_addr, bitmap_size);

    // setting memory map
    m_mem_map      = reinterpret_cast<page_t*>(m_bitmap.m_data + bitmap_size);
    m_mem_map_size = sizeof(page_t) * m_max_pages;

    // physical memory map starts right after the bitmap end
    kstd::memset(m_mem_map, 0, m_mem_map_size);

    // setting page frame numbers
    for (size_t i = 0; i < m_mem_map_size; i++)
        m_mem_map[i].m_pfn = i;

    // mark all memory as used
    kstd::memset(bitmap_addr, 0xFF, bitmap_size);
    m_used_pages = m_max_pages;

    free_available_memory();

    // mark kernel memory as used
    mark_as_used(phys_addr_t(KERNEL_START_PADDR), KERNEL_SIZE + PAGE_SIZE);

    // mark bitmap memory as used
    mark_as_used(phys_addr_t(m_bitmap.m_data), m_bitmap.m_size);

    // mark pages memory map as used
    mark_as_used(phys_addr_t(m_mem_map), m_mem_map_size);

    // first page containing reserved data (e.g. GDT), that should not
    // be accessed, so it was set as used:
    m_bitmap.set(0);
    m_mem_map[0].m_pfn = PG::RESERVED;
    m_used_pages++;
}

void phys_mman_t::mark_as_free(phys_addr_t addr, size_t size) noexcept
{
    size_t pos = PHYS_PFN(addr);
    size_t n   = size >> PAGE_SHIFT;

    while (n > 0) {
        m_bitmap.unset(pos);
        m_used_pages--;
        pos++;
        n--;
    }
}

void phys_mman_t::mark_as_used(phys_addr_t addr, size_t size) noexcept
{
    size_t pos = PHYS_PFN(addr);
    size_t n   = size >> PAGE_SHIFT;

    while (n > 0) {
        m_bitmap.set(pos);
        m_used_pages++;
        pos++;
        n--;
    }
}

size_t phys_mman_t::get_free_pages(gfp_t mask, uint32_t order) noexcept
{
    size_t pos, k;

    if (!(mask & GFP::KERNEL))
        return 0;

    uint32_t n = 1 << order; // find 2^order free pages

    for (size_t i = 0; i < m_bitmap.capacity(); i++) {
        // skip groups of used pages
        if (m_bitmap.m_data[i] != 0xFFFFFFFF) {
            // handle each group
            for (size_t j = 0; j < m_bitmap.bits_per_element(); j++) {
                pos = 32 * i + j;

                // skip until free page
                while (m_bitmap.get(pos) != PAGE_FREE)
                    pos++;

                if (m_bitmap.get(pos) == PAGE_FREE) {
                    // check that number of free pages equals to
                    // number of needed pages (n)
                    for (k = 0; k < n; k++) {
                        if (m_bitmap.get(pos + k) != PAGE_FREE)
                            break;
                    }

                    // if used page was found check next group of pages
                    if (k < n - 1)
                        continue;
                    else
                        return pos;
                }
            }
        }
    }

    return 0;
}

page_t *phys_mman_t::alloc_pages(gfp_t mask, uint32_t order) noexcept
{
    uint32_t n = 1 << order; // allocate 2^order pages

    // not enough of free blocks
    if((m_max_pages - m_used_pages) <= n)
        return nullptr;

    size_t start_pos = get_free_pages(mask, order);

    if (!start_pos)
        return nullptr;

    // set page to zero
    if (mask & GFP::ZERO) {
        auto addr = reinterpret_cast<void*>(PFN_PHYS(start_pos));
        kstd::memset(addr, 0, n << PAGE_SHIFT);
    }

    // set n pages as used
    for (size_t i = 0; i < n; i++)
        m_bitmap.set(start_pos + i);

    m_used_pages += n;

    return &m_mem_map[start_pos];
}

page_t *phys_mman_t::get_zeroed_page(gfp_t mask) noexcept
{
    // handle incorrect flags
    if (!(mask & GFP::ZERO))
        return nullptr;

    page_t *page = alloc_pages(mask, 0);
    return page;
}

void phys_mman_t::free_pages(phys_addr_t addr, uint32_t order) noexcept
{
    size_t pos = PFN_PHYS(addr);

    // handle freeing first page
    if (!pos) {
        // TODO: replace with panic():
        printk(KERN_ERR "%s\n", "it is forbidden to free the first page");
        core::khalt();
    }

    uint32_t n = 1 << order; // free 2^order pages

    // set n pages as free
    for (size_t i = 0; i < n; i++)
        m_bitmap.unset(pos + i);

    m_used_pages -= n;
}

phys_mman_t pmm;

} // namespace memory
} // namespace core
} // namespace kernel