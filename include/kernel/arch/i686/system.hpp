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
 * @file  system.hpp
 * @brief Contains system functions.
 *
 * @author Alexander Kuzin (<a href="https://github.com/alkuzin">alkuzin</a>)
 * @date   26.09.2024
 */

#ifndef _KERNEL_ARCH_I686_SYSTEM_HPP_
#define _KERNEL_ARCH_I686_SYSTEM_HPP_

#include <kernel/arch/i686/register.hpp>


namespace kernel {
namespace arch {
namespace i686 {

/** @brief Halt CPU.*/
inline void halt(void) noexcept
{
    for (;;) asm volatile("hlt");
}

/**
 * @brief Get current privilege level .
 *
 * @return current privilege level.
 */
inline u8 ring(void) noexcept
{
    return get_register(REG::CS) & 0x3;
}

/**
 * @brief Get current mode.
 *
 * @return mode in bits.
 */
inline u32 mode(void) noexcept
{
    u16 cs = static_cast<u16>(get_register(REG::CS));

    if (cs & 0x0000FFFF)
        return 32;
    else if (cs & 0xFFFF0000)
        return 64;
    else
        return 16;
}

/** @brief Enable interrupts.*/
inline void sti(void) noexcept
{
    asm volatile("sti");
}

/** @brief Disable interrupts.*/
inline void cli(void) noexcept
{
    asm volatile("cli");
}

/**
 * @brief Enable paging.
 *
 * @param [in] page_directory - given page directory pointer.
 */
inline void enable_paging(void *page_directory) noexcept
{
    // asm volatile("mov %%eax, %%cr3" : : "a"(page_directory));
    // asm volatile("mov %cr0, %eax; or $0x80000001, %eax; mov %eax, %cr0");

    asm volatile(
        "push %%eax;"
        "mov %0, %%eax;"
        "mov %%eax, %%cr3;"
        "mov %%cr4, %%eax;"
        "and $-0x00000010, %%eax;"
        "mov %%eax, %%cr4;"
        "mov %%cr0, %%eax;"
        "or $0x80000000, %%eax;"
        "mov %%eax, %%cr0;"
        "pop %%eax;"
        :: "r"(page_directory)
    );
}

} // namespace i686
} // namespace arch
} // namespace kernel

#endif // _KERNEL_ARCH_I686_SYSTEM_HPP_