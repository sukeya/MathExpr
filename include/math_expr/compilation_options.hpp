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

#ifndef MATH_EXPR_COMPILATION_OPTIONS_HPP
#define MATH_EXPR_COMPILATION_OPTIONS_HPP

#include "math_expr/core/std_includes.hpp"

namespace math_expr
{

class compilation_options
{
   public:
    using flag = uint16_t;

    static constexpr flag e_unknown = 0;
    static constexpr flag e_replacer = 1;
    static constexpr flag e_joiner = 2;
    static constexpr flag e_numeric_check = 4;
    static constexpr flag e_bracket_check = 8;
    static constexpr flag e_sequence_check = 16;
    static constexpr flag e_commutative_check = 32;
    static constexpr flag e_strength_reduction = 64;
    static constexpr flag e_disable_vardef = 128;
    static constexpr flag e_collect_vars = 256;
    static constexpr flag e_collect_funcs = 512;
    static constexpr flag e_collect_assings = 1024;
    static constexpr flag e_disable_usr_on_rsrvd = 2048;
    static constexpr flag e_disable_zero_return = 4096;
    static constexpr flag e_all = 8191;

    static const compilation_options default_all;

    compilation_options() = default;
    compilation_options(const compilation_options&) = default;
    compilation_options(compilation_options&&) = default;
    compilation_options& operator=(const compilation_options&) = default;
    compilation_options& operator=(compilation_options&&) = default;
    ~compilation_options() = default;

    compilation_options(flag f) : compile_flags_(f)
    {
        assert(f <= e_all);
    }

    bool is_unknown() const
    {
        return compile_flags_ == e_unknown;
    }
    bool is_replacer() const
    {
        return (compile_flags_ & e_replacer) == e_replacer;
    }
    bool is_joiner() const
    {
        return (compile_flags_ & e_joiner) == e_joiner;
    }
    bool is_numeric_check() const
    {
        return (compile_flags_ & e_numeric_check) == e_numeric_check;
    }
    bool is_bracket_check() const
    {
        return (compile_flags_ & e_bracket_check) == e_bracket_check;
    }
    bool is_sequence_check() const
    {
        return (compile_flags_ & e_sequence_check) == e_sequence_check;
    }
    bool is_commutative_check() const
    {
        return (compile_flags_ & e_commutative_check) == e_commutative_check;
    }
    bool is_strength_reduction() const
    {
        return (compile_flags_ & e_strength_reduction) == e_strength_reduction;
    }
    bool is_disable_vardef() const
    {
        return (compile_flags_ & e_disable_vardef) == e_disable_vardef;
    }
    bool is_collect_vars() const
    {
        return (compile_flags_ & e_collect_vars) == e_collect_vars;
    }
    bool is_collect_funcs() const
    {
        return (compile_flags_ & e_collect_funcs) == e_collect_funcs;
    }
    bool is_collect_assings() const
    {
        return (compile_flags_ & e_collect_assings) == e_collect_assings;
    }
    bool is_disable_usr_on_rsrvd() const
    {
        return (compile_flags_ & e_disable_usr_on_rsrvd) == e_disable_usr_on_rsrvd;
    }
    bool is_disable_zero_return() const
    {
        return (compile_flags_ & e_disable_zero_return) == e_disable_zero_return;
    }

    compilation_options operator~() const
    {
        return compilation_options(static_cast<flag>((~compile_flags_) & e_all));
    }

    friend compilation_options operator&(compilation_options l, compilation_options r)
    {
        return compilation_options(static_cast<flag>(l.compile_flags_ & r.compile_flags_));
    }

    friend compilation_options operator^(compilation_options l, compilation_options r)
    {
        return compilation_options(static_cast<flag>(l.compile_flags_ ^ r.compile_flags_));
    }

    friend compilation_options operator|(compilation_options l, compilation_options r)
    {
        return compilation_options(static_cast<flag>(l.compile_flags_ | r.compile_flags_));
    }

    friend bool operator==(compilation_options l, compilation_options r)
    {
        return l.compile_flags_ == r.compile_flags_;
    }

    friend bool operator!=(compilation_options l, compilation_options r)
    {
        return !(l == r);
    }

   private:
    flag compile_flags_ = e_unknown;
};

inline const compilation_options compilation_options::default_all(
    static_cast<compilation_options::flag>(
        compilation_options::e_replacer | compilation_options::e_joiner |
        compilation_options::e_numeric_check | compilation_options::e_bracket_check |
        compilation_options::e_sequence_check | compilation_options::e_commutative_check |
        compilation_options::e_strength_reduction));

}  // namespace math_expr

#endif
