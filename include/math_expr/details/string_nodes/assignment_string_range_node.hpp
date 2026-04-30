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

#ifndef MATH_EXPR_DETAILS_ASSIGNMENT_STRING_RANGE_NODE_HPP
#define MATH_EXPR_DETAILS_ASSIGNMENT_STRING_RANGE_NODE_HPP

#include "math_expr/details/operator_nodes.hpp"
#include "math_expr/details/string_nodes/string_assignment_process.hpp"
#include "math_expr/details/string_nodes/string_range_node.hpp"

namespace math_expr::details::string_nodes
{
template <typename T, typename AssignmentProcess = asn_assignment>
class assignment_string_range_node final : public binary_node<T>,
                                           public string_base_node<T>,
                                           public range_interface<T>
{
  public:
    using range_t = typename range_interface<T>::range_t;
    using range_ptr = range_t*;
    using irange_t = range_interface<T>;
    using irange_ptr = irange_t*;
    using expression_ptr = expression_node<T>*;
    using strvar_node_ptr = stringvar_node<T>*;
    using str_rng_node_ptr = string_range_node<T>*;
    using str_base_ptr = string_base_node<T>*;

    using binary_node<T>::branch;

    assignment_string_range_node(const core::operators::operator_type& opr, expression_ptr branch0,
                                 expression_ptr branch1)
        : binary_node<T>(opr, branch0, branch1), initialised_(false), str0_base_ptr_(0),
          str1_base_ptr_(0), str0_rng_node_ptr_(0), str0_range_ptr_(0), str1_range_ptr_(0)
    {
        if (is_string_range_node(branch(0)))
        {
            str0_rng_node_ptr_ = static_cast<str_rng_node_ptr>(branch(0));
            str0_base_ptr_ = dynamic_cast<str_base_ptr>(branch(0));
            irange_ptr range = dynamic_cast<irange_ptr>(branch(0));

            if (0 == range)
                return;

            str0_range_ptr_ = &(range->range_ref());
        }

        if (is_generally_string_node(branch(1)))
        {
            str1_base_ptr_ = dynamic_cast<str_base_ptr>(branch(1));

            if (0 == str1_base_ptr_)
                return;

            irange_ptr range = dynamic_cast<irange_ptr>(branch(1));

            if (0 == range)
                return;

            str1_range_ptr_ = &(range->range_ref());
        }

        initialised_ = str0_base_ptr_ && str1_base_ptr_ && str0_rng_node_ptr_ && str0_range_ptr_ &&
                       str1_range_ptr_;

        assert(valid());
    }

    inline T value() const override
    {
        branch(0)->value();
        branch(1)->value();

        std::size_t s0_r0 = 0;
        std::size_t s0_r1 = 0;

        std::size_t s1_r0 = 0;
        std::size_t s1_r1 = 0;

        const range_t& range0 = (*str0_range_ptr_);
        const range_t& range1 = (*str1_range_ptr_);

        if (range0(s0_r0, s0_r1, str0_base_ptr_->size()) &&
            range1(s1_r0, s1_r1, str1_base_ptr_->size()))
        {
            const std::size_t size = std::min((s0_r1 - s0_r0), (s1_r1 - s1_r0));

            std::copy(str1_base_ptr_->base() + s1_r0, str1_base_ptr_->base() + s1_r0 + size,
                      const_cast<core::char_ptr>(base() + s0_r0));
        }

        return std::numeric_limits<T>::quiet_NaN();
    }

    std::string str() const override
    {
        return str0_base_ptr_->str();
    }

    core::char_cptr base() const override
    {
        return str0_base_ptr_->base();
    }

    std::size_t size() const override
    {
        return str0_base_ptr_->size();
    }

    range_t& range_ref() override
    {
        return str0_rng_node_ptr_->range_ref();
    }

    const range_t& range_ref() const override
    {
        return str0_rng_node_ptr_->range_ref();
    }

    inline typename expression_node<T>::node_type type() const override
    {
        return expression_node<T>::node_type::e_strass;
    }

    inline bool valid() const override
    {
        return initialised_ && binary_node<T>::valid();
    }

  private:
    bool initialised_;
    str_base_ptr str0_base_ptr_;
    str_base_ptr str1_base_ptr_;
    str_rng_node_ptr str0_rng_node_ptr_;
    range_ptr str0_range_ptr_;
    range_ptr str1_range_ptr_;
};
} // namespace math_expr::details::string_nodes

#endif
