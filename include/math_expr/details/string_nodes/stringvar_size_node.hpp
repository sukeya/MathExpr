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

#ifndef MATH_EXPR_DETAILS_STRINGVAR_SIZE_NODE_HPP
#define MATH_EXPR_DETAILS_STRINGVAR_SIZE_NODE_HPP

#include "math_expr/details/string_nodes/stringvar_node.hpp"

namespace math_expr::details::string_nodes
{
template <typename T> class stringvar_size_node final : public expression_node<T>
{
  public:
    static const std::string null_value;

    explicit stringvar_size_node() : value_(&null_value) {}

    explicit stringvar_size_node(std::string& v) : value_(&v) {}

    inline T value() const override
    {
        return T((*value_).size());
    }

    inline typename expression_node<T>::node_type type() const override
    {
        return expression_node<T>::node_type::e_stringvarsize;
    }

  private:
    const std::string* value_;
};

template <typename T> const std::string stringvar_size_node<T>::null_value = std::string("");
} // namespace math_expr::details::string_nodes

#endif
