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

#ifndef MATH_EXPR_RTL_VECOPS_COUNT_HPP
#define MATH_EXPR_RTL_VECOPS_COUNT_HPP

#include "math_expr/rtl/vecops/details/helper.hpp"

namespace math_expr::rtl::vecops
{
template <typename T> class count final : public math_expr::igeneric_function<T>
{
  public:
    typedef typename math_expr::igeneric_function<T> igfun_t;
    typedef typename igfun_t::parameter_list_t parameter_list_t;
    typedef typename igfun_t::generic_type generic_type;
    typedef typename generic_type::scalar_view scalar_t;
    typedef typename generic_type::vector_view vector_t;

    using igfun_t::operator();

    count() : math_expr::igeneric_function<T>("V|VTT|T*")
    /*
       Overloads:
       0. V   - vector
       1. VTT - vector, r0, r1
       2. T*  - T....T
    */
    {
    }

    inline T operator()(const std::size_t& ps_index, parameter_list_t parameters) override
    {
        std::size_t cnt = 0;

        if (2 == ps_index)
        {
            for (std::size_t i = 0; i < parameters.size(); ++i)
            {
                if (scalar_t(parameters[i])() != T(0))
                    ++cnt;
            }
        }
        else
        {
            const vector_t vec(parameters[0]);

            std::size_t r0 = 0;
            std::size_t r1 = vec.size() - 1;

            if ((1 == ps_index) &&
                !details::load_vector_range<T>::process(parameters, r0, r1, 1, 2, 0))
            {
                return std::numeric_limits<T>::quiet_NaN();
            }

            for (std::size_t i = r0; i <= r1; ++i)
            {
                if (vec[i] != T(0))
                    ++cnt;
            }
        }

        return T(cnt);
    }
};

} // namespace math_expr::rtl::vecops

#endif
