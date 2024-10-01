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
#include <kernel/pmm.hpp>


namespace kernel {
namespace core {
namespace memory {

void phys_mman_t::detect_memory(void) noexcept
{
    multiboot_entry_t *mmmt;
    size_t   max_index = 0;
    uint32_t max_size  = 0;

    for (size_t i = 0; i < m_mboot->mmap_length; i += sizeof(multiboot_entry_t)) {
        mmmt = reinterpret_cast<multiboot_entry_t*>(m_mboot->mmap_addr + i);

        if (mmmt->type == MULTIBOOT_MEMORY_AVAILABLE) {
            // find entry with max available memory
            if (max_size < mmmt->len) {
                max_size  = mmmt->len;
                max_index = i;
            }
        }
    }

    // handle entry with max available memory
    mmmt         = reinterpret_cast<multiboot_entry_t*>(m_mboot->mmap_addr + max_index);
    m_start_addr = reinterpret_cast<uint32_t*>(mmmt->addr);
    m_mem_total  = mmmt->len;
}

void phys_mman_t::init(const multiboot_t& mboot) noexcept
{
    m_mboot = &mboot;
    detect_memory();

    m_max_pages = m_mem_total / PAGE_SIZE;
    auto size   = m_max_pages / kstd::BITS_PER_BYTE; // size in bytes

    // initialize bitmap right after the end of kernel
    m_mmap.init(KERNEL_END_PADDR, size);

    // set all memory as used
    kstd::memset(m_mmap.m_data, 0xFF, m_mmap.m_size);
}

// -------------------------------------------------------------------------------------

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
    printk(KERN_DEBUG "kernel start: <%08p>\n", KERNEL_START_PADDR);
    printk(KERN_DEBUG "kernel end:   <%08p>\n", KERNEL_END_PADDR);
}

phys_mman_t pmm;

} // namespace memory
} // namespace core
} // namespace kernel