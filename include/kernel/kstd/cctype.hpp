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
 * @file  cctype.hpp
 * @brief Declares basic character handling functions.
 *
 * @author Alexander Kuzin (<a href="https://github.com/alkuzin">alkuzin</a>)
 * @date   25.09.2024
 */

#ifndef _KERNEL_KSTD_CTYPE_HPP_
#define _KERNEL_KSTD_CTYPE_HPP_


namespace kernel {
namespace kstd {

/**
 * @brief Checks for a printable character (including space).
 *
 * @param [in] c - given character.
 * @return whether c is a printable character (including space) or not.
 */
template <typename T>
constexpr inline bool isprint(T c) noexcept
{
    return ((c == ' ') || ((c > 32) && (c < 127)));
}

/**
 * @brief Checks for an uppercase character.
 *
 * @param [in] c - given character.
 * @return whether c is an uppercase character or not.
 */
template <typename T>
constexpr inline bool isupper(T c) noexcept
{
    return ((c >= 'A') && (c <= 'Z'));
}

/**
 * @brief Checks for a digit.
 *
 * @param [in] c - given character.
 * @return whether c is a digit or not.
 */
template <typename T>
constexpr inline bool isdigit(T c) noexcept
{
    return ((c >= '0') && (c <= '9'));
}

template <typename T>
constexpr inline bool isspace(T ch)
{
    return (ch == ' ' || ch == '\t' || ch == '\n' ||
            ch == '\v' || ch == '\f' || ch == '\r');
}

/**
 * @brief Checks for an alphabetic character.
 *
 * @param [in] c - given character.
 * @return whether c is an alphabetic character or not.
 */
template <typename T>
constexpr inline bool isalpha(T c) noexcept
{
    return (((c >= 'A') && (c <= 'Z')) || ((c >= 'a') && (c <= 'z')));
}

/**
 * @brief Checks for an alphanumeric character.
 *
 * @param [in] c - given character.
 * @return whether c is an alphanumeric character or not.
 */
template <typename T>
constexpr inline bool isalnum(T c) noexcept
{
    return (isalpha(c) || isdigit(c));
}

} // namespace kstd
} // namespace kernel

#endif // _KERNEL_KSTD_CTYPE_HPP_