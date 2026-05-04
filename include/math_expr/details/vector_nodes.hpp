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

#ifndef MATH_EXPR_DETAILS_VECTOR_NODES_HPP
#define MATH_EXPR_DETAILS_VECTOR_NODES_HPP

#include <variant>
#include "math_expr/assert_check.hpp"
#include "math_expr/core/numeric.hpp"
#include "math_expr/core/vec_data_store.hpp"
#include "math_expr/vector_access_runtime_check.hpp"
#include "math_expr/details/range_pack.hpp"
#include "math_expr/details/loop_nodes.hpp"
#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
#include "math_expr/details/string_nodes.hpp"
#endif

namespace math_expr::details
{
template <typename T>
class variable_node final : public expression_node<T>, public ivariable<T>
{
   public:
    static T null_value;

    explicit variable_node() : value_(&null_value) {}

    explicit variable_node(T& v) : value_(&v) {}

    inline bool operator<(const variable_node<T>& v) const
    {
        return this < (&v);
    }

    inline T value() const override
    {
        return (*value_);
    }

    inline T& ref() override
    {
        return (*value_);
    }

    inline const T& ref() const override
    {
        return (*value_);
    }

    inline typename expression_node<T>::node_type type() const override
    {
        return expression_node<T>::node_type::e_variable;
    }

   private:
    T* value_;
};

template <typename T>
T variable_node<T>::null_value = T(std::numeric_limits<T>::quiet_NaN());

template <typename T>
class string_base_node;

template <typename T>
struct range_data_type
{
    using range_t = range_pack<T>;
    using strbase_ptr_t = string_base_node<T>*;

    range_data_type()
        : range(nullptr), data(static_cast<T*>(nullptr)), size(0), type_size(0), str_node(nullptr)
    {
    }

    range_t* range;
    std::variant<T*, char*> data;
    std::size_t size;
    std::size_t type_size;
    strbase_ptr_t str_node;
};

template <typename T>
class vector_node;

template <typename T>
class vector_interface
{
   public:
    using vector_node_ptr = vector_node<T>*;
    using vds_t = core::vec_data_store<T>;

    virtual ~vector_interface() {}

    virtual std::size_t size() const = 0;

    virtual std::size_t base_size() const = 0;

    virtual vector_node_ptr vec() const = 0;

    virtual vector_node_ptr vec() = 0;

    virtual vds_t& vds() = 0;

    virtual const vds_t& vds() const = 0;

    virtual bool side_effect() const
    {
        return false;
    }
};

template <typename T>
class vector_node final : public expression_node<T>, public vector_interface<T>
{
   public:
    using expression_ptr = expression_node<T>*;
    using vector_holder_t = vector_holder<T>;
    using vector_node_ptr = vector_node<T>*;
    using vds_t = core::vec_data_store<T>;

    explicit vector_node(vector_holder_t* vh)
        : vector_holder_(vh), vds_((*vector_holder_).size(), (*vector_holder_)[0])
    {
        vector_holder_->set_ref(&vds_.ref());
    }

    vector_node(const vds_t& vds, vector_holder_t* vh) : vector_holder_(vh), vds_(vds) {}

    ~vector_node() override
    {
        assert(valid());
        vector_holder_->remove_ref(&vds_.ref());
    }

    inline T value() const override
    {
        return vds().data()[0];
    }

    vector_node_ptr vec() const override
    {
        return const_cast<vector_node_ptr>(this);
    }

    vector_node_ptr vec() override
    {
        return this;
    }

    inline typename expression_node<T>::node_type type() const override
    {
        return expression_node<T>::node_type::e_vector;
    }

    inline bool valid() const override
    {
        return vector_holder_;
    }

    std::size_t size() const override
    {
        return vec_holder().size();
    }

    std::size_t base_size() const override
    {
        return vec_holder().base_size();
    }

    vds_t& vds() override
    {
        return vds_;
    }

    const vds_t& vds() const override
    {
        return vds_;
    }

    inline vector_holder_t& vec_holder()
    {
        return (*vector_holder_);
    }

    inline vector_holder_t& vec_holder() const
    {
        return (*vector_holder_);
    }

   private:
    vector_holder_t* vector_holder_;
    vds_t vds_;
};

template <typename T>
class vector_size_node final : public expression_node<T>
{
   public:
    using expression_ptr = expression_node<T>*;
    using vector_holder_t = vector_holder<T>;

    explicit vector_size_node(vector_holder_t* vh) : vector_holder_(vh) {}

    ~vector_size_node() override
    {
        assert(valid());
    }

    inline T value() const override
    {
        assert(vector_holder_);
        return static_cast<T>(vector_holder_->size());
    }

    inline typename expression_node<T>::node_type type() const override
    {
        return expression_node<T>::node_type::e_vecsize;
    }

    inline bool valid() const override
    {
        return vector_holder_ && vector_holder_->size();
    }

    inline vector_holder_t* vec_holder()
    {
        return vector_holder_;
    }

   private:
    vector_holder_t* vector_holder_;
};

template <typename T>
class vector_elem_node final : public expression_node<T>, public ivariable<T>
{
   public:
    using expression_ptr = expression_node<T>*;
    using vector_holder_t = vector_holder<T>;
    using vector_holder_ptr = vector_holder_t*;
    using branch_t = std::pair<expression_ptr, bool>;

    vector_elem_node(expression_ptr vec_node, expression_ptr index, vector_holder_ptr vec_holder)
        : vector_holder_(vec_holder), vector_base_((*vec_holder)[0])
    {
        construct_branch_pair(vector_node_, vec_node);
        construct_branch_pair(index_, index);
        assert(valid());
    }

    inline T value() const override
    {
        return *access_vector();
    }

    inline T& ref() override
    {
        return *access_vector();
    }

    inline const T& ref() const override
    {
        return *access_vector();
    }

    inline typename expression_node<T>::node_type type() const override
    {
        return expression_node<T>::node_type::e_vecelem;
    }

    inline bool valid() const override
    {
        return vector_holder_ && index_.first && vector_node_.first && index_.first->valid() &&
               vector_node_.first->valid();
    }

    inline vector_holder_t& vec_holder()
    {
        return (*vector_holder_);
    }

    void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) override
    {
        expression_node<T>::ndb_t::collect(vector_node_, node_delete_list);
        expression_node<T>::ndb_t::collect(index_, node_delete_list);
    }

    std::size_t node_depth() const override
    {
        return expression_node<T>::ndb_t::compute_node_depth(vector_node_, index_);
    }

   private:
    inline T* access_vector() const
    {
        vector_node_.first->value();
        return (vector_base_ + core::numeric::to_uint64(index_.first->value()));
    }

    vector_holder_ptr vector_holder_;
    T* vector_base_;
    branch_t vector_node_;
    branch_t index_;
};

template <typename T>
class vector_celem_node final : public expression_node<T>, public ivariable<T>
{
   public:
    using expression_ptr = expression_node<T>*;
    using vector_holder_t = vector_holder<T>;
    using vector_holder_ptr = vector_holder_t*;
    using branch_t = std::pair<expression_ptr, bool>;

    vector_celem_node(expression_ptr vec_node, const std::size_t index,
                      vector_holder_ptr vec_holder)
        : index_(index), vector_holder_(vec_holder), vector_base_((*vec_holder)[0])
    {
        construct_branch_pair(vector_node_, vec_node);
        assert(valid());
    }

    inline T value() const override
    {
        return *access_vector();
    }

    inline T& ref() override
    {
        return *access_vector();
    }

    inline const T& ref() const override
    {
        return *access_vector();
    }

