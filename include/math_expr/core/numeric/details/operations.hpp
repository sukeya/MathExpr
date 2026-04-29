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
    };

math_expr_define_epsilon_type(float, 0.00000100000f)
    math_expr_define_epsilon_type(double, 0.000000000100)
        math_expr_define_epsilon_type(long double, 0.000000000001)

#undef math_expr_define_epsilon_type

template <typename T> inline bool is_nan_impl(const T v)
{
    validate_supported_real_type<T>();
    return std::not_equal_to<T>()(v, v);
}

template <typename T> inline std::int32_t to_int32_impl(const T v)
{
    validate_supported_real_type<T>();
    return static_cast<std::int32_t>(v);
}

template <typename T> inline std::int64_t to_int64_impl(const T v)
{
    validate_supported_real_type<T>();
    return static_cast<std::int64_t>(v);
}

template <typename T> inline std::uint64_t to_uint64_impl(const T v)
{
    validate_supported_real_type<T>();
    return static_cast<std::uint64_t>(v);
}

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

template <typename T> inline T abs_impl(const T v)
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

template <typename T> inline T min_impl(const T v0, const T v1)
{
    validate_supported_numeric_type<T>();
    return std::min<T>(v0, v1);
}

template <typename T> inline T max_impl(const T v0, const T v1)
{
    validate_supported_numeric_type<T>();
    return std::max<T>(v0, v1);
}

template <typename T> inline T equal_impl(const T v0, const T v1)
{
    validate_supported_numeric_type<T>();

    if constexpr (is_supported_integral_type_v<T>)
    {
        return (v0 == v1) ? T(1) : T(0);
    }
    else
    {
        const T epsilon = epsilon_type<T>::value();
        return (abs_impl(v0 - v1) <= (std::max(T(1), std::max(abs_impl(v0), abs_impl(v1))) *
                                      epsilon))
                   ? T(1)
                   : T(0);
    }
}

template <typename T> inline T nequal_impl(const T v0, const T v1)
{
    validate_supported_numeric_type<T>();

    if constexpr (is_supported_integral_type_v<T>)
    {
        return (v0 != v1) ? T(1) : T(0);
    }
    else
    {
        const T epsilon = epsilon_type<T>::value();
        return (abs_impl(v0 - v1) > (std::max(T(1), std::max(abs_impl(v0), abs_impl(v1))) *
                                     epsilon))
                   ? T(1)
                   : T(0);
    }
}

template <typename T> inline T modulus_impl(const T v0, const T v1)
{
    validate_supported_numeric_type<T>();

    if constexpr (is_supported_real_type_v<T>)
    {
        return std::fmod(v0, v1);
    }
    else
    {
        return v0 % v1;
    }
}

template <typename T> inline T pow_impl(const T v0, const T v1)
{
    validate_supported_numeric_type<T>();

    if constexpr (is_supported_real_type_v<T>)
    {
        return std::pow(v0, v1);
    }
    else
    {
        return static_cast<T>(std::pow(static_cast<double>(v0), static_cast<double>(v1)));
    }
}

template <typename T> inline T logn_impl(const T v0, const T v1)
{
    validate_supported_numeric_type<T>();

    if constexpr (is_supported_real_type_v<T>)
    {
        return std::log(v0) / std::log(v1);
    }
    else
    {
        return static_cast<T>(
            logn_impl<double>(static_cast<double>(v0), static_cast<double>(v1)));
    }
}

template <typename T> inline T root_impl(const T v0, const T v1)
{
    validate_supported_numeric_type<T>();

    if constexpr (is_supported_real_type_v<T>)
    {
        if (v0 < T(0))
        {
            return (v1 == std::trunc(v1)) && (std::fmod(v1, T(2)) != T(0))
                       ? -std::pow(abs_impl(v0), T(1) / v1)
                       : quiet_nan_impl<T>();
        }

        return std::pow(v0, T(1) / v1);
    }
    else
    {
        return static_cast<T>(
            root_impl<double>(static_cast<double>(v0), static_cast<double>(v1)));
    }
}

