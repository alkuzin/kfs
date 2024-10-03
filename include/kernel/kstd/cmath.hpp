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
 * @file  cmath.hpp
 * @brief Standard mathematical functions and constants.
 *
 * @author Alexander Kuzin (<a href="https://github.com/alkuzin">alkuzin</a>)
 * @date   25.09.2024
 */

#ifndef _KERNEL_KSTD_CMATH_HPP_
#define _KERNEL_KSTD_CMATH_HPP_

#include <kernel/types.hpp>


namespace kernel {
namespace kstd {

// Math Constants ---------------------------------------------------------

/** @brief The mathematical constant e (Euler's number).*/
constexpr inline float64_t M_E {2.7182818284590452354};

/** @brief Represents a NaN (Not-a-Number) value.*/
const auto NAN {(0.0f / 0.0f)};

// Exponential and Logarithmic Functions -------------------------------

/**
 * @brief Calculate natural logarithm.
 *
 * @param [in] x - given value.
 * @return natural logarithm of x.
 */
constexpr float64_t log(float64_t x) noexcept
{
    if(x <= 0)
        return NAN;

    if(x == 1)
        return 0;

    if(x == M_E)
        return 1;

    int32_t i        = 1;
    float64_t result = 0;
    float64_t term   = (x - 1) / (x + 1);
    float64_t square = term * term;

    while(i <= 60000) {
        result += term / i;
        term   *= square;
        i++;
        i++;
    }

    return (2.0 * result);
}

} // namespace kstd
} // namespace kernel

#endif // _KERNEL_KSTD_CMATH_HPP_