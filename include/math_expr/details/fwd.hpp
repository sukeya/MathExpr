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

#ifndef MATH_EXPR_DETAILS_FWD_HPP
#define MATH_EXPR_DETAILS_FWD_HPP

namespace math_expr::details
{
template <typename T>
class expression_node;
template <typename T>
class variable_node;
template <typename T>
class stringvar_node;
template <typename T>
class vector_holder;
template <typename T>
class vector_interface;
template <typename T>
class null_node;
template <typename T>
class literal_node;
template <typename T>
class unary_node;
template <typename T>
class binary_node;
template <typename T>
class trinary_node;
template <typename T>
class quaternary_node;
template <typename T>
class conditional_node;
template <typename T>
class cons_conditional_node;
template <typename T>
class while_loop_node;
template <typename T>
class while_loop_rtc_node;
template <typename T>
class repeat_until_loop_node;
template <typename T>
class repeat_until_loop_rtc_node;
template <typename T>
class for_loop_node;
template <typename T>
class for_loop_rtc_node;
template <typename T>
class vector_elem_node;
template <typename T>
class vector_celem_node;
template <typename T>
class return_node;
template <typename T>
class return_envelope_node;
}  // namespace math_expr::details

#endif