template <typename T> inline T round_impl(const T v)
{
    validate_supported_numeric_type<T>();

    if constexpr (is_supported_real_type_v<T>)
    {
        return ((v < T(0)) ? std::ceil(v - T(0.5)) : std::floor(v + T(0.5)));
    }
    else
    {
        return v;
    }
}

template <typename T> inline T roundn_impl(const T v0, const T v1)
{
    validate_supported_numeric_type<T>();

    if constexpr (is_supported_real_type_v<T>)
    {
        const int index =
            std::max<int>(0, std::min<int>(pow10_size - 1, static_cast<int>(std::floor(v1))));
        const T p10 = T(pow10[index]);

        if (v0 < T(0))
        {
            return T(std::ceil((v0 * p10) - T(0.5)) / p10);
        }
        else
        {
            return T(std::floor((v0 * p10) + T(0.5)) / p10);
        }
    }
    else
    {
        return v0;
    }
}

template <typename T> inline T hypot_impl(const T v0, const T v1)
{
    validate_supported_numeric_type<T>();

    if constexpr (is_supported_real_type_v<T>)
    {
        return std::sqrt((v0 * v0) + (v1 * v1));
    }
    else
    {
        return static_cast<T>(std::sqrt(static_cast<double>((v0 * v0) + (v1 * v1))));
    }
}

template <typename T> inline T atan2_impl(const T v0, const T v1)
{
    validate_supported_numeric_type<T>();

    if constexpr (is_supported_real_type_v<T>)
    {
        return std::atan2(v0, v1);
    }
    else
    {
        static_cast<void>(v0);
        static_cast<void>(v1);
        return T(0);
    }
}

template <typename T> inline T shr_impl(const T v0, const T v1)
{
    validate_supported_numeric_type<T>();

    if constexpr (is_supported_real_type_v<T>)
    {
        return v0 * (T(1) / std::pow(T(2), static_cast<T>(static_cast<int>(v1))));
    }
    else
    {
        return v0 >> v1;
    }
}

template <typename T> inline T shl_impl(const T v0, const T v1)
{
    validate_supported_numeric_type<T>();

    if constexpr (is_supported_real_type_v<T>)
    {
        return v0 * std::pow(T(2), static_cast<T>(static_cast<int>(v1)));
    }
    else
    {
        return v0 << v1;
    }
}

template <typename T> inline T sgn_impl(const T v)
{
    validate_supported_numeric_type<T>();

    if (v > T(0))
    {
        return T(+1);
    }
    else if (v < T(0))
    {
        return T(-1);
    }
    else
    {
        return T(0);
    }
}

template <typename T> inline T and_impl(const T v0, const T v1)
{
    validate_supported_numeric_type<T>();

    if constexpr (is_supported_real_type_v<T>)
    {
        return (is_true_impl(v0) && is_true_impl(v1)) ? T(1) : T(0);
    }
    else
    {
        return v0 && v1;
    }
}

template <typename T> inline T nand_impl(const T v0, const T v1)
{
    validate_supported_numeric_type<T>();

    if constexpr (is_supported_real_type_v<T>)
    {
        return (is_false_impl(v0) || is_false_impl(v1)) ? T(1) : T(0);
    }
    else
    {
        return !(v0 && v1);
    }
}

template <typename T> inline T or_impl(const T v0, const T v1)
{
    validate_supported_numeric_type<T>();

    if constexpr (is_supported_real_type_v<T>)
    {
        return (is_true_impl(v0) || is_true_impl(v1)) ? T(1) : T(0);
    }
    else
    {
        return (v0 || v1);
    }
}

template <typename T> inline T nor_impl(const T v0, const T v1)
{
    validate_supported_numeric_type<T>();

    if constexpr (is_supported_real_type_v<T>)
    {
        return (is_false_impl(v0) && is_false_impl(v1)) ? T(1) : T(0);
    }
    else
    {
        return !(v0 || v1);
    }
}

