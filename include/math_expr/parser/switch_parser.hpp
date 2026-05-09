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

#ifndef MATH_EXPR_PARSER_SWITCH_PARSER_HPP
#define MATH_EXPR_PARSER_SWITCH_PARSER_HPP

namespace math_expr
{
template <typename T>
class parser_switch
{
   public:
    using expression_node_ptr = details::expression_node<T>*;
    using token_t = lexer::token;

    template <typename Context>
    struct scoped_node_delete
    {
        scoped_node_delete(Context& ctx, expression_node_ptr& node)
            : delete_ptr(true), context_(ctx), node_(node)
        {
        }

        ~scoped_node_delete()
        {
            if (delete_ptr)
            {
                context_.free_node(node_);
            }
        }

        bool delete_ptr;
        Context& context_;
        expression_node_ptr& node_;
    };

    template <typename Context>
    struct scoped_node_vector_delete
    {
        scoped_node_vector_delete(Context& ctx, std::vector<expression_node_ptr>& nodes)
            : delete_ptr(true), context_(ctx), nodes_(nodes)
        {
        }

        ~scoped_node_vector_delete()
        {
            if (!delete_ptr)
                return;

            for (std::size_t i = 0; i < nodes_.size(); ++i)
            {
                context_.free_node(nodes_[i]);
            }

            nodes_.clear();
        }

        bool delete_ptr;
        Context& context_;
        std::vector<expression_node_ptr>& nodes_;
    };

    template <typename Context>
    static inline expression_node_ptr parse_switch_statement(Context& ctx)
    {
        using token_advance_mode = typename Context::token_advance_mode;

        std::vector<expression_node_ptr> arg_list;

        if (!core::imatch(ctx.current_token().value, "switch"))
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR090 - Expected keyword 'switch'", core::error_location()));

            return Context::error_node();
        }

        scoped_node_vector_delete<Context> scoped_args(ctx, arg_list);

        ctx.next_token();

        if (!ctx.token_is(token_t::e_lcrlbracket))
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR091 - Expected '{' for call to switch statement", core::error_location()));

            return Context::error_node();
        }

        expression_node_ptr default_statement = Context::error_node();
        scoped_node_delete<Context> scoped_default(ctx, default_statement);

        for (;;)
        {
            if (core::imatch("case", ctx.current_token().value))
            {
                ctx.next_token();

                expression_node_ptr condition = ctx.parse_expression();

                if (nullptr == condition)
                {
                    return Context::error_node();
                }
                else if (!ctx.token_is(token_t::e_colon))
                {
                    ctx.set_error(parser_error::make_error(
                        parser_error::error_mode::e_syntax, ctx.current_token(),
                        "ERR092 - Expected ':' for case of switch statement",
                        core::error_location()));

                    ctx.free_node(condition);
                    return Context::error_node();
                }

                expression_node_ptr consequent =
                    (ctx.token_is(token_t::e_lcrlbracket, token_advance_mode::e_hold))
                        ? ctx.parse_multi_sequence("switch-consequent")
                        : ctx.parse_expression();

                if (nullptr == consequent)
                {
                    ctx.free_node(condition);
                    return Context::error_node();
                }
                else if (!ctx.token_is(token_t::e_eof))
                {
                    ctx.set_error(parser_error::make_error(
                        parser_error::error_mode::e_syntax, ctx.current_token(),
                        "ERR093 - Expected ';' at end of case for switch statement",
                        core::error_location()));

                    ctx.free_node(condition);
                    ctx.free_node(consequent);
                    return Context::error_node();
                }

                if (details::is_constant_node(condition) && details::is_false(condition))
                {
                    ctx.free_node(condition);
                    ctx.free_node(consequent);
                }
                else
                {
                    arg_list.push_back(condition);
                    arg_list.push_back(consequent);
                }
            }
            else if (core::imatch("default", ctx.current_token().value))
            {
                if (nullptr != default_statement)
                {
                    ctx.set_error(parser_error::make_error(
                        parser_error::error_mode::e_syntax, ctx.current_token(),
                        "ERR094 - Multiple default cases for switch statement",
                        core::error_location()));

                    return Context::error_node();
                }

                ctx.next_token();

                if (!ctx.token_is(token_t::e_colon))
                {
                    ctx.set_error(parser_error::make_error(
                        parser_error::error_mode::e_syntax, ctx.current_token(),
                        "ERR095 - Expected ':' for default of switch statement",
                        core::error_location()));

                    return Context::error_node();
                }

                default_statement =
                    (ctx.token_is(token_t::e_lcrlbracket, token_advance_mode::e_hold))
                        ? ctx.parse_multi_sequence("switch-default")
                        : ctx.parse_expression();

                if (nullptr == default_statement)
                {
                    return Context::error_node();
                }
                else if (!ctx.token_is(token_t::e_eof))
                {
                    ctx.set_error(parser_error::make_error(
                        parser_error::error_mode::e_syntax, ctx.current_token(),
                        "ERR096 - Expected ';' at end of default for switch statement",
                        core::error_location()));

                    return Context::error_node();
                }
            }
            else if (ctx.token_is(token_t::e_rcrlbracket))
            {
                break;
            }
            else
            {
                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, ctx.current_token(),
                    "ERR097 - Expected '}' at end of switch statement", core::error_location()));

