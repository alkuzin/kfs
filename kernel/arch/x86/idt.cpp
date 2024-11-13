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

#include <kernel/arch/x86/system.hpp>
#include <kernel/arch/x86/idt.hpp>
#include <kernel/arch/x86/irq.hpp>
#include <kernel/arch/x86/gdt.hpp>
#include <kernel/kstd/cstring.hpp>
#include <kernel/arch/x86/pic.hpp>
#include <kernel/panic.hpp>


namespace kernel {
namespace arch {
namespace x86 {
namespace idt {

inline const int32_t IDT_SIZE       {256};
inline const uint8_t INTERRUPT_GATE {0x8E};

entry_t IDT[IDT_SIZE];
ptr_t   idt_ptr;

/**
 * @brief Set the IDT gate.
 *
 * @param [in] num - given IDT number.
 * @param [in] offset - given entry point of the ISR.
 * @param [in] selector - given point to a valid code segment in GDT.
 * @param [in] flags - given gate type & other control bits.
 */
void set_gate(uint8_t num, uint32_t offset, uint16_t selector, uint8_t flags) noexcept
{
    IDT[num].offset_low  = (offset & 0xFFFF);
    IDT[num].selector    = selector;
    IDT[num].reserved    = 0;
    IDT[num].flags       = flags | 0x60;
    IDT[num].offset_high = static_cast<u16>((offset >> 0x10) & 0xFFFF);
}

/**
 * @brief Set the IDT gate.
 *
 * @param [in] num - given IDT number.
 * @param [in] offset - given entry point of the interrupt function.
 */
inline void set_int_function(uint8_t num, uint32_t offset) noexcept
{
    set_gate(num, offset, gdt::KERNEL_CODE_SEGMENT, INTERRUPT_GATE);
}

/** @brief Set IDT gates.*/
void set_gates(void) noexcept
{
    // set gates for ISR functions for hardware interrupts 0-31
    set_int_function(0,  phys_addr_t(isr0));
    set_int_function(1,  phys_addr_t(isr1));
    set_int_function(2,  phys_addr_t(isr2));
    set_int_function(3,  phys_addr_t(isr3));
    set_int_function(4,  phys_addr_t(isr4));
    set_int_function(5,  phys_addr_t(isr5));
    set_int_function(6,  phys_addr_t(isr6));
    set_int_function(7,  phys_addr_t(isr7));
    set_int_function(8,  phys_addr_t(isr8));
    set_int_function(9,  phys_addr_t(isr9));
    set_int_function(10, phys_addr_t(isr10));
    set_int_function(11, phys_addr_t(isr11));
    set_int_function(12, phys_addr_t(isr12));
    set_int_function(13, phys_addr_t(isr13));
    set_int_function(14, phys_addr_t(isr14));
    set_int_function(15, phys_addr_t(isr15));
    set_int_function(16, phys_addr_t(isr16));
    set_int_function(17, phys_addr_t(isr17));
    set_int_function(18, phys_addr_t(isr18));
    set_int_function(19, phys_addr_t(isr19));
    set_int_function(20, phys_addr_t(isr20));
    set_int_function(21, phys_addr_t(isr21));
    set_int_function(22, phys_addr_t(isr22));
    set_int_function(23, phys_addr_t(isr23));
    set_int_function(24, phys_addr_t(isr24));
    set_int_function(25, phys_addr_t(isr25));
    set_int_function(26, phys_addr_t(isr26));
    set_int_function(27, phys_addr_t(isr27));
    set_int_function(28, phys_addr_t(isr28));
    set_int_function(29, phys_addr_t(isr29));
    set_int_function(30, phys_addr_t(isr30));
    set_int_function(31, phys_addr_t(isr31));

    // set gates for IRQ functions for for system calls
    set_int_function(32, phys_addr_t(irq0));
    set_int_function(33, phys_addr_t(irq1));
    set_int_function(34, phys_addr_t(irq2));
    set_int_function(35, phys_addr_t(irq3));
    set_int_function(36, phys_addr_t(irq4));
    set_int_function(37, phys_addr_t(irq5));
    set_int_function(38, phys_addr_t(irq6));
    set_int_function(39, phys_addr_t(irq7));
    set_int_function(40, phys_addr_t(irq8));
    set_int_function(41, phys_addr_t(irq9));
    set_int_function(42, phys_addr_t(irq10));
    set_int_function(43, phys_addr_t(irq11));
    set_int_function(44, phys_addr_t(irq12));
    set_int_function(45, phys_addr_t(irq13));
    set_int_function(46, phys_addr_t(irq14));
    set_int_function(47, phys_addr_t(irq15));

    // set gates for ISR functions for hardware interrupts 0-15
    set_int_function(128, phys_addr_t(isr128));
    set_int_function(177, phys_addr_t(isr177));
}

/**
 * @brief Flush out the old IDT and install the new changes.
 *
 * @param [in] ptr - new IDT pointer to update.
 */
asmlinkage void idt_flush(phys_addr_t ptr);

void init(void) noexcept
{
    // set pointer structure to IDT
    idt_ptr.size   = sizeof(IDT) - 1;
    idt_ptr.offset = phys_addr_t(&IDT);
    kstd::memset(&IDT, 0, sizeof(IDT));

    pic::init();
    set_gates();

    // update IDT
    idt_flush(phys_addr_t(&idt_ptr));
}

} // namespace idt
} // namespace x86
} // namespace arch
} // namespace kernel