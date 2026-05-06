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

#ifndef MATH_EXPR_PARSER_STATEMENT_PARSER_HPP
#define MATH_EXPR_PARSER_STATEMENT_PARSER_HPP

namespace math_expr
{
template <typename T>
class parser_statement
{
   public:
    using expression_node_ptr = details::expression_node<T>*;
    using token_t = lexer::token;

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
        }

        bool delete_ptr;
        Context& context_;
        std::vector<expression_node_ptr>& nodes_;
    };

    template <typename Context>
    static inline expression_node_ptr parse_swap_statement(Context& ctx)
    {
        if (!core::imatch(ctx.current_token().value, "swap"))
            return Context::error_node();

        ctx.next_token();

        if (!ctx.token_is(token_t::e_lbracket))
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR205 - Expected '(' at start of swap statement", core::error_location()));

            return Context::error_node();
        }

        expression_node_ptr variable0 = Context::error_node();
        expression_node_ptr variable1 = Context::error_node();
        bool variable0_generated = false;
        bool variable1_generated = false;

        if (!parse_swap_operand(ctx, variable0, variable0_generated, true))
            return Context::error_node();

        if (!ctx.token_is(token_t::e_comma))
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR209 - Expected ',' between parameters to swap", core::error_location()));

            if (variable0_generated)
                ctx.free_node(variable0);

            return Context::error_node();
        }

        if (!parse_swap_operand(ctx, variable1, variable1_generated, false))
        {
            if (variable0_generated)
                ctx.free_node(variable0);

            return Context::error_node();
        }

        if (!ctx.token_is(token_t::e_rbracket))
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR213 - Expected ')' at end of swap statement", core::error_location()));

            if (variable0_generated)
                ctx.free_node(variable0);

            if (variable1_generated)
                ctx.free_node(variable1);

            return Context::error_node();
        }

        typename Context::variable_node_ptr v0 = nullptr;
        typename Context::variable_node_ptr v1 = nullptr;
        expression_node_ptr result = Context::error_node();

        if ((nullptr != (v0 = ctx.as_variable_node(variable0))) &&
            (nullptr != (v1 = ctx.as_variable_node(variable1))))
        {
            result = ctx.make_swap_node(v0, v1);

            if (variable0_generated)
                ctx.free_node(variable0);

            if (variable1_generated)
                ctx.free_node(variable1);
        }
        else
        {
            result = ctx.make_swap_generic_node(variable0, variable1);
        }

        ctx.activate_side_effect("parse_swap_statement()");
        return result;
    }

#ifndef MATH_EXPR_DISABLE_RETURN_STATEMENT
    template <typename Context>
    static inline expression_node_ptr parse_return_statement(Context& ctx)
    {
        if (ctx.state.parsing_return_stmt)
        {
            ctx.set_error(
                parser_error::make_error(parser_error::error_mode::e_syntax, ctx.current_token(),
                                         "ERR214 - Return call within a return call is not allowed",
                                         core::error_location()));

            return Context::error_node();
        }

        scoped_bool_negator<Context> scoped_return(ctx.state.parsing_return_stmt);

        std::vector<expression_node_ptr> arg_list;
        scoped_node_vector_delete<Context> scoped_args(ctx, arg_list);

        if (!core::imatch(ctx.current_token().value, "return"))
            return Context::error_node();

        ctx.next_token();

        if (!ctx.token_is(token_t::e_lsqrbracket))
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR215 - Expected '[' at start of return statement", core::error_location()));

            return Context::error_node();
        }
        else if (!ctx.token_is(token_t::e_rsqrbracket))
        {
            for (;;)
            {
                expression_node_ptr arg = ctx.parse_expression();

                if (nullptr == arg)
                    return Context::error_node();

                arg_list.push_back(arg);

                if (ctx.token_is(token_t::e_rsqrbracket))
                    break;
                else if (!ctx.token_is(token_t::e_comma))
                {
                    ctx.set_error(parser_error::make_error(
                        parser_error::error_mode::e_syntax, ctx.current_token(),
                        "ERR216 - Expected ',' between values during call to return",
                        core::error_location()));

                    return Context::error_node();
                }
            }
        }
        else if (ctx.settings.zero_return_disabled())
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR217 - Zero parameter return statement not allowed", core::error_location()));

            return Context::error_node();
        }

        const token_t prev_token = ctx.current_token();

        if (ctx.token_is(token_t::e_rsqrbracket))
        {
            if (!arg_list.empty())
            {
                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, prev_token,
                    "ERR218 - Invalid ']' found during return call", core::error_location()));

                return Context::error_node();
            }
        }

        std::string ret_param_type_list;

        for (std::size_t i = 0; i < arg_list.size(); ++i)
        {
            if (nullptr == arg_list[i])
                return Context::error_node();
            else if (is_ivector_node(arg_list[i]))
                ret_param_type_list += 'V';
            else if (is_generally_string_node(arg_list[i]))
                ret_param_type_list += 'S';
            else
                ret_param_type_list += 'T';
        }

        ctx.retparam_list.push_back(ret_param_type_list);

        expression_node_ptr result = ctx.return_call(arg_list);
        scoped_args.delete_ptr = (nullptr == result);
        ctx.state.return_stmt_present = true;
        ctx.activate_side_effect("parse_return_statement()");
        return result;
    }
