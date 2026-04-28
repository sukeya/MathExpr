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

#ifndef MATH_EXPR_PARSER_ERROR_HPP
#define MATH_EXPR_PARSER_ERROR_HPP

#include "math_expr/core/std_includes.hpp"
#include "math_expr/core/macros.hpp"
#include "math_expr/core/types.hpp"
#include "math_expr/lexer/token.hpp"

namespace math_expr
{
   namespace parser_error
   {
      enum error_mode
      {
         e_unknown   = 0,
         e_syntax    = 1,
         e_token     = 2,
         e_numeric   = 4,
         e_symtab    = 5,
         e_lexer     = 6,
         e_synthesis = 7,
         e_helper    = 8,
         e_parser    = 9
      };

      struct type
      {
         type()
         : mode(parser_error::e_unknown)
         , line_no  (0)
         , column_no(0)
         {}

         lexer::token token;
         error_mode mode;
         std::string diagnostic;
         std::string src_location;
         std::string error_line;
         std::size_t line_no;
         std::size_t column_no;
      };

      inline type make_error(const error_mode mode,
                             const std::string& diagnostic   = "",
                             const std::string& src_location = "")
      {
         type t;
         t.mode         = mode;
         t.token.type   = lexer::token::e_error;
         t.diagnostic   = diagnostic;
         t.src_location = src_location;
         math_expr_debug(("%s\n", diagnostic .c_str()));
         return t;
      }

      inline type make_error(const error_mode mode,
                             const lexer::token& tk,
                             const std::string& diagnostic   = "",
                             const std::string& src_location = "")
      {
         type t;
         t.mode         = mode;
         t.token        = tk;
         t.diagnostic   = diagnostic;
         t.src_location = src_location;
         math_expr_debug(("%s\n", diagnostic .c_str()));
         return t;
      }

      inline std::string to_str(error_mode mode)
      {
         switch (mode)
         {
            case e_unknown : return std::string("Unknown Error");
            case e_syntax  : return std::string("Syntax Error" );
            case e_token   : return std::string("Token Error"  );
            case e_numeric : return std::string("Numeric Error");
            case e_symtab  : return std::string("Symbol Error" );
            case e_lexer   : return std::string("Lexer Error"  );
            case e_helper  : return std::string("Helper Error" );
            case e_parser  : return std::string("Parser Error" );
            default        : return std::string("Unknown Error");
         }
      }

      inline bool update_error(type& error, const std::string& expression)
      {
         if (
              expression.empty()                         ||
              (error.token.position > expression.size()) ||
              (std::numeric_limits<std::size_t>::max() == error.token.position)
            )
         {
            return false;
         }

         std::size_t error_line_start = 0;

         for (std::size_t i = error.token.position; i > 0; --i)
         {
            const details::char_t c = expression[i];

            if (('\n' == c) || ('\r' == c))
            {
               error_line_start = i + 1;
               break;
            }
         }

         std::size_t next_nl_position = std::min(expression.size(),
                                                 expression.find_first_of('\n',error.token.position + 1));

         error.column_no  = error.token.position - error_line_start;
         error.error_line = expression.substr(error_line_start,
                                              next_nl_position - error_line_start);

         error.line_no = 0;

         for (std::size_t i = 0; i < next_nl_position; ++i)
         {
            if ('\n' == expression[i])
               ++error.line_no;
         }

         return true;
      }

      inline void dump_error(const type& error)
      {
         printf("Position: %02d   Type: [%s]   Msg: %s\n",
                static_cast<int>(error.token.position),
                math_expr::parser_error::to_str(error.mode).c_str(),
                error.diagnostic.c_str());
      }
   }

   namespace details
   {
      template <typename Parser>
      inline void disable_type_checking(Parser& p)
      {
         p.state_.type_check_enabled = false;
      }
   }

} // namespace math_expr

#endif
