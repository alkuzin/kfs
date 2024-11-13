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
 * @file  irq.hpp
 * @brief Provides definitions for Interrupt Requests (IRQ).
 *
 * @author Alexander Kuzin (<a href="https://github.com/alkuzin">alkuzin</a>)
 * @date   12.10.2024
 */

#ifndef _KERNEL_ARCH_X86_IRQ_HPP_
#define _KERNEL_ARCH_X86_IRQ_HPP_

#include <kernel/linkage.hpp>
#include <kernel/types.hpp>


namespace kernel {

enum IRQ : s32 {
    TIMER    = 0,
    KEYBOARD = 1,
    CLOCK    = 8
};

namespace arch {
namespace x86 {
namespace irq {

/** @brief Structure representing interrupt register state.*/
struct int_regs_t {
    u32 cr2;         // control register 2
    u32 ds;          // data segment
    u32 edi;         // destination index
    u32 esi;         // source index
    u32 ebp;         // base pointer
    u32 esp;         // stack pointer
    u32 ebx;         // base register
    u32 edx;         // data register
    u32 ecx;         // counter register
    u32 eax;         // accumulator register
    u32 int_no;      // interrupt number
    u32 err_code;    // error code
    u32 eip;         // instruction pointer
    u32 cs;          // code segment
    u32 eflags;      // flags register
    u32 useresp;     // user stack pointer
    u32 ss;          // stack segment
} __attribute__((packed));

// Interrupt requests handler function alias
using handler_t = void (*)(int_regs_t*);

/**
 * @brief Install handler for IRQ.
 *
 * @param [in] irq - given IRQ number.
 * @param [in] handler - given pointer to IRQ handler function.
 */
void request(s32 irq, irq::handler_t handler) noexcept;

/**
 * @brief Uninstall handler for IRQ.
 *
 * @param [in] irq - given IRQ number.
 */
void free(s32 irq) noexcept;

/**
 * @brief ISR handler function.
 *
 * @param [in] regs - given pointer to interrupt register state.
 */
asmlinkage void isr_handler(int_regs_t *regs) noexcept;

/**
 * @brief IRQ handler function.
 *
 * @param [in] regs - given pointer to interrupt register state.
 */
asmlinkage void irq_handler(int_regs_t *regs) noexcept;

} // namespace irq

// ISR functions for hardware interrupts 0-31
asmlinkage void isr0(void);
asmlinkage void isr1(void);
asmlinkage void isr2(void);
asmlinkage void isr3(void);
asmlinkage void isr4(void);
asmlinkage void isr5(void);
asmlinkage void isr6(void);
asmlinkage void isr7(void);
asmlinkage void isr8(void);
asmlinkage void isr9(void);
asmlinkage void isr10(void);
asmlinkage void isr11(void);
asmlinkage void isr12(void);
asmlinkage void isr13(void);
asmlinkage void isr14(void);
asmlinkage void isr15(void);
asmlinkage void isr16(void);
asmlinkage void isr17(void);
asmlinkage void isr18(void);
asmlinkage void isr19(void);
asmlinkage void isr20(void);
asmlinkage void isr21(void);
asmlinkage void isr22(void);
asmlinkage void isr23(void);
asmlinkage void isr24(void);
asmlinkage void isr25(void);
asmlinkage void isr26(void);
asmlinkage void isr27(void);
asmlinkage void isr28(void);
asmlinkage void isr29(void);
asmlinkage void isr30(void);
asmlinkage void isr31(void);

// ISR functions for system calls
asmlinkage void isr128(void);
asmlinkage void isr177(void);

// IRQ functions for hardware interrupts 0-15
asmlinkage void irq0(void);
asmlinkage void irq1(void);
asmlinkage void irq2(void);
asmlinkage void irq3(void);
asmlinkage void irq4(void);
asmlinkage void irq5(void);
asmlinkage void irq6(void);
asmlinkage void irq7(void);
asmlinkage void irq8(void);
asmlinkage void irq9(void);
asmlinkage void irq10(void);
asmlinkage void irq11(void);
asmlinkage void irq12(void);
asmlinkage void irq13(void);
asmlinkage void irq14(void);
asmlinkage void irq15(void);

} // namespace x86
} // namespace arch
} // namespace kernel

#endif // _KERNEL_ARCH_X86_IRQ_HPP_