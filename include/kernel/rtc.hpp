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
 * @file  rtc.hpp
 * @brief Contains Real-Time Clock (RTC) handeling functions declarations.
 *
 * @author Alexander Kuzin (<a href="https://github.com/alkuzin">alkuzin</a>)
 * @date   28.10.2024
 */

#ifndef _KERNEL_RTC_HPP_
#define _KERNEL_RTC_HPP_

#include <kernel/types.hpp>


namespace kernel {
namespace rtc {

struct rtc_time_t {
    int32_t sec;    // seconds (0-59)
    int32_t min;    // minutes (0-59)
    int32_t hour;   // hours (0–23/1–12 (highest bit set if pm))
    int32_t mday;   // day of the month (1-31)
    int32_t mon;    // month (1-12)
    int32_t year;   // year since 1900
    int32_t wday;   // day of the week (1-7, sunday = 1)
};

/** @brief Initialize real-time clock.*/
void init(void) noexcept;

/**
 * @brief Read current time.
 *
 * @param [out] tm - given structure to store current time info.
 */
void read_time(rtc_time_t& tm) noexcept;

} // namespace rtc
} // namespace kernel

#endif // _KERNEL_RTC_HPP_