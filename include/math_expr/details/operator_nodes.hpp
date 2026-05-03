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

#ifndef MATH_EXPR_DETAILS_OPERATOR_NODES_HPP
#define MATH_EXPR_DETAILS_OPERATOR_NODES_HPP

#include "math_expr/details/literal_nodes.hpp"

namespace math_expr::details
{
template <typename T>
class unary_node : public expression_node<T>
{
   public:
    using expression_ptr = expression_node<T>*;
    using branch_t = std::pair<expression_ptr, bool>;

    unary_node(const core::operators::operator_type& opr, expression_ptr branch) : operation_(opr)
    {
        construct_branch_pair(branch_, branch);
        assert(valid());
    }

    inline T value() const override
    {
        return core::operators::process<T>(operation_, branch_.first->value());
    }

    inline typename expression_node<T>::node_type type() const override
    {
        return expression_node<T>::node_type::e_unary;
    }

    inline core::operators::operator_type operation()
    {
        return operation_;
    }

    inline expression_node<T>* branch(const std::size_t&) const override
    {
        return branch_.first;
    }

    inline bool valid() const override
    {
        return branch_.first && branch_.first->valid();
    }

    inline void release()
    {
        branch_.second = false;
    }

    void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) override
    {
        expression_node<T>::ndb_t::collect(branch_, node_delete_list);
    }

    std::size_t node_depth() const final
    {
        return expression_node<T>::ndb_t::compute_node_depth(branch_);
    }

   private:
    core::operators::operator_type operation_;
    branch_t branch_;
};

template <typename T>
class binary_node : public expression_node<T>
{
   public:
    using expression_ptr = expression_node<T>*;
    using branch_t = std::pair<expression_ptr, bool>;

    binary_node(const core::operators::operator_type& opr, expression_ptr branch0,
                expression_ptr branch1)
        : operation_(opr)
    {
        init_branches<2>(branch_, branch0, branch1);
        assert(valid());
    }

    inline T value() const override
    {
        return core::operators::process<T>(operation_, branch_[0].first->value(),
                                           branch_[1].first->value());
    }

    inline typename expression_node<T>::node_type type() const override
    {
        return expression_node<T>::node_type::e_binary;
    }

    inline core::operators::operator_type operation()
    {
        return operation_;
    }

    inline expression_node<T>* branch(const std::size_t& index = 0) const override
    {
        assert(index < 2);
        return branch_[index].first;
    }

    inline bool valid() const override
    {
        return branch_[0].first && branch_[0].first->valid() && branch_[1].first &&
               branch_[1].first->valid();
    }

    void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) override
    {
        expression_node<T>::ndb_t::collect(branch_, node_delete_list);
    }

    std::size_t node_depth() const final
    {
        return expression_node<T>::ndb_t::template compute_node_depth<2>(branch_);
    }

   private:
    core::operators::operator_type operation_;
    branch_t branch_[2];
};

template <typename T, typename Operation>
class binary_ext_node final : public expression_node<T>
{
   public:
    using expression_ptr = expression_node<T>*;
    using branch_t = std::pair<expression_ptr, bool>;

    binary_ext_node(expression_ptr branch0, expression_ptr branch1)
    {
        init_branches<2>(branch_, branch0, branch1);
        assert(valid());
    }

    inline T value() const override
    {
        const T arg0 = branch_[0].first->value();
        const T arg1 = branch_[1].first->value();
        return Operation::process(arg0, arg1);
    }

    inline typename expression_node<T>::node_type type() const override
    {
        return expression_node<T>::node_type::e_binary_ext;
    }

    inline core::operators::operator_type operation()
    {
        return Operation::operation();
    }

    inline expression_node<T>* branch(const std::size_t& index = 0) const override
    {
        assert(index < 2);
        return branch_[index].first;
    }

    inline bool valid() const override
    {
        return branch_[0].first && branch_[0].first->valid() && branch_[1].first &&
               branch_[1].first->valid();
    }

    void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) override
    {
        expression_node<T>::ndb_t::collect(branch_, node_delete_list);
    }

    std::size_t node_depth() const override
    {
        return expression_node<T>::ndb_t::template compute_node_depth<2>(branch_);
    }

   protected:
    branch_t branch_[2];
};

template <typename T>
class trinary_node : public expression_node<T>
{
   public:
    using expression_ptr = expression_node<T>*;
    using branch_t = std::pair<expression_ptr, bool>;

    trinary_node(const core::operators::operator_type& opr, expression_ptr branch0,
                 expression_ptr branch1, expression_ptr branch2)
        : operation_(opr)
    {
        init_branches<3>(branch_, branch0, branch1, branch2);
        assert(valid());
    }

    inline T value() const override
    {
        const T arg0 = branch_[0].first->value();
        const T arg1 = branch_[1].first->value();
        const T arg2 = branch_[2].first->value();

        switch (operation_)
        {
            case core::operators::operator_type::inrange:
                return (arg1 < arg0)
                           ? core::numeric::false_v<T>
                           : ((arg1 > arg2) ? core::numeric::false_v<T> : core::numeric::true_v<T>);

            case core::operators::operator_type::clamp:
                return (arg1 < arg0) ? arg0 : (arg1 > arg2 ? arg2 : arg1);

            case core::operators::operator_type::iclamp:
                if ((arg1 <= arg0) || (arg1 >= arg2))
                    return arg1;
                else
                    return ((T(2) * arg1 <= (arg2 + arg0)) ? arg0 : arg2);

            default:
                MATH_EXPR_DEBUG(("trinary_node::value() - Error: Invalid operation\n"));
                return std::numeric_limits<T>::quiet_NaN();
        }
    }

    inline typename expression_node<T>::node_type type() const override
    {
        return expression_node<T>::node_type::e_trinary;
    }

    inline bool valid() const override
    {
        return branch_[0].first && branch_[0].first->valid() && branch_[1].first &&
               branch_[1].first->valid() && branch_[2].first && branch_[2].first->valid();
    }

    void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) override
    {
        expression_node<T>::ndb_t::collect(branch_, node_delete_list);
    }

    std::size_t node_depth() const override final
    {
        return expression_node<T>::ndb_t::template compute_node_depth<3>(branch_);
    }

   protected:
    core::operators::operator_type operation_;
    branch_t branch_[3];
};

template <typename T>
class quaternary_node : public expression_node<T>
{
   public:
    using expression_ptr = expression_node<T>*;
    using branch_t = std::pair<expression_ptr, bool>;

    quaternary_node(const core::operators::operator_type& opr, expression_ptr branch0,
                    expression_ptr branch1, expression_ptr branch2, expression_ptr branch3)
        : operation_(opr)
    {
        init_branches<4>(branch_, branch0, branch1, branch2, branch3);
    }

    inline T value() const override
    {
        return std::numeric_limits<T>::quiet_NaN();
    }

    inline typename expression_node<T>::node_type type() const override
    {
        return expression_node<T>::node_type::e_quaternary;
    }

    void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) override
    {
        expression_node<T>::ndb_t::collect(branch_, node_delete_list);
    }

    std::size_t node_depth() const override final
    {
        return expression_node<T>::ndb_t::template compute_node_depth<4>(branch_);
    }

    inline bool valid() const override
    {
        return branch_[0].first && branch_[0].first->valid() && branch_[1].first &&
               branch_[1].first->valid() && branch_[2].first && branch_[2].first->valid() &&
               branch_[3].first && branch_[3].first->valid();
    }

   protected:
    core::operators::operator_type operation_;
    branch_t branch_[4];
};

}  // namespace math_expr::details

#endif
