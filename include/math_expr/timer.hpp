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

#ifndef MATH_EXPR_TIMER_HPP
#define MATH_EXPR_TIMER_HPP

#include "math_expr/core/std_includes.hpp"
#include "math_expr/core/types.hpp"

#if defined(_MSC_VER) || defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <ctime>
#else
#include <ctime>
#include <sys/time.h>
#include <sys/types.h>
#endif

namespace math_expr
{
class timer
{
  public:
#if defined(_MSC_VER) || defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
    timer() : in_use_(false), start_time_{{0, 0}}, stop_time_{{0, 0}}
    {
        QueryPerformanceFrequency(&clock_frequency_);
    }

    inline void start()
    {
        in_use_ = true;
        QueryPerformanceCounter(&start_time_);
    }

    inline void stop()
    {
        QueryPerformanceCounter(&stop_time_);
        in_use_ = false;
    }

    inline double time() const
    {
        return (1.0 * (stop_time_.QuadPart - start_time_.QuadPart)) /
               (1.0 * clock_frequency_.QuadPart);
    }

#else

    timer() : in_use_(false)
    {
        start_time_.tv_sec = 0;
        start_time_.tv_usec = 0;

        stop_time_.tv_sec = 0;
        stop_time_.tv_usec = 0;
    }

    inline void start()
    {
        in_use_ = true;
        gettimeofday(&start_time_, 0);
    }

    inline void stop()
    {
        gettimeofday(&stop_time_, 0);
        in_use_ = false;
    }

    inline std::uint64_t usec_time() const
    {
        if (!in_use_)
        {
            if (stop_time_.tv_sec >= start_time_.tv_sec)
            {
                return 1000000LLU *
                           static_cast<std::uint64_t>(stop_time_.tv_sec - start_time_.tv_sec) +
                       static_cast<std::uint64_t>(stop_time_.tv_usec - start_time_.tv_usec);
            }
            else
                return std::numeric_limits<std::uint64_t>::max();
        }
        else
            return std::numeric_limits<std::uint64_t>::max();
    }

    inline double time() const
    {
        return usec_time() * 0.000001;
    }

#endif

    inline bool in_use() const
    {
        return in_use_;
    }

  private:
    bool in_use_;

#if defined(_MSC_VER) || defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
    LARGE_INTEGER start_time_;
    LARGE_INTEGER stop_time_;
    LARGE_INTEGER clock_frequency_;
#else
    struct timeval start_time_;
    struct timeval stop_time_;
#endif
};

} // namespace math_expr

#endif
