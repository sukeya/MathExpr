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

#ifndef MATH_EXPR_PARSER_SEQUENCE_PARSER_HPP
#define MATH_EXPR_PARSER_SEQUENCE_PARSER_HPP

namespace math_expr
{
template <typename T>
class parser_sequence
{
   public:
    using expression_node_ptr = details::expression_node<T>*;
    using token_t = lexer::token;
    using scope_handler_t = math_expr::scope_handler<T>;

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
    struct scoped_bool_restore
    {
        explicit scoped_bool_restore(Context& ctx)
            : value_(ctx.state.side_effect_present), original_value_(ctx.state.side_effect_present)
        {
        }

        ~scoped_bool_restore()
        {
            value_ = value_ || original_value_;
        }

        bool& value_;
        bool original_value_;
    };

    template <typename Context>
    static inline expression_node_ptr parse_multi_sequence(Context& ctx,
                                                           const std::string& source = "",
                                                           const bool enforce_crlbrackets = false)
    {
        token_t::token_type open_bracket = token_t::e_lcrlbracket;
        token_t::token_type close_bracket = token_t::e_rcrlbracket;
        token_t::token_type separator = token_t::e_eof;

        if (!ctx.token_is(open_bracket))
        {
            if (!enforce_crlbrackets && ctx.token_is(token_t::e_lbracket))
            {
                open_bracket = token_t::e_lbracket;
                close_bracket = token_t::e_rbracket;
                separator = token_t::e_comma;
            }
            else
            {
                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, ctx.current_token(),
                    "ERR111 - Expected '" + token_t::to_str(open_bracket) +
                        "' for call to multi-sequence" +
                        ((!source.empty()) ? std::string(" section of " + source) : ""),
                    core::error_location()));

                return Context::error_node();
            }
        }
        else if (ctx.token_is(close_bracket))
        {
            return ctx.make_null_node();
        }

        std::vector<expression_node_ptr> arg_list;
        std::vector<bool> side_effect_list;

        scoped_node_vector_delete<Context> scoped_args(ctx, arg_list);
        scope_handler_t scope_handler(ctx.state.scope_depth, ctx.sem);
        scoped_bool_restore<Context> side_effect_restore(ctx);

        for (;;)
        {
            ctx.state.side_effect_present = false;

            expression_node_ptr arg = ctx.parse_expression();

            if (nullptr == arg)
            {
                return Context::error_node();
            }

            arg_list.push_back(arg);
            side_effect_list.push_back(ctx.state.side_effect_present);

            if (ctx.token_is(close_bracket))
            {
                break;
            }

            const bool is_next_close = ctx.peek_token_is(close_bracket);

            if (!ctx.token_is(separator) && is_next_close)
            {
                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, ctx.current_token(),
                    "ERR112 - Expected '" + lexer::token::seperator_to_str(separator) +
                        "' for call to multi-sequence section of " + source,
                    core::error_location()));

                return Context::error_node();
            }

            if (ctx.token_is(close_bracket))
            {
                break;
            }
        }

        expression_node_ptr result = ctx.simplify(arg_list, side_effect_list, source.empty());

        scoped_args.delete_ptr = (nullptr == result);
        return result;
    }
};
}  // namespace math_expr

#endif
