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

#include <kernel/arch/x86/pic.hpp>
#include <kernel/arch/x86/io.hpp>


namespace kernel {
namespace arch {
namespace x86 {
namespace pic {

void init(void) noexcept
{
    // send the initialization command to both the master and slave PICs
    outb(MASTER_PIC_CMD, PIC_INIT_CMD);
    outb(SLAVE_PIC_CMD,  PIC_INIT_CMD);

    // set the vector offsets for the master and slave PICs
    outb(MASTER_PIC_DATA, MASTER_PIC_VECTOR_OFFSET);
    outb(SLAVE_PIC_DATA,  SLAVE_PIC_VECTOR_OFFSET);

    // configure the master and slave PICs for cascade operation
    outb(MASTER_PIC_DATA, MASTER_PIC_CASCADE);
    outb(SLAVE_PIC_DATA,  SLAVE_PIC_CASCADE);

    // set both PICs to operate in 8086/8088 mode
    outb(MASTER_PIC_DATA, PIC_8086_MODE);
    outb(SLAVE_PIC_DATA,  PIC_8086_MODE);

    // enable all interrupts on both PICs
    outb(MASTER_PIC_DATA, ALL_INTERRUPTS_ENABLED);
    outb(SLAVE_PIC_DATA,  ALL_INTERRUPTS_ENABLED);
}

} // namespace pic
} // namespace x86
} // namespace arch
} // namespace kernel