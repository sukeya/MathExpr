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

#ifndef MATH_EXPR_COLLECT_TYPES_HPP
#define MATH_EXPR_COLLECT_TYPES_HPP

#include "math_expr/core/std_includes.hpp"

namespace math_expr
{

class collect_types
{
   private:
    using flag = uint8_t;

    static constexpr flag none_ = 0;
    static constexpr flag variables_ = 1;
    static constexpr flag functions_ = 2;
    static constexpr flag assignments_ = 4;
    static constexpr flag all_ = 7;

   public:
    static collect_types e_none()
    {
        return collect_types(none_);
    }
    static collect_types e_variables()
    {
        return collect_types(variables_);
    }
    static collect_types e_functions()
    {
        return collect_types(functions_);
    }
    static collect_types e_assignments()
    {
        return collect_types(assignments_);
    }
    static collect_types e_all()
    {
        return collect_types(all_);
    }

    collect_types() = default;
    collect_types(const collect_types&) = default;
    collect_types(collect_types&&) = default;
    collect_types& operator=(const collect_types&) = default;
    collect_types& operator=(collect_types&&) = default;
    ~collect_types() = default;

    explicit collect_types(flag f) : collect_flags_(f)
    {
        assert(f <= all_);
    }

    bool is_none() const
    {
        return collect_flags_ == none_;
    }
    bool is_variables() const
    {
        return (collect_flags_ & variables_) == variables_;
    }
    bool is_functions() const
    {
        return (collect_flags_ & functions_) == functions_;
    }
    bool is_assignments() const
    {
        return (collect_flags_ & assignments_) == assignments_;
    }
    bool is_all() const
    {
        return (collect_flags_ & all_) == all_;
    }

    collect_types operator~() const
    {
        return collect_types((~collect_flags_) & all_);
    }

    friend collect_types operator&(collect_types l, collect_types r)
    {
        return collect_types(l.collect_flags_ & r.collect_flags_);
    }

    friend collect_types operator^(collect_types l, collect_types r)
    {
        return collect_types(l.collect_flags_ ^ r.collect_flags_);
    }

    friend collect_types operator|(collect_types l, collect_types r)
    {
        return collect_types(l.collect_flags_ | r.collect_flags_);
    }

    friend bool operator==(collect_types l, collect_types r)
    {
        return l.collect_flags_ == r.collect_flags_;
    }

    friend bool operator!=(collect_types l, collect_types r)
    {
        return !(l == r);
    }

   private:
    flag collect_flags_;
};

}  // namespace math_expr

#endif
