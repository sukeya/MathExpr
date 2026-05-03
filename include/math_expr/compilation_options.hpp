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
   private:
    using flag = uint16_t;

    static constexpr flag unknown_ = 0;
    static constexpr flag replacer_ = 1;
    static constexpr flag joiner_ = 2;
    static constexpr flag numeric_check_ = 4;
    static constexpr flag bracket_check_ = 8;
    static constexpr flag sequence_check_ = 16;
    static constexpr flag commutative_check_ = 32;
    static constexpr flag strength_reduction_ = 64;
    static constexpr flag disable_vardef_ = 128;
    static constexpr flag collect_vars_ = 256;
    static constexpr flag collect_funcs_ = 512;
    static constexpr flag collect_assings_ = 1024;
    static constexpr flag disable_usr_on_rsrvd_ = 2048;
    static constexpr flag disable_zero_return_ = 4096;
    static constexpr flag all_ = 8191;

   public:
    static compilation_options e_unknown()
    {
        return compilation_options(unknown_);
    }
    static compilation_options e_replacer()
    {
        return compilation_options(replacer_);
    }
    static compilation_options e_joiner()
    {
        return compilation_options(joiner_);
    }
    static compilation_options e_numeric_check()
    {
        return compilation_options(numeric_check_);
    }
    static compilation_options e_bracket_check()
    {
        return compilation_options(bracket_check_);
    }
    static compilation_options e_sequence_check()
    {
        return compilation_options(sequence_check_);
    }
    static compilation_options e_commutative_check()
    {
        return compilation_options(commutative_check_);
    }
    static compilation_options e_strength_reduction()
    {
        return compilation_options(strength_reduction_);
    }
    static compilation_options e_disable_vardef()
    {
        return compilation_options(disable_vardef_);
    }
    static compilation_options e_collect_vars()
    {
        return compilation_options(collect_vars_);
    }
    static compilation_options e_collect_funcs()
    {
        return compilation_options(collect_funcs_);
    }
    static compilation_options e_collect_assings()
    {
        return compilation_options(collect_assings_);
    }
    static compilation_options e_disable_usr_on_rsrvd()
    {
        return compilation_options(disable_usr_on_rsrvd_);
    }
    static compilation_options e_disable_zero_return()
    {
        return compilation_options(disable_zero_return_);
    }
    static compilation_options default_all()
    {
        return compilation_options(replacer_ | joiner_ | numeric_check_ | bracket_check_ |
                                   sequence_check_ | commutative_check_ | strength_reduction_);
    }

    compilation_options() = default;
    compilation_options(const compilation_options&) = default;
    compilation_options(compilation_options&&) = default;
    compilation_options& operator=(const compilation_options&) = default;
    compilation_options& operator=(compilation_options&&) = default;
    ~compilation_options() = default;

    explicit compilation_options(flag f) : compile_flags_(f)
    {
        assert(f <= all_);
    }

    bool is_unknown() const
    {
        return compile_flags_ == unknown_;
    }
    bool is_replacer() const
    {
        return (compile_flags_ & replacer_) == replacer_;
    }
    bool is_joiner() const
    {
        return (compile_flags_ & joiner_) == joiner_;
    }
    bool is_numeric_check() const
    {
        return (compile_flags_ & numeric_check_) == numeric_check_;
    }
    bool is_bracket_check() const
    {
        return (compile_flags_ & bracket_check_) == bracket_check_;
    }
    bool is_sequence_check() const
    {
        return (compile_flags_ & sequence_check_) == sequence_check_;
    }
    bool is_commutative_check() const
    {
        return (compile_flags_ & commutative_check_) == commutative_check_;
    }
    bool is_strength_reduction() const
    {
        return (compile_flags_ & strength_reduction_) == strength_reduction_;
    }
    bool is_disable_vardef() const
    {
        return (compile_flags_ & disable_vardef_) == disable_vardef_;
    }
    bool is_collect_vars() const
    {
        return (compile_flags_ & collect_vars_) == collect_vars_;
    }
    bool is_collect_funcs() const
    {
        return (compile_flags_ & collect_funcs_) == collect_funcs_;
    }
    bool is_collect_assings() const
    {
        return (compile_flags_ & collect_assings_) == collect_assings_;
    }
    bool is_disable_usr_on_rsrvd() const
    {
        return (compile_flags_ & disable_usr_on_rsrvd_) == disable_usr_on_rsrvd_;
    }
    bool is_disable_zero_return() const
    {
        return (compile_flags_ & disable_zero_return_) == disable_zero_return_;
    }

    compilation_options operator~() const
    {
        return compilation_options((~compile_flags_) & all_);
    }

    friend compilation_options operator&(compilation_options l, compilation_options r)
    {
        return compilation_options(l.compile_flags_ & r.compile_flags_);
    }

    friend compilation_options operator^(compilation_options l, compilation_options r)
    {
        return compilation_options(l.compile_flags_ ^ r.compile_flags_);
    }

    friend compilation_options operator|(compilation_options l, compilation_options r)
    {
        return compilation_options(l.compile_flags_ | r.compile_flags_);
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
    flag compile_flags_;
};

}  // namespace math_expr

#endif
