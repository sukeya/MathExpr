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

#ifndef MATH_EXPR_LOOP_TYPES_HPP
#define MATH_EXPR_LOOP_TYPES_HPP

#include "math_expr/core/std_includes.hpp"

class loop_types
{
   public:
    using flag = uint8_t;

    static constexpr flag e_invalid = 0;
    static constexpr flag e_for_loop = 1;
    static constexpr flag e_while_loop = 2;
    static constexpr flag e_repeat_until_loop = 4;
    static constexpr flag e_all_loops = 7;

    loop_types() = default;
    loop_types(const loop_types&) = default;
    loop_types(loop_types&&) = default;
    loop_types& operator=(const loop_types&) = default;
    loop_types& operator=(loop_types&&) = default;
    ~loop_types() = default;

    loop_types(flag f) : loop_flags_(f)
    {
        assert(f <= e_all_loops);
    }

    bool is_invalid() const
    {
        return (loop_flags_ & e_invalid) == e_invalid;
    }
    bool is_for_loop() const
    {
        return (loop_flags_ & e_for_loop) == e_for_loop;
    }
    bool is_while_loop() const
    {
        return (loop_flags_ & e_while_loop) == e_while_loop;
    }
    bool is_repeat_until_loop() const
    {
        return (loop_flags_ & e_repeat_until_loop) == e_repeat_until_loop;
    }
    bool is_all_loops() const
    {
        return (loop_flags_ & e_all_loops) == e_all_loops;
    }

    loop_types operator~() const
    {
        return loop_types((~loop_flags_) & e_all_loops);
    }

    friend loop_types operator&(loop_types l, loop_types r)
    {
        return loop_types(l.loop_flags_ & r.loop_flags_);
    }

    friend loop_types operator^(loop_types l, loop_types r)
    {
        return loop_types(l.loop_flags_ ^ r.loop_flags_);
    }

    friend loop_types operator|(loop_types l, loop_types r)
    {
        return loop_types(l.loop_flags_ | r.loop_flags_);
    }

    friend bool operator==(loop_types l, loop_types r)
    {
        return l.loop_flags_ == r.loop_flags_;
    }

    friend bool operator!=(loop_types l, loop_types r)
    {
        return !(l == r);
    }

   private:
    flag loop_flags_;
};

#endif
