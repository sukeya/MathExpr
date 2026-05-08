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

#ifndef MATH_EXPR_DETAILS_FUNCTION_NODES_HPP
#define MATH_EXPR_DETAILS_FUNCTION_NODES_HPP

#include "math_expr/ifunction.hpp"
#include "math_expr/ivararg_function.hpp"
#include "math_expr/igeneric_function.hpp"
#include "math_expr/details/operator_nodes.hpp"
#include "math_expr/details/vector_nodes.hpp"

namespace math_expr::details
{
template <typename T, typename SpecialFunction>
class sf3_var_node final : public expression_node<T>
{
   public:
    using expression_ptr = expression_node<T>*;

    sf3_var_node(const T& v0, const T& v1, const T& v2) : v0_(v0), v1_(v1), v2_(v2) {}

    inline T value() const override
    {
        return SpecialFunction::process(v0_, v1_, v2_);
    }

    inline typename expression_node<T>::node_type type() const override
    {
        return expression_node<T>::node_type::e_trinary;
    }

   private:
    sf3_var_node(const sf3_var_node<T, SpecialFunction>&) = delete;
    sf3_var_node<T, SpecialFunction>& operator=(const sf3_var_node<T, SpecialFunction>&) = delete;

    const T& v0_;
    const T& v1_;
    const T& v2_;
};

template <typename T, typename SpecialFunction>
class sf4_var_node final : public expression_node<T>
{
   public:
    using expression_ptr = expression_node<T>*;

    sf4_var_node(const T& v0, const T& v1, const T& v2, const T& v3)
        : v0_(v0), v1_(v1), v2_(v2), v3_(v3)
    {
    }

    inline T value() const override
    {
        return SpecialFunction::process(v0_, v1_, v2_, v3_);
    }

    inline typename expression_node<T>::node_type type() const override
    {
        return expression_node<T>::node_type::e_trinary;
    }

   private:
    sf4_var_node(const sf4_var_node<T, SpecialFunction>&) = delete;
    sf4_var_node<T, SpecialFunction>& operator=(const sf4_var_node<T, SpecialFunction>&) = delete;

    const T& v0_;
    const T& v1_;
    const T& v2_;
    const T& v3_;
};

template <typename T, typename VarArgFunction>
class vararg_node final : public expression_node<T>
{
   public:
    using expression_ptr = expression_node<T>*;
    using branch_t = std::pair<expression_ptr, bool>;

    template <typename Allocator, template <typename, typename> class Sequence>
    explicit vararg_node(const Sequence<expression_ptr, Allocator>& arg_list) : initialised_(false)
    {
        arg_list_.resize(arg_list.size());

        for (std::size_t i = 0; i < arg_list.size(); ++i)
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

        initialised_ = (arg_list_.size() == arg_list.size());
        assert(valid());
    }

    inline T value() const override
    {
        return VarArgFunction::process(arg_list_);
    }

    inline typename expression_node<T>::node_type type() const override
    {
        if constexpr (requires { requires VarArgFunction::is_multi; })
            return expression_node<T>::node_type::e_vararg_multi;
        return expression_node<T>::node_type::e_vararg;
    }

    inline bool valid() const override
    {
        return initialised_;
    }

    void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) override
    {
        expression_node<T>::ndb_t::collect(arg_list_, node_delete_list);
    }

    std::size_t node_depth() const override
    {
        return expression_node<T>::ndb_t::compute_node_depth(arg_list_);
    }

    std::size_t size() const
    {
        return arg_list_.size();
    }

    expression_ptr operator[](const std::size_t& index) const
    {
        return arg_list_[index].first;
    }

    inline expression_node<T>* branch(const std::size_t& index = 0) const override
    {
        return (index < arg_list_.size()) ? arg_list_[index].first : nullptr;
    }

    std::size_t arg_size() const override
    {
        return arg_list_.size();
    }

   private:
    std::vector<branch_t> arg_list_;
    bool initialised_;
};

template <typename T, typename VarArgFunction>
class vararg_varnode final : public expression_node<T>
{
   public:
    using expression_ptr = expression_node<T>*;

    template <typename Allocator, template <typename, typename> class Sequence>
    explicit vararg_varnode(const Sequence<expression_ptr, Allocator>& arg_list)
        : initialised_(false)
    {
        arg_list_.resize(arg_list.size());

        for (std::size_t i = 0; i < arg_list.size(); ++i)
        {
            if (arg_list[i] && arg_list[i]->valid() && is_variable_node(arg_list[i]))
            {
                variable_node<T>* var_node_ptr = static_cast<variable_node<T>*>(arg_list[i]);
                arg_list_[i] = (&var_node_ptr->ref());
            }
            else
            {
                arg_list_.clear();
                return;
            }
        }

        initialised_ = (arg_list.size() == arg_list_.size());
        assert(valid());
    }

    inline T value() const override
    {
        return VarArgFunction::process(arg_list_);
    }

    inline typename expression_node<T>::node_type type() const override
    {
        return expression_node<T>::node_type::e_vararg;
    }

    inline bool valid() const override
    {
        return initialised_;
    }

   private:
    std::vector<const T*> arg_list_;
    bool initialised_;
};

template <typename T, typename VecFunction>
class vectorize_node final : public expression_node<T>
{
   public:
    using expression_ptr = expression_node<T>*;
    using branch_t = std::pair<expression_ptr, bool>;

    explicit vectorize_node(const expression_ptr v) : ivec_ptr_(nullptr)
    {
        construct_branch_pair(v_, v);

        if (is_ivector_node(v_.first))
        {
            ivec_ptr_ = v_.first->as_vector_iface();
            assert(ivec_ptr_);
        }
    }

    inline T value() const override
    {
        v_.first->value();
        return VecFunction::process(ivec_ptr_);
    }

    inline typename expression_node<T>::node_type type() const override
    {
        return expression_node<T>::node_type::e_vecfunc;
    }

    inline bool valid() const override
    {
        return ivec_ptr_ && v_.first && v_.first->valid();
    }

    void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) override
    {
        expression_node<T>::ndb_t::collect(v_, node_delete_list);
    }

    std::size_t node_depth() const override
    {
        return expression_node<T>::ndb_t::compute_node_depth(v_);
    }

   private:
    vector_interface<T>* ivec_ptr_;
    branch_t v_;
};

template <typename T>
class assignment_node final : public binary_node<T>
{
   public:
    using expression_ptr = expression_node<T>*;
    using binary_node<T>::branch;

    assignment_node(const core::operators::operator_type& opr, expression_ptr branch0,
                    expression_ptr branch1)
        : binary_node<T>(opr, branch0, branch1), var_node_ptr_(nullptr)
    {
        if (is_variable_node(branch(0)))
        {
            var_node_ptr_ = static_cast<variable_node<T>*>(branch(0));
        }
    }

    inline T value() const override
    {
        T& result = var_node_ptr_->ref();
        result = branch(1)->value();

        return result;
    }

    inline bool valid() const override
    {
        return var_node_ptr_ && binary_node<T>::valid();
    }

   private:
    variable_node<T>* var_node_ptr_;
};

template <typename T>
class assignment_vec_elem_node final : public binary_node<T>
{
   public:
    using expression_ptr = expression_node<T>*;
    using binary_node<T>::branch;

    assignment_vec_elem_node(const core::operators::operator_type& opr, expression_ptr branch0,
                             expression_ptr branch1)
        : binary_node<T>(opr, branch0, branch1), vec_node_ptr_(nullptr)
    {
        if (is_vector_elem_node(branch(0)))
        {
            vec_node_ptr_ = static_cast<vector_elem_node<T>*>(branch(0));
        }

        assert(valid());
    }

    inline T value() const override
    {
        T& result = vec_node_ptr_->ref();
        result = branch(1)->value();

        return result;
    }

    inline bool valid() const override
    {
        return vec_node_ptr_ && binary_node<T>::valid();
    }

   private:
    vector_elem_node<T>* vec_node_ptr_;
};

template <typename T>
class assignment_vec_elem_rtc_node final : public binary_node<T>
{
   public:
    using expression_ptr = expression_node<T>*;
    using binary_node<T>::branch;

    assignment_vec_elem_rtc_node(const core::operators::operator_type& opr, expression_ptr branch0,
                                 expression_ptr branch1)
        : binary_node<T>(opr, branch0, branch1), vec_node_ptr_(nullptr)
    {
        if (is_vector_elem_rtc_node(branch(0)))
        {
            vec_node_ptr_ = static_cast<vector_elem_rtc_node<T>*>(branch(0));
        }

        assert(valid());
    }

    inline T value() const override
    {
        T& result = vec_node_ptr_->ref();
        result = branch(1)->value();

        return result;
    }

    inline bool valid() const override
    {
        return vec_node_ptr_ && binary_node<T>::valid();
    }

   private:
    vector_elem_rtc_node<T>* vec_node_ptr_;
};

template <typename T>
class assignment_rebasevec_elem_node final : public binary_node<T>
{
   public:
    using expression_ptr = expression_node<T>*;
    using expression_node<T>::branch;

    assignment_rebasevec_elem_node(const core::operators::operator_type& opr,
                                   expression_ptr branch0, expression_ptr branch1)
        : binary_node<T>(opr, branch0, branch1), rbvec_node_ptr_(nullptr)
    {
        if (is_rebasevector_elem_node(branch(0)))
        {
            rbvec_node_ptr_ = static_cast<rebasevector_elem_node<T>*>(branch(0));
        }

        assert(valid());
    }

    inline T value() const override
    {
        T& result = rbvec_node_ptr_->ref();
        result = branch(1)->value();

        return result;
    }

    inline bool valid() const override
    {
        return rbvec_node_ptr_ && binary_node<T>::valid();
    }

