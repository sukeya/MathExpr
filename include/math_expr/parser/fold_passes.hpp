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

#ifndef MATH_EXPR_PARSER_FOLD_PASSES_HPP
#define MATH_EXPR_PARSER_FOLD_PASSES_HPP

#include "math_expr/core/std_includes.hpp"
#include "math_expr/details/node_utils.hpp"

namespace math_expr
{
template <typename T>
class fold_passes
{
   public:
    template <std::size_t N, typename NodePtr>
    static inline bool is_constant_foldable(NodePtr (&branch)[N])
    {
        for (std::size_t i = 0; i < N; ++i)
        {
            if ((nullptr == branch[i]) || !details::is_constant_node(branch[i]))
            {
                return false;
            }
        }

        return true;
    }

    template <typename NodePtr, typename Allocator, template <typename, typename> class Sequence>
    static inline bool is_constant_foldable(const Sequence<NodePtr, Allocator>& branch)
    {
        for (std::size_t i = 0; i < branch.size(); ++i)
        {
            if ((nullptr == branch[i]) || !details::is_constant_node(branch[i]))
            {
                return false;
            }
        }

        return true;
    }
};
}  // namespace math_expr

#endif
