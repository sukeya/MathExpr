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

#ifndef MATH_EXPR_PARSER_SYMBOL_PARSER_HPP
#define MATH_EXPR_PARSER_SYMBOL_PARSER_HPP

namespace math_expr
{
template <typename T>
class parser_symbol
{
   public:
    using expression_node_ptr = details::expression_node<T>*;

    template <typename Context>
    static inline expression_node_ptr parse_symbol(Context& ctx)
    {
        static constexpr std::string_view symbol_if = "if";
        static constexpr std::string_view symbol_while = "while";
        static constexpr std::string_view symbol_repeat = "repeat";
        static constexpr std::string_view symbol_for = "for";
        static constexpr std::string_view symbol_switch = "switch";
        static constexpr std::string_view symbol_null = "null";
        static constexpr std::string_view symbol_break = "break";
        static constexpr std::string_view symbol_continue = "continue";
        static constexpr std::string_view symbol_var = "var";
        static constexpr std::string_view symbol_const = "const";
        static constexpr std::string_view symbol_swap = "swap";
        static constexpr std::string_view symbol_return = "return";
        static constexpr std::string_view symbol_not = "not";
        static constexpr std::string_view symbol_assert = "assert";
        static constexpr std::string_view symbol_true = "true";
        static constexpr std::string_view symbol_false = "false";

        const std::string symbol = ctx.current_token().value;

        if (ctx.valid_vararg_operation(symbol))
        {
            return ctx.parse_vararg_function();
        }
        else if (core::imatch(symbol, symbol_not))
        {
            return ctx.parse_not_statement();
        }
        else if (core::imatch(symbol, symbol_true))
        {
            ctx.next_token();
            return ctx.make_numeric_literal(core::numeric::true_v<T>);
        }
        else if (core::imatch(symbol, symbol_false))
        {
            ctx.next_token();
            return ctx.make_numeric_literal(core::numeric::false_v<T>);
        }
        else if (ctx.valid_base_operation(symbol))
        {
            return ctx.parse_base_operation();
        }
        else if (core::imatch(symbol, symbol_if) && ctx.settings.control_struct_enabled(symbol))
        {
            return ctx.parse_conditional_statement();
        }
        else if (core::imatch(symbol, symbol_while) && ctx.settings.control_struct_enabled(symbol))
        {
            return ctx.check_block_statement_closure(ctx.parse_while_loop());
        }
        else if (core::imatch(symbol, symbol_repeat) && ctx.settings.control_struct_enabled(symbol))
        {
            return ctx.check_block_statement_closure(ctx.parse_repeat_until_loop());
        }
        else if (core::imatch(symbol, symbol_for) && ctx.settings.control_struct_enabled(symbol))
        {
            return ctx.check_block_statement_closure(ctx.parse_for_loop());
        }
        else if (core::imatch(symbol, symbol_switch) && ctx.settings.control_struct_enabled(symbol))
        {
            return ctx.check_block_statement_closure(ctx.parse_switch_statement());
        }
        else if (core::is_valid_sf_symbol(symbol))
        {
            return ctx.parse_special_function();
        }
        else if (core::imatch(symbol, symbol_null))
        {
            return ctx.parse_null_statement();
        }
#ifndef MATH_EXPR_DISABLE_BREAK_CONTINUE
        else if (core::imatch(symbol, symbol_break))
        {
            return ctx.parse_break_statement();
        }
        else if (core::imatch(symbol, symbol_continue))
        {
            return ctx.parse_continue_statement();
        }
#endif
        else if (core::imatch(symbol, symbol_var))
        {
            return ctx.parse_define_var_statement();
        }
        else if (core::imatch(symbol, symbol_const))
        {
            return ctx.parse_define_constvar_statement();
        }
        else if (core::imatch(symbol, symbol_swap))
        {
            return ctx.parse_swap_statement();
        }
#ifndef MATH_EXPR_DISABLE_RETURN_STATEMENT
        else if (core::imatch(symbol, symbol_return) && ctx.settings.control_struct_enabled(symbol))
        {
            return ctx.check_block_statement_closure(ctx.parse_return_statement());
        }
#endif
        else if (core::imatch(symbol, symbol_assert))
        {
            return ctx.parse_assert_statement();
        }
        else if (ctx.symtab_valid() || !ctx.scope_empty())
        {
            return ctx.parse_symtab_symbol();
        }

        ctx.set_error(parser_error::make_error(
            parser_error::error_mode::e_symtab, ctx.current_token(),
            "ERR241 - Unknown variable or function encountered. Symbol table(s) "
            "is either invalid or does not contain symbol: '" +
                symbol + "'",
            core::error_location()));

        return Context::error_node();
    }
};
}  // namespace math_expr

#endif
