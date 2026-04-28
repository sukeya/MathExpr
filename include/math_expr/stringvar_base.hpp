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

#ifndef MATH_EXPR_STRINGVAR_BASE_HPP
#define MATH_EXPR_STRINGVAR_BASE_HPP

#include "math_expr/core/std_includes.hpp"
#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
#include "math_expr/details/string_nodes.hpp"
#endif

namespace math_expr
{
   #ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
   template <typename T>
   class stringvar_base
   {
   public:

      typedef typename details::string_nodes::stringvar_node<T> stringvar_node_t;

      stringvar_base(const std::string& name, stringvar_node_t* svn)
      : name_(name)
      , string_varnode_(svn)
      {}

      bool valid() const
      {
         return !name_.empty() && (0 != string_varnode_);
      }

      std::string name() const
      {
         assert(string_varnode_);
         return name_;
      }

      void rebase(std::string& s)
      {
         assert(string_varnode_);
         string_varnode_->rebase(s);
      }

   private:

      std::string name_;
      stringvar_node_t* string_varnode_;
   };
   #endif

   template <typename T> class parser;
   template <typename T> class expression_helper;

} // namespace math_expr

#endif