   private:
    rebasevector_elem_node<T>* rbvec_node_ptr_;
};

template <typename T>
class assignment_rebasevec_elem_rtc_node final : public binary_node<T>
{
   public:
    using expression_ptr = expression_node<T>*;
    using expression_node<T>::branch;

    assignment_rebasevec_elem_rtc_node(const core::operators::operator_type& opr,
                                       expression_ptr branch0, expression_ptr branch1)
        : binary_node<T>(opr, branch0, branch1), rbvec_node_ptr_(nullptr)
    {
        if (is_rebasevector_elem_rtc_node(branch(0)))
        {
            rbvec_node_ptr_ = static_cast<rebasevector_elem_rtc_node<T>*>(branch(0));
        }

        assert(valid());
    }

    inline T value() const override
    {
        T& result = rbvec_node_ptr_->ref();
        result = branch(1)->value();

        return result;
    }

    inline bool valid() const override
    {
        return rbvec_node_ptr_ && binary_node<T>::valid();
    }

   private:
    rebasevector_elem_rtc_node<T>* rbvec_node_ptr_;
};

template <typename T>
class assignment_rebasevec_celem_node final : public binary_node<T>
{
   public:
    using expression_ptr = expression_node<T>*;
    using binary_node<T>::branch;

    assignment_rebasevec_celem_node(const core::operators::operator_type& opr,
                                    expression_ptr branch0, expression_ptr branch1)
        : binary_node<T>(opr, branch0, branch1), rbvec_node_ptr_(nullptr)
    {
        if (is_rebasevector_celem_node(branch(0)))
        {
            rbvec_node_ptr_ = static_cast<rebasevector_celem_node<T>*>(branch(0));
        }

        assert(valid());
    }

    inline T value() const override
    {
        T& result = rbvec_node_ptr_->ref();
        result = branch(1)->value();

        return result;
    }

    inline bool valid() const override
    {
        return rbvec_node_ptr_ && binary_node<T>::valid();
    }

   private:
    rebasevector_celem_node<T>* rbvec_node_ptr_;
};

/**
 * @brief Assigns a scalar rhs value to every element of a vector lhs.
 *
 * The template parameter `T` is the expression's numeric value type. It is
 * propagated from the owning `math_expr::parser<T>` instance, so vector
 * elements, scalar rhs values, and the generated assignment node all share
 * the same `T`.
 *
 * @tparam T Numeric value type for the expression and vector elements.
 */
template <typename T>
class assignment_vec_node final : public binary_node<T>, public vector_interface<T>
{
   public:
    using expression_ptr = expression_node<T>*;
    using vector_node_ptr = vector_node<T>*;
    using vds_t = core::vec_data_store<T>;

    using binary_node<T>::branch;

    assignment_vec_node(const core::operators::operator_type& opr, expression_ptr branch0,
                        expression_ptr branch1)
        : binary_node<T>(opr, branch0, branch1), vec_node_ptr_(nullptr)
    {
        if (is_vector_node(branch(0)))
        {
            vec_node_ptr_ = static_cast<vector_node<T>*>(branch(0));
            vds() = vec_node_ptr_->vds();
        }

        assert(valid());
    }

    inline T value() const override
    {
        const T v = branch(1)->value();

        T* vec = vds().data();

        core::operators::loop_unroll lud(size());
        const T* upper_bound = vec + lud.upper_bound;

        while (vec < upper_bound)
        {
            lud.foreach_batch([v, &vec](unsigned int i) { vec[i] = v; });

            vec += lud.loop_batch_size;
        }

        lud.foreach_remainder([&vec, v]() { *vec++ = v; });

        return vec_node_ptr_->value();
    }

    vector_node_ptr vec() const override
    {
        return vec_node_ptr_;
    }

    vector_node_ptr vec() override
    {
        return vec_node_ptr_;
    }

    inline typename expression_node<T>::node_type type() const override
    {
        return expression_node<T>::node_type::e_vecvalass;
    }

    vector_interface<T>* as_vector_iface() override
    {
        return this;
    }

    inline bool valid() const override
    {
        return vec_node_ptr_ && (vds().size() <= vec_node_ptr_->vec_holder().base_size()) &&
               binary_node<T>::valid();
    }

    std::size_t size() const override
    {
        return vec_node_ptr_->vec_holder().size();
    }

    std::size_t base_size() const override
    {
        return vec_node_ptr_->vec_holder().base_size();
    }

    vds_t& vds() override
    {
        return vds_;
    }

    const vds_t& vds() const override
    {
        return vds_;
    }

   private:
    vector_node<T>* vec_node_ptr_;
    vds_t vds_;
};

template <typename T>
class assignment_vecvec_node final : public binary_node<T>, public vector_interface<T>
{
   public:
    using expression_ptr = expression_node<T>*;
    using vector_node_ptr = vector_node<T>*;
    using vds_t = core::vec_data_store<T>;

    using binary_node<T>::branch;

    assignment_vecvec_node(const core::operators::operator_type& opr, expression_ptr branch0,
                           expression_ptr branch1)
        : binary_node<T>(opr, branch0, branch1),
          vec0_node_ptr_(nullptr),
          vec1_node_ptr_(nullptr),
          initialised_(false),
          src_is_ivec_(false)
    {
        if (is_vector_node(branch(0)))
        {
            vec0_node_ptr_ = static_cast<vector_node<T>*>(branch(0));
            vds() = vec0_node_ptr_->vds();
        }

        if (is_vector_node(branch(1)))
        {
            vec1_node_ptr_ = static_cast<vector_node<T>*>(branch(1));
            vds_t::match_sizes(vds(), vec1_node_ptr_->vds());
        }
        else if (is_ivector_node(branch(1)))
        {
            vector_interface<T>* vi = nullptr;

            if (nullptr != (vi = branch(1)->as_vector_iface()))
            {
                vec1_node_ptr_ = vi->vec();

                if (!vi->side_effect())
                {
                    vi->vds() = vds();
                    src_is_ivec_ = true;
                }
                else
                    vds_t::match_sizes(vds(), vi->vds());
            }
        }

        initialised_ = vec0_node_ptr_ && vec1_node_ptr_ && (size() <= base_size()) &&
                       (vds_.size() <= base_size()) && binary_node<T>::valid();

        assert(valid());
    }

    inline T value() const override
    {
        branch(1)->value();

        if (src_is_ivec_)
            return vec0_node_ptr_->value();

        T* vec0 = vec0_node_ptr_->vds().data();
        T* vec1 = vec1_node_ptr_->vds().data();

        core::operators::loop_unroll lud(size());
        const T* upper_bound = vec0 + lud.upper_bound;

        while (vec0 < upper_bound)
        {
            lud.foreach_batch([&vec0, &vec1](unsigned int i) { vec0[i] = vec1[i]; });

            vec0 += lud.loop_batch_size;
            vec1 += lud.loop_batch_size;
        }

        lud.foreach_remainder([&vec0, &vec1]() { *vec0++ = *vec1++; });

        return vec0_node_ptr_->value();
    }

    vector_node_ptr vec() override
    {
        return vec0_node_ptr_;
    }

    vector_node_ptr vec() const override
    {
        return vec0_node_ptr_;
    }

    inline typename expression_node<T>::node_type type() const override
    {
        return expression_node<T>::node_type::e_vecvecass;
    }

    vector_interface<T>* as_vector_iface() override
    {
        return this;
    }

    inline bool valid() const override
    {
        return initialised_;
    }

    std::size_t size() const override
    {
        return std::min(vec0_node_ptr_->vec_holder().size(), vec1_node_ptr_->vec_holder().size());
    }

    std::size_t base_size() const override
    {
        return std::min(vec0_node_ptr_->vec_holder().base_size(),
                        vec1_node_ptr_->vec_holder().base_size());
    }

    vds_t& vds() override
    {
        return vds_;
    }

    const vds_t& vds() const override
    {
        return vds_;
    }

   private:
    vector_node<T>* vec0_node_ptr_;
    vector_node<T>* vec1_node_ptr_;
    bool initialised_;
    bool src_is_ivec_;
    vds_t vds_;
};

template <typename T, typename Operation>
class assignment_op_node final : public binary_node<T>
{
   public:
    using expression_ptr = expression_node<T>*;
    using binary_node<T>::branch;

    assignment_op_node(const core::operators::operator_type& opr, expression_ptr branch0,
                       expression_ptr branch1)
        : binary_node<T>(opr, branch0, branch1), var_node_ptr_(nullptr)
    {
        if (is_variable_node(branch(0)))
        {
            var_node_ptr_ = static_cast<variable_node<T>*>(branch(0));
        }

        assert(valid());
    }

    inline T value() const override
    {
        T& v = var_node_ptr_->ref();
        v = Operation::process(v, branch(1)->value());

        return v;
    }

    inline bool valid() const override
    {
        return var_node_ptr_ && binary_node<T>::valid();
    }

   private:
    variable_node<T>* var_node_ptr_;
};

template <typename T, typename Operation>
class assignment_vec_elem_op_node final : public binary_node<T>
{
   public:
    using expression_ptr = expression_node<T>*;
    using binary_node<T>::branch;

    assignment_vec_elem_op_node(const core::operators::operator_type& opr, expression_ptr branch0,
                                expression_ptr branch1)
        : binary_node<T>(opr, branch0, branch1), vec_node_ptr_(nullptr)
    {
        if (is_vector_elem_node(branch(0)))
        {
            vec_node_ptr_ = static_cast<vector_elem_node<T>*>(branch(0));
        }

        assert(valid());
    }

    inline T value() const override
    {
        T& v = vec_node_ptr_->ref();
        v = Operation::process(v, branch(1)->value());

        return v;
    }

