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

#include <kernel/arch/i386/system.hpp>
#include <kernel/arch/i386/irq.hpp>
#include <kernel/arch/i386/pic.hpp>
#include <kernel/arch/i386/io.hpp>
#include <kernel/panic.hpp>


namespace kernel {
namespace arch {
namespace i386 {
namespace irq {

inline const char *exception_msgs[]
{
    "division by zero",
    "debug",
    "non maskable interrupt",
    "breakpoint",
    "into detected overflow",
    "out of bounds",
    "invalid opcode",
    "no coprocessor",
    "double fault",
    "coprocessor segment overrun",
    "bad TSS",
    "segment not present",
    "stack fault",
    "general protection fault",
    "page fault",
    "unknown interrupt",
    "coprocessor fault",
    "alignment fault",
    "machine check",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
    "reserved"
};

// handlers that are designed to respond to hardware interrupts
static handler_t routines[16] = {nullptr};

void request(s32 irq, handler_t handler) noexcept
{
    routines[irq] = handler;
    sti();
}

void free(s32 irq) noexcept
{
    routines[irq] = 0;
}

asmlinkage void irq_handler(int_regs_t *regs) noexcept
{
    // IRQ handler processes the interrupt by calling the appropriate
    // handler function based on the interrupt number
    handler_t handler = routines[regs->int_no - 32];

    // handle interrupt if handler exists
    if(handler)
        handler(regs);

    // this tells the slave PIC that interrupt handling was finished
    if(regs->int_no >= 40)
        outb(pic::SLAVE_PIC_CMD, pic::END_OF_INTERRUPT);

    // this tells the master PIC that interrupt handling was finished
    outb(pic::MASTER_PIC_CMD, pic::END_OF_INTERRUPT);
}

asmlinkage void isr_handler(int_regs_t *regs) noexcept
{
    // handle exceptions
    if(regs->int_no < 32)
        panic(PANIC_DEBUG "%s\n", exception_msgs[regs->int_no]);
}

} // namespace irq
} // namespace i386
} // namespace arch
} // namespace kernel