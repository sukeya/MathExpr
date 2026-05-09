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

#ifndef MATH_EXPR_PARSER_DEFINITION_PARSER_HPP
#define MATH_EXPR_PARSER_DEFINITION_PARSER_HPP

namespace math_expr
{
template <typename T>
class parser_definition
{
   public:
    using expression_node_ptr = details::expression_node<T>*;
    using token_t = lexer::token;

    template <typename Context>
    static inline bool local_variable_is_shadowed(Context& ctx, const std::string& symbol)
    {
        const typename Context::scope_element_t& se = ctx.get_element(symbol);
        return (se.name == symbol) && se.active;
    }

#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
    template <typename Context>
    static inline expression_node_ptr parse_define_string_statement(
        Context& ctx, const std::string& str_name, expression_node_ptr initialisation_expression)
    {
        typename Context::stringvar_node_ptr str_node = nullptr;

        typename Context::scope_element_t& se = ctx.get_element(str_name);

        if (se.name == str_name)
        {
            if (se.active)
            {
                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, ctx.current_token(),
                    "ERR175 - Illegal redefinition of local variable: '" + str_name + "'",
                    core::error_location()));

                ctx.free_node(initialisation_expression);

                return Context::error_node();
            }
            else if (Context::scope_element_t::element_type::e_string == se.type)
            {
                str_node = se.str_node;
                se.active = true;
                se.depth = ctx.state.scope_depth;
                se.ref_count++;
            }
        }

        if (nullptr == str_node)
        {
            typename Context::scope_element_t nse;
            nse.name = str_name;
            nse.active = true;
            nse.ref_count = 1;
            nse.type = Context::scope_element_t::element_type::e_string;
            nse.depth = ctx.state.scope_depth;
            nse.str_data = std::make_unique<std::string>();
            nse.str_node = ctx.make_stringvar_node(*nse.str_data);

            if (!ctx.add_element(std::move(nse)))
            {
                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, ctx.current_token(),
                    "ERR176 - Failed to add new local string variable '" + str_name + "' to SEM",
                    core::error_location()));

                ctx.free_node(initialisation_expression);
                ctx.free_element(nse);

                return Context::error_node();
            }

            assert(ctx.total_local_symb_size_bytes() <= ctx.max_total_local_symbol_size_bytes());

            str_node = nse.str_node;

            core::debug_print(
                "parse_define_string_statement() - INFO - Added new local string variable: %s\n",
                nse.name.c_str());
        }

        ctx.lodge_symbol(str_name, symbol_type::e_st_local_string);
        ctx.activate_side_effect("parse_define_string_statement()");

        return ctx.make_assignment(str_node, initialisation_expression);
    }
#else
    template <typename Context>
    static inline expression_node_ptr parse_define_string_statement(Context&, const std::string&,
                                                                    expression_node_ptr)
    {
        return Context::error_node();
    }
