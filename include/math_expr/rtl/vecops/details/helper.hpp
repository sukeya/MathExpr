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

#ifndef MATH_EXPR_RTL_VECOPS_HELPER_HPP
#define MATH_EXPR_RTL_VECOPS_HELPER_HPP

#include "math_expr/core/std_includes.hpp"
#include "math_expr/core/macros.hpp"
#include "math_expr/igeneric_function.hpp"
#include "math_expr/symbol_table.hpp"

namespace math_expr::rtl::vecops::details
{
template <typename Vector>
inline bool invalid_range(const Vector& v, const std::size_t r0, const std::size_t r1)
{
    if (r0 > (v.size() - 1))
        return true;
    else if (r1 > (v.size() - 1))
        return true;
    else if (r1 < r0)
        return true;
    else
        return false;
}

template <typename T>
struct load_vector_range
{
    using igfun_t = typename math_expr::igeneric_function<T>;
    using parameter_list_t = typename igfun_t::parameter_list_t;
    using generic_type = typename igfun_t::generic_type;
    using scalar_t = typename generic_type::scalar_view;
    using vector_t = typename generic_type::vector_view;

    static inline bool process(parameter_list_t& parameters, std::size_t& r0, std::size_t& r1,
                               const std::size_t& r0_prmidx, const std::size_t& r1_prmidx,
                               const std::size_t vec_idx = 0)
    {
        if (r0_prmidx >= parameters.size())
            return false;

        if (r1_prmidx >= parameters.size())
            return false;

        if (!scalar_t(parameters[r0_prmidx]).to_uint(r0))
            return false;

        if (!scalar_t(parameters[r1_prmidx]).to_uint(r1))
            return false;

        return !invalid_range(vector_t(parameters[vec_idx]), r0, r1);
    }
};

template <typename T>
inline void kahan_sum(T& sum, T& error, const T v)
{
    const T x = v - error;
    const T y = sum + x;
    error = (y - sum) - x;
    sum = y;
}
}  // namespace math_expr::rtl::vecops::details

#endif
