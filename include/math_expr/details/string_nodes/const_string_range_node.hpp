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

#ifndef MATH_EXPR_DETAILS_CONST_STRING_RANGE_NODE_HPP
#define MATH_EXPR_DETAILS_CONST_STRING_RANGE_NODE_HPP

#include "math_expr/details/string_nodes/string_range_node.hpp"

namespace math_expr::details::string_nodes
{
template <typename T>
class const_string_range_node final : public expression_node<T>,
                                      public string_base_node<T>,
                                      public range_interface<T>
{
   public:
    using range_t = typename range_interface<T>::range_t;

    explicit const_string_range_node(const std::string& v, const range_t& rp) : value_(v), rp_(rp)
    {
    }

    ~const_string_range_node() override
    {
        rp_.free();
    }

    inline T value() const override
    {
        return std::numeric_limits<T>::quiet_NaN();
    }

    std::string str() const override
    {
        return value_;
    }

    core::char_cptr base() const override
    {
        return value_.data();
    }

    std::size_t size() const override
    {
        return value_.size();
    }

    range_t range() const
    {
        return rp_;
    }

    range_t& range_ref() override
    {
        return rp_;
    }

    const range_t& range_ref() const override
    {
        return rp_;
    }

    inline typename expression_node<T>::node_type type() const override
    {
        return expression_node<T>::node_type::e_cstringvarrng;
    }

   private:
    const_string_range_node(const const_string_range_node<T>&) = delete;
    const_string_range_node<T>& operator=(const const_string_range_node<T>&) = delete;

    const std::string value_;
    range_t rp_;
};
}  // namespace math_expr::details::string_nodes

#endif
