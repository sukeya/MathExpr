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

#ifndef MATH_EXPR_VECTOR_ACCESS_RUNTIME_CHECK_HPP
#define MATH_EXPR_VECTOR_ACCESS_RUNTIME_CHECK_HPP

#include "math_expr/core/std_includes.hpp"

namespace math_expr
{
struct vector_access_runtime_check
{
    struct violation_context
    {
        void* base_ptr;
        void* end_ptr;
        void* access_ptr;
        std::size_t type_size;
    };

    virtual ~vector_access_runtime_check() {}

    virtual bool handle_runtime_violation(violation_context& /*context*/)
    {
        throw std::runtime_error("ExprTk runtime vector access violation.");
#ifdef __NVCOMPILER
        return false;
#endif
    }
};

using vector_access_runtime_check_ptr = vector_access_runtime_check*;

}  // namespace math_expr

#endif
