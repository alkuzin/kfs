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
 * @file  debug.hpp
 * @brief Contains kernel debug functions.
 *
 * @author Alexander Kuzin (<a href="https://github.com/alkuzin">alkuzin</a>)
 * @date   26.09.2024
 */

#ifndef _KERNEL_DEBUG_HPP_
#define _KERNEL_DEBUG_HPP_

#include <kernel/types.hpp>


namespace kernel {
namespace debug {

/**
 * @brief Get ESP (stack pointer) register value.
 *
 * @return esp register value.
 */
inline uint32_t esp(void) noexcept
{
    uint32_t ret;
    __asm__ volatile("mov %%esp, %0" : "=r"(ret));
    return ret;
}

/**
 * @brief Get EBP (base pointer) register value.
 *
 * @return ebp register value.
 */
inline uint32_t ebp(void) noexcept
{
    uint32_t ret;
    __asm__ volatile("mov %%ebp, %0" : "=r"(ret));
    return ret;
}

/**
 * @brief Get dump of kernel stack.
 *
 * @param [in] addr - given stack pointer.
 * @param [in] size - given number of bytes to dump.
 */
void kdump(uint32_t addr, size_t size) noexcept;

} // namespace debug
} // namespace kernel

#endif // _KERNEL_DEBUG_HPP_