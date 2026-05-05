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

#ifndef MATH_EXPR_DETAILS_SWAP_GENSTRINGS_NODE_HPP
#define MATH_EXPR_DETAILS_SWAP_GENSTRINGS_NODE_HPP

#include "math_expr/details/operator_nodes.hpp"

namespace math_expr::details::string_nodes
{
template <typename T>
class swap_genstrings_node final : public binary_node<T>
{
   public:
    using range_t = typename range_interface<T>::range_t;
    using range_ptr = range_t*;
    using irange_t = range_interface<T>;
    using irange_ptr = irange_t*;
    using expression_ptr = expression_node<T>*;
    using str_base_ptr = string_base_node<T>*;

    using binary_node<T>::branch;

    swap_genstrings_node(expression_ptr branch0, expression_ptr branch1)
        : binary_node<T>(core::operators::operator_type::default_op, branch0, branch1),
          str0_base_ptr_(nullptr),
          str1_base_ptr_(nullptr),
          str0_range_ptr_(nullptr),
          str1_range_ptr_(nullptr),
          initialised_(false)
    {
        if (is_generally_string_node(branch(0)))
        {
            str0_base_ptr_ = branch(0)->as_string_base();

            if (nullptr == str0_base_ptr_)
                return;

            irange_ptr range = branch(0)->as_range_iface();

            if (nullptr == range)
                return;

            str0_range_ptr_ = &(range->range_ref());
        }

        if (is_generally_string_node(branch(1)))
        {
            str1_base_ptr_ = branch(1)->as_string_base();

            if (nullptr == str1_base_ptr_)
                return;

            irange_ptr range = branch(1)->as_range_iface();

            if (nullptr == range)
                return;

            str1_range_ptr_ = &(range->range_ref());
        }

        initialised_ = str0_base_ptr_ && str1_base_ptr_ && str0_range_ptr_ && str1_range_ptr_;

        assert(valid());
    }

    inline T value() const override
    {
        branch(0)->value();
        branch(1)->value();

        std::size_t str0_r0 = 0;
        std::size_t str0_r1 = 0;

        std::size_t str1_r0 = 0;
        std::size_t str1_r1 = 0;

        const range_t& range0 = (*str0_range_ptr_);
        const range_t& range1 = (*str1_range_ptr_);

        if (range0(str0_r0, str0_r1, str0_base_ptr_->size()) &&
            range1(str1_r0, str1_r1, str1_base_ptr_->size()))
        {
            const std::size_t size0 = range0.cache_size();
            const std::size_t size1 = range1.cache_size();
            const std::size_t max_size = std::min(size0, size1);

            core::char_ptr s0 = const_cast<core::char_ptr>(str0_base_ptr_->base() + str0_r0);
            core::char_ptr s1 = const_cast<core::char_ptr>(str1_base_ptr_->base() + str1_r0);

            core::operators::loop_unroll lud(max_size);
            core::char_cptr upper_bound = s0 + lud.upper_bound;

            while (s0 < upper_bound)
            {
                lud.foreach_batch([&s0, &s1](unsigned int i) { std::swap(s0[i], s1[i]); });

                s0 += lud.loop_batch_size;
                s1 += lud.loop_batch_size;
            }

            int i = 0;

            lud.foreach_remainder(
                [&i, s0, s1]()
                {
                    std::swap(s0[i], s1[i]);
                    ++i;
                });
        }

        return std::numeric_limits<T>::quiet_NaN();
    }

    inline typename expression_node<T>::node_type type() const override
    {
        return expression_node<T>::node_type::e_strswap;
    }

    inline bool valid() const override
    {
        return initialised_ && binary_node<T>::valid();
    }

   private:
    swap_genstrings_node(const swap_genstrings_node<T>&) = delete;
    swap_genstrings_node<T>& operator=(const swap_genstrings_node<T>&) = delete;

    str_base_ptr str0_base_ptr_;
    str_base_ptr str1_base_ptr_;
    range_ptr str0_range_ptr_;
    range_ptr str1_range_ptr_;
    bool initialised_;
};
}  // namespace math_expr::details::string_nodes

#endif
