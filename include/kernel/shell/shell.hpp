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
 * @file  shell.hpp
 * @brief Contains kernel shell decalarations.
 *
 * @author Alexander Kuzin (<a href="https://github.com/alkuzin">alkuzin</a>)
 * @date   26.09.2024
 */

#ifndef _KERNEL_SHELL_HPP_
#define _KERNEL_SHELL_HPP_

#include <kernel/types.hpp>


namespace kernel {

const uint32_t SHELL_BUFFER_SIZE {128};

struct shell_t
{
    char m_buffer[SHELL_BUFFER_SIZE];

private:
    /** @brief Display kernel shell prompt.*/
    void display_prompt(void) const noexcept;

    /** @brief Get the line from user.*/
    void get_line(void) noexcept;

    /**
     * @brief Execute command.
     *
     * @param [in] cmd - given command to execute.
     */
    void exec(const char *cmd) const noexcept;

public:
    /** @brief Initialize kernel shell.*/
    void set(void) noexcept;

    /** @brief Process user input.*/
    void process(void) noexcept;
};

extern shell_t shell;

} // namespace kernel

#endif // _KERNEL_SHELL_HPP_