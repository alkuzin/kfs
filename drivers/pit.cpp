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

#include <kernel/arch/x86/irq.hpp>
#include <kernel/arch/x86/io.hpp>
#include <kernel/drivers/pit.hpp>
#include <kernel/kstd/cstdio.hpp>


namespace kernel {
namespace driver {
namespace pit {

using namespace arch::x86;

inline const uint8_t  PIT_DATA_PORT_0    {0x40};     // channel 0 data port
inline const uint8_t  PIT_RATE_GEN_MODE  {0x36};     // rate generator mode
inline const uint8_t  PIT_MODE_REG       {0x43};     // mode/command register

static volatile uint32_t ticks {0};

/**
 * @brief Handle interrupts.
 *
 * @param [in] regs - given CPU state.
 */
static void timer_handler(irq::int_regs_t *regs) noexcept
{
    (void)regs; // unused
    ticks += 1;
}

void init(void) noexcept
{
    constexpr uint32_t divisor = PIT_FREQUENCY / FREQUENCY;

    irq::request(IRQ::TIMER, timer_handler);

    outb(PIT_MODE_REG, PIT_RATE_GEN_MODE);
    outb(PIT_DATA_PORT_0, divisor & 0xFF);
    outb(PIT_DATA_PORT_0, divisor >> 8);
}

uint32_t get_ticks(void) noexcept
{
    return ticks;
}

void delay(uint32_t msec) noexcept
{
    uint32_t target_ticks = get_ticks() + (msec / MILLISEC_PER_TICK);

    // waiting until the target tick count is reached
    while (get_ticks() < target_ticks)
        __asm__ volatile ("nop");   // no operation
}

} // namespace pit
} // namespace driver
} // namespace kernel