    inline typename expression_node<T>::node_type type() const override
    {
        return expression_node<T>::node_type::e_veccelem;
    }

    inline bool valid() const override
    {
        return vector_holder_ && vector_node_.first && vector_node_.first->valid();
    }

    inline vector_holder_t& vec_holder()
    {
        return (*vector_holder_);
    }

    void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) override
    {
        expression_node<T>::ndb_t::collect(vector_node_, node_delete_list);
    }

    std::size_t node_depth() const override
    {
        return expression_node<T>::ndb_t::compute_node_depth(vector_node_);
    }

   private:
    inline T* access_vector() const
    {
        vector_node_.first->value();
        return (vector_base_ + index_);
    }

    const std::size_t index_;
    vector_holder_ptr vector_holder_;
    T* vector_base_;
    branch_t vector_node_;
};

template <typename T>
class vector_elem_rtc_node final : public expression_node<T>, public ivariable<T>
{
   public:
    using expression_ptr = expression_node<T>*;
    using vector_holder_t = vector_holder<T>;
    using vector_holder_ptr = vector_holder_t*;
    using branch_t = std::pair<expression_ptr, bool>;

    vector_elem_rtc_node(expression_ptr vec_node, expression_ptr index,
                         vector_holder_ptr vec_holder, vector_access_runtime_check<T>* vec_rt_chk)
        : vector_holder_(vec_holder),
          vector_base_((*vec_holder)[0]),
          vec_rt_chk_(vec_rt_chk),
          max_vector_index_(vector_holder_->size() - 1)
    {
        construct_branch_pair(vector_node_, vec_node);
        construct_branch_pair(index_, index);
        assert(valid());
    }

    inline T value() const override
    {
        return *access_vector();
    }

    inline T& ref() override
    {
        return *access_vector();
    }

    inline const T& ref() const override
    {
        return *access_vector();
    }

    inline typename expression_node<T>::node_type type() const override
    {
        return expression_node<T>::node_type::e_vecelemrtc;
    }

    inline bool valid() const override
    {
        return vector_holder_ && index_.first && vector_node_.first && index_.first->valid() &&
               vector_node_.first->valid();
    }

    inline vector_holder_t& vec_holder()
    {
        return (*vector_holder_);
    }

    void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) override
    {
        expression_node<T>::ndb_t::collect(vector_node_, node_delete_list);
        expression_node<T>::ndb_t::collect(index_, node_delete_list);
    }

    std::size_t node_depth() const override
    {
        return expression_node<T>::ndb_t::compute_node_depth(vector_node_, index_);
    }

   private:
    inline T* access_vector() const
    {
        const std::uint64_t index = core::numeric::to_uint64(index_.first->value());
        vector_node_.first->value();

        if (index <= max_vector_index_)
        {
            return (vector_holder_->data() + index);
        }

        assert(vec_rt_chk_);

        typename vector_access_runtime_check<T>::violation_context context;
        context.base_ptr = vector_base_;
        context.end_ptr = vector_base_ + vector_holder_->size();
        context.access_ptr = vector_base_ + index;
        context.type_size = sizeof(T);

        return vec_rt_chk_->handle_runtime_violation(context) ? context.access_ptr : vector_base_;
    }

    vector_holder_ptr vector_holder_;
    T* vector_base_;
    branch_t vector_node_;
    branch_t index_;
    vector_access_runtime_check<T>* vec_rt_chk_;
    const std::size_t max_vector_index_;
};

template <typename T>
class vector_celem_rtc_node final : public expression_node<T>, public ivariable<T>
{
   public:
    using expression_ptr = expression_node<T>*;
    using vector_holder_t = vector_holder<T>;
    using vector_holder_ptr = vector_holder_t*;
    using branch_t = std::pair<expression_ptr, bool>;

    vector_celem_rtc_node(expression_ptr vec_node, const std::size_t index,
                          vector_holder_ptr vec_holder, vector_access_runtime_check<T>* vec_rt_chk)
        : index_(index),
          max_vector_index_(vec_holder->size() - 1),
          vector_holder_(vec_holder),
          vector_base_((*vec_holder)[0]),
          vec_rt_chk_(vec_rt_chk)
    {
        construct_branch_pair(vector_node_, vec_node);
        assert(valid());
    }

    inline T value() const override
    {
        return *access_vector();
    }

    inline T& ref() override
    {
        return *access_vector();
    }

    inline const T& ref() const override
    {
        return *access_vector();
    }

    inline typename expression_node<T>::node_type type() const override
    {
        return expression_node<T>::node_type::e_veccelemrtc;
    }

    inline bool valid() const override
    {
        return vector_holder_ && vector_node_.first && vector_node_.first->valid();
    }

    inline vector_holder_t& vec_holder()
    {
        return (*vector_holder_);
    }

    void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) override
    {
        expression_node<T>::ndb_t::collect(vector_node_, node_delete_list);
    }

    std::size_t node_depth() const override
    {
        return expression_node<T>::ndb_t::compute_node_depth(vector_node_);
    }

   private:
    inline T* access_vector() const
    {
        vector_node_.first->value();

        if (index_ <= max_vector_index_)
        {
            return (vector_holder_->data() + index_);
        }

        assert(vec_rt_chk_);

        typename vector_access_runtime_check<T>::violation_context context;
        context.base_ptr = vector_base_;
        context.end_ptr = vector_base_ + vector_holder_->size();
        context.access_ptr = vector_base_ + index_;
        context.type_size = sizeof(T);

        return vec_rt_chk_->handle_runtime_violation(context) ? context.access_ptr : vector_base_;
    }

    const std::size_t index_;
    const std::size_t max_vector_index_;
    vector_holder_ptr vector_holder_;
    T* vector_base_;
    branch_t vector_node_;
    vector_access_runtime_check<T>* vec_rt_chk_;
};

template <typename T>
class rebasevector_elem_node final : public expression_node<T>, public ivariable<T>
{
   public:
    using expression_ptr = expression_node<T>*;
    using vector_holder_t = vector_holder<T>;
    using vector_holder_ptr = vector_holder_t*;
    using vds_t = core::vec_data_store<T>;
    using branch_t = std::pair<expression_ptr, bool>;

    rebasevector_elem_node(expression_ptr vec_node, expression_ptr index,
                           vector_holder_ptr vec_holder)
        : vector_holder_(vec_holder)
    {
        construct_branch_pair(vector_node_, vec_node);
        construct_branch_pair(index_, index);
        assert(valid());
    }

    inline T value() const override
    {
        return *access_vector();
    }

    inline T& ref() override
    {
        return *access_vector();
    }

    inline const T& ref() const override
    {
        return *access_vector();
    }

    inline typename expression_node<T>::node_type type() const override
    {
        return expression_node<T>::node_type::e_rbvecelem;
    }

    inline bool valid() const override
    {
        return vector_holder_ && index_.first && vector_node_.first && index_.first->valid() &&
               vector_node_.first->valid();
    }

    inline vector_holder_t& vec_holder()
    {
        return (*vector_holder_);
    }

    void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) override
    {
        expression_node<T>::ndb_t::collect(vector_node_, node_delete_list);
        expression_node<T>::ndb_t::collect(index_, node_delete_list);
    }

    std::size_t node_depth() const override
    {
        return expression_node<T>::ndb_t::compute_node_depth(vector_node_, index_);
    }

   private:
    inline T* access_vector() const
    {
        vector_node_.first->value();
        return (vector_holder_->data() + core::numeric::to_uint64(index_.first->value()));
    }

    vector_holder_ptr vector_holder_;
    branch_t vector_node_;
    branch_t index_;
};

