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

#ifndef MATH_EXPR_PARSER_SPECIAL_CASE_PARSER_HPP
#define MATH_EXPR_PARSER_SPECIAL_CASE_PARSER_HPP

namespace math_expr
{
template <typename T>
class parser_special_case
{
   public:
    using expression_node_ptr = details::expression_node<T>*;
    using token_t = lexer::token;

    template <typename Context, std::size_t N>
    struct scoped_node_array_delete
    {
        scoped_node_array_delete(Context& ctx, expression_node_ptr (&nodes)[N])
            : delete_ptr(true), context_(ctx), nodes_(nodes)
        {
        }

        ~scoped_node_array_delete()
        {
            if (!delete_ptr)
                return;

            for (std::size_t i = 0; i < N; ++i)
            {
                context_.free_node(nodes_[i]);
            }
        }

        bool delete_ptr;
        Context& context_;
        expression_node_ptr (&nodes_)[N];
    };

    template <typename Context>
    struct scoped_bool_negator
    {
        explicit scoped_bool_negator(bool& value) : value_(value)
        {
            value_ = !value_;
        }

        ~scoped_bool_negator()
        {
            value_ = !value_;
        }

        bool& value_;
    };

    template <typename Context, std::size_t NumberOfParameters>
    static inline expression_node_ptr parse_special_function_impl(
        Context& ctx, const core::operators::operator_type opt_type, const std::string& sf_name)
    {
        expression_node_ptr branch[NumberOfParameters];
        expression_node_ptr result = Context::error_node();

        std::fill_n(branch, NumberOfParameters, nullptr);

        scoped_node_array_delete<Context, NumberOfParameters> scoped_branch(ctx, branch);

        ctx.next_token();

        if (!ctx.token_is(token_t::e_lbracket))
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR146 - Expected '(' for special function '" + sf_name + "'",
                core::error_location()));

            return Context::error_node();
        }

        for (std::size_t i = 0; i < NumberOfParameters; ++i)
        {
            branch[i] = ctx.parse_expression();

            if (nullptr == branch[i])
            {
                return Context::error_node();
            }
            else if (i < (NumberOfParameters - 1))
            {
                if (!ctx.token_is(token_t::e_comma))
                {
                    ctx.set_error(parser_error::make_error(
                        parser_error::error_mode::e_syntax, ctx.current_token(),
                        "ERR147 - Expected ',' before next parameter of special function '" +
                            sf_name + "'",
                        core::error_location()));

                    return Context::error_node();
                }
            }
        }

        if (!ctx.token_is(token_t::e_rbracket))
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR148 - Invalid number of parameters for special function '" + sf_name + "'",
                core::error_location()));

            return Context::error_node();
        }

        result = ctx.special_function(opt_type, branch);
        scoped_branch.delete_ptr = (nullptr == result);
        return result;
    }

    template <typename Context>
    static inline expression_node_ptr parse_special_function(Context& ctx)
    {
        const std::string sf_name = ctx.current_token().value;

        if (!core::is_digit(sf_name[2]) || !core::is_digit(sf_name[3]))
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_token, ctx.current_token(),
                "ERR149 - Invalid special function[1]: " + sf_name, core::error_location()));

            return Context::error_node();
        }

        const int id = (sf_name[2] - '0') * 10 + (sf_name[3] - '0');

        if (id >= static_cast<int>(core::operators::operator_type::sffinal))
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_token, ctx.current_token(),
                "ERR150 - Invalid special function[2]: " + sf_name, core::error_location()));

            return Context::error_node();
        }

        const int sf_3_to_4 = static_cast<int>(core::operators::operator_type::sf48);
        const auto opt_type = static_cast<core::operators::operator_type>(id + 1000);
        const std::size_t number_of_parameters = (id < (sf_3_to_4 - 1000)) ? 3U : 4U;

        switch (number_of_parameters)
        {
            case 3:
                return parse_special_function_impl<Context, 3>(ctx, opt_type, sf_name);
            case 4:
                return parse_special_function_impl<Context, 4>(ctx, opt_type, sf_name);
            default:
                return Context::error_node();
        }
    }

    template <typename Context>
    static inline expression_node_ptr parse_null_statement(Context& ctx)
    {
        ctx.next_token();
        return ctx.make_null_node();
    }

#ifndef MATH_EXPR_DISABLE_BREAK_CONTINUE
    template <typename Context>
    static inline expression_node_ptr parse_break_statement(Context& ctx)
    {
        if (ctx.state.parsing_break_stmt)
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR151 - Invoking 'break' within a break call is not allowed",
                core::error_location()));

            return Context::error_node();
        }
        else if (0 == ctx.state.parsing_loop_stmt_count)
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR152 - Invalid use of 'break', allowed only in the scope of a loop",
                core::error_location()));

            return Context::error_node();
        }

        scoped_bool_negator<Context> scoped_break(ctx.state.parsing_break_stmt);

        if (ctx.brkcnt_list.empty())
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR155 - Invalid use of 'break', allowed only in the scope of a loop",
                core::error_location()));
            return Context::error_node();
        }

        ctx.next_token();

        ctx.brkcnt_list.front() = true;

        expression_node_ptr return_expr = Context::error_node();

        if (ctx.token_is(token_t::e_lsqrbracket))
        {
            return_expr = ctx.parse_expression();

            if (nullptr == return_expr)
            {
                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, ctx.current_token(),
                    "ERR153 - Failed to parse return expression for 'break' statement",
                    core::error_location()));

                return Context::error_node();
            }
            else if (!ctx.token_is(token_t::e_rsqrbracket))
            {
                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, ctx.current_token(),
                    "ERR154 - Expected ']' at the completion of break's return expression",
                    core::error_location()));

                ctx.free_node(return_expr);
                return Context::error_node();
            }
        }

        ctx.activate_side_effect("parse_break_statement()");
        return ctx.make_break_node(return_expr);
    }

    template <typename Context>
    static inline expression_node_ptr parse_continue_statement(Context& ctx)
    {
        if (0 == ctx.state.parsing_loop_stmt_count)
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR156 - Invalid use of 'continue', allowed only in the scope of a loop",
                core::error_location()));

            return Context::error_node();
        }

        ctx.next_token();
        ctx.brkcnt_list.front() = true;
        ctx.activate_side_effect("parse_continue_statement()");
        return ctx.make_continue_node();
    }
#endif
};
}  // namespace math_expr

#endif
