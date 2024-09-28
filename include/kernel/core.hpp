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
 * @file  core.hpp
 * @brief Contains kernel auxilar functions.
 *
 * @author Alexander Kuzin (<a href="https://github.com/alkuzin">alkuzin</a>)
 * @date   26.09.2024
 */

#ifndef _KERNEL_CORE_HPP_
#define _KERNEL_CORE_HPP_

#include <kernel/arch/x86/system.hpp>


namespace kernel {
namespace core {

/** @brief Halt CPU.*/
inline void khalt(void) noexcept
{
    for (;;) arch::x86::halt();
}

} // namespace core
} // namespace kernel

#endif // _KERNEL_CORE_HPP_