template <typename T>
class rebasevector_celem_node final : public expression_node<T>, public ivariable<T>
{
   public:
    using expression_ptr = expression_node<T>*;
    using vector_holder_t = vector_holder<T>;
    using vector_holder_ptr = vector_holder_t*;
    using branch_t = std::pair<expression_ptr, bool>;

    rebasevector_celem_node(expression_ptr vec_node, const std::size_t index,
                            vector_holder_ptr vec_holder)
        : index_(index), vector_holder_(vec_holder)
    {
        construct_branch_pair(vector_node_, vec_node);
        assert(valid());
    }

    inline T value() const override
    {
        vector_node_.first->value();
        return ref();
    }

    inline T& ref() override
    {
        return *(vector_holder_->data() + index_);
    }

    inline const T& ref() const override
    {
        return *(vector_holder_->data() + index_);
    }

    inline typename expression_node<T>::node_type type() const override
    {
        return expression_node<T>::node_type::e_rbveccelem;
    }

    inline bool valid() const override
    {
        return vector_holder_ && vector_node_.first && vector_node_.first->valid();
    }

    inline vector_holder_t& vec_holder()
    {
        return (*vector_holder_);
    }

    void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) override
    {
        expression_node<T>::ndb_t::collect(vector_node_, node_delete_list);
    }

    std::size_t node_depth() const override
    {
        return expression_node<T>::ndb_t::compute_node_depth(vector_node_);
    }

   private:
    const std::size_t index_;
    vector_holder_ptr vector_holder_;
    branch_t vector_node_;
};

template <typename T>
class rebasevector_elem_rtc_node final : public expression_node<T>, public ivariable<T>
{
   public:
    using expression_ptr = expression_node<T>*;
    using vector_holder_t = vector_holder<T>;
    using vector_holder_ptr = vector_holder_t*;
    using branch_t = std::pair<expression_ptr, bool>;

    rebasevector_elem_rtc_node(expression_ptr vec_node, expression_ptr index,
                               vector_holder_ptr vec_holder,
                               vector_access_runtime_check<T>* vec_rt_chk)
        : vector_holder_(vec_holder), vec_rt_chk_(vec_rt_chk)
    {
        construct_branch_pair(vector_node_, vec_node);
        construct_branch_pair(index_, index);
        assert(valid());
    }

    inline T value() const override
    {
        return *access_vector();
    }

    inline T& ref() override
    {
        return *access_vector();
    }

    inline const T& ref() const override
    {
        return *access_vector();
    }

    inline typename expression_node<T>::node_type type() const override
    {
        return expression_node<T>::node_type::e_rbvecelemrtc;
    }

    inline bool valid() const override
    {
        return vector_holder_ && index_.first && vector_node_.first && index_.first->valid() &&
               vector_node_.first->valid();
    }

    inline vector_holder_t& vec_holder()
    {
        return (*vector_holder_);
    }

    void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) override
    {
        expression_node<T>::ndb_t::collect(vector_node_, node_delete_list);
        expression_node<T>::ndb_t::collect(index_, node_delete_list);
    }

    std::size_t node_depth() const override
    {
        return expression_node<T>::ndb_t::compute_node_depth(vector_node_, index_);
    }

   private:
    inline T* access_vector() const
    {
        vector_node_.first->value();
        const std::uint64_t index = core::numeric::to_uint64(index_.first->value());

        if (index <= (vector_holder_->size() - 1))
        {
            return (vector_holder_->data() + index);
        }

        assert(vec_rt_chk_);

        typename vector_access_runtime_check<T>::violation_context context;
        context.base_ptr = vector_holder_->data();
        context.end_ptr = vector_holder_->data() + vector_holder_->size();
        context.access_ptr = vector_holder_->data() + index;
        context.type_size = sizeof(T);

        return vec_rt_chk_->handle_runtime_violation(context) ? context.access_ptr
                                                              : vector_holder_->data();
    }

    vector_holder_ptr vector_holder_;
    branch_t vector_node_;
    branch_t index_;
    vector_access_runtime_check<T>* vec_rt_chk_;
};

template <typename T>
class rebasevector_celem_rtc_node final : public expression_node<T>, public ivariable<T>
{
   public:
    using expression_ptr = expression_node<T>*;
    using vector_holder_t = vector_holder<T>;
    using vector_holder_ptr = vector_holder_t*;
    using branch_t = std::pair<expression_ptr, bool>;

    rebasevector_celem_rtc_node(expression_ptr vec_node, const std::size_t index,
                                vector_holder_ptr vec_holder,
                                vector_access_runtime_check<T>* vec_rt_chk)
        : index_(index),
          vector_holder_(vec_holder),
          vector_base_((*vec_holder)[0]),
          vec_rt_chk_(vec_rt_chk)
    {
        construct_branch_pair(vector_node_, vec_node);
        assert(valid());
    }

    inline T value() const override
    {
        return *access_vector();
    }

    inline T& ref() override
    {
        return *access_vector();
    }

    inline const T& ref() const override
    {
        return *access_vector();
    }

    inline typename expression_node<T>::node_type type() const override
    {
        return expression_node<T>::node_type::e_rbveccelemrtc;
    }

    inline bool valid() const override
    {
        return vector_holder_ && vector_node_.first && vector_node_.first->valid();
    }

    inline vector_holder_t& vec_holder()
    {
        return (*vector_holder_);
    }

    void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) override
    {
        expression_node<T>::ndb_t::collect(vector_node_, node_delete_list);
    }

    std::size_t node_depth() const override
    {
        return expression_node<T>::ndb_t::compute_node_depth(vector_node_);
    }

   private:
    inline T* access_vector() const
    {
        vector_node_.first->value();

        if (index_ <= vector_holder_->size() - 1)
        {
            return (vector_holder_->data() + index_);
        }

        assert(vec_rt_chk_);

        typename vector_access_runtime_check<T>::violation_context context;
        context.base_ptr = vector_base_;
        context.end_ptr = vector_base_ + vector_holder_->size();
        context.access_ptr = vector_base_ + index_;
        context.type_size = sizeof(T);

        return vec_rt_chk_->handle_runtime_violation(context) ? context.access_ptr : vector_base_;
    }

    const std::size_t index_;
    vector_holder_ptr vector_holder_;
    T* vector_base_;
    branch_t vector_node_;
    vector_access_runtime_check<T>* vec_rt_chk_;
};

template <typename T>
class vector_initialisation_node final : public expression_node<T>
{
   public:
    using expression_ptr = expression_node<T>*;

    vector_initialisation_node(T* vector_base, const std::size_t& size,
                               const std::vector<expression_ptr>& initialiser_list,
                               const bool single_value_initialse)
        : vector_base_(vector_base),
          initialiser_list_(initialiser_list),
          size_(size),
          single_value_initialse_(single_value_initialse),
          zero_value_initialse_(false),
          const_nonzero_literal_value_initialse_(false),
          single_initialiser_value_(T(0))
    {
        if (single_value_initialse_)
        {
            if (initialiser_list_.empty())
                zero_value_initialse_ = true;
            else if ((initialiser_list_.size() == 1) &&
                     details::is_constant_node(initialiser_list_[0]) &&
                     (T(0) == initialiser_list_[0]->value()))
            {
                zero_value_initialse_ = true;
            }
            else
            {
                assert(initialiser_list_.size() == 1);

                if (details::is_constant_node(initialiser_list_[0]))
                {
                    const_nonzero_literal_value_initialse_ = true;
                    single_initialiser_value_ = initialiser_list_[0]->value();
                    assert(T(0) != single_initialiser_value_);
                }
            }
        }
    }

