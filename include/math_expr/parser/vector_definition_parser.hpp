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

#ifndef MATH_EXPR_PARSER_VECTOR_DEFINITION_PARSER_HPP
#define MATH_EXPR_PARSER_VECTOR_DEFINITION_PARSER_HPP

namespace math_expr
{
template <typename T>
class parser_vector_definition
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
    static inline expression_node_ptr parse_define_vector_statement(Context& ctx,
                                                                    const std::string& vec_name)
    {
        expression_node_ptr size_expression_node = Context::error_node();

        if (!ctx.token_is(token_t::e_lsqrbracket))
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR157 - Expected '[' as part of vector size definition", core::error_location()));

            return Context::error_node();
        }
        else if (nullptr == (size_expression_node = ctx.parse_expression()))
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR158 - Failed to determine size of vector '" + vec_name + "'",
                core::error_location()));

            return Context::error_node();
        }
        else if (!ctx.is_constant_node(size_expression_node))
        {
            const bool is_rebaseable_vector =
                (size_expression_node->type() ==
                 details::expression_node<T>::node_type::e_vecsize) &&
                static_cast<details::vector_size_node<T>*>(size_expression_node)
                    ->vec_holder()
                    ->rebaseable();

            ctx.free_node(size_expression_node);

            const std::string error_msg =
                is_rebaseable_vector
                    ? std::string(
                          "Rebasable/Resizable vector cannot be used to define the size of vector")
                    : std::string("Expected a constant literal number as size of vector");
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR159 - " + error_msg + " '" + vec_name + "'", core::error_location()));

            return Context::error_node();
        }

        const T vector_size = size_expression_node->value();
        ctx.free_node(size_expression_node);

        const std::size_t max_vector_size = ctx.max_local_vector_size();

        if ((vector_size <= T(0)) ||
            std::not_equal_to<T>()(T(0), vector_size - core::numeric::trunc(vector_size)) ||
            (static_cast<std::size_t>(vector_size) > max_vector_size))
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR160 - Invalid vector size. Must be an integer in the range [0," +
                    core::to_str(static_cast<std::size_t>(max_vector_size)) +
                    "], size: " + core::to_str(core::numeric::to_int32(vector_size)),
                core::error_location()));

            return Context::error_node();
        }

        typename Context::vector_holder_ptr vec_holder = nullptr;

        const std::size_t vec_size = static_cast<std::size_t>(core::numeric::to_int32(vector_size));
        const std::size_t predicted_total_lclsymb_size =
            sizeof(T) * vec_size + ctx.total_local_symb_size_bytes();

        if (predicted_total_lclsymb_size > ctx.max_total_local_symbol_size_bytes())
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR161 - Adding vector '" + vec_name + "' of size " + core::to_str(vec_size) +
                    " bytes will exceed max total local symbol size of: " +
                    core::to_str(ctx.max_total_local_symbol_size_bytes()) +
                    " bytes, current total size: " +
                    core::to_str(ctx.total_local_symb_size_bytes()) + " bytes",
                core::error_location()));

            return Context::error_node();
        }

        typename Context::scope_element_t& se = ctx.get_element(vec_name);

        if (se.name == vec_name)
        {
            if (se.active)
            {
                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, ctx.current_token(),
                    "ERR162 - Illegal redefinition of local vector: '" + vec_name + "'",
                    core::error_location()));

                return Context::error_node();
            }
            else if ((se.size == vec_size) &&
                     (Context::scope_element_t::element_type::e_vector == se.type))
            {
                vec_holder = se.vec_node;
                se.active = true;
                se.depth = ctx.state.scope_depth;
                se.ref_count++;
            }
        }

        if (nullptr == vec_holder)
        {
            typename Context::scope_element_t nse;
            nse.name = vec_name;
            nse.active = true;
            nse.ref_count = 1;
            nse.type = Context::scope_element_t::element_type::e_vector;
            nse.depth = ctx.state.scope_depth;
            nse.size = vec_size;
            nse.vector_data = std::make_unique<T[]>(vec_size);
            nse.vec_node = ctx.make_vector_holder(nse.vector_data.get(), nse.size);

            core::numeric::set_zero_value(nse.vector_data.get(), vec_size);

            if (!ctx.add_element(std::move(nse)))
            {
                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, ctx.current_token(),
                    "ERR163 - Failed to add new local vector '" + vec_name + "' to SEM",
                    core::error_location()));

                ctx.free_element(nse);

                return Context::error_node();
            }

            assert(ctx.total_local_symb_size_bytes() <= ctx.max_total_local_symbol_size_bytes());

            vec_holder = nse.vec_node;

            core::debug_print(
                "parse_define_vector_statement() - INFO - Added new local vector: %s[%d]\n",
                nse.name.c_str(), static_cast<int>(nse.size));
        }

        ctx.activate_side_effect("parse_define_vector_statement()");
        ctx.lodge_symbol(vec_name, symbol_type::e_st_local_vector);

        std::vector<expression_node_ptr> vec_initialiser_list;
        scoped_node_vector_delete<Context> scoped_initialisers(ctx, vec_initialiser_list);

        bool single_value_initialiser = false;
        bool range_value_initialiser = false;
        bool vec_to_vec_initialiser = false;
        bool null_initialisation = false;

        if (!ctx.token_is(token_t::e_rsqrbracket))
        {
            ctx.set_error(parser_error::make_error(
                parser_error::error_mode::e_syntax, ctx.current_token(),
                "ERR164 - Expected ']' as part of vector size definition", core::error_location()));

            return Context::error_node();
        }
        else if (!ctx.token_is(token_t::e_eof, Context::token_advance_mode::e_hold))
        {
            if (!ctx.token_is(token_t::e_assign))
            {
                ctx.set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, ctx.current_token(),
                    "ERR165 - Expected ':=' as part of vector definition", core::error_location()));

                return Context::error_node();
            }
            else if (ctx.token_is(token_t::e_lsqrbracket))
            {
                expression_node_ptr initialiser_component = ctx.parse_expression();

                if (nullptr == initialiser_component)
                {
                    ctx.set_error(parser_error::make_error(
                        parser_error::error_mode::e_syntax, ctx.current_token(),
                        "ERR166 - Failed to parse first component of vector initialiser for "
                        "vector: " +
                            vec_name,
                        core::error_location()));

                    return Context::error_node();
                }

                vec_initialiser_list.push_back(initialiser_component);

                if (ctx.token_is(token_t::e_colon))
                {
                    initialiser_component = ctx.parse_expression();

                    if (nullptr == initialiser_component)
                    {
                        ctx.set_error(parser_error::make_error(
                            parser_error::error_mode::e_syntax, ctx.current_token(),
                            "ERR167 - Failed to parse second component of vector initialiser for "
                            "vector: " +
                                vec_name,
                            core::error_location()));

                        return Context::error_node();
                    }

                    vec_initialiser_list.push_back(initialiser_component);
                }

                if (!ctx.token_is(token_t::e_rsqrbracket))
                {
                    ctx.set_error(parser_error::make_error(
                        parser_error::error_mode::e_syntax, ctx.current_token(),
                        "ERR168 - Expected ']' to close single value vector initialiser",
                        core::error_location()));

                    return Context::error_node();
                }

                switch (vec_initialiser_list.size())
                {
                    case 1:
                        single_value_initialiser = true;
                        break;
                    case 2:
                        range_value_initialiser = true;
                        break;
                    default:
                        break;
                }
            }
            else if (!ctx.token_is(token_t::e_lcrlbracket))
            {
                expression_node_ptr initialiser = Context::error_node();

                if (token_t::e_symbol == ctx.current_token().type)
                {
                    const typename Context::scope_element_t& local_se =
                        ctx.get_active_element(ctx.current_token().value);

                    if (Context::scope_element_t::element_type::e_vector == local_se.type)
                    {
                        if (nullptr != (initialiser = ctx.parse_expression()))
                        {
                            vec_initialiser_list.push_back(initialiser);
                        }
                        else
                        {
                            return Context::error_node();
                        }
                    }
                    else if (ctx.symtab_store.is_vector(ctx.current_token().value))
                    {
                        ctx.lodge_symbol(ctx.current_token().value, symbol_type::e_st_vector);

                        if (nullptr != (initialiser = ctx.parse_expression()))
                        {
                            vec_initialiser_list.push_back(initialiser);
                        }
                        else
                        {
                            return Context::error_node();
                        }
                    }
                    else if (ctx.token_is(token_t::e_symbol, "null"))
                    {
                        null_initialisation = true;
                    }
                }

                if (!null_initialisation)
                {
                    if (nullptr == initialiser)
                    {
                        ctx.set_error(parser_error::make_error(
                            parser_error::error_mode::e_syntax, ctx.current_token(),
                            "ERR169 - Expected '{' as part of vector initialiser list",
                            core::error_location()));

                        return Context::error_node();
                    }
                    else
                    {
                        vec_to_vec_initialiser = true;
                    }
                }
            }
            else if (!ctx.token_is(token_t::e_rcrlbracket))
            {
                for (;;)
                {
                    expression_node_ptr initialiser = ctx.parse_expression();

                    if (nullptr == initialiser)
                    {
                        ctx.set_error(parser_error::make_error(
                            parser_error::error_mode::e_syntax, ctx.current_token(),
                            "ERR170 - Expected '{' as part of vector initialiser list",
                            core::error_location()));

                        return Context::error_node();
                    }

                    vec_initialiser_list.push_back(initialiser);

                    if (ctx.token_is(token_t::e_rcrlbracket))
                        break;

                    const bool is_next_close = ctx.peek_token_is(token_t::e_rcrlbracket);

                    if (!ctx.token_is(token_t::e_comma) && is_next_close)
                    {
                        ctx.set_error(parser_error::make_error(
                            parser_error::error_mode::e_syntax, ctx.current_token(),
                            "ERR171 - Expected ',' between vector initialisers",
                            core::error_location()));

                        return Context::error_node();
                    }

                    if (ctx.token_is(token_t::e_rcrlbracket))
                        break;
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
                        "ERR172 - Expected ';' at end of vector definition",
                        core::error_location()));

                    return Context::error_node();
                }
            }

            if (!single_value_initialiser && !range_value_initialiser &&
                (T(vec_initialiser_list.size()) > vector_size))
            {
                ctx.set_error(parser_error::make_error(parser_error::error_mode::e_syntax,
                                                       ctx.current_token(),
                                                       "ERR173 - Initialiser list larger than the "
                                                       "number of elements in the vector: '" +
                                                           vec_name + "'",
                                                       core::error_location()));

                return Context::error_node();
            }
        }

        expression_node_ptr result = Context::error_node();

        if ((vec_initialiser_list.size() == 1) && single_value_initialiser)
        {
            if (details::is_constant_node(vec_initialiser_list[0]))
            {
                if (T(0) == vec_initialiser_list[0]->value())
                {
                    result = ctx.node_allocator
                                 .template allocate<details::vector_init_zero_value_node<T>>(
                                     (*vec_holder)[0], vec_size, vec_initialiser_list);
                }
                else
                {
                    result = ctx.node_allocator
                                 .template allocate<details::vector_init_single_constvalue_node<T>>(
                                     (*vec_holder)[0], vec_size, vec_initialiser_list);
                }
            }
            else
            {
                result =
                    ctx.node_allocator.template allocate<details::vector_init_single_value_node<T>>(
                        (*vec_holder)[0], vec_size, vec_initialiser_list);
            }
        }
        else if ((vec_initialiser_list.size() == 2) && range_value_initialiser)
        {
            const bool base_const = details::is_constant_node(vec_initialiser_list[0]);
            const bool inc_const = details::is_constant_node(vec_initialiser_list[1]);

            if (base_const && inc_const)
            {
                result = ctx.node_allocator
                             .template allocate<details::vector_init_iota_constconst_node<T>>(
                                 (*vec_holder)[0], vec_size, vec_initialiser_list);
            }
            else if (base_const && !inc_const)
            {
                result = ctx.node_allocator
                             .template allocate<details::vector_init_iota_constnconst_node<T>>(
                                 (*vec_holder)[0], vec_size, vec_initialiser_list);
            }
            else if (!base_const && inc_const)
            {
                result = ctx.node_allocator
                             .template allocate<details::vector_init_iota_nconstconst_node<T>>(
                                 (*vec_holder)[0], vec_size, vec_initialiser_list);
            }
            else
            {
                result = ctx.node_allocator
                             .template allocate<details::vector_init_iota_nconstnconst_node<T>>(
                                 (*vec_holder)[0], vec_size, vec_initialiser_list);
            }
        }
        else if (null_initialisation)
        {
            result = ctx.make_numeric_literal(T(0));
        }
        else if (vec_to_vec_initialiser)
        {
            expression_node_ptr vec_node =
                ctx.node_allocator.template allocate<typename Context::vector_node_t>(vec_holder);
            result = ctx.make_assign_vector(vec_node, vec_initialiser_list[0]);
        }
        else
        {
            result = ctx.node_allocator.template allocate<details::vector_initialisation_node<T>>(
                (*vec_holder)[0], vec_size, vec_initialiser_list, single_value_initialiser);
        }

        scoped_initialisers.delete_ptr = false;

        if (result && result->valid())
        {
            return result;
        }

        ctx.free_node(result);
        ctx.set_error(parser_error::make_error(
            parser_error::error_mode::e_synthesis, ctx.current_token(),
            "ERR174 - Failed to generate initialisation node for vector: " + vec_name,
            core::error_location()));

        return Context::error_node();
    }
};
}  // namespace math_expr

#endif
