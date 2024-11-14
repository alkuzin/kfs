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

#include <kernel/arch/i386/system.hpp>
#include <kernel/arch/i386/irq.hpp>
#include <kernel/arch/i386/io.hpp>
#include <kernel/rtc.hpp>


namespace kernel {
namespace rtc {

inline const u8 RTC_PORT_INDEX     {0x70}; // RTC index port
inline const u8 RTC_PORT_DATA      {0x71}; // RTC data port
inline const u8 RTC_REGISTER_A     {0x8A}; // RTC status register A
inline const u8 RTC_REGISTER_B     {0x8B}; // RTC status register B
inline const u8 RTC_VALUE          {0x20}; // value to write to register A

enum class REG : u8 {
    SEC     = 0x00, // seconds       (0–59)
    MIN     = 0x02, // minutes       (0–59)
    HOUR    = 0x04, // hours         (0–23/1–12 (highest bit set if pm))
    WDAY    = 0x06, // weekday       (1–7, sunday = 1)
    MDAY    = 0x07, // day of month  (1–31)
    MON     = 0x08, // month         (1–12)
    YEAR    = 0x09, // year          (0–99)
    CENTURY = 0x32  // century       (19–20)
};

using namespace arch::i386;

/**
 * @brief Handle interrupts.
 *
 * @param [in] regs - given CPU state.
 */
static void clock_handler(irq::int_regs_t *regs) noexcept
{
    // Turn on the periodic interrupt with the default 1024 Hz rate
    (void)regs;                             // unused
    cli();                                  // disable interrupts
    outb(RTC_PORT_INDEX, RTC_REGISTER_B);   // disable non-maskable-interrupt
    u8 prev = inb(RTC_PORT_DATA);      // read the value of register B
    outb(RTC_PORT_INDEX, RTC_REGISTER_B);   // set index again
    outb(RTC_PORT_DATA, (prev | 0x40));     // turn on bit 6 of register B
    sti();                                  // enable interrupts
}

void init(void) noexcept
{
    cli();                                  // disable interrupts
    outb(RTC_PORT_INDEX, RTC_REGISTER_A);   // disable non-maskable-interrupt
    outb(RTC_PORT_DATA,  RTC_VALUE);        // write to CMOS/RTC RAM
    sti();                                  // enable interrupts

    irq::request(IRQ::CLOCK, &clock_handler);
}

/**
 * @brief Convert value from Binary-Coded Decimal to human-readable format.
 *
 * @param [in] bcd - given value in BCD format.
 * @return human-readable format.
 */
static inline s32 from_bcd(u8 bcd) noexcept
{
    return ((bcd & 0xF0) >> 1) + ((bcd & 0xF0) >> 3) + (bcd & 0xf);
}

/**
 * @brief Get the RTC register value.
 *
 * @param [in] reg - given RTC register.
 * @return RTC date value human-readable format.
 */
static inline u8 get_register(REG reg) noexcept
{
    outb(RTC_PORT_INDEX, static_cast<u32>(reg));
    return static_cast<u8>(from_bcd(inb(RTC_PORT_DATA)));
}

/** @brief Set RTC time structure.*/
void read_time(rtc_time_t& tm) noexcept
{
    tm.sec  = get_register(REG::SEC);
    tm.min  = get_register(REG::MIN);
    tm.hour = get_register(REG::HOUR);
    tm.mday = get_register(REG::MDAY);
    tm.mon  = get_register(REG::MON);
    tm.year = get_register(REG::YEAR) + 100;    // get number of years since 1900
    tm.wday = get_register(REG::WDAY);
}

} // namespace rtc
} // namespace kernel