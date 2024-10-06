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

inline const auto      KERNEL_START_PADDR {phys_addr_t(&kernel_phys_start)};
inline const auto      KERNEL_END_PADDR   {phys_addr_t(&kernel_phys_end)};
inline const uint32_t *KERNEL_START_PTR   {reinterpret_cast<uint32_t*>(KERNEL_START_PADDR)};
inline const uint32_t *KERNEL_END_PTR     {reinterpret_cast<uint32_t*>(KERNEL_END_PADDR)};

inline const phys_addr_t MEM_START_PADDR    {0x00000};
inline const uint32_t    STACK_SIZE         {0x10000}; // 64 KB

#define KERNEL_SIZE ((KERNEL_END_PADDR) - (KERNEL_START_PADDR))

} // namespace memory
} // namespace core
} // namespace kernel

#endif // _KERNEL_MEMLAYOUT_HPP_