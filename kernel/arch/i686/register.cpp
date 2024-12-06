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

#include <kernel/arch/i686/register.hpp>


namespace kernel {
namespace arch {
namespace i686 {

// TODO: add debug function that prints all registers
u32 get_register(REG reg)
{
    u32 value = 0;

    switch (reg) {
    case REG::ESP:
        asm volatile("mov %%esp, %0" : "=r"(value));
        break;

    case REG::EBP:
        asm volatile("mov %%ebp, %0" : "=r"(value));
        break;

    case REG::EBX:
        asm volatile("mov %%ebx, %0" : "=r"(value));
        break;

    case REG::EDX:
        asm volatile("mov %%edx, %0" : "=r"(value));
        break;

    case REG::ECX:
        asm volatile("mov %%ecx, %0" : "=r"(value));
        break;

    case REG::EAX:
        asm volatile("mov %%eax, %0" : "=r"(value));
        break;

    case REG::EIP:
        asm volatile("call 1f \n\t1: pop %0" : "=r"(value));
        break;

    case REG::CS:
        asm volatile("mov %%cs, %0" : "=r"(value));
        break;

    case REG::EFLAGS:
        asm volatile("pushfl\n\tpop %0" : "=r"(value));
        break;

    case REG::SS:
        asm volatile("mov %%ss, %0" : "=r"(value));
        break;

    case REG::DS:
        asm volatile("mov %%ds, %0" : "=r"(value));
        break;

    case REG::EDI:
        asm volatile("mov %%edi, %0" : "=r"(value));
        break;

    case REG::ESI:
        asm volatile("mov %%esi, %0" : "=r"(value));
        break;

    case REG::CR0:
        asm volatile("mov %%cr0, %0" : "=r"(value));
        break;

    case REG::CR2:
        asm volatile("mov %%cr2, %0" : "=r"(value));
        break;

    case REG::CR3:
        asm volatile("mov %%cr3, %0" : "=r"(value));
        break;

    default:
        break;
    }

    return value;
}

void set_register(REG reg, u32 value)
{
    switch (reg) {
    case REG::ESP:
        asm volatile("mov %0, %%esp" : : "r"(value));
        break;

    case REG::EBP:
        asm volatile("mov %0, %%ebp" : : "r"(value));
        break;

    case REG::EBX:
        asm volatile("mov %0, %%ebx" : : "r"(value));
        break;

    case REG::EDX:
        asm volatile("mov %0, %%edx" : : "r"(value));
        break;

    case REG::ECX:
        asm volatile("mov %0, %%ecx" : : "r"(value));
        break;

    case REG::EAX:
        asm volatile("mov %0, %%eax" : : "r"(value));
        break;

    case REG::EIP:
        // it is better to use jump
        // TODO: handle this
        value = 0;
        break;

    case REG::CS:
        asm volatile("mov %0, %%cs" : : "r"(value));
        break;

    case REG::EFLAGS:
        asm volatile("push %0\n\tpopf" : : "r"(value));
        break;

    case REG::SS:
        asm volatile("mov %0, %%ss" : : "r"(value));
        break;

    case REG::DS:
        asm volatile("mov %0, %%ds" : : "r"(value));
        break;

    case REG::EDI:
        asm volatile("mov %0, %%edi" : : "r"(value));
        break;

    case REG::ESI:
        asm volatile("mov %0, %%esi" : : "r"(value));
        break;

    case REG::CR0:
        asm volatile("mov %0, %%cr0" : : "r"(value));
        break;

    case REG::CR2:
        asm volatile("mov %0, %%cr2" : : "r"(value));
        break;

    case REG::CR3:
        asm volatile("mov %0, %%cr3" : : "r"(value));
        break;

    default:
        break;
    }
}

} // namespace i686
} // namespace arch
} // namespace kernel