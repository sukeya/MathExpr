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

#ifndef MATH_EXPR_DETAILS_NODE_VARIANT_ADAPTER_HPP
#define MATH_EXPR_DETAILS_NODE_VARIANT_ADAPTER_HPP

#include "math_expr/details/node_utils.hpp"

namespace math_expr::details
{
template <typename T>
class node_variant_adapter
{
   public:
    using expression_ptr = expression_node<T>*;

    struct null_view
    {
        expression_ptr node;
    };

    struct literal_view
    {
        expression_ptr node;
    };

    struct variable_view
    {
        expression_ptr node;
    };

    struct string_view
    {
        expression_ptr node;
    };

    struct unary_view
    {
        expression_ptr node;
    };

    struct binary_view
    {
        expression_ptr node;
    };

    struct function_view
    {
        expression_ptr node;
    };

    struct vararg_view
    {
        expression_ptr node;
    };

    struct multi_vararg_view
    {
        expression_ptr node;
    };

    struct assert_view
    {
        expression_ptr node;
    };

    struct sf3ext_view
    {
        expression_ptr node;
    };

    struct sf4ext_view
    {
        expression_ptr node;
    };

    struct other_view
    {
        expression_ptr node;
    };

    using variant_type =
        std::variant<std::monostate, null_view, literal_view, variable_view, string_view,
                     unary_view, binary_view, function_view, vararg_view, multi_vararg_view,
                     assert_view, sf3ext_view, sf4ext_view, other_view>;

    static inline variant_type classify(expression_ptr node)
    {
        if (nullptr == node)
        {
            return std::monostate{};
        }
        else if (is_null_node(node))
        {
            return null_view{node};
        }
        else if (is_variable_node(node))
        {
            return variable_view{node};
        }
        else if (is_generally_string_node(node))
        {
            return string_view{node};
        }
        else if (is_literal_node(node))
        {
            return literal_view{node};
        }
        else if (is_unary_node(node))
        {
            return unary_view{node};
        }
        else if (is_binary_node(node))
        {
            return binary_view{node};
        }
        else if (is_function(node))
        {
            return function_view{node};
        }
        else if (node->type() == expression_node<T>::node_type::e_vararg_multi)
        {
            return multi_vararg_view{node};
        }
        else if (is_vararg_node(node))
        {
            return vararg_view{node};
        }
        else if (is_assert_node(node))
        {
            return assert_view{node};
        }
        else if (is_sf3ext_node(node))
        {
            return sf3ext_view{node};
        }
        else if (is_sf4ext_node(node))
        {
            return sf4ext_view{node};
        }

        return other_view{node};
    }

    template <typename Alternative>
    static inline bool holds(expression_ptr node)
    {
        return std::holds_alternative<Alternative>(classify(node));
    }

    template <typename Visitor>
    static inline decltype(auto) visit(expression_ptr node, Visitor&& visitor)
    {
        return std::visit(std::forward<Visitor>(visitor), classify(node));
    }
};
}  // namespace math_expr::details

#endif
