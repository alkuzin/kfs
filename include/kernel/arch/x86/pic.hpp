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
 * @file  pic.hpp
 * @brief Provides definitions for Programmable Interrupt Controller (PIC).
 *
 * @author Alexander Kuzin (<a href="https://github.com/alkuzin">alkuzin</a>)
 * @date   12.10.2024
 */

#ifndef _KERNEL_ARCH_X86_PIC_HPP_
#define _KERNEL_ARCH_X86_PIC_HPP_

#include <kernel/types.hpp>


namespace kernel {
namespace arch {
namespace x86 {
namespace pic {

// 8259 PIC input/output ports:
inline const uint8_t MASTER_PIC_CMD             {0x20};
inline const uint8_t MASTER_PIC_DATA            {0x21};
inline const uint8_t SLAVE_PIC_CMD              {0xA0};
inline const uint8_t SLAVE_PIC_DATA             {0xA1};

// PIC configurations:
inline const uint8_t PIC_INIT_CMD               {0x11};
inline const uint8_t MASTER_PIC_VECTOR_OFFSET   {0x20};
inline const uint8_t SLAVE_PIC_VECTOR_OFFSET    {0x28};
inline const uint8_t MASTER_PIC_CASCADE         {0x04};
inline const uint8_t SLAVE_PIC_CASCADE          {0x02};
inline const uint8_t PIC_8086_MODE              {0x01};
inline const uint8_t ALL_INTERRUPTS_ENABLED     {0x00};
inline const uint8_t END_OF_INTERRUPT           {0x20};

/** @brief Initialize Programmable Interrupt Controller (PIC).*/
void init(void) noexcept;

} // namespace pic
} // namespace x86
} // namespace arch
} // namespace kernel

#endif // _KERNEL_ARCH_X86_PIC_HPP_