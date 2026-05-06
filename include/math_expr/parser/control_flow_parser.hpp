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

#ifndef MATH_EXPR_PARSER_CONTROL_FLOW_PARSER_HPP
#define MATH_EXPR_PARSER_CONTROL_FLOW_PARSER_HPP

namespace math_expr
{
template <typename T>
class parser_control_flow
{
   public:
    using expression_node_ptr = details::expression_node<T>*;
    using token_t = lexer::token;
    using scope_handler_t = math_expr::scope_handler<T>;
    using scope_element_t = math_expr::scope_element<T>;

    template <typename Context>
    struct scoped_counter_increment
    {
        explicit scoped_counter_increment(Context& ctx)
            : counter_(ctx.state.parsing_loop_stmt_count)
        {
            ++counter_;
        }

        ~scoped_counter_increment()
        {
            assert(counter_ > 0);
            --counter_;
        }

        std::size_t& counter_;
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
    static inline expression_node_ptr parse_conditional_statement_01(Context& ctx,
                                                                     expression_node_ptr condition)
    {
        expression_node_ptr consequent = Context::error_node();
        expression_node_ptr alternative = Context::error_node();
        bool result = true;

        if (!ctx.token_is(token_t::e_comma))
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR035 - Expected ',' between if-statement condition and consequent",
                core::error_location()));
            result = false;
        }
        else if (nullptr == (consequent = ctx.parse_expression()))
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR036 - Failed to parse consequent for if-statement", core::error_location()));
            result = false;
        }
        else if (!ctx.token_is(token_t::e_comma))
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR037 - Expected ',' between if-statement consequent and alternative",
                core::error_location()));
            result = false;
        }
        else if (nullptr == (alternative = ctx.parse_expression()))
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR038 - Failed to parse alternative for if-statement", core::error_location()));
            result = false;
        }
        else if (!ctx.token_is(token_t::e_rbracket))
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR039 - Expected ')' at the end of if-statement", core::error_location()));
            result = false;
        }

#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
        if (result)
        {
            const bool consequent_is_string = is_generally_string_node(consequent);
            const bool alternative_is_string = is_generally_string_node(alternative);

            if (consequent_is_string || alternative_is_string)
            {
                if (consequent_is_string && alternative_is_string)
                {
                    expression_node_ptr result_node =
                        ctx.conditional_string(condition, consequent, alternative);

                    if (result_node && result_node->valid())
                    {
                        return result_node;
                    }

                    ctx.set_error(parser_error::make_error(
                        parser_error::error_mode::e_synthesis, ctx.current_token(),
                        "ERR040 - Failed to synthesize node: conditional_string",
                        core::error_location()));
                    ctx.free_node(result_node);
                    return Context::error_node();
                }

                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, ctx.current_token(),
                    "ERR041 - Return types of if-statement differ: string/non-string",
                    core::error_location()));
                result = false;
            }
        }
