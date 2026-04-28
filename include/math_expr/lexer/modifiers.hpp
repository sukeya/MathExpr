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

#ifndef MATH_EXPR_LEXER_MODIFIERS_HPP
#define MATH_EXPR_LEXER_MODIFIERS_HPP

#include "math_expr/lexer/scanners.hpp"

namespace math_expr::lexer
{
      class token_inserter : public helper_interface
      {
      public:

         explicit token_inserter(const std::size_t& stride)
         : stride_(stride)
         {
            if (stride > 5)
            {
               throw std::invalid_argument("token_inserter() - Invalid stride value");
            }
         }

         inline std::size_t process(generator& g) override
         {
            if (g.token_list_.empty())
               return 0;
            else if (g.token_list_.size() < stride_)
               return 0;

            std::size_t changes = 0;

            typedef std::pair<std::size_t, token> insert_t;
            std::vector<insert_t> insert_list;
            insert_list.reserve(10000);

            for (std::size_t i = 0; i < (g.token_list_.size() - stride_ + 1); ++i)
            {
               int insert_index = -1;
               token t;

               switch (stride_)
               {
                  case 1 : insert_index = insert(g.token_list_[i],t);
                           break;

                  case 2 : insert_index = insert(g.token_list_[i], g.token_list_[i + 1], t);
                           break;

                  case 3 : insert_index = insert(g.token_list_[i], g.token_list_[i + 1], g.token_list_[i + 2], t);
                           break;

                  case 4 : insert_index = insert(g.token_list_[i], g.token_list_[i + 1], g.token_list_[i + 2], g.token_list_[i + 3], t);
                           break;

                  case 5 : insert_index = insert(g.token_list_[i], g.token_list_[i + 1], g.token_list_[i + 2], g.token_list_[i + 3], g.token_list_[i + 4], t);
                           break;
               }

               if ((insert_index >= 0) && (insert_index <= (static_cast<int>(stride_) + 1)))
               {
                  insert_list.push_back(insert_t(i, t));
                  changes++;
               }
            }

            if (!insert_list.empty())
            {
               generator::token_list_t token_list;

               std::size_t insert_index = 0;

               for (std::size_t i = 0; i < g.token_list_.size(); ++i)
               {
                  token_list.push_back(g.token_list_[i]);

                  if (
                       (insert_index < insert_list.size()) &&
                       (insert_list[insert_index].first == i)
                     )
                  {
                     token_list.push_back(insert_list[insert_index].second);
                     insert_index++;
                  }
               }

               std::swap(g.token_list_,token_list);
            }

            return changes;
         }

         #define token_inserter_empty_body \
         {                                 \
            return -1;                     \
         }                                 \

         inline virtual int insert(const token&, token&)
         token_inserter_empty_body

         inline virtual int insert(const token&, const token&, token&)
         token_inserter_empty_body

         inline virtual int insert(const token&, const token&, const token&, token&)
         token_inserter_empty_body

         inline virtual int insert(const token&, const token&, const token&, const token&, token&)
         token_inserter_empty_body

         inline virtual int insert(const token&, const token&, const token&, const token&, const token&, token&)
         token_inserter_empty_body

         #undef token_inserter_empty_body

      private:

         const std::size_t stride_;
      };

} // namespace math_expr::lexer

#endif
