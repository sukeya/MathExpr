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

#ifndef MATH_EXPR_TIMER_HPP
#define MATH_EXPR_TIMER_HPP

#include <chrono>

#include "math_expr/core/std_includes.hpp"
#include "math_expr/core/types.hpp"

namespace math_expr
{
class timer
{
   public:
    using clock_t = std::chrono::steady_clock;
    using time_point_t = typename clock_t::time_point;

    timer() : in_use_(false), start_time_(), stop_time_() {}

    inline void start()
    {
        in_use_ = true;
        start_time_ = clock_t::now();
    }

    inline void stop()
    {
        stop_time_ = clock_t::now();
        in_use_ = false;
    }

    inline std::uint64_t usec_time() const
    {
        if (in_use_)
        {
            return std::numeric_limits<std::uint64_t>::max();
        }

        if (stop_time_ < start_time_)
        {
            return std::numeric_limits<std::uint64_t>::max();
        }

        return static_cast<std::uint64_t>(
            std::chrono::duration_cast<std::chrono::microseconds>(stop_time_ - start_time_)
                .count());
    }

    inline double time() const
    {
        return usec_time() * 0.000001;
    }

    inline bool in_use() const
    {
        return in_use_;
    }

   private:
    bool in_use_;
    time_point_t start_time_;
    time_point_t stop_time_;
};

}  // namespace math_expr

#endif