#endif

        if (result)
        {
            const bool consequent_is_vector = is_ivector_node(consequent);
            const bool alternative_is_vector = is_ivector_node(alternative);

            if (consequent_is_vector || alternative_is_vector)
            {
                if (consequent_is_vector && alternative_is_vector)
                {
                    return ctx.conditional_vector(condition, consequent, alternative);
                }

                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, ctx.current_token(),
                    "ERR042 - Return types of if-statement differ: vector/non-vector",
                    core::error_location()));
                result = false;
            }
        }

        if (!result)
        {
            ctx.free_node(condition);
            ctx.free_node(consequent);
            ctx.free_node(alternative);
            return Context::error_node();
        }

        return ctx.conditional(condition, consequent, alternative);
    }

    template <typename Context>
    static inline expression_node_ptr parse_conditional_statement_02(Context& ctx,
                                                                     expression_node_ptr condition)
    {
        using token_advance_mode = typename Context::token_advance_mode;

        expression_node_ptr consequent = Context::error_node();
        expression_node_ptr alternative = Context::error_node();
        bool result = true;

        if (ctx.token_is(token_t::e_lcrlbracket, token_advance_mode::e_hold))
        {
            if (nullptr == (consequent = ctx.parse_multi_sequence("if-statement-01")))
            {
                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, ctx.current_token(),
                    "ERR043 - Failed to parse body of consequent for if-statement",
                    core::error_location()));
                result = false;
            }
            else if (!ctx.settings.commutative_check_enabled() &&
                     !ctx.token_is("else", token_advance_mode::e_hold) &&
                     !ctx.token_is_loop(token_advance_mode::e_hold) &&
                     !ctx.token_is_arithmetic_opr(token_advance_mode::e_hold) &&
                     !ctx.token_is_right_bracket(token_advance_mode::e_hold) &&
                     !ctx.token_is_ineq_opr(token_advance_mode::e_hold) &&
                     !ctx.token_is(token_t::e_ternary, token_advance_mode::e_hold) &&
                     !ctx.token_is(token_t::e_eof, token_advance_mode::e_hold))
            {
                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, ctx.current_token(),
                    "ERR044 - Expected ';' at the end of the consequent for if-statement (1)",
                    core::error_location()));
                result = false;
            }
        }
        else
        {
            if (ctx.settings.commutative_check_enabled() &&
                ctx.token_is(token_t::e_mul, token_advance_mode::e_hold))
            {
                ctx.next_token();
            }

            if (nullptr != (consequent = ctx.parse_expression()))
            {
                if (!ctx.token_is(token_t::e_eof, token_advance_mode::e_hold))
                {
                    ctx.set_error(parser_error::make_error(
                        parser_error::error_mode::e_syntax, ctx.current_token(),
                        "ERR045 - Expected ';' at the end of the consequent for if-statement (2)",
                        core::error_location()));
                    result = false;
                }
            }
            else
            {
                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, ctx.current_token(),
                    "ERR046 - Failed to parse body of consequent for if-statement",
                    core::error_location()));
                result = false;
            }
        }

        if (result)
        {
            if (core::imatch(ctx.current_token().value, "else") ||
                (ctx.token_is(token_t::e_eof, token_advance_mode::e_hold) &&
                 ctx.peek_token_is("else")))
            {
                ctx.next_token();

                if (core::imatch(ctx.current_token().value, "else"))
                {
                    ctx.next_token();
                }

                if (ctx.token_is(token_t::e_lcrlbracket, token_advance_mode::e_hold))
                {
                    if (nullptr == (alternative = ctx.parse_multi_sequence("else-statement-01")))
                    {
                        ctx.set_error(parser_error::make_error(
                            parser_error::error_mode::e_syntax, ctx.current_token(),
                            "ERR047 - Failed to parse body of the 'else' for if-statement",
                            core::error_location()));
                        result = false;
                    }
                }
                else if (core::imatch(ctx.current_token().value, "if"))
                {
                    if (nullptr == (alternative = parse_conditional_statement(ctx)))
                    {
                        ctx.set_error(parser_error::make_error(
                            parser_error::error_mode::e_syntax, ctx.current_token(),
                            "ERR048 - Failed to parse body of if-else statement",
                            core::error_location()));
                        result = false;
                    }
                }
                else if (nullptr != (alternative = ctx.parse_expression()))
                {
                    if (!ctx.token_is(token_t::e_ternary, token_advance_mode::e_hold) &&
                        !ctx.token_is(token_t::e_rcrlbracket, token_advance_mode::e_hold) &&
                        !ctx.token_is(token_t::e_eof))
                    {
                        ctx.set_error(parser_error::make_error(
                            parser_error::error_mode::e_syntax, ctx.current_token(),
                            "ERR049 - Expected ';' at the end of the 'else-if' "
                            "for the if-statement",
                            core::error_location()));
                        result = false;
                    }
                }
                else
                {
                    ctx.set_error(parser_error::make_error(
                        parser_error::error_mode::e_syntax, ctx.current_token(),
                        "ERR050 - Failed to parse body of the 'else' for if-statement",
                        core::error_location()));
                    result = false;
                }
            }
        }

#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
        if (result)
        {
            const bool consequent_is_string = is_generally_string_node(consequent);
            const bool alternative_is_string = is_generally_string_node(alternative);

            if (consequent_is_string || alternative_is_string)
            {
                if (consequent_is_string && alternative_is_string)
                {
                    return ctx.conditional_string(condition, consequent, alternative);
                }

                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, ctx.current_token(),
                    "ERR051 - Return types of if-statement differ: string/non-string",
                    core::error_location()));
                result = false;
            }
        }
