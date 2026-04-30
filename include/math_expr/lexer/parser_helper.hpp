/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * Simple Example 02                                          *
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

#ifndef MATH_EXPR_LEXER_PARSER_HELPER_HPP
#define MATH_EXPR_LEXER_PARSER_HELPER_HPP

#include "math_expr/core/string_utils.hpp"
#include "math_expr/lexer/generator.hpp"

namespace math_expr::lexer
{
class parser_helper
{
  public:
    typedef token token_t;
    typedef generator generator_t;

    inline bool init(const std::string& str)
    {
        if (!lexer_.process(str))
        {
            return false;
        }

        lexer_.begin();

        next_token();

        return true;
    }

    inline generator_t& lexer()
    {
        return lexer_;
    }

    inline const generator_t& lexer() const
    {
        return lexer_;
    }

    inline void store_token()
    {
        lexer_.store();
        store_current_token_ = current_token_;
    }

    inline void restore_token()
    {
        lexer_.restore();
        current_token_ = store_current_token_;
    }

    inline void next_token()
    {
        current_token_ = lexer_.next_token();
    }

    inline const token_t& current_token() const
    {
        return current_token_;
    }

    inline const token_t& peek_next_token()
    {
        return lexer_.peek_next_token();
    }

    enum class token_advance_mode
    {
        e_hold = 0,
        e_advance = 1
    };

    inline void advance_token(const token_advance_mode mode)
    {
        if (token_advance_mode::e_advance == mode)
        {
            next_token();
        }
    }

    inline bool token_is(const token_t::token_type& ttype,
                         const token_advance_mode mode = token_advance_mode::e_advance)
    {
        if (current_token().type != ttype)
        {
            return false;
        }

        advance_token(mode);

        return true;
    }

    inline bool token_is(const token_t::token_type& ttype, const std::string& value,
                         const token_advance_mode mode = token_advance_mode::e_advance)
    {
        if ((current_token().type != ttype) ||
            !math_expr::core::imatch(value, current_token().value))
        {
            return false;
        }

        advance_token(mode);

        return true;
    }

    inline bool token_is(const std::string& value,
                         const token_advance_mode mode = token_advance_mode::e_advance)
    {
        if (!math_expr::core::imatch(value, current_token().value))
        {
            return false;
        }

        advance_token(mode);

        return true;
    }

    inline bool
    token_is_arithmetic_opr(const token_advance_mode mode = token_advance_mode::e_advance)
    {
        switch (current_token().type)
        {
        case token_t::e_add:
        case token_t::e_sub:
        case token_t::e_div:
        case token_t::e_mul:
        case token_t::e_mod:
        case token_t::e_pow:
            break;
        default:
            return false;
        }

        advance_token(mode);

        return true;
    }

    inline bool token_is_ineq_opr(const token_advance_mode mode = token_advance_mode::e_advance)
    {
        switch (current_token().type)
        {
        case token_t::e_eq:
        case token_t::e_lte:
        case token_t::e_ne:
        case token_t::e_gte:
        case token_t::e_lt:
        case token_t::e_gt:
            break;
        default:
            return false;
        }

        advance_token(mode);

        return true;
    }

    inline bool token_is_left_bracket(const token_advance_mode mode = token_advance_mode::e_advance)
    {
        switch (current_token().type)
        {
        case token_t::e_lbracket:
        case token_t::e_lcrlbracket:
        case token_t::e_lsqrbracket:
            break;
        default:
            return false;
        }

        advance_token(mode);

        return true;
    }

    inline bool
    token_is_right_bracket(const token_advance_mode mode = token_advance_mode::e_advance)
    {
        switch (current_token().type)
        {
        case token_t::e_rbracket:
        case token_t::e_rcrlbracket:
        case token_t::e_rsqrbracket:
            break;
        default:
            return false;
        }

        advance_token(mode);

        return true;
    }

    inline bool token_is_bracket(const token_advance_mode mode = token_advance_mode::e_advance)
    {
        switch (current_token().type)
        {
        case token_t::e_rbracket:
        case token_t::e_rcrlbracket:
        case token_t::e_rsqrbracket:
        case token_t::e_lbracket:
        case token_t::e_lcrlbracket:
        case token_t::e_lsqrbracket:
            break;
        default:
            return false;
        }

        advance_token(mode);

        return true;
    }

    inline bool token_is_loop(const token_advance_mode mode = token_advance_mode::e_advance)
    {
        return token_is("for", mode) || token_is("while", mode) || token_is("repeat", mode);
    }

    inline bool peek_token_is(const token_t::token_type& ttype)
    {
        return (lexer_.peek_next_token().type == ttype);
    }

    inline bool peek_token_is(const std::string& s)
    {
        return (math_expr::core::imatch(lexer_.peek_next_token().value, s));
    }

  private:
    generator_t lexer_;
    token_t current_token_;
    token_t store_current_token_;
};
} // namespace math_expr::lexer

#endif
