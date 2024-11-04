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

#include <kernel/kstd/cstdlib.hpp>
#include <kernel/slab.hpp>


namespace kernel {
namespace kstd {


char **split(const char *str, const char *delim, int *count) noexcept
{
    char* str_copy = strdup(str);

    if (!str_copy)
        return nullptr;

    // count the number of tokens
    int32_t token_count = 0;
    char *token = strtok(str_copy, delim);

    while (token) {
        token_count++;
        token = strtok(nullptr, delim);
    }

    // allocating memory for the array of strings
    auto size     = (token_count + 1) * sizeof(char*);
    char **result = static_cast<char**>(kmalloc(size, GFP::KERNEL));

    if (!result) {
        kfree(str_copy);
        return nullptr; // memory allocation failed
    }

    // reset the string copy and split the string again
    strncpy(str_copy, str, strlen(str));

    token         = strtok(str_copy, delim);
    int32_t index = 0;

    while (token) {
        result[index] = strdup(token);

        if (!result[index]) {
            // freeing previously allocated strings in case of failure
            for (int j = 0; j < index; j++)
                kfree(result[j]);

            kfree(result);
            kfree(str_copy);
            return nullptr; // memory allocation failed
        }

        index++;
        token = strtok(nullptr, delim);
    }
    result[index] = nullptr;

    kfree(str_copy);
    *count = token_count;
    return result;
}


void free_split(char **result) noexcept
{
    for (int i = 0; result[i]; i++)
        kfree(result[i]);

    kfree(result);
}

} // namespace kstd
} // namespace kernel