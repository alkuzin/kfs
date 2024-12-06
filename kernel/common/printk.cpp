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
#include <kernel/kstd/cctype.hpp>
#include <kernel/printk.hpp>
#include <kernel/ktime.hpp>


namespace kernel {

// inner buffer to store printk string
inline const auto BUF_SIZE {1_KB};
static char buffer[BUF_SIZE];

// kernel log types:
inline const auto LOG_OK           {0};
inline const auto LOG_ERR          {1};
inline const auto LOG_DEBUG        {2};
inline const auto LOG_EMERG        {3};
inline const auto LOG_INFO         {4};
inline const auto LOG_DEFAULT      {9};

// kernel log types messages:
inline const auto LOG_OK_MSG      {"  OK  "};
inline const auto LOG_ERR_MSG     {"ERROR"};
inline const auto LOG_EMERG_MSG   {"EMERG"};
inline const auto LOG_DEBUG_MSG   {"DEBUG"};

/**
 * @brief Print log info.
 *
 * @param [in] fmt - given format string.
 * @return buffer shift.
 */
static s32 print_log(const char *fmt)
{
    auto type = LOG_DEFAULT;

    // checking that there is an explicit log type '<N>', where N - log type
    if (fmt[0] == '<' && kstd::isdigit(fmt[1]) && fmt[2] == '>')
        type = fmt[1] - '0';

    if (type == LOG_DEFAULT)
        return 0;

    static char timestamp[32] {};
    ktime_t boot_time, cur_time, diff_time, sec, millisec;

    // print log type
    kstd::putchar('[');
    switch (type) {
    case LOG_OK:
        kstd::putk(LOG_OK_MSG, gfx::color::green);
        break;

    case LOG_ERR:
        kstd::putk(LOG_ERR_MSG, gfx::color::red);
        break;

    case LOG_DEBUG:
        kstd::putk(LOG_DEBUG_MSG, gfx::color::gray);
        break;

    case LOG_EMERG:
        kstd::putk(LOG_EMERG_MSG, gfx::color::yellow);
        break;

    case LOG_INFO:
        boot_time = ktime::get_boot_time();
        cur_time  = ktime::clock();
        diff_time = cur_time - boot_time;
        sec       = (diff_time / 1000) % 60;
        millisec  = diff_time % 1000;

        kstd::snprintk(timestamp, sizeof(timestamp), "%u.%u", sec, millisec);
        kstd::putk(timestamp, gfx::color::gray);
        break;

    default:
        break;
    }

    kstd::putchar(']');
    kstd::putchar(' ');

    return 3;
}

void printk(const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    auto shift = print_log(fmt);
    kstd::vsnprintk(buffer, BUF_SIZE, fmt + shift, args);
    va_end(args);

    kstd::putk(buffer);

    // clean buffer
    for (usize i = 0; buffer[i] && i < BUF_SIZE; i++)
        buffer[i] = 0;
}

void cprintk(rgb_t fg, rgb_t bg, const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    kstd::vsnprintk(buffer, BUF_SIZE, fmt, args);
    va_end(args);

    kstd::putk(buffer, fg, bg);

    // clean buffer
    for (usize i = 0; buffer[i] && i < BUF_SIZE; i++)
        buffer[i] = 0;
}

} // namespace kernel