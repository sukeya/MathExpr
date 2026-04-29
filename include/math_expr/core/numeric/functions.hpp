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

#ifndef MATH_EXPR_CORE_NUMERIC_FUNCTIONS_HPP
#define MATH_EXPR_CORE_NUMERIC_FUNCTIONS_HPP

#include "math_expr/core/std_includes.hpp"
#include "math_expr/core/types.hpp"
#include "math_expr/core/numeric/constants.hpp"

namespace math_expr::core::numeric
{
template <typename T> struct set_zero_value_impl
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

#define pod_set_zero_value(T)                                                                      \
    template <> struct set_zero_value_impl<T>                                                      \
    {                                                                                              \
        static inline void process(T* base_ptr, const std::size_t size)                            \
        {                                                                                          \
            std::memset(base_ptr, 0x00, size * sizeof(T));                                         \
        }                                                                                          \
    };

pod_set_zero_value(float) pod_set_zero_value(double) pod_set_zero_value(long double)

#ifdef pod_set_zero_value
#undef pod_set_zero_value
#endif

    template <typename T>
    inline void set_zero_value(T* data, const std::size_t size)
{
    set_zero_value_impl<T>::process(data, size);
}

template <typename T> inline void set_zero_value(std::vector<T>& v)
{
    set_zero_value(v.data(), v.size());
}

} // namespace math_expr::core::numeric

#include "math_expr/core/numeric/details/operations.hpp"

