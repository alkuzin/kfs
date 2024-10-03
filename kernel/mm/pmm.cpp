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

    for (size_t i = 0; i < m_mboot->mmap_length; i += sizeof(multiboot_entry_t)) {
        mmmt = reinterpret_cast<multiboot_entry_t*>(m_mboot->mmap_addr + i);

        if (mmmt->type == MULTIBOOT_MEMORY_AVAILABLE)
            m_mem_available += mmmt->len;

        m_mem_total += mmmt->len;
    }
}

void phys_mman_t::free_available_memory(void) noexcept
{
    multiboot_entry_t *mmmt;

    for (size_t i = 0; i < m_mboot->mmap_length; i += sizeof(multiboot_entry_t)) {
        mmmt = reinterpret_cast<multiboot_entry_t*>(m_mboot->mmap_addr + i);

        if (mmmt->type == MULTIBOOT_MEMORY_AVAILABLE)
            mark_as_free(mmmt->addr, mmmt->len);
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
    m_max_pages = m_mem_total / PAGE_SIZE;

    // physical memory bitmap starts right after the kernel end
    m_bitmap.init(KERNEL_END_PADDR, m_max_pages / kstd::BITS_PER_BYTE);

    // mark all memory as used
    kstd::memset(m_bitmap.m_data, 0xFF, m_bitmap.m_size);
    m_used_pages = m_max_pages;

    free_available_memory();

    // mark kernel memory as used
    mark_as_used(reinterpret_cast<phys_addr_t>(KERNEL_START_PADDR), KERNEL_END_PADDR - KERNEL_START_PADDR);

    // mark bitmap memory as used
    mark_as_used(reinterpret_cast<phys_addr_t>(m_bitmap.m_data), m_bitmap.m_size);

    // for error handeling purpose setting first block as used
    // in order to make possible to use null address as undefined
    m_bitmap.set(0);
    m_used_pages++;
}

inline phys_addr_t phys_mman_t::get_addr(size_t pos) const noexcept
{
    return start_addr + PAGE_SIZE * pos;
}

inline size_t phys_mman_t::get_pos(phys_addr_t addr) const noexcept
{
    return addr / PAGE_SIZE;
}

void phys_mman_t::mark_as_free(phys_addr_t addr, size_t size) noexcept
{
    size_t pos = get_pos(addr);
    size_t n   = size / PAGE_SIZE;

    while (n > 0) {
        m_bitmap.unset(pos);
        m_used_pages--;
        pos++;
        n--;
    }
}

void phys_mman_t::mark_as_used(phys_addr_t addr, size_t size) noexcept
{
    size_t pos = get_pos(addr);
    size_t n   = size / PAGE_SIZE;

    while (n > 0) {
        m_bitmap.set(pos);
        m_used_pages++;
        pos++;
        n--;
    }
}

// -------------------------------------------------------------------------------------

size_t phys_mman_t::get_free_pages(size_t n) noexcept
{
    size_t pos, k;

    for (size_t i = 0; i < m_max_pages / m_bitmap.bits_per_element(); i++) {
        // skip groups of used pages
        if (m_bitmap.m_data[i] != 0xFFFFFFFF) {
            // handle each group
            for (size_t j = 0; j < m_bitmap.bits_per_element(); j++) {
                pos = 32 * i + j + 1;

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

                    // if used page was found - start checking next group of pages
                    if (k < n - 1)
                        continue;
                    else
                        return pos;
                }
            }
        }
    }

    return undefined;
}

phys_addr_t phys_mman_t::alloc_pages(size_t n) noexcept
{
    // not enough of free blocks
    if((m_max_pages - m_used_pages) <= n)
        return undefined;

    // handle 0 pages to allocate
    if (n == 0)
        return undefined;

    size_t start_pos = get_free_pages(n);

    if (!start_pos)
        return undefined;

    // set n pages as used
    for (size_t i = 0; i < n; i++)
        m_bitmap.set(start_pos + i);

    m_used_pages += n;

    return get_addr(start_pos);
}

void phys_mman_t::free_pages(phys_addr_t addr, size_t n) noexcept
{
    size_t pos = get_pos(addr);

    // handle freeing first page
    if (!pos) {
        // TODO: replace with panic():
        printk(KERN_ERR "%s\n", "it is forbidden to free the first page");
        core::khalt();
    }

    // set n pages as free
    for (size_t i = 0; i < n; i++)
        m_bitmap.unset(pos + i);

    m_used_pages -= n;
}

phys_mman_t pmm;

} // namespace memory
} // namespace core
} // namespace kernel