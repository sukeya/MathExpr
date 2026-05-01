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

#ifndef MATH_EXPR_DETAILS_RANGE_PACK_HPP
#define MATH_EXPR_DETAILS_RANGE_PACK_HPP

#include "math_expr/details/literal_nodes.hpp"

namespace math_expr::details
{
template <typename T>
struct range_pack
{
    using expression_node_ptr = expression_node<T>*;
    using cached_range_t = std::pair<std::size_t, std::size_t>;

    range_pack()
        : n0_e(std::make_pair(false, expression_node_ptr(0))),
          n1_e(std::make_pair(false, expression_node_ptr(0))),
          n0_c(std::make_pair(false, 0)),
          n1_c(std::make_pair(false, 0)),
          cache(std::make_pair(0, 0))
    {
    }

    void clear()
    {
        n0_e = std::make_pair(false, expression_node_ptr(0));
        n1_e = std::make_pair(false, expression_node_ptr(0));
        n0_c = std::make_pair(false, 0);
        n1_c = std::make_pair(false, 0);
        cache = std::make_pair(0, 0);
    }

    void free()
    {
        if (n0_e.first && n0_e.second)
        {
            n0_e.first = false;

            if (!is_variable_node(n0_e.second) && !is_string_node(n0_e.second))
            {
                destroy_node(n0_e.second);
            }
        }

        if (n1_e.first && n1_e.second)
        {
            n1_e.first = false;

            if (!is_variable_node(n1_e.second) && !is_string_node(n1_e.second))
            {
                destroy_node(n1_e.second);
            }
        }
    }

    bool const_range() const
    {
        return (n0_c.first && n1_c.first) && (!n0_e.first && !n1_e.first);
    }

    bool var_range() const
    {
        return (n0_e.first && n1_e.first) && (!n0_c.first && !n1_c.first);
    }

    bool operator()(std::size_t& r0, std::size_t& r1,
                    const std::size_t& size = std::numeric_limits<std::size_t>::max()) const
    {
        if (n0_c.first)
            r0 = n0_c.second;
        else if (n0_e.first)
        {
            r0 = static_cast<std::size_t>(core::numeric::to_int64(n0_e.second->value()));
        }
        else
            return false;

        if (n1_c.first)
            r1 = n1_c.second;
        else if (n1_e.first)
        {
            r1 = static_cast<std::size_t>(core::numeric::to_int64(n1_e.second->value()));
        }
        else
            return false;

        if ((std::numeric_limits<std::size_t>::max() != size) &&
            (std::numeric_limits<std::size_t>::max() == r1))
        {
            r1 = size;
        }

        cache.first = r0;
        cache.second = r1;

        if constexpr (::math_expr::core::build_options::kEnableRangeRuntimeChecks)
        {
            return range_runtime_check(r0, r1, size);
        }
        else
        {
            return (r0 <= r1);
        }
    }

    inline std::size_t const_size() const
    {
        return (n1_c.second - n0_c.second);
    }

    inline std::size_t cache_size() const
    {
        return (cache.second - cache.first);
    }

    std::pair<bool, expression_node_ptr> n0_e;
    std::pair<bool, expression_node_ptr> n1_e;
    std::pair<bool, std::size_t> n0_c;
    std::pair<bool, std::size_t> n1_c;
    mutable cached_range_t cache;

    bool range_runtime_check(const std::size_t r0, const std::size_t r1,
                             const std::size_t size) const
    {
        if (r0 > size)
        {
            throw std::runtime_error("range error: (r0 < 0) || (r0 > size)");
#ifdef __NVCOMPILER
            return false;
#endif
        }

        if (r1 > size)
        {
            throw std::runtime_error("range error: (r1 < 0) || (r1 > size)");
#ifdef __NVCOMPILER
            return false;
#endif
        }

        return (r0 <= r1);
    }
};
}  // namespace math_expr::details

#endif
