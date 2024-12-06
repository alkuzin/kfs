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

#ifndef _KERNEL_ARCH_I686_GDT_HPP_
#define _KERNEL_ARCH_I686_GDT_HPP_

#include <kernel/types.hpp>


namespace kernel {
namespace arch {
namespace i686 {
namespace gdt {

inline const u32 GDT_BASE {0x00000800}; // GDT pointer address

// GDT segment offsets
inline const u8 NULL_SEGMENT           {0x00};
inline const u8 KERNEL_CODE_SEGMENT    {0x08};
inline const u8 KERNEL_DATA_SEGMENT    {0x10};
inline const u8 KERNEL_STACK_SEGMENT   {0x18};
inline const u8 USER_CODE_SEGMENT      {0x20};
inline const u8 USER_DATA_SEGMENT      {0x28};
inline const u8 USER_STACK_SEGMENT     {0x30};

/** @brief GDT segment structure in 32-bit mode.*/
struct entry_t
{
    u16 limit;       // maximum addressable unit.
    u16 base_low;    // linear address where the segment begins.
    u8  base_mid;    // linear address where the segment begins.
    u8  access;      // privileges of segment.
    u8  flags;       // segment mode
    u8  base_high;   // linear address where the segment begins.
} __attribute__((packed));

/** @brief GDT pointer.*/
struct ptr_t
{
   u16 size;     // GDT size - 1
   u32 offset;   // linear address of GDT.
} __attribute__((packed));

/** @brief Initialize Global Descriptor Table.*/
void init(void);

} // namespace gdt
} // namespace i686
} // namespace arch
} // namespace kernel

#endif // _KERNEL_ARCH_I686_GDT_HPP_