    inline bool valid() const override
    {
        return vec_node_ptr_ && binary_node<T>::valid();
    }

   private:
    vector_elem_node<T>* vec_node_ptr_;
};

template <typename T, typename Operation>
class assignment_vec_elem_op_rtc_node final : public binary_node<T>
{
   public:
    using expression_ptr = expression_node<T>*;
    using binary_node<T>::branch;

    assignment_vec_elem_op_rtc_node(const core::operators::operator_type& opr,
                                    expression_ptr branch0, expression_ptr branch1)
        : binary_node<T>(opr, branch0, branch1), vec_node_ptr_(nullptr)
    {
        if (is_vector_elem_rtc_node(branch(0)))
        {
            vec_node_ptr_ = static_cast<vector_elem_rtc_node<T>*>(branch(0));
        }

        assert(valid());
    }

    inline T value() const override
    {
        T& v = vec_node_ptr_->ref();
        v = Operation::process(v, branch(1)->value());

        return v;
    }

    inline bool valid() const override
    {
        return vec_node_ptr_ && binary_node<T>::valid();
    }

   private:
    vector_elem_rtc_node<T>* vec_node_ptr_;
};

template <typename T, typename Operation>
class assignment_vec_celem_op_rtc_node final : public binary_node<T>
{
   public:
    using expression_ptr = expression_node<T>*;
    using binary_node<T>::branch;

    assignment_vec_celem_op_rtc_node(const core::operators::operator_type& opr,
                                     expression_ptr branch0, expression_ptr branch1)
        : binary_node<T>(opr, branch0, branch1), vec_node_ptr_(nullptr)
    {
        if (is_vector_celem_rtc_node(branch(0)))
        {
            vec_node_ptr_ = static_cast<vector_celem_rtc_node<T>*>(branch(0));
        }

        assert(valid());
    }

    inline T value() const override
    {
        T& v = vec_node_ptr_->ref();
        v = Operation::process(v, branch(1)->value());

        return v;
    }

    inline bool valid() const override
    {
        return vec_node_ptr_ && binary_node<T>::valid();
    }

   private:
    vector_celem_rtc_node<T>* vec_node_ptr_;
};

template <typename T, typename Operation>
class assignment_rebasevec_elem_op_node final : public binary_node<T>
{
   public:
    using expression_ptr = expression_node<T>*;
    using binary_node<T>::branch;

    assignment_rebasevec_elem_op_node(const core::operators::operator_type& opr,
                                      expression_ptr branch0, expression_ptr branch1)
        : binary_node<T>(opr, branch0, branch1), rbvec_node_ptr_(nullptr)
    {
        if (is_rebasevector_elem_node(branch(0)))
        {
            rbvec_node_ptr_ = static_cast<rebasevector_elem_node<T>*>(branch(0));
        }

        assert(valid());
    }

    inline T value() const override
    {
        T& v = rbvec_node_ptr_->ref();
        v = Operation::process(v, branch(1)->value());

        return v;
    }

    inline bool valid() const override
    {
        return rbvec_node_ptr_ && binary_node<T>::valid();
    }

   private:
    rebasevector_elem_node<T>* rbvec_node_ptr_;
};

template <typename T, typename Operation>
class assignment_rebasevec_celem_op_node final : public binary_node<T>
{
   public:
    using expression_ptr = expression_node<T>*;
    using binary_node<T>::branch;

    assignment_rebasevec_celem_op_node(const core::operators::operator_type& opr,
                                       expression_ptr branch0, expression_ptr branch1)
        : binary_node<T>(opr, branch0, branch1), rbvec_node_ptr_(nullptr)
    {
        if (is_rebasevector_celem_node(branch(0)))
        {
            rbvec_node_ptr_ = static_cast<rebasevector_celem_node<T>*>(branch(0));
        }

        assert(valid());
    }

    inline T value() const override
    {
        T& v = rbvec_node_ptr_->ref();
        v = Operation::process(v, branch(1)->value());

        return v;
    }

    inline bool valid() const override
    {
        return rbvec_node_ptr_ && binary_node<T>::valid();
    }

   private:
    rebasevector_celem_node<T>* rbvec_node_ptr_;
};

template <typename T, typename Operation>
class assignment_rebasevec_elem_op_rtc_node final : public binary_node<T>
{
   public:
    using expression_ptr = expression_node<T>*;
    using binary_node<T>::branch;

    assignment_rebasevec_elem_op_rtc_node(const core::operators::operator_type& opr,
                                          expression_ptr branch0, expression_ptr branch1)
        : binary_node<T>(opr, branch0, branch1), rbvec_node_ptr_(nullptr)
    {
        if (is_rebasevector_elem_rtc_node(branch(0)))
        {
            rbvec_node_ptr_ = static_cast<rebasevector_elem_rtc_node<T>*>(branch(0));
        }

        assert(valid());
    }

    inline T value() const override
    {
        T& v = rbvec_node_ptr_->ref();
        v = Operation::process(v, branch(1)->value());

        return v;
    }

    inline bool valid() const override
    {
        return rbvec_node_ptr_ && binary_node<T>::valid();
    }

   private:
    rebasevector_elem_rtc_node<T>* rbvec_node_ptr_;
};

template <typename T, typename Operation>
class assignment_rebasevec_celem_op_rtc_node final : public binary_node<T>
{
   public:
    using expression_ptr = expression_node<T>*;
    using binary_node<T>::branch;

    assignment_rebasevec_celem_op_rtc_node(const core::operators::operator_type& opr,
                                           expression_ptr branch0, expression_ptr branch1)
        : binary_node<T>(opr, branch0, branch1), rbvec_node_ptr_(nullptr)
    {
        if (is_rebasevector_celem_rtc_node(branch(0)))
        {
            rbvec_node_ptr_ = static_cast<rebasevector_celem_rtc_node<T>*>(branch(0));
        }

        assert(valid());
    }

    inline T value() const override
    {
        T& v = rbvec_node_ptr_->ref();
        v = Operation::process(v, branch(1)->value());

        return v;
    }

    inline bool valid() const override
    {
        return rbvec_node_ptr_ && binary_node<T>::valid();
    }

   private:
    rebasevector_celem_rtc_node<T>* rbvec_node_ptr_;
};

/**
 * @brief Applies a compound assignment between a vector lhs and a scalar rhs.
 *
 * The template parameter `T` is the expression's numeric value type. It is
 * propagated from the owning `math_expr::parser<T>` instance, so vector
 * elements, scalar rhs values, and the generated assignment node all share
 * the same `T`.
 *
 * @tparam T Numeric value type for the expression and vector elements.
 * @tparam Operation Compound-assignment operation policy such as add/sub/mul.
 */
template <typename T, typename Operation>
class assignment_vec_op_node final : public binary_node<T>, public vector_interface<T>
{
   public:
    using expression_ptr = expression_node<T>*;
    using vector_node_ptr = vector_node<T>*;
    using vds_t = core::vec_data_store<T>;

    using binary_node<T>::branch;

    assignment_vec_op_node(const core::operators::operator_type& opr, expression_ptr branch0,
                           expression_ptr branch1)
        : binary_node<T>(opr, branch0, branch1), vec_node_ptr_(nullptr)
    {
        if (is_vector_node(branch(0)))
        {
            vec_node_ptr_ = static_cast<vector_node<T>*>(branch(0));
            vds() = vec_node_ptr_->vds();
        }

        assert(valid());
    }

    inline T value() const override
    {
        const T v = branch(1)->value();

        T* vec = vds().data();

        core::operators::loop_unroll lud(size());
        const T* upper_bound = vec + lud.upper_bound;

        while (vec < upper_bound)
        {
            lud.foreach_batch([&vec, v](unsigned int i) { Operation::assign(vec[i], v); });

            vec += lud.loop_batch_size;
        }

        lud.foreach_remainder([&vec, v]() { Operation::assign(*vec++, v); });

        return vec_node_ptr_->value();
    }

    vector_node_ptr vec() const override
    {
        return vec_node_ptr_;
    }

    vector_node_ptr vec() override
    {
        return vec_node_ptr_;
    }

    inline typename expression_node<T>::node_type type() const override
    {
        return expression_node<T>::node_type::e_vecopvalass;
    }

    vector_interface<T>* as_vector_iface() override
    {
        return this;
    }

    inline bool valid() const override
    {
        return vec_node_ptr_ && (size() <= base_size()) && binary_node<T>::valid();
    }

    std::size_t size() const override
    {
        return vec_node_ptr_->vec_holder().size();
    }

    std::size_t base_size() const override
    {
        return vec_node_ptr_->vec_holder().base_size();
    }

    vds_t& vds() override
    {
        return vds_;
    }

    const vds_t& vds() const override
    {
        return vds_;
    }

    bool side_effect() const override
    {
        return true;
    }

   private:
    vector_node<T>* vec_node_ptr_;
    vds_t vds_;
};

template <typename T, typename Operation>
class assignment_vecvec_op_node final : public binary_node<T>, public vector_interface<T>
{
   public:
    using expression_ptr = expression_node<T>*;
    using vector_node_ptr = vector_node<T>*;
    using vds_t = core::vec_data_store<T>;

    using binary_node<T>::branch;

