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
#include <kernel/arch/i686/io.hpp>
#include <kernel/kstd/cstdio.hpp>
#include <kernel/shell/shell.hpp>


namespace kernel {
namespace shell {

using namespace driver;

inline const auto SHELL_BUFFER_SIZE {128};
static char shell_buffer[SHELL_BUFFER_SIZE];

static s32 x_pos {0};
static s32 y_pos {0};

/** @brief Display kernel shell prompt.*/
static inline void display_prompt(void)
{
    kstd::putchar('$');
    kstd::putchar(' ');
    x_pos = tty::terminal.x_pos;
    y_pos = tty::terminal.y_pos;
}

/** @brief Custom TAB key handler.*/
static bool tab_handler(void)
{
    s32 len = kstd::strlen(shell_buffer);
    const char *command = get_suitable_cmd(shell_buffer, len);

    if (command) {
        // updating shell buffer
        kstd::strncpy(shell_buffer, command, SHELL_BUFFER_SIZE);

        // clearing input
        for (s32 i = 0; i < len; i++)
            kstd::putchar('\b');

        // printing full command
        kstd::putk(shell_buffer);

        // update keyboard get_line position
        auto new_pos = keyboard::get_pos() + kstd::strlen(command) - len;
        keyboard::set_pos(new_pos);

        shell_buffer[keyboard::get_pos()] = ' ';
        kstd::putchar(' ');
        return true;
    }

    for (s32 i = 0; i < tty::TAB_WIDTH; i++)
        kstd::putchar(' ');

    return false;
}

static bool clear_input(void)
{
    s32 len = kstd::strlen(shell_buffer);

    // clearing input & shell buffer
    for (s32 i = 0; i < len; i++) {
        shell_buffer[i] = 0;
        kstd::putchar('\b');
    }

    tty::terminal.x_pos = x_pos;
    tty::terminal.y_pos = y_pos;
    driver::keyboard::set_pos(0);

    return true;
}

void init(void)
{
    kstd::memset(shell_buffer, 0, SHELL_BUFFER_SIZE);

    auto clear_screen = []() -> bool {
        tty::clear();
        display_prompt();
        kstd::putk(shell_buffer);
        return true;
    };

    keyboard::set_ctrl_handler(keyboard::KEY::BACKSPACE, clear_input);
    keyboard::set_ctrl_handler(keyboard::KEY::L, clear_screen);
}

void process(void)
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