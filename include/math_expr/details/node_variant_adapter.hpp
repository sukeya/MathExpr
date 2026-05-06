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
    using literal_node_t = literal_node<T>;
    using variable_node_t = variable_node<T>;
    using string_base_node_t = string_base_node<T>;
    using string_literal_node_t = string_literal_node<T>;
    using stringvar_node_t = string_nodes::stringvar_node<T>;

    struct null_view
    {
        expression_ptr node;
    };

    struct literal_view
    {
        expression_ptr node;
        literal_node_t* literal;
    };

    struct variable_view
    {
        expression_ptr node;
        variable_node_t* variable;
    };

    struct string_view
    {
        expression_ptr node;
        string_base_node_t* base;
        stringvar_node_t* mutable_node;
        bool is_const_literal;
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
            return variable_view{node, static_cast<variable_node_t*>(node)};
        }
        else if (is_generally_string_node(node))
        {
            return string_view{
                node, static_cast<string_base_node_t*>(node->as_string_base()),
                is_string_node(node) ? static_cast<stringvar_node_t*>(node) : nullptr,
                is_const_string_node(node)};
        }
        else if (is_literal_node(node))
        {
            return literal_view{node, static_cast<literal_node_t*>(node)};
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

    static inline literal_node_t* literal(expression_ptr node)
    {
        const variant_type node_variant = classify(node);
        const auto* view = std::get_if<literal_view>(&node_variant);
        return view ? view->literal : nullptr;
    }

    static inline variable_node_t* variable(expression_ptr node)
    {
        const variant_type node_variant = classify(node);
        const auto* view = std::get_if<variable_view>(&node_variant);
        return view ? view->variable : nullptr;
    }

    static inline string_base_node_t* string_base(expression_ptr node)
    {
        const variant_type node_variant = classify(node);
        const auto* view = std::get_if<string_view>(&node_variant);
        return view ? view->base : nullptr;
    }

    static inline stringvar_node_t* string_variable(expression_ptr node)
    {
        const variant_type node_variant = classify(node);
        const auto* view = std::get_if<string_view>(&node_variant);
        return view ? view->mutable_node : nullptr;
    }

    static inline string_literal_node_t* const_string(expression_ptr node)
    {
        const variant_type node_variant = classify(node);
        const auto* view = std::get_if<string_view>(&node_variant);
        return (view && view->is_const_literal) ? static_cast<string_literal_node_t*>(view->node)
                                                : nullptr;
    }

    static inline bool is_string_variable(expression_ptr node)
    {
        return nullptr != string_variable(node);
    }

    static inline bool is_const_string(expression_ptr node)
    {
        return nullptr != const_string(node);
    }

    static inline bool is_null(expression_ptr node)
    {
        return holds<null_view>(node);
    }

    static inline bool is_literal(expression_ptr node)
    {
        return nullptr != literal(node);
    }

    static inline bool is_variable(expression_ptr node)
    {
        return nullptr != variable(node);
    }
};
}  // namespace math_expr::details

#endif