    assignment_vecvec_op_node(const core::operators::operator_type& opr, expression_ptr branch0,
                              expression_ptr branch1)
        : binary_node<T>(opr, branch0, branch1),
          vec0_node_ptr_(nullptr),
          vec1_node_ptr_(nullptr),
          initialised_(false)
    {
        if (is_vector_node(branch(0)))
        {
            vec0_node_ptr_ = static_cast<vector_node<T>*>(branch(0));
            vds() = vec0_node_ptr_->vds();
        }

        if (is_vector_node(branch(1)))
        {
            vec1_node_ptr_ = static_cast<vector_node<T>*>(branch(1));
            vec1_node_ptr_->vds() = vds();
        }
        else if (is_ivector_node(branch(1)))
        {
            vector_interface<T>* vi = nullptr;

            if (nullptr != (vi = branch(1)->as_vector_iface()))
            {
                vec1_node_ptr_ = vi->vec();
                vec1_node_ptr_->vds() = vi->vds();
            }
            else
                vds_t::match_sizes(vds(), vec1_node_ptr_->vds());
        }

        initialised_ =
            vec0_node_ptr_ && vec1_node_ptr_ && (size() <= base_size()) && binary_node<T>::valid();

        assert(valid());
    }

    inline T value() const override
    {
        branch(0)->value();
        branch(1)->value();

        T* vec0 = vec0_node_ptr_->vds().data();
        const T* vec1 = vec1_node_ptr_->vds().data();

        core::operators::loop_unroll lud(size());
        const T* upper_bound = vec0 + lud.upper_bound;

        while (vec0 < upper_bound)
        {
            lud.foreach_batch([&vec0, &vec1](unsigned int i)
                              { vec0[i] = Operation::process(vec0[i], vec1[i]); });

            vec0 += lud.loop_batch_size;
            vec1 += lud.loop_batch_size;
        }

        int i = 0;

        lud.foreach_remainder(
            [&i, vec0, vec1]()
            {
                vec0[i] = Operation::process(vec0[i], vec1[i]);
                ++i;
            });

        return vec0_node_ptr_->value();
    }

    vector_node_ptr vec() const override
    {
        return vec0_node_ptr_;
    }

    vector_node_ptr vec() override
    {
        return vec0_node_ptr_;
    }

    inline typename expression_node<T>::node_type type() const override
    {
        return expression_node<T>::node_type::e_vecopvecass;
    }

    vector_interface<T>* as_vector_iface() override
    {
        return this;
    }

    inline bool valid() const override
    {
        return initialised_;
    }

    std::size_t size() const override
    {
        return std::min(vec0_node_ptr_->vec_holder().size(), vec1_node_ptr_->vec_holder().size());
    }

    std::size_t base_size() const override
    {
        return std::min(vec0_node_ptr_->vec_holder().base_size(),
                        vec1_node_ptr_->vec_holder().base_size());
    }

    vds_t& vds() override
    {
        return vds_;
    }

    const vds_t& vds() const override
    {
        return vds_;
    }

    bool side_effect() const override
    {
        return true;
    }

   private:
    vector_node<T>* vec0_node_ptr_;
    vector_node<T>* vec1_node_ptr_;
    bool initialised_;
    vds_t vds_;
};

template <typename T>
struct memory_context_t
{
    using vector_node_ptr = vector_node<T>*;
    using vector_holder_t = vector_holder<T>;
    using vector_holder_ptr = vector_holder_t*;

    memory_context_t() : temp_(nullptr), temp_vec_node_(nullptr) {}

    void clear()
    {
        delete temp_vec_node_;
        delete temp_;
    }

    vector_holder_ptr temp_;
    vector_node_ptr temp_vec_node_;
};

template <typename T>
inline memory_context_t<T> make_memory_context(vector_holder<T>& vec_holder,
                                               core::vec_data_store<T>& vds)
{
    memory_context_t<T> result_ctxt;

    result_ctxt.temp_ = (vec_holder.rebaseable()) ? new vector_holder<T>(vec_holder, vds)
                                                  : new vector_holder<T>(vds);

    result_ctxt.temp_vec_node_ = new vector_node<T>(vds, result_ctxt.temp_);

    return result_ctxt;
}

template <typename T>
inline memory_context_t<T> make_memory_context(vector_holder<T>& vec_holder0,
                                               vector_holder<T>& vec_holder1,
                                               core::vec_data_store<T>& vds)
{
    memory_context_t<T> result_ctxt;

    if (!vec_holder0.rebaseable() && !vec_holder1.rebaseable())
        result_ctxt.temp_ = new vector_holder<T>(vds);
    else if (vec_holder0.rebaseable() && !vec_holder1.rebaseable())
        result_ctxt.temp_ = new vector_holder<T>(vec_holder0, vds);
    else if (!vec_holder0.rebaseable() && vec_holder1.rebaseable())
        result_ctxt.temp_ = new vector_holder<T>(vec_holder1, vds);
    else
    {
        result_ctxt.temp_ = (vec_holder0.base_size() >= vec_holder1.base_size())
                                ? new vector_holder<T>(vec_holder0, vds)
                                : new vector_holder<T>(vec_holder1, vds);
    }

    result_ctxt.temp_vec_node_ = new vector_node<T>(vds, result_ctxt.temp_);

    return result_ctxt;
}

template <typename T, typename Operation>
class vec_binop_vecvec_node final : public binary_node<T>, public vector_interface<T>
{
   public:
    using expression_ptr = expression_node<T>*;
    using vector_node_ptr = vector_node<T>*;
    using vector_holder_t = vector_holder<T>;
    using vector_holder_ptr = vector_holder_t*;
    using vds_t = core::vec_data_store<T>;
    using memory_context = memory_context_t<T>;

    using binary_node<T>::branch;

    vec_binop_vecvec_node(const core::operators::operator_type& opr, expression_ptr branch0,
                          expression_ptr branch1)
        : binary_node<T>(opr, branch0, branch1),
          vec0_node_ptr_(nullptr),
          vec1_node_ptr_(nullptr),
          initialised_(false)
    {
        bool v0_is_ivec = false;
        bool v1_is_ivec = false;

        if (is_vector_node(branch(0)))
        {
            vec0_node_ptr_ = static_cast<vector_node_ptr>(branch(0));
        }
        else if (is_ivector_node(branch(0)))
        {
            vector_interface<T>* vi = nullptr;

            if (nullptr != (vi = branch(0)->as_vector_iface()))
            {
                vec0_node_ptr_ = vi->vec();
                v0_is_ivec = true;
            }
        }

        if (is_vector_node(branch(1)))
        {
            vec1_node_ptr_ = static_cast<vector_node_ptr>(branch(1));
        }
        else if (is_ivector_node(branch(1)))
        {
            vector_interface<T>* vi = nullptr;

            if (nullptr != (vi = branch(1)->as_vector_iface()))
            {
                vec1_node_ptr_ = vi->vec();
                v1_is_ivec = true;
            }
        }

        if (vec0_node_ptr_ && vec1_node_ptr_)
        {
            vector_holder<T>& vec0 = vec0_node_ptr_->vec_holder();
            vector_holder<T>& vec1 = vec1_node_ptr_->vec_holder();

            if (v0_is_ivec && (vec0.base_size() <= vec1.base_size()))
            {
                vds_ = vds_t(vec0_node_ptr_->vds());
            }
            else if (v1_is_ivec && (vec1.base_size() <= vec0.base_size()))
            {
                vds_ = vds_t(vec1_node_ptr_->vds());
            }
            else
            {
                vds_ = vds_t(std::min(vec0.base_size(), vec1.base_size()));
            }

            memory_context_ = make_memory_context(vec0, vec1, vds());

            initialised_ = (size() <= base_size()) && binary_node<T>::valid();
        }

        assert(valid());
    }

    ~vec_binop_vecvec_node()
    {
        memory_context_.clear();
    }

    inline T value() const override
    {
        branch(0)->value();
        branch(1)->value();

        const T* vec0 = vec0_node_ptr_->vds().data();
        const T* vec1 = vec1_node_ptr_->vds().data();
        T* vec2 = vds().data();

        core::operators::loop_unroll lud(size());
        const T* upper_bound = vec2 + lud.upper_bound;

        while (vec2 < upper_bound)
        {
            lud.foreach_batch([&vec0, &vec1, &vec2](unsigned int i)
                              { vec2[i] = Operation::process(vec0[i], vec1[i]); });

            vec0 += lud.loop_batch_size;
            vec1 += lud.loop_batch_size;
            vec2 += lud.loop_batch_size;
        }

        int i = 0;

        lud.foreach_remainder(
            [&i, vec0, vec1, vec2]()
            {
                vec2[i] = Operation::process(vec0[i], vec1[i]);
                ++i;
            });

        return (vds().data())[0];
    }

    vector_node_ptr vec() const override
    {
        return memory_context_.temp_vec_node_;
    }

    vector_node_ptr vec() override
    {
        return memory_context_.temp_vec_node_;
    }

    inline typename expression_node<T>::node_type type() const override
    {
        return expression_node<T>::node_type::e_vecvecarith;
    }

    vector_interface<T>* as_vector_iface() override
    {
        return this;
    }

    inline bool valid() const override
    {
        return initialised_;
    }

    std::size_t size() const override
    {
        return std::min(vec0_node_ptr_->vec_holder().size(), vec1_node_ptr_->vec_holder().size());
    }

    std::size_t base_size() const override
    {
        return std::min(vec0_node_ptr_->vec_holder().base_size(),
                        vec1_node_ptr_->vec_holder().base_size());
    }

    vds_t& vds() override
    {
        return vds_;
    }

    const vds_t& vds() const override
    {
        return vds_;
    }

   private:
    vector_node_ptr vec0_node_ptr_;
    vector_node_ptr vec1_node_ptr_;
    bool initialised_;
    vds_t vds_;
    memory_context memory_context_;
};

template <typename T, typename Operation>
class vec_binop_vecval_node final : public binary_node<T>, public vector_interface<T>
{
   public:
    using expression_ptr = expression_node<T>*;
    using vector_node_ptr = vector_node<T>*;
    using vector_holder_t = vector_holder<T>;
    using vector_holder_ptr = vector_holder_t*;
    using vds_t = core::vec_data_store<T>;
    using memory_context = memory_context_t<T>;

