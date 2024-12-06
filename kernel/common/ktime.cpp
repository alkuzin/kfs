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
#include <kernel/drivers/pit.hpp>
#include <kernel/panic.hpp>
#include <kernel/ktime.hpp>
#include <kernel/rtc.hpp>


namespace kernel {
namespace ktime {

inline const s32 SECONDS_PER_MINUTE    {60};
inline const s32 SECONDS_PER_HOUR      {3600};
inline const s32 SECONDS_PER_DAY       {86400};
inline const s32 DAYS_PER_YEAR         {365};
inline const s32 DAYS_PER_LEAP_YEAR    {366};
inline const s32 MONTHS_PER_YEAR       {12};

static const char *months[12] {
    "Jan", "Feb", "Mar", "Apr",
    "May", "Jun", "Jul", "Aug",
    "Sep", "Oct", "Nov", "Dec"
};

static const char *wdays[7] {
    "Sun", "Mon", "Tue", "Wed",
    "Thr", "Fri", "Sat"
};

static const char *utc_offsets[25] {
    "ABI", "NST", "HST", "AKST", "PST", "MST", "CST", "EST",
    "VET", "ART", "GST", "AZOT", "GMT", "CET", "EET", "MSK",
    "GST_UAE", "PKT", "ALMT", "ICT", "CST_CHINA", "JST",
    "AEDT", "SBT","FJT"
};

static char timebuf[32];
static s32 utc {0};

static ktime_t boot_time {0};

/**
 * @brief Get the UTC offset string representation.
 *
 * @param [in] offset - given UTC offset.
 * @return UTC offset string representation.
 */
static const char *get_utc_name(UTC offset)
{
    auto index = static_cast<s32>(offset) + 12;

    if (index >= 0 && index < 25)
        return utc_offsets[index];
    else
        return "unknown UTC";
}

/**
 * @brief Convert RTC time to POSIX time.
 *
 * @param [in] rtm - given real-time clock time structure.
 * @param [out] ptm - given POSIX time structure.
 */
static void rtc_to_posix(const rtc::rtc_time_t& rtm, tm& ptm)
{
    // adjusting the hour: if the highest bit is set, it's PM
    if (rtm.hour & 0x80)
        ptm.tm_hour = (rtm.hour & 0x7F) + 12;   // convert to 24-hour format
    else
        ptm.tm_hour = rtm.hour & 0x7F;          // just take the lower 7 bits

    ptm.tm_sec   = rtm.sec;
    ptm.tm_min   = rtm.min;
    ptm.tm_mday  = rtm.mday;
    ptm.tm_year  = rtm.year;
    ptm.tm_mon   = rtm.mon - 1;     // convert from 1-12 to 0-11
    ptm.tm_wday  = (rtm.wday % 7);  // convert from 1-7 to 0-6 (sunday = 0)
    ptm.tm_yday  = 0;               // This can be calculated if needed
    ptm.tm_isdst = -1;              // information is not available

    // converting local time to seconds since epoch
    ktime_t local_time = mktime(ptm);

    // adjusting for the UTC offset (in hours)
    local_time += utc * SECONDS_PER_HOUR; // Convert hours to seconds

    // Convert back to struct tm
    tm tm_utc {};
    gmtime(local_time, tm_utc);
    ptm = tm_utc;
}

inline const char *date_fmt {"%s %s %d %02d:%02d:%02d %s %d"};

char *get_date(void)
{
    rtc::rtc_time_t rtm;
    tm t;

    rtc::read_time(rtm);
    rtc_to_posix(rtm, t);
    kstd::memset(timebuf, 0, sizeof(timebuf));

    kstd::snprintk(timebuf, sizeof(timebuf), date_fmt,
        wdays[t.tm_wday], months[t.tm_mon], t.tm_mday,
        t.tm_hour, t.tm_min, t.tm_sec,
        get_utc_name(UTC(utc)), t.tm_year + UNIX_BASE_YEAR
    );

    return timebuf;
}

/**
 * @brief Check if a year is a leap year.
 *
 * @param [in] year - given year to check.
 * @return true - if year is leap.
 * @return false - otherwise.
 */
static inline bool is_leap_year(s32 year)
{
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

/**
 * @brief Calculate the number of days in a month.
 *
 * @param [in] year - given year.
 * @param [in] month - given month.
 * @return number of days in a month.
 */
static s32 days_in_month(s32 year, s32 month)
{
    // handle February
    if (month == 1)
        return is_leap_year(year) ? 29 : 28;

    // months with 30 days
    if (month == 3 || month == 5 || month == 8 || month == 10)
        return 30;

    // months with 31 days
    return 31;
}

/**
 * @brief Get number of days in specific year.
 *
 * @param [in] year - given year.
 * @return number of days.
 */
static inline s32 days_in_year(s32 year)
{
    return is_leap_year(year) ? DAYS_PER_LEAP_YEAR : DAYS_PER_YEAR;
}

ktime_t mktime(const tm& ptm)
{
    if (ptm.tm_year < 70)
        panic("%s\n", "year must be >= 1970");

    // normalizing the time structure
    s32 year    = ptm.tm_year + UNIX_BASE_YEAR;
    s32 month   = ptm.tm_mon;
    s32 day     = ptm.tm_mday;
    s32 hour    = ptm.tm_hour;
    s32 minute  = ptm.tm_min;
    s32 second  = ptm.tm_sec;

    u32 total_days = 0;

    // adding days for the complete years
    for (int y = UNIX_EPOCH_YEAR; y < year; ++y)
        total_days += days_in_year(y);

    // adding days for the complete months of the current year
    for (int m = 0; m < month; ++m)
        total_days += days_in_month(year, m);

    // adding the days of the current month
    total_days += (day);

    // calculate total seconds
    ktime_t total_seconds = total_days * SECONDS_PER_DAY;
    total_seconds += hour * SECONDS_PER_HOUR;
    total_seconds += minute * SECONDS_PER_MINUTE;
    total_seconds += second;

    return total_seconds;
}

void gmtime(ktime_t timer, tm& result)
{
    u32 seconds          = timer;
    s32 total_days        = seconds / SECONDS_PER_DAY;
    s32 remaining_seconds = seconds % SECONDS_PER_DAY;

    result.tm_hour    = remaining_seconds / SECONDS_PER_HOUR;
    remaining_seconds %= SECONDS_PER_HOUR;
    result.tm_min     = remaining_seconds / SECONDS_PER_MINUTE;
    result.tm_sec     = remaining_seconds % SECONDS_PER_MINUTE;

    // calculating the current year
    result.tm_year = UNIX_EPOCH_YEAR;
    s32 days   = 0;

    for (;;) {
        days = days_in_year(result.tm_year);

        if (total_days < days)
            break;

        total_days -= days;
        result.tm_year++;
    }

    // calculating the current month
    result.tm_mon = 0;
    days          = 0;

    for (result.tm_mon = 0; result.tm_mon < MONTHS_PER_YEAR; result.tm_mon++) {
        days = days_in_month(result.tm_year, result.tm_mon);

        if (total_days < days)
            break;

        total_days -= days;
    }

    result.tm_mday = total_days;
    result.tm_year -= UNIX_BASE_YEAR; // adjusting year and month for struct tm

    // Zeller's Congruence algorithm to calculate day of the week
    s32 century         = result.tm_year / 100;
    s32 year_of_century = result.tm_year % 100;

    auto part1       = (year_of_century + (year_of_century / 4));
    auto part2       = ((century / 4) - (2 * century));
    auto part3       = ((26 * (result.tm_mon + 1)) + result.tm_mday + 2);
    auto day_of_week = (part1 + part2 + part3) % 7;

    // adjusting for Sunday being index 0
    day_of_week = (day_of_week == 6) ? 0 : (day_of_week - 1);

    // adjusting week day for UNIX Epoch that started at
    // the 1st January of 1970 (Thursday (4)):
    result.tm_wday = (day_of_week + 4) % 7;
}

void set_utc(UTC offset)
{
    utc = static_cast<s32>(offset);
}

s32 get_utc(void)
{
    return utc;
}

ktime_t clock(void)
{
    return driver::pit::get_ticks() * driver::pit::MILLISEC_PER_TICK;
}

void set_boot_time(void)
{
    boot_time = clock();
}

ktime_t get_boot_time(void)
{
    return boot_time;
}

} // namespace ktime

} // namespace kernel