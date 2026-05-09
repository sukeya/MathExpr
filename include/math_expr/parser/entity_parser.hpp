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

#ifndef MATH_EXPR_PARSER_ENTITY_PARSER_HPP
#define MATH_EXPR_PARSER_ENTITY_PARSER_HPP

namespace math_expr
{
template <typename T>
class parser_entity
{
   public:
    using expression_node_ptr = details::expression_node<T>*;
    using token_t = lexer::token;

#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
    template <typename Context>
    static inline expression_node_ptr parse_string(Context& ctx)
    {
        const std::string symbol = ctx.current_token().value;

        expression_node_ptr result = Context::error_node();
        typename Context::stringvar_node_ptr const_str_node = nullptr;

        typename Context::scope_element_t& se = ctx.get_active_element(symbol);

        if (Context::scope_element_t::element_type::e_string == se.type)
        {
            se.active = true;
            result = se.str_node;
            ctx.lodge_symbol(symbol, symbol_type::e_st_local_string);
        }
        else
        {
            typename Context::string_context_t str_ctx = ctx.get_string_context(symbol);

            if ((nullptr == str_ctx.str_var) || !ctx.is_conststr_stringvar(symbol))
            {
                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, ctx.current_token(),
                    "ERR121 - Unknown string symbol", core::error_location()));

                return Context::error_node();
            }

            assert(str_ctx.str_var != nullptr);
            assert(str_ctx.symbol_table != nullptr);

            result = str_ctx.str_var;

            if (ctx.is_constant_string(symbol))
            {
                const_str_node = static_cast<typename Context::stringvar_node_ptr>(result);
                result = ctx.make_string_literal(const_str_node->str());
            }
            else if (Context::symbol_table_t::symtab_mutability_type::e_immutable ==
                     str_ctx.symbol_table->mutability())
            {
                ctx.lodge_immutable_string_symbol(ctx.current_token(), str_ctx.str_var->base(),
                                                  str_ctx.str_var->size());
            }

            ctx.lodge_symbol(symbol, symbol_type::e_st_string);
        }

        if (ctx.peek_token_is(token_t::e_lsqrbracket))
        {
            ctx.next_token();

            if (ctx.peek_token_is(token_t::e_rsqrbracket))
            {
                ctx.next_token();
                ctx.next_token();

                if (const_str_node)
                {
                    ctx.free_node(result);
                    return ctx.make_numeric_literal(T(const_str_node->size()));
                }

                return ctx.make_stringvar_size_node(
                    static_cast<typename Context::stringvar_node_ptr>(result));
            }

            typename Context::range_t range;

            if (!ctx.parse_range(range))
            {
                ctx.free_node(result);
                return Context::error_node();
            }

            if (const_str_node)
            {
                ctx.free_node(result);
            }

            result = ctx.make_string_range(
                (const_str_node ? const_str_node
                                : static_cast<typename Context::stringvar_node_ptr>(result)),
                range);

            if (result)
                range.clear();
        }
        else
        {
            ctx.next_token();
        }

        return result;
    }

    template <typename Context>
    static inline expression_node_ptr parse_const_string(Context& ctx)
    {
        const std::string const_str = ctx.current_token().value;
        expression_node_ptr result = ctx.make_string_literal(const_str);

        if (ctx.peek_token_is(token_t::e_lsqrbracket))
        {
            ctx.next_token();

            if (ctx.peek_token_is(token_t::e_rsqrbracket))
            {
                ctx.next_token();
                ctx.next_token();

                ctx.free_node(result);
                return ctx.make_numeric_literal(T(const_str.size()));
            }

            typename Context::range_t range;

            if (!ctx.parse_range(range))
            {
                ctx.free_node(result);
                range.free();
                return Context::error_node();
            }

            ctx.free_node(result);

            if (range.n1_c.first && (range.n1_c.second == std::numeric_limits<std::size_t>::max()))
            {
                range.n1_c.second = const_str.size() - 1;
                range.cache.second = range.n1_c.second;
            }

            if ((range.n0_c.first && (range.n0_c.second >= const_str.size())) ||
                (range.n1_c.first && (range.n1_c.second >= const_str.size())))
            {
                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, ctx.current_token(),
                    "ERR122 - Overflow in range for string: '" + const_str + "'[" +
                        (range.n0_c.first ? core::to_str(static_cast<int>(range.n0_c.second))
                                          : "?") +
                        ":" +
                        (range.n1_c.first ? core::to_str(static_cast<int>(range.n1_c.second))
                                          : "?") +
                        "]",
                    core::error_location()));

                range.free();
                return Context::error_node();
            }

            result = ctx.make_const_string_range(const_str, range);

            if (result)
                range.clear();
        }
        else
        {
            ctx.next_token();
        }

        return result;
    }
#endif

    template <typename Context>
    static inline expression_node_ptr parse_vector(Context& ctx)
    {
        const std::string vector_name = ctx.current_token().value;
        typename Context::vector_holder_ptr vector_holder = nullptr;

        const typename Context::scope_element_t& se = ctx.get_active_element(vector_name);

        if (!core::imatch(se.name, vector_name) || (se.depth > ctx.state.scope_depth) ||
            (Context::scope_element_t::element_type::e_vector != se.type))
        {
            typename Context::vector_context_t vec_ctx = ctx.get_vector_context(vector_name);

            if (nullptr == vec_ctx.vector_holder)
            {
                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, ctx.current_token(),
                    "ERR125 - Symbol '" + vector_name + " not a vector", core::error_location()));

                return Context::error_node();
            }

            assert(nullptr != vec_ctx.vector_holder);
            assert(nullptr != vec_ctx.symbol_table);

            vector_holder = vec_ctx.vector_holder;

            if (Context::symbol_table_t::symtab_mutability_type::e_immutable ==
                vec_ctx.symbol_table->mutability())
            {
                ctx.lodge_immutable_vector_symbol(ctx.current_token(), vector_holder->data(),
                                                  vector_holder->size());
            }
        }
        else
        {
            vector_holder = se.vec_node;
        }

        assert(nullptr != vector_holder);

        ctx.next_token();

        if (!ctx.token_is(token_t::e_lsqrbracket))
        {
            return ctx.make_vector_node(vector_holder);
        }
        else if (ctx.token_is(token_t::e_rsqrbracket))
        {
            return vector_holder->rebaseable() ? ctx.make_vector_size_node(vector_holder)
                                               : ctx.make_numeric_literal(T(vector_holder->size()));
        }

        expression_node_ptr index_expr = ctx.parse_vector_index(vector_name);

        if (index_expr)
        {
            expression_node_ptr vector_node = ctx.make_vector_node(vector_holder);
            return ctx.synthesize_vector_element(vector_name, vector_holder, vector_node,
                                                 index_expr);
        }

        return Context::error_node();
    }
};
}  // namespace math_expr

#endif
