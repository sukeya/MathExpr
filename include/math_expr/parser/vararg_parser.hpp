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

#ifndef MATH_EXPR_PARSER_VARARG_PARSER_HPP
#define MATH_EXPR_PARSER_VARARG_PARSER_HPP

namespace math_expr
{
template <typename T>
class parser_vararg
{
   public:
    using expression_node_ptr = details::expression_node<T>*;
    using token_t = lexer::token;

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

    static inline core::operators::operator_type resolve_builtin_operator(const std::string& symbol)
    {
        using operator_type = core::operators::operator_type;

        if (core::imatch(symbol, "avg"))
            return operator_type::avg;
        if (core::imatch(symbol, "mand"))
            return operator_type::mand;
        if (core::imatch(symbol, "max"))
            return operator_type::max;
        if (core::imatch(symbol, "min"))
            return operator_type::min;
        if (core::imatch(symbol, "mor"))
            return operator_type::mor;
        if (core::imatch(symbol, "mul"))
            return operator_type::prod;
        if (core::imatch(symbol, "sum"))
            return operator_type::sum;

        return operator_type::default_op;
    }

    template <typename Context>
    static inline expression_node_ptr parse_vararg_function(Context& ctx)
    {
        std::vector<expression_node_ptr> arg_list;

        const std::string symbol = ctx.current_token().value;

        if (core::imatch(symbol, "~"))
        {
            ctx.next_token();
            return ctx.check_block_statement_closure(ctx.parse_multi_sequence());
        }
        else if (core::imatch(symbol, "[*]"))
        {
            return ctx.check_block_statement_closure(ctx.parse_multi_switch_statement());
        }

        const core::operators::operator_type opt_type = resolve_builtin_operator(symbol);

        if (core::operators::operator_type::default_op == opt_type)
        {
            ctx.set_error(
                parser_error::make_error(parser_error::error_mode::e_syntax, ctx.current_token(),
                                         "ERR104 - Unsupported built-in vararg function: " + symbol,
                                         core::error_location()));

            return Context::error_node();
        }

        scoped_node_vector_delete<Context> scoped_args(ctx, arg_list);

        ctx.lodge_symbol(symbol, symbol_type::e_st_function);

        ctx.next_token();

        if (!ctx.token_is(token_t::e_lbracket))
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR105 - Expected '(' for call to vararg function: " + symbol,
                core::error_location()));

            return Context::error_node();
        }

        if (ctx.token_is(token_t::e_rbracket))
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR106 - vararg function: " + symbol + " requires at least one input parameter",
                core::error_location()));

            return Context::error_node();
        }

        for (;;)
        {
            expression_node_ptr arg = ctx.parse_expression();

            if (nullptr == arg)
            {
                return Context::error_node();
            }

            arg_list.push_back(arg);

            if (ctx.token_is(token_t::e_rbracket))
            {
                break;
            }
            else if (!ctx.token_is(token_t::e_comma))
            {
                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, ctx.current_token(),
                    "ERR107 - Expected ',' for call to vararg function: " + symbol,
                    core::error_location()));

                return Context::error_node();
            }
        }

        expression_node_ptr result = ctx.vararg_function(opt_type, arg_list);

        scoped_args.delete_ptr = (nullptr == result);
        return result;
    }
};
}  // namespace math_expr

#endif
