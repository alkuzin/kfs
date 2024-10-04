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
 * @file  memlayout.hpp
 * @brief Declares kernel memory layout.
 *
 * @author Alexander Kuzin (<a href="https://github.com/alkuzin">alkuzin</a>)
 * @date   01.10.2024
 */

#ifndef _KERNEL_MEMLAYOUT_HPP_
#define _KERNEL_MEMLAYOUT_HPP_

#include <kernel/types.hpp>

extern kernel::uint32_t kernel_phys_start;
extern kernel::uint32_t kernel_phys_end;

namespace kernel {
namespace core {
namespace memory {

inline const phys_addr_t MEM_START_PADDR    {0x00000000}; // physical memory start address
inline const uint32_t   *KERNEL_START_PADDR {reinterpret_cast<uint32_t*>(&kernel_phys_start)};
inline const uint32_t   *KERNEL_END_PADDR   {reinterpret_cast<uint32_t*>(&kernel_phys_end)};

} // namespace memory
} // namespace core
} // namespace kernel

#endif // _KERNEL_MEMLAYOUT_HPP_