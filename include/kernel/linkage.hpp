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
 * @file  linkage.hpp
 * @brief Contain linkage declarations.
 *
 * @author Alexander Kuzin (<a href="https://github.com/alkuzin">alkuzin</a>)
 * @date   03.10.2024
 */

#ifndef _KERNEL_LINKAGE_HPP_
#define _KERNEL_LINKAGE_HPP_

namespace kernel {

// for C++ code, the assembly linkage must be declared
// as extern "C" to prevent name mangling
#ifdef __cplusplus
#define cpp_linkage extern "C"
#else
#define cpp_linkage
#endif // __cplusplus

#ifndef asmlinkage
#define asmlinkage cpp_linkage
#endif // asmlinkage

} // namespace kernel

#endif // _KERNEL_LINKAGE_HPP_