    inline T value() const override
    {
        if (single_value_initialse_)
        {
            if (zero_value_initialse_)
            {
                core::numeric::set_zero_value(vector_base_, size_);
            }
            else if (const_nonzero_literal_value_initialse_)
            {
                for (std::size_t i = 0; i < size_; ++i)
                {
                    *(vector_base_ + i) = single_initialiser_value_;
                }
            }
            else
            {
                for (std::size_t i = 0; i < size_; ++i)
                {
                    *(vector_base_ + i) = initialiser_list_[0]->value();
                }
            }
        }
        else
        {
            const std::size_t initialiser_list_size = initialiser_list_.size();

            for (std::size_t i = 0; i < initialiser_list_size; ++i)
            {
                *(vector_base_ + i) = initialiser_list_[i]->value();
            }

            if (initialiser_list_size < size_)
            {
                core::numeric::set_zero_value(vector_base_ + initialiser_list_size,
                                              (size_ - initialiser_list_size));
            }
        }

        return *(vector_base_);
    }

    inline typename expression_node<T>::node_type type() const override
    {
        return expression_node<T>::node_type::e_vecinit;
    }

    inline bool valid() const override
    {
        return vector_base_;
    }

    void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) override
    {
        expression_node<T>::ndb_t::collect(initialiser_list_, node_delete_list);
    }

    std::size_t node_depth() const override
    {
        return expression_node<T>::ndb_t::compute_node_depth(initialiser_list_);
    }

   private:
    vector_initialisation_node(const vector_initialisation_node<T>&) = delete;
    vector_initialisation_node<T>& operator=(const vector_initialisation_node<T>&) = delete;

    mutable T* vector_base_;
    std::vector<expression_ptr> initialiser_list_;
    const std::size_t size_;
    const bool single_value_initialse_;
    bool zero_value_initialse_;
    bool const_nonzero_literal_value_initialse_;
    T single_initialiser_value_;
};

template <typename T>
class vector_init_zero_value_node final : public expression_node<T>
{
   public:
    using expression_ptr = expression_node<T>*;

    vector_init_zero_value_node(T* vector_base, const std::size_t& size,
                                const std::vector<expression_ptr>& initialiser_list)
        : vector_base_(vector_base), size_(size), initialiser_list_(initialiser_list)
    {
    }

    inline T value() const override
    {
        core::numeric::set_zero_value(vector_base_, size_);
        return *(vector_base_);
    }

    inline typename expression_node<T>::node_type type() const override
    {
        return expression_node<T>::node_type::e_vecinit;
    }

    inline bool valid() const override
    {
        return vector_base_;
    }

    void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) override
    {
        expression_node<T>::ndb_t::collect(initialiser_list_, node_delete_list);
    }

    std::size_t node_depth() const override
    {
        return expression_node<T>::ndb_t::compute_node_depth(initialiser_list_);
    }

   private:
    vector_init_zero_value_node(const vector_init_zero_value_node<T>&) = delete;
    vector_init_zero_value_node<T>& operator=(const vector_init_zero_value_node<T>&) = delete;

    mutable T* vector_base_;
    const std::size_t size_;
    std::vector<expression_ptr> initialiser_list_;
};

template <typename T>
class vector_init_single_constvalue_node final : public expression_node<T>
{
   public:
    using expression_ptr = expression_node<T>*;

    vector_init_single_constvalue_node(T* vector_base, const std::size_t& size,
                                       const std::vector<expression_ptr>& initialiser_list)
        : vector_base_(vector_base), size_(size), initialiser_list_(initialiser_list)
    {
        single_initialiser_value_ = initialiser_list_[0]->value();
        assert(valid());
    }

    inline T value() const override
    {
        for (std::size_t i = 0; i < size_; ++i)
        {
            *(vector_base_ + i) = single_initialiser_value_;
        }

        return *(vector_base_);
    }

    inline typename expression_node<T>::node_type type() const override
    {
        return expression_node<T>::node_type::e_vecinit;
    }

    inline bool valid() const override
    {
        return vector_base_ && (initialiser_list_.size() == 1) &&
               (details::is_constant_node(initialiser_list_[0])) &&
               (single_initialiser_value_ != T(0));
    }

    void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) override
    {
        expression_node<T>::ndb_t::collect(initialiser_list_, node_delete_list);
    }

    std::size_t node_depth() const override
    {
        return expression_node<T>::ndb_t::compute_node_depth(initialiser_list_);
    }

   private:
    vector_init_single_constvalue_node(const vector_init_single_constvalue_node<T>&) = delete;
    vector_init_single_constvalue_node<T>& operator=(const vector_init_single_constvalue_node<T>&) =
        delete;

    mutable T* vector_base_;
    const std::size_t size_;
    std::vector<expression_ptr> initialiser_list_;
    T single_initialiser_value_;
};

template <typename T>
class vector_init_single_value_node final : public expression_node<T>
{
   public:
    using expression_ptr = expression_node<T>*;

    vector_init_single_value_node(T* vector_base, const std::size_t& size,
                                  const std::vector<expression_ptr>& initialiser_list)
        : vector_base_(vector_base), size_(size), initialiser_list_(initialiser_list)
    {
        assert(valid());
    }

    inline T value() const override
    {
        expression_node<T>& node = *initialiser_list_[0];

        for (std::size_t i = 0; i < size_; ++i)
        {
            *(vector_base_ + i) = node.value();
        }

        return *(vector_base_);
    }

    inline typename expression_node<T>::node_type type() const override
    {
        return expression_node<T>::node_type::e_vecinit;
    }

    inline bool valid() const override
    {
        return vector_base_ && (initialiser_list_.size() == 1) &&
               !details::is_constant_node(initialiser_list_[0]);
    }

    void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) override
    {
        expression_node<T>::ndb_t::collect(initialiser_list_, node_delete_list);
    }

    std::size_t node_depth() const override
    {
        return expression_node<T>::ndb_t::compute_node_depth(initialiser_list_);
    }

   private:
    vector_init_single_value_node(const vector_init_single_value_node<T>&) = delete;
    vector_init_single_value_node<T>& operator=(const vector_init_single_value_node<T>&) = delete;

    mutable T* vector_base_;
    const std::size_t size_;
    std::vector<expression_ptr> initialiser_list_;
};

template <typename T>
class vector_init_iota_constconst_node final : public expression_node<T>
{
   public:
    using expression_ptr = expression_node<T>*;

    vector_init_iota_constconst_node(T* vector_base, const std::size_t& size,
                                     const std::vector<expression_ptr>& initialiser_list)
        : vector_base_(vector_base), size_(size), initialiser_list_(initialiser_list)
    {
        base_value_ = initialiser_list_[0]->value();
        increment_value_ = initialiser_list_[1]->value();

        assert(valid());
    }

    inline T value() const override
    {
        T value = base_value_;

        for (std::size_t i = 0; i < size_; ++i, value += increment_value_)
        {
            *(vector_base_ + i) = value;
        }

        return *(vector_base_);
    }

    inline typename expression_node<T>::node_type type() const override
    {
        return expression_node<T>::node_type::e_vecinit;
    }

    inline bool valid() const override
    {
        return vector_base_ && (initialiser_list_.size() == 2) &&
               (details::is_constant_node(initialiser_list_[0])) &&
               (details::is_constant_node(initialiser_list_[1]));
    }

    void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) override
    {
        expression_node<T>::ndb_t::collect(initialiser_list_, node_delete_list);
    }

    std::size_t node_depth() const override
    {
        return expression_node<T>::ndb_t::compute_node_depth(initialiser_list_);
    }

   private:
    vector_init_iota_constconst_node(const vector_init_iota_constconst_node<T>&) = delete;
    vector_init_iota_constconst_node<T>& operator=(const vector_init_iota_constconst_node<T>&) =
        delete;

    mutable T* vector_base_;
    const std::size_t size_;
    std::vector<expression_ptr> initialiser_list_;
    T base_value_;
    T increment_value_;
};

