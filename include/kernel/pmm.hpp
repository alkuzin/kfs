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
 * @file  pmm.hpp
 * @brief Declares physical memory manager.
 *
 * @author Alexander Kuzin (<a href="https://github.com/alkuzin">alkuzin</a>)
 * @date   30.09.2024
 */

#ifndef _KERNEL_PMM_HPP_
#define _KERNEL_PMM_HPP_

#include <kernel/kstd/bitmap.hpp>
#include <kernel/multiboot.hpp>
#include <kernel/mm_types.hpp>
#include <kernel/gfp.hpp>


namespace kernel {
namespace core {
namespace memory {

struct phys_mman_t
{
    const multiboot_info_t  *m_mboot;
    kstd::bitmap_t<uint32_t> m_bitmap;  // physical memory map
    page_t *m_mem_map;
    size_t m_mem_map_size;
    size_t m_mem_total;                 // total physical memory
    size_t m_mem_available;             // total available memory
    size_t m_max_pages;                 // total number of pages
    size_t m_used_pages;
    size_t m_free_pages;

private:
    /** @brief Get information about memory regions.*/
    void detect_memory(void) noexcept;

    /** @brief Free all available memory regions.*/
    void free_available_memory(void) noexcept;

    /**
     * @brief Mark memory region as free.
     *
     * @param [in] addr - given base address of the region.
     * @param [in] size - given size of the region in bytes.
     */
    void mark_as_used(phys_addr_t addr, size_t size) noexcept;

    /**
     * @brief Mark memory region as used.
     *
     * @param [in] addr - given base address of the region.
     * @param [in] size - given size of the region in bytes.
     */
    void mark_as_free(phys_addr_t addr, size_t size) noexcept;

    /**
     * @brief  Get free pages.
     *
     * @param [in] mask - given allocation flags.
     * @param [in] order - given power of two (finding 2^order pages).
     * @return page position in bitmap - in case of success.
     * @return 0 - in case of error.
     */
    size_t get_free_pages(gfp_t mask, uint32_t order) noexcept;

public:
    /**
     * @brief Initialize the physical memory manager.
     *
     * @param [in] mboot - given multiboot information structure.
     */
    void init(const multiboot_t& mboot) noexcept;

    /**
     * @brief Allocate memory pages.
     *
     * @param [in] mask - given allocation flags.
     * @param [in] order - given power of two (allocating 2^order pages).
     * @return allocated page pointer - in case of success.
     * @return nullptr - in case of errors.
     */
    page_t *alloc_pages(gfp_t mask, uint32_t order) noexcept;

    /**
     * @brief Get the zeroed page.
     *
     * @param [in] mask - given allocation flags.
     * @return allocated zeroed page pointer - in case of success.
     * @return nullptr - in case of errors.
     */
    page_t *get_zeroed_page(gfp_t mask) noexcept;

    /**
     * @brief Free allocated pages.
     *
     * @param [in] addr - given first page address.
     * @param [in] order - given power of two (freeing 2^order pages).
     */
    void free_pages(phys_addr_t addr, uint32_t order) noexcept;

    /**
     * @brief Get the page struct.
     *
     * @param [in] addr - given memory address.
     * @return page struct.
     */
    page_t *get_page(phys_addr_t addr) const noexcept;
};

extern phys_mman_t pmm;

} // namespace memory
} // namespace core
} // namespace kernel

#endif // _KERNEL_PMM_HPP_