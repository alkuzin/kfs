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


namespace kernel {
namespace info {

inline const auto __kernel_name__                {"unknown"};
inline const auto __kernel_arch__                {"i386"};
inline const auto __kernel_author__              {"Alexander (@alkuzin)"};
inline const auto __kernel_build_date__          {__DATE__};
inline const auto __kernel_build_time__          {__TIME__};
inline const auto __kernel_compiler_version__    {__VERSION__};
inline const auto __kernel_version_major__       {0};
inline const auto __kernel_version_minor__       {3};
inline const auto __kernel_version_lower__       {0};

} // namespace info
} // namespace kernel

#endif // _KERNEL_CONFIG_HPP_