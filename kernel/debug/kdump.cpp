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

#include <kernel/kstd/cstring.hpp>
#include <kernel/kstd/cstdio.hpp>
#include <kernel/printk.hpp>
#include <kernel/debug.hpp>


namespace kernel {
namespace debug {

/**
 * @brief Convert byte to ASCII.
 *
 * @param [in] ch - given byte to convert.
 * @return character to print.
 */
static inline char to_print(uint8_t ch)
{
    return (ch > 31 && ch < 127)? ch : '.';
}

inline const auto BYTES_PER_LINE    {16};
inline const auto FG_COLOR          {gfx::color::gray};
inline const auto PTR_COLOR         {gfx::color::green};
inline const auto BG_COLOR          {tty::terminal.m_bg};


void kdump(phys_addr_t addr, size_t size) noexcept
{
    char    line[BYTES_PER_LINE + 1];
    uint8_t bytes[BYTES_PER_LINE];

    uint8_t *stack_ptr = reinterpret_cast<uint8_t*>(addr);
    uint8_t byte       = 0;
    size_t  byte_pos   = 0;
    size_t  rows       = 0;

    for (size_t k = 0; k < (size / BYTES_PER_LINE) + 1; k++) {
        kstd::memset(line, ' ', BYTES_PER_LINE);
        kstd::memset(bytes, ' ', BYTES_PER_LINE);

        for (size_t i = 0; i < BYTES_PER_LINE; i++) {
            byte     = stack_ptr[byte_pos];
            line[i]  = to_print(byte);
            bytes[i] = byte;
            byte_pos++;
        }

        cprintk(FG_COLOR, BG_COLOR, "%08x  ", rows);
        kstd::putchar('<');
        cprintk(PTR_COLOR, BG_COLOR, "%08p", stack_ptr + k * 0x10);
        printk("%s", ">  ");

        // print first half of 8 bytes in hexadecimal format
        for (size_t i = 0; i < BYTES_PER_LINE >> 1; i++) {
            if (bytes[i] == 0x00)
                cprintk(FG_COLOR, BG_COLOR, "%02x ", bytes[i]);
            else
                printk("%02x ", bytes[i]);
        }

        kstd::putchar(' ');

        // print second half of 8 bytes in hexadecimal format
        for (size_t i = BYTES_PER_LINE >> 1; i < BYTES_PER_LINE; i++) {
            if (bytes[i] == 0x00)
                cprintk(FG_COLOR, BG_COLOR, "%02x ", bytes[i]);
            else
                printk("%02x ", bytes[i]);
        }

        // print string representation of bytes
        printk("%s", "  |");
        for (const auto& ch : line) {
            if (ch == '.')
                tty::terminal.putc('.', FG_COLOR, BG_COLOR);
            else
                kstd::putchar(ch);
        }
        kstd::putchar('|');
        kstd::putchar('\n');

        rows += 0x10;
    }
}

} // namespace debug
} // namespace kernel