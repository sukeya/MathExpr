/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
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

#ifndef MATH_EXPR_CORE_NUMERIC_FUNCTIONS_HPP
#define MATH_EXPR_CORE_NUMERIC_FUNCTIONS_HPP

#include "math_expr/core/std_includes.hpp"
#include "math_expr/core/types.hpp"
#include "math_expr/core/numeric/constants.hpp"

namespace math_expr::core::numeric
{
template <typename T>
struct set_zero_value_impl
{
    static inline void process(T* base_ptr, const std::size_t size)
    {
        const T zero = T(0);
        for (std::size_t i = 0; i < size; ++i)
        {
            base_ptr[i] = zero;
        }
    }
};

#define POD_SET_ZERO_VALUE(T)                                           \
    template <>                                                         \
    struct set_zero_value_impl<T>                                       \
    {                                                                   \
        static inline void process(T* base_ptr, const std::size_t size) \
        {                                                               \
            std::memset(base_ptr, 0, size * sizeof(T));                 \
        }                                                               \
    }

POD_SET_ZERO_VALUE(float);
POD_SET_ZERO_VALUE(double);
POD_SET_ZERO_VALUE(long double);

#ifdef POD_SET_ZERO_VALUE
#undef POD_SET_ZERO_VALUE
#endif

template <typename T>
inline void set_zero_value(T* data, const std::size_t size)
{
    set_zero_value_impl<T>::process(data, size);
}

template <typename T>
inline void set_zero_value(std::vector<T>& v)
{
    set_zero_value(v.data(), v.size());
}

}  // namespace math_expr::core::numeric

#include "math_expr/core/numeric/details/operations.hpp"

