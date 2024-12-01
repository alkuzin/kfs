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
#include <kernel/arch/i686/gdt.hpp>
#include <kernel/arch/i686/idt.hpp>
#include <kernel/drivers/pit.hpp>
#include <kernel/shell/shell.hpp>
#include <kernel/terminal.hpp>
#include <kernel/linkage.hpp>
#include <kernel/printk.hpp>
#include <kernel/panic.hpp>
#include <kernel/ktime.hpp>
#include <kernel/core.hpp>
#include <kernel/slab.hpp>
#include <kernel/pmm.hpp>
#include <kernel/vmm.hpp>
#include <kernel/rtc.hpp>


namespace kernel {
namespace core {

/**
 * @brief Initializes kernel components.
 *
 * @param [in] magic - given magic number.
 * @param [in] mboot - given multiboot information structure.
 */
static void kboot(u32 magic, const multiboot_t& mboot) noexcept
{
    using namespace driver;

    // set kernel subsystems
    vesa::init(mboot);
    tty::terminal.init();
    printk(KERN_OK "%s\n", "initialized VESA driver");
    printk(KERN_OK "%s\n", "initialized kernel terminal");

    if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
        panic("invalid magic number: %#X\n", magic);

    printk(KERN_INFO "Multiboot structure: <%08p>\n", &mboot);
    printk(KERN_INFO "Multiboot magic number: %#08X\n", magic);

    arch::i686::gdt::init();
    printk(KERN_OK "%s\n", "initialized GDT");

    arch::i686::idt::init();
    printk(KERN_OK "%s\n", "initialized IDT");

    pit::init();
    printk(KERN_OK "%s\n", "initialized PIT driver");

    rtc::init();
    ktime::set_utc(ktime::UTC::MSK);
    ktime::set_boot_time();
    printk(KERN_OK "%s\n", "initialized RTC driver");

    core::memory::init(mboot);
    printk(KERN_OK "%s\n", "initialized physical memory manager");

    core::memory::vmm::init();
    printk(KERN_OK "%s\n", "initialized virtual memory manager");

    kmem::init();
    printk(KERN_OK "%s\n", "initialized kernel heap");

    keyboard::init();
    printk(KERN_OK "%s\n", "initialized PS/2 keyboard driver");

    shell::init();
    printk(KERN_OK "%s\n", "initialized kernel shell");
    printk(KERN_INFO "%s\n", "initialized kernel shell");
    shell::process();
}

asmlinkage void kmain(u32 magic, const multiboot_t& mboot) noexcept
{
    kboot(magic, mboot);
    khalt();
}

} // namespace core
} // namespace kernel