template <typename T>
class vector_init_iota_constnconst_node final : public expression_node<T>
{
   public:
    using expression_ptr = expression_node<T>*;

    vector_init_iota_constnconst_node(T* vector_base, const std::size_t& size,
                                      const std::vector<expression_ptr>& initialiser_list)
        : vector_base_(vector_base), size_(size), initialiser_list_(initialiser_list)
    {
        assert(valid());
        base_value_ = initialiser_list_[0]->value();
    }

    inline T value() const override
    {
        T value = base_value_;
        expression_node<T>& increment = *initialiser_list_[1];

        for (std::size_t i = 0; i < size_; ++i, value += increment.value())
        {
            *(vector_base_ + i) = value;
        }

        return *(vector_base_);
    }

    inline typename expression_node<T>::node_type type() const override
    {
        return expression_node<T>::node_type::e_vecinit;
    }

    inline bool valid() const override
    {
        return vector_base_ && (initialiser_list_.size() == 2) &&
               (details::is_constant_node(initialiser_list_[0])) &&
               (!details::is_constant_node(initialiser_list_[1]));
    }

    void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) override
    {
        expression_node<T>::ndb_t::collect(initialiser_list_, node_delete_list);
    }

    std::size_t node_depth() const override
    {
        return expression_node<T>::ndb_t::compute_node_depth(initialiser_list_);
    }

   private:
    vector_init_iota_constnconst_node(const vector_init_iota_constnconst_node<T>&) = delete;
    vector_init_iota_constnconst_node<T>& operator=(const vector_init_iota_constnconst_node<T>&) =
        delete;

    mutable T* vector_base_;
    const std::size_t size_;
    std::vector<expression_ptr> initialiser_list_;
    T base_value_;
};

template <typename T>
class vector_init_iota_nconstconst_node final : public expression_node<T>
{
   public:
    using expression_ptr = expression_node<T>*;

    vector_init_iota_nconstconst_node(T* vector_base, const std::size_t& size,
                                      const std::vector<expression_ptr>& initialiser_list)
        : vector_base_(vector_base), size_(size), initialiser_list_(initialiser_list)
    {
        assert(valid());
    }

    inline T value() const override
    {
        T value = initialiser_list_[0]->value();
        const T increment = initialiser_list_[1]->value();

        for (std::size_t i = 0; i < size_; ++i, value += increment)
        {
            *(vector_base_ + i) = value;
        }

        return *(vector_base_);
    }

    inline typename expression_node<T>::node_type type() const override
    {
        return expression_node<T>::node_type::e_vecinit;
    }

    inline bool valid() const override
    {
        return vector_base_ && (initialiser_list_.size() == 2) &&
               (!details::is_constant_node(initialiser_list_[0])) &&
               (details::is_constant_node(initialiser_list_[1]));
    }

    void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) override
    {
        expression_node<T>::ndb_t::collect(initialiser_list_, node_delete_list);
    }

    std::size_t node_depth() const override
    {
        return expression_node<T>::ndb_t::compute_node_depth(initialiser_list_);
    }

   private:
    vector_init_iota_nconstconst_node(const vector_init_iota_nconstconst_node<T>&) = delete;
    vector_init_iota_nconstconst_node<T>& operator=(const vector_init_iota_nconstconst_node<T>&) =
        delete;

    mutable T* vector_base_;
    const std::size_t size_;
    std::vector<expression_ptr> initialiser_list_;
};

template <typename T>
class vector_init_iota_nconstnconst_node final : public expression_node<T>
{
   public:
    using expression_ptr = expression_node<T>*;

    vector_init_iota_nconstnconst_node(T* vector_base, const std::size_t& size,
                                       const std::vector<expression_ptr>& initialiser_list)
        : vector_base_(vector_base), size_(size), initialiser_list_(initialiser_list)
    {
        assert(valid());
    }

    inline T value() const override
    {
        T value = initialiser_list_[0]->value();
        expression_node<T>& increment = *initialiser_list_[1];

        for (std::size_t i = 0; i < size_; ++i, value += increment.value())
        {
            *(vector_base_ + i) = value;
        }

        return *(vector_base_);
    }

    inline typename expression_node<T>::node_type type() const override
    {
        return expression_node<T>::node_type::e_vecinit;
    }

    inline bool valid() const override
    {
        return vector_base_ && (initialiser_list_.size() == 2) &&
               (!details::is_constant_node(initialiser_list_[0])) &&
               (!details::is_constant_node(initialiser_list_[1]));
    }

    void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) override
    {
        expression_node<T>::ndb_t::collect(initialiser_list_, node_delete_list);
    }

    std::size_t node_depth() const override
    {
        return expression_node<T>::ndb_t::compute_node_depth(initialiser_list_);
    }

   private:
    vector_init_iota_nconstnconst_node(const vector_init_iota_nconstnconst_node<T>&) = delete;
    vector_init_iota_nconstnconst_node<T>& operator=(const vector_init_iota_nconstnconst_node<T>&) =
        delete;

    mutable T* vector_base_;
    const std::size_t size_;
    std::vector<expression_ptr> initialiser_list_;
};

template <typename T>
class swap_node final : public expression_node<T>
{
   public:
    using expression_ptr = expression_node<T>*;
    using variable_node_ptr = variable_node<T>*;

    swap_node(variable_node_ptr var0, variable_node_ptr var1) : var0_(var0), var1_(var1) {}

    inline T value() const override
    {
        std::swap(var0_->ref(), var1_->ref());
        return var1_->ref();
    }

    inline typename expression_node<T>::node_type type() const override
    {
        return expression_node<T>::node_type::e_swap;
    }

   private:
    variable_node_ptr var0_;
    variable_node_ptr var1_;
};

template <typename T>
class swap_generic_node final : public binary_node<T>
{
   public:
    using expression_ptr = expression_node<T>*;
    using ivariable_ptr = ivariable<T>*;

    swap_generic_node(expression_ptr var0, expression_ptr var1)
        : binary_node<T>(core::operators::operator_type::swap, var0, var1),
          var0_(dynamic_cast<ivariable_ptr>(var0)),
          var1_(dynamic_cast<ivariable_ptr>(var1))
    {
    }

    inline T value() const override
    {
        std::swap(var0_->ref(), var1_->ref());
        return var1_->ref();
    }

    inline typename expression_node<T>::node_type type() const override
    {
        return expression_node<T>::node_type::e_swap;
    }

   private:
    ivariable_ptr var0_;
    ivariable_ptr var1_;
};

template <typename T>
class swap_vecvec_node final : public binary_node<T>, public vector_interface<T>
{
   public:
    using expression_ptr = expression_node<T>*;
    using vector_node_ptr = vector_node<T>*;
    using vds_t = core::vec_data_store<T>;

    using binary_node<T>::branch;

    swap_vecvec_node(expression_ptr branch0, expression_ptr branch1)
        : binary_node<T>(core::operators::operator_type::swap, branch0, branch1),
          vec0_node_ptr_(0),
          vec1_node_ptr_(0),
          initialised_(false)
    {
        if (is_ivector_node(branch(0)))
        {
            vector_interface<T>* vi = nullptr;

            if (0 != (vi = dynamic_cast<vector_interface<T>*>(branch(0))))
            {
                vec0_node_ptr_ = vi->vec();
                vds() = vi->vds();
            }
        }

        if (is_ivector_node(branch(1)))
        {
            vector_interface<T>* vi = nullptr;

            if (0 != (vi = dynamic_cast<vector_interface<T>*>(branch(1))))
            {
                vec1_node_ptr_ = vi->vec();
            }
        }

        if (vec0_node_ptr_ && vec1_node_ptr_)
        {
            initialised_ = size() <= base_size();
        }

        assert(valid());
    }

