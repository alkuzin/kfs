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
#include <kernel/arch/x86/cpu.hpp>
#include <kernel/kstd/cstring.hpp>
#include <kernel/kstd/cctype.hpp>
#include <kernel/shell/shell.hpp>
#include <kernel/drivers/pit.hpp>
#include <kernel/config.hpp>
#include <kernel/debug.hpp>
#include <kernel/ktime.hpp>
#include <kernel/pmm.hpp>


namespace kernel {
namespace shell {

inline const auto SHELL_BUFFER_SIZE {128};
static char shell_buffer[SHELL_BUFFER_SIZE];

/** @brief Display kernel shell prompt.*/
static inline void display_prompt(void) noexcept
{
    kstd::putchar('$');
    kstd::putchar(' ');
}

/**
 * @brief Execute command.
 *
 * @param [in] cmd - given command to execute.
 */
static void exec(const char *cmd) noexcept
{
    // TODO: move to shell builtins
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
        printk("offset:         <%08p>\n", gdt_ptr->offset);
        printk("size:             %u bytes\n", gdt_ptr->size);

        printk("\n%s\n", "kernel dump of GDT descriptor:");
        debug::kdump(gdt::GDT_BASE, 0);

        printk("\n%s\n", "kernel dump of Global Descriptor Table:");
        debug::kdump(gdt_ptr->offset, gdt_ptr->size);

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
    else if (kstd::strncmp(cmd, "lscpu", 5) == 0) {
        using namespace arch::x86;

        cpu::details_t details {};
        cpu::get_details(details);

        printk("Architecture:   %s\n", cpu::arch);
        printk("CPU op-mode(s): %s\n", cpu::get_op_modes());
        printk("Byte Order:     %s\n", cpu::byte_order);
        printk("Vendor ID:      %s\n", details.vendor);
        printk("CPU type:       %s\n", cpu::type_to_str(cpu::TYPE(details.type)));
        printk("Model name:     %s\n", details.brand);
        printk("CPU family:     %u\n", details.family);
        printk("CPU model:      %u\n", details.model);
    }
    else
        printk("sh: %s: command not found \n", cmd);
}

void init(void) noexcept
{
    kstd::memset(shell_buffer, 0, SHELL_BUFFER_SIZE);
}

// TODO: implement structures for shell builtins
static const char *builtins[7] {
    "clear",
    "uname",
    "date",
    "lscpu",
    "lsmem",
    "gdt",
    "ticks",
};

static const char *get_suitable_cmd(const char *str, int32_t len) noexcept
{
    // warning: for large number of shell commands it is better to use algorithm
    // based on the search tree (with search complexity O(log N)) in order to get
    // suitable command instead of this (with search complexity O(N))

    for (int32_t i = 0; i < 7; i++) {
        // TODO: handle few similar commands
        if (kstd::strncmp(builtins[i], str, len) == 0)
            return builtins[i];
    }

    return nullptr;
}

static bool tab_handler(void) noexcept
{
    int32_t len         = kstd::strlen(shell_buffer);
    const char *command = get_suitable_cmd(shell_buffer, len);

    if (command) {
        // updating shell buffer
        kstd::strncpy(shell_buffer, command, SHELL_BUFFER_SIZE);

        // clearing input
        for (int32_t i = 0; i < len; i++)
            kstd::putchar('\b');

        // printing full command
        kstd::putk(shell_buffer);
        return true;
    }

    return false;
}

void process(void) noexcept
{
    for (;;) {
        display_prompt();
        driver::keyboard::set_tab_handler(tab_handler);
        driver::keyboard::get_line(shell_buffer, SHELL_BUFFER_SIZE);

        if (shell_buffer[0])
            exec(shell_buffer);

        kstd::memset(shell_buffer, 0, SHELL_BUFFER_SIZE);
    }
}

} // namespace shell
} // namespace kernel