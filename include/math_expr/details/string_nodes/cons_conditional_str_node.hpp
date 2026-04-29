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

#ifndef MATH_EXPR_DETAILS_CONS_CONDITIONAL_STR_NODE_HPP
#define MATH_EXPR_DETAILS_CONS_CONDITIONAL_STR_NODE_HPP

#include "math_expr/details/operator_nodes.hpp"

namespace math_expr::details::string_nodes
{
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
         : binary_node<T>(core::operators::operator_type::default_op, consequent, condition)
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

            initialised_ = str0_base_ptr_ && str0_range_ptr_;
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

         core::char_cptr base() const
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
}

#endif
