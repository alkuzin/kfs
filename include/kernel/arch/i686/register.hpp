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
 * @file  register.hpp
 * @brief Contains functions for managing i686 registers.
 *
 * @author Alexander Kuzin (<a href="https://github.com/alkuzin">alkuzin</a>)
 * @date   09.10.2024
 */

#ifndef _KERNEL_ARCH_I686_REGISTER_HPP_
#define _KERNEL_ARCH_I686_REGISTER_HPP_

#include <kernel/types.hpp>


namespace kernel {
namespace arch {
namespace i686 {

enum class REG {
    CR2,
    DS,
    EDI,
    ESI,
    EBP,
    ESP,
    EBX,
    EDX,
    ECX,
    EAX,
    EIP,
    CS,
    EFLAGS,
    SS,
    CR0
};

/**
 * @brief Get the specific register value.
 *
 * @param [in] reg - given register name.
 * @return register value.
 */
u32 get_register(REG reg) noexcept;

/**
 * @brief Set the specific register.
 *
 * @param [in] reg - given register name.
 * @param [in] value - given register value to set.
 */
void set_register(REG reg, u32 value) noexcept;

} // namespace i686
} // namespace arch
} // namespace kernel

#endif // _KERNEL_ARCH_I686_REGISTER_HPP_