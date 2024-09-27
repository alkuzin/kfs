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
#include <kernel/kstd/cstdio.hpp>
#include <kernel/kstd/cctype.hpp>
#include <kernel/shell/shell.hpp>
#include <kernel/printk.hpp>
#include <kernel/debug.hpp>


namespace kernel {

using namespace arch::x86;

void shell_t::set(void) noexcept
{
    kstd::memset(m_buffer, 0, SHELL_BUFFER_SIZE);
}

void shell_t::display_prompt(void) const noexcept
{
    kstd::putchar('$');
    kstd::putchar(' ');
}

void shell_t::get_line(void) noexcept
{
    uint32_t pos = 0;
    char     ch  = 0;

    do {
        ch = static_cast<char>(driver::keyboard.getchar());

        if (ch && ch != '\n') {
            // handle backspace character
            if (ch == '\b') {
                if (pos == 0)
                    continue;
                else {
                    pos--;
                    m_buffer[pos] = 0;
                }
            }

            kstd::putchar(ch);

            // add character to the kernel shell buffer
            if (pos < SHELL_BUFFER_SIZE && kstd::isprint(ch)) {
                m_buffer[pos] = ch;
                pos++;
            }
        }
    } while (ch != '\n');

    // truncate buffer
    m_buffer[pos] = 0;
    kstd::putchar('\n');
}

void shell_t::process(void) noexcept
{
    for (;;) {
        display_prompt();
        get_line();

        if (m_buffer[0])
            exec(m_buffer);

        kstd::memset(m_buffer, 0, SHELL_BUFFER_SIZE);
    }
}

void shell_t::exec(const char *cmd) const noexcept
{
    if (kstd::strncmp(cmd, "gdt", 3) == 0) {
        // while trying to get GDT info by using GDT pointer struct
        // that set at GDT_BASE <0x00000800> compiler show warnings
        // -Werror=array-bounds=. I know what I'm doing, so
        // I disabled it in this case:
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Warray-bounds"

        printk("mode: %u\n", arch::x86::mode());
        gdt::ptr_t *gdt_ptr = reinterpret_cast<gdt::ptr_t*>(gdt::GDT_BASE);

        printk("\nGDT descriptor: <%08p>\n", gdt_ptr);
        printk("offset:         <%08p>\n", gdt_ptr->m_offset);
        printk("size:             %u bytes\n", gdt_ptr->m_size);

        printk("\n%s\n", "kernel dump of GDT descriptor:");
        debug::kdump(gdt::GDT_BASE, 0);

        printk("\n%s\n", "kernel dump of Global Descriptor Table:");
        debug::kdump(gdt_ptr->m_offset, gdt_ptr->m_size);

        #pragma GCC diagnostic pop
    }
    else
        printk("sh: %s: command not found \n", cmd);
}

shell_t shell;

} // namespace kernel