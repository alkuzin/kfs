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
 * @file  builtin.hpp
 * @brief Contains kernel shell builtin functions decalarations.
 *
 * @author Alexander Kuzin (<a href="https://github.com/alkuzin">alkuzin</a>)
 * @date   30.10.2024
 */

#ifndef _KERNEL_SHELL_BUILTIN_HPP_
#define _KERNEL_SHELL_BUILTIN_HPP_

#include <kernel/types.hpp>


namespace kernel {
namespace shell {

using command_t = void (*)(s32, char **);

struct builtin_t {
    const char *name;   // command name
    const char *descr;  // short description
    char      **argv;   // shell arguments
    s32         argc;   // number of shell arguments
    command_t   func;   // command entry point
};

/**
 * @brief Execute command.
 *
 * @param [in] cmd - given command to execute.
 */
void exec(const char *cmd) noexcept;

/**
 * @brief Get the first suitable shell command.
 *
 * @param [in] str - given string.
 * @param [in] len - given string length.
 * @return first suitable shell command - in case of success.
 * @return nullptr - otherwise.
 */
const char *get_suitable_cmd(const char *str, s32 len) noexcept;

} // namespace shell
} // namespace kernel

#endif // _KERNEL_SHELL_BUILTIN_HPP_