    using binary_node<T>::branch;

    vec_binop_vecval_node(const core::operators::operator_type& opr, expression_ptr branch0,
                          expression_ptr branch1)
        : binary_node<T>(opr, branch0, branch1), vec0_node_ptr_(nullptr)
    {
        bool v0_is_ivec = false;

        if (is_vector_node(branch(0)))
        {
            vec0_node_ptr_ = static_cast<vector_node_ptr>(branch(0));
        }
        else if (is_ivector_node(branch(0)))
        {
            vector_interface<T>* vi = nullptr;

            if (nullptr != (vi = branch(0)->as_vector_iface()))
            {
                vec0_node_ptr_ = vi->vec();
                v0_is_ivec = true;
            }
        }

        if (vec0_node_ptr_)
        {
            if (v0_is_ivec)
                vds() = vec0_node_ptr_->vds();
            else
                vds() = vds_t(vec0_node_ptr_->base_size());

            memory_context_ = make_memory_context(vec0_node_ptr_->vec_holder(), vds());
        }

        assert(valid());
    }

    ~vec_binop_vecval_node()
    {
        memory_context_.clear();
    }

    inline T value() const override
    {
        branch(0)->value();
        const T v = branch(1)->value();

        const T* vec0 = vec0_node_ptr_->vds().data();
        T* vec1 = vds().data();

        core::operators::loop_unroll lud(size());
        const T* upper_bound = vec0 + lud.upper_bound;

        while (vec0 < upper_bound)
        {
            lud.foreach_batch([&vec0, &vec1, v](unsigned int i)
                              { vec1[i] = Operation::process(vec0[i], v); });

            vec0 += lud.loop_batch_size;
            vec1 += lud.loop_batch_size;
        }

        int i = 0;

        lud.foreach_remainder(
            [&i, v, vec0, vec1]()
            {
                vec1[i] = Operation::process(vec0[i], v);
                ++i;
            });

        return (vds().data())[0];
    }

    vector_node_ptr vec() const override
    {
        return memory_context_.temp_vec_node_;
    }

    vector_node_ptr vec() override
    {
        return memory_context_.temp_vec_node_;
    }

    inline typename expression_node<T>::node_type type() const override
    {
        return expression_node<T>::node_type::e_vecvalarith;
    }

    vector_interface<T>* as_vector_iface() override
    {
        return this;
    }

    inline bool valid() const override
    {
        return vec0_node_ptr_ && (size() <= base_size()) && binary_node<T>::valid();
    }

    std::size_t size() const override
    {
        return vec0_node_ptr_->size();
    }

    std::size_t base_size() const override
    {
        return vec0_node_ptr_->vec_holder().base_size();
    }

    vds_t& vds() override
    {
        return vds_;
    }

    const vds_t& vds() const override
    {
        return vds_;
    }

   private:
    vector_node_ptr vec0_node_ptr_;
    vds_t vds_;
    memory_context memory_context_;
};

template <typename T, typename Operation>
class vec_binop_valvec_node final : public binary_node<T>, public vector_interface<T>
{
   public:
    using expression_ptr = expression_node<T>*;
    using vector_node_ptr = vector_node<T>*;
    using vector_holder_t = vector_holder<T>;
    using vector_holder_ptr = vector_holder_t*;
    using vds_t = core::vec_data_store<T>;
    using memory_context = memory_context_t<T>;

    using binary_node<T>::branch;

    vec_binop_valvec_node(const core::operators::operator_type& opr, expression_ptr branch0,
                          expression_ptr branch1)
        : binary_node<T>(opr, branch0, branch1), vec1_node_ptr_(nullptr)
    {
        bool v1_is_ivec = false;

        if (is_vector_node(branch(1)))
        {
            vec1_node_ptr_ = static_cast<vector_node_ptr>(branch(1));
        }
        else if (is_ivector_node(branch(1)))
        {
            vector_interface<T>* vi = nullptr;

            if (nullptr != (vi = branch(1)->as_vector_iface()))
            {
                vec1_node_ptr_ = vi->vec();
                v1_is_ivec = true;
            }
        }

        if (vec1_node_ptr_)
        {
            if (v1_is_ivec)
                vds() = vec1_node_ptr_->vds();
            else
                vds() = vds_t(vec1_node_ptr_->base_size());

            memory_context_ = make_memory_context(vec1_node_ptr_->vec_holder(), vds());
        }

        assert(valid());
    }

    ~vec_binop_valvec_node()
    {
        memory_context_.clear();
    }

    inline T value() const override
    {
        const T v = branch(0)->value();
        branch(1)->value();

        T* vec0 = vds().data();
        const T* vec1 = vec1_node_ptr_->vds().data();

        core::operators::loop_unroll lud(size());
        const T* upper_bound = vec0 + lud.upper_bound;

        while (vec0 < upper_bound)
        {
            lud.foreach_batch([&vec0, &vec1, v](unsigned int i)
                              { vec0[i] = Operation::process(v, vec1[i]); });

            vec0 += lud.loop_batch_size;
            vec1 += lud.loop_batch_size;
        }

        int i = 0;

        lud.foreach_remainder(
            [&i, v, vec0, vec1]()
            {
                vec0[i] = Operation::process(v, vec1[i]);
                ++i;
            });

        return (vds().data())[0];
    }

    vector_node_ptr vec() const override
    {
        return memory_context_.temp_vec_node_;
    }

    vector_node_ptr vec() override
    {
        return memory_context_.temp_vec_node_;
    }

    inline typename expression_node<T>::node_type type() const override
    {
        return expression_node<T>::node_type::e_vecvalarith;
    }

    vector_interface<T>* as_vector_iface() override
    {
        return this;
    }

    inline bool valid() const override
    {
        return vec1_node_ptr_ && (size() <= base_size()) && (vds_.size() <= base_size()) &&
               binary_node<T>::valid();
    }

    std::size_t size() const override
    {
        return vec1_node_ptr_->vec_holder().size();
    }

    std::size_t base_size() const override
    {
        return vec1_node_ptr_->vec_holder().base_size();
    }

    vds_t& vds() override
    {
        return vds_;
    }

    const vds_t& vds() const override
    {
        return vds_;
    }

   private:
    vector_node_ptr vec1_node_ptr_;
    vds_t vds_;
    memory_context memory_context_;
};

template <typename T, typename Operation>
class unary_vector_node final : public unary_node<T>, public vector_interface<T>
{
   public:
    using expression_ptr = expression_node<T>*;
    using vector_node_ptr = vector_node<T>*;
    using vector_holder_t = vector_holder<T>;
    using vector_holder_ptr = vector_holder_t*;
    using vds_t = core::vec_data_store<T>;
    using memory_context = memory_context_t<T>;

    using expression_node<T>::branch;

    unary_vector_node(const core::operators::operator_type& opr, expression_ptr branch0)
        : unary_node<T>(opr, branch0), vec0_node_ptr_(nullptr)
    {
        bool vec0_is_ivec = false;

        if (is_vector_node(branch(0)))
        {
            vec0_node_ptr_ = static_cast<vector_node_ptr>(branch(0));
        }
        else if (is_ivector_node(branch(0)))
        {
            vector_interface<T>* vi = nullptr;

            if (nullptr != (vi = branch(0)->as_vector_iface()))
            {
                vec0_node_ptr_ = vi->vec();
                vec0_is_ivec = true;
            }
        }

        if (vec0_node_ptr_)
        {
            if (vec0_is_ivec)
                vds_ = vec0_node_ptr_->vds();
            else
                vds_ = vds_t(vec0_node_ptr_->base_size());

            memory_context_ = make_memory_context(vec0_node_ptr_->vec_holder(), vds());
        }

        assert(valid());
    }

    ~unary_vector_node()
    {
        memory_context_.clear();
    }

    inline T value() const override
    {
        branch()->value();

        const T* vec0 = vec0_node_ptr_->vds().data();
        T* vec1 = vds().data();

        core::operators::loop_unroll lud(size());
        const T* upper_bound = vec0 + lud.upper_bound;

        while (vec0 < upper_bound)
        {
            lud.foreach_batch([&vec0, &vec1](unsigned int i)
                              { vec1[i] = Operation::process(vec0[i]); });

            vec0 += lud.loop_batch_size;
            vec1 += lud.loop_batch_size;
        }

        int i = 0;
        lud.foreach_remainder(
            [&i, vec0, vec1]()
            {
                vec1[i] = Operation::process(vec0[i]);
                ++i;
            });

        return (vds().data())[0];
    }

    vector_node_ptr vec() const override
    {
        return memory_context_.temp_vec_node_;
    }

    vector_node_ptr vec() override
    {
        return memory_context_.temp_vec_node_;
    }

    inline typename expression_node<T>::node_type type() const override
    {
        return expression_node<T>::node_type::e_vecunaryop;
    }

    vector_interface<T>* as_vector_iface() override
    {
        return this;
    }

    inline bool valid() const override
    {
        return vec0_node_ptr_ && unary_node<T>::valid();
    }

    std::size_t size() const override
    {
        return vec0_node_ptr_->vec_holder().size();
    }

    std::size_t base_size() const override
    {
        return vec0_node_ptr_->vec_holder().base_size();
    }

    vds_t& vds() override
    {
        return vds_;
    }

    const vds_t& vds() const override
    {
        return vds_;
    }

   private:
    vector_node_ptr vec0_node_ptr_;
    vds_t vds_;
    memory_context memory_context_;
};

