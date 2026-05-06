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

#ifndef MATH_EXPR_EXPRESSION_HELPER_HPP
#define MATH_EXPR_EXPRESSION_HELPER_HPP

#include "math_expr/expression.hpp"
#include "math_expr/details/node_variant_adapter.hpp"

namespace math_expr
{
template <typename T>
class expression_helper
{
   public:
    using node_variant_adapter_t = details::node_variant_adapter<T>;
    using node_variant_t = typename node_variant_adapter_t::variant_type;

    enum class node_types
    {
        e_literal,
        e_variable,
        e_string,
        e_unary,
        e_binary,
        e_function,
        e_vararg,
        e_null,
        e_assert,
        e_sf3ext,
        e_sf4ext
    };

   private:
    using null_view = typename node_variant_adapter_t::null_view;
    using literal_view = typename node_variant_adapter_t::literal_view;
    using variable_view = typename node_variant_adapter_t::variable_view;
    using string_view = typename node_variant_adapter_t::string_view;
    using unary_view = typename node_variant_adapter_t::unary_view;
    using binary_view = typename node_variant_adapter_t::binary_view;
    using function_view = typename node_variant_adapter_t::function_view;
    using vararg_view = typename node_variant_adapter_t::vararg_view;
    using multi_vararg_view = typename node_variant_adapter_t::multi_vararg_view;
    using assert_view = typename node_variant_adapter_t::assert_view;
    using sf3ext_view = typename node_variant_adapter_t::sf3ext_view;
    using sf4ext_view = typename node_variant_adapter_t::sf4ext_view;
    using other_view = typename node_variant_adapter_t::other_view;

    static inline node_variant_t classify(const expression<T>& expr)
    {
        return expr.classify_root_node();
    }

    static inline bool matches_node_type(const node_variant_t& node, const node_types expected)
    {
        return std::visit(
            [expected](const auto& view) -> bool
            {
                using view_t = std::decay_t<decltype(view)>;

                if constexpr (std::is_same_v<view_t, std::monostate> ||
                              std::is_same_v<view_t, other_view>)
                {
                    return false;
                }
                else if constexpr (std::is_same_v<view_t, literal_view>)
                {
                    return (node_types::e_literal == expected);
                }
                else if constexpr (std::is_same_v<view_t, variable_view>)
                {
                    return (node_types::e_variable == expected);
                }
                else if constexpr (std::is_same_v<view_t, string_view>)
                {
                    return (node_types::e_string == expected);
                }
                else if constexpr (std::is_same_v<view_t, unary_view>)
                {
                    return (node_types::e_unary == expected);
                }
                else if constexpr (std::is_same_v<view_t, binary_view>)
                {
                    return (node_types::e_binary == expected);
                }
                else if constexpr (std::is_same_v<view_t, function_view>)
                {
                    return (node_types::e_function == expected);
                }
                else if constexpr (std::is_same_v<view_t, vararg_view> ||
                                   std::is_same_v<view_t, multi_vararg_view>)
                {
                    return (node_types::e_vararg == expected);
                }
                else if constexpr (std::is_same_v<view_t, null_view>)
                {
                    return (node_types::e_null == expected);
                }
                else if constexpr (std::is_same_v<view_t, assert_view>)
                {
                    return (node_types::e_assert == expected);
                }
                else if constexpr (std::is_same_v<view_t, sf3ext_view>)
                {
                    return (node_types::e_sf3ext == expected);
                }
                else if constexpr (std::is_same_v<view_t, sf4ext_view>)
                {
                    return (node_types::e_sf4ext == expected);
                }
                else
                {
                    return false;
                }
            },
            node);
    }

   public:
    static inline bool is_literal(const expression<T>& expr)
    {
        return matches_node_type(classify(expr), node_types::e_literal);
    }

    static inline bool is_variable(const expression<T>& expr)
    {
        return matches_node_type(classify(expr), node_types::e_variable);
    }

    static inline bool is_string(const expression<T>& expr)
    {
        return matches_node_type(classify(expr), node_types::e_string);
    }

    static inline bool is_unary(const expression<T>& expr)
    {
        return matches_node_type(classify(expr), node_types::e_unary);
    }

    static inline bool is_binary(const expression<T>& expr)
    {
        return matches_node_type(classify(expr), node_types::e_binary);
    }

    static inline bool is_function(const expression<T>& expr)
    {
        return matches_node_type(classify(expr), node_types::e_function);
    }

    static inline bool is_vararg(const expression<T>& expr)
    {
        return matches_node_type(classify(expr), node_types::e_vararg);
    }

    static inline bool is_null(const expression<T>& expr)
    {
        return matches_node_type(classify(expr), node_types::e_null);
    }

    static inline bool is_assert(const expression<T>& expr)
    {
        return matches_node_type(classify(expr), node_types::e_assert);
    }

    static inline bool is_sf3ext(const expression<T>& expr)
    {
        return matches_node_type(classify(expr), node_types::e_sf3ext);
    }

    static inline bool is_sf4ext(const expression<T>& expr)
    {
        return matches_node_type(classify(expr), node_types::e_sf4ext);
    }

    static inline bool is_type(const expression<T>& expr, const node_types node_type)
    {
        return matches_node_type(classify(expr), node_type);
    }

    static inline bool match_type_sequence(const expression<T>& expr,
                                           const std::vector<node_types>& type_seq)
    {
        if ((nullptr == expr.get_control_block()) || !is_vararg(expr))
        {
            return false;
        }

        using mo_vararg_t = details::vararg_node<T, math_expr::details::vararg_multi_op<T>>;
        const node_variant_t root_node = classify(expr);

        const auto* multi_vararg = std::get_if<multi_vararg_view>(&root_node);
        mo_vararg_t* vnode = multi_vararg ? static_cast<mo_vararg_t*>(multi_vararg->node) : nullptr;

        if ((nullptr == vnode) || type_seq.empty() || (vnode->size() < type_seq.size()))
        {
            return false;
        }

        for (std::size_t i = 0; i < type_seq.size(); ++i)
        {
            assert((*vnode)[i]);

            if (!matches_node_type(node_variant_adapter_t::classify((*vnode)[i]), type_seq[i]))
            {
                return false;
            }
        }

        return true;
    }
};

template <typename T>
inline bool is_valid(const expression<T>& expr)
{
    return expr.get_control_block() && !expression_helper<T>::is_null(expr);
}

}  // namespace math_expr

#endif
