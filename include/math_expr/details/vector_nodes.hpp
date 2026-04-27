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

#ifndef MATH_EXPR_DETAILS_VECTOR_NODES_HPP
#define MATH_EXPR_DETAILS_VECTOR_NODES_HPP

namespace math_expr::details
{
      template <typename T>
      class variable_node final
                          : public expression_node<T>
                          , public ivariable      <T>
      {
      public:

         static T null_value;

         explicit variable_node()
         : value_(&null_value)
         {}

         explicit variable_node(T& v)
         : value_(&v)
         {}

         inline bool operator <(const variable_node<T>& v) const
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
            return expression_node<T>::e_variable;
         }

      private:

         T* value_;
      };

      template <typename T>
      T variable_node<T>::null_value = T(std::numeric_limits<T>::quiet_NaN());

      template <typename T>
      struct range_pack
      {
         typedef expression_node<T>*           expression_node_ptr;
         typedef std::pair<std::size_t,std::size_t> cached_range_t;

         range_pack()
         : n0_e (std::make_pair(false,expression_node_ptr(0)))
         , n1_e (std::make_pair(false,expression_node_ptr(0)))
         , n0_c (std::make_pair(false,0))
         , n1_c (std::make_pair(false,0))
         , cache(std::make_pair(0,0))
         {}

         void clear()
         {
            n0_e  = std::make_pair(false,expression_node_ptr(0));
            n1_e  = std::make_pair(false,expression_node_ptr(0));
            n0_c  = std::make_pair(false,0);
            n1_c  = std::make_pair(false,0);
            cache = std::make_pair(0,0);
         }

         void free()
         {
            if (n0_e.first && n0_e.second)
            {
               n0_e.first = false;

               if (
                    !is_variable_node(n0_e.second) &&
                    !is_string_node  (n0_e.second)
                  )
               {
                  destroy_node(n0_e.second);
               }
            }

            if (n1_e.first && n1_e.second)
            {
               n1_e.first = false;

               if (
                    !is_variable_node(n1_e.second) &&
                    !is_string_node  (n1_e.second)
                  )
               {
                  destroy_node(n1_e.second);
               }
            }
         }

         bool const_range() const
         {
           return ( n0_c.first &&  n1_c.first) &&
                  (!n0_e.first && !n1_e.first);
         }

         bool var_range() const
         {
           return ( n0_e.first &&  n1_e.first) &&
                  (!n0_c.first && !n1_c.first);
         }

         bool operator() (std::size_t& r0, std::size_t& r1,
                          const std::size_t& size = std::numeric_limits<std::size_t>::max()) const
         {
            if (n0_c.first)
               r0 = n0_c.second;
            else if (n0_e.first)
            {
               r0 = static_cast<std::size_t>(details::numeric::to_int64(n0_e.second->value()));
            }
            else
               return false;

            if (n1_c.first)
               r1 = n1_c.second;
            else if (n1_e.first)
            {
               r1 = static_cast<std::size_t>(details::numeric::to_int64(n1_e.second->value()));
            }
            else
               return false;

            if (
                 (std::numeric_limits<std::size_t>::max() != size) &&
                 (std::numeric_limits<std::size_t>::max() == r1  )
               )
            {
               r1 = size;
            }

            cache.first  = r0;
            cache.second = r1;

            #ifndef math_expr_enable_range_runtime_checks
            return (r0 <= r1);
            #else
            return range_runtime_check(r0, r1, size);
            #endif
         }

         inline std::size_t const_size() const
         {
            return (n1_c.second - n0_c.second);
         }

         inline std::size_t cache_size() const
         {
            return (cache.second - cache.first);
         }

         std::pair<bool,expression_node_ptr> n0_e;
         std::pair<bool,expression_node_ptr> n1_e;
         std::pair<bool,std::size_t        > n0_c;
         std::pair<bool,std::size_t        > n1_c;
         mutable cached_range_t             cache;

         #ifdef math_expr_enable_range_runtime_checks
         bool range_runtime_check(const std::size_t r0,
                                  const std::size_t r1,
                                  const std::size_t size) const
         {
            if (r0 > size)
            {
               throw std::runtime_error("range error: (r0 < 0) || (r0 > size)");
               #if !defined(_MSC_VER) && !defined(__NVCOMPILER)
               return false;
               #endif
            }

            if (r1 > size)
            {
               throw std::runtime_error("range error: (r1 < 0) || (r1 > size)");
               #if !defined(_MSC_VER) && !defined(__NVCOMPILER)
               return false;
               #endif
            }

            return (r0 <= r1);
         }
         #endif
      };

      template <typename T>
      class string_base_node;

      template <typename T>
      struct range_data_type
      {
         typedef range_pack<T> range_t;
         typedef string_base_node<T>* strbase_ptr_t;

         range_data_type()
         : range(0)
         , data (0)
         , size (0)
         , type_size(0)
         , str_node (0)
         {}

         range_t*      range;
         void*         data;
         std::size_t   size;
         std::size_t   type_size;
         strbase_ptr_t str_node;
      };

      template <typename T> class vector_node;

      template <typename T>
      class vector_interface
      {
      public:

         typedef vector_node<T>*   vector_node_ptr;
         typedef vec_data_store<T> vds_t;

         virtual ~vector_interface()
         {}

         virtual std::size_t size     () const = 0;

         virtual std::size_t base_size() const = 0;

         virtual vector_node_ptr vec  () const = 0;

         virtual vector_node_ptr vec  ()       = 0;

         virtual       vds_t& vds     ()       = 0;

         virtual const vds_t& vds     () const = 0;

         virtual bool side_effect     () const { return false; }
      };

      template <typename T>
      class vector_node final
                        : public expression_node <T>
                        , public vector_interface<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef vector_holder<T>    vector_holder_t;
         typedef vector_node<T>*     vector_node_ptr;
         typedef vec_data_store<T>   vds_t;

         explicit vector_node(vector_holder_t* vh)
         : vector_holder_(vh)
         , vds_((*vector_holder_).size(),(*vector_holder_)[0])
         {
            vector_holder_->set_ref(&vds_.ref());
         }

         vector_node(const vds_t& vds, vector_holder_t* vh)
         : vector_holder_(vh)
         , vds_(vds)
         {}

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
            return expression_node<T>::e_vector;
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
         vds_t                      vds_;
      };

      template <typename T>
      class vector_size_node final
                        : public expression_node <T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef vector_holder<T>    vector_holder_t;

         explicit vector_size_node(vector_holder_t* vh)
         : vector_holder_(vh)
         {}

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
            return expression_node<T>::e_vecsize;
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
      class vector_elem_node final
                             : public expression_node<T>
                             , public ivariable      <T>
      {
      public:

         typedef expression_node<T>*            expression_ptr;
         typedef vector_holder<T>               vector_holder_t;
         typedef vector_holder_t*               vector_holder_ptr;
         typedef std::pair<expression_ptr,bool> branch_t;

         vector_elem_node(expression_ptr vec_node,
                          expression_ptr index,
                          vector_holder_ptr vec_holder)
         : vector_holder_(vec_holder)
         , vector_base_((*vec_holder)[0])
         {
            construct_branch_pair(vector_node_, vec_node);
            construct_branch_pair(index_      , index   );
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
            return expression_node<T>::e_vecelem;
         }

         inline bool valid() const override
         {
            return
               vector_holder_        &&
               index_.first          &&
               vector_node_.first    &&
               index_.first->valid() &&
               vector_node_.first->valid();
         }

         inline vector_holder_t& vec_holder()
         {
            return (*vector_holder_);
         }

         void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) override
         {
            expression_node<T>::ndb_t::collect(vector_node_, node_delete_list);
            expression_node<T>::ndb_t::collect(index_      , node_delete_list);
         }

         std::size_t node_depth() const override
         {
            return expression_node<T>::ndb_t::compute_node_depth
               (vector_node_, index_);
         }

      private:

         inline T* access_vector() const
         {
            vector_node_.first->value();
            return (vector_base_ + details::numeric::to_uint64(index_.first->value()));
         }

         vector_holder_ptr vector_holder_;
         T* vector_base_;
         branch_t vector_node_;
         branch_t index_;
      };

      template <typename T>
      class vector_celem_node final
                              : public expression_node<T>
                              , public ivariable      <T>
      {
      public:

         typedef expression_node<T>*            expression_ptr;
         typedef vector_holder<T>               vector_holder_t;
         typedef vector_holder_t*               vector_holder_ptr;
         typedef std::pair<expression_ptr,bool> branch_t;

         vector_celem_node(expression_ptr vec_node,
                           const std::size_t index,
                           vector_holder_ptr vec_holder)
         : index_(index)
         , vector_holder_(vec_holder)
         , vector_base_((*vec_holder)[0])
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
            return expression_node<T>::e_veccelem;
         }

         inline bool valid() const override
         {
            return
               vector_holder_     &&
               vector_node_.first &&
               vector_node_.first->valid();
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
      class vector_elem_rtc_node final
                                 : public expression_node<T>
                                 , public ivariable      <T>
      {
      public:

         typedef expression_node<T>*            expression_ptr;
         typedef vector_holder<T>               vector_holder_t;
         typedef vector_holder_t*               vector_holder_ptr;
         typedef std::pair<expression_ptr,bool> branch_t;

         vector_elem_rtc_node(expression_ptr vec_node,
                              expression_ptr index,
                              vector_holder_ptr vec_holder,
                              vector_access_runtime_check_ptr vec_rt_chk)
         : vector_holder_(vec_holder)
         , vector_base_((*vec_holder)[0])
         , vec_rt_chk_(vec_rt_chk)
         , max_vector_index_(vector_holder_->size() - 1)
         {
            construct_branch_pair(vector_node_, vec_node);
            construct_branch_pair(index_      , index   );
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
            return expression_node<T>::e_vecelemrtc;
         }

         inline bool valid() const override
         {
            return
               vector_holder_        &&
               index_.first          &&
               vector_node_.first    &&
               index_.first->valid() &&
               vector_node_.first->valid();
         }

         inline vector_holder_t& vec_holder()
         {
            return (*vector_holder_);
         }

         void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) override
         {
            expression_node<T>::ndb_t::collect(vector_node_, node_delete_list);
            expression_node<T>::ndb_t::collect(index_,       node_delete_list);
         }

         std::size_t node_depth() const override
         {
            return expression_node<T>::ndb_t::compute_node_depth
               (vector_node_, index_);
         }

      private:

         inline T* access_vector() const
         {
            const _uint64_t index = details::numeric::to_uint64(index_.first->value());
            vector_node_.first->value();

            if (index <= max_vector_index_)
            {
               return (vector_holder_->data() + index);
            }

            assert(vec_rt_chk_);

            vector_access_runtime_check::violation_context context;
            context.base_ptr   = reinterpret_cast<void*>(vector_base_);
            context.end_ptr    = reinterpret_cast<void*>(vector_base_ + vector_holder_->size());
            context.access_ptr = reinterpret_cast<void*>(vector_base_ + index);
            context.type_size  = sizeof(T);

            return vec_rt_chk_->handle_runtime_violation(context) ?
               reinterpret_cast<T*>(context.access_ptr) :
               vector_base_ ;
         }

         vector_holder_ptr vector_holder_;
         T*                vector_base_;
         branch_t          vector_node_;
         branch_t          index_;
         vector_access_runtime_check_ptr vec_rt_chk_;
         const std::size_t max_vector_index_;
      };

      template <typename T>
      class vector_celem_rtc_node final
                                 : public expression_node<T>
                                 , public ivariable      <T>
      {
      public:

         typedef expression_node<T>*            expression_ptr;
         typedef vector_holder<T>               vector_holder_t;
         typedef vector_holder_t*               vector_holder_ptr;
         typedef std::pair<expression_ptr,bool> branch_t;

         vector_celem_rtc_node(expression_ptr vec_node,
                               const std::size_t index,
                               vector_holder_ptr vec_holder,
                               vector_access_runtime_check_ptr vec_rt_chk)
         : index_(index)
         , max_vector_index_(vec_holder->size() - 1)
         , vector_holder_(vec_holder)
         , vector_base_((*vec_holder)[0])
         , vec_rt_chk_(vec_rt_chk)
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
            return expression_node<T>::e_veccelemrtc;
         }

         inline bool valid() const override
         {
            return
               vector_holder_     &&
               vector_node_.first &&
               vector_node_.first->valid();
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

            vector_access_runtime_check::violation_context context;
            context.base_ptr   = reinterpret_cast<void*>(vector_base_);
            context.end_ptr    = reinterpret_cast<void*>(vector_base_ + vector_holder_->size());
            context.access_ptr = reinterpret_cast<void*>(vector_base_ + index_);
            context.type_size  = sizeof(T);

            return vec_rt_chk_->handle_runtime_violation(context) ?
               reinterpret_cast<T*>(context.access_ptr) :
               vector_base_ ;
         }

         const std::size_t index_;
         const std::size_t max_vector_index_;
         vector_holder_ptr vector_holder_;
         T*                vector_base_;
         branch_t          vector_node_;
         vector_access_runtime_check_ptr vec_rt_chk_;
      };

      template <typename T>
      class rebasevector_elem_node final
                                   : public expression_node<T>
                                   , public ivariable      <T>
      {
      public:

         typedef expression_node<T>*            expression_ptr;
         typedef vector_holder<T>               vector_holder_t;
         typedef vector_holder_t*               vector_holder_ptr;
         typedef vec_data_store<T>              vds_t;
         typedef std::pair<expression_ptr,bool> branch_t;

         rebasevector_elem_node(expression_ptr vec_node,
                                expression_ptr index,
                                vector_holder_ptr vec_holder)
         : vector_holder_(vec_holder)
         {
            construct_branch_pair(vector_node_, vec_node);
            construct_branch_pair(index_      , index   );
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
            return expression_node<T>::e_rbvecelem;
         }

         inline bool valid() const override
         {
            return
               vector_holder_        &&
               index_.first          &&
               vector_node_.first    &&
               index_.first->valid() &&
               vector_node_.first->valid();
         }

         inline vector_holder_t& vec_holder()
         {
            return (*vector_holder_);
         }

         void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) override
         {
            expression_node<T>::ndb_t::collect(vector_node_, node_delete_list);
            expression_node<T>::ndb_t::collect(index_,       node_delete_list);
         }

         std::size_t node_depth() const override
         {
            return expression_node<T>::ndb_t::compute_node_depth
               (vector_node_, index_);
         }

      private:

         inline T* access_vector() const
         {
            vector_node_.first->value();
            return (vector_holder_->data() + details::numeric::to_uint64(index_.first->value()));
         }

         vector_holder_ptr vector_holder_;
         branch_t          vector_node_;
         branch_t          index_;
      };

      template <typename T>
      class rebasevector_celem_node final
                                    : public expression_node<T>
                                    , public ivariable      <T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef vector_holder<T>    vector_holder_t;
         typedef vector_holder_t*    vector_holder_ptr;
         typedef std::pair<expression_ptr,bool> branch_t;

         rebasevector_celem_node(expression_ptr vec_node,
                                 const std::size_t index,
                                 vector_holder_ptr vec_holder)
         : index_(index)
         , vector_holder_(vec_holder)
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
            return expression_node<T>::e_rbveccelem;
         }

         inline bool valid() const override
         {
            return
               vector_holder_     &&
               vector_node_.first &&
               vector_node_.first->valid();
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
         branch_t          vector_node_;
      };

      template <typename T>
      class rebasevector_elem_rtc_node final
                                       : public expression_node<T>
                                       , public ivariable      <T>
      {
      public:

         typedef expression_node<T>*            expression_ptr;
         typedef vector_holder<T>               vector_holder_t;
         typedef vector_holder_t*               vector_holder_ptr;
         typedef std::pair<expression_ptr,bool> branch_t;

         rebasevector_elem_rtc_node(expression_ptr vec_node,
                                    expression_ptr index,
                                    vector_holder_ptr vec_holder,
                                    vector_access_runtime_check_ptr vec_rt_chk)
         : vector_holder_(vec_holder)
         , vec_rt_chk_(vec_rt_chk)
         {
            construct_branch_pair(vector_node_, vec_node);
            construct_branch_pair(index_      , index   );
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
            return expression_node<T>::e_rbvecelemrtc;
         }

         inline bool valid() const override
         {
            return
               vector_holder_        &&
               index_.first          &&
               vector_node_.first    &&
               index_.first->valid() &&
               vector_node_.first->valid();
         }

         inline vector_holder_t& vec_holder()
         {
            return (*vector_holder_);
         }

         void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) override
         {
            expression_node<T>::ndb_t::collect(vector_node_, node_delete_list);
            expression_node<T>::ndb_t::collect(index_      , node_delete_list);
         }

         std::size_t node_depth() const override
         {
            return expression_node<T>::ndb_t::compute_node_depth
               (vector_node_, index_);
         }

      private:

         inline T* access_vector() const
         {
            vector_node_.first->value();
            const _uint64_t index = details::numeric::to_uint64(index_.first->value());

            if (index <= (vector_holder_->size() - 1))
            {
               return (vector_holder_->data() + index);
            }

            assert(vec_rt_chk_);

            vector_access_runtime_check::violation_context context;
            context.base_ptr   = reinterpret_cast<void*>(vector_holder_->data());
            context.end_ptr    = reinterpret_cast<void*>(vector_holder_->data() + vector_holder_->size());
            context.access_ptr = reinterpret_cast<void*>(vector_holder_->data() + index);
            context.type_size  = sizeof(T);

            return vec_rt_chk_->handle_runtime_violation(context) ?
                   reinterpret_cast<T*>(context.access_ptr) :
                   vector_holder_->data() ;
         }

         vector_holder_ptr vector_holder_;
         branch_t          vector_node_;
         branch_t          index_;
         vector_access_runtime_check_ptr vec_rt_chk_;
      };

      template <typename T>
      class rebasevector_celem_rtc_node final
                                    : public expression_node<T>
                                    , public ivariable      <T>
      {
      public:

         typedef expression_node<T>*            expression_ptr;
         typedef vector_holder<T>               vector_holder_t;
         typedef vector_holder_t*               vector_holder_ptr;
         typedef std::pair<expression_ptr,bool> branch_t;

         rebasevector_celem_rtc_node(expression_ptr vec_node,
                                     const std::size_t index,
                                     vector_holder_ptr vec_holder,
                                     vector_access_runtime_check_ptr vec_rt_chk)
         : index_(index)
         , vector_holder_(vec_holder)
         , vector_base_((*vec_holder)[0])
         , vec_rt_chk_(vec_rt_chk)
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
            return expression_node<T>::e_rbveccelemrtc;
         }

         inline bool valid() const override
         {
            return
               vector_holder_     &&
               vector_node_.first &&
               vector_node_.first->valid();
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

            vector_access_runtime_check::violation_context context;
            context.base_ptr   = reinterpret_cast<void*>(vector_base_);
            context.end_ptr    = reinterpret_cast<void*>(vector_base_ + vector_holder_->size());
            context.access_ptr = reinterpret_cast<void*>(vector_base_ + index_);
            context.type_size  = sizeof(T);

            return vec_rt_chk_->handle_runtime_violation(context) ?
               reinterpret_cast<T*>(context.access_ptr) :
               vector_base_ ;
         }

         const std::size_t index_;
         vector_holder_ptr vector_holder_;
         T*                vector_base_;
         branch_t          vector_node_;
         vector_access_runtime_check_ptr vec_rt_chk_;
      };

      template <typename T>
      class vector_initialisation_node final : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;

         vector_initialisation_node(T* vector_base,
                                    const std::size_t& size,
                                    const std::vector<expression_ptr>& initialiser_list,
                                    const bool single_value_initialse)
         : vector_base_(vector_base)
         , initialiser_list_(initialiser_list)
         , size_(size)
         , single_value_initialse_(single_value_initialse)
         , zero_value_initialse_(false)
         , const_nonzero_literal_value_initialse_(false)
         , single_initialiser_value_(T(0))
         {
            if (single_value_initialse_)
            {
               if (initialiser_list_.empty())
                  zero_value_initialse_ = true;
               else if (
                         (initialiser_list_.size() == 1) &&
                         details::is_constant_node(initialiser_list_[0]) &&
                         (T(0) == initialiser_list_[0]->value())
                       )
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
                  details::set_zero_value(vector_base_, size_);
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
                  details::set_zero_value(
                     vector_base_ + initialiser_list_size,
                     (size_ - initialiser_list_size));
               }
            }

            return *(vector_base_);
         }

         inline typename expression_node<T>::node_type type() const override
         {
            return expression_node<T>::e_vecinit;
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

         typedef expression_node<T>* expression_ptr;

         vector_init_zero_value_node(T* vector_base,
                                     const std::size_t& size,
                                     const std::vector<expression_ptr>& initialiser_list)
         : vector_base_(vector_base)
         , size_(size)
         , initialiser_list_(initialiser_list)
         {}

         inline T value() const override
         {
            details::set_zero_value(vector_base_, size_);
            return *(vector_base_);
         }

         inline typename expression_node<T>::node_type type() const override
         {
            return expression_node<T>::e_vecinit;
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

         typedef expression_node<T>* expression_ptr;

         vector_init_single_constvalue_node(T* vector_base,
                                            const std::size_t& size,
                                            const std::vector<expression_ptr>& initialiser_list)
         : vector_base_(vector_base)
         , size_(size)
         , initialiser_list_(initialiser_list)
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
            return expression_node<T>::e_vecinit;
         }

         inline bool valid() const override
         {
            return vector_base_ &&
                   (initialiser_list_.size() == 1) &&
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
         vector_init_single_constvalue_node<T>& operator=(const vector_init_single_constvalue_node<T>&) = delete;

         mutable T* vector_base_;
         const std::size_t size_;
         std::vector<expression_ptr> initialiser_list_;
         T single_initialiser_value_;
      };

      template <typename T>
      class vector_init_single_value_node final : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;

         vector_init_single_value_node(T* vector_base,
                                       const std::size_t& size,
                                       const std::vector<expression_ptr>& initialiser_list)
         : vector_base_(vector_base)
         , size_(size)
         , initialiser_list_(initialiser_list)
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
            return expression_node<T>::e_vecinit;
         }

         inline bool valid() const override
         {
            return vector_base_ &&
                   (initialiser_list_.size() == 1) &&
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

         typedef expression_node<T>* expression_ptr;

         vector_init_iota_constconst_node(T* vector_base,
                                          const std::size_t& size,
                                          const std::vector<expression_ptr>& initialiser_list)
         : vector_base_(vector_base)
         , size_(size)
         , initialiser_list_(initialiser_list)
         {
            base_value_      = initialiser_list_[0]->value();
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
            return expression_node<T>::e_vecinit;
         }

         inline bool valid() const override
         {
            return vector_base_ &&
                   (initialiser_list_.size() == 2) &&
                   (details::is_constant_node(initialiser_list_[0])) &&
                   (details::is_constant_node(initialiser_list_[1])) ;
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
         vector_init_iota_constconst_node<T>& operator=(const vector_init_iota_constconst_node<T>&) = delete;

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

         typedef expression_node<T>* expression_ptr;

         vector_init_iota_constnconst_node(T* vector_base,
                                           const std::size_t& size,
                                           const std::vector<expression_ptr>& initialiser_list)
         : vector_base_(vector_base)
         , size_(size)
         , initialiser_list_(initialiser_list)
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
            return expression_node<T>::e_vecinit;
         }

         inline bool valid() const override
         {
            return vector_base_ &&
                  (initialiser_list_.size() == 2) &&
                  ( details::is_constant_node(initialiser_list_[0])) &&
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
         vector_init_iota_constnconst_node<T>& operator=(const vector_init_iota_constnconst_node<T>&) = delete;

         mutable T* vector_base_;
         const std::size_t size_;
         std::vector<expression_ptr> initialiser_list_;
         T base_value_;
      };

      template <typename T>
      class vector_init_iota_nconstconst_node final : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;

         vector_init_iota_nconstconst_node(T* vector_base,
                                           const std::size_t& size,
                                           const std::vector<expression_ptr>& initialiser_list)
         : vector_base_(vector_base)
         , size_(size)
         , initialiser_list_(initialiser_list)
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
            return expression_node<T>::e_vecinit;
         }

         inline bool valid() const override
         {
            return vector_base_ &&
                   (initialiser_list_.size() == 2) &&
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
         vector_init_iota_nconstconst_node<T>& operator=(const vector_init_iota_nconstconst_node<T>&) = delete;

         mutable T* vector_base_;
         const std::size_t size_;
         std::vector<expression_ptr> initialiser_list_;
      };

      template <typename T>
      class vector_init_iota_nconstnconst_node final : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;

         vector_init_iota_nconstnconst_node(T* vector_base,
                                            const std::size_t& size,
                                            const std::vector<expression_ptr>& initialiser_list)
         : vector_base_(vector_base)
         , size_(size)
         , initialiser_list_(initialiser_list)
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
            return expression_node<T>::e_vecinit;
         }

         inline bool valid() const override
         {
            return vector_base_ &&
                   (initialiser_list_.size() == 2) &&
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
         vector_init_iota_nconstnconst_node<T>& operator=(const vector_init_iota_nconstnconst_node<T>&) = delete;

         mutable T* vector_base_;
         const std::size_t size_;
         std::vector<expression_ptr> initialiser_list_;
      };

      template <typename T>
      class swap_node final : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef variable_node<T>*   variable_node_ptr;

         swap_node(variable_node_ptr var0, variable_node_ptr var1)
         : var0_(var0)
         , var1_(var1)
         {}

         inline T value() const override
         {
            std::swap(var0_->ref(),var1_->ref());
            return var1_->ref();
         }

         inline typename expression_node<T>::node_type type() const override
         {
            return expression_node<T>::e_swap;
         }

      private:

         variable_node_ptr var0_;
         variable_node_ptr var1_;
      };

      template <typename T>
      class swap_generic_node final : public binary_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef ivariable<T>*       ivariable_ptr;

         swap_generic_node(expression_ptr var0, expression_ptr var1)
         : binary_node<T>(details::e_swap, var0, var1)
         , var0_(dynamic_cast<ivariable_ptr>(var0))
         , var1_(dynamic_cast<ivariable_ptr>(var1))
         {}

         inline T value() const override
         {
            std::swap(var0_->ref(),var1_->ref());
            return var1_->ref();
         }

         inline typename expression_node<T>::node_type type() const override
         {
            return expression_node<T>::e_swap;
         }

      private:

         ivariable_ptr var0_;
         ivariable_ptr var1_;
      };

      template <typename T>
      class swap_vecvec_node final
                             : public binary_node     <T>
                             , public vector_interface<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef vector_node    <T>* vector_node_ptr;
         typedef vec_data_store <T>  vds_t;

         using binary_node<T>::branch;

         swap_vecvec_node(expression_ptr branch0,
                          expression_ptr branch1)
         : binary_node<T>(details::e_swap, branch0, branch1)
         , vec0_node_ptr_(0)
         , vec1_node_ptr_(0)
         , initialised_  (false)
         {
            if (is_ivector_node(branch(0)))
            {
               vector_interface<T>* vi = reinterpret_cast<vector_interface<T>*>(0);

               if (0 != (vi = dynamic_cast<vector_interface<T>*>(branch(0))))
               {
                  vec0_node_ptr_ = vi->vec();
                  vds()          = vi->vds();
               }
            }

            if (is_ivector_node(branch(1)))
            {
               vector_interface<T>* vi = reinterpret_cast<vector_interface<T>*>(0);

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
               std::swap(vec0[i],vec1[i]);
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
            return expression_node<T>::e_vecvecswap;
         }

         inline bool valid() const override
         {
            return initialised_ && binary_node<T>::valid();
         }

         std::size_t size() const override
         {
            return std::min(
               vec0_node_ptr_->vec_holder().size(),
               vec1_node_ptr_->vec_holder().size());
         }

         std::size_t base_size() const override
         {
            return std::min(
               vec0_node_ptr_->vec_holder().base_size(),
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
         bool            initialised_;
         vds_t           vds_;
      };

      #ifndef math_expr_disable_string_capabilities
      template <typename T>
      class stringvar_node final
                           : public expression_node <T>
                           , public string_base_node<T>
                           , public range_interface <T>
      {
      public:

         typedef typename range_interface<T>::range_t range_t;

         static std::string null_value;

         explicit stringvar_node()
         : value_(&null_value)
         {}

         explicit stringvar_node(std::string& v)
         : value_(&v)
         {
            rp_.n0_c = std::make_pair<bool,std::size_t>(true,0);
            rp_.n1_c = std::make_pair<bool,std::size_t>(true,v.size());
            rp_.cache.first  = rp_.n0_c.second;
            rp_.cache.second = rp_.n1_c.second;
         }

         inline bool operator <(const stringvar_node<T>& v) const
         {
            return this < (&v);
         }

         inline T value() const override
         {
            rp_.n1_c.second  = (*value_).size();
            rp_.cache.second = rp_.n1_c.second;

            return std::numeric_limits<T>::quiet_NaN();
         }

         std::string str() const override
         {
            return ref();
         }

         char_cptr base() const override
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
            return expression_node<T>::e_stringvar;
         }

         void rebase(std::string& s)
         {
            value_ = &s;
            rp_.n0_c = std::make_pair<bool,std::size_t>(true,0);
            rp_.n1_c = std::make_pair<bool,std::size_t>(true,value_->size() - 1);
            rp_.cache.first  = rp_.n0_c.second;
            rp_.cache.second = rp_.n1_c.second;
         }

      private:

         std::string* value_;
         mutable range_t rp_;
      };

      template <typename T>
      std::string stringvar_node<T>::null_value = std::string("");

      template <typename T>
      class string_range_node final
                              : public expression_node <T>
                              , public string_base_node<T>
                              , public range_interface <T>
      {
      public:

         typedef typename range_interface<T>::range_t range_t;

         static std::string null_value;

         explicit string_range_node(std::string& v, const range_t& rp)
         : value_(&v)
         , rp_(rp)
         {}

         virtual ~string_range_node()
         {
            rp_.free();
         }

         inline bool operator <(const string_range_node<T>& v) const
         {
            return this < (&v);
         }

         inline T value() const override
         {
            return std::numeric_limits<T>::quiet_NaN();
         }

         inline std::string str() const override
         {
            return (*value_);
         }

         char_cptr base() const override
         {
            return &(*value_)[0];
         }

         std::size_t size() const override
         {
            return ref().size();
         }

         inline range_t range() const
         {
            return rp_;
         }

         inline std::string& ref()
         {
            return (*value_);
         }

         inline const std::string& ref() const
         {
            return (*value_);
         }

         inline range_t& range_ref() override
         {
            return rp_;
         }

         inline const range_t& range_ref() const override
         {
            return rp_;
         }

         inline typename expression_node<T>::node_type type() const override
         {
            return expression_node<T>::e_stringvarrng;
         }

      private:

         std::string* value_;
         range_t      rp_;
      };

      template <typename T>
      std::string string_range_node<T>::null_value = std::string("");

      template <typename T>
      class const_string_range_node final
                                    : public expression_node <T>
                                    , public string_base_node<T>
                                    , public range_interface <T>
      {
      public:

         typedef typename range_interface<T>::range_t range_t;

         explicit const_string_range_node(const std::string& v, const range_t& rp)
         : value_(v)
         , rp_(rp)
         {}

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

         char_cptr base() const override
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
            return expression_node<T>::e_cstringvarrng;
         }

      private:

         const_string_range_node(const const_string_range_node<T>&) = delete;
         const_string_range_node<T>& operator=(const const_string_range_node<T>&) = delete;

         const std::string value_;
         range_t rp_;
      };

      template <typename T>
      class generic_string_range_node final
                                      : public expression_node <T>
                                      , public string_base_node<T>
                                      , public range_interface <T>
      {
      public:

         typedef expression_node <T>* expression_ptr;
         typedef stringvar_node  <T>* strvar_node_ptr;
         typedef string_base_node<T>* str_base_ptr;
         typedef typename range_interface<T>::range_t range_t;
         typedef range_t*             range_ptr;
         typedef range_interface<T>   irange_t;
         typedef irange_t*            irange_ptr;
         typedef std::pair<expression_ptr,bool>  branch_t;

         generic_string_range_node(expression_ptr str_branch, const range_t& brange)
         : initialised_(false)
         , str_base_ptr_ (0)
         , str_range_ptr_(0)
         , base_range_(brange)
         {
            range_.n0_c = std::make_pair<bool,std::size_t>(true,0);
            range_.n1_c = std::make_pair<bool,std::size_t>(true,0);
            range_.cache.first  = range_.n0_c.second;
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

            if (
                  range      (str_r0, str_r1, base_str_size         ) &&
                  base_range_(r0    , r1    , base_str_size - str_r0)
               )
            {
               const std::size_t size = r1 - r0;

               range_.n1_c.second  = size;
               range_.cache.second = range_.n1_c.second;

               value_.assign(str_base_ptr_->base() + str_r0 + r0, size);
            }

            return std::numeric_limits<T>::quiet_NaN();
         }

         std::string str() const override
         {
            return value_;
         }

         char_cptr base() const override
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

         bool                initialised_;
         branch_t            branch_;
         str_base_ptr        str_base_ptr_;
         irange_ptr          str_range_ptr_;
         mutable range_t     base_range_;
         mutable range_t     range_;
         mutable std::string value_;
      };

      template <typename T>
      class string_concat_node final
                               : public binary_node     <T>
                               , public string_base_node<T>
                               , public range_interface <T>
      {
      public:

         typedef typename range_interface<T>::range_t range_t;
         typedef range_interface<T>   irange_t;
         typedef irange_t*            irange_ptr;
         typedef range_t*             range_ptr;
         typedef expression_node <T>* expression_ptr;
         typedef string_base_node<T>* str_base_ptr;

         using binary_node<T>::branch;

         string_concat_node(const operator_type& opr,
                            expression_ptr branch0,
                            expression_ptr branch1)
         : binary_node<T>(opr, branch0, branch1)
         , initialised_(false)
         , str0_base_ptr_ (0)
         , str1_base_ptr_ (0)
         , str0_range_ptr_(0)
         , str1_range_ptr_(0)
         {
            range_.n0_c = std::make_pair<bool,std::size_t>(true,0);
            range_.n1_c = std::make_pair<bool,std::size_t>(true,0);

            range_.cache.first  = range_.n0_c.second;
            range_.cache.second = range_.n1_c.second;

            if (is_generally_string_node(branch(0)))
            {
               str0_base_ptr_ = dynamic_cast<str_base_ptr>(branch(0));

               if (0 == str0_base_ptr_)
                  return;

               str0_range_ptr_ = dynamic_cast<irange_ptr>(branch(0));

               if (0 == str0_range_ptr_)
                  return;
            }

            if (is_generally_string_node(branch(1)))
            {
               str1_base_ptr_ = dynamic_cast<str_base_ptr>(branch(1));

               if (0 == str1_base_ptr_)
                  return;

               str1_range_ptr_ = dynamic_cast<irange_ptr>(branch(1));

               if (0 == str1_range_ptr_)
                  return;
            }

            initialised_ = str0_base_ptr_  &&
                           str1_base_ptr_  &&
                           str0_range_ptr_ &&
                           str1_range_ptr_ ;

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

            const range_t& range0 = str0_range_ptr_->range_ref();
            const range_t& range1 = str1_range_ptr_->range_ref();

            if (
                  range0(str0_r0, str0_r1, str0_base_ptr_->size()) &&
                  range1(str1_r0, str1_r1, str1_base_ptr_->size())
               )
            {
               const std::size_t size0 = (str0_r1 - str0_r0);
               const std::size_t size1 = (str1_r1 - str1_r0);

               value_.assign(str0_base_ptr_->base() + str0_r0, size0);
               value_.append(str1_base_ptr_->base() + str1_r0, size1);

               range_.n1_c.second  = value_.size();
               range_.cache.second = range_.n1_c.second;
            }

            return std::numeric_limits<T>::quiet_NaN();
         }

         std::string str() const override
         {
            return value_;
         }

         char_cptr base() const override
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
            return expression_node<T>::e_strconcat;
         }

         inline bool valid() const override
         {
            return initialised_ && binary_node<T>::valid();
         }

      private:

         bool                initialised_;
         str_base_ptr        str0_base_ptr_;
         str_base_ptr        str1_base_ptr_;
         irange_ptr          str0_range_ptr_;
         irange_ptr          str1_range_ptr_;
         mutable range_t     range_;
         mutable std::string value_;
      };

      template <typename T>
      class swap_string_node final
                             : public binary_node     <T>
                             , public string_base_node<T>
                             , public range_interface <T>
      {
      public:

         typedef typename range_interface<T>::range_t range_t;
         typedef range_t*             range_ptr;
         typedef range_interface<T>   irange_t;
         typedef irange_t*            irange_ptr;
         typedef expression_node <T>* expression_ptr;
         typedef stringvar_node  <T>* strvar_node_ptr;
         typedef string_base_node<T>* str_base_ptr;

         using binary_node<T>::branch;

         swap_string_node(expression_ptr branch0, expression_ptr branch1)
         : binary_node<T>(details::e_swap, branch0, branch1)
         , initialised_(false)
         , str0_node_ptr_(0)
         , str1_node_ptr_(0)
         {
            if (is_string_node(branch(0)))
            {
               str0_node_ptr_ = static_cast<strvar_node_ptr>(branch(0));
            }

            if (is_string_node(branch(1)))
            {
               str1_node_ptr_ = static_cast<strvar_node_ptr>(branch(1));
            }

            initialised_ = (str0_node_ptr_ && str1_node_ptr_);
            assert(valid());
         }

         inline T value() const override
         {
            branch(0)->value();
            branch(1)->value();

            std::swap(str0_node_ptr_->ref(), str1_node_ptr_->ref());

            return std::numeric_limits<T>::quiet_NaN();
         }

         std::string str() const override
         {
            return str0_node_ptr_->str();
         }

         char_cptr base() const override
         {
           return str0_node_ptr_->base();
         }

         std::size_t size() const override
         {
            return str0_node_ptr_->size();
         }

         range_t& range_ref() override
         {
            return str0_node_ptr_->range_ref();
         }

         const range_t& range_ref() const override
         {
            return str0_node_ptr_->range_ref();
         }

         inline typename expression_node<T>::node_type type() const override
         {
            return expression_node<T>::e_strswap;
         }

         inline bool valid() const override
         {
            return initialised_ && binary_node<T>::valid();
         }

      private:

         bool initialised_;
         strvar_node_ptr str0_node_ptr_;
         strvar_node_ptr str1_node_ptr_;
      };

      template <typename T>
      class swap_genstrings_node final : public binary_node<T>
      {
      public:

         typedef typename range_interface<T>::range_t range_t;
         typedef range_t*             range_ptr;
         typedef range_interface<T>   irange_t;
         typedef irange_t*            irange_ptr;
         typedef expression_node <T>* expression_ptr;
         typedef string_base_node<T>* str_base_ptr;

         using binary_node<T>::branch;

         swap_genstrings_node(expression_ptr branch0,
                              expression_ptr branch1)
         : binary_node<T>(details::e_default, branch0, branch1)
         , str0_base_ptr_ (0)
         , str1_base_ptr_ (0)
         , str0_range_ptr_(0)
         , str1_range_ptr_(0)
         , initialised_(false)
         {
            if (is_generally_string_node(branch(0)))
            {
               str0_base_ptr_ = dynamic_cast<str_base_ptr>(branch(0));

               if (0 == str0_base_ptr_)
                  return;

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

            initialised_ = str0_base_ptr_  &&
                           str1_base_ptr_  &&
                           str0_range_ptr_ &&
                           str1_range_ptr_ ;

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

            if (
                  range0(str0_r0, str0_r1, str0_base_ptr_->size()) &&
                  range1(str1_r0, str1_r1, str1_base_ptr_->size())
               )
            {
               const std::size_t size0    = range0.cache_size();
               const std::size_t size1    = range1.cache_size();
               const std::size_t max_size = std::min(size0,size1);

               char_ptr s0 = const_cast<char_ptr>(str0_base_ptr_->base() + str0_r0);
               char_ptr s1 = const_cast<char_ptr>(str1_base_ptr_->base() + str1_r0);

               loop_unroll::details lud(max_size);
               char_cptr upper_bound = s0 + lud.upper_bound;

               while (s0 < upper_bound)
               {
                  #define math_expr_loop(N)   \
                  std::swap(s0[N], s1[N]); \

                  math_expr_loop( 0) math_expr_loop( 1)
                  math_expr_loop( 2) math_expr_loop( 3)
                  #ifndef math_expr_disable_superscalar_unroll
                  math_expr_loop( 4) math_expr_loop( 5)
                  math_expr_loop( 6) math_expr_loop( 7)
                  math_expr_loop( 8) math_expr_loop( 9)
                  math_expr_loop(10) math_expr_loop(11)
                  math_expr_loop(12) math_expr_loop(13)
                  math_expr_loop(14) math_expr_loop(15)
                  #endif

                  s0 += lud.batch_size;
                  s1 += lud.batch_size;
               }

               int i = 0;

               switch (lud.remainder)
               {
                  #define case_stmt(N)                       \
                  case N : { std::swap(s0[i], s1[i]); ++i; } \
                  [[fallthrough]];                         \

                  #ifndef math_expr_disable_superscalar_unroll
                  case_stmt(15) case_stmt(14)
                  case_stmt(13) case_stmt(12)
                  case_stmt(11) case_stmt(10)
                  case_stmt( 9) case_stmt( 8)
                  case_stmt( 7) case_stmt( 6)
                  case_stmt( 5) case_stmt( 4)
                  #endif
                  case_stmt( 3) case_stmt( 2)
                  case_stmt( 1)
                  default: break;
               }

               #undef math_expr_loop
               #undef case_stmt
            }

            return std::numeric_limits<T>::quiet_NaN();
         }

         inline typename expression_node<T>::node_type type() const override
         {
            return expression_node<T>::e_strswap;
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
         range_ptr    str0_range_ptr_;
         range_ptr    str1_range_ptr_;
         bool         initialised_;
      };

      template <typename T>
      class stringvar_size_node final : public expression_node<T>
      {
      public:

         static const std::string null_value;

         explicit stringvar_size_node()
         : value_(&null_value)
         {}

         explicit stringvar_size_node(std::string& v)
         : value_(&v)
         {}

         inline T value() const override
         {
            return T((*value_).size());
         }

         inline typename expression_node<T>::node_type type() const override
         {
            return expression_node<T>::e_stringvarsize;
         }

      private:

         const std::string* value_;
      };

      template <typename T>
      const std::string stringvar_size_node<T>::null_value = std::string("");

      template <typename T>
      class string_size_node final : public expression_node<T>
      {
      public:

         typedef expression_node <T>* expression_ptr;
         typedef string_base_node<T>* str_base_ptr;
         typedef std::pair<expression_ptr,bool>  branch_t;

         explicit string_size_node(expression_ptr branch)
         : str_base_ptr_(0)
         {
            construct_branch_pair(branch_, branch);

            if (is_generally_string_node(branch_.first))
            {
               str_base_ptr_ = dynamic_cast<str_base_ptr>(branch_.first);
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
            return expression_node<T>::e_stringsize;
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

         branch_t     branch_;
         str_base_ptr str_base_ptr_;
      };

      struct asn_assignment
      {
         static inline void execute(std::string& s, char_cptr data, const std::size_t size)
         { s.assign(data,size); }
      };

      struct asn_addassignment
      {
         static inline void execute(std::string& s, char_cptr data, const std::size_t size)
         { s.append(data,size); }
      };

      template <typename T, typename AssignmentProcess = asn_assignment>
      class assignment_string_node final
                                   : public binary_node     <T>
                                   , public string_base_node<T>
                                   , public range_interface <T>
      {
      public:

         typedef typename range_interface<T>::range_t range_t;
         typedef range_t*             range_ptr;
         typedef range_interface <T>  irange_t;
         typedef irange_t*            irange_ptr;
         typedef expression_node <T>* expression_ptr;
         typedef stringvar_node  <T>* strvar_node_ptr;
         typedef string_base_node<T>* str_base_ptr;

         using binary_node<T>::branch;

         assignment_string_node(const operator_type& opr,
                                expression_ptr branch0,
                                expression_ptr branch1)
         : binary_node<T>(opr, branch0, branch1)
         , initialised_(false)
         , str0_base_ptr_ (0)
         , str1_base_ptr_ (0)
         , str0_node_ptr_ (0)
         , str1_range_ptr_(0)
         {
            if (is_string_node(branch(0)))
            {
               str0_node_ptr_ = static_cast<strvar_node_ptr>(branch(0));
               str0_base_ptr_ = dynamic_cast<str_base_ptr>(branch(0));
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

            initialised_ = str0_base_ptr_  &&
                           str1_base_ptr_  &&
                           str0_node_ptr_  &&
                           str1_range_ptr_ ;

            assert(valid());
         }

         inline T value() const override
         {
            branch(1)->value();

            std::size_t r0 = 0;
            std::size_t r1 = 0;

            const range_t& range = (*str1_range_ptr_);

            if (range(r0, r1, str1_base_ptr_->size()))
            {
               AssignmentProcess::execute(
                  str0_node_ptr_->ref(),
                  str1_base_ptr_->base() + r0, (r1 - r0));

               branch(0)->value();
            }

            return std::numeric_limits<T>::quiet_NaN();
         }

         std::string str() const override
         {
            return str0_node_ptr_->str();
         }

         char_cptr base() const override
         {
           return str0_node_ptr_->base();
         }

         std::size_t size() const override
         {
            return str0_node_ptr_->size();
         }

         range_t& range_ref() override
         {
            return str0_node_ptr_->range_ref();
         }

         const range_t& range_ref() const override
         {
            return str0_node_ptr_->range_ref();
         }

         inline typename expression_node<T>::node_type type() const override
         {
            return expression_node<T>::e_strass;
         }

         inline bool valid() const override
         {
            return initialised_ && binary_node<T>::valid();
         }

      private:

         bool            initialised_;
         str_base_ptr    str0_base_ptr_;
         str_base_ptr    str1_base_ptr_;
         strvar_node_ptr str0_node_ptr_;
         range_ptr       str1_range_ptr_;
      };

      template <typename T, typename AssignmentProcess = asn_assignment>
      class assignment_string_range_node final
                                         : public binary_node     <T>
                                         , public string_base_node<T>
                                         , public range_interface <T>
      {
      public:

         typedef typename range_interface<T>::range_t range_t;
         typedef range_t*              range_ptr;
         typedef range_interface  <T>  irange_t;
         typedef irange_t*             irange_ptr;
         typedef expression_node  <T>* expression_ptr;
         typedef stringvar_node   <T>* strvar_node_ptr;
         typedef string_range_node<T>* str_rng_node_ptr;
         typedef string_base_node <T>* str_base_ptr;

         using binary_node<T>::branch;

         assignment_string_range_node(const operator_type& opr,
                                      expression_ptr branch0,
                                      expression_ptr branch1)
         : binary_node<T>(opr, branch0, branch1)
         , initialised_(false)
         , str0_base_ptr_    (0)
         , str1_base_ptr_    (0)
         , str0_rng_node_ptr_(0)
         , str0_range_ptr_   (0)
         , str1_range_ptr_   (0)
         {
            if (is_string_range_node(branch(0)))
            {
               str0_rng_node_ptr_ = static_cast<str_rng_node_ptr>(branch(0));
               str0_base_ptr_     = dynamic_cast<str_base_ptr>(branch(0));
               irange_ptr range   = dynamic_cast<irange_ptr>(branch(0));

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

            initialised_ = str0_base_ptr_     &&
                           str1_base_ptr_     &&
                           str0_rng_node_ptr_ &&
                           str0_range_ptr_    &&
                           str1_range_ptr_    ;

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

            if (
                  range0(s0_r0, s0_r1, str0_base_ptr_->size()) &&
                  range1(s1_r0, s1_r1, str1_base_ptr_->size())
               )
            {
               const std::size_t size = std::min((s0_r1 - s0_r0), (s1_r1 - s1_r0));

               std::copy(
                  str1_base_ptr_->base() + s1_r0,
                  str1_base_ptr_->base() + s1_r0 + size,
                  const_cast<char_ptr>(base() + s0_r0));
            }

            return std::numeric_limits<T>::quiet_NaN();
         }

         std::string str() const override
         {
            return str0_base_ptr_->str();
         }

         char_cptr base() const override
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
            return expression_node<T>::e_strass;
         }

         inline bool valid() const override
         {
            return initialised_ && binary_node<T>::valid();
         }

      private:

         bool             initialised_;
         str_base_ptr     str0_base_ptr_;
         str_base_ptr     str1_base_ptr_;
         str_rng_node_ptr str0_rng_node_ptr_;
         range_ptr        str0_range_ptr_;
         range_ptr        str1_range_ptr_;
      };

      template <typename T>
      class conditional_string_node final
                                    : public trinary_node    <T>
                                    , public string_base_node<T>
                                    , public range_interface <T>
      {
      public:

         typedef typename range_interface<T>::range_t range_t;
         typedef range_t*             range_ptr;
         typedef range_interface <T>  irange_t;
         typedef irange_t*            irange_ptr;
         typedef expression_node <T>* expression_ptr;
         typedef string_base_node<T>* str_base_ptr;

         conditional_string_node(expression_ptr condition,
                                 expression_ptr consequent,
                                 expression_ptr alternative)
         : trinary_node<T>(details::e_default, consequent, alternative, condition)
         , initialised_(false)
         , str0_base_ptr_ (0)
         , str1_base_ptr_ (0)
         , str0_range_ptr_(0)
         , str1_range_ptr_(0)
         , condition_  (condition  )
         , consequent_ (consequent )
         , alternative_(alternative)
         {
            range_.n0_c = std::make_pair<bool,std::size_t>(true,0);
            range_.n1_c = std::make_pair<bool,std::size_t>(true,0);

            range_.cache.first  = range_.n0_c.second;
            range_.cache.second = range_.n1_c.second;

            if (is_generally_string_node(trinary_node<T>::branch_[0].first))
            {
               str0_base_ptr_ = dynamic_cast<str_base_ptr>(trinary_node<T>::branch_[0].first);

               if (0 == str0_base_ptr_)
                  return;

               str0_range_ptr_ = dynamic_cast<irange_ptr>(trinary_node<T>::branch_[0].first);

               if (0 == str0_range_ptr_)
                  return;
            }

            if (is_generally_string_node(trinary_node<T>::branch_[1].first))
            {
               str1_base_ptr_ = dynamic_cast<str_base_ptr>(trinary_node<T>::branch_[1].first);

               if (0 == str1_base_ptr_)
                  return;

               str1_range_ptr_ = dynamic_cast<irange_ptr>(trinary_node<T>::branch_[1].first);

               if (0 == str1_range_ptr_)
                  return;
            }

            initialised_ = str0_base_ptr_  &&
                           str1_base_ptr_  &&
                           str0_range_ptr_ &&
                           str1_range_ptr_ ;

            assert(valid());
         }

         inline T value() const override
         {
            std::size_t r0 = 0;
            std::size_t r1 = 0;

            if (is_true(condition_))
            {
               consequent_->value();

               const range_t& range = str0_range_ptr_->range_ref();

               if (range(r0, r1, str0_base_ptr_->size()))
               {
                  const std::size_t size = (r1 - r0);

                  value_.assign(str0_base_ptr_->base() + r0, size);

                  range_.n1_c.second  = value_.size();
                  range_.cache.second = range_.n1_c.second;

                  return T(1);
               }
            }
            else
            {
               alternative_->value();

               const range_t& range = str1_range_ptr_->range_ref();

               if (range(r0, r1, str1_base_ptr_->size()))
               {
                  const std::size_t size = (r1 - r0);

                  value_.assign(str1_base_ptr_->base() + r0, size);

                  range_.n1_c.second  = value_.size();
                  range_.cache.second = range_.n1_c.second;

                  return T(0);
               }
            }

            return std::numeric_limits<T>::quiet_NaN();
         }

         std::string str() const override
         {
            return value_;
         }

         char_cptr base() const override
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
            return expression_node<T>::e_strcondition;
         }

         inline bool valid() const override
         {
            return
               initialised_                         &&
               condition_  && condition_  ->valid() &&
               consequent_ && consequent_ ->valid() &&
               alternative_&& alternative_->valid() ;
         }

      private:

         bool initialised_;
         str_base_ptr str0_base_ptr_;
         str_base_ptr str1_base_ptr_;
         irange_ptr   str0_range_ptr_;
         irange_ptr   str1_range_ptr_;
         mutable range_t     range_;
         mutable std::string value_;

         expression_ptr condition_;
         expression_ptr consequent_;
         expression_ptr alternative_;
      };

      template <typename T>
      class cons_conditional_str_node final
                                      : public binary_node     <T>
                                      , public string_base_node<T>
                                      , public range_interface <T>
      {
      public:

         typedef typename range_interface<T>::range_t range_t;
         typedef range_t*             range_ptr;
         typedef range_interface <T>  irange_t;
         typedef irange_t*            irange_ptr;
         typedef expression_node <T>* expression_ptr;
         typedef string_base_node<T>* str_base_ptr;

         using binary_node<T>::branch;

         cons_conditional_str_node(expression_ptr condition,
                                   expression_ptr consequent)
         : binary_node<T>(details::e_default, consequent, condition)
         , initialised_(false)
         , str0_base_ptr_ (0)
         , str0_range_ptr_(0)
         , condition_ (condition )
         , consequent_(consequent)
         {
            range_.n0_c = std::make_pair<bool,std::size_t>(true,0);
            range_.n1_c = std::make_pair<bool,std::size_t>(true,0);

            range_.cache.first  = range_.n0_c.second;
            range_.cache.second = range_.n1_c.second;

            if (is_generally_string_node(branch(0)))
            {
               str0_base_ptr_ = dynamic_cast<str_base_ptr>(branch(0));

               if (0 == str0_base_ptr_)
                  return;

               str0_range_ptr_ = dynamic_cast<irange_ptr>(branch(0));

               if (0 == str0_range_ptr_)
                  return;
            }

            initialised_ = str0_base_ptr_ && str0_range_ptr_ ;
            assert(valid());
         }

         inline T value() const override
         {
            if (is_true(condition_))
            {
               consequent_->value();

               const range_t& range = str0_range_ptr_->range_ref();

               std::size_t r0 = 0;
               std::size_t r1 = 0;

               if (range(r0, r1, str0_base_ptr_->size()))
               {
                  const std::size_t size = (r1 - r0);

                  value_.assign(str0_base_ptr_->base() + r0, size);

                  range_.n1_c.second  = value_.size();
                  range_.cache.second = range_.n1_c.second;

                  return T(1);
               }
            }

            return std::numeric_limits<T>::quiet_NaN();
         }

         std::string str() const
         {
            return value_;
         }

         char_cptr base() const
         {
            return &value_[0];
         }

         std::size_t size() const
         {
            return value_.size();
         }

         range_t& range_ref()
         {
            return range_;
         }

         const range_t& range_ref() const
         {
            return range_;
         }

         inline typename expression_node<T>::node_type type() const override
         {
            return expression_node<T>::e_strccondition;
         }

         inline bool valid() const override
         {
            return
               initialised_                         &&
               condition_  && condition_  ->valid() &&
               consequent_ && consequent_ ->valid() ;
         }

      private:

         bool initialised_;
         str_base_ptr str0_base_ptr_;
         irange_ptr   str0_range_ptr_;
         mutable range_t     range_;
         mutable std::string value_;

         expression_ptr condition_;
         expression_ptr consequent_;
      };

      template <typename T, typename VarArgFunction>
      class str_vararg_node final
                            : public expression_node <T>
                            , public string_base_node<T>
                            , public range_interface <T>
      {
      public:

         typedef typename range_interface<T>::range_t range_t;
         typedef range_t*             range_ptr;
         typedef range_interface <T>  irange_t;
         typedef irange_t*            irange_ptr;
         typedef expression_node <T>* expression_ptr;
         typedef string_base_node<T>* str_base_ptr;
         typedef std::pair<expression_ptr,bool> branch_t;

         template <typename Allocator,
                   template <typename, typename> class Sequence>
         explicit str_vararg_node(const Sequence<expression_ptr,Allocator>& arg_list)
         : initialised_(false)
         , str_base_ptr_ (0)
         , str_range_ptr_(0)
         {
            construct_branch_pair(final_node_, const_cast<expression_ptr>(arg_list.back()));

            if (0 == final_node_.first)
               return;
            else if (!is_generally_string_node(final_node_.first))
               return;

            str_base_ptr_ = dynamic_cast<str_base_ptr>(final_node_.first);

            if (0 == str_base_ptr_)
               return;

            str_range_ptr_ = dynamic_cast<irange_ptr>(final_node_.first);

            if (0 == str_range_ptr_)
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

         char_cptr base() const override
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
            return expression_node<T>::e_stringvararg;
         }

         inline bool valid() const override
         {
            return
               initialised_ &&
               final_node_.first && final_node_.first->valid();
         }

         void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) override
         {
            expression_node<T>::ndb_t::collect(final_node_ , node_delete_list);
            expression_node<T>::ndb_t::collect(arg_list_   , node_delete_list);
         }

         std::size_t node_depth() const override
         {
            return std::max(
               expression_node<T>::ndb_t::compute_node_depth(final_node_),
               expression_node<T>::ndb_t::compute_node_depth(arg_list_  ));
         }

      private:

         bool                  initialised_;
         branch_t              final_node_;
         str_base_ptr          str_base_ptr_;
         irange_ptr            str_range_ptr_;
         std::vector<branch_t> arg_list_;
      };
      #endif

      template <typename T>
      class assert_node final : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef std::pair<expression_ptr,bool> branch_t;
         typedef string_base_node<T>* str_base_ptr;
         typedef assert_check::assert_context assert_context_t;

         assert_node(expression_ptr   assert_condition_node,
                     expression_ptr   assert_message_node,
                     assert_check_ptr assert_check,
                     const assert_context_t& context)
         : assert_message_str_base_(0)
         , assert_check_(assert_check)
         , context_(context)
         {
            construct_branch_pair(assert_condition_node_, assert_condition_node);
            construct_branch_pair(assert_message_node_  , assert_message_node  );

            #ifndef math_expr_disable_string_capabilities
            if (
                  assert_message_node_.first &&
                  details::is_generally_string_node(assert_message_node_.first)
               )
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

            #ifndef math_expr_disable_string_capabilities
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
            return expression_node<T>::e_assert;
         }

         inline bool valid() const override
         {
            return (
                     assert_check_ &&
                     assert_condition_node_.first &&
                     assert_condition_node_.first->valid()
                   ) &&
                   (
                     (0 == assert_message_node_.first) ||
                     (
                       assert_message_node_.first &&
                       assert_message_str_base_   &&
                       assert_message_node_.first->valid() &&
                       details::is_generally_string_node(assert_message_node_.first)
                     )
                   );
         }

         void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) override
         {
            expression_node<T>::ndb_t::collect(assert_condition_node_, node_delete_list);
            expression_node<T>::ndb_t::collect(assert_message_node_  , node_delete_list);
         }

         std::size_t node_depth() const override
         {
            return expression_node<T>::ndb_t::compute_node_depth
               (assert_condition_node_, assert_message_node_);
         }

      private:

         branch_t         assert_condition_node_;
         branch_t         assert_message_node_;
         str_base_ptr     assert_message_str_base_;
         assert_check_ptr assert_check_;
         mutable assert_context_t context_;
      };

      template <typename T, std::size_t N>
      inline T axn(const T a, const T x)
      {
         // a*x^n
         return a * math_expr::details::numeric::fast_exp<T,N>::result(x);
      }

      template <typename T, std::size_t N>
      inline T axnb(const T a, const T x, const T b)
      {
         // a*x^n+b
         return a * math_expr::details::numeric::fast_exp<T,N>::result(x) + b;
      }

      template <typename T>
      struct sf_base
      {
         typedef typename details::functor_t<T>::Type Type;
         typedef typename details::functor_t<T> functor_t;
         typedef typename functor_t::qfunc_t quaternary_functor_t;
         typedef typename functor_t::tfunc_t trinary_functor_t;
         typedef typename functor_t::bfunc_t binary_functor_t;
         typedef typename functor_t::ufunc_t unary_functor_t;
      };

      #define define_sfop3(NN, OP0, OP1)                 \
      template <typename T>                              \
      struct sf##NN##_op : public sf_base<T>             \
      {                                                  \
         typedef typename sf_base<T>::Type const Type;   \
         static inline T process(Type x, Type y, Type z) \
         {                                               \
            return (OP0);                                \
         }                                               \
         static inline std::string id()                  \
         {                                               \
            return (OP1);                                \
         }                                               \
      };                                                 \

      define_sfop3(00,(x + y) / z       ,"(t+t)/t")
      define_sfop3(01,(x + y) * z       ,"(t+t)*t")
      define_sfop3(02,(x + y) - z       ,"(t+t)-t")
      define_sfop3(03,(x + y) + z       ,"(t+t)+t")
      define_sfop3(04,(x - y) + z       ,"(t-t)+t")
      define_sfop3(05,(x - y) / z       ,"(t-t)/t")
      define_sfop3(06,(x - y) * z       ,"(t-t)*t")
      define_sfop3(07,(x * y) + z       ,"(t*t)+t")
      define_sfop3(08,(x * y) - z       ,"(t*t)-t")
      define_sfop3(09,(x * y) / z       ,"(t*t)/t")
      define_sfop3(10,(x * y) * z       ,"(t*t)*t")
      define_sfop3(11,(x / y) + z       ,"(t/t)+t")
      define_sfop3(12,(x / y) - z       ,"(t/t)-t")
      define_sfop3(13,(x / y) / z       ,"(t/t)/t")
      define_sfop3(14,(x / y) * z       ,"(t/t)*t")
      define_sfop3(15,x / (y + z)       ,"t/(t+t)")
      define_sfop3(16,x / (y - z)       ,"t/(t-t)")
      define_sfop3(17,x / (y * z)       ,"t/(t*t)")
      define_sfop3(18,x / (y / z)       ,"t/(t/t)")
      define_sfop3(19,x * (y + z)       ,"t*(t+t)")
      define_sfop3(20,x * (y - z)       ,"t*(t-t)")
      define_sfop3(21,x * (y * z)       ,"t*(t*t)")
      define_sfop3(22,x * (y / z)       ,"t*(t/t)")
      define_sfop3(23,x - (y + z)       ,"t-(t+t)")
      define_sfop3(24,x - (y - z)       ,"t-(t-t)")
      define_sfop3(25,x - (y / z)       ,"t-(t/t)")
      define_sfop3(26,x - (y * z)       ,"t-(t*t)")
      define_sfop3(27,x + (y * z)       ,"t+(t*t)")
      define_sfop3(28,x + (y / z)       ,"t+(t/t)")
      define_sfop3(29,x + (y + z)       ,"t+(t+t)")
      define_sfop3(30,x + (y - z)       ,"t+(t-t)")
      define_sfop3(31,(axnb<T,2>(x,y,z)),"       ")
      define_sfop3(32,(axnb<T,3>(x,y,z)),"       ")
      define_sfop3(33,(axnb<T,4>(x,y,z)),"       ")
      define_sfop3(34,(axnb<T,5>(x,y,z)),"       ")
      define_sfop3(35,(axnb<T,6>(x,y,z)),"       ")
      define_sfop3(36,(axnb<T,7>(x,y,z)),"       ")
      define_sfop3(37,(axnb<T,8>(x,y,z)),"       ")
      define_sfop3(38,(axnb<T,9>(x,y,z)),"       ")
      define_sfop3(39,x * numeric::log(y)   + z,"")
      define_sfop3(40,x * numeric::log(y)   - z,"")
      define_sfop3(41,x * numeric::log10(y) + z,"")
      define_sfop3(42,x * numeric::log10(y) - z,"")
      define_sfop3(43,x * numeric::sin(y) + z  ,"")
      define_sfop3(44,x * numeric::sin(y) - z  ,"")
      define_sfop3(45,x * numeric::cos(y) + z  ,"")
      define_sfop3(46,x * numeric::cos(y) - z  ,"")
      define_sfop3(47,details::is_true(x) ? y : z,"")

      #define define_sfop4(NN, OP0, OP1)                         \
      template <typename T>                                      \
      struct sf##NN##_op : public sf_base<T>                     \
      {                                                          \
         typedef typename sf_base<T>::Type const Type;           \
         static inline T process(Type x, Type y, Type z, Type w) \
         {                                                       \
            return (OP0);                                        \
         }                                                       \
         static inline std::string id()                          \
         {                                                       \
            return (OP1);                                        \
         }                                                       \
      };                                                         \

      define_sfop4(48,(x + ((y + z) / w)),"t+((t+t)/t)")
      define_sfop4(49,(x + ((y + z) * w)),"t+((t+t)*t)")
      define_sfop4(50,(x + ((y - z) / w)),"t+((t-t)/t)")
      define_sfop4(51,(x + ((y - z) * w)),"t+((t-t)*t)")
      define_sfop4(52,(x + ((y * z) / w)),"t+((t*t)/t)")
      define_sfop4(53,(x + ((y * z) * w)),"t+((t*t)*t)")
      define_sfop4(54,(x + ((y / z) + w)),"t+((t/t)+t)")
      define_sfop4(55,(x + ((y / z) / w)),"t+((t/t)/t)")
      define_sfop4(56,(x + ((y / z) * w)),"t+((t/t)*t)")
      define_sfop4(57,(x - ((y + z) / w)),"t-((t+t)/t)")
      define_sfop4(58,(x - ((y + z) * w)),"t-((t+t)*t)")
      define_sfop4(59,(x - ((y - z) / w)),"t-((t-t)/t)")
      define_sfop4(60,(x - ((y - z) * w)),"t-((t-t)*t)")
      define_sfop4(61,(x - ((y * z) / w)),"t-((t*t)/t)")
      define_sfop4(62,(x - ((y * z) * w)),"t-((t*t)*t)")
      define_sfop4(63,(x - ((y / z) / w)),"t-((t/t)/t)")
      define_sfop4(64,(x - ((y / z) * w)),"t-((t/t)*t)")
      define_sfop4(65,(((x + y) * z) - w),"((t+t)*t)-t")
      define_sfop4(66,(((x - y) * z) - w),"((t-t)*t)-t")
      define_sfop4(67,(((x * y) * z) - w),"((t*t)*t)-t")
      define_sfop4(68,(((x / y) * z) - w),"((t/t)*t)-t")
      define_sfop4(69,(((x + y) / z) - w),"((t+t)/t)-t")
      define_sfop4(70,(((x - y) / z) - w),"((t-t)/t)-t")
      define_sfop4(71,(((x * y) / z) - w),"((t*t)/t)-t")
      define_sfop4(72,(((x / y) / z) - w),"((t/t)/t)-t")
      define_sfop4(73,((x * y) + (z * w)),"(t*t)+(t*t)")
      define_sfop4(74,((x * y) - (z * w)),"(t*t)-(t*t)")
      define_sfop4(75,((x * y) + (z / w)),"(t*t)+(t/t)")
      define_sfop4(76,((x * y) - (z / w)),"(t*t)-(t/t)")
      define_sfop4(77,((x / y) + (z / w)),"(t/t)+(t/t)")
      define_sfop4(78,((x / y) - (z / w)),"(t/t)-(t/t)")
      define_sfop4(79,((x / y) - (z * w)),"(t/t)-(t*t)")
      define_sfop4(80,(x / (y + (z * w))),"t/(t+(t*t))")
      define_sfop4(81,(x / (y - (z * w))),"t/(t-(t*t))")
      define_sfop4(82,(x * (y + (z * w))),"t*(t+(t*t))")
      define_sfop4(83,(x * (y - (z * w))),"t*(t-(t*t))")

      define_sfop4(84,(axn<T,2>(x,y) + axn<T,2>(z,w)),"")
      define_sfop4(85,(axn<T,3>(x,y) + axn<T,3>(z,w)),"")
      define_sfop4(86,(axn<T,4>(x,y) + axn<T,4>(z,w)),"")
      define_sfop4(87,(axn<T,5>(x,y) + axn<T,5>(z,w)),"")
      define_sfop4(88,(axn<T,6>(x,y) + axn<T,6>(z,w)),"")
      define_sfop4(89,(axn<T,7>(x,y) + axn<T,7>(z,w)),"")
      define_sfop4(90,(axn<T,8>(x,y) + axn<T,8>(z,w)),"")
      define_sfop4(91,(axn<T,9>(x,y) + axn<T,9>(z,w)),"")
      define_sfop4(92,((details::is_true(x) && details::is_true(y)) ? z : w),"")
      define_sfop4(93,((details::is_true(x) || details::is_true(y)) ? z : w),"")
      define_sfop4(94,((x <  y) ? z : w),"")
      define_sfop4(95,((x <= y) ? z : w),"")
      define_sfop4(96,((x >  y) ? z : w),"")
      define_sfop4(97,((x >= y) ? z : w),"")
      define_sfop4(98,(details::is_true(numeric::equal(x,y)) ? z : w),"")
      define_sfop4(99,(x * numeric::sin(y) + z * numeric::cos(w)),"")

      define_sfop4(ext00,((x + y) - (z * w)),"(t+t)-(t*t)")
      define_sfop4(ext01,((x + y) - (z / w)),"(t+t)-(t/t)")
      define_sfop4(ext02,((x + y) + (z * w)),"(t+t)+(t*t)")
      define_sfop4(ext03,((x + y) + (z / w)),"(t+t)+(t/t)")
      define_sfop4(ext04,((x - y) + (z * w)),"(t-t)+(t*t)")
      define_sfop4(ext05,((x - y) + (z / w)),"(t-t)+(t/t)")
      define_sfop4(ext06,((x - y) - (z * w)),"(t-t)-(t*t)")
      define_sfop4(ext07,((x - y) - (z / w)),"(t-t)-(t/t)")
      define_sfop4(ext08,((x + y) - (z - w)),"(t+t)-(t-t)")
      define_sfop4(ext09,((x + y) + (z - w)),"(t+t)+(t-t)")
      define_sfop4(ext10,((x + y) + (z + w)),"(t+t)+(t+t)")
      define_sfop4(ext11,((x + y) * (z - w)),"(t+t)*(t-t)")
      define_sfop4(ext12,((x + y) / (z - w)),"(t+t)/(t-t)")
      define_sfop4(ext13,((x - y) - (z + w)),"(t-t)-(t+t)")
      define_sfop4(ext14,((x - y) + (z + w)),"(t-t)+(t+t)")
      define_sfop4(ext15,((x - y) * (z + w)),"(t-t)*(t+t)")
      define_sfop4(ext16,((x - y) / (z + w)),"(t-t)/(t+t)")
      define_sfop4(ext17,((x * y) - (z + w)),"(t*t)-(t+t)")
      define_sfop4(ext18,((x / y) - (z + w)),"(t/t)-(t+t)")
      define_sfop4(ext19,((x * y) + (z + w)),"(t*t)+(t+t)")
      define_sfop4(ext20,((x / y) + (z + w)),"(t/t)+(t+t)")
      define_sfop4(ext21,((x * y) + (z - w)),"(t*t)+(t-t)")
      define_sfop4(ext22,((x / y) + (z - w)),"(t/t)+(t-t)")
      define_sfop4(ext23,((x * y) - (z - w)),"(t*t)-(t-t)")
      define_sfop4(ext24,((x / y) - (z - w)),"(t/t)-(t-t)")
      define_sfop4(ext25,((x + y) * (z * w)),"(t+t)*(t*t)")
      define_sfop4(ext26,((x + y) * (z / w)),"(t+t)*(t/t)")
      define_sfop4(ext27,((x + y) / (z * w)),"(t+t)/(t*t)")
      define_sfop4(ext28,((x + y) / (z / w)),"(t+t)/(t/t)")
      define_sfop4(ext29,((x - y) / (z * w)),"(t-t)/(t*t)")
      define_sfop4(ext30,((x - y) / (z / w)),"(t-t)/(t/t)")
      define_sfop4(ext31,((x - y) * (z * w)),"(t-t)*(t*t)")
      define_sfop4(ext32,((x - y) * (z / w)),"(t-t)*(t/t)")
      define_sfop4(ext33,((x * y) * (z + w)),"(t*t)*(t+t)")
      define_sfop4(ext34,((x / y) * (z + w)),"(t/t)*(t+t)")
      define_sfop4(ext35,((x * y) / (z + w)),"(t*t)/(t+t)")
      define_sfop4(ext36,((x / y) / (z + w)),"(t/t)/(t+t)")
      define_sfop4(ext37,((x * y) / (z - w)),"(t*t)/(t-t)")
      define_sfop4(ext38,((x / y) / (z - w)),"(t/t)/(t-t)")
      define_sfop4(ext39,((x * y) * (z - w)),"(t*t)*(t-t)")
      define_sfop4(ext40,((x * y) / (z * w)),"(t*t)/(t*t)")
      define_sfop4(ext41,((x / y) * (z / w)),"(t/t)*(t/t)")
      define_sfop4(ext42,((x / y) * (z - w)),"(t/t)*(t-t)")
      define_sfop4(ext43,((x * y) * (z * w)),"(t*t)*(t*t)")
      define_sfop4(ext44,(x + (y * (z / w))),"t+(t*(t/t))")
      define_sfop4(ext45,(x - (y * (z / w))),"t-(t*(t/t))")
      define_sfop4(ext46,(x + (y / (z * w))),"t+(t/(t*t))")
      define_sfop4(ext47,(x - (y / (z * w))),"t-(t/(t*t))")
      define_sfop4(ext48,(((x - y) - z) * w),"((t-t)-t)*t")
      define_sfop4(ext49,(((x - y) - z) / w),"((t-t)-t)/t")
      define_sfop4(ext50,(((x - y) + z) * w),"((t-t)+t)*t")
      define_sfop4(ext51,(((x - y) + z) / w),"((t-t)+t)/t")
      define_sfop4(ext52,((x + (y - z)) * w),"(t+(t-t))*t")
      define_sfop4(ext53,((x + (y - z)) / w),"(t+(t-t))/t")
      define_sfop4(ext54,((x + y) / (z + w)),"(t+t)/(t+t)")
      define_sfop4(ext55,((x - y) / (z - w)),"(t-t)/(t-t)")
      define_sfop4(ext56,((x + y) * (z + w)),"(t+t)*(t+t)")
      define_sfop4(ext57,((x - y) * (z - w)),"(t-t)*(t-t)")
      define_sfop4(ext58,((x - y) + (z - w)),"(t-t)+(t-t)")
      define_sfop4(ext59,((x - y) - (z - w)),"(t-t)-(t-t)")
      define_sfop4(ext60,((x / y) + (z * w)),"(t/t)+(t*t)")
      define_sfop4(ext61,(((x * y) * z) / w),"((t*t)*t)/t")

      #undef define_sfop3
      #undef define_sfop4

      template <typename T, typename SpecialFunction>
      class sf3_node final : public trinary_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;

         sf3_node(const operator_type& opr,
                  expression_ptr branch0,
                  expression_ptr branch1,
                  expression_ptr branch2)
         : trinary_node<T>(opr, branch0, branch1, branch2)
         {}

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

         typedef expression_node<T>* expression_ptr;

         sf4_node(const operator_type& opr,
                  expression_ptr branch0,
                  expression_ptr branch1,
                  expression_ptr branch2,
                  expression_ptr branch3)
         : quaternary_node<T>(opr, branch0, branch1, branch2, branch3)
         {}

         inline T value() const override
         {
            const T x = quaternary_node<T>::branch_[0].first->value();
            const T y = quaternary_node<T>::branch_[1].first->value();
            const T z = quaternary_node<T>::branch_[2].first->value();
            const T w = quaternary_node<T>::branch_[3].first->value();

            return SpecialFunction::process(x, y, z, w);
         }
      };


} // namespace math_expr::details

#endif