    inline T value() const override
    {
        binary_node<T>::branch(0)->value();
        binary_node<T>::branch(1)->value();

        T* vec0 = vec0_node_ptr_->vds().data();
        T* vec1 = vec1_node_ptr_->vds().data();

        assert(size() <= base_size());
        const std::size_t n = size();

        for (std::size_t i = 0; i < n; ++i)
        {
            std::swap(vec0[i], vec1[i]);
        }

        return vec1_node_ptr_->value();
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
        return expression_node<T>::node_type::e_vecvecswap;
    }

    inline bool valid() const override
    {
        return initialised_ && binary_node<T>::valid();
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
    vds_t vds_;
};

template <typename T>
class assert_node final : public expression_node<T>
{
   public:
    using expression_ptr = expression_node<T>*;
    using branch_t = std::pair<expression_ptr, bool>;
    using str_base_ptr = string_base_node<T>*;
    using assert_context_t = assert_check::assert_context;

    assert_node(expression_ptr assert_condition_node, expression_ptr assert_message_node,
                assert_check_ptr assert_check, const assert_context_t& context)
        : assert_message_str_base_(0), assert_check_(assert_check), context_(context)
    {
        construct_branch_pair(assert_condition_node_, assert_condition_node);
        construct_branch_pair(assert_message_node_, assert_message_node);

#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
        if (assert_message_node_.first &&
            details::is_generally_string_node(assert_message_node_.first))
        {
            assert_message_str_base_ = dynamic_cast<str_base_ptr>(assert_message_node_.first);
        }
#endif

        assert(valid());
    }

    inline T value() const override
    {
        if (details::is_true(assert_condition_node_.first->value()))
        {
            return T(1);
        }

#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
        if (assert_message_node_.first)
        {
            assert_message_node_.first->value();
            assert(assert_message_str_base_);
            context_.message = assert_message_str_base_->str();
        }
#endif

        assert_check_->handle_assert(context_);
        return T(0);
    }

    inline typename expression_node<T>::node_type type() const override
    {
        return expression_node<T>::node_type::e_assert;
    }

    inline bool valid() const override
    {
        return (assert_check_ && assert_condition_node_.first &&
                assert_condition_node_.first->valid()) &&
               ((0 == assert_message_node_.first) ||
                (assert_message_node_.first && assert_message_str_base_ &&
                 assert_message_node_.first->valid() &&
                 details::is_generally_string_node(assert_message_node_.first)));
    }

    void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) override
    {
        expression_node<T>::ndb_t::collect(assert_condition_node_, node_delete_list);
        expression_node<T>::ndb_t::collect(assert_message_node_, node_delete_list);
    }

    std::size_t node_depth() const override
    {
        return expression_node<T>::ndb_t::compute_node_depth(assert_condition_node_,
                                                             assert_message_node_);
    }

   private:
    branch_t assert_condition_node_;
    branch_t assert_message_node_;
    str_base_ptr assert_message_str_base_;
    assert_check_ptr assert_check_;
    mutable assert_context_t context_;
};

template <typename T, std::size_t N>
inline T axn(const T a, const T x)
{
    // a*x^n
    return a * math_expr::core::numeric::fast_exp<T, N>::result(x);
}

template <typename T, std::size_t N>
inline T axnb(const T a, const T x, const T b)
{
    // a*x^n+b
    return a * math_expr::core::numeric::fast_exp<T, N>::result(x) + b;
}

template <typename T>
struct sf_base
{
    using Type = typename core::numeric::functor_t<T>::Type;
    using functor_t = typename core::numeric::functor_t<T>;
    using quaternary_functor_t = typename functor_t::qfunc_t;
    using trinary_functor_t = typename functor_t::tfunc_t;
    using binary_functor_t = typename functor_t::bfunc_t;
    using unary_functor_t = typename functor_t::ufunc_t;
};

// clang-format off
#define DEFINE_SFOP3(NN, OP0, OP1)                                                                 \
    template <typename T> struct sf##NN##_op : public sf_base<T>                                   \
    {                                                                                              \
        using Type = typename sf_base<T>::Type const;                                              \
        static inline T process(Type x, Type y, Type z)                                            \
        {                                                                                          \
            return (OP0);                                                                          \
        }                                                                                          \
        static inline std::string id()                                                             \
        {                                                                                          \
            return (OP1);                                                                          \
        }                                                                                          \
    };

      DEFINE_SFOP3(00,(x + y) / z       ,"(t+t)/t");
      DEFINE_SFOP3(01,(x + y) * z       ,"(t+t)*t");
      DEFINE_SFOP3(02,(x + y) - z       ,"(t+t)-t");
      DEFINE_SFOP3(03,(x + y) + z       ,"(t+t)+t");
      DEFINE_SFOP3(04,(x - y) + z       ,"(t-t)+t");
      DEFINE_SFOP3(05,(x - y) / z       ,"(t-t)/t");
      DEFINE_SFOP3(06,(x - y) * z       ,"(t-t)*t");
      DEFINE_SFOP3(07,(x * y) + z       ,"(t*t)+t");
      DEFINE_SFOP3(08,(x * y) - z       ,"(t*t)-t");
      DEFINE_SFOP3(09,(x * y) / z       ,"(t*t)/t");
      DEFINE_SFOP3(10,(x * y) * z       ,"(t*t)*t");
      DEFINE_SFOP3(11,(x / y) + z       ,"(t/t)+t");
      DEFINE_SFOP3(12,(x / y) - z       ,"(t/t)-t");
      DEFINE_SFOP3(13,(x / y) / z       ,"(t/t)/t");
      DEFINE_SFOP3(14,(x / y) * z       ,"(t/t)*t");
      DEFINE_SFOP3(15,x / (y + z)       ,"t/(t+t)");
      DEFINE_SFOP3(16,x / (y - z)       ,"t/(t-t)");
      DEFINE_SFOP3(17,x / (y * z)       ,"t/(t*t)");
      DEFINE_SFOP3(18,x / (y / z)       ,"t/(t/t)");
      DEFINE_SFOP3(19,x * (y + z)       ,"t*(t+t)");
      DEFINE_SFOP3(20,x * (y - z)       ,"t*(t-t)");
      DEFINE_SFOP3(21,x * (y * z)       ,"t*(t*t)");
      DEFINE_SFOP3(22,x * (y / z)       ,"t*(t/t)");
      DEFINE_SFOP3(23,x - (y + z)       ,"t-(t+t)");
      DEFINE_SFOP3(24,x - (y - z)       ,"t-(t-t)");
      DEFINE_SFOP3(25,x - (y / z)       ,"t-(t/t)");
      DEFINE_SFOP3(26,x - (y * z)       ,"t-(t*t)");
      DEFINE_SFOP3(27,x + (y * z)       ,"t+(t*t)");
      DEFINE_SFOP3(28,x + (y / z)       ,"t+(t/t)");
      DEFINE_SFOP3(29,x + (y + z)       ,"t+(t+t)");
      DEFINE_SFOP3(30,x + (y - z)       ,"t+(t-t)");
      DEFINE_SFOP3(31,(axnb<T,2>(x,y,z)),"       ");
      DEFINE_SFOP3(32,(axnb<T,3>(x,y,z)),"       ");
      DEFINE_SFOP3(33,(axnb<T,4>(x,y,z)),"       ");
      DEFINE_SFOP3(34,(axnb<T,5>(x,y,z)),"       ");
      DEFINE_SFOP3(35,(axnb<T,6>(x,y,z)),"       ");
      DEFINE_SFOP3(36,(axnb<T,7>(x,y,z)),"       ");
      DEFINE_SFOP3(37,(axnb<T,8>(x,y,z)),"       ");
      DEFINE_SFOP3(38,(axnb<T,9>(x,y,z)),"       ");
      DEFINE_SFOP3(39,x * core::numeric::log(y)   + z,"");
      DEFINE_SFOP3(40,x * core::numeric::log(y)   - z,"");
      DEFINE_SFOP3(41,x * core::numeric::log10(y) + z,"");
      DEFINE_SFOP3(42,x * core::numeric::log10(y) - z,"");
      DEFINE_SFOP3(43,x * core::numeric::sin(y) + z  ,"");
      DEFINE_SFOP3(44,x * core::numeric::sin(y) - z  ,"");
      DEFINE_SFOP3(45,x * core::numeric::cos(y) + z  ,"");
      DEFINE_SFOP3(46,x * core::numeric::cos(y) - z  ,"");
      DEFINE_SFOP3(47,details::is_true(x) ? y : z,"");

