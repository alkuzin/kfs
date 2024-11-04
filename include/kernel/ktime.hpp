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

/**
 * @file  ktime.hpp
 * @brief Contains kernel time handeling functions declarations.
 *
 * @author Alexander Kuzin (<a href="https://github.com/alkuzin">alkuzin</a>)
 * @date   28.10.2024
 */

#ifndef _KERNEL_KTIME_HPP_
#define _KERNEL_KTIME_HPP_

#include <kernel/types.hpp>


namespace kernel {
namespace ktime {

inline const uint32_t UNIX_BASE_YEAR    {1900};
inline const uint32_t UNIX_EPOCH_YEAR   {1970};

enum class UTC : int32_t {
    ABI         = -12,  // UTC-12: Baker Island, USA
    NST         = -11,  // UTC-11: Pago Pago, American Samoa
    HST         = -10,  // UTC-10: Honolulu, Hawaii, USA
    AKST        = -9,   // UTC-9: Anchorage, Alaska, USA
    PST         = -8,   // UTC-8: Los Angeles, California, USA
    MST         = -7,   // UTC-7: Denver, Colorado, USA
    CST         = -6,   // UTC-6: Chicago, Illinois, USA
    EST         = -5,   // UTC-5: New York City, New York, USA
    VET         = -4,   // UTC-4: Caracas, Venezuela
    ART         = -3,   // UTC-3: Buenos Aires, Argentina
    GST         = -2,   // UTC-2: South Georgia
    AZOT        = -1,   // UTC-1: Azores, Portugal
    GMT         = 0,    // UTC: London, United Kingdom
    CET         = 1,    // UTC+1: Berlin, Germany
    EET         = 2,    // UTC+2: Athens, Greece
    MSK         = 3,    // UTC+3: Moscow, Russia
    GST_UAE     = 4,    // UTC+4: Dubai, UAE
    PKT         = 5,    // UTC+5: Islamabad, Pakistan
    ALMT        = 6,    // UTC+6: Almaty, Kazakhstan
    ICT         = 7,    // UTC+7: Bangkok, Thailand
    CST_CHINA   = 8,    // UTC+8: Beijing, China
    JST         = 9,    // UTC+9: Tokyo, Japan
    AEDT        = 10,   // UTC+10: Sydney, Australia
    SBT         = 11,   // UTC+11: Honiara, Solomon Islands
    FJT         = 12    // UTC+12: Suva, Fiji
};

struct tm {
    int32_t tm_sec;     // seconds (0-60)
    int32_t tm_min;     // minutes (0-59)
    int32_t tm_hour;    // hours (0-23)
    int32_t tm_mday;    // day of the month (1-31)
    int32_t tm_mon;     // month (0-11)
    int32_t tm_year;    // year since 1900
    int32_t tm_wday;    // day of the week (sunday = 0)
    int32_t tm_yday;    // day of the year (0-365; 1 Jan = 0)
    int32_t tm_isdst;   // daylight saving time flag
};

/**
 * @brief Get seconds since the Epoch corresponding to given time.
 *
 * @param [in] ptm - given POSIX time structure.
 * @return seconds since Epoch.
 */
ktime_t mktime(const tm& ptm) noexcept;

/**
 * @brief Converts seconds since the Epoch to a struct tm representation.
 *
 * @param [in] timer - given number of seconds since the Epoch to convert.
 * @param [out] result - given time struct tm to fill.
 */
void gmtime(ktime_t timer, tm& result) noexcept;

/**
 * @brief Get current date string representation.
 *
 * @return current date.
 */
char *get_date(void) noexcept;

/**
 * @brief Set the UTC offset.
 *
 * @param [in] offset - given UTC offset.
 */
void set_utc(UTC offset) noexcept;

/**
 * @brief Get the UTC offset.
 *
 * @return UTC offset.
 */
int32_t get_utc(void) noexcept;

/**
 * @brief Get number of milliseconds since boot.
 *
 * @return number of milliseconds since boot.
 */
ktime_t clock(void) noexcept;

void set_boot_time(void) noexcept;
ktime_t get_boot_time(void) noexcept;

} // namespace ktime
} // namespace kernel

#endif // _KERNEL_KTIME_HPP_