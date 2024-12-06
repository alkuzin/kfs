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
 * @file  cstdlib.hpp
 * @brief Declares auxilar standard library functions.
 *
 * @author Alexander Kuzin (<a href="https://github.com/alkuzin">alkuzin</a>)
 * @date   04.11.2024
 */

#ifndef _KERNEL_KSTD_CSTDLIB_HPP_
#define _KERNEL_KSTD_CSTDLIB_HPP_

#include <kernel/kstd/cstring.hpp>
#include <kernel/kstd/cctype.hpp>
#include <kernel/panic.hpp>


namespace kernel {
namespace kstd {

/**
 * @brief Split string.
 *
 * @param [in] str - given string to split.
 * @param [in] delim - given string delimeters.
 * @param [out] count - given number of splitted parts.
 * @return splitted string.
 */
char **split(const char *str, const char *delim, int *count);

/**
 * @brief Free memory allocated for split().
 *
 * @param [in] result - given splitted string to free.
 */
void free_split(char **result);

/**
 * @brief Convert string to unsigned number.
 *
 * @param [in] str - given string to convert.
 * @return unsigned number representation of string.
 */
template <typename T>
T stou(const char *str)
{
    s32 index = 0;
    T result      = 0;
    auto size     = kstd::strlen(str);

    // skip leading whitespace
    while (index < size && kstd::isspace(str[index]))
        ++index;

    // check for empty string
    if (index == size) {
        panic(PANIC_ERR "%s\n", "empty string");
        return 0;
    }

    u32 digit {0};
    char ch {0};

    // parse the string
    while (index < size) {
        ch = str[index];

        if (!kstd::isdigit(ch)) {
            panic(PANIC_ERR "%s\n", "invalid character in input string");
            return 0;
        }

        // convert character to digit
        digit  = ch - '0';
        result = result * 10 + digit;
        ++index;
    }

    return result;
}

/**
 * @brief Convert from string to hex.
 *
 * @param [in] str - given string to convert.
 * @return hex representation of string.
 */
u32 stoh(const char *str);

} // namespace kstd
} // namespace kernel

#endif // _KERNEL_KSTD_CSTDLIB_HPP_