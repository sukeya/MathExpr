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

#ifndef MATH_EXPR_IVARARG_FUNCTION_HPP
#define MATH_EXPR_IVARARG_FUNCTION_HPP

#include "math_expr/core/std_includes.hpp"
#include "math_expr/core/macros.hpp"
#include "math_expr/function_traits.hpp"

namespace math_expr
{
template <typename T>
class ivararg_function : public function_traits
{
   public:
    virtual ~ivararg_function() {}

    inline virtual T operator()(const std::vector<T>&)
    {
        core::debug_print("ivararg_function::operator() - Operator has not been overridden\n");
        return std::numeric_limits<T>::quiet_NaN();
    }
};

}  // namespace math_expr

#endif
