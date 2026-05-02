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

#ifndef MATH_EXPR_LOOP_RUNTIME_CHECK_HPP
#define MATH_EXPR_LOOP_RUNTIME_CHECK_HPP

#include "math_expr/core/std_includes.hpp"
#include "math_expr/core/types.hpp"
#include "math_expr/loop_types.hpp"

namespace math_expr
{
struct loop_runtime_check
{
    enum class violation_type
    {
        e_unknown = 0,
        e_iteration_count = 1,
        e_timeout = 2
    };

    loop_types loop_set;

    loop_runtime_check() : loop_set(loop_types::e_invalid()), max_loop_iterations(0) {}

    std::uint64_t max_loop_iterations;

    struct violation_context
    {
        loop_types loop;
        violation_type violation;
        std::uint64_t iteration_count;
    };

    virtual bool check()
    {
        return true;
    }

    virtual void handle_runtime_violation(const violation_context&)
    {
        throw std::runtime_error("ExprTk Loop runtime violation.");
    }

    virtual ~loop_runtime_check() {}
};

using loop_runtime_check_ptr = loop_runtime_check*;

}  // namespace math_expr

#endif