                return Context::error_node();
            }
        }

        const bool default_statement_present = (nullptr != default_statement);

        if (default_statement_present)
        {
            arg_list.push_back(default_statement);
        }
        else
        {
            arg_list.push_back(ctx.make_nan_literal());
        }

        expression_node_ptr result = ctx.switch_statement(arg_list, default_statement_present);

        scoped_args.delete_ptr = (nullptr == result);
        scoped_default.delete_ptr = (nullptr == result);

        return result;
    }

    template <typename Context>
    static inline expression_node_ptr parse_multi_switch_statement(Context& ctx)
    {
        using token_advance_mode = typename Context::token_advance_mode;

        std::vector<expression_node_ptr> arg_list;

        if (!core::imatch(ctx.current_token().value, "[*]"))
        {
            ctx.set_error(
                parser_error::make_error(parser_error::error_mode::e_syntax, ctx.current_token(),
                                         "ERR098 - Expected token '[*]'", core::error_location()));

            return Context::error_node();
        }

        scoped_node_vector_delete<Context> scoped_args(ctx, arg_list);

        ctx.next_token();

        if (!ctx.token_is(token_t::e_lcrlbracket))
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR099 - Expected '{' for call to [*] statement", core::error_location()));

            return Context::error_node();
        }

        for (;;)
        {
            if (!core::imatch("case", ctx.current_token().value))
            {
                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, ctx.current_token(),
                    "ERR100 - Expected a 'case' statement for multi-switch",
                    core::error_location()));

                return Context::error_node();
            }

            ctx.next_token();

            expression_node_ptr condition = ctx.parse_expression();

            if (nullptr == condition)
            {
                return Context::error_node();
            }

            if (!ctx.token_is(token_t::e_colon))
            {
                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, ctx.current_token(),
                    "ERR101 - Expected ':' for case of [*] statement", core::error_location()));

                ctx.free_node(condition);
                return Context::error_node();
            }

            expression_node_ptr consequent =
                (ctx.token_is(token_t::e_lcrlbracket, token_advance_mode::e_hold))
                    ? ctx.parse_multi_sequence("multi-switch-consequent")
                    : ctx.parse_expression();

            if (nullptr == consequent)
            {
                ctx.free_node(condition);
                return Context::error_node();
            }

            if (!ctx.token_is(token_t::e_eof))
            {
                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, ctx.current_token(),
                    "ERR102 - Expected ';' at end of case for [*] statement",
                    core::error_location()));

                ctx.free_node(condition);
                ctx.free_node(consequent);
                return Context::error_node();
            }

            if (details::is_constant_node(condition) && details::is_false(condition))
            {
                ctx.free_node(condition);
                ctx.free_node(consequent);
            }
            else
            {
                arg_list.push_back(condition);
                arg_list.push_back(consequent);
            }

            if (ctx.token_is(token_t::e_rcrlbracket, token_advance_mode::e_hold))
            {
                break;
            }
        }

        if (!ctx.token_is(token_t::e_rcrlbracket))
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR103 - Expected '}' at end of [*] statement", core::error_location()));

            return Context::error_node();
        }

        expression_node_ptr result = ctx.multi_switch_statement(arg_list);

        scoped_args.delete_ptr = (nullptr == result);

        return result;
    }
};
}  // namespace math_expr

#endif
