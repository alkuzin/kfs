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
 * @file  io.hpp
 * @brief Contains functions for input/output operations on ports.
 * 
 * @author Alexander Kuzin (<a href="https://github.com/alkuzin">alkuzin</a>)
 * @date   26.09.2024
 */

#ifndef _KERNEL_ARCH_X86_IO_HPP_
#define _KERNEL_ARCH_X86_IO_HPP_

#include <kernel/kstd/ctypes.hpp>


namespace kernel {
namespace arch {
namespace x86 {

/**
 * @brief Receive a byte of data from a specified input/output port.
 *
 * @param [in] port - given port from which the data will be read.
 * @return the byte of data read from the port.
 */
inline uint8_t inb(uint16_t port) noexcept
{
    uint8_t rv = 0;
    __asm__ volatile("inb %1, %0" : "=a" (rv) : "dN" (port));
    return rv;
}
    
} // namespace x86
} // namespace arch
} // namespace kernel

#endif // _KERNEL_ARCH_X86_IO_HPP_