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

#ifndef MATH_EXPR_PARSER_EXPRESSION_TABLE_HPP
#define MATH_EXPR_PARSER_EXPRESSION_TABLE_HPP

#include "math_expr/core/operator_types.hpp"
#include "math_expr/core/string_utils.hpp"
#include "math_expr/lexer/token.hpp"

namespace math_expr
{
template <typename Token>
class expression_table
{
   public:
    enum class precedence_level
    {
        e_level00,
        e_level01,
        e_level02,
        e_level03,
        e_level04,
        e_level05,
        e_level06,
        e_level07,
        e_level08,
        e_level09,
        e_level10,
        e_level11,
        e_level12,
        e_level13,
        e_level14
    };

    struct state_t
    {
        inline void set(const precedence_level& l, const precedence_level& r,
                        const core::operators::operator_type& o, const Token& tkn = Token())
        {
            left = l;
            right = r;
            operation = o;
            token = tkn;
        }

        inline void reset()
        {
            left = precedence_level::e_level00;
            right = precedence_level::e_level00;
            operation = core::operators::operator_type::default_op;
            token = Token();
        }

        precedence_level left = precedence_level::e_level00;
        precedence_level right = precedence_level::e_level00;
        core::operators::operator_type operation = core::operators::operator_type::default_op;
        Token token;
    };