template <typename T>
class conditional_vector_node final : public expression_node<T>, public vector_interface<T>
{
   public:
    using expression_ptr = expression_node<T>*;
    using vec_interface_ptr = vector_interface<T>*;
    using vector_node_ptr = vector_node<T>*;
    using vector_holder_t = vector_holder<T>;
    using vector_holder_ptr = vector_holder_t*;
    using vector_view_t = math_expr::vector_view<T>;
    using vds_t = core::vec_data_store<T>;
    using branch_t = std::pair<expression_ptr, bool>;

    conditional_vector_node(expression_ptr condition, expression_ptr consequent,
                            expression_ptr alternative)
        : consequent_node_ptr_(nullptr),
          alternative_node_ptr_(nullptr),
          temp_vec_node_(nullptr),
          temp_(nullptr),
          temp_view_(nullptr),
          result_vec_size_(0),
          initialised_(false)
    {
        construct_branch_pair(condition_, condition);
        construct_branch_pair(consequent_, consequent);
        construct_branch_pair(alternative_, alternative);

        if (details::is_ivector_node(consequent_.first))
        {
            vec_interface_ptr ivec_ptr = consequent_.first->as_vector_iface();

            if (nullptr != ivec_ptr)
            {
                consequent_node_ptr_ = ivec_ptr->vec();
            }
        }

        if (details::is_ivector_node(alternative_.first))
        {
            vec_interface_ptr ivec_ptr = alternative_.first->as_vector_iface();

            if (nullptr != ivec_ptr)
            {
                alternative_node_ptr_ = ivec_ptr->vec();
            }
        }

        if (consequent_node_ptr_ && alternative_node_ptr_)
        {
            const std::size_t vec_size = std::max(consequent_node_ptr_->vec_holder().base_size(),
                                                  alternative_node_ptr_->vec_holder().base_size());

            if (vec_size > 0)
            {
                vds_ = vds_t(vec_size);
                temp_view_ = new vector_view_t(vds_.data(), vec_size);
                temp_ = new vector_holder_t(*temp_view_);
                temp_vec_node_ = new vector_node<T>(vds_, temp_);
                result_vec_size_ = vec_size;
                initialised_ = (nullptr != temp_view_) && (nullptr != temp_) &&
                               (nullptr != temp_vec_node_) && (nullptr != vds_.data());
            }
        }

        assert(initialised_);
    }

    ~conditional_vector_node()
    {
        delete temp_vec_node_;
        delete temp_;
        delete temp_view_;
    }

    inline T value() const override
    {
        T result = T(0);
        T* source_vector = nullptr;
        T* result_vector = vds().data();

        if (is_true(condition_))
        {
            result = consequent_.first->value();
            source_vector = consequent_node_ptr_->vds().data();
            result_vec_size_ = consequent_node_ptr_->size();
        }
        else
        {
            result = alternative_.first->value();
            source_vector = alternative_node_ptr_->vds().data();
            result_vec_size_ = alternative_node_ptr_->size();
        }

        if (temp_view_)
        {
            temp_view_->set_size(result_vec_size_);
        }

        for (std::size_t i = 0; i < result_vec_size_; ++i)
        {
            result_vector[i] = source_vector[i];
        }

        return result;
    }

    vector_node_ptr vec() const override
    {
        return temp_vec_node_;
    }

    vector_node_ptr vec() override
    {
        return temp_vec_node_;
    }

    inline typename expression_node<T>::node_type type() const override
    {
        return expression_node<T>::node_type::e_vecondition;
    }

    vector_interface<T>* as_vector_iface() override
    {
        return this;
    }

    inline bool valid() const override
    {
        return initialised_ && condition_.first && condition_.first->valid() && consequent_.first &&
               consequent_.first->valid() && alternative_.first && alternative_.first->valid() &&
               size() <= base_size();
    }

    std::size_t size() const override
    {
        return result_vec_size_;
    }

    std::size_t base_size() const override
    {
        return vds_.size();
    }

    vds_t& vds() override
    {
        return vds_;
    }

    const vds_t& vds() const override
    {
        return vds_;
    }

    void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) override
    {
        expression_node<T>::ndb_t::collect(condition_, node_delete_list);
        expression_node<T>::ndb_t::collect(consequent_, node_delete_list);
        expression_node<T>::ndb_t::collect(alternative_, node_delete_list);
    }

    std::size_t node_depth() const override
    {
        return expression_node<T>::ndb_t::compute_node_depth(condition_, consequent_, alternative_);
    }

   private:
    branch_t condition_;
    branch_t consequent_;
    branch_t alternative_;
    vector_node_ptr consequent_node_ptr_;
    vector_node_ptr alternative_node_ptr_;
    vector_node_ptr temp_vec_node_;
    vector_holder_ptr temp_;
    vector_view_t* temp_view_;
    vds_t vds_;
    mutable std::size_t result_vec_size_;
    bool initialised_;
};

template <typename T>
class scand_node final : public binary_node<T>
{
   public:
    using expression_ptr = expression_node<T>*;
    using binary_node<T>::branch;

    scand_node(const core::operators::operator_type& opr, expression_ptr branch0,
               expression_ptr branch1)
        : binary_node<T>(opr, branch0, branch1)
    {
        assert(binary_node<T>::valid());
    }

    inline T value() const override
    {
        return (details::is_true(branch(0)->value()) && details::is_true(branch(1)->value()))
                   ? core::numeric::true_v<T>
                   : core::numeric::false_v<T>;
    }
};

template <typename T>
class scor_node final : public binary_node<T>
{
   public:
    using expression_ptr = expression_node<T>*;
    using binary_node<T>::branch;

    scor_node(const core::operators::operator_type& opr, expression_ptr branch0,
              expression_ptr branch1)
        : binary_node<T>(opr, branch0, branch1)
    {
        assert(binary_node<T>::valid());
    }

    inline T value() const override
    {
        return (details::is_true(branch(0)->value()) || details::is_true(branch(1)->value()))
                   ? core::numeric::true_v<T>
                   : core::numeric::false_v<T>;
    }
};

template <typename T>
class fixed_function_base_node : public expression_node<T>
{
   public:
    using expression_ptr = expression_node<T>*;

    fixed_function_base_node<T>* as_fixed_function_base() override
    {
        return this;
    }

    virtual std::size_t arity() const = 0;
    virtual T evaluate_values(const T* values) const = 0;
};

template <typename T, typename IFunction, std::size_t N>
class function_N_node final : public fixed_function_base_node<T>
{
   public:
    // Function of N parameters.
    using expression_ptr = expression_node<T>*;
    using branch_t = std::pair<expression_ptr, bool>;
    using ifunction = IFunction;

    explicit function_N_node(ifunction* func)
        : function_((N == func->param_count) ? func : nullptr),
          parameter_count_(func->param_count),
          initialised_(false)
    {
    }

    template <std::size_t NumBranches>
    bool init_branches(expression_ptr (&b)[NumBranches])
    {
        if constexpr (N != NumBranches)
        {
            return false;
        }
        else
        {
            for (std::size_t i = 0; i < NumBranches; ++i)
            {
                if (b[i] && b[i]->valid())
                    branch_[i] = std::make_pair(b[i], branch_deletable(b[i]));
                else
                    return false;
            }

            initialised_ = function_;
            assert(valid());
            return initialised_;
        }
    }

    inline bool operator<(const function_N_node<T, IFunction, N>& fn) const
    {
        return this < (&fn);
    }

    inline T value() const override
    {
        T v[N];
        evaluate_branches<T, N>::execute(v, branch_);
        return invoke<T, N>::execute(*function_, v);
    }

    inline typename expression_node<T>::node_type type() const override
    {
        return expression_node<T>::node_type::e_function;
    }

    expression_ptr branch(const std::size_t& index = 0) const override
    {
        return (index < N) ? branch_[index].first : nullptr;
    }

    inline bool valid() const override
    {
        return initialised_;
    }

    std::size_t arity() const override
    {
        return N;
    }

    T evaluate_values(const T* values) const override
    {
        return invoke_values(std::make_index_sequence<N>{}, values);
    }