template <typename T> inline T xor_impl(const T v0, const T v1)
{
    validate_supported_numeric_type<T>();

    if constexpr (is_supported_real_type_v<T>)
    {
        return (is_false_impl(v0) != is_false_impl(v1)) ? T(1) : T(0);
    }
    else
    {
        return v0 ^ v1;
    }
}

template <typename T> inline T xnor_impl(const T v0, const T v1)
{
    validate_supported_numeric_type<T>();

    const bool v0_true = is_true_impl(v0);
    const bool v1_true = is_true_impl(v1);

    if ((v0_true && v1_true) || (!v0_true && !v1_true))
    {
        return T(1);
    }
    else
    {
        return T(0);
    }
}

#if (defined(_MSC_VER) && (_MSC_VER >= 1900)) || !defined(_MSC_VER)
#define math_expr_define_erf(TT, impl)                                                             \
    inline TT erf_native_impl(const TT v)                                                          \
    {                                                                                              \
        return impl(v);                                                                            \
    }

math_expr_define_erf(float, ::erff) math_expr_define_erf(double, ::erf)
    math_expr_define_erf(long double, ::erfl)
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

    const T t = T(1) / (T(1) + T(0.5) * abs_impl(v));

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

template <typename T> inline T erf_impl(const T v)
{
    validate_supported_numeric_type<T>();

    if constexpr (is_supported_real_type_v<T>)
    {
        return erf_real_impl(v);
    }
    else
    {
        return static_cast<T>(erf_real_impl(static_cast<double>(v)));
    }
}

#if (defined(_MSC_VER) && (_MSC_VER >= 1900)) || !defined(_MSC_VER)
#define math_expr_define_erfc(TT, impl)                                                            \
    inline TT erfc_native_impl(const TT v)                                                         \
    {                                                                                              \
        return impl(v);                                                                            \
    }

math_expr_define_erfc(float, ::erfcf) math_expr_define_erfc(double, ::erfc)
    math_expr_define_erfc(long double, ::erfcl)
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

template <typename T> inline T erfc_impl(const T v)
{
    validate_supported_numeric_type<T>();

    if constexpr (is_supported_real_type_v<T>)
    {
        return erfc_real_impl(v);
    }
    else
    {
        return static_cast<T>(erfc_real_impl(static_cast<double>(v)));
    }
}

template <typename T> inline T ncdf_impl(const T v)
{
    validate_supported_numeric_type<T>();

    if constexpr (is_supported_real_type_v<T>)
    {
        return T(0.5) * erfc_real_impl(-(v / T(sqrt2)));
    }
    else
    {
        return static_cast<T>(ncdf_impl(static_cast<double>(v)));
    }
}

