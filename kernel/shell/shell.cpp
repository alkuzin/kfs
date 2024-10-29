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
#include <kernel/arch/x86/system.hpp>
#include <kernel/arch/x86/gdt.hpp>
#include <kernel/kstd/cstring.hpp>
#include <kernel/kstd/cctype.hpp>
#include <kernel/shell/shell.hpp>
#include <kernel/drivers/pit.hpp>
#include <kernel/config.hpp>
#include <kernel/debug.hpp>
#include <kernel/ktime.hpp>
#include <kernel/pmm.hpp>


namespace kernel {

void shell_t::init(void) noexcept
{
    kstd::memset(m_buffer, 0, SHELL_BUFFER_SIZE);
}

inline void shell_t::display_prompt(void) const noexcept
{
    kstd::putchar('$');
    kstd::putchar(' ');
}

void shell_t::process(void) noexcept
{
    for (;;) {
        display_prompt();
        driver::keyboard::get_line(m_buffer, SHELL_BUFFER_SIZE);

        if (m_buffer[0])
            exec(m_buffer);

        kstd::memset(m_buffer, 0, SHELL_BUFFER_SIZE);
    }
}

// TODO: move to shell builtins
void shell_t::exec(const char *cmd) const noexcept
{
    if (kstd::strncmp(cmd, "gdt", 3) == 0) {
        // while trying to get GDT info by using GDT pointer struct
        // that set at GDT_BASE <0x00000800> compiler show warnings
        // -Werror=array-bounds=. I know what I'm doing, so
        // I disabled it in this case:
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Warray-bounds"

        using namespace arch::x86;

        gdt::ptr_t *gdt_ptr = reinterpret_cast<gdt::ptr_t*>(gdt::GDT_BASE);

        printk("GDT descriptor: <%08p>\n", gdt_ptr);
        printk("offset:         <%08p>\n", gdt_ptr->m_offset);
        printk("size:             %u bytes\n", gdt_ptr->m_size);

        printk("\n%s\n", "kernel dump of GDT descriptor:");
        debug::kdump(gdt::GDT_BASE, 0);

        printk("\n%s\n", "kernel dump of Global Descriptor Table:");
        debug::kdump(gdt_ptr->m_offset, gdt_ptr->m_size);

        #pragma GCC diagnostic pop
    }
    else if (kstd::strncmp(cmd, "clear", 5) == 0)
        tty::clear();
    else if (kstd::strncmp(cmd, "uname", 5) == 0) {
        info::display_general();
        kstd::putchar('\n');
        info::display_build();
    }
    else if (kstd::strncmp(cmd, "lsmem", 5) == 0)
        core::memory::display_memory();
    else if (kstd::strncmp(cmd, "date", 4) == 0)
        printk("%s\n", ktime::get_date());
    else if (kstd::strncmp(cmd, "ticks", 5) == 0)
        printk("PIT ticks: %u\n", driver::pit::get_ticks());
    else
        printk("sh: %s: command not found \n", cmd);
}

shell_t shell;

} // namespace kernel