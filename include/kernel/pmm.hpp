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
page_t *alloc_pages(gfp_t mask, u32 order) noexcept;

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
void free_pages(phys_addr_t addr, u32 order) noexcept;

/**
 * @brief Get the page struct.
 *
 * @param [in] addr - given memory address.
 * @return page struct.
 */
page_t *get_page(phys_addr_t addr) noexcept;

/** @brief Display memory usage.*/
void display_memory(void) noexcept;

} // namespace memory
} // namespace core
} // namespace kernel

#endif // _KERNEL_PMM_HPP_