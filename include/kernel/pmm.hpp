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


namespace kernel {
namespace core {
namespace memory {

const size_t PAGE_SIZE {4096}; // 4 KB

struct phys_mman_t
{
    kstd::bitmap_t<uint32_t> m_bitmap;  // physical memory map
    const multiboot_info_t   *m_mboot;
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
     * @brief Get the physical address of specific page.
     *
     * @param [in] pos - given position of page.
     * @return physical address.
     */
    inline phys_addr_t get_addr(size_t pos) const noexcept;

    /**
     * @brief Get the position of page.
     *
     * @param [in] addr - given page address.
     * @return page position.
     */
    inline size_t get_pos(phys_addr_t addr) const noexcept;

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

public:
    /**
     * @brief Initialize the physical memory manager.
     *
     * @param [in] mboot - given multiboot information structure.
     */
    void init(const multiboot_t& mboot) noexcept;

    /** @brief Print multiboot entries content.*/
    void print_entries(void) const noexcept;
};

extern phys_mman_t pmm;

} // namespace memory
} // namespace core
} // namespace kernel

#endif // _KERNEL_PMM_HPP_