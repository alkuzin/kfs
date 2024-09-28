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
 * @file  config.hpp
 * @brief Contains kernel information.
 *
 * @author Alexander Kuzin (<a href="https://github.com/alkuzin">alkuzin</a>)
 * @date   28.09.2024
 */

#ifndef _KERNEL_CONFIG_HPP_
#define _KERNEL_CONFIG_HPP_

#include <kernel/arch/x86/system.hpp>
#include <kernel/printk.hpp>


namespace kernel {
namespace info {

extern const char      *__kernel_name__;
extern const char      *__kernel_arch__;
extern const char      *__kernel_author__;
extern const char      *__kernel_build_date__;
extern const char      *__kernel_build_time__;
extern const char      *__kernel_compiler_version__;
extern const uint32_t   __kernel_version_major__;
extern const uint32_t   __kernel_version_minor__;
extern const uint32_t   __kernel_version_lower__;

/**
 * @brief Print current space.
 *
 * @param [in] ring - given current privilege level.
 * @return current space.
 */
inline const char *current_space(uint32_t ring) noexcept
{
    if (ring == 0)
        return "kernel space";
    else if(ring == 3)
        return "user space";
    else
        return "unknown";
}

/** @brief Display main OS info: name, version and architecture.*/
inline void display_general(void) noexcept
{
    uint32_t mode = arch::x86::mode();
    uint32_t ring = arch::x86::ring();

    printk(
        "kernel name:      |  %s\n"
        "kernel version:   |  v%d.%d.%d\n"
        "Architecture:     |  %s\n"
        "Operating mode:   |  %u-bit protected\n"
        "Privilege level:  |  ring %u (%s)\n"
        "Author:           |  %s - 2024\n",
        __kernel_name__,
        __kernel_version_major__,
        __kernel_version_minor__,
        __kernel_version_lower__,
        __kernel_arch__,
        mode,
        ring,
        current_space(ring),
        __kernel_author__
    );
}

/** @brief Display OS build info: build date and time.*/
inline void display_build(void) noexcept
{
    printk("build time: %s %s [g++-%s]\n",
    __kernel_build_time__,
    __kernel_build_date__,
    __kernel_compiler_version__
    );
}

} // namespace info
} // namespace kernel

#endif // _KERNEL_CONFIG_HPP_