    static inline bool resolve(const Token& token, state_t& current_state)
    {
        current_state.reset();

        switch (token.type)
        {
            case lexer::token::e_assign:
                current_state.set(precedence_level::e_level00, precedence_level::e_level00,
                                  core::operators::operator_type::assign, token);
                return true;
            case lexer::token::e_addass:
                current_state.set(precedence_level::e_level00, precedence_level::e_level00,
                                  core::operators::operator_type::addass, token);
                return true;
            case lexer::token::e_subass:
                current_state.set(precedence_level::e_level00, precedence_level::e_level00,
                                  core::operators::operator_type::subass, token);
                return true;
            case lexer::token::e_mulass:
                current_state.set(precedence_level::e_level00, precedence_level::e_level00,
                                  core::operators::operator_type::mulass, token);
                return true;
            case lexer::token::e_divass:
                current_state.set(precedence_level::e_level00, precedence_level::e_level00,
                                  core::operators::operator_type::divass, token);
                return true;
            case lexer::token::e_modass:
                current_state.set(precedence_level::e_level00, precedence_level::e_level00,
                                  core::operators::operator_type::modass, token);
                return true;
            case lexer::token::e_swap:
                current_state.set(precedence_level::e_level00, precedence_level::e_level00,
                                  core::operators::operator_type::swap, token);
                return true;
            case lexer::token::e_lt:
                current_state.set(precedence_level::e_level05, precedence_level::e_level06,
                                  core::operators::operator_type::lt, token);
                return true;
            case lexer::token::e_lte:
                current_state.set(precedence_level::e_level05, precedence_level::e_level06,
                                  core::operators::operator_type::lte, token);
                return true;
            case lexer::token::e_eq:
                current_state.set(precedence_level::e_level05, precedence_level::e_level06,
                                  core::operators::operator_type::eq, token);
                return true;
            case lexer::token::e_ne:
                current_state.set(precedence_level::e_level05, precedence_level::e_level06,
                                  core::operators::operator_type::ne, token);
                return true;
            case lexer::token::e_gte:
                current_state.set(precedence_level::e_level05, precedence_level::e_level06,
                                  core::operators::operator_type::gte, token);
                return true;
            case lexer::token::e_gt:
                current_state.set(precedence_level::e_level05, precedence_level::e_level06,
                                  core::operators::operator_type::gt, token);
                return true;
            case lexer::token::e_add:
                current_state.set(precedence_level::e_level07, precedence_level::e_level08,
                                  core::operators::operator_type::add, token);
                return true;
            case lexer::token::e_sub:
                current_state.set(precedence_level::e_level07, precedence_level::e_level08,
                                  core::operators::operator_type::sub, token);
                return true;
            case lexer::token::e_div:
                current_state.set(precedence_level::e_level10, precedence_level::e_level11,
                                  core::operators::operator_type::div, token);
                return true;
            case lexer::token::e_mul:
                current_state.set(precedence_level::e_level10, precedence_level::e_level11,
                                  core::operators::operator_type::mul, token);
                return true;
            case lexer::token::e_mod:
                current_state.set(precedence_level::e_level10, precedence_level::e_level11,
                                  core::operators::operator_type::mod, token);
                return true;
            case lexer::token::e_pow:
                current_state.set(precedence_level::e_level12, precedence_level::e_level12,
                                  core::operators::operator_type::pow, token);
                return true;
            default:
                break;
        }

        if (lexer::token::e_symbol != token.type)
            return false;

        static constexpr std::string_view s_and = "and";
        static constexpr std::string_view s_nand = "nand";
        static constexpr std::string_view s_or = "or";
        static constexpr std::string_view s_nor = "nor";
        static constexpr std::string_view s_xor = "xor";
        static constexpr std::string_view s_xnor = "xnor";
        static constexpr std::string_view s_in = "in";
        static constexpr std::string_view s_like = "like";
        static constexpr std::string_view s_ilike = "ilike";
        static constexpr std::string_view s_and1 = "&";
        static constexpr std::string_view s_or1 = "|";
        static constexpr std::string_view s_not = "not";

        if (core::imatch(token.value, s_and))
        {
            current_state.set(precedence_level::e_level03, precedence_level::e_level04,
                              core::operators::operator_type::logical_and, token);
            return true;
        }
        if (core::imatch(token.value, s_and1))
        {
            current_state.set(precedence_level::e_level03, precedence_level::e_level04,
                              ::math_expr::core::build_options::kDisableScAndOr
                                  ? core::operators::operator_type::logical_and
                                  : core::operators::operator_type::scand,
                              token);
            return true;
        }
        if (core::imatch(token.value, s_nand))
        {
            current_state.set(precedence_level::e_level03, precedence_level::e_level04,
                              core::operators::operator_type::nand, token);
            return true;
        }
        if (core::imatch(token.value, s_or))
        {
            current_state.set(precedence_level::e_level01, precedence_level::e_level02,
                              core::operators::operator_type::logical_or, token);
            return true;
        }
        if (core::imatch(token.value, s_or1))
        {
            current_state.set(precedence_level::e_level01, precedence_level::e_level02,
                              ::math_expr::core::build_options::kDisableScAndOr
                                  ? core::operators::operator_type::logical_or
                                  : core::operators::operator_type::scor,
                              token);
            return true;
        }
        if (core::imatch(token.value, s_nor))
        {
            current_state.set(precedence_level::e_level01, precedence_level::e_level02,
                              core::operators::operator_type::nor, token);
            return true;
        }
        if (core::imatch(token.value, s_xor))
        {
            current_state.set(precedence_level::e_level01, precedence_level::e_level02,
                              core::operators::operator_type::logical_xor, token);
            return true;
        }
        if (core::imatch(token.value, s_xnor))
        {
            current_state.set(precedence_level::e_level01, precedence_level::e_level02,
                              core::operators::operator_type::xnor, token);
            return true;
        }
        if (core::imatch(token.value, s_in))
        {
            current_state.set(precedence_level::e_level04, precedence_level::e_level04,
                              core::operators::operator_type::in, token);
            return true;
        }
        if (core::imatch(token.value, s_like))
        {
            current_state.set(precedence_level::e_level04, precedence_level::e_level04,
                              core::operators::operator_type::like, token);
            return true;
        }
        if (core::imatch(token.value, s_ilike))
        {
            current_state.set(precedence_level::e_level04, precedence_level::e_level04,
                              core::operators::operator_type::ilike, token);
            return true;
        }
        if (core::imatch(token.value, s_not))
        {
            return false;
        }

        return false;
    }
};
}  // namespace math_expr

#endif