#endif

    template <typename Context>
    static inline expression_node_ptr parse_define_var_statement(Context& ctx)
    {
        if (ctx.settings.vardef_disabled())
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR177 - Illegal variable definition", core::error_location()));

            return Context::error_node();
        }
        else if (!core::imatch(ctx.current_token().value, "var"))
        {
            return Context::error_node();
        }
        else
        {
            ctx.next_token();
        }

        const std::string var_name = ctx.current_token().value;

        expression_node_ptr initialisation_expression = Context::error_node();

        if (!ctx.token_is(token_t::e_symbol))
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR178 - Expected a symbol for variable definition", core::error_location()));

            return Context::error_node();
        }
        else if (core::is_reserved_symbol(var_name))
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR179 - Illegal redefinition of reserved keyword: '" + var_name + "'",
                core::error_location()));

            return Context::error_node();
        }
        else if (ctx.symtab_store.symbol_exists(var_name))
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR180 - Illegal redefinition of variable '" + var_name + "'",
                core::error_location()));

            return Context::error_node();
        }
        else if (local_variable_is_shadowed(ctx, var_name))
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR181 - Illegal redefinition of local variable: '" + var_name + "'",
                core::error_location()));

            return Context::error_node();
        }
        else if (ctx.token_is(token_t::e_lsqrbracket, Context::token_advance_mode::e_hold))
        {
            return ctx.parse_define_vector_statement(var_name);
        }
        else if (ctx.token_is(token_t::e_lcrlbracket, Context::token_advance_mode::e_hold))
        {
            return parse_uninitialised_var_statement(ctx, var_name);
        }
        else if (ctx.token_is(token_t::e_assign))
        {
            if (nullptr == (initialisation_expression = ctx.parse_expression()))
            {
                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, ctx.current_token(),
                    "ERR182 - Failed to parse initialisation expression for variable '" + var_name +
                        "'",
                    core::error_location()));

                return Context::error_node();
            }
        }

        if (!ctx.token_is(token_t::e_rbracket, Context::token_advance_mode::e_hold) &&
            !ctx.token_is(token_t::e_rcrlbracket, Context::token_advance_mode::e_hold) &&
            !ctx.token_is(token_t::e_rsqrbracket, Context::token_advance_mode::e_hold))
        {
            if (!ctx.token_is(token_t::e_eof, Context::token_advance_mode::e_hold))
            {
                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, ctx.current_token(),
                    "ERR183 - Expected ';' after variable '" + var_name + "' definition",
                    core::error_location()));

                ctx.free_node(initialisation_expression);

                return Context::error_node();
            }
        }

        if ((nullptr != initialisation_expression) &&
            details::is_generally_string_node(initialisation_expression))
        {
            return parse_define_string_statement(ctx, var_name, initialisation_expression);
        }

        expression_node_ptr var_node = nullptr;

        typename Context::scope_element_t& se = ctx.get_element(var_name);

        if (se.name == var_name)
        {
            if (se.active)
            {
                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, ctx.current_token(),
                    "ERR184 - Illegal redefinition of local variable: '" + var_name + "'",
                    core::error_location()));

                ctx.free_node(initialisation_expression);

                return Context::error_node();
            }
            else if (Context::scope_element_t::element_type::e_variable == se.type)
            {
                var_node = se.var_node;
                se.active = true;
                se.depth = ctx.state.scope_depth;
                se.ref_count++;
            }
        }

        if (nullptr == var_node)
        {
            const std::size_t predicted_total_lclsymb_size =
                sizeof(T) + ctx.total_local_symb_size_bytes();

            if (predicted_total_lclsymb_size > ctx.max_total_local_symbol_size_bytes())
            {
                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, ctx.current_token(),
                    "ERR185 - Adding variable '" + var_name +
                        "' will exceed max total local symbol size of: " +
                        core::to_str(ctx.max_total_local_symbol_size_bytes()) +
                        " bytes, current total size: " +
                        core::to_str(ctx.total_local_symb_size_bytes()) + " bytes",
                    core::error_location()));

                ctx.free_node(initialisation_expression);

                return Context::error_node();
            }

            typename Context::scope_element_t nse;
            nse.name = var_name;
            nse.active = true;
            nse.ref_count = 1;
            nse.type = Context::scope_element_t::element_type::e_variable;
            nse.depth = ctx.state.scope_depth;
            nse.scalar_data = std::make_unique<T>(T(0));
            nse.var_node = ctx.make_variable_node(*nse.scalar_data);

            if (!ctx.add_element(std::move(nse)))
            {
                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, ctx.current_token(),
                    "ERR186 - Failed to add new local variable '" + var_name + "' to SEM",
                    core::error_location()));

                ctx.free_node(initialisation_expression);
                ctx.free_element(nse);

                return Context::error_node();
            }

            assert(ctx.total_local_symb_size_bytes() <= ctx.max_total_local_symbol_size_bytes());

            var_node = nse.var_node;

            core::debug_print(
                "parse_define_var_statement() - INFO - Added new local variable: %s\n",
                nse.name.c_str());
        }

        ctx.activate_side_effect("parse_define_var_statement()");
        ctx.lodge_symbol(var_name, symbol_type::e_st_local_variable);

        return ctx.make_assignment(var_node, initialisation_expression
                                                 ? initialisation_expression
                                                 : ctx.make_numeric_literal(T(0)));
    }

    template <typename Context>
    static inline expression_node_ptr parse_define_constvar_statement(Context& ctx)
    {
        if (ctx.settings.vardef_disabled())
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR187 - Illegal const variable definition", core::error_location()));

            return Context::error_node();
        }
        else if (!ctx.token_is("const"))
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR188 - Expected 'const' keyword for const-variable definition",
                core::error_location()));

            return Context::error_node();
        }
        else if (!ctx.token_is("var"))
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR189 - Expected 'var' keyword for const-variable definition",
                core::error_location()));

            return Context::error_node();
        }

        const std::string var_name = ctx.current_token().value;

        expression_node_ptr initialisation_expression = Context::error_node();

        if (!ctx.token_is(token_t::e_symbol))
        {
            ctx.set_error(
                parser_error::make_error(parser_error::error_mode::e_syntax, ctx.current_token(),
                                         "ERR190 - Expected a symbol for const-variable definition",
                                         core::error_location()));

            return Context::error_node();
        }
        else if (core::is_reserved_symbol(var_name))
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR191 - Illegal redefinition of reserved keyword: '" + var_name + "'",
                core::error_location()));

            return Context::error_node();
        }
        else if (ctx.symtab_store.symbol_exists(var_name))
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR192 - Illegal redefinition of variable '" + var_name + "'",
                core::error_location()));

            return Context::error_node();
        }
        else if (local_variable_is_shadowed(ctx, var_name))
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR193 - Illegal redefinition of local variable: '" + var_name + "'",
                core::error_location()));

            return Context::error_node();
        }
        else if (!ctx.token_is(token_t::e_assign))
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR194 - Expected assignment operator after const-variable: '" + var_name +
                    "' definition",
                core::error_location()));

            return Context::error_node();
        }
        else if (nullptr == (initialisation_expression = ctx.parse_expression()))
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR195 - Failed to parse initialisation expression for const-variable: '" +
                    var_name + "'",
                core::error_location()));

            return Context::error_node();
        }

        if (!details::is_literal_node(initialisation_expression))
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR196 - initialisation expression for const-variable: '" + var_name +
                    "' must be a constant/literal",
                core::error_location()));

            ctx.free_node(initialisation_expression);

            return Context::error_node();
        }

        assert(initialisation_expression);

        const T init_value = initialisation_expression->value();
        ctx.free_node(initialisation_expression);

        expression_node_ptr var_node = nullptr;

        typename Context::scope_element_t& se = ctx.get_element(var_name);

        if (se.name == var_name)
        {
            if (se.active)
            {
                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, ctx.current_token(),
                    "ERR197 - Illegal redefinition of local variable: '" + var_name + "'",
                    core::error_location()));

                return Context::error_node();
            }
            else if (Context::scope_element_t::element_type::e_literal == se.type)
            {
                var_node = se.var_node;
                se.active = true;
                se.depth = ctx.state.scope_depth;
                se.ref_count++;
            }
        }

        if (nullptr == var_node)
        {
            const std::size_t predicted_total_lclsymb_size =
                sizeof(T) + ctx.total_local_symb_size_bytes();

            if (predicted_total_lclsymb_size > ctx.max_total_local_symbol_size_bytes())
            {
                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, ctx.current_token(),
                    "ERR198 - Adding variable '" + var_name +
                        "' will exceed max total local symbol size of: " +
                        core::to_str(ctx.max_total_local_symbol_size_bytes()) +
                        " bytes, current total size: " +
                        core::to_str(ctx.total_local_symb_size_bytes()) + " bytes",
                    core::error_location()));

                return Context::error_node();
            }

            typename Context::scope_element_t nse;
            nse.name = var_name;
            nse.active = true;
            nse.ref_count = 1;
            nse.type = Context::scope_element_t::element_type::e_literal;
            nse.depth = ctx.state.scope_depth;
            nse.var_node = ctx.make_literal_node(init_value);

            if (!ctx.add_element(std::move(nse)))
            {
                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, ctx.current_token(),
                    "ERR199 - Failed to add new local const-variable '" + var_name + "' to SEM",
                    core::error_location()));

                ctx.free_element(nse);

                return Context::error_node();
            }

            assert(ctx.total_local_symb_size_bytes() <= ctx.max_total_local_symbol_size_bytes());

            var_node = nse.var_node;

            core::debug_print(
                "parse_define_constvar_statement() - INFO - Added new local const-variable: %s\n",
                nse.name.c_str());
        }

        ctx.activate_side_effect("parse_define_constvar_statement()");
        ctx.lodge_symbol(var_name, symbol_type::e_st_local_variable);

        return ctx.make_numeric_literal(var_node->value());
    }

    template <typename Context>
    static inline expression_node_ptr parse_uninitialised_var_statement(Context& ctx,
                                                                        const std::string& var_name)
    {
        if (!ctx.token_is(token_t::e_lcrlbracket) || !ctx.token_is(token_t::e_rcrlbracket))
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR200 - Expected a '{}' for uninitialised var definition",
                core::error_location()));

            return Context::error_node();
        }
        else if (!ctx.token_is(token_t::e_eof, Context::token_advance_mode::e_hold))
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR201 - Expected ';' after uninitialised variable definition",
                core::error_location()));

            return Context::error_node();
        }

        expression_node_ptr var_node = nullptr;

        typename Context::scope_element_t& se = ctx.get_element(var_name);

        if (se.name == var_name)
        {
            if (se.active)
            {
                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, ctx.current_token(),
                    "ERR202 - Illegal redefinition of local variable: '" + var_name + "'",
                    core::error_location()));

                return Context::error_node();
            }
            else if (Context::scope_element_t::element_type::e_variable == se.type)
            {
                var_node = se.var_node;
                se.active = true;
                se.ref_count++;
            }
        }

        if (nullptr == var_node)
        {
            const std::size_t predicted_total_lclsymb_size =
                sizeof(T) + ctx.total_local_symb_size_bytes();

            if (predicted_total_lclsymb_size > ctx.max_total_local_symbol_size_bytes())
            {
                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, ctx.current_token(),
                    "ERR203 - Adding variable '" + var_name +
                        "' will exceed max total local symbol size of: " +
                        core::to_str(ctx.max_total_local_symbol_size_bytes()) +
                        " bytes, current total size: " +
                        core::to_str(ctx.total_local_symb_size_bytes()) + " bytes",
                    core::error_location()));

                return Context::error_node();
            }

            typename Context::scope_element_t nse;
            nse.name = var_name;
            nse.active = true;
            nse.ref_count = 1;
            nse.type = Context::scope_element_t::element_type::e_variable;
            nse.depth = ctx.state.scope_depth;
            nse.ip_index = ctx.next_ip_index();
            nse.scalar_data = std::make_unique<T>(T(0));
            nse.var_node = ctx.make_variable_node(*nse.scalar_data);

            if (!ctx.add_element(std::move(nse)))
            {
                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, ctx.current_token(),
                    "ERR204 - Failed to add new local variable '" + var_name + "' to SEM",
                    core::error_location()));

                ctx.free_element(nse);

                return Context::error_node();
            }

            assert(ctx.total_local_symb_size_bytes() <= ctx.max_total_local_symbol_size_bytes());

            core::debug_print(
                "parse_uninitialised_var_statement() - INFO - Added new local variable: %s\n",
                nse.name.c_str());
        }

        ctx.lodge_symbol(var_name, symbol_type::e_st_local_variable);
        ctx.activate_side_effect("parse_uninitialised_var_statement()");

        return ctx.make_numeric_literal(T(0));
    }
};
}  // namespace math_expr

#endif