    void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) override
    {
        expression_node<T>::ndb_t::collect(branch_, node_delete_list);
    }

    std::size_t node_depth() const override
    {
        return expression_node<T>::ndb_t::template compute_node_depth<N>(branch_);
    }

    template <typename T_, std::size_t BranchCount>
    struct evaluate_branches
    {
        static inline void execute(T_ (&v)[BranchCount], const branch_t (&b)[BranchCount])
        {
            for (std::size_t i = 0; i < BranchCount; ++i)
            {
                v[i] = b[i].first->value();
            }
        }
    };

    template <typename T_>
    struct evaluate_branches<T_, 6>
    {
        static inline void execute(T_ (&v)[6], const branch_t (&b)[6])
        {
            v[0] = b[0].first->value();
            v[1] = b[1].first->value();
            v[2] = b[2].first->value();
            v[3] = b[3].first->value();
            v[4] = b[4].first->value();
            v[5] = b[5].first->value();
        }
    };

    template <typename T_>
    struct evaluate_branches<T_, 5>
    {
        static inline void execute(T_ (&v)[5], const branch_t (&b)[5])
        {
            v[0] = b[0].first->value();
            v[1] = b[1].first->value();
            v[2] = b[2].first->value();
            v[3] = b[3].first->value();
            v[4] = b[4].first->value();
        }
    };

    template <typename T_>
    struct evaluate_branches<T_, 4>
    {
        static inline void execute(T_ (&v)[4], const branch_t (&b)[4])
        {
            v[0] = b[0].first->value();
            v[1] = b[1].first->value();
            v[2] = b[2].first->value();
            v[3] = b[3].first->value();
        }
    };

    template <typename T_>
    struct evaluate_branches<T_, 3>
    {
        static inline void execute(T_ (&v)[3], const branch_t (&b)[3])
        {
            v[0] = b[0].first->value();
            v[1] = b[1].first->value();
            v[2] = b[2].first->value();
        }
    };

    template <typename T_>
    struct evaluate_branches<T_, 2>
    {
        static inline void execute(T_ (&v)[2], const branch_t (&b)[2])
        {
            v[0] = b[0].first->value();
            v[1] = b[1].first->value();
        }
    };

    template <typename T_>
    struct evaluate_branches<T_, 1>
    {
        static inline void execute(T_ (&v)[1], const branch_t (&b)[1])
        {
            v[0] = b[0].first->value();
        }
    };

    template <typename T_, std::size_t ParamCount>
    struct invoke
    {
        static inline T execute(ifunction&, branch_t (&)[ParamCount])
        {
            return std::numeric_limits<T_>::quiet_NaN();
        }
    };

    template <typename T_>
    struct invoke<T_, 20>
    {
        static inline T_ execute(ifunction& f, T_ (&v)[20])
        {
            return f(v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[8], v[9], v[10], v[11],
                     v[12], v[13], v[14], v[15], v[16], v[17], v[18], v[19]);
        }
    };

    template <typename T_>
    struct invoke<T_, 19>
    {
        static inline T_ execute(ifunction& f, T_ (&v)[19])
        {
            return f(v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[8], v[9], v[10], v[11],
                     v[12], v[13], v[14], v[15], v[16], v[17], v[18]);
        }
    };

    template <typename T_>
    struct invoke<T_, 18>
    {
        static inline T_ execute(ifunction& f, T_ (&v)[18])
        {
            return f(v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[8], v[9], v[10], v[11],
                     v[12], v[13], v[14], v[15], v[16], v[17]);
        }
    };

    template <typename T_>
    struct invoke<T_, 17>
    {
        static inline T_ execute(ifunction& f, T_ (&v)[17])
        {
            return f(v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[8], v[9], v[10], v[11],
                     v[12], v[13], v[14], v[15], v[16]);
        }
    };

    template <typename T_>
    struct invoke<T_, 16>
    {
        static inline T_ execute(ifunction& f, T_ (&v)[16])
        {
            return f(v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[8], v[9], v[10], v[11],
                     v[12], v[13], v[14], v[15]);
        }
    };

    template <typename T_>
    struct invoke<T_, 15>
    {
        static inline T_ execute(ifunction& f, T_ (&v)[15])
        {
            return f(v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[8], v[9], v[10], v[11],
                     v[12], v[13], v[14]);
        }
    };

    template <typename T_>
    struct invoke<T_, 14>
    {
        static inline T_ execute(ifunction& f, T_ (&v)[14])
        {
            return f(v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[8], v[9], v[10], v[11],
                     v[12], v[13]);
        }
    };

    template <typename T_>
    struct invoke<T_, 13>
    {
        static inline T_ execute(ifunction& f, T_ (&v)[13])
        {
            return f(v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[8], v[9], v[10], v[11],
                     v[12]);
        }
    };

    template <typename T_>
    struct invoke<T_, 12>
    {
        static inline T_ execute(ifunction& f, T_ (&v)[12])
        {
            return f(v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[8], v[9], v[10], v[11]);
        }
    };

    template <typename T_>
    struct invoke<T_, 11>
    {
        static inline T_ execute(ifunction& f, T_ (&v)[11])
        {
            return f(v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[8], v[9], v[10]);
        }
    };

    template <typename T_>
    struct invoke<T_, 10>
    {
        static inline T_ execute(ifunction& f, T_ (&v)[10])
        {
            return f(v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[8], v[9]);
        }
    };

    template <typename T_>
    struct invoke<T_, 9>
    {
        static inline T_ execute(ifunction& f, T_ (&v)[9])
        {
            return f(v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[8]);
        }
    };

    template <typename T_>
    struct invoke<T_, 8>
    {
        static inline T_ execute(ifunction& f, T_ (&v)[8])
        {
            return f(v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7]);
        }
    };

    template <typename T_>
    struct invoke<T_, 7>
    {
        static inline T_ execute(ifunction& f, T_ (&v)[7])
        {
            return f(v[0], v[1], v[2], v[3], v[4], v[5], v[6]);
        }
    };

    template <typename T_>
    struct invoke<T_, 6>
    {
        static inline T_ execute(ifunction& f, T_ (&v)[6])
        {
            return f(v[0], v[1], v[2], v[3], v[4], v[5]);
        }
    };

    template <typename T_>
    struct invoke<T_, 5>
    {
        static inline T_ execute(ifunction& f, T_ (&v)[5])
        {
            return f(v[0], v[1], v[2], v[3], v[4]);
        }
    };

    template <typename T_>
    struct invoke<T_, 4>
    {
        static inline T_ execute(ifunction& f, T_ (&v)[4])
        {
            return f(v[0], v[1], v[2], v[3]);
        }
    };

    template <typename T_>
    struct invoke<T_, 3>
    {
        static inline T_ execute(ifunction& f, T_ (&v)[3])
        {
            return f(v[0], v[1], v[2]);
        }
    };

    template <typename T_>
    struct invoke<T_, 2>
    {
        static inline T_ execute(ifunction& f, T_ (&v)[2])
        {
            return f(v[0], v[1]);
        }
    };

    template <typename T_>
    struct invoke<T_, 1>
    {
        static inline T_ execute(ifunction& f, T_ (&v)[1])
        {
            return f(v[0]);
        }
    };

   private:
    template <std::size_t... I>
    T invoke_values(std::index_sequence<I...>, const T* values) const
    {
        return (*function_)(values[I]...);
    }

    ifunction* function_;
    std::size_t parameter_count_;
    branch_t branch_[N];
    bool initialised_;
};

template <typename T, typename IFunction>
class function_N_node<T, IFunction, 0> final : public fixed_function_base_node<T>
{
   public:
    using expression_ptr = expression_node<T>*;
    using ifunction = IFunction;

    explicit function_N_node(ifunction* func) : function_((0 == func->param_count) ? func : nullptr)
    {
        assert(valid());
    }

    inline bool operator<(const function_N_node<T, IFunction, 0>& fn) const
    {
        return this < (&fn);
    }

    inline T value() const override
    {
        return (*function_)();
    }

    inline typename expression_node<T>::node_type type() const override
    {
        return expression_node<T>::node_type::e_function;
    }

    inline bool valid() const override
    {
        return function_;
    }

    std::size_t arity() const override
    {
        return 0;
    }

    T evaluate_values(const T*) const override
    {
        return (*function_)();
    }

   private:
    ifunction* function_;
};

template <typename T, typename VarArgFunction>
class vararg_function_node final : public expression_node<T>
{
   public:
    using expression_ptr = expression_node<T>*;

    vararg_function_node(VarArgFunction* func, const std::vector<expression_ptr>& arg_list)
        : function_(func), arg_list_(arg_list)
    {
        value_list_.resize(arg_list.size(), std::numeric_limits<T>::quiet_NaN());
        assert(valid());
    }

    inline bool operator<(const vararg_function_node<T, VarArgFunction>& fn) const
    {
        return this < (&fn);
    }

    inline T value() const override
    {
        populate_value_list();
        return (*function_)(value_list_);
    }

    inline typename expression_node<T>::node_type type() const override
    {
        return expression_node<T>::node_type::e_vafunction;
    }

    inline bool valid() const override
    {
        return function_;
    }

    void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) override
    {
        for (std::size_t i = 0; i < arg_list_.size(); ++i)
        {
            if (arg_list_[i] && !details::is_variable_node(arg_list_[i]))
            {
                node_delete_list.push_back(&arg_list_[i]);
            }
        }
    }

    std::size_t node_depth() const override
    {
        return expression_node<T>::ndb_t::compute_node_depth(arg_list_);
    }

   private:
    inline void populate_value_list() const
    {
        for (std::size_t i = 0; i < arg_list_.size(); ++i)
        {
            value_list_[i] = arg_list_[i]->value();
        }
    }

    VarArgFunction* function_;
    std::vector<expression_ptr> arg_list_;
    mutable std::vector<T> value_list_;
};

template <typename T, typename GenericFunction>
class generic_function_node : public expression_node<T>
{
   public:
    using type_store_t = type_store<T>;
    using expression_ptr = expression_node<T>*;
    using variable_node_t = variable_node<T>;
    using vector_node_t = vector_node<T>;
    using variable_node_ptr_t = variable_node_t*;
    using vector_node_ptr_t = vector_node_t*;
    using range_interface_t = range_interface<T>;
    using range_data_type_t = range_data_type<T>;
    using range_t = typename range_interface<T>::range_t;

    using branch_t = std::pair<expression_ptr, bool>;
    using vh_t = vector_holder<T>*;
    using vecview_t = vector_view<T>*;

    using tmp_vs_t = std::vector<T>;
    using typestore_list_t = std::vector<type_store_t>;
    using range_list_t = std::vector<range_data_type_t>;

    explicit generic_function_node(const std::vector<expression_ptr>& arg_list,
                                   GenericFunction* func = nullptr)
        : function_(func), arg_list_(arg_list)
    {
    }

    virtual ~generic_function_node()
    {
        for (std::size_t i = 0; i < vv_list_.size(); ++i)
        {
            vecview_t& vv = vv_list_[i];
            if (vv)
            {
                if (auto* pp = std::get_if<T*>(&typestore_list_[i].data); pp && *pp)
                {
                    vv->remove_ref(pp);
                }

                vv->remove_size_ref(&typestore_list_[i].size);
                typestore_list_[i].data = static_cast<T*>(nullptr);
            }
        }
    }