namespace math_expr::core::numeric
{
template <typename T>
inline std::int32_t to_int32(const T v)
{
    details::validate_supported_real_type<T>();
    return static_cast<std::int32_t>(v);
}

template <typename T>
inline std::int64_t to_int64(const T v)
{
    details::validate_supported_real_type<T>();
    return static_cast<std::int64_t>(v);
}

template <typename T>
inline std::uint64_t to_uint64(const T v)
{
    details::validate_supported_real_type<T>();
    return static_cast<std::uint64_t>(v);
}

template <typename T>
inline bool is_nan(const T v)
{
    details::validate_supported_real_type<T>();
    return std::not_equal_to<T>()(v, v);
}

template <typename T>
inline T quiet_nan()
{
    return std::numeric_limits<T>::quiet_NaN();
}

template <typename T>
inline bool is_true(const T v)
{
    details::validate_supported_numeric_type<T>();
    return std::not_equal_to<T>()(::math_expr::core::numeric::false_v<T>, v);
}

template <typename T>
inline bool is_false(const T v)
{
    details::validate_supported_numeric_type<T>();
    return std::equal_to<T>()(::math_expr::core::numeric::false_v<T>, v);
}

template <typename T>
inline constexpr T const_pi()
{
    details::validate_supported_real_type<T>();
    return T(pi);
}

template <typename T>
inline constexpr T const_e()
{
    details::validate_supported_real_type<T>();
    return T(e);
}

template <typename T>
inline T const_qnan()
{
    details::validate_supported_real_type<T>();
    return std::numeric_limits<T>::quiet_NaN();
}

template <typename T>
inline T min(const T v0, const T v1)
{
    details::validate_supported_numeric_type<T>();
    return std::min<T>(v0, v1);
}

template <typename T>
inline T max(const T v0, const T v1)
{
    details::validate_supported_numeric_type<T>();
    return std::max<T>(v0, v1);
}

template <typename T>
inline T equal(const T v0, const T v1)
{
    details::validate_supported_numeric_type<T>();

    if constexpr (details::is_supported_integral_type_v<T>)
    {
        return std::equal_to<T>()(v0, v1) ? true_v<T> : false_v<T>;
    }
    else
    {
        const T epsilon = details::epsilon_type<T>::value();
        return (details::abs_value(v0 - v1) <=
                (std::max(T(1), std::max(details::abs_value(v0), details::abs_value(v1))) *
                 epsilon))
                   ? true_v<T>
                   : false_v<T>;
    }
}

template <typename T>
inline T nequal(const T v0, const T v1)
{
    details::validate_supported_numeric_type<T>();

    if constexpr (details::is_supported_integral_type_v<T>)
    {
        return std::not_equal_to<T>()(v0, v1) ? true_v<T> : false_v<T>;
    }
    else
    {
        const T epsilon = details::epsilon_type<T>::value();
        return (details::abs_value(v0 - v1) >
                (std::max(T(1), std::max(details::abs_value(v0), details::abs_value(v1))) *
                 epsilon))
                   ? true_v<T>
                   : false_v<T>;
    }
}

template <typename T>
inline T modulus(const T v0, const T v1)
{
    details::validate_supported_numeric_type<T>();

    if constexpr (details::is_supported_real_type_v<T>)
    {
        return std::fmod(v0, v1);
    }
    else
    {
        return v0 % v1;
    }
}

template <typename T>
inline T pow(const T v0, const T v1)
{
    details::validate_supported_numeric_type<T>();

    if constexpr (details::is_supported_real_type_v<T>)
    {
        return std::pow(v0, v1);
    }
    else
    {
        return static_cast<T>(std::pow(static_cast<double>(v0), static_cast<double>(v1)));
    }
}

template <typename T>
inline T logn(const T v0, const T v1)
{
    details::validate_supported_numeric_type<T>();

    if constexpr (details::is_supported_real_type_v<T>)
    {
        return std::log(v0) / std::log(v1);
    }
    else
    {
        return static_cast<T>(logn<double>(static_cast<double>(v0), static_cast<double>(v1)));
    }
}

template <typename T>
inline T root(const T v0, const T v1)
{
    details::validate_supported_numeric_type<T>();

    if constexpr (details::is_supported_real_type_v<T>)
    {
        if (v0 < T(0))
        {
            return (v1 == std::trunc(v1)) && (std::fmod(v1, T(2)) != T(0))
                       ? -std::pow(details::abs_value(v0), T(1) / v1)
                       : quiet_nan<T>();
        }

        return std::pow(v0, T(1) / v1);
    }
    else
    {
        return static_cast<T>(root<double>(static_cast<double>(v0), static_cast<double>(v1)));
    }
}

template <typename T>
inline T roundn(const T v0, const T v1)
{
    details::validate_supported_numeric_type<T>();

    if constexpr (details::is_supported_real_type_v<T>)
    {
        const int index = std::max<int>(
            0, std::min<int>(details::pow10_size - 1, static_cast<int>(std::floor(v1))));
        const T p10 = T(details::pow10[index]);

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

template <typename T>
inline T hypot(const T v0, const T v1)
{
    details::validate_supported_numeric_type<T>();

    if constexpr (details::is_supported_real_type_v<T>)
    {
        return std::hypot(v0, v1);
    }
    else
    {
        return static_cast<T>(std::hypot(static_cast<double>(v0), static_cast<double>(v1)));
    }
}

template <typename T>
inline T atan2(const T v0, const T v1)
{
    details::validate_supported_numeric_type<T>();

    if constexpr (details::is_supported_real_type_v<T>)
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

template <typename T>
inline T shr(const T v0, const T v1)
{
    details::validate_supported_numeric_type<T>();

    if constexpr (details::is_supported_real_type_v<T>)
    {
        return v0 * (T(1) / std::pow(T(2), static_cast<T>(static_cast<int>(v1))));
    }
    else
    {
        return v0 >> v1;
    }
}

template <typename T>
inline T shl(const T v0, const T v1)
{
    details::validate_supported_numeric_type<T>();

    if constexpr (details::is_supported_real_type_v<T>)
    {
        return v0 * std::pow(T(2), static_cast<T>(static_cast<int>(v1)));
    }
    else
    {
        return v0 << v1;
    }
}

template <typename T>
inline T and_opr(const T v0, const T v1)
{
    details::validate_supported_numeric_type<T>();

    return (is_true(v0) && is_true(v1)) ? true_v<T> : false_v<T>;
}

template <typename T>
inline T nand_opr(const T v0, const T v1)
{
    details::validate_supported_numeric_type<T>();

    return (is_false(v0) || is_false(v1)) ? true_v<T> : false_v<T>;
}

template <typename T>
inline T or_opr(const T v0, const T v1)
{
    details::validate_supported_numeric_type<T>();

    return (is_true(v0) || is_true(v1)) ? true_v<T> : false_v<T>;
}

template <typename T>
inline T nor_opr(const T v0, const T v1)
{
    details::validate_supported_numeric_type<T>();

    return (is_false(v0) && is_false(v1)) ? true_v<T> : false_v<T>;
}

template <typename T>
inline T xor_opr(const T v0, const T v1)
{
    details::validate_supported_numeric_type<T>();

    if constexpr (details::is_supported_real_type_v<T>)
    {
        return (is_false(v0) != is_false(v1)) ? true_v<T> : false_v<T>;
    }
    else
    {
        return v0 ^ v1;
    }
}

template <typename T>
inline T xnor_opr(const T v0, const T v1)
{
    details::validate_supported_numeric_type<T>();

    const bool v0_true = is_true(v0);
    const bool v1_true = is_true(v1);

    if ((v0_true && v1_true) || (!v0_true && !v1_true))
    {
        return true_v<T>;
    }
    else
    {
        return false_v<T>;
    }
}

template <typename T>
inline bool is_integer(const T v)
{
    details::validate_supported_numeric_type<T>();

    if constexpr (details::is_supported_real_type_v<T>)
    {
        return std::equal_to<T>()(T(0), std::fmod(v, T(1)));
    }
    else
    {
        static_cast<void>(v);
        return true;
    }
}

template <typename T, unsigned int N>
struct fast_exp
{
    static inline T result(T v)
    {
        unsigned int k = N;
        T l = T(1);

        while (k)
        {
            if (1 == (k % 2))
            {
                l *= v;
                --k;
            }

            v *= v;
            k /= 2;
        }

        return l;
    }
};

template <typename T>
struct fast_exp<T, 10>
{
    static inline T result(const T v)
    {
        T v_5 = fast_exp<T, 5>::result(v);
        return v_5 * v_5;
    }
};
template <typename T>
struct fast_exp<T, 9>
{
    static inline T result(const T v)
    {
        return fast_exp<T, 8>::result(v) * v;
    }
};
template <typename T>
struct fast_exp<T, 8>
{
    static inline T result(const T v)
    {
        T v_4 = fast_exp<T, 4>::result(v);
        return v_4 * v_4;
    }
};
template <typename T>
struct fast_exp<T, 7>
{
    static inline T result(const T v)
    {
        return fast_exp<T, 6>::result(v) * v;
    }
};
template <typename T>
struct fast_exp<T, 6>
{
    static inline T result(const T v)
    {
        T v_3 = fast_exp<T, 3>::result(v);
        return v_3 * v_3;
    }
};
template <typename T>
struct fast_exp<T, 5>
{
    static inline T result(const T v)
    {
        return fast_exp<T, 4>::result(v) * v;
    }
};
template <typename T>
struct fast_exp<T, 4>
{
    static inline T result(const T v)
    {
        T v_2 = v * v;
        return v_2 * v_2;
    }
};
template <typename T>
struct fast_exp<T, 3>
{
    static inline T result(const T v)
    {
        return v * v * v;
    }
};
template <typename T>
struct fast_exp<T, 2>
{
    static inline T result(const T v)
    {
        return v * v;
    }
};
template <typename T>
struct fast_exp<T, 1>
{
    static inline T result(const T v)
    {
        return v;
    }
};
template <typename T>
struct fast_exp<T, 0>
{
    static inline T result(const T)
    {
        return T(1);
    }
};

template <typename T>
inline T abs(const T v)
{
    details::validate_supported_numeric_type<T>();

    if constexpr (details::is_supported_real_type_v<T>)
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

template <typename T>
inline T acos(const T v)
{
    details::validate_supported_numeric_type<T>();
    if constexpr (details::is_supported_real_type_v<T>)
    {
        return std::acos(v);
    }
    else
    {
        static_cast<void>(v);
        return quiet_nan<T>();
    }
}

template <typename T>
inline T acosh(const T v)
{
    details::validate_supported_numeric_type<T>();
    if constexpr (details::is_supported_real_type_v<T>)
    {
        return std::acosh(v);
    }
    else
    {
        static_cast<void>(v);
        return quiet_nan<T>();
    }
}

template <typename T>
inline T asin(const T v)
{
    details::validate_supported_numeric_type<T>();
    if constexpr (details::is_supported_real_type_v<T>)
    {
        return std::asin(v);
    }
    else
    {
        static_cast<void>(v);
        return quiet_nan<T>();
    }
}

template <typename T>
inline T asinh(const T v)
{
    details::validate_supported_numeric_type<T>();
    if constexpr (details::is_supported_real_type_v<T>)
    {
        return std::asinh(v);
    }
    else
    {
        static_cast<void>(v);
        return quiet_nan<T>();
    }
}

template <typename T>
inline T atan(const T v)
{
    details::validate_supported_numeric_type<T>();
    if constexpr (details::is_supported_real_type_v<T>)
    {
        return std::atan(v);
    }
    else
    {
        static_cast<void>(v);
        return quiet_nan<T>();
    }
}

template <typename T>
inline T atanh(const T v)
{
    details::validate_supported_numeric_type<T>();
    if constexpr (details::is_supported_real_type_v<T>)
    {
        return std::atanh(v);
    }
    else
    {
        static_cast<void>(v);
        return quiet_nan<T>();
    }
}

template <typename T>
inline T ceil(const T v)
{
    details::validate_supported_numeric_type<T>();
    if constexpr (details::is_supported_real_type_v<T>)
    {
        return std::ceil(v);
    }
    else
    {
        return v;
    }
}

template <typename T>
inline T cos(const T v)
{
    details::validate_supported_numeric_type<T>();
    if constexpr (details::is_supported_real_type_v<T>)
    {
        return std::cos(v);
    }
    else
    {
        static_cast<void>(v);
        return quiet_nan<T>();
    }
}

template <typename T>
inline T cosh(const T v)
{
    details::validate_supported_numeric_type<T>();
    if constexpr (details::is_supported_real_type_v<T>)
    {
        return std::cosh(v);
    }
    else
    {
        static_cast<void>(v);
        return quiet_nan<T>();
    }
}

template <typename T>
inline T exp(const T v)
{
    details::validate_supported_numeric_type<T>();
    return std::exp(v);
}

template <typename T>
inline T expm1(const T v)
{
    details::validate_supported_numeric_type<T>();

    if constexpr (details::is_supported_real_type_v<T>)
    {
        return std::expm1(v);
    }
    else
    {
        return std::expm1(v);
    }
}

template <typename T>
inline T floor(const T v)
{
    details::validate_supported_numeric_type<T>();
    if constexpr (details::is_supported_real_type_v<T>)
    {
        return std::floor(v);
    }
    else
    {
        return v;
    }
}

template <typename T>
inline T log(const T v)
{
    details::validate_supported_numeric_type<T>();
    return std::log(v);
}

template <typename T>
inline T log10(const T v)
{
    details::validate_supported_numeric_type<T>();
    return std::log10(v);
}

template <typename T>
inline T log2(const T v)
{
    details::validate_supported_numeric_type<T>();
    return std::log(v) / T(log2_value);
}

template <typename T>
inline T log1p(const T v)
{
    details::validate_supported_numeric_type<T>();

    if constexpr (details::is_supported_real_type_v<T>)
    {
        return std::log1p(v);
    }
    else
    {
        return std::log1p(v);
    }
}

template <typename T>
inline T neg(const T v)
{
    details::validate_supported_numeric_type<T>();
    return -v;
}

template <typename T>
inline T pos(const T v)
{
    details::validate_supported_numeric_type<T>();
    return +v;
}

template <typename T>
inline T round(const T v)
{
    details::validate_supported_numeric_type<T>();

    if constexpr (details::is_supported_real_type_v<T>)
    {
        return ((v < T(0)) ? std::ceil(v - T(0.5)) : std::floor(v + T(0.5)));
    }
    else
    {
        return v;
    }
}

template <typename T>
inline T sin(const T v)
{
    details::validate_supported_numeric_type<T>();
    if constexpr (details::is_supported_real_type_v<T>)
    {
        return std::sin(v);
    }
    else
    {
        static_cast<void>(v);
        return quiet_nan<T>();
    }
}

template <typename T>
inline T sinc(const T v)
{
    details::validate_supported_numeric_type<T>();

    if constexpr (details::is_supported_real_type_v<T>)
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
        return static_cast<T>(sinc(static_cast<double>(v)));
    }
}

template <typename T>
inline T sinh(const T v)
{
    details::validate_supported_numeric_type<T>();
    if constexpr (details::is_supported_real_type_v<T>)
    {
        return std::sinh(v);
    }
    else
    {
        static_cast<void>(v);
        return quiet_nan<T>();
    }
}

template <typename T>
inline T sqrt(const T v)
{
    details::validate_supported_numeric_type<T>();
    return std::sqrt(v);
}

template <typename T>
inline T tan(const T v)
{
    details::validate_supported_numeric_type<T>();
    if constexpr (details::is_supported_real_type_v<T>)
    {
        return std::tan(v);
    }
    else
    {
        static_cast<void>(v);
        return quiet_nan<T>();
    }
}

template <typename T>
inline T tanh(const T v)
{
    details::validate_supported_numeric_type<T>();
    if constexpr (details::is_supported_real_type_v<T>)
    {
        return std::tanh(v);
    }
    else
    {
        static_cast<void>(v);
        return quiet_nan<T>();
    }
}

template <typename T>
inline T cot(const T v)
{
    details::validate_supported_numeric_type<T>();
    if constexpr (details::is_supported_real_type_v<T>)
    {
        return T(1) / std::tan(v);
    }
    else
    {
        static_cast<void>(v);
        return quiet_nan<T>();
    }
}

template <typename T>
inline T sec(const T v)
{
    details::validate_supported_numeric_type<T>();
    if constexpr (details::is_supported_real_type_v<T>)
    {
        return T(1) / std::cos(v);
    }
    else
    {
        static_cast<void>(v);
        return quiet_nan<T>();
    }
}

template <typename T>
inline T csc(const T v)
{
    details::validate_supported_numeric_type<T>();
    if constexpr (details::is_supported_real_type_v<T>)
    {
        return T(1) / std::sin(v);
    }
    else
    {
        static_cast<void>(v);
        return quiet_nan<T>();
    }
}

template <typename T>
inline T r2d(const T v)
{
    details::validate_supported_numeric_type<T>();
    if constexpr (details::is_supported_real_type_v<T>)
    {
        return (v * T(_180_pi));
    }
    else
    {
        static_cast<void>(v);
        return quiet_nan<T>();
    }
}

template <typename T>
inline T d2r(const T v)
{
    details::validate_supported_numeric_type<T>();
    if constexpr (details::is_supported_real_type_v<T>)
    {
        return (v * T(pi_180));
    }
    else
    {
        static_cast<void>(v);
        return quiet_nan<T>();
    }
}

template <typename T>
inline T d2g(const T v)
{
    details::validate_supported_numeric_type<T>();
    if constexpr (details::is_supported_real_type_v<T>)
    {
        return (v * T(10.0 / 9.0));
    }
    else
    {
        static_cast<void>(v);
        return quiet_nan<T>();
    }
}

template <typename T>
inline T g2d(const T v)
{
    details::validate_supported_numeric_type<T>();
    if constexpr (details::is_supported_real_type_v<T>)
    {
        return (v * T(9.0 / 10.0));
    }
    else
    {
        static_cast<void>(v);
        return quiet_nan<T>();
    }
}

template <typename T>
inline T notl(const T v)
{
    details::validate_supported_numeric_type<T>();

    if constexpr (details::is_supported_real_type_v<T>)
    {
        return is_true(v) ? false_v<T> : true_v<T>;
    }
    else
    {
        return is_false(v) ? true_v<T> : false_v<T>;
    }
}

template <typename T>
inline T sgn(const T v)
{
    details::validate_supported_numeric_type<T>();

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

template <typename T>
inline T erf(const T v)
{
    details::validate_supported_numeric_type<T>();

    if constexpr (details::is_supported_real_type_v<T>)
    {
        return std::erf(v);
    }
    else
    {
        return static_cast<T>(std::erf(static_cast<double>(v)));
    }
}

template <typename T>
inline T erfc(const T v)
{
    details::validate_supported_numeric_type<T>();

    if constexpr (details::is_supported_real_type_v<T>)
    {
        return std::erfc(v);
    }
    else
    {
        return static_cast<T>(std::erfc(static_cast<double>(v)));
    }
}

template <typename T>
inline T ncdf(const T v)
{
    details::validate_supported_numeric_type<T>();

    if constexpr (details::is_supported_real_type_v<T>)
    {
        return T(0.5) * erfc(-(v / T(sqrt2)));
    }
    else
    {
        return static_cast<T>(ncdf(static_cast<double>(v)));
    }
}

template <typename T>
inline T frac(const T v)
{
    details::validate_supported_numeric_type<T>();

    if constexpr (details::is_supported_real_type_v<T>)
    {
        return (v - trunc(v));
    }
    else
    {
        return T(0);
    }
}

template <typename T>
inline T trunc(const T v)
{
    details::validate_supported_numeric_type<T>();

    if constexpr (details::is_supported_real_type_v<T>)
    {
        return std::trunc(v);
    }
    else
    {
        return v;
    }
}

template <typename T>
inline T compute_pow10(T d, const int exponent)
{
    static constexpr double fract10[] = {
        0.0,      1.0E+001, 1.0E+002, 1.0E+003, 1.0E+004, 1.0E+005, 1.0E+006, 1.0E+007, 1.0E+008,
        1.0E+009, 1.0E+010, 1.0E+011, 1.0E+012, 1.0E+013, 1.0E+014, 1.0E+015, 1.0E+016, 1.0E+017,
        1.0E+018, 1.0E+019, 1.0E+020, 1.0E+021, 1.0E+022, 1.0E+023, 1.0E+024, 1.0E+025, 1.0E+026,
        1.0E+027, 1.0E+028, 1.0E+029, 1.0E+030, 1.0E+031, 1.0E+032, 1.0E+033, 1.0E+034, 1.0E+035,
        1.0E+036, 1.0E+037, 1.0E+038, 1.0E+039, 1.0E+040, 1.0E+041, 1.0E+042, 1.0E+043, 1.0E+044,
        1.0E+045, 1.0E+046, 1.0E+047, 1.0E+048, 1.0E+049, 1.0E+050, 1.0E+051, 1.0E+052, 1.0E+053,
        1.0E+054, 1.0E+055, 1.0E+056, 1.0E+057, 1.0E+058, 1.0E+059, 1.0E+060, 1.0E+061, 1.0E+062,
        1.0E+063, 1.0E+064, 1.0E+065, 1.0E+066, 1.0E+067, 1.0E+068, 1.0E+069, 1.0E+070, 1.0E+071,
        1.0E+072, 1.0E+073, 1.0E+074, 1.0E+075, 1.0E+076, 1.0E+077, 1.0E+078, 1.0E+079, 1.0E+080,
        1.0E+081, 1.0E+082, 1.0E+083, 1.0E+084, 1.0E+085, 1.0E+086, 1.0E+087, 1.0E+088, 1.0E+089,
        1.0E+090, 1.0E+091, 1.0E+092, 1.0E+093, 1.0E+094, 1.0E+095, 1.0E+096, 1.0E+097, 1.0E+098,
        1.0E+099, 1.0E+100, 1.0E+101, 1.0E+102, 1.0E+103, 1.0E+104, 1.0E+105, 1.0E+106, 1.0E+107,
        1.0E+108, 1.0E+109, 1.0E+110, 1.0E+111, 1.0E+112, 1.0E+113, 1.0E+114, 1.0E+115, 1.0E+116,
        1.0E+117, 1.0E+118, 1.0E+119, 1.0E+120, 1.0E+121, 1.0E+122, 1.0E+123, 1.0E+124, 1.0E+125,
        1.0E+126, 1.0E+127, 1.0E+128, 1.0E+129, 1.0E+130, 1.0E+131, 1.0E+132, 1.0E+133, 1.0E+134,
        1.0E+135, 1.0E+136, 1.0E+137, 1.0E+138, 1.0E+139, 1.0E+140, 1.0E+141, 1.0E+142, 1.0E+143,
        1.0E+144, 1.0E+145, 1.0E+146, 1.0E+147, 1.0E+148, 1.0E+149, 1.0E+150, 1.0E+151, 1.0E+152,
        1.0E+153, 1.0E+154, 1.0E+155, 1.0E+156, 1.0E+157, 1.0E+158, 1.0E+159, 1.0E+160, 1.0E+161,
        1.0E+162, 1.0E+163, 1.0E+164, 1.0E+165, 1.0E+166, 1.0E+167, 1.0E+168, 1.0E+169, 1.0E+170,
        1.0E+171, 1.0E+172, 1.0E+173, 1.0E+174, 1.0E+175, 1.0E+176, 1.0E+177, 1.0E+178, 1.0E+179,
        1.0E+180, 1.0E+181, 1.0E+182, 1.0E+183, 1.0E+184, 1.0E+185, 1.0E+186, 1.0E+187, 1.0E+188,
        1.0E+189, 1.0E+190, 1.0E+191, 1.0E+192, 1.0E+193, 1.0E+194, 1.0E+195, 1.0E+196, 1.0E+197,
        1.0E+198, 1.0E+199, 1.0E+200, 1.0E+201, 1.0E+202, 1.0E+203, 1.0E+204, 1.0E+205, 1.0E+206,
        1.0E+207, 1.0E+208, 1.0E+209, 1.0E+210, 1.0E+211, 1.0E+212, 1.0E+213, 1.0E+214, 1.0E+215,
        1.0E+216, 1.0E+217, 1.0E+218, 1.0E+219, 1.0E+220, 1.0E+221, 1.0E+222, 1.0E+223, 1.0E+224,
        1.0E+225, 1.0E+226, 1.0E+227, 1.0E+228, 1.0E+229, 1.0E+230, 1.0E+231, 1.0E+232, 1.0E+233,
        1.0E+234, 1.0E+235, 1.0E+236, 1.0E+237, 1.0E+238, 1.0E+239, 1.0E+240, 1.0E+241, 1.0E+242,
        1.0E+243, 1.0E+244, 1.0E+245, 1.0E+246, 1.0E+247, 1.0E+248, 1.0E+249, 1.0E+250, 1.0E+251,
        1.0E+252, 1.0E+253, 1.0E+254, 1.0E+255, 1.0E+256, 1.0E+257, 1.0E+258, 1.0E+259, 1.0E+260,
        1.0E+261, 1.0E+262, 1.0E+263, 1.0E+264, 1.0E+265, 1.0E+266, 1.0E+267, 1.0E+268, 1.0E+269,
        1.0E+270, 1.0E+271, 1.0E+272, 1.0E+273, 1.0E+274, 1.0E+275, 1.0E+276, 1.0E+277, 1.0E+278,
        1.0E+279, 1.0E+280, 1.0E+281, 1.0E+282, 1.0E+283, 1.0E+284, 1.0E+285, 1.0E+286, 1.0E+287,
        1.0E+288, 1.0E+289, 1.0E+290, 1.0E+291, 1.0E+292, 1.0E+293, 1.0E+294, 1.0E+295, 1.0E+296,
        1.0E+297, 1.0E+298, 1.0E+299, 1.0E+300, 1.0E+301, 1.0E+302, 1.0E+303, 1.0E+304, 1.0E+305,
        1.0E+306, 1.0E+307, 1.0E+308};

    static constexpr int fract10_size = static_cast<int>(std::size(fract10));

    const int e = std::abs(exponent);

    if (exponent >= std::numeric_limits<T>::min_exponent10)
    {
        if (e < fract10_size)
        {
            if (exponent > 0)
                return T(d * fract10[e]);
            else
                return T(d / fract10[e]);
        }
        else
            return T(d * std::pow(10.0, 10.0 * exponent));
    }
    else
    {
        d /= T(fract10[-std::numeric_limits<T>::min_exponent10]);
        return T(d / fract10[-exponent + std::numeric_limits<T>::min_exponent10]);
    }
}

template <typename Iterator, typename T>
inline bool string_to_type_converter_impl_ref(Iterator& itr, const Iterator end, T& result)
{
    if (itr == end)
        return false;

    const bool negative = ('-' == (*itr));

    if (negative || ('+' == (*itr)))
    {
        if (end == ++itr)
            return false;
    }

    static constexpr ::math_expr::core::uchar_t zero = static_cast<::math_expr::core::uchar_t>('0');

    while ((end != itr) && (zero == (*itr))) ++itr;

    bool return_result = true;
    unsigned int digit = 0;
    const std::size_t length = static_cast<std::size_t>(std::distance(itr, end));

    if (length <= 4)
    {
        switch (length)
        {
#ifdef math_expr_use_lut

#define MATH_EXPR_PROCESS_DIGIT                           \
    if ((digit = details::digit_table[(int)*itr++]) < 10) \
        result = result * 10 + (digit);                   \
    else                                                  \
    {                                                     \
        return_result = false;                            \
        break;                                            \
    }                                                     \
    [[fallthrough]];

#else

#define MATH_EXPR_PROCESS_DIGIT          \
    if ((digit = (*itr++ - zero)) < 10)  \
        result = result * T(10) + digit; \
    else                                 \
    {                                    \
        return_result = false;           \
        break;                           \
    }                                    \
    [[fallthrough]];

#endif

            case 4:
                MATH_EXPR_PROCESS_DIGIT;
            case 3:
                MATH_EXPR_PROCESS_DIGIT;
            case 2:
                MATH_EXPR_PROCESS_DIGIT;
            case 1:
                if ((digit = (*itr - zero)) >= 10)
                {
                    digit = 0;
                    return_result = false;
                }

#undef MATH_EXPR_PROCESS_DIGIT
        }
    }
    else
        return_result = false;

    if (length && return_result)
    {
        result = result * 10 + static_cast<T>(digit);
        ++itr;
    }

    result = negative ? -result : result;
    return return_result;
}

template <typename Iterator, typename T>
static inline bool parse_nan(Iterator& itr, const Iterator end, T& t)
{
    using type = typename std::iterator_traits<Iterator>::value_type;

    static constexpr std::size_t nan_length = 3;

    if (std::distance(itr, end) != static_cast<int>(nan_length))
        return false;

    if (static_cast<type>('n') == (*itr))
    {
        if ((static_cast<type>('a') != *(itr + 1)) || (static_cast<type>('n') != *(itr + 2)))
        {
            return false;
        }
    }
    else if ((static_cast<type>('A') != *(itr + 1)) || (static_cast<type>('N') != *(itr + 2)))
    {
        return false;
    }

    t = std::numeric_limits<T>::quiet_NaN();

    return true;
}

template <typename Iterator, typename T>
static inline bool parse_inf(Iterator& itr, const Iterator end, T& t, const bool negative)
{
    static constexpr ::math_expr::core::char_t inf_uc[] = "INFINITY";
    static constexpr ::math_expr::core::char_t inf_lc[] = "infinity";
    static constexpr std::size_t inf_length = 8;

    const std::size_t length = static_cast<std::size_t>(std::distance(itr, end));

    if ((3 != length) && (inf_length != length))
        return false;

    ::math_expr::core::char_cptr inf_itr = ('i' == (*itr)) ? inf_lc : inf_uc;

    while (end != itr)
    {
        if (*inf_itr == static_cast<::math_expr::core::char_t>(*itr))
        {
            ++itr;
            ++inf_itr;
            continue;
        }
        else
            return false;
    }

    if (negative)
        t = -std::numeric_limits<T>::infinity();
    else
        t = std::numeric_limits<T>::infinity();

    return true;
}

template <typename T>
inline bool valid_exponent(const int exponent)
{
    ::math_expr::core::numeric::details::validate_supported_real_type<T>();
    return (std::numeric_limits<T>::min_exponent10 <= exponent) &&
           (exponent <= std::numeric_limits<T>::max_exponent10);
}

template <typename Iterator, typename T>
inline bool string_to_real(Iterator& itr_external, const Iterator end, T& t)
{
    ::math_expr::core::numeric::details::validate_supported_real_type<T>();

    if (end == itr_external)
        return false;

    Iterator itr = itr_external;

    T d = T(0);

    const bool negative = ('-' == (*itr));

    if (negative || '+' == (*itr))
    {
        if (end == ++itr)
            return false;
    }

    bool instate = false;

    static constexpr ::math_expr::core::char_t zero = static_cast<::math_expr::core::uchar_t>('0');

#define PARSE_DIGIT_1(d)              \
    if ((digit = (*itr - zero)) < 10) \
    {                                 \
        d = d * T(10) + digit;        \
    }                                 \
    else                              \
    {                                 \
        break;                        \
    }                                 \
    if (end == ++itr)                 \
        break;

#define PARSE_DIGIT_2(d)              \
    if ((digit = (*itr - zero)) < 10) \
    {                                 \
        d = d * T(10) + digit;        \
    }                                 \
    else                              \
    {                                 \
        break;                        \
    }                                 \
    ++itr;

    if ('.' != (*itr))
    {
        const Iterator curr = itr;

        while ((end != itr) && (zero == (*itr))) ++itr;

        while (end != itr)
        {
            unsigned int digit;
            PARSE_DIGIT_1(d);
            PARSE_DIGIT_1(d);
            PARSE_DIGIT_2(d);
        }

        if (curr != itr)
            instate = true;
    }

    int exponent = 0;

    if (end != itr)
    {
        if ('.' == (*itr))
        {
            const Iterator curr = ++itr;
            T tmp_d = T(0);

            while (end != itr)
            {
                unsigned int digit;
                PARSE_DIGIT_1(tmp_d);
                PARSE_DIGIT_1(tmp_d);
                PARSE_DIGIT_2(tmp_d);
            }

            if (curr != itr)
            {
                instate = true;

                const int frac_exponent = static_cast<int>(-std::distance(curr, itr));

                if (!valid_exponent<T>(frac_exponent))
                    return false;

                d += compute_pow10(tmp_d, frac_exponent);
            }

#undef PARSE_DIGIT_1
#undef PARSE_DIGIT_2
        }

        if (end != itr)
        {
            typename std::iterator_traits<Iterator>::value_type c = (*itr);

            if (('e' == c) || ('E' == c))
            {
                int exp = 0;

                if (!string_to_type_converter_impl_ref(++itr, end, exp))
                {
                    if (end == itr)
                        return false;
                    else
                        c = (*itr);
                }

                exponent += exp;
            }

            if (end != itr)
            {
                if (('f' == c) || ('F' == c) || ('l' == c) || ('L' == c))
                    ++itr;
                else if ('#' == c)
                {
                    if (end == ++itr)
                        return false;

                    if (('I' <= (*itr)) && ((*itr) <= 'n'))
                    {
                        if (('i' == (*itr)) || ('I' == (*itr)))
                        {
                            return parse_inf(itr, end, t, negative);
                        }
                        else if (('n' == (*itr)) || ('N' == (*itr)))
                        {
                            return parse_nan(itr, end, t);
                        }
                        else
                            return false;
                    }
                    else
                        return false;
                }
                else if (('I' <= (*itr)) && ((*itr) <= 'n'))
                {
                    if (('i' == (*itr)) || ('I' == (*itr)))
                    {
                        return parse_inf(itr, end, t, negative);
                    }
                    else if (('n' == (*itr)) || ('N' == (*itr)))
                    {
                        return parse_nan(itr, end, t);
                    }
                    else
                        return false;
                }
                else
                    return false;
            }
        }
    }

    if ((end != itr) || (!instate))
        return false;
    else if (!valid_exponent<T>(exponent))
        return false;
    else if (exponent)
        d = compute_pow10(d, exponent);

    t = static_cast<T>((negative) ? -d : d);
    return true;
}

template <typename T>
inline bool string_to_real(const std::string& s, T& t)
{
    ::math_expr::core::char_cptr begin = s.data();
    ::math_expr::core::char_cptr end = s.data() + s.size();

    return string_to_real(begin, end, t);
}

template <typename T>
struct functor_t
{
    /*
       Note: The following definitions for Type, may require tweaking
             based on the compiler and target architecture. The benchmark
             should provide enough information to make the right choice.
    */
    // using Type = T;
    // using Type = const T;
    using Type = const T&;
    using RefType = T&;
    using qfunc_t = T (*)(Type t0, Type t1, Type t2, Type t3);
    using tfunc_t = T (*)(Type t0, Type t1, Type t2);
    using bfunc_t = T (*)(Type t0, Type t1);
    using ufunc_t = T (*)(Type t0);
};

}  // namespace math_expr::core::numeric

#endif
