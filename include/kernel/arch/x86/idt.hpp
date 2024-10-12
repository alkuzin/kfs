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
 * @file  idt.hpp
 * @brief Provides definitions for Interrupt Descriptor Table (IDT).
 *
 * @author Alexander Kuzin (<a href="https://github.com/alkuzin">alkuzin</a>)
 * @date   12.10.2024
 */

#ifndef _KERNEL_ARCH_X86_IDT_HPP_
#define _KERNEL_ARCH_X86_IDT_HPP_

#include <kernel/types.hpp>


namespace kernel {
namespace arch {
namespace x86 {
namespace idt {

/** @brief IDT gate descriptor structure in 32-bit mode.*/
struct entry_t
{
    uint16_t m_offset_low;  // entry point of the ISR (lower bits)
    uint16_t m_selector;    // point to a valid code segment in GDT
    uint8_t  m_reserved;    // unused
    uint8_t  m_flags;       // gate type & other control bits
    uint16_t m_offset_high; // entry point of the ISR (higher bits)
} __attribute__((packed));

/** @brief IDT pointer.*/
struct ptr_t
{
    uint16_t m_size;        // size of IDT
    uint32_t m_offset;      // the linear address of the IDT
} __attribute__((packed));

/** @brief Initialize Interrupt Descriptor Table.*/
void init(void) noexcept;

} // namespace idt
} // namespace x86
} // namespace arch
} // namespace kernel

#endif // _KERNEL_ARCH_X86_IDT_HPP_