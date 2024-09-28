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

#include <kernel/config.hpp>


namespace kernel {
namespace info {

const char      *__kernel_name__                {"unknown"};
const char      *__kernel_arch__                {"x86"};
const char      *__kernel_author__              {"Alexander (@alkuzin)"};
const char      *__kernel_build_date__          {__DATE__};
const char      *__kernel_build_time__          {__TIME__};
const char      *__kernel_compiler_version__    {__VERSION__};
const uint32_t   __kernel_version_major__       {0};
const uint32_t   __kernel_version_minor__       {2};
const uint32_t   __kernel_version_lower__       {0};

} // namespace info
} // namespace kernel