#endif

        if (result)
        {
            const bool consequent_is_vector = is_ivector_node(consequent);
            const bool alternative_is_vector = is_ivector_node(alternative);

            if (consequent_is_vector || alternative_is_vector)
            {
                if (consequent_is_vector && alternative_is_vector)
                {
                    return ctx.conditional_vector(condition, consequent, alternative);
                }

                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, ctx.current_token(),
                    "ERR052 - Return types of if-statement differ: vector/non-vector",
                    core::error_location()));
                result = false;
            }
        }

        if (!result)
        {
            ctx.free_node(condition);
            ctx.free_node(consequent);
            ctx.free_node(alternative);
            return Context::error_node();
        }

        return ctx.conditional(condition, consequent, alternative);
    }

    template <typename Context>
    static inline expression_node_ptr parse_conditional_statement(Context& ctx)
    {
        using token_advance_mode = typename Context::token_advance_mode;

        expression_node_ptr condition = Context::error_node();
        ctx.next_token();

        if (!ctx.token_is(token_t::e_lbracket))
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR053 - Expected '(' at start of if-statement, instead got: '" +
                    ctx.current_token().value + "'",
                core::error_location()));
            return Context::error_node();
        }
        else if (nullptr == (condition = ctx.parse_expression()))
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR054 - Failed to parse condition for if-statement", core::error_location()));
            return Context::error_node();
        }
        else if (ctx.token_is(token_t::e_comma, token_advance_mode::e_hold))
        {
            return parse_conditional_statement_01(ctx, condition);
        }
        else if (ctx.token_is(token_t::e_rbracket))
        {
            return parse_conditional_statement_02(ctx, condition);
        }

        ctx.set_error(parser_error::make_error(parser_error::error_mode::e_syntax,
                                               ctx.current_token(), "ERR055 - Invalid if-statement",
                                               core::error_location()));
        ctx.free_node(condition);
        return Context::error_node();
    }

    template <typename Context>
    static inline expression_node_ptr parse_ternary_conditional_statement(
        Context& ctx, expression_node_ptr condition)
    {
        expression_node_ptr consequent = Context::error_node();
        expression_node_ptr alternative = Context::error_node();
        bool result = true;

        if (nullptr == condition)
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR056 - Encountered invalid condition branch for ternary if-statement",
                core::error_location()));
            return Context::error_node();
        }
        else if (!ctx.token_is(token_t::e_ternary))
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR057 - Expected '?' after condition of ternary if-statement",
                core::error_location()));
            result = false;
        }
        else if (nullptr == (consequent = ctx.parse_expression()))
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR058 - Failed to parse consequent for ternary if-statement",
                core::error_location()));
            result = false;
        }
        else if (!ctx.token_is(token_t::e_colon))
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR059 - Expected ':' between ternary if-statement consequent and alternative",
                core::error_location()));
            result = false;
        }
        else if (nullptr == (alternative = ctx.parse_expression()))
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR060 - Failed to parse alternative for ternary if-statement",
                core::error_location()));
            result = false;
        }

#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
        if (result)
        {
            const bool consequent_is_string = is_generally_string_node(consequent);
            const bool alternative_is_string = is_generally_string_node(alternative);

            if (consequent_is_string || alternative_is_string)
            {
                if (consequent_is_string && alternative_is_string)
                {
                    return ctx.conditional_string(condition, consequent, alternative);
                }

                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, ctx.current_token(),
                    "ERR061 - Return types of ternary differ: string/non-string",
                    core::error_location()));
                result = false;
            }
        }