    void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) override
    {
        expression_node<T>::ndb_t::collect(branch_, node_delete_list);
    }

    std::size_t node_depth() const override final
    {
        return expression_node<T>::ndb_t::compute_node_depth(branch_);
    }

    virtual bool init_branches()
    {
        expr_as_vec1_store_.resize(arg_list_.size(), T(0));
        typestore_list_.resize(arg_list_.size(), type_store_t());
        range_list_.resize(arg_list_.size(), range_data_type_t());
        branch_.resize(arg_list_.size(), branch_t(nullptr, false));
        vv_list_.resize(arg_list_.size(), vecview_t(0));

        for (std::size_t i = 0; i < arg_list_.size(); ++i)
        {
            type_store_t& ts = typestore_list_[i];

            if (nullptr == arg_list_[i])
                return false;
            else if (is_ivector_node(arg_list_[i]))
            {
                vector_interface<T>* vi = nullptr;

                vi = arg_list_[i]->as_vector_iface();
                if (vi == nullptr)
                    return false;

                ts.size = vi->size();
                ts.data = vi->vds().data();
                ts.type = type_store_t::store_type::e_vector;
                ts.ivec = vi;

                if (vi->vec()->vec_holder().rebaseable() &&
                    vi->vec()->vec_holder().rebaseable_instance())
                {
                    vv_list_[i] = vi->vec()->vec_holder().rebaseable_instance();
                    vv_list_[i]->set_size_ref(&ts.size);

                    if (!amalgamated_vecop(arg_list_[i]))
                    {
                        vv_list_[i]->set_ref(std::get_if<T*>(&ts.data));
                    }
                }
            }
#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
            else if (is_generally_string_node(arg_list_[i]))
            {
                string_base_node<T>* sbn = nullptr;

                sbn = arg_list_[i]->as_string_base();
                if (sbn == nullptr)
                    return false;

                ts.size = sbn->size();
                ts.data = const_cast<char*>(sbn->base());
                ts.type = type_store_t::store_type::e_string;

                range_list_[i].data = std::get<char*>(ts.data);
                range_list_[i].size = ts.size;
                range_list_[i].type_size = sizeof(char);
                range_list_[i].str_node = sbn;

                range_interface_t* ri = nullptr;

                ri = arg_list_[i]->as_range_iface();
                if (ri == nullptr)
                    return false;

                const range_t& rp = ri->range_ref();

                if (rp.const_range() && is_const_string_range_node(arg_list_[i]))
                {
                    ts.size = rp.const_size();
                    ts.data = std::get<char*>(ts.data) + rp.n0_c.second;
                    range_list_[i].range = nullptr;
                }
                else
                {
                    range_list_[i].range = &(ri->range_ref());
                    range_param_list_.push_back(i);
                }
            }
#endif
            else if (is_variable_node(arg_list_[i]))
            {
                variable_node_ptr_t var =
                    static_cast<variable_node_ptr_t>(arg_list_[i]->as_variable_node());

                if (var == nullptr)
                    return false;

                ts.size = 1;
                ts.data = &var->ref();
                ts.type = type_store_t::store_type::e_scalar;
            }
            else
            {
                ts.size = 1;
                ts.data = &expr_as_vec1_store_[i];
                ts.type = type_store_t::store_type::e_scalar;
            }

            branch_[i] = std::make_pair(arg_list_[i], branch_deletable(arg_list_[i]));
        }

        return true;
    }

    inline bool operator<(const generic_function_node<T, GenericFunction>& fn) const
    {
        return this < (&fn);
    }

    inline T value() const override
    {
        if (populate_value_list())
        {
            using parameter_list_t = typename GenericFunction::parameter_list_t;

            return (*function_)(parameter_list_t(typestore_list_));
        }

        return std::numeric_limits<T>::quiet_NaN();
    }

    inline typename expression_node<T>::node_type type() const override
    {
        return expression_node<T>::node_type::e_genfunction;
    }

    inline bool valid() const override
    {
        return function_;
    }

   protected:
    inline virtual bool populate_value_list() const
    {
        assert(branch_.size() == typestore_list_.size());

        for (std::size_t i = 0; i < branch_.size(); ++i)
        {
            expr_as_vec1_store_[i] = branch_[i].first->value();
        }

        if (!range_param_list_.empty())
        {
            assert(range_param_list_.size() <= branch_.size());

            for (std::size_t i = 0; i < range_param_list_.size(); ++i)
            {
                const std::size_t index = range_param_list_[i];
                range_data_type_t& rdt = range_list_[index];

                const range_t& rp = (*rdt.range);
                std::size_t r0 = 0;
                std::size_t r1 = 0;

                const std::size_t data_size =
#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
                    rdt.str_node ? rdt.str_node->size() : rdt.size;
#else
                    rdt.size;
#endif

                if (!rp(r0, r1, data_size))
                {
                    return false;
                }

                type_store_t& ts = typestore_list_[index];

                ts.size = rp.cache_size();
#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
                if (ts.type == type_store_t::store_type::e_string)
                    ts.data = const_cast<char*>(rdt.str_node->base()) + rp.cache.first;
                else
#endif
                    ts.data = reinterpret_cast<char*>(std::get<T*>(rdt.data)) +
                              (rp.cache.first * rdt.type_size);
            }
        }

        return true;
    }

    GenericFunction* function_;
    mutable typestore_list_t typestore_list_;

   private:
    std::vector<expression_ptr> arg_list_;
    std::vector<branch_t> branch_;
    std::vector<vecview_t> vv_list_;
    mutable tmp_vs_t expr_as_vec1_store_;
    mutable range_list_t range_list_;
    std::vector<std::size_t> range_param_list_;
};

#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
template <typename T, typename StringFunction>
class string_function_node : public generic_function_node<T, StringFunction>,
                             public string_base_node<T>,
                             public range_interface<T>
{
   public:
    using gen_function_t = generic_function_node<T, StringFunction>;
    using range_t = typename range_interface<T>::range_t;

    string_function_node(StringFunction* func,
                         const std::vector<typename gen_function_t::expression_ptr>& arg_list)
        : gen_function_t(arg_list, func)
    {
        range_.n0_c = std::make_pair<bool, std::size_t>(true, 0);
        range_.n1_c = std::make_pair<bool, std::size_t>(true, 0);
        range_.cache.first = range_.n0_c.second;
        range_.cache.second = range_.n1_c.second;
        assert(valid());
    }

    inline bool operator<(const string_function_node<T, StringFunction>& fn) const
    {
        return this < (&fn);
    }

    inline T value() const override
    {
        if (gen_function_t::populate_value_list())
        {
            using parameter_list_t = typename StringFunction::parameter_list_t;

            const T result = (*gen_function_t::function_)(
                ret_string_, parameter_list_t(gen_function_t::typestore_list_));

            range_.n1_c.second = ret_string_.size();
            range_.cache.second = range_.n1_c.second;

            return result;
        }

        return std::numeric_limits<T>::quiet_NaN();
    }

    inline typename expression_node<T>::node_type type() const override
    {
        return expression_node<T>::node_type::e_strfunction;
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
        return gen_function_t::function_;
    }

    std::string str() const override
    {
        return ret_string_;
    }

    core::char_cptr base() const override
    {
        return &ret_string_[0];
    }

    std::size_t size() const override
    {
        return ret_string_.size();
    }

    range_t& range_ref() override
    {
        return range_;
    }

    const range_t& range_ref() const override
    {
        return range_;
    }

   protected:
    mutable range_t range_;
    mutable std::string ret_string_;
};
#endif

template <typename T, typename GenericFunction>
class multimode_genfunction_node final : public generic_function_node<T, GenericFunction>
{
   public:
    using gen_function_t = generic_function_node<T, GenericFunction>;
    using range_t = typename gen_function_t::range_t;

    multimode_genfunction_node(GenericFunction* func, const std::size_t& param_seq_index,
                               const std::vector<typename gen_function_t::expression_ptr>& arg_list)
        : gen_function_t(arg_list, func), param_seq_index_(param_seq_index)
    {
    }

    inline T value() const override
    {
        assert(gen_function_t::valid());

        if (gen_function_t::populate_value_list())
        {
            using parameter_list_t = typename GenericFunction::parameter_list_t;

            return (*gen_function_t::function_)(param_seq_index_,
                                                parameter_list_t(gen_function_t::typestore_list_));
        }

        return std::numeric_limits<T>::quiet_NaN();
    }

    inline typename expression_node<T>::node_type type() const override final
    {
        return expression_node<T>::node_type::e_genfunction;
    }

   private:
    std::size_t param_seq_index_;
};

#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
template <typename T, typename StringFunction>
class multimode_strfunction_node final : public string_function_node<T, StringFunction>
{
   public:
    using str_function_t = string_function_node<T, StringFunction>;
    using range_t = typename str_function_t::range_t;

    multimode_strfunction_node(StringFunction* func, const std::size_t& param_seq_index,
                               const std::vector<typename str_function_t::expression_ptr>& arg_list)
        : str_function_t(func, arg_list), param_seq_index_(param_seq_index)
    {
    }

    inline T value() const override
    {
        if (str_function_t::populate_value_list())
        {
            using parameter_list_t = typename StringFunction::parameter_list_t;

            const T result =
                (*str_function_t::function_)(param_seq_index_, str_function_t::ret_string_,
                                             parameter_list_t(str_function_t::typestore_list_));

            str_function_t::range_.n1_c.second = str_function_t::ret_string_.size();
            str_function_t::range_.cache.second = str_function_t::range_.n1_c.second;

            return result;
        }

        return std::numeric_limits<T>::quiet_NaN();
    }

    inline typename expression_node<T>::node_type type() const override
    {
        return expression_node<T>::node_type::e_strfunction;
    }

   private:
    const std::size_t param_seq_index_;
};
#endif

}  // namespace math_expr::details

#endif
