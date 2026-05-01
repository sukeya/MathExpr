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

#ifndef MATH_EXPR_CORE_DEBUG_HPP
#define MATH_EXPR_CORE_DEBUG_HPP

#include "math_expr/core/std_includes.hpp"
#include "math_expr/core/config.hpp"
#include "math_expr/core/macros.hpp"

namespace math_expr::core
{
inline void dump_ptr(const std::string& s, const void* ptr, const std::size_t size = 0)
{
    if constexpr (::math_expr::core::build_options::kEnableDebugging)
    {
        if (size)
            math_expr_debug(
                ("%s - addr: %p size: %d\n", s.c_str(), ptr, static_cast<unsigned int>(size)));
        else
            math_expr_debug(("%s - addr: %p\n", s.c_str(), ptr));
    }
}

template <typename T>
inline void dump_vector(const std::string& vec_name, const T* data, const std::size_t size)
{
    if constexpr (::math_expr::core::build_options::kEnableDebugging)
    {
        std::printf("----- %s (%p) -----\n", vec_name.c_str(), static_cast<const void*>(data));
        std::printf("[ ");
        for (std::size_t i = 0; i < size; ++i)
        {
            std::printf("%8.3f\t", data[i]);
        }
        std::printf(" ]\n");
        std::printf("---------------------\n");
    }
}

}  // namespace math_expr::core

#endif
