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

#ifndef MATH_EXPR_PARSER_BRANCH_PARSER_HPP
#define MATH_EXPR_PARSER_BRANCH_PARSER_HPP

namespace math_expr
{
template <typename T>
class parser_branch
{
   public:
    using expression_node_ptr = details::expression_node<T>*;
    using token_t = lexer::token;

    template <typename Context>
    static inline bool post_variable_process(Context& ctx, const std::string& symbol)
    {
        if (ctx.peek_token_is(token_t::e_lbracket) || ctx.peek_token_is(token_t::e_lcrlbracket) ||
            ctx.peek_token_is(token_t::e_lsqrbracket))
        {
            if (!ctx.commutative_check_enabled())
            {
                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, ctx.current_token(),
                    "ERR229 - Invalid sequence of variable '" + symbol + "' and bracket",
                    core::error_location()));

                return false;
            }

            ctx.insert_front(token_t::e_mul);
        }

        return true;
    }

    template <typename Context>
    static inline bool post_bracket_process(Context& ctx, const token_t::token_type& token,
                                            expression_node_ptr& branch)
    {
        bool implied_mul = false;

        if (details::is_generally_string_node(branch))
            return true;

        if (details::is_ivector_node(branch))
            return true;

        const typename Context::token_advance_mode hold = Context::token_advance_mode::e_hold;

        switch (token)
        {
            case token_t::e_lcrlbracket:
            case token_t::e_lbracket:
            case token_t::e_lsqrbracket:
                implied_mul = ctx.token_is(token_t::e_lbracket, hold) ||
                              ctx.token_is(token_t::e_lcrlbracket, hold) ||
                              ctx.token_is(token_t::e_lsqrbracket, hold);
                break;

            default:
                return true;
        }

        if (implied_mul)
        {
            if (!ctx.commutative_check_enabled())
            {
                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, ctx.current_token(),
                    "ERR230 - Invalid sequence of brackets", core::error_location()));

                return false;
            }
            else if (token_t::e_eof != ctx.current_token().type)
            {
                ctx.insert_front(ctx.current_token().type);
                ctx.insert_front(token_t::e_mul);
                ctx.next_token();
            }
        }

        return true;
    }

    template <typename Context>
    static inline expression_node_ptr check_block_statement_closure(Context& ctx,
                                                                    expression_node_ptr expression)
    {
        if (expression && ((ctx.current_token().type == token_t::e_symbol) ||
                           (ctx.current_token().type == token_t::e_number)))
        {
            ctx.free_node(expression);

            ctx.set_error(
                parser_error::make_error(parser_error::error_mode::e_syntax, ctx.current_token(),
                                         "ERR240 - Invalid syntax '" + ctx.current_token().value +
                                             "' possible missing operator or context",
                                         core::error_location()));

            return Context::error_node();
        }

        return expression;
    }

    template <typename Context>
    static inline expression_node_ptr parse_branch(
        Context& ctx, const typename Context::precedence_level precedence)
    {
        expression_node_ptr branch = Context::error_node();

        if (token_t::e_number == ctx.current_token().type)
        {
            T numeric_value = T(0);

            if (core::numeric::string_to_real(ctx.current_token().value, numeric_value))
            {
                expression_node_ptr literal_exp = ctx.make_numeric_literal(numeric_value);

                if (nullptr == literal_exp)
                {
                    ctx.set_error(parser_error::make_error(
                        parser_error::error_mode::e_numeric, ctx.current_token(),
                        "ERR242 - Failed generate node for scalar: '" + ctx.current_token().value +
                            "'",
                        core::error_location()));

                    return Context::error_node();
                }

                ctx.next_token();
                branch = literal_exp;
            }
            else
            {
                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_numeric, ctx.current_token(),
                    "ERR243 - Failed to convert '" + ctx.current_token().value + "' to a number",
                    core::error_location()));

                return Context::error_node();
            }
        }
        else if (token_t::e_symbol == ctx.current_token().type)
        {
            branch = ctx.parse_symbol();
        }
        else if (token_t::e_lbracket == ctx.current_token().type)
        {
            ctx.next_token();

            if (nullptr == (branch = ctx.parse_expression()))
            {
                return Context::error_node();
            }

            ctx.token_is(token_t::e_eof);

            if (!ctx.token_is(token_t::e_rbracket))
            {
                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, ctx.current_token(),
                    "ERR244 - Expected ')' instead of: '" + ctx.current_token().value + "'",
                    core::error_location()));

                ctx.free_node(branch);

                return Context::error_node();
            }
            else if (!post_bracket_process(ctx, token_t::e_lbracket, branch))
            {
                ctx.free_node(branch);

                return Context::error_node();
            }

            ctx.parse_pending_vector_index_operator(branch);
        }
        else if (token_t::e_lsqrbracket == ctx.current_token().type)
        {
            ctx.next_token();

            if (nullptr == (branch = ctx.parse_expression()))
            {
                return Context::error_node();
            }
            else if (!ctx.token_is(token_t::e_rsqrbracket))
            {
                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, ctx.current_token(),
                    "ERR245 - Expected ']' instead of: '" + ctx.current_token().value + "'",
                    core::error_location()));

                ctx.free_node(branch);

                return Context::error_node();
            }
            else if (!post_bracket_process(ctx, token_t::e_lsqrbracket, branch))
            {
                ctx.free_node(branch);

                return Context::error_node();
            }
        }
        else if (token_t::e_lcrlbracket == ctx.current_token().type)
        {
            ctx.next_token();

            if (nullptr == (branch = ctx.parse_expression()))
            {
                return Context::error_node();
            }
            else if (!ctx.token_is(token_t::e_rcrlbracket))
            {
                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, ctx.current_token(),
                    "ERR246 - Expected '}' instead of: '" + ctx.current_token().value + "'",
                    core::error_location()));

                ctx.free_node(branch);

                return Context::error_node();
            }
            else if (!post_bracket_process(ctx, token_t::e_lcrlbracket, branch))
            {
                ctx.free_node(branch);

                return Context::error_node();
            }
        }
        else if (token_t::e_sub == ctx.current_token().type)
        {
            ctx.next_token();
            branch = ctx.parse_expression(Context::precedence_level::e_level11);

            if (branch &&
                !(details::is_neg_unary_node(branch) && ctx.simplify_unary_negation_branch(branch)))
            {
                expression_node_ptr result =
                    ctx.make_unary_operator(core::operators::operator_type::neg, branch);

                if (nullptr == result)
                {
                    ctx.free_node(branch);
                    return Context::error_node();
                }

                branch = result;
            }
        }
        else if (token_t::e_add == ctx.current_token().type)
        {
            ctx.next_token();
            branch = ctx.parse_expression(Context::precedence_level::e_level13);
        }
        else if (token_t::e_eof == ctx.current_token().type)
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR247 - Premature end of expression[1]", core::error_location()));

            return Context::error_node();
        }
        else
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR248 - Premature end of expression[2]", core::error_location()));

            return Context::error_node();
        }

        if (branch && (Context::precedence_level::e_level00 == precedence) &&
            ctx.token_is(token_t::e_ternary, Context::token_advance_mode::e_hold))
        {
            branch = ctx.parse_ternary_conditional_statement(branch);
        }

        ctx.parse_pending_string_rangesize(branch);

        return branch;
    }
};
}  // namespace math_expr

#endif
