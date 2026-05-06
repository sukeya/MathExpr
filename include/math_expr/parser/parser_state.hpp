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

#ifndef MATH_EXPR_PARSER_PARSER_STATE_HPP
#define MATH_EXPR_PARSER_PARSER_STATE_HPP

#include "math_expr/core/std_includes.hpp"

namespace math_expr
{
struct parser_state
{
    parser_state() : type_check_enabled(true)
    {
        reset();
    }

    void reset()
    {
        parsing_return_stmt = false;
        parsing_break_stmt = false;
        parsing_assert_stmt = false;
        return_stmt_present = false;
        side_effect_present = false;
        scope_depth = 0;
        stack_depth = 0;
        parsing_loop_stmt_count = 0;
    }

    void activate_side_effect(const std::string& source)
    {
        if (!side_effect_present)
        {
            side_effect_present = true;

            if constexpr (::math_expr::core::build_options::kEnableDebugging)
            {
                core::debug_print("activate_side_effect() - caller: %s\n", source.c_str());
            }
        }
    }

    bool parsing_return_stmt;
    bool parsing_break_stmt;
    bool parsing_assert_stmt;
    bool return_stmt_present;
    bool side_effect_present;
    bool type_check_enabled;
    std::size_t scope_depth;
    std::size_t stack_depth;
    std::size_t parsing_loop_stmt_count;
};
}  // namespace math_expr

#endif
