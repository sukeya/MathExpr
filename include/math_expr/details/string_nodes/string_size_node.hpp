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

#ifndef MATH_EXPR_DETAILS_STRING_SIZE_NODE_HPP
#define MATH_EXPR_DETAILS_STRING_SIZE_NODE_HPP

#include "math_expr/details/string_nodes/stringvar_node.hpp"

namespace math_expr::details::string_nodes
{
template <typename T>
class string_size_node final : public expression_node<T>
{
   public:
    using expression_ptr = expression_node<T>*;
    using str_base_ptr = string_base_node<T>*;
    using branch_t = std::pair<expression_ptr, bool>;

    explicit string_size_node(expression_ptr branch) : str_base_ptr_(nullptr)
    {
        construct_branch_pair(branch_, branch);

        if (is_generally_string_node(branch_.first))
        {
            str_base_ptr_ = branch_.first->as_string_base();
        }

        assert(valid());
    }

    inline T value() const override
    {
        branch_.first->value();
        return T(str_base_ptr_->size());
    }

    inline typename expression_node<T>::node_type type() const override
    {
        return expression_node<T>::node_type::e_stringsize;
    }

    inline bool valid() const override
    {
        return str_base_ptr_;
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
    branch_t branch_;
    str_base_ptr str_base_ptr_;
};
}  // namespace math_expr::details::string_nodes

#endif
