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

#ifndef MATH_EXPR_DETAILS_GENERIC_STRING_RANGE_NODE_HPP
#define MATH_EXPR_DETAILS_GENERIC_STRING_RANGE_NODE_HPP

#include "math_expr/details/string_nodes/stringvar_node.hpp"

namespace math_expr::details::string_nodes
{
template <typename T>
class generic_string_range_node final : public expression_node<T>,
                                        public string_base_node<T>,
                                        public range_interface<T>
{
  public:
    typedef expression_node<T>* expression_ptr;
    typedef stringvar_node<T>* strvar_node_ptr;
    typedef string_base_node<T>* str_base_ptr;
    typedef typename range_interface<T>::range_t range_t;
    typedef range_t* range_ptr;
    typedef range_interface<T> irange_t;
    typedef irange_t* irange_ptr;
    typedef std::pair<expression_ptr, bool> branch_t;

    generic_string_range_node(expression_ptr str_branch, const range_t& brange)
        : initialised_(false), str_base_ptr_(0), str_range_ptr_(0), base_range_(brange)
    {
        range_.n0_c = std::make_pair<bool, std::size_t>(true, 0);
        range_.n1_c = std::make_pair<bool, std::size_t>(true, 0);
        range_.cache.first = range_.n0_c.second;
        range_.cache.second = range_.n1_c.second;

        construct_branch_pair(branch_, str_branch);

        if (is_generally_string_node(branch_.first))
        {
            str_base_ptr_ = dynamic_cast<str_base_ptr>(branch_.first);

            if (0 == str_base_ptr_)
                return;

            str_range_ptr_ = dynamic_cast<irange_ptr>(branch_.first);

            if (0 == str_range_ptr_)
                return;
        }

        initialised_ = (str_base_ptr_ && str_range_ptr_);
        assert(valid());
    }

    ~generic_string_range_node() override
    {
        base_range_.free();
    }

    inline T value() const override
    {
        branch_.first->value();

        std::size_t str_r0 = 0;
        std::size_t str_r1 = 0;

        std::size_t r0 = 0;
        std::size_t r1 = 0;

        const range_t& range = str_range_ptr_->range_ref();

        const std::size_t base_str_size = str_base_ptr_->size();

        if (range(str_r0, str_r1, base_str_size) && base_range_(r0, r1, base_str_size - str_r0))
        {
            const std::size_t size = r1 - r0;

            range_.n1_c.second = size;
            range_.cache.second = range_.n1_c.second;

            value_.assign(str_base_ptr_->base() + str_r0 + r0, size);
        }

        return std::numeric_limits<T>::quiet_NaN();
    }

    std::string str() const override
    {
        return value_;
    }

    core::char_cptr base() const override
    {
        return &value_[0];
    }

    std::size_t size() const override
    {
        return value_.size();
    }

    range_t& range_ref() override
    {
        return range_;
    }

    const range_t& range_ref() const override
    {
        return range_;
    }

    inline typename expression_node<T>::node_type type() const override
    {
        return expression_node<T>::e_strgenrange;
    }

    inline bool valid() const override
    {
        return initialised_ && branch_.first;
    }

    void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) override
    {
        expression_node<T>::ndb_t::collect(branch_, node_delete_list);
    }

    std::size_t node_depth() const override
    {
        return expression_node<T>::ndb_t::compute_node_depth(branch_);
    }

  private:
    bool initialised_;
    branch_t branch_;
    str_base_ptr str_base_ptr_;
    irange_ptr str_range_ptr_;
    mutable range_t base_range_;
    mutable range_t range_;
    mutable std::string value_;
};
} // namespace math_expr::details::string_nodes

#endif
