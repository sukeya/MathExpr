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

#ifndef MATH_EXPR_CORE_TYPES_HPP
#define MATH_EXPR_CORE_TYPES_HPP

#include "math_expr/core/std_includes.hpp"

namespace math_expr::core
{
typedef char char_t;
typedef char_t* char_ptr;
typedef char_t const* char_cptr;
typedef unsigned char uchar_t;
typedef uchar_t* uchar_ptr;
typedef uchar_t const* uchar_cptr;

inline bool is_whitespace(const char_t c)
{
    return (' ' == c) || ('\n' == c) || ('\r' == c) || ('\t' == c) || ('\b' == c) || ('\v' == c) ||
           ('\f' == c);
}

inline bool is_operator_char(const char_t c)
{
    return ('+' == c) || ('-' == c) || ('*' == c) || ('/' == c) || ('^' == c) || ('<' == c) ||
           ('>' == c) || ('=' == c) || (',' == c) || ('!' == c) || ('(' == c) || (')' == c) ||
           ('[' == c) || (']' == c) || ('{' == c) || ('}' == c) || ('%' == c) || (':' == c) ||
           ('?' == c) || ('&' == c) || ('|' == c) || (';' == c);
}

inline bool is_letter(const char_t c)
{
    return (('a' <= c) && (c <= 'z')) || (('A' <= c) && (c <= 'Z'));
}

inline bool is_digit(const char_t c)
{
    return ('0' <= c) && (c <= '9');
}

inline bool is_letter_or_digit(const char_t c)
{
    return is_letter(c) || is_digit(c);
}

inline bool is_left_bracket(const char_t c)
{
    return ('(' == c) || ('[' == c) || ('{' == c);
}

inline bool is_right_bracket(const char_t c)
{
    return (')' == c) || (']' == c) || ('}' == c);
}

inline bool is_bracket(const char_t c)
{
    return is_left_bracket(c) || is_right_bracket(c);
}

inline bool is_sign(const char_t c)
{
    return ('+' == c) || ('-' == c);
}

inline bool is_invalid(const char_t c)
{
    return !is_whitespace(c) && !is_operator_char(c) && !is_letter(c) && !is_digit(c) &&
           ('.' != c) && ('_' != c) && ('$' != c) && ('~' != c) && ('\'' != c);
}

inline bool is_valid_string_char(const char_t c)
{
    return std::isprint(static_cast<uchar_t>(c)) || is_whitespace(c);
}

} // namespace math_expr::core

#endif
