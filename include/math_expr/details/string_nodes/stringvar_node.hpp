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

#ifndef MATH_EXPR_DETAILS_STRINGVAR_NODE_HPP
#define MATH_EXPR_DETAILS_STRINGVAR_NODE_HPP

#include "math_expr/details/range_pack.hpp"

namespace math_expr::details::string_nodes
{
template <typename T>
class stringvar_node final : public expression_node<T>,
                             public string_base_node<T>,
                             public range_interface<T>
{
   public:
    using range_t = typename range_interface<T>::range_t;

    static std::string null_value;

    explicit stringvar_node() : value_(&null_value) {}

    explicit stringvar_node(std::string& v) : value_(&v)
    {
        rp_.n0_c = std::make_pair<bool, std::size_t>(true, 0);
        rp_.n1_c = std::make_pair<bool, std::size_t>(true, v.size());
        rp_.cache.first = rp_.n0_c.second;
        rp_.cache.second = rp_.n1_c.second;
    }

    inline bool operator<(const stringvar_node<T>& v) const
    {
        return this < (&v);
    }

    inline T value() const override
    {
        rp_.n1_c.second = (*value_).size();
        rp_.cache.second = rp_.n1_c.second;

        return std::numeric_limits<T>::quiet_NaN();
    }

    std::string str() const override
    {
        return ref();
    }

    core::char_cptr base() const override
    {
        return &(*value_)[0];
    }

    std::size_t size() const override
    {
        return ref().size();
    }

    std::string& ref()
    {
        return (*value_);
    }

    const std::string& ref() const
    {
        return (*value_);
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
        return expression_node<T>::node_type::e_stringvar;
    }

    void rebase(std::string& s)
    {
        value_ = &s;
        rp_.n0_c = std::make_pair<bool, std::size_t>(true, 0);
        rp_.n1_c = std::make_pair<bool, std::size_t>(true, value_->size() - 1);
        rp_.cache.first = rp_.n0_c.second;
        rp_.cache.second = rp_.n1_c.second;
    }

   private:
    std::string* value_;
    mutable range_t rp_;
};

template <typename T>
std::string stringvar_node<T>::null_value = std::string("");
}  // namespace math_expr::details::string_nodes

#endif
