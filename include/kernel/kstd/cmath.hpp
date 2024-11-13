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
constexpr inline f64 M_E      {2.7182818284590452354};

/** @brief The mathematical constant Pi divided by 2.*/
constexpr inline f64 M_PI_2   {1.57079632679489661923};

/** @brief Represents a NaN (Not-a-Number) value.*/
const auto NAN {(0.0f / 0.0f)};

// Exponential and Logarithmic Functions -------------------------------

/**
 * @brief Calculate natural logarithm.
 *
 * @param [in] x - given value.
 * @return natural logarithm of x.
 */
constexpr f64 log(f64 x) noexcept
{
    if(x <= 0)
        return NAN;

    if(x == 1)
        return 0;

    if(x == M_E)
        return 1;

    s32 i        = 1;
    f64 result = 0;
    f64 term   = (x - 1) / (x + 1);
    f64 square = term * term;

    while(i <= 60000) {
        result += term / i;
        term   *= square;
        i++;
        i++;
    }

    return (2.0 * result);
}

/**
 * @brief Calculate the base-2 logarithm of a given value.
 *
 * @param [in] x - given value.
 * @return base-2 logarithm of @a x.
 */
constexpr inline f64 log2(f64 x) noexcept
{
    return log(x) / log(2.0);
}

// Rounding Functions -----------------------------------------------

/**
 * @brief Calculates the smallest integer value >= the given value.
 *
 * @param [in] x - given value to ceil.
 * @return ceil value of @a x.
 */
constexpr inline f64 ceil(f64 x) noexcept
{
    s32 int_part = static_cast<s32>(x);

    if (x == int_part)
        return x;
    else if (x > 0)
        return int_part + 1;
    else
        return int_part;
}

// Absolute and Maximum/Minimum Functions ----------------------------------

/**
 * @brief Calculate the absolute value of a given value.
 *
 * @param [in] x - given value.
 * @return the absolute value of the given value.
 */
template <typename T>
constexpr inline T abs(T x) noexcept
{
    return ((x) < 0 ? -(x) : (x));
}

// Trigonometric Functions -----------------------------------------------

/**
 * @brief Calculates the sine of given value.
 *
 * @param [in] x - given value.
 * @return sine of @a x.
 */
constexpr f64 sin(f64 x) noexcept
{
    f64 cur  = x;
    f64 acc  = 1;
    f64 fact = 1;
    f64 pow  = x;
    s32 i      = 1;

    while (kstd::abs(acc) > 1e-8 && i < 100) {
        fact *= ((2 * i) * (2 * i + 1));
        pow  *= -1 * x*x;
        acc  =  pow / fact;
        cur  += acc;
        i++;
    }

    return cur;
}

/**
 * @brief Calculates the cosine of given value.
 *
 * @param [in] x - given value.
 * @return cosine of @a x.
 */
constexpr inline f64 cos(f64 x) noexcept
{
    return sin(x + M_PI_2);
}

} // namespace kstd
} // namespace kernel

#endif // _KERNEL_KSTD_CMATH_HPP_