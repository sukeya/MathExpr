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

#ifndef MATH_EXPR_PARSER_SYMBOL_RESOLUTION_PARSER_HPP
#define MATH_EXPR_PARSER_SYMBOL_RESOLUTION_PARSER_HPP

namespace math_expr
{
template <typename T>
class parser_symbol_resolution
{
   public:
    using expression_node_ptr = details::expression_node<T>*;

    template <typename Context>
    static inline expression_node_ptr parse_symtab_symbol(Context& ctx)
    {
        const std::string symbol = ctx.current_token().value;

        typename Context::variable_context_t var_ctx = ctx.get_variable_context(symbol);

        if (var_ctx.variable)
        {
            assert(var_ctx.symbol_table);

            expression_node_ptr result_variable = var_ctx.variable;

            if (ctx.is_constant_node(symbol))
            {
                result_variable = ctx.make_numeric_literal(var_ctx.variable->value());
            }
            else if (Context::symbol_table_t::symtab_mutability_type::e_immutable ==
                     var_ctx.symbol_table->mutability())
            {
                ctx.lodge_immutable_variable_symbol(ctx.current_token(), var_ctx.variable->ref());
                result_variable = var_ctx.variable;
            }

            if (!ctx.post_variable_process(symbol))
                return Context::error_node();

            ctx.lodge_symbol(symbol, symbol_type::e_st_variable);
            ctx.next_token();
            return result_variable;
        }

        if (!ctx.scope_empty())
        {
            typename Context::scope_element_t& se = ctx.get_active_element(symbol);

            if (se.active && core::imatch(se.name, symbol))
            {
                if ((Context::scope_element_t::element_type::e_variable == se.type) ||
                    (Context::scope_element_t::element_type::e_literal == se.type))
                {
                    se.active = true;
                    ctx.lodge_symbol(symbol, symbol_type::e_st_local_variable);

                    if (!ctx.post_variable_process(symbol))
                        return Context::error_node();

                    ctx.next_token();
                    return (Context::scope_element_t::element_type::e_variable == se.type)
                               ? se.var_node
                               : ctx.make_numeric_literal(se.var_node->value());
                }
                else if (Context::scope_element_t::element_type::e_vector == se.type)
                {
                    return ctx.parse_vector();
                }
            }
        }

        if (ifunction<T>* function = ctx.get_function(symbol))
        {
            ctx.lodge_symbol(symbol, symbol_type::e_st_function);

            expression_node_ptr function_node = ctx.parse_function_invocation(function, symbol);

            if (function_node)
                return function_node;

            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR231 - Failed to generate node for function: '" + symbol + "'",
                core::error_location()));
            return Context::error_node();
        }

        if (ivararg_function<T>* function = ctx.get_vararg_function(symbol))
        {
            ctx.lodge_symbol(symbol, symbol_type::e_st_function);

            expression_node_ptr function_node = ctx.parse_vararg_function_call(function, symbol);

            if (function_node)
                return function_node;

            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR232 - Failed to generate node for vararg function: '" + symbol + "'",
                core::error_location()));
            return Context::error_node();
        }

        if (igeneric_function<T>* function = ctx.get_generic_function(symbol))
        {
            ctx.lodge_symbol(symbol, symbol_type::e_st_function);

            expression_node_ptr function_node = ctx.parse_generic_function_call(function, symbol);

            if (function_node)
                return function_node;

            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR233 - Failed to generate node for generic function: '" + symbol + "'",
                core::error_location()));
            return Context::error_node();
        }

        if (ctx.is_vector(symbol))
        {
            ctx.lodge_symbol(symbol, symbol_type::e_st_vector);
            return ctx.parse_vector();
        }

        if (core::is_reserved_symbol(symbol))
        {
            if (ctx.settings.function_enabled(symbol) || !core::is_base_function(symbol))
            {
                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, ctx.current_token(),
                    "ERR236 - Invalid use of reserved symbol '" + symbol + "'",
                    core::error_location()));

                return Context::error_node();
            }
        }

        if (ctx.resolve_unknown_symbol && ctx.unknown_symbol_resolver)
        {
            if (!(ctx.settings.rsrvd_sym_usr_disabled() && core::is_reserved_symbol(symbol)))
            {
                typename Context::symbol_table_t& symtab = ctx.get_symbol_table();
                std::string error_message;

                if (Context::unknown_symbol_resolver_t::usr_mode::e_usrmode_default ==
                    ctx.unknown_symbol_resolver->mode)
                {
                    T default_value = T(0);
                    typename Context::unknown_symbol_resolver_t::usr_symbol_type usr_symbol_type =
                        Context::unknown_symbol_resolver_t::usr_symbol_type::e_usr_unknown_type;

                    if (ctx.unknown_symbol_resolver->process(symbol, usr_symbol_type, default_value,
                                                             error_message))
                    {
                        bool create_result = false;

                        switch (usr_symbol_type)
                        {
                            case Context::unknown_symbol_resolver_t::usr_symbol_type::
                                e_usr_variable_type:
                                create_result = symtab.create_variable(symbol, default_value);
                                break;

                            case Context::unknown_symbol_resolver_t::usr_symbol_type::
                                e_usr_constant_type:
                                create_result = symtab.add_constant(symbol, default_value);
                                break;

                            default:
                                create_result = false;
                        }

                        if (create_result)
                        {
                            expression_node_ptr variable = ctx.get_variable(symbol);

                            if (variable)
                            {
                                if (ctx.is_constant_node(symbol))
                                {
                                    variable = ctx.make_numeric_literal(variable->value());
                                }

                                ctx.lodge_symbol(symbol, symbol_type::e_st_variable);

                                if (!ctx.post_variable_process(symbol))
                                    return Context::error_node();

                                ctx.next_token();
                                return variable;
                            }
                        }
                    }

                    ctx.set_error(parser_error::make_error(
                        parser_error::error_mode::e_symtab, ctx.current_token(),
                        "ERR237 - Failed to create variable: '" + symbol + "'" +
                            (error_message.empty() ? "" : " - " + error_message),
                        core::error_location()));
                }
                else if (Context::unknown_symbol_resolver_t::usr_mode::e_usrmode_extended ==
                         ctx.unknown_symbol_resolver->mode)
                {
                    if (ctx.unknown_symbol_resolver->process(symbol, symtab, error_message))
                    {
                        expression_node_ptr result = parse_symtab_symbol(ctx);

                        if (result)
                            return result;
                    }

                    ctx.set_error(parser_error::make_error(
                        parser_error::error_mode::e_symtab, ctx.current_token(),
                        "ERR238 - Failed to resolve symbol: '" + symbol + "'" +
                            (error_message.empty() ? "" : " - " + error_message),
                        core::error_location()));
                }

                return Context::error_node();
            }
        }

        ctx.set_error(parser_error::make_error(
            parser_error::error_mode::e_syntax, ctx.current_token(),
            "ERR239 - Undefined symbol: '" + symbol + "'", core::error_location()));

        return Context::error_node();
    }
};
}  // namespace math_expr

#endif
