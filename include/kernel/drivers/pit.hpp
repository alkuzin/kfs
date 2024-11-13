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
 * @file  pit.hpp
 * @brief Contains Programmable Interval Timer (PIT) driver declaration.
 *
 * @author Alexander Kuzin (<a href="https://github.com/alkuzin">alkuzin</a>)
 * @date   29.10.2024
 */

#ifndef _KERNEL_DRIVER_PIT_HPP_
#define _KERNEL_DRIVER_PIT_HPP_

#include <kernel/types.hpp>


namespace kernel {
namespace driver {
namespace pit {

inline const u32 PIT_FREQUENCY      {1193180};  // 1.193182 MHz
inline const u32 FREQUENCY          {100};      // (ticking each 10 ms)
inline const u32 MILLISEC_PER_SEC   {1000};
inline const u32 MILLISEC_PER_TICK  {MILLISEC_PER_SEC / FREQUENCY};

/** @brief Initialize Programmable Interval Timer (PIT).*/
void init(void) noexcept;

/**
 * @brief Get the current number of timer ticks.
 *
 * @return number of timer ticks.
 */
u32 get_ticks(void) noexcept;

/**
 * @brief Time delay in ms.
 *
 * @param [in] msec - given number of milliseconds to delay.
 */
void delay(u32 msec) noexcept;

} // namespace pit
} // namespace driver
} // namespace kernel

#endif // _KERNEL_DRIVER_PIT_HPP_