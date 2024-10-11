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

#include <kernel/arch/x86/register.hpp>


namespace kernel {
namespace arch {
namespace x86 {

// TODO: add all registers
// TODO: add debug function that prints all registers
uint32_t get_register(enum REG reg) noexcept
{
    uint32_t value = 0;

    switch (reg) {

    case REG::ESP:
        __asm__ volatile ("mov %%esp, %0" : "=r"(value));
        break;

    case REG::EBP:
        __asm__ volatile ("mov %%ebp, %0" : "=r"(value));
        break;

    case REG::CR0:
        __asm__ volatile ("mov %%cr0, %0" : "=r"(value));
        break;

    default:
        break;
    }

    return value;
}

} // namespace x86
} // namespace arch
} // namespace kernel