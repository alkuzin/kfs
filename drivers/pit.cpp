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

#include <kernel/arch/i686/irq.hpp>
#include <kernel/arch/i686/io.hpp>
#include <kernel/drivers/pit.hpp>
#include <kernel/kstd/cstdio.hpp>
#include <kernel/kernel.hpp>


namespace kernel {
namespace driver {
namespace pit {

using namespace arch::i686;

inline const u8  PIT_DATA_PORT_0    {0x40};     // channel 0 data port
inline const u8  PIT_RATE_GEN_MODE  {0x36};     // rate generator mode
inline const u8  PIT_MODE_REG       {0x43};     // mode/command register

static volatile u32 ticks {0};

/**
 * @brief Handle interrupts.
 *
 * @param [in] regs - given CPU state.
 */
static void timer_handler(irq::int_regs_t *regs)
{
    IGNORE_UNUSED(regs);
    ticks += 1;
}

void init(void)
{
    constexpr u32 divisor = PIT_FREQUENCY / FREQUENCY;

    irq::request(IRQ::TIMER, timer_handler);

    outb(PIT_MODE_REG, PIT_RATE_GEN_MODE);
    outb(PIT_DATA_PORT_0, divisor & 0xFF);
    outb(PIT_DATA_PORT_0, divisor >> 8);
}

u32 get_ticks(void)
{
    return ticks;
}

void delay(u32 msec)
{
    u32 target_ticks = get_ticks() + (msec / MILLISEC_PER_TICK);

    // waiting until the target tick count is reached
    while (get_ticks() < target_ticks)
        asm volatile ("nop");   // no operation
}

} // namespace pit
} // namespace driver
} // namespace kernel