#endif

    template <typename Context>
    static inline expression_node_ptr parse_assert_statement(Context& ctx)
    {
        assert(core::imatch(ctx.current_token().value, "assert"));

        if (ctx.state.parsing_assert_stmt)
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR219 - Assert statement within an assert statement is not allowed",
                core::error_location()));

            return Context::error_node();
        }

        scoped_bool_negator<Context> scoped_assert(ctx.state.parsing_assert_stmt);
        ctx.next_token();

        std::vector<expression_node_ptr> assert_arg_list(3, Context::error_node());
        scoped_node_vector_delete<Context> scoped_args(ctx, assert_arg_list);

        expression_node_ptr& assert_condition = assert_arg_list[0];
        expression_node_ptr& assert_message = assert_arg_list[1];
        expression_node_ptr& assert_id = assert_arg_list[2];

        if (!ctx.token_is(token_t::e_lbracket))
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR220 - Expected '(' at start of assert statement", core::error_location()));

            return Context::error_node();
        }

        const token_t start_token = ctx.current_token();

        if (nullptr == (assert_condition = ctx.parse_expression()))
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR221 - Failed to parse condition for assert statement", core::error_location()));

            return Context::error_node();
        }

        const token_t end_token = ctx.current_token();

        if (!ctx.token_is(token_t::e_rbracket))
        {
            if (!ctx.token_is(token_t::e_comma))
            {
                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, ctx.current_token(),
                    "ERR222 - Expected ',' between condition and message for assert statement",
                    core::error_location()));

                return Context::error_node();
            }
            else if ((nullptr == (assert_message = ctx.parse_expression())) ||
                     !details::is_generally_string_node(assert_message))
            {
                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, ctx.current_token(),
                    "ERR223 - " +
                        (assert_message
                             ? std::string("Expected string for assert message")
                             : std::string("Failed to parse message for assert statement")),
                    core::error_location()));

                return Context::error_node();
            }
            else if (!ctx.token_is(token_t::e_rbracket))
            {
                if (!ctx.token_is(token_t::e_comma))
                {
                    ctx.set_error(parser_error::make_error(
                        parser_error::error_mode::e_syntax, ctx.current_token(),
                        "ERR224 - Expected ',' between message and ID for assert statement",
                        core::error_location()));

                    return Context::error_node();
                }
                else if ((nullptr == (assert_id = ctx.parse_expression())) ||
                         !details::is_const_string_node(assert_id))
                {
                    ctx.set_error(parser_error::make_error(
                        parser_error::error_mode::e_syntax, ctx.current_token(),
                        "ERR225 - " + (assert_id
                                           ? std::string("Expected literal string for assert ID")
                                           : std::string("Failed to parse string for assert ID")),
                        core::error_location()));

                    return Context::error_node();
                }
                else if (!ctx.token_is(token_t::e_rbracket))
                {
                    ctx.set_error(parser_error::make_error(
                        parser_error::error_mode::e_syntax, ctx.current_token(),
                        "ERR226 - Expected ')' at start of assert statement",
                        core::error_location()));

                    return Context::error_node();
                }
            }
        }

        math_expr::assert_check::assert_context assert_context;
        assert_context.condition = ctx.lexer_substr(start_token.position, end_token.position);
        assert_context.offet = start_token.position;

        if (nullptr == ctx.assert_check)
        {
            core::debug_print(
                "parse_assert_statement() - assert functionality is disabled. assert "
                "condition: %s\n",
                assert_context.condition.c_str());

            return ctx.make_null_node();
        }

