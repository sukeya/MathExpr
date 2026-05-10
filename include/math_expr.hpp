/*
 ******************************************************************
 *           C++ Mathematical Expression Toolkit Library          *
 *                                                                *
 * Author: Arash Partow (1999-2025)                               *
 * URL: https://www.partow.net/programming/exprtk/index.html      *
 *                                                                *
 * Copyright notice:                                              *
 * Free use of the C++ Mathematical Expression Toolkit Library is *
 * permitted under the guidelines and in accordance with the most *
 * current version of the MIT License.                            *
 * https://www.opensource.org/licenses/MIT                        *
 * SPDX-License-Identifier: MIT                                   *
 *                                                                *
 * Example expressions:                                           *
 * (00) (y + x / y) * (x - y / x)                                 *
 * (01) (x^2 / sin(2 * pi / y)) - x / 2                           *
 * (02) sqrt(1 - (x^2))                                           *
 * (03) 1 - sin(2 * x) + cos(pi / y)                              *
 * (04) a * exp(2 * t) + c                                        *
 * (05) if(((x + 2) == 3) and ((y + 5) <= 9), 1 + w, 2 / z)       *
 * (06) (avg(x,y) <= x + y ? x - y : x * y) + 2 * pi / x          *
 * (07) z := x + sin(2 * pi / y)                                  *
 * (08) u := 2 * (pi * z) / (w := x + cos(y / pi))                *
 * (09) clamp(-1, sin(2 * pi * x) + cos(y / 2 * pi), +1)          *
 * (10) inrange(-2, m, +2) == if(({-2 <= m} and [m <= +2]), 1, 0) *
 * (11) (2sin(x)cos(2y)7 + 1) == (2 * sin(x) * cos(2*y) * 7 + 1)  *
 * (12) (x ilike 's*ri?g') and [y < (3 z^7 + w)]                  *
 *                                                                *
 ******************************************************************


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

#ifndef INCLUDE_MATH_EXPR_HPP
#define INCLUDE_MATH_EXPR_HPP

#include "math_expr/core/std_includes.hpp"
#include "math_expr/fwd.hpp"
#include "math_expr/details/fwd.hpp"
#include "math_expr/core/macros.hpp"
#include "math_expr/core/types.hpp"
#include "math_expr/core/string_utils.hpp"
#include "math_expr/core/numeric.hpp"
#include "math_expr/loop_runtime_check.hpp"
#include "math_expr/vector_access_runtime_check.hpp"
#include "math_expr/assert_check.hpp"
#include "math_expr/compilation_check.hpp"
#include "math_expr/lexer/token.hpp"
#include "math_expr/lexer/generator.hpp"
#include "math_expr/lexer/scanners.hpp"
#include "math_expr/lexer/modifiers.hpp"
#include "math_expr/lexer/joiners.hpp"
#include "math_expr/lexer/helper.hpp"
#include "math_expr/lexer/parser_helper.hpp"
#include "math_expr/vector_view.hpp"
#include "math_expr/type_store.hpp"
#include "math_expr/results_context.hpp"
#include "math_expr/core/operator_types.hpp"
#include "math_expr/details/expression_node.hpp"
#include "math_expr/details/literal_nodes.hpp"
#include "math_expr/details/operator_nodes.hpp"
#include "math_expr/details/conditional_nodes.hpp"
#include "math_expr/details/loop_nodes.hpp"
#include "math_expr/details/vector_nodes.hpp"
#include "math_expr/details/function_nodes.hpp"
#include "math_expr/details/return_nodes.hpp"
#include "math_expr/details/node_utils.hpp"
#include "math_expr/function_traits.hpp"
#include "math_expr/ifunction.hpp"
#include "math_expr/ivararg_function.hpp"
#include "math_expr/igeneric_function.hpp"
#include "math_expr/symbol_table.hpp"
#include "math_expr/expression.hpp"
#include "math_expr/expression_helper.hpp"
#include "math_expr/parser_error.hpp"
#include "math_expr/parser.hpp"
#include "math_expr/polynomial.hpp"
#include "math_expr/function_compositor.hpp"
#include "math_expr/timer.hpp"
#include "math_expr/type_defs.hpp"
#include "math_expr/rtl/io/package.hpp"
#include "math_expr/rtl/io/file/package.hpp"
#include "math_expr/rtl/vecops/package.hpp"
#include "math_expr/information/metadata.hpp"

#endif
