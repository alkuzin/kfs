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
 * @file  gfp.hpp
 * @brief Provides definitions for kernel memory allocation flags.
 *
 * @author Alexander Kuzin (<a href="https://github.com/alkuzin">alkuzin</a>)
 * @date   04.10.2024
 */

#ifndef _KERNEL_GFP_HPP_
#define _KERNEL_GFP_HPP_

#include <kernel/types.hpp>


namespace kernel {

// get free pages flags type for memory allocation functions
using gfp_t = uint8_t;

enum GFP : gfp_t {
    KERNEL = 0b00000001,    // for kernel-internal allocation
    ZERO   = 0b00000010     // set allocated pages payload with zeros
};

} // namespace kernel

#endif // _KERNEL_GFP_HPP_