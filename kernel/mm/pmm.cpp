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
#include <kernel/debug.hpp>
#include <kernel/core.hpp>
#include <kernel/pmm.hpp>


namespace kernel {
namespace core {
namespace memory {

const phys_addr_t start_addr {0x00000000}; // physical memory start address


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

    m_free_pages = m_max_pages - m_used_pages;
    printk(KERN_DEBUG "free pages: %u\n", m_free_pages);
    printk(KERN_DEBUG "used pages: %u\n", m_used_pages);
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
        pos++;
        n--;
        m_used_pages--;
    }
}

void phys_mman_t::mark_as_used(phys_addr_t addr, size_t size) noexcept
{
    size_t pos = get_pos(addr);
    size_t n   = size / PAGE_SIZE;

    while (n > 0) {
        m_bitmap.set(pos);
        pos++;
        n--;
        m_used_pages++;
    }
}

// -------------------------------------------------------------------------------------

// TODO: implement getters for phys_mman_t members
// TODO: move functions below to shell builtins

const char *mem_types[5] = {
    "available",        // available RAM to use
    "reserved",         // reserved memory for kernel
    "ACPI reclaimable", // memory that managed by Advanced Configuration and Power Interface (ACPI)
    "NVS",              // Non-Volatile Storage memory (store data that must persist across system reboots)
    "bad RAM"           // should not be used by the OS
};

void phys_mman_t::print_entries(void) const noexcept
{
    multiboot_entry_t *mmmt;

    for (size_t i = 0; i < m_mboot->mmap_length; i += sizeof(multiboot_entry_t)) {
        mmmt = reinterpret_cast<multiboot_entry_t*>(m_mboot->mmap_addr + i);

        printk(KERN_DEBUG "addr: <%08p> ", mmmt->addr);
        printk("len: %u ", mmmt->len);
        printk("type: %s\n", mem_types[mmmt->type - 1]);
    }

    printk(KERN_DEBUG "total memory: %u KB\n", m_mem_total >> 0xA);
    printk(KERN_DEBUG "total pages:  %u\n", m_max_pages);
    printk(KERN_DEBUG "start addr:   <%08p>\n", start_addr);
    printk(KERN_DEBUG "bitmap addr:  <%08p>\n", m_bitmap.m_data);
}

phys_mman_t pmm;

} // namespace memory
} // namespace core
} // namespace kernel