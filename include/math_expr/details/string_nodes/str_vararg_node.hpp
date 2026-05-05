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

#ifndef MATH_EXPR_DETAILS_STR_VARARG_NODE_HPP
#define MATH_EXPR_DETAILS_STR_VARARG_NODE_HPP

#include "math_expr/details/string_nodes/stringvar_node.hpp"

namespace math_expr::details::string_nodes
{
template <typename T, typename VarArgFunction>
class str_vararg_node final : public expression_node<T>,
                              public string_base_node<T>,
                              public range_interface<T>
{
   public:
    using range_t = typename range_interface<T>::range_t;
    using range_ptr = range_t*;
    using irange_t = range_interface<T>;
    using irange_ptr = irange_t*;
    using expression_ptr = expression_node<T>*;
    using str_base_ptr = string_base_node<T>*;
    using branch_t = std::pair<expression_ptr, bool>;

    template <typename Allocator, template <typename, typename> class Sequence>
    explicit str_vararg_node(const Sequence<expression_ptr, Allocator>& arg_list)
        : initialised_(false), str_base_ptr_(nullptr), str_range_ptr_(nullptr)
    {
        construct_branch_pair(final_node_, const_cast<expression_ptr>(arg_list.back()));

        if (nullptr == final_node_.first)
            return;
        else if (!is_generally_string_node(final_node_.first))
            return;

        str_base_ptr_ = final_node_.first->as_string_base();

        if (nullptr == str_base_ptr_)
            return;

        str_range_ptr_ = final_node_.first->as_range_iface();

        if (nullptr == str_range_ptr_)
            return;

        if (arg_list.size() > 1)
        {
            const std::size_t arg_list_size = arg_list.size() - 1;

            arg_list_.resize(arg_list_size);

            for (std::size_t i = 0; i < arg_list_size; ++i)
            {
                if (arg_list[i] && arg_list[i]->valid())
                {
                    construct_branch_pair(arg_list_[i], arg_list[i]);
                }
                else
                {
                    arg_list_.clear();
                    return;
                }
            }

            initialised_ = true;
        }

        initialised_ &= str_base_ptr_ && str_range_ptr_;
        assert(valid());
    }

    inline T value() const override
    {
        if (!arg_list_.empty())
        {
            VarArgFunction::process(arg_list_);
        }

        final_node_.first->value();

        return std::numeric_limits<T>::quiet_NaN();
    }

    std::string str() const override
    {
        return str_base_ptr_->str();
    }

    core::char_cptr base() const override
    {
        return str_base_ptr_->base();
    }

    std::size_t size() const override
    {
        return str_base_ptr_->size();
    }

    range_t& range_ref() override
    {
        return str_range_ptr_->range_ref();
    }

    const range_t& range_ref() const override
    {
        return str_range_ptr_->range_ref();
    }

    inline typename expression_node<T>::node_type type() const override
    {
        return expression_node<T>::node_type::e_stringvararg;
    }

    string_base_node<T>* as_string_base() override
    {
        return this;
    }
    range_interface<T>* as_range_iface() override
    {
        return this;
    }

    inline bool valid() const override
    {
        return initialised_ && final_node_.first && final_node_.first->valid();
    }

    void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) override
    {
        expression_node<T>::ndb_t::collect(final_node_, node_delete_list);
        expression_node<T>::ndb_t::collect(arg_list_, node_delete_list);
    }

    std::size_t node_depth() const override
    {
        return std::max(expression_node<T>::ndb_t::compute_node_depth(final_node_),
                        expression_node<T>::ndb_t::compute_node_depth(arg_list_));
    }

   private:
    bool initialised_;
    branch_t final_node_;
    str_base_ptr str_base_ptr_;
    irange_ptr str_range_ptr_;
    std::vector<branch_t> arg_list_;
};
}  // namespace math_expr::details::string_nodes

#endif
