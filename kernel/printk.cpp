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

#include <kernel/kstd/cstdio.hpp>
#include <kernel/kstd/ctypes.hpp>


namespace kernel {

static constexpr uint32_t BUF_SIZE {1024};
static char buffer[BUF_SIZE];


void printk(const char *fmt, ...) noexcept
{
    va_list args;

    va_start(args, fmt);
    kstd::vsnprintk(buffer, BUF_SIZE, fmt, args);
    va_end(args);

    uint32_t i = 0;

    while(buffer[i]) {
        kstd::putchar(buffer[i]);       
        i++;
	}
}

} // namespace kernel