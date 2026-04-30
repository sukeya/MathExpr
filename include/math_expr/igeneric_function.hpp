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

#ifndef MATH_EXPR_IGENERIC_FUNCTION_HPP
#define MATH_EXPR_IGENERIC_FUNCTION_HPP

#include "math_expr/core/std_includes.hpp"
#include "math_expr/core/macros.hpp"
#include "math_expr/function_traits.hpp"
#include "math_expr/type_store.hpp"

namespace math_expr
{
template <typename T> class igeneric_function : public function_traits
{
  public:
    enum class return_type
    {
        e_rtrn_scalar = 0,
        e_rtrn_string = 1,
        e_rtrn_overload = 2
    };

    using type = T;
    using generic_type = type_store<T>;
    using parameter_list_t = typename generic_type::parameter_list;

    explicit igeneric_function(const std::string& param_seq = "",
                               const return_type rtr_type = return_type::e_rtrn_scalar)
        : parameter_sequence(param_seq), rtrn_type(rtr_type)
    {
    }

    virtual ~igeneric_function() {}

#define igeneric_function_empty_body(N)                                                            \
    {                                                                                              \
        math_expr_debug(                                                                           \
            ("igeneric_function::operator() - Operator(" #N ") has not been overridden\n"));       \
        return std::numeric_limits<T>::quiet_NaN();                                                \
    }

    // f(i_0,i_1,....,i_N) --> Scalar
    inline virtual T operator()(parameter_list_t) igeneric_function_empty_body(1)

        // f(i_0,i_1,....,i_N) --> String
        inline virtual T operator()(std::string&, parameter_list_t) igeneric_function_empty_body(2)

        // f(psi,i_0,i_1,....,i_N) --> Scalar
        inline virtual T operator()(const std::size_t&, parameter_list_t)
            igeneric_function_empty_body(3)

        // f(psi,i_0,i_1,....,i_N) --> String
        inline virtual T operator()(const std::size_t&, std::string&, parameter_list_t)
            igeneric_function_empty_body(4)

#undef igeneric_function_empty_body

                std::string parameter_sequence;
    return_type rtrn_type;

    static inline std::string generate_prefix_args(const std::string& prefix_args,
                                                   std::size_t start = 0, std::size_t end = 10)
    {
        std::string result;

        for (std::size_t i = start; i <= end; ++i)
        {
            result += prefix_args + std::string(i, '?');
            result += (i != end) ? "|" : "";
        }

        return result;
    }

    static inline std::string generate_suffix_args(const std::string& suffix_args,
                                                   std::size_t start = 0, std::size_t end = 10)
    {
        std::string result;

        for (std::size_t i = start; i <= end; ++i)
        {
            result += std::string(i, '?') + suffix_args;
            result += (i != end) ? "|" : "";
        }

        return result;
    }
};

} // namespace math_expr

#endif
