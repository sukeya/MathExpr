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
