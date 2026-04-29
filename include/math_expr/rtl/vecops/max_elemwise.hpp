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

#ifndef MATH_EXPR_RTL_VECOPS_MAX_ELEMWISE_HPP
#define MATH_EXPR_RTL_VECOPS_MAX_ELEMWISE_HPP

#include "math_expr/rtl/vecops/details/helper.hpp"

namespace math_expr::rtl::vecops
{
   template <typename T>
   class max_elemwise final : public math_expr::igeneric_function<T>
   {
   public:

      typedef typename math_expr::igeneric_function<T> igfun_t;
      typedef typename igfun_t::parameter_list_t    parameter_list_t;
      typedef typename igfun_t::generic_type        generic_type;
      typedef typename generic_type::scalar_view    scalar_t;
      typedef typename generic_type::vector_view    vector_t;

      using igfun_t::operator();

      max_elemwise()
      : math_expr::igeneric_function<T>("VT|VVT|VTTT|VVTTT")
      /*
         Overloads:
         0. VT    - vector, T
         1. VVT   - vector, vector, T
         2. VTTT  - vector, r0, r1, T
         3. VVTTT - vector, vector, r0, r1, T
      */
      {}

      inline T operator() (const std::size_t& ps_index, parameter_list_t parameters) override
      {
         std::size_t out_vec_index = 0;
         std::size_t in_vec_index  = (ps_index & 1) ? 1 : 0;
         std::size_t scalar_index  = parameters.size() - 1;

         vector_t out_vec(parameters[out_vec_index]);
         vector_t in_vec (parameters[in_vec_index ]);

         const T s = scalar_t(parameters[scalar_index ])();

         std::size_t r0 = 0;
         std::size_t r1 = in_vec.size() - 1;

         if ((2 == ps_index) || (3 == ps_index))
         {
            std::size_t rng_idx0 = 0;
            std::size_t rng_idx1 = 0;

            switch (ps_index)
            {
               case 2 : { rng_idx0 = 1; rng_idx1 = 2; }; break;
               case 3 : { rng_idx0 = 2; rng_idx1 = 3; }; break;
            }

            if (!details::load_vector_range<T>::process(parameters, r0, r1, rng_idx0, rng_idx1, 0))
            {
               return T(0);
            }
         }

         for (std::size_t i = r0; i <= r1; ++i)
         {
            out_vec[i] = math_expr::core::numeric::max(in_vec[i], s);
         }

         return T(1);
      }
   };


} // namespace math_expr::rtl::vecops

#endif
