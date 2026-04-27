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

#ifndef MATH_EXPR_IFUNCTION_HPP
#define MATH_EXPR_IFUNCTION_HPP

namespace math_expr
{
   template <typename T>
   class ifunction : public function_traits
   {
   public:

      explicit ifunction(const std::size_t& pc)
      : param_count(pc)
      {}

      virtual ~ifunction()
      {}

      #define empty_method_body(N)                   \
      {                                              \
         math_expr_debug(("ifunction::operator() - Operator(" #N ") has not been overridden\n")); \
         return std::numeric_limits<T>::quiet_NaN(); \
      }                                              \

      inline virtual T operator() ()
      empty_method_body(0)

      inline virtual T operator() (const T&)
      empty_method_body(1)

      inline virtual T operator() (const T&,const T&)
      empty_method_body(2)

      inline virtual T operator() (const T&, const T&, const T&)
      empty_method_body(3)

      inline virtual T operator() (const T&, const T&, const T&, const T&)
      empty_method_body(4)

      inline virtual T operator() (const T&, const T&, const T&, const T&, const T&)
      empty_method_body(5)

      inline virtual T operator() (const T&, const T&, const T&, const T&, const T&, const T&)
      empty_method_body(6)

      inline virtual T operator() (const T&, const T&, const T&, const T&, const T&, const T&, const T&)
      empty_method_body(7)

      inline virtual T operator() (const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&)
      empty_method_body(8)

      inline virtual T operator() (const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&)
      empty_method_body(9)

      inline virtual T operator() (const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&)
      empty_method_body(10)

      inline virtual T operator() (const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&,
                                   const T&)
      empty_method_body(11)

      inline virtual T operator() (const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&,
                                   const T&, const T&)
      empty_method_body(12)

      inline virtual T operator() (const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&,
                                   const T&, const T&, const T&)
      empty_method_body(13)

      inline virtual T operator() (const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&,
                                   const T&, const T&, const T&, const T&)
      empty_method_body(14)

      inline virtual T operator() (const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&,
                                   const T&, const T&, const T&, const T&, const T&)
      empty_method_body(15)

      inline virtual T operator() (const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&,
                                   const T&, const T&, const T&, const T&, const T&, const T&)
      empty_method_body(16)

      inline virtual T operator() (const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&,
                                   const T&, const T&, const T&, const T&, const T&, const T&, const T&)
      empty_method_body(17)

      inline virtual T operator() (const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&,
                                   const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&)
      empty_method_body(18)

      inline virtual T operator() (const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&,
                                   const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&)
      empty_method_body(19)

      inline virtual T operator() (const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&,
                                   const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&)
      empty_method_body(20)

      #undef empty_method_body

      std::size_t param_count;
   };

} // namespace math_expr

#endif
