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

#ifndef MATH_EXPR_PARSER_SYMBOL_TYPES_HPP
#define MATH_EXPR_PARSER_SYMBOL_TYPES_HPP

#include "math_expr/collect_types.hpp"

namespace math_expr
{

using collect_type = collect_types;

enum class symbol_type
{
    e_st_unknown = 0,
    e_st_variable = 1,
    e_st_vector = 2,
    e_st_vecelem = 3,
    e_st_string = 4,
    e_st_function = 5,
    e_st_local_variable = 6,
    e_st_local_vector = 7,
    e_st_local_string = 8
};

}  // namespace math_expr

#endif
