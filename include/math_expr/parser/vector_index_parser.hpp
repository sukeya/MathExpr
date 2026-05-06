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

#ifndef MATH_EXPR_PARSER_VECTOR_INDEX_PARSER_HPP
#define MATH_EXPR_PARSER_VECTOR_INDEX_PARSER_HPP

namespace math_expr
{
template <typename T>
class parser_vector_index
{
   public:
    using expression_node_ptr = details::expression_node<T>*;
    using token_t = lexer::token;

    template <typename Context>
    static inline expression_node_ptr parse_vector_index(Context& ctx,
                                                         const std::string& vector_name = "")
    {
        expression_node_ptr index_expr = ctx.parse_expression();

        if (nullptr == index_expr)
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR123 - Failed to parse index for vector: '" + vector_name + "'",
                core::error_location()));

            return Context::error_node();
        }
        else if (!ctx.token_is(token_t::e_rsqrbracket))
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR124 - Expected ']' for index of vector: '" + vector_name + "'",
                core::error_location()));

            ctx.free_node(index_expr);
            return Context::error_node();
        }

        return index_expr;
    }

    template <typename Context>
    static inline void parse_pending_vector_index_operator(Context& ctx,
                                                           expression_node_ptr& expression)
    {
        using token_advance_mode = typename Context::token_advance_mode;

        if ((nullptr == expression) || !ctx.errors_empty() || !is_ivector_node(expression))
        {
            return;
        }

        if (ctx.settings.commutative_check_enabled() &&
            ctx.token_is(token_t::e_mul, token_advance_mode::e_hold) &&
            ctx.peek_token_is(token_t::e_lsqrbracket))
        {
            ctx.token_is(token_t::e_mul);
            ctx.token_is(token_t::e_lsqrbracket);
        }
        else if (ctx.token_is(token_t::e_lsqrbracket, token_advance_mode::e_hold))
        {
            ctx.token_is(token_t::e_lsqrbracket);
        }
        else if (ctx.token_is(token_t::e_rbracket, token_advance_mode::e_hold) &&
                 ctx.peek_token_is(token_t::e_lsqrbracket))
        {
            ctx.token_is(token_t::e_rbracket);
            ctx.token_is(token_t::e_lsqrbracket);
        }
        else
        {
            return;
        }

        typename Context::vector_interface_t* vi = expression->as_vector_iface();

        if (vi)
        {
            details::vector_holder<T>& vec = vi->vec()->vec_holder();
            const std::string vector_name = ctx.sem.get_vector_name(vec.data());
            expression_node_ptr index = parse_vector_index(ctx, vector_name);

            if (index)
            {
                expression = ctx.synthesize_vector_element(vector_name, &vec, expression, index);
                return;
            }
        }

        ctx.free_node(expression);
        expression = Context::error_node();
    }
};
}  // namespace math_expr

#endif
