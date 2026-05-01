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

#ifndef MATH_EXPR_RTL_VECOPS_SUMK_HPP
#define MATH_EXPR_RTL_VECOPS_SUMK_HPP

#include "math_expr/rtl/vecops/details/helper.hpp"

namespace math_expr::rtl::vecops
{
template <typename T>
class sumk final : public math_expr::igeneric_function<T>
{
   public:
    using igfun_t = typename math_expr::igeneric_function<T>;
    using parameter_list_t = typename igfun_t::parameter_list_t;
    using generic_type = typename igfun_t::generic_type;
    using scalar_t = typename generic_type::scalar_view;
    using vector_t = typename generic_type::vector_view;

    using igfun_t::operator();

    sumk() : math_expr::igeneric_function<T>("V|VTT|VTTT")
    /*
       Overloads:
       0. V    - vector
       1. VTT  - vector, r0, r1
       2. VTTT - vector, r0, r1, stride
    */
    {
    }

    inline T operator()(const std::size_t& ps_index, parameter_list_t parameters) override
    {
        const vector_t vec(parameters[0]);

        const std::size_t stride =
            (2 != ps_index) ? 1 : static_cast<std::size_t>(scalar_t(parameters[3])());

        std::size_t r0 = 0;
        std::size_t r1 = vec.size() - 1;

        if (((1 == ps_index) || (2 == ps_index)) &&
            !details::load_vector_range<T>::process(parameters, r0, r1, 1, 2, 0))
        {
            return std::numeric_limits<T>::quiet_NaN();
        }

        T result = T(0);
        T error = T(0);

        for (std::size_t i = r0; i <= r1; i += stride)
        {
            details::kahan_sum(result, error, vec[i]);
        }

        return result;
    }
};

}  // namespace math_expr::rtl::vecops

#endif
