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
#include <kernel/kstd/cctype.hpp>
#include <kernel/drivers/pit.hpp>
#include <kernel/printk.hpp>
#include <kernel/bitops.hpp>
#include <kernel/debug.hpp>
#include <kernel/panic.hpp>
#include <kernel/core.hpp>


namespace kernel {

struct eflag {
    u32         mask;
    const char *label;
};

// inner buffer to store printk string
inline const auto BUF_SIZE {1_KB};
static char buffer[BUF_SIZE];

// kernel panic types:
inline const auto P_FATAL       {0};   // fatal error (stop kernel)
inline const auto P_ERR         {1};   // simple error (don't stop kernel)
inline const auto P_DEFAULT     {9};   // fatal error (stop kernel)


void _panic(const char *file, const char *func, s32 line, const char *fmt, ...) noexcept
{
    bool stop_cpu = true;
    auto type  = P_DEFAULT;
    auto shift = 3;

    // checking that there is an explicit log type '<N>', where N - log type
    if (fmt[0] == '<' && kstd::isdigit(fmt[1]) && fmt[2] == '>')
        type = fmt[1] - '0';

    switch (type) {
    case P_FATAL:
        break;

    case P_ERR:
        stop_cpu = false;
        break;

    default:
        shift = 0;
        break;
    }

    va_list args;

    va_start(args, fmt);
    kstd::vsnprintk(buffer, BUF_SIZE, fmt + shift, args);
    va_end(args);

    printk(KERN_INFO "%s\n", "---");
    printk(KERN_INFO "Kernel panic - an error occured: %s", buffer);
    printk(KERN_INFO "File: \"%s\"\n", file);
    printk(KERN_INFO "Function: %s() at line %d\n", func, line);

    if (stop_cpu) {
        using namespace arch::i686;
        using namespace driver;

        u32 eax = get_register(REG::EAX);
        u32 ebx = get_register(REG::EBX);
        u32 ecx = get_register(REG::ECX);
        u32 edx = get_register(REG::EDX);

        u32 esi = get_register(REG::ESI);
        u32 edi = get_register(REG::EDI);
        u32 ebp = get_register(REG::EBP);
        u32 esp = get_register(REG::ESP);

        u32 eip    = get_register(REG::EIP);
        u32 eflags = get_register(REG::EFLAGS);

        printk(KERN_INFO "%s\n", "---");
        printk(KERN_INFO "EAX: %#08X EBX: %#08X ECX: %#08X EDX: %#08X\n", eax, ebx, ecx, edx);
        printk(KERN_INFO "ESI: %#08X EDI: %#08X EBP: %#08X ESP: %#08X\n", esi, edi, ebp, esp);
        printk(KERN_INFO "EIP: %#08X EFLAGS: %#08X [ ", eip, eflags);

        const auto EFLAGS_COUNT {20};

        eflag eflags_array[EFLAGS_COUNT] = {
            {0x00000001, "CF"  },   // Carry flag
            {0x00000004, "PF"  },   // Parity flag
            {0x00000010, "AF"  },   // Auxiliary flag
            {0x00000040, "ZF"  },   // Zero flag
            {0x00000080, "SF"  },   // Sign flag
            {0x00000100, "TF"  },   // Trap flag
            {0x00000200, "IF"  },   // Interrupt enable flag
            {0x00000400, "DF"  },   // Direction flag
            {0x00000800, "OF"  },   // Overflow flag
            {0x00003000, "IOPL"},   // I/O privilege level
            {0x00004000, "NT"  },   // Nested task flag
            {0x00008000, "MD"  },   // Mode flag
            {0x00010000, "RF"  },   // Resume flag
            {0x00020000, "VM"  },   // Virtual 8086 mode flag
            {0x00040000, "AC"  },   // Alignment check
            {0x00080000, "VIF" },   // Virtual interrupt flag
            {0x00100000, "VIP" },   // Virtual interrupt pending
            {0x00200000, "ID"  },   // Able to use CPUID instruction
            {0x40000000, "AES" },   // AES key schedule loaded flag
            {0x80000000, "AI"  }    // Alternate Instruction Set
        };

        for (usize i = 0; i < EFLAGS_COUNT; i++) {
            if (test_bit<u32>(eflags, eflags_array[i].mask)) {
                kstd::putk(eflags_array[i].label);
                kstd::putk(" ");
            }
        }

        kstd::putk("]\n");

        u32 cs  = get_register(REG::CS);
        u32 ds  = get_register(REG::DS);
        u32 ss  = get_register(REG::SS);

        u32 cr0 = get_register(REG::CR0);
        u32 cr2 = get_register(REG::CR2);
        u32 cr3 = get_register(REG::CR3);

        printk(KERN_INFO "CS:  %#08X DS:  %#08X SS:  %#08X\n", cs, ds, ss);
        printk(KERN_INFO "CR0: %#08X CR2: %#08X CR3: %#08X\n", cr0, cr2, cr3);
        printk(KERN_INFO "%s\n", "---");

        core::khalt();
    }

    // clean buffer
    for (usize i = 0; buffer[i] && i < BUF_SIZE; i++)
        buffer[i] = 0;
}

} // namespace kernel