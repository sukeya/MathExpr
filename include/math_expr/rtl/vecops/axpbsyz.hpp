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

#ifndef MATH_EXPR_RTL_VECOPS_AXPBSYZ_HPP
#define MATH_EXPR_RTL_VECOPS_AXPBSYZ_HPP

#include "math_expr/rtl/vecops/details/helper.hpp"

namespace math_expr::rtl::vecops
{
template <typename T> class axpbsyz final : public math_expr::igeneric_function<T>
{
  public:
    using igfun_t = typename math_expr::igeneric_function<T>;
    using parameter_list_t = typename igfun_t::parameter_list_t;
    using generic_type = typename igfun_t::generic_type;
    using scalar_t = typename generic_type::scalar_view;
    using vector_t = typename generic_type::vector_view;

    using igfun_t::operator();

    axpbsyz() : math_expr::igeneric_function<T>("TVTTVV|TVTTVVTT")
    /*
       z <- ax + by
       Overloads:
       0. TVTVV   - a, x(vector), b, shift, y(vector), z(vector)
       1. TVTVVTT - a, x(vector), b, shift, y(vector), z(vector), r0, r1
    */
    {
    }

    inline T operator()(const std::size_t& ps_index, parameter_list_t parameters) override
    {
        const vector_t x(parameters[1]);
        const vector_t y(parameters[4]);
        vector_t z(parameters[5]);

        std::size_t r0 = 0;
        std::size_t r1 = std::min(x.size(), y.size()) - 1;

        if ((1 == ps_index) && !details::load_vector_range<T>::process(parameters, r0, r1, 6, 7, 1))
            return std::numeric_limits<T>::quiet_NaN();
        else if (details::invalid_range(y, r0, r1))
            return std::numeric_limits<T>::quiet_NaN();
        else if (details::invalid_range(z, r0, r1))
            return std::numeric_limits<T>::quiet_NaN();

        const T a = scalar_t(parameters[0])();
        const T b = scalar_t(parameters[2])();

        const std::size_t s = static_cast<std::size_t>(scalar_t(parameters[3])());

        for (std::size_t i = r0; i <= r1; ++i)
        {
            z[i] = (a * x[i]) + (b * y[i + s]);
        }

        return T(1);
    }
};

} // namespace math_expr::rtl::vecops

#endif
