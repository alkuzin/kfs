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
 * @file  gdt.hpp
 * @brief Contains Global Decriptor Table declaration.
 * 
 * @author Alexander Kuzin (<a href="https://github.com/alkuzin">alkuzin</a>)
 * @date   26.09.2024
 */

#ifndef _KERNEL_ARCH_X86_GDT_HPP_
#define _KERNEL_ARCH_X86_GDT_HPP_

#include <kernel/kstd/ctypes.hpp>


namespace kernel {
namespace arch {
namespace x86 {
namespace gdt {

/** @brief GDT segment structure in 32-bit mode.*/
struct entry_t
{
    uint16_t m_limit;       // maximum addressable unit.
    uint16_t m_base_low;    // linear address where the segment begins (low bytes).
    uint8_t  m_base_mid;    // linear address where the segment begins (middle byte).
    uint8_t  m_access;      // privileges of segment.
    uint8_t  m_flags;       // segment mode (16-bit, 32-bit or long mode - 64-bit).
    uint8_t  m_base_high;   // linear address where the segment begins (high byte).
} __attribute__((packed));

/** @brief GDT pointer.*/
struct ptr_t
{
   uint16_t m_size;     // GDT size - 1
   uint32_t m_offset;   // linear address of GDT.
} __attribute__((packed));


/** @brief Initialize Global Descriptor Table.*/
void init(void) noexcept;
    
} // namespace gdt    
} // namespace x86
} // namespace arch
} // namespace kernel

#endif // _KERNEL_ARCH_X86_GDT_HPP_