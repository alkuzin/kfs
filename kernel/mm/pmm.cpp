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
#include <kernel/panic.hpp>
#include <kernel/pmm.hpp>


namespace kernel {
namespace core {
namespace memory {

struct phys_mman_t
{
    const multiboot_info_t  *mboot;
    kstd::bitmap_t<uint32_t> bitmap;  // physical memory map
    page_t *mem_map;
    size_t mem_map_size;
    size_t mem_total;                 // total physical memory
    size_t mem_available;             // total available memory
    size_t max_pages;                 // total number of pages
    size_t used_pages;
    size_t free_pages;
};

static phys_mman_t pmm {};

/** @brief Get information about memory regions.*/
static void detect_memory(void) noexcept
{
    multiboot_entry_t *mmmt;
    size_t i = 0;

    while (i < pmm.mboot->mmap_length) {
        mmmt = reinterpret_cast<multiboot_entry_t*>(pmm.mboot->mmap_addr + i);

        if (mmmt->type == MULTIBOOT_MEMORY_AVAILABLE)
            pmm.mem_available += mmmt->len;

        pmm.mem_total += mmmt->len;
        i += sizeof(multiboot_entry_t);
    }
}

/**
 * @brief Mark memory region as used.
 *
 * @param [in] addr - given base address of the region.
 * @param [in] size - given size of the region in bytes.
 */
static void mark_as_free(phys_addr_t addr, size_t size) noexcept
{
    size_t pos = PHYS_PFN(addr);
    size_t n   = size >> PAGE_SHIFT;

    while (n > 0) {
        pmm.bitmap.unset(pos);
        pmm.used_pages--;
        pos++;
        n--;
    }
}

/**
 * @brief Mark memory region as free.
 *
 * @param [in] addr - given base address of the region.
 * @param [in] size - given size of the region in bytes.
 */
static void mark_as_used(phys_addr_t addr, size_t size) noexcept
{
    size_t pos = PHYS_PFN(addr);
    size_t n   = size >> PAGE_SHIFT;

    while (n > 0) {
        pmm.bitmap.set(pos);
        pmm.used_pages++;
        pos++;
        n--;
    }
}


/** @brief Free all available memory regions.*/
static void free_available_memory(void) noexcept
{
    multiboot_entry_t *mmmt;
    size_t i = 0;

    while (i < pmm.mboot->mmap_length) {
        mmmt = reinterpret_cast<multiboot_entry_t*>(pmm.mboot->mmap_addr + i);

        if (mmmt->type == MULTIBOOT_MEMORY_AVAILABLE)
            mark_as_free(mmmt->addr, mmmt->len);

        i += sizeof(multiboot_entry_t);
    }
}

void init(const multiboot_t& mboot) noexcept
{
    // check that multiboot memory map is set correctly
    if ((mboot.flags & (1 << 6)) == 0)
        panic("%s\n", "multiboot memory map wasn't set correctly");

    pmm.mboot = &mboot;
    detect_memory();
    pmm.max_pages = pmm.mem_total >> PAGE_SHIFT;

    /** @warning There is an issue with overwriting global variables
     * with bitmap data, so I added additional offset to prevent that.*/
    auto bitmap_addr = const_cast<phys_addr_t*>(KERNEL_END_PTR) + STACK_SIZE;
    auto bitmap_size = BITS_TO_BYTES(pmm.max_pages);

    // physical memory bitmap starts right after the kernel end
    pmm.bitmap.init(bitmap_addr, bitmap_size);

    // setting memory map
    auto pos = pmm.bitmap.m_data + bitmap_size;
    pmm.mem_map      = reinterpret_cast<page_t*>(pos);
    pmm.mem_map_size = sizeof(page_t) * pmm.max_pages;

    // physical memory map starts right after the bitmap end
    kstd::memset(pmm.mem_map, 0, pmm.mem_map_size);

    // setting page frame numbers
    for (size_t i = 0; i < pmm.max_pages; i++) {
        pmm.mem_map[i].m_cache = nullptr;
        pmm.mem_map[i].m_slab  = nullptr;
        pmm.mem_map[i].m_flags = 0;
        pmm.mem_map[i].m_pfn   = i;
    }

    // mark all memory as used
    kstd::memset(bitmap_addr, 0xFF, bitmap_size);
    pmm.used_pages = pmm.max_pages;

    free_available_memory();

    // mark kernel memory as used
    mark_as_used(phys_addr_t(KERNEL_START_PADDR), KERNEL_SIZE + PAGE_SIZE);

    // mark memory between kernel end & bitmap
    mark_as_used(phys_addr_t(bitmap_addr - STACK_SIZE), STACK_SIZE);

    // mark bitmap memory as used
    mark_as_used(phys_addr_t(pmm.bitmap.m_data), pmm.bitmap.m_size);

    // mark pages memory map as used
    mark_as_used(phys_addr_t(pmm.mem_map), pmm.mem_map_size);

    // first page containing reserved data (e.g. GDT), that should not
    // be accessed, so it was set as used:
    pmm.bitmap.set(0);
    pmm.mem_map[0].m_pfn = PG::RESERVED;
    pmm.used_pages++;
}

/**
 * @brief  Get free pages.
 *
 * @param [in] mask - given allocation flags.
 * @param [in] order - given power of two (finding 2^order pages).
 * @return page position in bitmap - in case of success.
 * @return 0 - in case of error.
 */
static size_t get_free_pages(gfp_t mask, uint32_t order) noexcept
{
    size_t pos, k;

    if (!(mask & GFP::KERNEL))
        return 0;

    uint32_t n = 1 << order; // find 2^order free pages

    for (size_t i = 0; i < pmm.bitmap.capacity(); i++) {
        // skip groups of used pages
        if (pmm.bitmap.m_data[i] != 0xFFFFFFFF) {
            // handle each group
            for (size_t j = 0; j < pmm.bitmap.bits_per_element(); j++) {
                pos = 32 * i + j;

                // skip until free page
                while (pmm.bitmap.get(pos) != PAGE_FREE)
                    pos++;

                if (pmm.bitmap.get(pos) == PAGE_FREE) {
                    // check that number of free pages equals to
                    // number of needed pages (n)
                    for (k = 0; k < n; k++) {
                        if (pmm.bitmap.get(pos + k) != PAGE_FREE)
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

page_t *alloc_pages(gfp_t mask, uint32_t order) noexcept
{
    uint32_t n = 1 << order; // allocate 2^order pages

    // not enough of free blocks
    if((pmm.max_pages - pmm.used_pages) <= n)
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
        pmm.bitmap.set(start_pos + i);

    pmm.used_pages += n;

    return &pmm.mem_map[start_pos];
}

page_t *get_zeroed_page(gfp_t mask) noexcept
{
    page_t *page = alloc_pages(mask | GFP::ZERO, 0);
    return page;
}

void free_pages(phys_addr_t addr, uint32_t order) noexcept
{
    size_t pos = PFN_PHYS(addr);

    // handle freeing first page
    if (!pos)
        panic("%s\n", "it is forbidden to free the first page");

    uint32_t n = 1 << order; // free 2^order pages

    // set n pages as free
    for (size_t i = 0; i < n; i++)
        pmm.bitmap.unset(pos + i);

    pmm.used_pages -= n;
}

page_t *get_page(phys_addr_t addr) noexcept
{
    size_t pfn = PHYS_PFN(addr);
    return &pmm.mem_map[pfn];
}

const char *mem_types[5] = {
    "available",        // available RAM to use
    "reserved",         // reserved memory for kernel
    "ACPI reclaimable", // memory that managed by Advanced Configuration and Power Interface (ACPI)
    "NVS",              // Non-Volatile Storage memory (store data that must persist across system reboots)
    "bad RAM"           // should not be used by the OS
};

void display_memory(void) noexcept
{
    multiboot_entry_t *mmmt;

    for (size_t i = 0; i < pmm.mboot->mmap_length; i += sizeof(multiboot_entry_t)) {
        mmmt = reinterpret_cast<multiboot_entry_t*>(pmm.mboot->mmap_addr + i);

        printk("%#08X-", mmmt->addr);
        printk("%#08X  ", mmmt->addr + mmmt->len - 1);
        printk("%u KB  ", mmmt->len >> 0xA);
        printk("<%s>\n", mem_types[mmmt->type - 1]);
    }

    printk("Memory page size:   %u KB\n", PAGE_SIZE);
    printk("Total memory:       %u KB\n", pmm.mem_total >> 0xA);
    printk("Used memory:        %u KB\n", (pmm.used_pages * PAGE_SIZE) >> 0xA);
}

} // namespace memory
} // namespace core
} // namespace kernel