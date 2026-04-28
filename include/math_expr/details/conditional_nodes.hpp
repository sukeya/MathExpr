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

#ifndef MATH_EXPR_DETAILS_CONDITIONAL_NODES_HPP
#define MATH_EXPR_DETAILS_CONDITIONAL_NODES_HPP

#include "math_expr/details/literal_nodes.hpp"

namespace math_expr::details
{
      template <typename T>
      class conditional_node final : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef std::pair<expression_ptr,bool> branch_t;

         conditional_node(expression_ptr condition,
                          expression_ptr consequent,
                          expression_ptr alternative)
         {
            construct_branch_pair(condition_  , condition  );
            construct_branch_pair(consequent_ , consequent );
            construct_branch_pair(alternative_, alternative);
            assert(valid());
         }

         inline T value() const override
         {
            if (is_true(condition_))
               return consequent_.first->value();
            else
               return alternative_.first->value();
         }

         inline typename expression_node<T>::node_type type() const override
         {
            return expression_node<T>::e_conditional;
         }

         inline bool valid() const override
         {
            return
               condition_  .first && condition_  .first->valid() &&
               consequent_ .first && consequent_ .first->valid() &&
               alternative_.first && alternative_.first->valid() ;
         }

         void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) override
         {
            expression_node<T>::ndb_t::collect(condition_   , node_delete_list);
            expression_node<T>::ndb_t::collect(consequent_  , node_delete_list);
            expression_node<T>::ndb_t::collect(alternative_ , node_delete_list);
         }

         std::size_t node_depth() const override
         {
            return expression_node<T>::ndb_t::compute_node_depth
               (condition_, consequent_, alternative_);
         }

      private:

         branch_t condition_;
         branch_t consequent_;
         branch_t alternative_;
      };

      template <typename T>
      class cons_conditional_node final : public expression_node<T>
      {
      public:

         // Consequent only conditional statement node
         typedef expression_node<T>* expression_ptr;
         typedef std::pair<expression_ptr,bool> branch_t;

         cons_conditional_node(expression_ptr condition,
                               expression_ptr consequent)
         {
            construct_branch_pair(condition_ , condition );
            construct_branch_pair(consequent_, consequent);
            assert(valid());
         }

         inline T value() const override
         {
            if (is_true(condition_))
               return consequent_.first->value();
            else
               return std::numeric_limits<T>::quiet_NaN();
         }

         inline typename expression_node<T>::node_type type() const override
         {
            return expression_node<T>::e_conditional;
         }

         inline bool valid() const override
         {
            return
               condition_ .first && condition_ .first->valid() &&
               consequent_.first && consequent_.first->valid() ;
         }

         void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) override
         {
            expression_node<T>::ndb_t::collect(condition_  , node_delete_list);
            expression_node<T>::ndb_t::collect(consequent_ , node_delete_list);
         }

         std::size_t node_depth() const override
         {
            return expression_node<T>::ndb_t::
               compute_node_depth(condition_, consequent_);
         }

      private:

         branch_t condition_;
         branch_t consequent_;
      };

} // namespace math_expr::details

#endif