#define DEFINE_SFOP4(NN, OP0, OP1)                                                                 \
    template <typename T> struct sf##NN##_op : public sf_base<T>                                   \
    {                                                                                              \
        using Type = typename sf_base<T>::Type const;                                              \
        static inline T process(Type x, Type y, Type z, Type w)                                    \
        {                                                                                          \
            return (OP0);                                                                          \
        }                                                                                          \
        static inline std::string id()                                                             \
        {                                                                                          \
            return (OP1);                                                                          \
        }                                                                                          \
    }

      DEFINE_SFOP4(48,(x + ((y + z) / w)),"t+((t+t)/t)");
      DEFINE_SFOP4(49,(x + ((y + z) * w)),"t+((t+t)*t)");
      DEFINE_SFOP4(50,(x + ((y - z) / w)),"t+((t-t)/t)");
      DEFINE_SFOP4(51,(x + ((y - z) * w)),"t+((t-t)*t)");
      DEFINE_SFOP4(52,(x + ((y * z) / w)),"t+((t*t)/t)");
      DEFINE_SFOP4(53,(x + ((y * z) * w)),"t+((t*t)*t)");
      DEFINE_SFOP4(54,(x + ((y / z) + w)),"t+((t/t)+t)");
      DEFINE_SFOP4(55,(x + ((y / z) / w)),"t+((t/t)/t)");
      DEFINE_SFOP4(56,(x + ((y / z) * w)),"t+((t/t)*t)");
      DEFINE_SFOP4(57,(x - ((y + z) / w)),"t-((t+t)/t)");
      DEFINE_SFOP4(58,(x - ((y + z) * w)),"t-((t+t)*t)");
      DEFINE_SFOP4(59,(x - ((y - z) / w)),"t-((t-t)/t)");
      DEFINE_SFOP4(60,(x - ((y - z) * w)),"t-((t-t)*t)");
      DEFINE_SFOP4(61,(x - ((y * z) / w)),"t-((t*t)/t)");
      DEFINE_SFOP4(62,(x - ((y * z) * w)),"t-((t*t)*t)");
      DEFINE_SFOP4(63,(x - ((y / z) / w)),"t-((t/t)/t)");
      DEFINE_SFOP4(64,(x - ((y / z) * w)),"t-((t/t)*t)");
      DEFINE_SFOP4(65,(((x + y) * z) - w),"((t+t)*t)-t");
      DEFINE_SFOP4(66,(((x - y) * z) - w),"((t-t)*t)-t");
      DEFINE_SFOP4(67,(((x * y) * z) - w),"((t*t)*t)-t");
      DEFINE_SFOP4(68,(((x / y) * z) - w),"((t/t)*t)-t");
      DEFINE_SFOP4(69,(((x + y) / z) - w),"((t+t)/t)-t");
      DEFINE_SFOP4(70,(((x - y) / z) - w),"((t-t)/t)-t");
      DEFINE_SFOP4(71,(((x * y) / z) - w),"((t*t)/t)-t");
      DEFINE_SFOP4(72,(((x / y) / z) - w),"((t/t)/t)-t");
      DEFINE_SFOP4(73,((x * y) + (z * w)),"(t*t)+(t*t)");
      DEFINE_SFOP4(74,((x * y) - (z * w)),"(t*t)-(t*t)");
      DEFINE_SFOP4(75,((x * y) + (z / w)),"(t*t)+(t/t)");
      DEFINE_SFOP4(76,((x * y) - (z / w)),"(t*t)-(t/t)");
      DEFINE_SFOP4(77,((x / y) + (z / w)),"(t/t)+(t/t)");
      DEFINE_SFOP4(78,((x / y) - (z / w)),"(t/t)-(t/t)");
      DEFINE_SFOP4(79,((x / y) - (z * w)),"(t/t)-(t*t)");
      DEFINE_SFOP4(80,(x / (y + (z * w))),"t/(t+(t*t))");
      DEFINE_SFOP4(81,(x / (y - (z * w))),"t/(t-(t*t))");
      DEFINE_SFOP4(82,(x * (y + (z * w))),"t*(t+(t*t))");
      DEFINE_SFOP4(83,(x * (y - (z * w))),"t*(t-(t*t))");

      DEFINE_SFOP4(84,(axn<T,2>(x,y) + axn<T,2>(z,w)),"");
      DEFINE_SFOP4(85,(axn<T,3>(x,y) + axn<T,3>(z,w)),"");
      DEFINE_SFOP4(86,(axn<T,4>(x,y) + axn<T,4>(z,w)),"");
      DEFINE_SFOP4(87,(axn<T,5>(x,y) + axn<T,5>(z,w)),"");
      DEFINE_SFOP4(88,(axn<T,6>(x,y) + axn<T,6>(z,w)),"");
      DEFINE_SFOP4(89,(axn<T,7>(x,y) + axn<T,7>(z,w)),"");
      DEFINE_SFOP4(90,(axn<T,8>(x,y) + axn<T,8>(z,w)),"");
      DEFINE_SFOP4(91,(axn<T,9>(x,y) + axn<T,9>(z,w)),"");
      DEFINE_SFOP4(92,((details::is_true(x) && details::is_true(y)) ? z : w),"");
      DEFINE_SFOP4(93,((details::is_true(x) || details::is_true(y)) ? z : w),"");
      DEFINE_SFOP4(94,((x <  y) ? z : w),"");
      DEFINE_SFOP4(95,((x <= y) ? z : w),"");
      DEFINE_SFOP4(96,((x >  y) ? z : w),"");
      DEFINE_SFOP4(97,((x >= y) ? z : w),"");
      DEFINE_SFOP4(98,(details::is_true(core::numeric::equal(x,y)) ? z : w),"");
      DEFINE_SFOP4(99,(x * core::numeric::sin(y) + z * core::numeric::cos(w)),"");

      DEFINE_SFOP4(ext00,((x + y) - (z * w)),"(t+t)-(t*t)");
      DEFINE_SFOP4(ext01,((x + y) - (z / w)),"(t+t)-(t/t)");
      DEFINE_SFOP4(ext02,((x + y) + (z * w)),"(t+t)+(t*t)");
      DEFINE_SFOP4(ext03,((x + y) + (z / w)),"(t+t)+(t/t)");
      DEFINE_SFOP4(ext04,((x - y) + (z * w)),"(t-t)+(t*t)");
      DEFINE_SFOP4(ext05,((x - y) + (z / w)),"(t-t)+(t/t)");
      DEFINE_SFOP4(ext06,((x - y) - (z * w)),"(t-t)-(t*t)");
      DEFINE_SFOP4(ext07,((x - y) - (z / w)),"(t-t)-(t/t)");
      DEFINE_SFOP4(ext08,((x + y) - (z - w)),"(t+t)-(t-t)");
      DEFINE_SFOP4(ext09,((x + y) + (z - w)),"(t+t)+(t-t)");
      DEFINE_SFOP4(ext10,((x + y) + (z + w)),"(t+t)+(t+t)");
      DEFINE_SFOP4(ext11,((x + y) * (z - w)),"(t+t)*(t-t)");
      DEFINE_SFOP4(ext12,((x + y) / (z - w)),"(t+t)/(t-t)");
      DEFINE_SFOP4(ext13,((x - y) - (z + w)),"(t-t)-(t+t)");
      DEFINE_SFOP4(ext14,((x - y) + (z + w)),"(t-t)+(t+t)");
      DEFINE_SFOP4(ext15,((x - y) * (z + w)),"(t-t)*(t+t)");
      DEFINE_SFOP4(ext16,((x - y) / (z + w)),"(t-t)/(t+t)");
      DEFINE_SFOP4(ext17,((x * y) - (z + w)),"(t*t)-(t+t)");
      DEFINE_SFOP4(ext18,((x / y) - (z + w)),"(t/t)-(t+t)");
      DEFINE_SFOP4(ext19,((x * y) + (z + w)),"(t*t)+(t+t)");
      DEFINE_SFOP4(ext20,((x / y) + (z + w)),"(t/t)+(t+t)");
      DEFINE_SFOP4(ext21,((x * y) + (z - w)),"(t*t)+(t-t)");
      DEFINE_SFOP4(ext22,((x / y) + (z - w)),"(t/t)+(t-t)");
      DEFINE_SFOP4(ext23,((x * y) - (z - w)),"(t*t)-(t-t)");
      DEFINE_SFOP4(ext24,((x / y) - (z - w)),"(t/t)-(t-t)");
      DEFINE_SFOP4(ext25,((x + y) * (z * w)),"(t+t)*(t*t)");
      DEFINE_SFOP4(ext26,((x + y) * (z / w)),"(t+t)*(t/t)");
      DEFINE_SFOP4(ext27,((x + y) / (z * w)),"(t+t)/(t*t)");
      DEFINE_SFOP4(ext28,((x + y) / (z / w)),"(t+t)/(t/t)");
      DEFINE_SFOP4(ext29,((x - y) / (z * w)),"(t-t)/(t*t)");
      DEFINE_SFOP4(ext30,((x - y) / (z / w)),"(t-t)/(t/t)");
      DEFINE_SFOP4(ext31,((x - y) * (z * w)),"(t-t)*(t*t)");
      DEFINE_SFOP4(ext32,((x - y) * (z / w)),"(t-t)*(t/t)");
      DEFINE_SFOP4(ext33,((x * y) * (z + w)),"(t*t)*(t+t)");
      DEFINE_SFOP4(ext34,((x / y) * (z + w)),"(t/t)*(t+t)");
      DEFINE_SFOP4(ext35,((x * y) / (z + w)),"(t*t)/(t+t)");
      DEFINE_SFOP4(ext36,((x / y) / (z + w)),"(t/t)/(t+t)");
      DEFINE_SFOP4(ext37,((x * y) / (z - w)),"(t*t)/(t-t)");
      DEFINE_SFOP4(ext38,((x / y) / (z - w)),"(t/t)/(t-t)");
      DEFINE_SFOP4(ext39,((x * y) * (z - w)),"(t*t)*(t-t)");
      DEFINE_SFOP4(ext40,((x * y) / (z * w)),"(t*t)/(t*t)");
      DEFINE_SFOP4(ext41,((x / y) * (z / w)),"(t/t)*(t/t)");
      DEFINE_SFOP4(ext42,((x / y) * (z - w)),"(t/t)*(t-t)");
      DEFINE_SFOP4(ext43,((x * y) * (z * w)),"(t*t)*(t*t)");
      DEFINE_SFOP4(ext44,(x + (y * (z / w))),"t+(t*(t/t))");
      DEFINE_SFOP4(ext45,(x - (y * (z / w))),"t-(t*(t/t))");
      DEFINE_SFOP4(ext46,(x + (y / (z * w))),"t+(t/(t*t))");
      DEFINE_SFOP4(ext47,(x - (y / (z * w))),"t-(t/(t*t))");
      DEFINE_SFOP4(ext48,(((x - y) - z) * w),"((t-t)-t)*t");
      DEFINE_SFOP4(ext49,(((x - y) - z) / w),"((t-t)-t)/t");
      DEFINE_SFOP4(ext50,(((x - y) + z) * w),"((t-t)+t)*t");
      DEFINE_SFOP4(ext51,(((x - y) + z) / w),"((t-t)+t)/t");
      DEFINE_SFOP4(ext52,((x + (y - z)) * w),"(t+(t-t))*t");
      DEFINE_SFOP4(ext53,((x + (y - z)) / w),"(t+(t-t))/t");
      DEFINE_SFOP4(ext54,((x + y) / (z + w)),"(t+t)/(t+t)");
      DEFINE_SFOP4(ext55,((x - y) / (z - w)),"(t-t)/(t-t)");
      DEFINE_SFOP4(ext56,((x + y) * (z + w)),"(t+t)*(t+t)");
      DEFINE_SFOP4(ext57,((x - y) * (z - w)),"(t-t)*(t-t)");
      DEFINE_SFOP4(ext58,((x - y) + (z - w)),"(t-t)+(t-t)");
      DEFINE_SFOP4(ext59,((x - y) - (z - w)),"(t-t)-(t-t)");
      DEFINE_SFOP4(ext60,((x / y) + (z * w)),"(t/t)+(t*t)");
      DEFINE_SFOP4(ext61,(((x * y) * z) / w),"((t*t)*t)/t");

