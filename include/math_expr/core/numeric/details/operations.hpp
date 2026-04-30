/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * Simple Example 02                                          *
 * Author: Arash Partow (1999-2025)                           *
 * URL: https://www.partow.net/programming/exprtk/index.html  *
 *                                                            *
 * Copyright notice:                                          *
 * Free use of the Mathematical Expression Toolkit Library is *
 * permitted under the guidelines and in accordance with the  *
 * most current version of the MIT License.                   *
 * https://www.opensource.org/licenses/MIT                    *
 * SPDX-License-Identifier: MIT                               *
 *                                                            *
 **************************************************************


Copyright 2026 Yuya Asano <my_favorite_theory@yahoo.co.jp>

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef MATH_EXPR_CORE_NUMERIC_DETAILS_OPERATIONS_HPP
#define MATH_EXPR_CORE_NUMERIC_DETAILS_OPERATIONS_HPP

#include "math_expr/core/numeric/constants.hpp"
#include "math_expr/core/std_includes.hpp"
#include "math_expr/core/types.hpp"

namespace math_expr::core::numeric::details
{
static const double pow10[] = {1.0,      1.0E+001, 1.0E+002, 1.0E+003, 1.0E+004, 1.0E+005,
                               1.0E+006, 1.0E+007, 1.0E+008, 1.0E+009, 1.0E+010, 1.0E+011,
                               1.0E+012, 1.0E+013, 1.0E+014, 1.0E+015, 1.0E+016};

static const std::size_t pow10_size = sizeof(pow10) / sizeof(double);

template <typename T>
inline constexpr bool is_supported_real_type_v =
    std::is_same_v<T, float> || std::is_same_v<T, double> || std::is_same_v<T, long double>;

template <typename T>
inline constexpr bool is_supported_integral_type_v =
    std::is_same_v<T, std::int16_t> || std::is_same_v<T, std::int32_t> ||
    std::is_same_v<T, std::int64_t> || std::is_same_v<T, std::uint16_t> ||
    std::is_same_v<T, std::uint32_t> || std::is_same_v<T, std::uint64_t>;

template <typename T>
inline constexpr bool is_supported_numeric_type_v =
    is_supported_real_type_v<T> || is_supported_integral_type_v<T>;

template <typename T> inline constexpr void validate_supported_numeric_type()
{
    static_assert(is_supported_numeric_type_v<T>,
                  "math_expr numeric helpers support float, double, long double, std::int16_t, "
                  "std::int32_t, std::int64_t, std::uint16_t, std::uint32_t, and std::uint64_t "
                  "only.");
}

template <typename T> inline constexpr void validate_supported_real_type()
{
    static_assert(is_supported_real_type_v<T>,
                  "math_expr real-only helpers support float, double, and long double only.");
}

template <typename T> inline T quiet_nan_impl()
{
    return std::numeric_limits<T>::quiet_NaN();
}

template <typename T> struct epsilon_type
{
};

#define math_expr_define_epsilon_type(Type, Epsilon)                                               \
    template <> struct epsilon_type<Type>                                                          \
    {                                                                                              \
        static inline Type value()                                                                 \
        {                                                                                          \
            const Type epsilon = static_cast<Type>(Epsilon);                                       \
            return epsilon;                                                                        \
        }                                                                                          \
    }

math_expr_define_epsilon_type(float, 0.00000100000f);
math_expr_define_epsilon_type(double, 0.000000000100);
math_expr_define_epsilon_type(long double, 0.000000000001);

#undef math_expr_define_epsilon_type

template <typename T> inline bool is_true_impl(const T v)
{
    validate_supported_numeric_type<T>();
    return std::not_equal_to<T>()(T(0), v);
}

template <typename T> inline bool is_false_impl(const T v)
{
    validate_supported_numeric_type<T>();
    return std::equal_to<T>()(T(0), v);
}

template <typename T> inline T abs_value(const T v)
{
    validate_supported_numeric_type<T>();

    if constexpr (is_supported_real_type_v<T>)
    {
        return ((v < T(0)) ? -v : v);
    }
    else if constexpr (std::is_unsigned_v<T>)
    {
        return v;
    }
    else
    {
        return ((v >= T(0)) ? v : -v);
    }
}

#if (defined(_MSC_VER) && (_MSC_VER >= 1900)) || !defined(_MSC_VER)
#define math_expr_define_erf(TT, impl)                                                             \
    inline TT erf_native_impl(const TT v)                                                          \
    {                                                                                              \
        return impl(v);                                                                            \
    }

math_expr_define_erf(float, ::erff);
math_expr_define_erf(double, ::erf);
math_expr_define_erf(long double, ::erfl);
#undef math_expr_define_erf
#endif

template <typename T> inline T erf_real_impl(const T v)
{
    validate_supported_real_type<T>();

#if defined(_MSC_VER) && (_MSC_VER < 1900)
    // Credits: Abramowitz & Stegun Equations 7.1.25-28
    static const T c[] = {T(1.26551223),  T(1.00002368), T(0.37409196),  T(0.09678418),
                          T(-0.18628806), T(0.27886807), T(-1.13520398), T(1.48851587),
                          T(-0.82215223), T(0.17087277)};

    const T t = T(1) / (T(1) + T(0.5) * abs_value(v));

    const T result =
        T(1) -
        t * std::exp(
                (-v * v) - c[0] +
                t * (c[1] +
                     t * (c[2] +
                          t * (c[3] +
                               t * (c[4] +
                                    t * (c[5] +
                                         t * (c[6] + t * (c[7] + t * (c[8] + t * (c[9]))))))))));

    return (v >= T(0)) ? result : -result;
#else
    return erf_native_impl(v);
#endif
}

#if (defined(_MSC_VER) && (_MSC_VER >= 1900)) || !defined(_MSC_VER)
#define math_expr_define_erfc(TT, impl)                                                            \
    inline TT erfc_native_impl(const TT v)                                                         \
    {                                                                                              \
        return impl(v);                                                                            \
    }

math_expr_define_erfc(float, ::erfcf);
math_expr_define_erfc(double, ::erfc);
math_expr_define_erfc(long double, ::erfcl);
#undef math_expr_define_erfc
#endif

template <typename T> inline T erfc_real_impl(const T v)
{
    validate_supported_real_type<T>();

#if defined(_MSC_VER) && (_MSC_VER < 1900)
    return T(1) - erf_real_impl(v);
#else
    return erfc_native_impl(v);
#endif
}

#if __cplusplus >= 201103L
template <typename T> inline T acosh_real_impl(const T v)
{
    return std::acosh(v);
}

template <typename T> inline T asinh_real_impl(const T v)
{
    return std::asinh(v);
}

template <typename T> inline T atanh_real_impl(const T v)
{
    return std::atanh(v);
}

template <typename T> inline T trunc_real_impl(const T v)
{
    return std::trunc(v);
}

template <typename T> inline T expm1_real_impl(const T v)
{
    return std::expm1(v);
}

template <typename T> inline T log1p_real_impl(const T v)
{
    return std::log1p(v);
}
#else
template <typename T> inline T acosh_real_impl(const T v)
{
    return std::log(v + std::sqrt((v * v) - T(1)));
}

template <typename T> inline T asinh_real_impl(const T v)
{
    return std::log(v + std::sqrt((v * v) + T(1)));
}

template <typename T> inline T atanh_real_impl(const T v)
{
    return (std::log(T(1) + v) - std::log(T(1) - v)) / T(2);
}

template <typename T> inline T trunc_real_impl(const T v)
{
    return T(static_cast<long long>(v));
}

template <typename T> inline T expm1_real_impl(const T v)
{
    if (abs_value(v) < T(0.00001))
    {
        return v + (T(0.5) * v * v);
    }
    else
    {
        return std::exp(v) - T(1);
    }
}

template <typename T> inline T log1p_real_impl(const T v)
{
    if (v > T(-1))
    {
        if (abs_value(v) > T(0.0001))
        {
            return std::log(T(1) + v);
        }
        else
        {
            return (T(-0.5) * v + T(1)) * v;
        }
    }

    return quiet_nan_impl<T>();
}
#endif

template <typename T> inline T const_pi_impl()
{
    validate_supported_real_type<T>();
    return T(pi);
}

template <typename T> inline T const_e_impl()
{
    validate_supported_real_type<T>();
    return T(e);
}

template <typename T> inline T const_qnan_impl()
{
    validate_supported_real_type<T>();
    return std::numeric_limits<T>::quiet_NaN();
}

} // namespace math_expr::core::numeric::details

#endif
