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

#ifndef MATH_EXPR_DETAILS_STRING_CONCAT_NODE_HPP
#define MATH_EXPR_DETAILS_STRING_CONCAT_NODE_HPP

#include "math_expr/details/operator_nodes.hpp"

namespace math_expr::details::string_nodes
{
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
}

#endif
