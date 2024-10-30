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

#include <kernel/drivers/keyboard.hpp>
#include <kernel/shell/builtin.hpp>
#include <kernel/kstd/cstring.hpp>
#include <kernel/kstd/cstdio.hpp>
#include <kernel/shell/shell.hpp>


namespace kernel {
namespace shell {

inline const auto SHELL_BUFFER_SIZE {128};
static char shell_buffer[SHELL_BUFFER_SIZE];

/** @brief Display kernel shell prompt.*/
static inline void display_prompt(void) noexcept
{
    kstd::putchar('$');
    kstd::putchar(' ');
}

/** @brief Custom TAB key handler.*/
static bool tab_handler(void) noexcept
{
    int32_t len         = kstd::strlen(shell_buffer);
    const char *command = get_suitable_cmd(shell_buffer, len);

    if (command) {
        // updating shell buffer
        kstd::strncpy(shell_buffer, command, SHELL_BUFFER_SIZE);

        // clearing input
        for (int32_t i = 0; i < len; i++)
            kstd::putchar('\b');

        // printing full command
        kstd::putk(shell_buffer);
        return true;
    }

    return false;
}

void init(void) noexcept
{
    kstd::memset(shell_buffer, 0, SHELL_BUFFER_SIZE);
}

void process(void) noexcept
{
    for (;;) {
        display_prompt();
        driver::keyboard::set_tab_handler(tab_handler);
        driver::keyboard::get_line(shell_buffer, SHELL_BUFFER_SIZE);

        if (shell_buffer[0])
            exec(shell_buffer);

        kstd::memset(shell_buffer, 0, SHELL_BUFFER_SIZE);
    }
}

} // namespace shell
} // namespace kernel