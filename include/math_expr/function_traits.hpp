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

#ifndef MATH_EXPR_FUNCTION_TRAITS_HPP
#define MATH_EXPR_FUNCTION_TRAITS_HPP

#include "math_expr/core/std_includes.hpp"

namespace math_expr
{
class function_traits
{
   public:
    function_traits()
        : allow_zero_parameters_(false),
          has_side_effects_(true),
          min_num_args_(0),
          max_num_args_(std::numeric_limits<std::size_t>::max())
    {
    }

    inline bool& allow_zero_parameters()
    {
        return allow_zero_parameters_;
    }

    inline bool& has_side_effects()
    {
        return has_side_effects_;
    }

    std::size_t& min_num_args()
    {
        return min_num_args_;
    }

    std::size_t& max_num_args()
    {
        return max_num_args_;
    }

   private:
    bool allow_zero_parameters_;
    bool has_side_effects_;
    std::size_t min_num_args_;
    std::size_t max_num_args_;
};

template <typename FunctionType>
void enable_zero_parameters(FunctionType& func)
{
    func.allow_zero_parameters() = true;

    if (0 != func.min_num_args())
    {
        func.min_num_args() = 0;
    }
}

template <typename FunctionType>
void disable_zero_parameters(FunctionType& func)
{
    func.allow_zero_parameters() = false;
}

template <typename FunctionType>
void enable_has_side_effects(FunctionType& func)
{
    func.has_side_effects() = true;
}

template <typename FunctionType>
void disable_has_side_effects(FunctionType& func)
{
    func.has_side_effects() = false;
}

template <typename FunctionType>
void set_min_num_args(FunctionType& func, const std::size_t& num_args)
{
    func.min_num_args() = num_args;

    if ((0 != func.min_num_args()) && func.allow_zero_parameters())
        func.allow_zero_parameters() = false;
}

template <typename FunctionType>
void set_max_num_args(FunctionType& func, const std::size_t& num_args)
{
    func.max_num_args() = num_args;
}

}  // namespace math_expr

#endif
