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
    kstd::bitmap_t<uint32_t> m_mmap;    // physical memory map
    const multiboot_info_t *m_mboot;
    size_t m_mem_total;
    size_t m_max_pages;
    uint32_t *m_start_addr;

private:
    /** @brief Get information about memory regions.*/
    void detect_memory(void) noexcept;

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