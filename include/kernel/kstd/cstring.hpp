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
 * @file  cstring.hpp
 * @brief Defines several strings and memory management functions.
 *
 * @details This header file provides functions for manipulating strings,
 * such as copying, concatenating and comparing.
 *
 * @author Alexander Kuzin (<a href="https://github.com/alkuzin">alkuzin</a>)
 * @date   25.09.2024
 */

#ifndef _KERNEL_KSTD_CSTRING_HPP_
#define _KERNEL_KSTD_CSTRING_HPP_

#include <kernel/types.hpp>


namespace kernel {
namespace kstd {

/**
 * @brief Fills the first n bytes of the memory of the area pointed to by s
 * with the constant byte c.
 *
 * @param [out] s - given buffer pointer.
 * @param [in] c - given byte for filling buffer.
 * @param [in] n - given number of buffer bytes to fill.
 * @return filled buffer pointer.
 */
void *memset(void *s, s32 c, usize n);

/**
 * @brief Copies n bytes from memory area src to memory area dest.
 *
 * @param [out] dest - given destination buffer.
 * @param [in] src - given source buffer.
 * @param [in] n - given number of bytes to copy.
 * @return destination buffer pointer.
 */
void *memcpy(void *dest, const void *src, usize n);

/**
 * @brief Compares the two strings s1 and s2.
 *
 * @param [in] s1 - first given null terminated string.
 * @param [in] s2 - second given null terminated string.
 * @param [in] n - given number of symbols for comparison.
 * @return 0, if s1 and s2 are equal;
 * @return a negative value if s1 is less than s2;
 * @return a positive value if s1 is greater than s2.
 */
s32 strncmp(const char *s1, const char *s2, usize n);

/**
 * @brief Copy a string with truncation.
 *
 * @param [out] dest - given buffer for copied string.
 * @param [in] src - given source null terminated string.
 * @param [in] size - given size to copy.
 * @return number of copied string characters.
 */
usize strncpy(char *dest, const char *src, usize size);

/**
 * @brief Get string length.
 *
 * @param [in] str - given null terminated string.
 * @return str length.
 */
s32 strlen(const char *str);

/**
 * @brief Get string duplicate.
 *
 * @param [in] str - given string to duplicate.
 * @return pointer to a duplicate string in case of success.
 * @return nullptr in case of failure.
 */
char *strdup(const char* str);

/**
 * @brief Breaks a string into a sequence of zero or more nonempty tokens.
 *
 * On the first call to strtok(), the string to be parsed should be
 * specified in str. In each subsequent call that should parse
 * the same string, str must be nullptr.
 *
 * @param [in] str - given string to parse.
 * @param [in] delim - given set of the tockens delimiters in the string.
 * @return pointer to the next token.
 * @return nullptr if there are no more tokens.
 */
char *strtok(char *str, const char *delim);

/**
 * @brief Search a string for any of a set of bytes.
 *
 * @param [in] str - given string.
 * @param [in] accept - given string to find.
 * @return pointer to the byte in s that matches one of the bytes in accept.
 * @return nullptr if no such byte is found.
 */
char *strpbrk(const char *str, const char *accept);

} // namespace kstd
} // namespace kernel

#endif // _KERNEL_KSTD_CSTRING_HPP_