#undef DEFINE_SFOP3
#undef DEFINE_SFOP4
// clang-format on

template <typename T, typename SpecialFunction>
class sf3_node final : public trinary_node<T>
{
   public:
    using expression_ptr = expression_node<T>*;

    sf3_node(const core::operators::operator_type& opr, expression_ptr branch0,
             expression_ptr branch1, expression_ptr branch2)
        : trinary_node<T>(opr, branch0, branch1, branch2)
    {
    }

    inline T value() const override
    {
        const T x = trinary_node<T>::branch_[0].first->value();
        const T y = trinary_node<T>::branch_[1].first->value();
        const T z = trinary_node<T>::branch_[2].first->value();

        return SpecialFunction::process(x, y, z);
    }
};

template <typename T, typename SpecialFunction>
class sf4_node final : public quaternary_node<T>
{
   public:
    using expression_ptr = expression_node<T>*;

    sf4_node(const core::operators::operator_type& opr, expression_ptr branch0,
             expression_ptr branch1, expression_ptr branch2, expression_ptr branch3)
        : quaternary_node<T>(opr, branch0, branch1, branch2, branch3)
    {
    }

    inline T value() const override
    {
        const T x = quaternary_node<T>::branch_[0].first->value();
        const T y = quaternary_node<T>::branch_[1].first->value();
        const T z = quaternary_node<T>::branch_[2].first->value();
        const T w = quaternary_node<T>::branch_[3].first->value();

        return SpecialFunction::process(x, y, z, w);
    }
};

}  // namespace math_expr::details

#endif
