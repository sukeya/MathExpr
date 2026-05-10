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

#ifndef MATH_EXPR_PARSER_DYNAMIC_FUNCTION_PARSER_HPP
#define MATH_EXPR_PARSER_DYNAMIC_FUNCTION_PARSER_HPP

namespace math_expr
{
template <typename T>
class parser_dynamic_function
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

    template <typename Context>
    static inline expression_node_ptr parse_vararg_function_call(
        Context& ctx, ivararg_function<T>* vararg_function, const std::string& function_name)
    {
        std::vector<expression_node_ptr> arg_list;
        scoped_node_vector_delete<Context> scoped_args(ctx, arg_list);

        ctx.next_token();

        if (ctx.token_is(token_t::e_lbracket))
        {
            if (ctx.token_is(token_t::e_rbracket))
            {
                if (!vararg_function->allow_zero_parameters())
                {
                    ctx.set_error(parser_error::make_error(
                        parser_error::error_mode::e_syntax, ctx.current_token(),
                        "ERR127 - Zero parameter call to vararg function: " + function_name +
                            " not allowed",
                        core::error_location()));

                    return Context::error_node();
                }
            }
            else
            {
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
                            "ERR128 - Expected ',' for call to vararg function: " + function_name,
                            core::error_location()));

                        return Context::error_node();
                    }
                }
            }
        }
        else if (!vararg_function->allow_zero_parameters())
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR129 - Zero parameter call to vararg function: " + function_name +
                    " not allowed",
                core::error_location()));

            return Context::error_node();
        }

        if (arg_list.size() < vararg_function->min_num_args())
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR130 - Invalid number of parameters to call to vararg function: " +
                    function_name + ", require at least " +
                    core::to_str(static_cast<int>(vararg_function->min_num_args())) + " parameters",
                core::error_location()));

            return Context::error_node();
        }
        else if (arg_list.size() > vararg_function->max_num_args())
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR131 - Invalid number of parameters to call to vararg function: " +
                    function_name + ", require no more than " +
                    core::to_str(static_cast<int>(vararg_function->max_num_args())) + " parameters",
                core::error_location()));

            return Context::error_node();
        }

        expression_node_ptr result = ctx.vararg_function_call(vararg_function, arg_list);
        scoped_args.delete_ptr = (nullptr == result);
        return result;
    }

    template <typename Context>
    static inline bool parse_igeneric_function_params(Context& ctx, std::string& param_type_list,
                                                      std::vector<expression_node_ptr>& arg_list,
                                                      const std::string& function_name,
                                                      igeneric_function<T>* function,
                                                      const typename Context::type_checker_t& tc)
    {
        if (ctx.token_is(token_t::e_lbracket))
        {
            if (ctx.token_is(token_t::e_rbracket))
            {
                if (!function->allow_zero_parameters() && !tc.allow_zero_parameters())
                {
                    ctx.set_error(parser_error::make_error(
                        parser_error::error_mode::e_syntax, ctx.current_token(),
                        "ERR141 - Zero parameter call to generic function: " + function_name +
                            " not allowed",
                        core::error_location()));

                    return false;
                }
            }
            else
            {
                for (;;)
                {
                    expression_node_ptr arg = ctx.parse_expression();

                    if (nullptr == arg)
                    {
                        return false;
                    }

                    if (is_ivector_node(arg))
                        param_type_list += 'V';
                    else if (is_generally_string_node(arg))
                        param_type_list += 'S';
                    else
                        param_type_list += 'T';

                    arg_list.push_back(arg);

                    if (ctx.token_is(token_t::e_rbracket))
                    {
                        break;
                    }
                    else if (!ctx.token_is(token_t::e_comma))
                    {
                        ctx.set_error(parser_error::make_error(
                            parser_error::error_mode::e_syntax, ctx.current_token(),
                            "ERR142 - Expected ',' for call to string function: " + function_name,
                            core::error_location()));

                        return false;
                    }
                }
            }

            return true;
        }

        return false;
    }

    template <typename Context>
    static inline expression_node_ptr parse_generic_function_call(Context& ctx,
                                                                  igeneric_function<T>* function,
                                                                  const std::string& function_name)
    {
        std::vector<expression_node_ptr> arg_list;
        scoped_node_vector_delete<Context> scoped_args(ctx, arg_list);

        ctx.next_token();

        std::string param_type_list;
        typename Context::type_checker_t tc(ctx.parser_, function_name,
                                            function->parameter_sequence,
                                            Context::type_checker_t::e_string);

        if (tc.invalid())
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR136 - Type checker instantiation failure for generic function: " +
                    function_name,
                core::error_location()));

            return Context::error_node();
        }

        if (ctx.token_is(token_t::e_lbracket))
        {
            if (ctx.token_is(token_t::e_rbracket))
            {
                if (!function->allow_zero_parameters() && !tc.allow_zero_parameters())
                {
                    ctx.set_error(parser_error::make_error(
                        parser_error::error_mode::e_syntax, ctx.current_token(),
                        "ERR137 - Zero parameter call to generic function: " + function_name +
                            " not allowed",
                        core::error_location()));

                    return Context::error_node();
                }
            }
            else
            {
                for (;;)
                {
                    expression_node_ptr arg = ctx.parse_expression();

                    if (nullptr == arg)
                    {
                        return Context::error_node();
                    }

                    if (is_ivector_node(arg))
                        param_type_list += 'V';
                    else if (is_generally_string_node(arg))
                        param_type_list += 'S';
                    else
                        param_type_list += 'T';

                    arg_list.push_back(arg);

                    if (ctx.token_is(token_t::e_rbracket))
                    {
                        break;
                    }
                    else if (!ctx.token_is(token_t::e_comma))
                    {
                        ctx.set_error(parser_error::make_error(
                            parser_error::error_mode::e_syntax, ctx.current_token(),
                            "ERR138 - Expected ',' for call to generic function: " + function_name,
                            core::error_location()));

                        return Context::error_node();
                    }
                }
            }
        }
        else if (!function->parameter_sequence.empty() && function->allow_zero_parameters() &&
                 !tc.allow_zero_parameters())
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR139 - Zero parameter call to generic function: " + function_name +
                    " not allowed",
                core::error_location()));

            return Context::error_node();
        }

        std::size_t param_seq_index = 0;

        if (ctx.state.type_check_enabled && !tc.verify(param_type_list, param_seq_index))
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR140 - Invalid input parameter sequence for call to generic function: " +
                    function_name,
                core::error_location()));

            return Context::error_node();
        }

        expression_node_ptr result =
            (tc.paramseq_count() <= 1)
                ? ctx.generic_function_call(function, arg_list)
                : ctx.generic_function_call(function, arg_list, param_seq_index);

        scoped_args.delete_ptr = (nullptr == result);
        return result;
    }
};
}  // namespace math_expr

#endif
