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

#ifndef MATH_EXPR_PARSER_FUNCTION_CALL_PARSER_HPP
#define MATH_EXPR_PARSER_FUNCTION_CALL_PARSER_HPP

namespace math_expr
{
template <typename T>
class parser_function_call
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
    static inline expression_node_ptr parse_function_invocation(Context& ctx,
                                                                ifunction<T>* function,
                                                                const std::string& function_name)
    {
        expression_node_ptr func_node = nullptr;

        switch (function->param_count)
        {
            case 0:
                func_node = parse_function_call_0(ctx, function, function_name);
                break;
            case 1:
                func_node = parse_function_call<1>(ctx, function, function_name);
                break;
            case 2:
                func_node = parse_function_call<2>(ctx, function, function_name);
                break;
            case 3:
                func_node = parse_function_call<3>(ctx, function, function_name);
                break;
            case 4:
                func_node = parse_function_call<4>(ctx, function, function_name);
                break;
            case 5:
                func_node = parse_function_call<5>(ctx, function, function_name);
                break;
            case 6:
                func_node = parse_function_call<6>(ctx, function, function_name);
                break;
            case 7:
                func_node = parse_function_call<7>(ctx, function, function_name);
                break;
            case 8:
                func_node = parse_function_call<8>(ctx, function, function_name);
                break;
            case 9:
                func_node = parse_function_call<9>(ctx, function, function_name);
                break;
            case 10:
                func_node = parse_function_call<10>(ctx, function, function_name);
                break;
            case 11:
                func_node = parse_function_call<11>(ctx, function, function_name);
                break;
            case 12:
                func_node = parse_function_call<12>(ctx, function, function_name);
                break;
            case 13:
                func_node = parse_function_call<13>(ctx, function, function_name);
                break;
            case 14:
                func_node = parse_function_call<14>(ctx, function, function_name);
                break;
            case 15:
                func_node = parse_function_call<15>(ctx, function, function_name);
                break;
            case 16:
                func_node = parse_function_call<16>(ctx, function, function_name);
                break;
            case 17:
                func_node = parse_function_call<17>(ctx, function, function_name);
                break;
            case 18:
                func_node = parse_function_call<18>(ctx, function, function_name);
                break;
            case 19:
                func_node = parse_function_call<19>(ctx, function, function_name);
                break;
            case 20:
                func_node = parse_function_call<20>(ctx, function, function_name);
                break;
            default:
            {
                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, ctx.current_token(),
                    "ERR021 - Invalid number of parameters for function: '" + function_name + "'",
                    core::error_location()));

                return Context::error_node();
            }
        }

        if (func_node)
        {
            return func_node;
        }

        ctx.set_error(parser_error::make_error(
            parser_error::error_mode::e_syntax, ctx.current_token(),
            "ERR022 - Failed to generate call to function: '" + function_name + "'",
            core::error_location()));

        return Context::error_node();
    }

    template <std::size_t NumberofParameters, typename Context>
    static inline expression_node_ptr parse_function_call(Context& ctx, ifunction<T>* function,
                                                          const std::string& function_name)
    {
        if constexpr (0 == NumberofParameters)
        {
            ctx.set_error(
                parser_error::make_error(parser_error::error_mode::e_syntax, ctx.current_token(),
                                         "ERR023 - Expecting ifunction '" + function_name +
                                             "' to have non-zero parameter "
                                             "count",
                                         core::error_location()));

            return Context::error_node();
        }
        else
        {
            expression_node_ptr branch[NumberofParameters];
            std::fill_n(branch, NumberofParameters, nullptr);

            scoped_node_array_delete<Context, NumberofParameters> scoped_branch(ctx, branch);

            ctx.next_token();

            if (!ctx.token_is(token_t::e_lbracket))
            {
                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, ctx.current_token(),
                    "ERR024 - Expecting argument list for function: '" + function_name + "'",
                    core::error_location()));

                return Context::error_node();
            }

            for (int i = 0; i < static_cast<int>(NumberofParameters); ++i)
            {
                branch[i] = ctx.parse_expression();

                if (nullptr == branch[i])
                {
                    ctx.set_error(parser_error::make_error(
                        parser_error::error_mode::e_syntax, ctx.current_token(),
                        "ERR025 - Failed to parse argument " + core::to_str(i) +
                            " for function: '" + function_name + "'",
                        core::error_location()));

                    return Context::error_node();
                }
                else if (i < static_cast<int>(NumberofParameters - 1))
                {
                    if (!ctx.token_is(token_t::e_comma))
                    {
                        ctx.set_error(parser_error::make_error(
                            parser_error::error_mode::e_syntax, ctx.current_token(),
                            "ERR026 - Invalid number of arguments for function: '" + function_name +
                                "'",
                            core::error_location()));

                        return Context::error_node();
                    }
                }
            }

            if (!ctx.token_is(token_t::e_rbracket))
            {
                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, ctx.current_token(),
                    "ERR027 - Invalid number of arguments for function: '" + function_name + "'",
                    core::error_location()));

                return Context::error_node();
            }

            expression_node_ptr result = ctx.function(function, branch);
            scoped_branch.delete_ptr = (nullptr == result);
            return result;
        }
    }

    template <typename Context>
    static inline expression_node_ptr parse_function_call_0(Context& ctx, ifunction<T>* function,
                                                            const std::string& function_name)
    {
        expression_node_ptr result = ctx.function(function);
        ctx.state.side_effect_present = function->has_side_effects();

        ctx.next_token();

        if (ctx.token_is(token_t::e_lbracket) && !ctx.token_is(token_t::e_rbracket))
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR028 - Expecting '()' to proceed call to function: '" + function_name + "'",
                core::error_location()));

            ctx.free_node(result);
            return Context::error_node();
        }

        return result;
    }

    template <std::size_t MaxNumberofParameters, typename Context>
    static inline std::size_t parse_base_function_call(
        Context& ctx, expression_node_ptr (&param_list)[MaxNumberofParameters],
        const std::string& function_name = "")
    {
        std::fill_n(param_list, MaxNumberofParameters, nullptr);
        scoped_node_array_delete<Context, MaxNumberofParameters> scoped_params(ctx, param_list);

        ctx.next_token();

        if (!ctx.token_is(token_t::e_lbracket))
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR029 - Expected a '(' at start of function call to '" + function_name +
                    "', instead got: '" + ctx.current_token().value + "'",
                core::error_location()));

            return 0;
        }

        if (ctx.token_is(token_t::e_rbracket, Context::token_advance_mode::e_hold))
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR030 - Expected at least one input parameter for function call '" +
                    function_name + "'",
                core::error_location()));

            return 0;
        }

        std::size_t param_index = 0;

        for (; param_index < MaxNumberofParameters; ++param_index)
        {
            param_list[param_index] = ctx.parse_expression();

            if (nullptr == param_list[param_index])
            {
                return 0;
            }
            else if (ctx.token_is(token_t::e_rbracket))
            {
                scoped_params.delete_ptr = false;
                break;
            }
            else if (ctx.token_is(token_t::e_comma))
            {
                continue;
            }
            else
            {
                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, ctx.current_token(),
                    "ERR031 - Expected a ',' between function input parameters, instead got: '" +
                        ctx.current_token().value + "'",
                    core::error_location()));

                return 0;
            }
        }

        if (scoped_params.delete_ptr)
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR032 - Invalid number of input parameters passed to function '" + function_name +
                    "'",
                core::error_location()));

            return 0;
        }

        return (param_index + 1);
    }

    template <typename Context>
    static inline expression_node_ptr parse_base_operation(Context& ctx)
    {
        using map_range_t = std::pair<typename Context::base_ops_map_t::iterator,
                                      typename Context::base_ops_map_t::iterator>;

        const std::string operation_name = ctx.current_token().value;
        const token_t diagnostic_token = ctx.current_token();

        map_range_t itr_range = ctx.base_ops_map.equal_range(operation_name);

        if (0 == std::distance(itr_range.first, itr_range.second))
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, diagnostic_token,
                "ERR033 - No entry found for base operation: " + operation_name,
                core::error_location()));

            return Context::error_node();
        }

        static constexpr std::size_t MaxNumberofParameters = 4;
        expression_node_ptr param_list[MaxNumberofParameters] = {0};

        const std::size_t parameter_count =
            parse_base_function_call(ctx, param_list, operation_name);

        if ((parameter_count > 0) && (parameter_count <= MaxNumberofParameters))
        {
            for (typename Context::base_ops_map_t::iterator itr = itr_range.first;
                 itr != itr_range.second; ++itr)
            {
                const core::operators::base_operation_t& operation = itr->second;

                if (operation.num_params == parameter_count)
                {
                    switch (parameter_count)
                    {
                        case 1:
                        {
                            expression_node_ptr pl[1] = {param_list[0]};
                            ctx.lodge_symbol(operation_name, symbol_type::e_st_function);
                            return ctx.base_operation(operation.type, pl);
                        }
                        case 2:
                        {
                            expression_node_ptr pl[2] = {param_list[0], param_list[1]};
                            ctx.lodge_symbol(operation_name, symbol_type::e_st_function);
                            return ctx.base_operation(operation.type, pl);
                        }
                        case 3:
                        {
                            expression_node_ptr pl[3] = {param_list[0], param_list[1],
                                                         param_list[2]};
                            ctx.lodge_symbol(operation_name, symbol_type::e_st_function);
                            return ctx.base_operation(operation.type, pl);
                        }
                        case 4:
                        {
                            expression_node_ptr pl[4] = {param_list[0], param_list[1],
                                                         param_list[2], param_list[3]};
                            ctx.lodge_symbol(operation_name, symbol_type::e_st_function);
                            return ctx.base_operation(operation.type, pl);
                        }
                    }
                }
            }
        }

        for (std::size_t i = 0; i < MaxNumberofParameters; ++i)
        {
            ctx.free_node(param_list[i]);
        }

        ctx.set_error(parser_error::make_error(
            parser_error::error_mode::e_syntax, diagnostic_token,
            "ERR034 - Invalid number of input parameters for call to function: '" + operation_name +
                "'",
            core::error_location()));

        return Context::error_node();
    }
};
}  // namespace math_expr

#endif
