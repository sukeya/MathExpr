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

#ifndef MATH_EXPR_PARSER_UNKNOWN_SYMBOL_RESOLVER_HPP
#define MATH_EXPR_PARSER_UNKNOWN_SYMBOL_RESOLVER_HPP

#include "math_expr/symbol_table.hpp"

namespace math_expr
{

template <typename T>
struct unknown_symbol_resolver
{
    enum class usr_symbol_type
    {
        e_usr_unknown_type = 0,
        e_usr_variable_type = 1,
        e_usr_constant_type = 2
    };

    enum class usr_mode
    {
        e_usrmode_default = 0,
        e_usrmode_extended = 1
    };

    usr_mode mode;

    explicit unknown_symbol_resolver(const usr_mode m = usr_mode::e_usrmode_default) : mode(m) {}

    virtual ~unknown_symbol_resolver() {}

    virtual bool process(const std::string& /*unknown_symbol*/, usr_symbol_type& st,
                         T& default_value, std::string& error_message)
    {
        if (usr_mode::e_usrmode_default != mode)
            return false;

        st = usr_symbol_type::e_usr_variable_type;
        default_value = T(0);
        error_message.clear();

        return true;
    }

    virtual bool process(const std::string& /* unknown_symbol */,
                         symbol_table<T>& /* symbol_table   */, std::string& /* error_message */)
    {
        return false;
    }
};

}  // namespace math_expr

#endif