namespace math_expr::core::numeric
{
template <typename Type> struct numeric_info
{
    enum
    {
        length = 0,
        size = 32,
        bound_length = 0,
        min_exp = 0,
        max_exp = 0
    };
};

template <> struct numeric_info<int>
{
    enum
    {
        length = 10,
        size = 16,
        bound_length = 9
    };
};
template <> struct numeric_info<float>
{
    enum
    {
        min_exp = -38,
        max_exp = +38
    };
};
template <> struct numeric_info<double>
{
    enum
    {
        min_exp = -308,
        max_exp = +308
    };
};
template <> struct numeric_info<long double>
{
    enum
    {
        min_exp = -308,
        max_exp = +308
    };
};

template <typename T> inline int to_int32(const T v)
{
    return details::to_int32_impl(v);
}

template <typename T> inline ::math_expr::core::_int64_t to_int64(const T v)
{
    return details::to_int64_impl(v);
}

template <typename T> inline ::math_expr::core::_uint64_t to_uint64(const T v)
{
    return details::to_uint64_impl(v);
}

template <typename T> inline bool is_nan(const T v)
{
    return details::is_nan_impl(v);
}

template <typename T> inline T min(const T v0, const T v1)
{
    return details::min_impl(v0, v1);
}

template <typename T> inline T max(const T v0, const T v1)
{
    return details::max_impl(v0, v1);
}

template <typename T> inline T equal(const T v0, const T v1)
{
    return details::equal_impl(v0, v1);
}

template <typename T> inline T nequal(const T v0, const T v1)
{
    return details::nequal_impl(v0, v1);
}

template <typename T> inline T modulus(const T v0, const T v1)
{
    return details::modulus_impl(v0, v1);
}

template <typename T> inline T pow(const T v0, const T v1)
{
    return details::pow_impl(v0, v1);
}

template <typename T> inline T logn(const T v0, const T v1)
{
    return details::logn_impl(v0, v1);
}

template <typename T> inline T root(const T v0, const T v1)
{
    return details::root_impl(v0, v1);
}

template <typename T> inline T roundn(const T v0, const T v1)
{
    return details::roundn_impl(v0, v1);
}

template <typename T> inline T hypot(const T v0, const T v1)
{
    return details::hypot_impl(v0, v1);
}

template <typename T> inline T atan2(const T v0, const T v1)
{
    return details::atan2_impl(v0, v1);
}

template <typename T> inline T shr(const T v0, const T v1)
{
    return details::shr_impl(v0, v1);
}

template <typename T> inline T shl(const T v0, const T v1)
{
    return details::shl_impl(v0, v1);
}

template <typename T> inline T and_opr(const T v0, const T v1)
{
    return details::and_impl(v0, v1);
}

template <typename T> inline T nand_opr(const T v0, const T v1)
{
    return details::nand_impl(v0, v1);
}

template <typename T> inline T or_opr(const T v0, const T v1)
{
    return details::or_impl(v0, v1);
}

template <typename T> inline T nor_opr(const T v0, const T v1)
{
    return details::nor_impl(v0, v1);
}

template <typename T> inline T xor_opr(const T v0, const T v1)
{
    return details::xor_impl(v0, v1);
}

template <typename T> inline T xnor_opr(const T v0, const T v1)
{
    return details::xnor_impl(v0, v1);
}

template <typename T> inline bool is_integer(const T v)
{
    return details::is_integer_impl(v);
}

template <typename T, unsigned int N> struct fast_exp
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

template <typename T> struct fast_exp<T, 10>
{
    static inline T result(const T v)
    {
        T v_5 = fast_exp<T, 5>::result(v);
        return v_5 * v_5;
    }
};
template <typename T> struct fast_exp<T, 9>
{
    static inline T result(const T v)
    {
        return fast_exp<T, 8>::result(v) * v;
    }
};
template <typename T> struct fast_exp<T, 8>
{
    static inline T result(const T v)
    {
        T v_4 = fast_exp<T, 4>::result(v);
        return v_4 * v_4;
    }
};
template <typename T> struct fast_exp<T, 7>
{
    static inline T result(const T v)
    {
        return fast_exp<T, 6>::result(v) * v;
    }
};
template <typename T> struct fast_exp<T, 6>
{
    static inline T result(const T v)
    {
        T v_3 = fast_exp<T, 3>::result(v);
        return v_3 * v_3;
    }
};
template <typename T> struct fast_exp<T, 5>
{
    static inline T result(const T v)
    {
        return fast_exp<T, 4>::result(v) * v;
    }
};
template <typename T> struct fast_exp<T, 4>
{
    static inline T result(const T v)
    {
        T v_2 = v * v;
        return v_2 * v_2;
    }
};
template <typename T> struct fast_exp<T, 3>
{
    static inline T result(const T v)
    {
        return v * v * v;
    }
};
template <typename T> struct fast_exp<T, 2>
{
    static inline T result(const T v)
    {
        return v * v;
    }
};
template <typename T> struct fast_exp<T, 1>
{
    static inline T result(const T v)
    {
        return v;
    }
};
template <typename T> struct fast_exp<T, 0>
{
    static inline T result(const T)
    {
        return T(1);
    }
};

#define math_expr_define_unary_function(FunctionName)                                              \
    template <typename T> inline T FunctionName(const T v)                                         \
    {                                                                                              \
        return details::FunctionName##_impl(v);                                                    \
    }

math_expr_define_unary_function(abs) math_expr_define_unary_function(
    acos) math_expr_define_unary_function(acosh) math_expr_define_unary_function(asin)
    math_expr_define_unary_function(asinh) math_expr_define_unary_function(
        atan) math_expr_define_unary_function(atanh) math_expr_define_unary_function(ceil)
        math_expr_define_unary_function(cos) math_expr_define_unary_function(
            cosh) math_expr_define_unary_function(exp) math_expr_define_unary_function(expm1)
            math_expr_define_unary_function(floor) math_expr_define_unary_function(
                log) math_expr_define_unary_function(log10) math_expr_define_unary_function(log2)
                math_expr_define_unary_function(log1p) math_expr_define_unary_function(
                    neg) math_expr_define_unary_function(pos) math_expr_define_unary_function(round)
                    math_expr_define_unary_function(sin) math_expr_define_unary_function(
                        sinc) math_expr_define_unary_function(sinh)
                        math_expr_define_unary_function(sqrt) math_expr_define_unary_function(
                            tan) math_expr_define_unary_function(tanh)
                            math_expr_define_unary_function(cot) math_expr_define_unary_function(
                                sec) math_expr_define_unary_function(csc)
                                math_expr_define_unary_function(
                                    r2d) math_expr_define_unary_function(d2r)
                                    math_expr_define_unary_function(
                                        d2g) math_expr_define_unary_function(g2d)
                                        math_expr_define_unary_function(
                                            notl) math_expr_define_unary_function(sgn)
                                            math_expr_define_unary_function(erf)
                                                math_expr_define_unary_function(erfc)
                                                    math_expr_define_unary_function(ncdf)
                                                        math_expr_define_unary_function(frac)
                                                            math_expr_define_unary_function(trunc)
#undef math_expr_define_unary_function
                                                                template <typename T>
                                                                inline T
    compute_pow10(T d, const int exponent)
{
    static const double fract10[] = {
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

    static const int fract10_size = static_cast<int>(sizeof(fract10) / sizeof(double));

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

    static const ::math_expr::core::uchar_t zero = static_cast<::math_expr::core::uchar_t>('0');

    while ((end != itr) && (zero == (*itr)))
        ++itr;

    bool return_result = true;
    unsigned int digit = 0;
    const std::size_t length = static_cast<std::size_t>(std::distance(itr, end));

    if (length <= 4)
    {
        switch (length)
        {
#ifdef math_expr_use_lut

#define math_expr_process_digit                                                                    \
    if ((digit = details::digit_table[(int)*itr++]) < 10)                                          \
        result = result * 10 + (digit);                                                            \
    else                                                                                           \
    {                                                                                              \
        return_result = false;                                                                     \
        break;                                                                                     \
    }                                                                                              \
    [[fallthrough]];

#else

#define math_expr_process_digit                                                                    \
    if ((digit = (*itr++ - zero)) < 10)                                                            \
        result = result * T(10) + digit;                                                           \
    else                                                                                           \
    {                                                                                              \
        return_result = false;                                                                     \
        break;                                                                                     \
    }                                                                                              \
    [[fallthrough]];

#endif

        case 4:
        math_expr_process_digit case 3:
        math_expr_process_digit case 2:
        math_expr_process_digit case 1:
            if ((digit = (*itr - zero)) >= 10)
            {
                digit = 0;
                return_result = false;
            }

#undef math_expr_process_digit
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
    typedef typename std::iterator_traits<Iterator>::value_type type;

    static const std::size_t nan_length = 3;

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
    static const ::math_expr::core::char_t inf_uc[] = "INFINITY";
    static const ::math_expr::core::char_t inf_lc[] = "infinity";
    static const std::size_t inf_length = 8;

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

template <typename T> inline bool valid_exponent(const int exponent)
{
    ::math_expr::core::numeric::details::validate_supported_real_type<T>();
    return (numeric_info<T>::min_exp <= exponent) && (exponent <= numeric_info<T>::max_exp);
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

    static const ::math_expr::core::char_t zero = static_cast<::math_expr::core::uchar_t>('0');

#define parse_digit_1(d)                                                                           \
    if ((digit = (*itr - zero)) < 10)                                                              \
    {                                                                                              \
        d = d * T(10) + digit;                                                                     \
    }                                                                                              \
    else                                                                                           \
    {                                                                                              \
        break;                                                                                     \
    }                                                                                              \
    if (end == ++itr)                                                                              \
        break;

#define parse_digit_2(d)                                                                           \
    if ((digit = (*itr - zero)) < 10)                                                              \
    {                                                                                              \
        d = d * T(10) + digit;                                                                     \
    }                                                                                              \
    else                                                                                           \
    {                                                                                              \
        break;                                                                                     \
    }                                                                                              \
    ++itr;

    if ('.' != (*itr))
    {
        const Iterator curr = itr;

        while ((end != itr) && (zero == (*itr)))
            ++itr;

        while (end != itr)
        {
            unsigned int digit;
            parse_digit_1(d) parse_digit_1(d) parse_digit_2(d)
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
                parse_digit_1(tmp_d) parse_digit_1(tmp_d) parse_digit_2(tmp_d)
            }

            if (curr != itr)
            {
                instate = true;

                const int frac_exponent = static_cast<int>(-std::distance(curr, itr));

                if (!valid_exponent<T>(frac_exponent))
                    return false;

                d += compute_pow10(tmp_d, frac_exponent);
            }

#undef parse_digit_1
#undef parse_digit_2
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

template <typename T> inline bool string_to_real(const std::string& s, T& t)
{
    ::math_expr::core::char_cptr begin = s.data();
    ::math_expr::core::char_cptr end = s.data() + s.size();

    return string_to_real(begin, end, t);
}

template <typename T> struct functor_t
{
    /*
       Note: The following definitions for Type, may require tweaking
             based on the compiler and target architecture. The benchmark
             should provide enough information to make the right choice.
    */
    // typedef T Type;
    // typedef const T Type;
    typedef const T& Type;
    typedef T& RefType;
    typedef T (*qfunc_t)(Type t0, Type t1, Type t2, Type t3);
    typedef T (*tfunc_t)(Type t0, Type t1, Type t2);
    typedef T (*bfunc_t)(Type t0, Type t1);
    typedef T (*ufunc_t)(Type t0);
};

} // namespace math_expr::core::numeric

#endif
