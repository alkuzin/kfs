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

#include <kernel/drivers/keyboard.hpp>
#include <kernel/arch/x86/gdt.hpp>
#include <kernel/shell/shell.hpp>
#include <kernel/terminal.hpp>
#include <kernel/printk.hpp>
#include <kernel/core.hpp>


namespace kernel {
namespace core {

/**
 * @brief Setup kernel.
 *
 * Initializes kernel components such as TTY, GDT,
 * IDT, timer and etc.
 *
 * @param [in] magic - given magic number.
 * @param [in] mboot - given multiboot information structure.
 */
static void kboot(uint32_t magic, const multiboot_t& mboot) noexcept
{
    // set kernel subsystems
    driver::vesa.set(mboot);
    tty::terminal.set();
    printk(KERN_OK "%s\n", "initialized VESA driver");
    printk(KERN_OK "%s\n", "initialized kernel terminal");

    driver::keyboard.set();
    printk(KERN_OK "%s\n", "initialized PS/2 keyboard driver");

    shell.set();
    printk(KERN_OK "%s\n", "initialized kernel shell");

    if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        printk(KERN_ERR "%s", "invalid magic number: %#X\n", magic);
        return;
    }

    arch::x86::gdt::init();
    printk(KERN_OK "%s\n", "initialized GDT\n");

    shell.process();
}

extern "C" void kmain(kernel::uint32_t magic, const multiboot_t& mboot) noexcept
{
    kboot(magic, mboot);
    core::khalt();
}

} // namespace core
} // namespace kernel