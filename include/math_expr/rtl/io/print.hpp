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

#ifndef MATH_EXPR_RTL_IO_PRINT_HPP
#define MATH_EXPR_RTL_IO_PRINT_HPP

#include "math_expr/core/std_includes.hpp"
#include "math_expr/core/macros.hpp"
#include "math_expr/core/numeric.hpp"
#include "math_expr/igeneric_function.hpp"
#include "math_expr/symbol_table.hpp"

namespace math_expr::rtl::io
{
namespace details
{
template <typename T> inline void print_type(const std::string& fmt, const T v)
{
    static_assert(math_expr::core::numeric::details::is_supported_real_type_v<T>,
                  "math_expr::rtl::io::print supports float, double, and long double only.");

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat-nonliteral"
#elif defined(__GNUC__) || defined(__GNUG__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#elif defined(_MSC_VER)
#endif

    printf(fmt.c_str(), v);

#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__) || defined(__GNUG__)
#pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#endif
}

template <typename T> struct print_impl
{
    typedef typename igeneric_function<T>::generic_type generic_type;
    typedef typename igeneric_function<T>::parameter_list_t parameter_list_t;
    typedef typename generic_type::scalar_view scalar_t;
    typedef typename generic_type::vector_view vector_t;
    typedef typename generic_type::string_view string_t;

    static void process(const std::string& scalar_format, parameter_list_t parameters)
    {
        for (std::size_t i = 0; i < parameters.size(); ++i)
        {
            generic_type& gt = parameters[i];

            switch (gt.type)
            {
            case generic_type::store_type::e_scalar:
                print(scalar_format, scalar_t(gt));
                break;

            case generic_type::store_type::e_vector:
                print(scalar_format, vector_t(gt));
                break;

            case generic_type::store_type::e_string:
                print(string_t(gt));
                break;

            default:
                continue;
            }
        }
    }

    static inline void print(const std::string& scalar_format, const scalar_t& s)
    {
        print_type(scalar_format, s());
    }

    static inline void print(const std::string& scalar_format, const vector_t& v)
    {
        for (std::size_t i = 0; i < v.size(); ++i)
        {
            print_type(scalar_format, v[i]);

            if ((i + 1) < v.size())
                printf(" ");
        }
    }

    static inline void print(const string_t& s)
    {
        printf("%s", to_str(s).c_str());
    }
};

} // namespace details

template <typename T> struct print final : public math_expr::igeneric_function<T>
{
    typedef typename igeneric_function<T>::parameter_list_t parameter_list_t;

    using math_expr::igeneric_function<T>::operator();

    explicit print(const std::string& scalar_format = "%10.5f") : scalar_format_(scalar_format)
    {
        math_expr::enable_zero_parameters(*this);
    }

    inline T operator()(parameter_list_t parameters) override
    {
        details::print_impl<T>::process(scalar_format_, parameters);
        return T(0);
    }

    std::string scalar_format_;
};

} // namespace math_expr::rtl::io
#endif
