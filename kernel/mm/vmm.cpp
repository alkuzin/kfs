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

#include <kernel/arch/i686/system.hpp>
#include <kernel/memlayout.hpp>
#include <kernel/printk.hpp>
#include <kernel/panic.hpp>
#include <kernel/debug.hpp>
#include <kernel/vmm.hpp>
#include <kernel/pmm.hpp>


namespace kernel {
namespace core {
namespace memory {

/**
 * @brief Set the specific attribute of given entry.
 *
 * @param [in] entry - given entry to change.
 * @param [in] attr - given attribute to set.
 */
template <typename T>
constexpr inline void set_attribute(u32 *entry, T attr) noexcept
{
    set_bit<u32>(*entry, s32(attr));
}

/**
 * @brief Clear the specific attribute of given entry.
 *
 * @param [in] entry - given entry to change.
 * @param [in] attr - given attribute to clear.
 */
template <typename T>
constexpr inline void clear_attribute(u32 *entry, T attr) noexcept
{
    clear_bit<u32>(*entry, s32(attr));
}

/**
 * @brief Get the specific attribute of given entry.
 *
 * @param [in] entry - given entry.
 * @param [in] attr - given attribute to get.
 */
template <typename T>
constexpr inline bool get_attribute(u32& entry, T attr) noexcept
{
    return test_bit<u32>(entry, s32(attr));
}

/**
 * @brief Get page directory index from virtual address.
 *
 * @param [in] vaddr - given virtual address.
 * @return page directory index.
 */
constexpr inline u32 pd_index(virt_addr_t vaddr) noexcept
{
    return ((vaddr >> 22) & 0x3ff);
}

/**
 * @brief Get page table index from virtual address.
 *
 * @param [in] vaddr - given virtual address.
 * @return page table index.
 */
constexpr inline u32 pt_index(virt_addr_t vaddr) noexcept
{
    return ((vaddr >> 12) & 0x3ff);
}

/**
 * @brief Set the frame number for page.
 *
 * @param [out] entry - given entry to set.
 * @param [in] addr - given frame/page table address.
 */
constexpr inline void set_frame(u32 *entry, phys_addr_t addr) noexcept
{
    *entry = ((*entry & ~0x7FFFF000) | addr);
}


static page_dir_t *cur_page_dir {nullptr};

/**
 * @brief Set the page directory.
 *
 * @param [in] pd - given new page directory to set.
 */
static void set_page_directory(const page_dir_t *pd) noexcept
{
    if (!pd)
        panic("incorrect page directory: <%08p>\n", pd);

    cur_page_dir = const_cast<page_dir_t*>(pd);
    asm volatile("movl %%eax, %%cr3" : : "a"(cur_page_dir));
}

namespace vmm {

void init(void) noexcept
{
    using namespace arch::i686;

    // allocating memory for page directory
    void *addr = get_zeroed_page(GFP::KERNEL)->addr();
    auto dir   = reinterpret_cast<page_dir_t*>(addr);

    if (!dir)
        panic("%s\n", "error to allocate memory for setting page directory");

    // contain initial page directory address
    u32 cr3 = get_register(REG::CR3);
    auto initial_page_dir = reinterpret_cast<page_dir_t*>(cr3);

    // allocating memory for page directory
    addr = get_zeroed_page(GFP::KERNEL)->addr();
    dir  = reinterpret_cast<page_dir_t*>(addr);

    if (!dir)
        panic("%s\n", "error to allocate memory for setting page directory");

    // copy initial page directory to local page directory
    for (usize i = 0; i < PAGE_DIR_SIZE; i++)
        dir->entries[i] = initial_page_dir->entries[i];

    set_page_directory(dir);
    enable_paging(phys_addr_t(dir));
}

} // namespace vmm
} // namespace memory
} // namespace core
} // namespace kernel