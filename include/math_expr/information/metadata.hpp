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

#ifndef MATH_EXPR_INFORMATION_METADATA_HPP
#define MATH_EXPR_INFORMATION_METADATA_HPP

#include "math_expr/core/std_includes.hpp"
#include "math_expr/core/types.hpp"

namespace math_expr
{
namespace information
{
using ::math_expr::core::char_cptr;

static char_cptr library = "Mathematical Expression Toolkit";
static char_cptr version =
    "2.71828182845904523536028747135266249775724"
    "7093699959574966967627724076630353547594571"
    "3821785251664274274663919320030599218174135"
    "9662904357290033429526059563073813232862794";
static char_cptr date = "20250101";
static char_cptr min_cpp = "199711L";

static inline std::string data()
{
    static const std::string info_str =
        std::string(library) + std::string(" v") + std::string(version) + std::string(" (") + date +
        std::string(")") + std::string(" (") + min_cpp + std::string(")");
    return info_str;
}

}  // namespace information

}  // namespace math_expr

#endif
