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

#ifndef _KERNEL_ARCH_X86_SYSTEM_HPP_
#define _KERNEL_ARCH_X86_SYSTEM_HPP_

#include <kernel/arch/x86/register.hpp>


namespace kernel {
namespace arch {
namespace x86 {

/** @brief Halt CPU.*/
inline void halt(void) noexcept
{
    for (;;) __asm__ volatile("hlt");
}

/**
 * @brief Get current privilege level .
 *
 * @return current privilege level.
 */
inline uint8_t ring(void) noexcept
{
    uint16_t cs;
    __asm__ volatile ("mov %%cs, %0" : "=r"(cs)); // Read the CS register into cs
    return cs & 0x3; // Return the lower 2 bits
}

/**
 * @brief Get current mode.
 *
 * @return mode in bits.
 */
inline uint32_t mode(void) noexcept
{
    // TODO: replace with reg::cs() for getting cs register:
    uint16_t cs;

    __asm__ volatile ("mov %%cs, %0" : "=r"(cs));

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
    __asm__ volatile ("sti");
}

/** @brief Disable interrupts.*/
inline void cli(void) noexcept
{
    __asm__ volatile ("cli");
}

} // namespace x86
} // namespace arch
} // namespace kernel

#endif // _KERNEL_ARCH_X86_SYSTEM_HPP_