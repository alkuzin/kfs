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
#include <kernel/slab.hpp>


namespace kernel {
namespace kstd {

void *memset(void *s, int32_t c, size_t n) noexcept
{
    uint8_t *src = static_cast<uint8_t*>(s);
    uint8_t cc   = static_cast<uint8_t>(c);

    while (n--)
        *src++ = cc;

    return s;
}

void *memcpy(void *dest, const void *src, size_t n) noexcept
{
    const uint8_t *csrc {nullptr};
    uint8_t *cdest {nullptr};
    int32_t i = 0;

    if (n == 0)
        return dest;

    cdest = reinterpret_cast<uint8_t*>(dest);
    csrc  = reinterpret_cast<const uint8_t*>(src);
    i     = 0;

   	while(csrc[i] && n > 0) {
        cdest[i] = csrc[i];
        n--;
        i++;
    }

    return dest;
}

int32_t strncmp(const char *s1, const char *s2, size_t n) noexcept
{
    size_t i = 0;

    while((i < n) && (s1[i] || s2[i])) {
        if(s1[i] != s2[i])
            return (s1[i] - s2[i]);
        i++;
    }

    return 0;
}

size_t strncpy(char *dest, const char *src, size_t size) noexcept
{
    size_t i = 0;

    while(src[i] && i < size) {
        dest[i] = src[i];
        i++;
    }

    dest[i] = '\0';
    return i;
}

int32_t strlen(const char *str) noexcept
{
    int32_t i = 0;

    while(str[i])
        i++;

    return i;
}

char *strdup(const char* str) noexcept
{
    if (!str)
        return nullptr;

    size_t len = kstd::strlen(str);

    auto size     = (len + 1) * sizeof(char);
    char* new_str = static_cast<char*>(kmalloc(size, GFP::KERNEL));

    if (!new_str)
        return nullptr;

    for (size_t i = 0; i < len; i++)
        new_str[i] = str[i];

    new_str[len] = '\0';

    return new_str;
}

char *strtok(char *str, const char *delim) noexcept
{
    static char *token = nullptr;

    if (str)
        token = str;

    if (!token)
        return nullptr;

    char *start = token;
    char *end   = strpbrk(token, delim);

    if (end) {
        *end  = '\0';
        token = end + 1;
    }
    else
        token = nullptr;

    return start;
}

char *strpbrk(const char *str, const char *accept) noexcept
{
    const char *s, *a;

    for (s = str; *s != '\0'; s++) {
        for (a = accept; *a != '\0'; a++) {
            if (*s == *a)
                return const_cast<char*>(s);
        }
    }

    return nullptr;
}

} // namespace kstd
} // namespace kernel