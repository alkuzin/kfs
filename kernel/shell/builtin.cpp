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

#include <kernel/shell/builtin.hpp>
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

inline const uint8_t BUILTINS_COUNT {8};

static void help(int32_t argc, char **argv) noexcept;
static void clear(int32_t argc, char **argv) noexcept;
static void uname(int32_t argc, char **argv) noexcept;
static void date(int32_t argc, char **argv) noexcept;
static void lscpu(int32_t argc, char **argv) noexcept;
static void lsmem(int32_t argc, char **argv) noexcept;
static void gdt(int32_t argc, char **argv) noexcept;
static void ticks(int32_t argc, char **argv) noexcept;

static builtin_t builtins[BUILTINS_COUNT] {
    {"help",  "show list of available commands", nullptr, 0, help},
    {"clear", "clear screen", nullptr, 0, clear},
    {"uname", "print system information", nullptr, 0, uname},
    {"date",  "print current date", nullptr, 0, date},
    {"lscpu", "display information about the CPU", nullptr, 0, lscpu},
    {"lsmem", "list the ranges of available memory", nullptr, 0, lsmem},
    {"gdt",   "display information about GDT", nullptr, 0, gdt},
    {"ticks", "display current number of PIT ticks", nullptr, 0, ticks},
};

void exec(const char *cmd) noexcept
{
    int32_t len = kstd::strlen(cmd);

    for (int32_t i = 0; i < BUILTINS_COUNT; i++) {
        if (kstd::strncmp(builtins[i].name, cmd, len) == 0) {
            // TODO: handle shell arguments
            builtins[i].func(0, nullptr);
            return;
        }
    }

    printk("sh: %s: command not found \n", cmd);
}

const char *get_suitable_cmd(const char *str, int32_t len) noexcept
{
    // warning: for large number of shell commands it is better to use
    // algorithm based on the search tree (with search complexity O(log N))
    // in order to get suitable command instead of this
    // (with search complexity O(N))

    for (int32_t i = 0; i < BUILTINS_COUNT; i++) {
        // TODO: handle few similar commands
        if (kstd::strncmp(builtins[i].name, str, len) == 0)
            return builtins[i].name;
    }

    return nullptr;
}

// Builtins -------------------------------------------------------------------

static void help(int32_t argc, char **argv) noexcept
{
    (void)argc; (void)argv; // unused

    builtin_t *cmd {nullptr};
    int32_t    len {0};

    for (uint8_t i = 0; i < BUILTINS_COUNT; i++) {
        cmd = &builtins[i];
        len = kstd::strlen(cmd->name);

        kstd::putk(builtins[i].name);

        for (int32_t i = 0; i < 16 - len; i++)
            kstd::putchar(' ');

        kstd::putk(builtins[i].descr);
        kstd::putchar('\n');
    }
}

static void clear(int32_t argc, char **argv) noexcept
{
    (void)argc; (void)argv; // unused

    tty::clear();
}

static void uname(int32_t argc, char **argv) noexcept
{
    (void)argc; (void)argv; // unused

    info::display_general();
    kstd::putchar('\n');
    info::display_build();
}

static void date(int32_t argc, char **argv) noexcept
{
    (void)argc; (void)argv; // unused

    printk("%s\n", ktime::get_date());
}

static void lscpu(int32_t argc, char **argv) noexcept
{
    (void)argc; (void)argv; // unused

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

static void lsmem(int32_t argc, char **argv) noexcept
{
    (void)argc; (void)argv; // unused

    core::memory::display_memory();
}

static void gdt(int32_t argc, char **argv) noexcept
{
    (void)argc; (void)argv; // unused

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

static void ticks(int32_t argc, char **argv) noexcept
{
    (void)argc; (void)argv; // unused
    printk("PIT ticks: %u\n", driver::pit::get_ticks());
}

} // namespace shell
} // namespace kernel