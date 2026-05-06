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

#ifndef MATH_EXPR_PARSER_STRING_RANGE_PARSER_HPP
#define MATH_EXPR_PARSER_STRING_RANGE_PARSER_HPP

namespace math_expr
{
template <typename T>
class parser_string_range
{
   public:
    using expression_node_ptr = details::expression_node<T>*;
    using token_t = lexer::token;

    template <typename Context>
    static inline expression_node_ptr parse_string_range_statement(Context& ctx,
                                                                   expression_node_ptr& expression)
    {
        if (!ctx.token_is(token_t::e_lsqrbracket))
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR108 - Expected '[' as start of string range definition",
                core::error_location()));

            ctx.free_node(expression);
            return Context::error_node();
        }
        else if (ctx.token_is(token_t::e_rsqrbracket))
        {
            return ctx.make_string_size_node(expression);
        }

        typename Context::range_t rp;

        if (!ctx.parse_range(rp, true))
        {
            ctx.free_node(expression);
            return Context::error_node();
        }

        expression_node_ptr result = ctx.string_range(expression, rp);

        if (nullptr == result)
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR109 - Failed to generate string range node", core::error_location()));

            ctx.free_node(expression);
            rp.free();
        }

        rp.clear();

        if (result && result->valid())
        {
            return result;
        }

        ctx.set_error(parser_error::make_error(
            parser_error::error_mode::e_synthesis, ctx.current_token(),
            "ERR110 - Failed to synthesize node: string_range_node", core::error_location()));

        ctx.free_node(result);
        rp.free();
        return Context::error_node();
    }

    template <typename Context>
    static inline bool parse_pending_string_rangesize(Context& ctx, expression_node_ptr& expression)
    {
        const std::size_t max_rangesize_parses = 100;
        std::size_t i = 0;

        while ((nullptr != expression) && (i++ < max_rangesize_parses) && ctx.errors_empty() &&
               is_generally_string_node(expression) &&
               ctx.token_is(token_t::e_lsqrbracket, Context::token_advance_mode::e_hold))
        {
            expression = parse_string_range_statement(ctx, expression);
        }

        return (i > 1);
    }
};
}  // namespace math_expr

#endif