#endif

        if (result)
        {
            const bool consequent_is_vector = is_ivector_node(consequent);
            const bool alternative_is_vector = is_ivector_node(alternative);

            if (consequent_is_vector || alternative_is_vector)
            {
                if (consequent_is_vector && alternative_is_vector)
                {
                    return ctx.conditional_vector(condition, consequent, alternative);
                }

                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, ctx.current_token(),
                    "ERR062 - Return types of ternary differ: vector/non-vector",
                    core::error_location()));
                result = false;
            }
        }

        if (!result)
        {
            ctx.free_node(condition);
            ctx.free_node(consequent);
            ctx.free_node(alternative);
            return Context::error_node();
        }

        return ctx.conditional(condition, consequent, alternative);
    }

    template <typename Context>
    static inline expression_node_ptr parse_while_loop(Context& ctx)
    {
        expression_node_ptr condition = Context::error_node();
        expression_node_ptr branch = Context::error_node();
        expression_node_ptr result_node = Context::error_node();
        bool result = true;

        ctx.next_token();

        if (!ctx.token_is(token_t::e_lbracket))
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR064 - Expected '(' at start of while-loop condition statement",
                core::error_location()));
            return Context::error_node();
        }
        else if (nullptr == (condition = ctx.parse_expression()))
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR065 - Failed to parse condition for while-loop", core::error_location()));
            return Context::error_node();
        }
        else if (!ctx.token_is(token_t::e_rbracket))
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR066 - Expected ')' at end of while-loop condition statement",
                core::error_location()));
            result = false;
        }

        ctx.brkcnt_list.push_front(false);

        if (result)
        {
            scoped_counter_increment<Context> loop_counter(ctx);

            if (nullptr == (branch = ctx.parse_multi_sequence("while-loop", true)))
            {
                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, ctx.current_token(),
                    "ERR067 - Failed to parse body of while-loop"));
                result = false;
            }
            else if (nullptr ==
                     (result_node = ctx.while_loop(condition, branch, ctx.brkcnt_list.front())))
            {
                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, ctx.current_token(),
                    "ERR068 - Failed to synthesize while-loop", core::error_location()));
                result = false;
            }
        }

        ctx.handle_brkcnt_scope_exit();

        if (!result)
        {
            ctx.free_node(branch);
            ctx.free_node(condition);
            ctx.free_node(result_node);
            return Context::error_node();
        }

        if (result_node && result_node->valid())
        {
            return result_node;
        }

        ctx.set_error(parser_error::make_error(
            parser_error::error_mode::e_synthesis, ctx.current_token(),
            "ERR069 - Failed to synthesize 'valid' while-loop", core::error_location()));
        ctx.free_node(result_node);
        return Context::error_node();
    }

    template <typename Context>
    static inline expression_node_ptr parse_repeat_until_loop(Context& ctx)
    {
        using token_advance_mode = typename Context::token_advance_mode;

        expression_node_ptr condition = Context::error_node();
        expression_node_ptr branch = Context::error_node();
        ctx.next_token();

        std::vector<expression_node_ptr> arg_list;
        std::vector<bool> side_effect_list;
        scoped_node_vector_delete<Context> scoped_delete(ctx, arg_list);

        ctx.brkcnt_list.push_front(false);

        if (core::imatch(ctx.current_token().value, "until"))
        {
            ctx.next_token();
            branch = ctx.make_null_node();
        }
        else
        {
            static constexpr token_t::token_type separator = token_t::e_eof;

            scope_handler_t scope_handler(ctx.state.scope_depth, ctx.sem);
            scoped_bool_restore<Context> side_effect_restore(ctx);
            scoped_counter_increment<Context> loop_counter(ctx);

            for (;;)
            {
                ctx.state.side_effect_present = false;
                expression_node_ptr arg = ctx.parse_expression();

                if (nullptr == arg)
                    return Context::error_node();

                arg_list.push_back(arg);
                side_effect_list.push_back(ctx.state.side_effect_present);

                if (core::imatch(ctx.current_token().value, "until"))
                {
                    ctx.next_token();
                    break;
                }

                const bool next_is_until =
                    ctx.peek_token_is(token_t::e_symbol) && ctx.peek_token_is("until");

                if (!ctx.token_is(separator) && next_is_until)
                {
                    ctx.set_error(parser_error::make_error(
                        parser_error::error_mode::e_syntax, ctx.current_token(),
                        "ERR070 - Expected '" + token_t::to_str(separator) +
                            "' in body of repeat until loop",
                        core::error_location()));
                    return Context::error_node();
                }

                if (core::imatch(ctx.current_token().value, "until"))
                {
                    ctx.next_token();
                    break;
                }
            }

            branch = ctx.simplify(arg_list, side_effect_list);
            scoped_delete.delete_ptr = (nullptr == branch);

            if (scoped_delete.delete_ptr)
            {
                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, ctx.current_token(),
                    "ERR071 - Failed to parse body of repeat until loop", core::error_location()));
                return Context::error_node();
            }
        }

        if (!ctx.token_is(token_t::e_lbracket))
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR072 - Expected '(' before condition statement of repeat until loop",
                core::error_location()));
            ctx.free_node(branch);
            return Context::error_node();
        }
        else if (nullptr == (condition = ctx.parse_expression()))
        {
            ctx.set_error(
                parser_error::make_error(parser_error::error_mode::e_syntax, ctx.current_token(),
                                         "ERR073 - Failed to parse condition for repeat until loop",
                                         core::error_location()));
            ctx.free_node(branch);
            return Context::error_node();
        }
        else if (!ctx.token_is(token_t::e_rbracket))
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR074 - Expected ')' after condition of repeat until loop",
                core::error_location()));
            ctx.free_node(branch);
            ctx.free_node(condition);
            return Context::error_node();
        }

        expression_node_ptr result_node =
            ctx.repeat_until_loop(condition, branch, ctx.brkcnt_list.front());

        if (nullptr == result_node)
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR075 - Failed to synthesize repeat until loop", core::error_location()));
            ctx.free_node(condition);
            return Context::error_node();
        }

        ctx.handle_brkcnt_scope_exit();

        if (result_node && result_node->valid())
        {
            return result_node;
        }

        ctx.set_error(parser_error::make_error(
            parser_error::error_mode::e_synthesis, ctx.current_token(),
            "ERR076 - Failed to synthesize 'valid' repeat until loop", core::error_location()));
        ctx.free_node(result_node);
        return Context::error_node();
    }

    template <typename Context>
    static inline expression_node_ptr parse_for_loop(Context& ctx)
    {
        using token_advance_mode = typename Context::token_advance_mode;

        expression_node_ptr initialiser = Context::error_node();
        expression_node_ptr condition = Context::error_node();
        expression_node_ptr incrementor = Context::error_node();
        expression_node_ptr loop_body = Context::error_node();

        scope_element_t* scope_element = nullptr;
        bool result = true;

        ctx.next_token();
        scope_handler_t scope_handler(ctx.state.scope_depth, ctx.sem);

        if (!ctx.token_is(token_t::e_lbracket))
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR077 - Expected '(' at start of for-loop", core::error_location()));
            return Context::error_node();
        }

        if (!ctx.token_is(token_t::e_eof))
        {
            if (!ctx.token_is(token_t::e_symbol, token_advance_mode::e_hold) &&
                core::imatch(ctx.current_token().value, "var"))
            {
                ctx.next_token();

                if (!ctx.token_is(token_t::e_symbol, token_advance_mode::e_hold))
                {
                    ctx.set_error(parser_error::make_error(
                        parser_error::error_mode::e_syntax, ctx.current_token(),
                        "ERR078 - Expected a variable at the start of initialiser "
                        "section of for-loop",
                        core::error_location()));
                    return Context::error_node();
                }
                else if (!ctx.peek_token_is(token_t::e_assign))
                {
                    ctx.set_error(parser_error::make_error(
                        parser_error::error_mode::e_syntax, ctx.current_token(),
                        "ERR079 - Expected variable assignment of initialiser section of "
                        "for-loop",
                        core::error_location()));
                    return Context::error_node();
                }

                const std::string loop_counter_symbol = ctx.current_token().value;
                scope_element = &ctx.sem.get_element(loop_counter_symbol);

                if ((scope_element->name == loop_counter_symbol) && scope_element->active)
                {
                    ctx.set_error(parser_error::make_error(
                        parser_error::error_mode::e_syntax, ctx.current_token(),
                        "ERR080 - For-loop variable '" + loop_counter_symbol +
                            "' is being shadowed by a previous declaration",
                        core::error_location()));
                    return Context::error_node();
                }
                else if (!ctx.symtab_store.is_variable(loop_counter_symbol))
                {
                    if (!scope_element->active && (scope_element->name == loop_counter_symbol) &&
                        (scope_element->type == scope_element_t::element_type::e_variable))
                    {
                        scope_element->active = true;
                        scope_element->ref_count++;
                    }
                    else
                    {
                        scope_element_t new_scope_element;
                        new_scope_element.name = loop_counter_symbol;
                        new_scope_element.active = true;
                        new_scope_element.ref_count = 1;
                        new_scope_element.type = scope_element_t::element_type::e_variable;
                        new_scope_element.depth = ctx.state.scope_depth;
                        new_scope_element.scalar_data = std::make_unique<T>(T(0));
                        new_scope_element.var_node =
                            static_cast<typename scope_element_t::variable_node_ptr>(
                                ctx.make_variable_node(*new_scope_element.scalar_data));

                        if (!ctx.sem.add_element(std::move(new_scope_element)))
                        {
                            ctx.set_error(parser_error::make_error(
                                parser_error::error_mode::e_syntax, ctx.current_token(),
                                "ERR081 - Failed to add new local variable '" +
                                    loop_counter_symbol + "' to SEM",
                                core::error_location()));

                            ctx.sem.free_element(new_scope_element);
                            result = false;
                        }
                        else
                        {
                            core::debug_print(
                                "parse_for_loop() - INFO - Added new local variable: %s\n",
                                new_scope_element.name.c_str());
                            ctx.activate_side_effect("parse_for_loop()");
                        }
                    }
                }
            }

            if (nullptr == (initialiser = ctx.parse_expression()))
            {
                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, ctx.current_token(),
                    "ERR082 - Failed to parse initialiser of for-loop", core::error_location()));
                result = false;
            }
            else if (!ctx.token_is(token_t::e_eof))
            {
                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, ctx.current_token(),
                    "ERR083 - Expected ';' after initialiser of for-loop", core::error_location()));
                result = false;
            }
        }

        if (!ctx.token_is(token_t::e_eof))
        {
            if (nullptr == (condition = ctx.parse_expression()))
            {
                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, ctx.current_token(),
                    "ERR084 - Failed to parse condition of for-loop", core::error_location()));
                result = false;
            }
            else if (!ctx.token_is(token_t::e_eof))
            {
                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, ctx.current_token(),
                    "ERR085 - Expected ';' after condition section of for-loop",
                    core::error_location()));
                result = false;
            }
        }

        if (!ctx.token_is(token_t::e_rbracket))
        {
            if (nullptr == (incrementor = ctx.parse_expression()))
            {
                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, ctx.current_token(),
                    "ERR086 - Failed to parse incrementor of for-loop", core::error_location()));
                result = false;
            }
            else if (!ctx.token_is(token_t::e_rbracket))
            {
                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, ctx.current_token(),
                    "ERR087 - Expected ')' after incrementor section of for-loop",
                    core::error_location()));
                result = false;
            }
        }

        if (result)
        {
            ctx.brkcnt_list.push_front(false);
            scoped_counter_increment<Context> loop_counter(ctx);

            if (nullptr == (loop_body = ctx.parse_multi_sequence("for-loop", true)))
            {
                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, ctx.current_token(),
                    "ERR088 - Failed to parse body of for-loop", core::error_location()));
                result = false;
            }
        }

        if (!result)
        {
            if (scope_element)
            {
                scope_element->ref_count--;
            }

            ctx.free_node(initialiser);
            ctx.free_node(condition);
            ctx.free_node(incrementor);
            ctx.free_node(loop_body);
            return Context::error_node();
        }

        expression_node_ptr result_node =
            ctx.for_loop(initialiser, condition, incrementor, loop_body, ctx.brkcnt_list.front());
        ctx.handle_brkcnt_scope_exit();

        if (result_node && result_node->valid())
        {
            return result_node;
        }

        ctx.set_error(parser_error::make_error(
            parser_error::error_mode::e_synthesis, ctx.current_token(),
            "ERR089 - Failed to synthesize 'valid' for-loop", core::error_location()));
        ctx.free_node(result_node);
        return Context::error_node();
    }
};
}  // namespace math_expr

#endif