template <typename T> inline T sinc_impl(const T v)
{
    validate_supported_numeric_type<T>();

    if constexpr (is_supported_real_type_v<T>)
    {
        if (std::abs(v) >= std::numeric_limits<T>::epsilon())
        {
            return (std::sin(v) / v);
        }
        else
        {
            return T(1);
        }
    }
    else
    {
        return static_cast<T>(sinc_impl(static_cast<double>(v)));
    }
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
    if (abs_impl(v) < T(0.00001))
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
        if (abs_impl(v) > T(0.0001))
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

template <typename T> inline T acos_impl(const T v)
{
    validate_supported_numeric_type<T>();
    if constexpr (is_supported_real_type_v<T>)
    {
        return std::acos(v);
    }
    else
    {
        static_cast<void>(v);
        return quiet_nan_impl<T>();
    }
}

template <typename T> inline T acosh_impl(const T v)
{
    validate_supported_numeric_type<T>();
    if constexpr (is_supported_real_type_v<T>)
    {
        return acosh_real_impl(v);
    }
    else
    {
        static_cast<void>(v);
        return quiet_nan_impl<T>();
    }
}

template <typename T> inline T asin_impl(const T v)
{
    validate_supported_numeric_type<T>();
    if constexpr (is_supported_real_type_v<T>)
    {
        return std::asin(v);
    }
    else
    {
        static_cast<void>(v);
        return quiet_nan_impl<T>();
    }
}

template <typename T> inline T asinh_impl(const T v)
{
    validate_supported_numeric_type<T>();
    if constexpr (is_supported_real_type_v<T>)
    {
        return asinh_real_impl(v);
    }
    else
    {
        static_cast<void>(v);
        return quiet_nan_impl<T>();
    }
}

template <typename T> inline T atan_impl(const T v)
{
    validate_supported_numeric_type<T>();
    if constexpr (is_supported_real_type_v<T>)
    {
        return std::atan(v);
    }
    else
    {
        static_cast<void>(v);
        return quiet_nan_impl<T>();
    }
}

template <typename T> inline T atanh_impl(const T v)
{
    validate_supported_numeric_type<T>();
    if constexpr (is_supported_real_type_v<T>)
    {
        return atanh_real_impl(v);
    }
    else
    {
        static_cast<void>(v);
        return quiet_nan_impl<T>();
    }
}

template <typename T> inline T ceil_impl(const T v)
{
    validate_supported_numeric_type<T>();
    if constexpr (is_supported_real_type_v<T>)
    {
        return std::ceil(v);
    }
    else
    {
        return v;
    }
}

template <typename T> inline T cos_impl(const T v)
{
    validate_supported_numeric_type<T>();
    if constexpr (is_supported_real_type_v<T>)
    {
        return std::cos(v);
    }
    else
    {
        static_cast<void>(v);
        return quiet_nan_impl<T>();
    }
}

template <typename T> inline T cosh_impl(const T v)
{
    validate_supported_numeric_type<T>();
    if constexpr (is_supported_real_type_v<T>)
    {
        return std::cosh(v);
    }
    else
    {
        static_cast<void>(v);
        return quiet_nan_impl<T>();
    }
}

template <typename T> inline T exp_impl(const T v)
{
    validate_supported_numeric_type<T>();
    return std::exp(v);
}

template <typename T> inline T floor_impl(const T v)
{
    validate_supported_numeric_type<T>();
    if constexpr (is_supported_real_type_v<T>)
    {
        return std::floor(v);
    }
    else
    {
        return v;
    }
}

template <typename T> inline T log_impl(const T v)
{
    validate_supported_numeric_type<T>();
    return std::log(v);
}

template <typename T> inline T log10_impl(const T v)
{
    validate_supported_numeric_type<T>();
    return std::log10(v);
}

template <typename T> inline T log2_impl(const T v)
{
    validate_supported_numeric_type<T>();
    return std::log(v) / T(log2_value);
}

template <typename T> inline T neg_impl(const T v)
{
    validate_supported_numeric_type<T>();
    return -v;
}

template <typename T> inline T pos_impl(const T v)
{
    validate_supported_numeric_type<T>();
    return +v;
}

template <typename T> inline T sin_impl(const T v)
{
    validate_supported_numeric_type<T>();
    if constexpr (is_supported_real_type_v<T>)
    {
        return std::sin(v);
    }
    else
    {
        static_cast<void>(v);
        return quiet_nan_impl<T>();
    }
}

template <typename T> inline T sinh_impl(const T v)
{
    validate_supported_numeric_type<T>();
    if constexpr (is_supported_real_type_v<T>)
    {
        return std::sinh(v);
    }
    else
    {
        static_cast<void>(v);
        return quiet_nan_impl<T>();
    }
}

template <typename T> inline T sqrt_impl(const T v)
{
    validate_supported_numeric_type<T>();
    return std::sqrt(v);
}

template <typename T> inline T tan_impl(const T v)
{
    validate_supported_numeric_type<T>();
    if constexpr (is_supported_real_type_v<T>)
    {
        return std::tan(v);
    }
    else
    {
        static_cast<void>(v);
        return quiet_nan_impl<T>();
    }
}

template <typename T> inline T tanh_impl(const T v)
{
    validate_supported_numeric_type<T>();
    if constexpr (is_supported_real_type_v<T>)
    {
        return std::tanh(v);
    }
    else
    {
        static_cast<void>(v);
        return quiet_nan_impl<T>();
    }
}

template <typename T> inline T cot_impl(const T v)
{
    validate_supported_numeric_type<T>();
    if constexpr (is_supported_real_type_v<T>)
    {
        return T(1) / std::tan(v);
    }
    else
    {
        static_cast<void>(v);
        return quiet_nan_impl<T>();
    }
}

template <typename T> inline T sec_impl(const T v)
{
    validate_supported_numeric_type<T>();
    if constexpr (is_supported_real_type_v<T>)
    {
        return T(1) / std::cos(v);
    }
    else
    {
        static_cast<void>(v);
        return quiet_nan_impl<T>();
    }
}

template <typename T> inline T csc_impl(const T v)
{
    validate_supported_numeric_type<T>();
    if constexpr (is_supported_real_type_v<T>)
    {
        return T(1) / std::sin(v);
    }
    else
    {
        static_cast<void>(v);
        return quiet_nan_impl<T>();
    }
}

template <typename T> inline T r2d_impl(const T v)
{
    validate_supported_numeric_type<T>();
    if constexpr (is_supported_real_type_v<T>)
    {
        return (v * T(_180_pi));
    }
    else
    {
        static_cast<void>(v);
        return quiet_nan_impl<T>();
    }
}

template <typename T> inline T d2r_impl(const T v)
{
    validate_supported_numeric_type<T>();
    if constexpr (is_supported_real_type_v<T>)
    {
        return (v * T(pi_180));
    }
    else
    {
        static_cast<void>(v);
        return quiet_nan_impl<T>();
    }
}

template <typename T> inline T d2g_impl(const T v)
{
    validate_supported_numeric_type<T>();
    if constexpr (is_supported_real_type_v<T>)
    {
        return (v * T(10.0 / 9.0));
    }
    else
    {
        static_cast<void>(v);
        return quiet_nan_impl<T>();
    }
}

template <typename T> inline T g2d_impl(const T v)
{
    validate_supported_numeric_type<T>();
    if constexpr (is_supported_real_type_v<T>)
    {
        return (v * T(9.0 / 10.0));
    }
    else
    {
        static_cast<void>(v);
        return quiet_nan_impl<T>();
    }
}

template <typename T> inline T notl_impl(const T v)
{
    validate_supported_numeric_type<T>();

    if constexpr (is_supported_real_type_v<T>)
    {
        return (std::not_equal_to<T>()(T(0), v) ? T(0) : T(1));
    }
    else
    {
        return !v;
    }
}

template <typename T> inline T trunc_impl(const T v)
{
    validate_supported_numeric_type<T>();

    if constexpr (is_supported_real_type_v<T>)
    {
        return trunc_real_impl(v);
    }
    else
    {
        return v;
    }
}

template <typename T> inline T frac_impl(const T v)
{
    validate_supported_numeric_type<T>();

    if constexpr (is_supported_real_type_v<T>)
    {
        return (v - trunc_impl(v));
    }
    else
    {
        static_cast<void>(v);
        return T(0);
    }
}

template <typename T> inline T expm1_impl(const T v)
{
    validate_supported_numeric_type<T>();

    if constexpr (is_supported_real_type_v<T>)
    {
        return expm1_real_impl(v);
    }
    else
    {
#if __cplusplus >= 201103L
        return std::expm1(v);
#else
        return T(std::exp<double>(v)) - T(1);
#endif
    }
}

template <typename T> inline T log1p_impl(const T v)
{
    validate_supported_numeric_type<T>();

    if constexpr (is_supported_real_type_v<T>)
    {
        return log1p_real_impl(v);
    }
    else
    {
#if __cplusplus >= 201103L
        return std::log1p(v);
#else
        if (v > T(-1))
        {
            return std::log(T(1) + v);
        }

        return quiet_nan_impl<T>();
#endif
    }
}

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

template <typename T> inline bool is_integer_impl(const T& v)
{
    validate_supported_numeric_type<T>();

    if constexpr (is_supported_real_type_v<T>)
    {
        return std::equal_to<T>()(T(0), std::fmod(v, T(1)));
    }
    else
    {
        static_cast<void>(v);
        return true;
    }
}

} // namespace math_expr::core::numeric::details

#endif
