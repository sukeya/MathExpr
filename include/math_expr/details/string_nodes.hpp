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

#ifndef MATH_EXPR_DETAILS_STRING_NODES_HPP
#define MATH_EXPR_DETAILS_STRING_NODES_HPP

#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
#include "math_expr/details/string_nodes/stringvar_node.hpp"
#include "math_expr/details/string_nodes/string_range_node.hpp"
#include "math_expr/details/string_nodes/const_string_range_node.hpp"
#include "math_expr/details/string_nodes/generic_string_range_node.hpp"
#include "math_expr/details/string_nodes/string_concat_node.hpp"
#include "math_expr/details/string_nodes/swap_string_node.hpp"
#include "math_expr/details/string_nodes/swap_genstrings_node.hpp"
#include "math_expr/details/string_nodes/stringvar_size_node.hpp"
#include "math_expr/details/string_nodes/string_size_node.hpp"
#include "math_expr/details/string_nodes/string_assignment_process.hpp"
#include "math_expr/details/string_nodes/assignment_string_node.hpp"
#include "math_expr/details/string_nodes/assignment_string_range_node.hpp"
#include "math_expr/details/string_nodes/conditional_string_node.hpp"
#include "math_expr/details/string_nodes/cons_conditional_str_node.hpp"
#include "math_expr/details/string_nodes/str_vararg_node.hpp"
#endif

#endif
