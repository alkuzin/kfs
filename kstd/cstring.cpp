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

} // namespace kstd
} // namespace kernel