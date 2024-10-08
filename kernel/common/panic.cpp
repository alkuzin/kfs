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

#include <kernel/kstd/cctype.hpp>
#include <kernel/printk.hpp>
#include <kernel/debug.hpp>
#include <kernel/panic.hpp>
#include <kernel/core.hpp>


namespace kernel {

// inner buffer to store printk string
inline const auto BUF_SIZE {1_KB};
static char buffer[BUF_SIZE];

// kernel panic types:
inline const auto P_FATAL       {0};   // fatal error  (stop kernel)
inline const auto P_ERR         {1};   // simple error (don't stop kernel)
inline const auto P_DEBUG       {2};   // like fatal error, but outputs memory dump
inline const auto P_DEFAULT     {9};   // like PANIC_FATAL


void panic(const char *fmt, ...) noexcept
{
    bool stop_cpu = true;
    bool mem_dump = true;
    auto type  = P_DEFAULT;
    auto shift = 3;

    // checking that there is an explicit log type '<N>', where N - log type
    if (fmt[0] == '<' && kstd::isdigit(fmt[1]) && fmt[2] == '>')
        type = fmt[1] - '0';

    switch (type) {
    case P_FATAL:
        stop_cpu = true;
        mem_dump = false;
        break;

    case P_ERR:
        stop_cpu = false;
        mem_dump = false;
        break;

    case P_DEBUG:
        stop_cpu = true;
        mem_dump = true;
        break;

    default:
        stop_cpu = true;
        mem_dump = false;
        shift = 0;
        break;
    }

    va_list args;

    va_start(args, fmt);
    kstd::vsnprintk(buffer, BUF_SIZE, fmt + shift, args);
    va_end(args);

    printk(KERN_EMERG "%s", "panic: ");
	kstd::putk(buffer);

    if (mem_dump) {
        auto esp = debug::esp();
        auto ebp = debug::ebp();
        printk(KERN_EMERG "ESP: <%08p>\n", esp);
        printk(KERN_EMERG "EBP: <%08p>\n", ebp);
        debug::kdump(esp-64, 128);
    }

    if (stop_cpu)
        core::khalt();

    // clean buffer
    for (size_t i = 0; buffer[i] && i < BUF_SIZE; i++)
        buffer[i] = 0;
}

} // namespace kernel