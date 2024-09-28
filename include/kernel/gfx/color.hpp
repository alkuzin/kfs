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
 * @file  colors.hpp
 * @brief Contains declarations for RGB pixel & colors.
 *
 * @author Alexander Kuzin (<a href="https://github.com/alkuzin">alkuzin</a>)
 * @date   25.09.2024
 */

#ifndef _KERNEL_GFX_COLOR_HPP_
#define _KERNEL_GFX_COLOR_HPP_

#include <kernel/kstd/ctypes.hpp>


namespace kernel {
namespace gfx {

using rgb_t = uint32_t;

namespace color {

constexpr rgb_t red    {0xfc3f42};
constexpr rgb_t green  {0x6AF957};
constexpr rgb_t blue   {0x578af9};
constexpr rgb_t black  {0x000000};
constexpr rgb_t white  {0xFFFFFF};
constexpr rgb_t gray   {0x919191};

} // namespace color

} // namespace gfx
} // namespace kernel

#endif // _KERNEL_GFX_COLOR_HPP_