#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
        if (assert_message && details::is_const_string_node(assert_message))
        {
            auto* message_base = assert_message->as_string_base();
            assert(message_base);
            assert_context.message = message_base->str();
        }

        if (assert_id && details::is_const_string_node(assert_id))
        {
            auto* id_base = assert_id->as_string_base();
            assert(id_base);
            assert_context.id = id_base->str();

            if (ctx.assert_ids.end() != ctx.assert_ids.find(assert_context.id))
            {
                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, ctx.current_token(),
                    "ERR227 - Duplicate assert ID: " + assert_context.id, core::error_location()));

                return Context::error_node();
            }

            ctx.assert_ids.insert(assert_context.id);
            ctx.free_node(assert_id);
        }
#endif

        expression_node_ptr result_node =
            ctx.assert_call(assert_condition, assert_message, assert_context);

        core::debug_print("parse_assert_statement() - assert condition: [%s]\n",
                          assert_context.condition.c_str());
        core::debug_print("parse_assert_statement() - assert message:   [%s]\n",
                          assert_context.message.c_str());
        core::debug_print("parse_assert_statement() - assert id:        [%s]\n",
                          assert_context.id.c_str());
        core::debug_print("parse_assert_statement() - assert offset:    [%d]\n",
                          static_cast<int>(assert_context.offet));

        if (nullptr == result_node)
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR228 - Failed to synthesize assert", core::error_location()));

            return Context::error_node();
        }

        scoped_args.delete_ptr = false;
        return result_node;
    }

   private:
    template <typename Context>
    static inline bool parse_swap_operand(Context& ctx, expression_node_ptr& variable,
                                          bool& generated, const bool first)
    {
        const std::string var_name = ctx.current_token().value;

        if (!ctx.token_is(token_t::e_symbol, Context::token_advance_mode::e_hold))
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR" + std::string(first ? "206" : "210") +
                    " - Expected a symbol for variable or vector element definition",
                core::error_location()));

            return false;
        }
        else if (ctx.peek_token_is(token_t::e_lsqrbracket))
        {
            variable = ctx.parse_vector();

            if (nullptr == variable)
            {
                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, ctx.current_token(),
                    "ERR" + std::string(first ? "207" : "211") +
                        std::string(first ? " - First parameter to swap is an invalid vector "
                                            "element: '"
                                          : " - Second parameter to swap is an invalid vector "
                                            "element: '") +
                        var_name + "'",
                    core::error_location()));

                return false;
            }

            generated = true;
            return true;
        }

        if (ctx.is_variable(var_name))
            variable = ctx.get_variable(var_name);

        const typename Context::scope_element_t& se = ctx.get_element(var_name);

        if (se.active && (se.name == var_name) &&
            (Context::scope_element_t::element_type::e_variable == se.type))
        {
            variable = se.var_node;
        }

        ctx.lodge_symbol(var_name, symbol_type::e_st_variable);

        if (nullptr == variable)
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR" + std::string(first ? "208" : "212") +
                    std::string(first ? " - First parameter to swap is an invalid variable: '"
                                      : " - Second parameter to swap is an invalid variable: '") +
                    var_name + "'",
                core::error_location()));

            return false;
        }

        ctx.next_token();
        return true;
    }
};
}  // namespace math_expr

#endif
