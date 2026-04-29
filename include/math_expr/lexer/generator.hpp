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

#ifndef MATH_EXPR_LEXER_GENERATOR_HPP
#define MATH_EXPR_LEXER_GENERATOR_HPP

#include "math_expr/core/types.hpp"
#include "math_expr/core/string_utils.hpp"
#include "math_expr/lexer/token.hpp"

namespace math_expr::lexer
{
      class generator
      {
      public:

         typedef token token_t;
         typedef std::vector<token_t> token_list_t;
         typedef token_list_t::iterator token_list_itr_t;
         typedef core::char_t char_t;

         generator()
         : base_itr_(0)
         , s_itr_   (0)
         , s_end_   (0)
         {
            clear();
         }

         inline void clear()
         {
            base_itr_ = 0;
            s_itr_    = 0;
            s_end_    = 0;
            token_list_.clear();
            token_itr_ = token_list_.end();
            store_token_itr_ = token_list_.end();
         }

         inline bool process(const std::string& str)
         {
            base_itr_ = str.data();
            s_itr_    = str.data();
            s_end_    = str.data() + str.size();

            eof_token_.set_operator(token_t::e_eof, s_end_, s_end_, base_itr_);
            token_list_.clear();

            while (!is_end(s_itr_))
            {
               scan_token();

               if (!token_list_.empty() && token_list_.back().is_error())
                  return false;
            }

            return true;
         }

         inline bool empty() const
         {
            return token_list_.empty();
         }

         inline std::size_t size() const
         {
            return token_list_.size();
         }

         inline void begin()
         {
            token_itr_ = token_list_.begin();
            store_token_itr_ = token_list_.begin();
         }

         inline void store()
         {
            store_token_itr_ = token_itr_;
         }

         inline void restore()
         {
            token_itr_ = store_token_itr_;
         }

         inline token_t& next_token()
         {
            if (token_list_.end() != token_itr_)
            {
               return *token_itr_++;
            }
            else
               return eof_token_;
         }

         inline token_t& peek_next_token()
         {
            if (token_list_.end() != token_itr_)
            {
               return *token_itr_;
            }
            else
               return eof_token_;
         }

         inline token_t& operator[](const std::size_t& index)
         {
            if (index < token_list_.size())
            {
               return token_list_[index];
            }
            else
               return eof_token_;
         }

         inline token_t operator[](const std::size_t& index) const
         {
            if (index < token_list_.size())
            {
               return token_list_[index];
            }
            else
               return eof_token_;
         }

         inline bool finished() const
         {
            return (token_list_.end() == token_itr_);
         }

         inline void insert_front(token_t::token_type tk_type)
         {
            if (
                 !token_list_.empty() &&
                 (token_list_.end() != token_itr_)
               )
            {
               token_t t = *token_itr_;

               t.type     = tk_type;
               token_itr_ = token_list_.insert(token_itr_,t);
            }
         }

         inline std::string substr(const std::size_t& begin, const std::size_t& end) const
         {
            const core::char_cptr begin_itr = ((base_itr_ + begin) < s_end_) ? (base_itr_ + begin) : s_end_;
            const core::char_cptr end_itr   = ((base_itr_ + end  ) < s_end_) ? (base_itr_ + end  ) : s_end_;

            return std::string(begin_itr,end_itr);
         }

         inline std::string remaining() const
         {
            if (finished())
               return "";
            else if (token_list_.begin() != token_itr_)
               return std::string(base_itr_ + (token_itr_ - 1)->position, s_end_);
            else
               return std::string(base_itr_ + token_itr_->position, s_end_);
         }

      private:

         inline bool is_end(core::char_cptr itr) const
         {
            return (s_end_ == itr);
         }

         inline bool is_comment_start(core::char_cptr itr) const
         {
            if constexpr (::math_expr::core::build_options::kDisableComments)
            {
               return false;
            }

            const char_t c0 = *(itr + 0);
            const char_t c1 = *(itr + 1);

            if ('#' == c0)
               return true;
            else if (!is_end(itr + 1))
            {
               if (('/' == c0) && ('/' == c1)) return true;
               if (('/' == c0) && ('*' == c1)) return true;
            }
            return false;
         }

         inline void skip_whitespace()
         {
            while (!is_end(s_itr_) && core::is_whitespace(*s_itr_))
            {
               ++s_itr_;
            }
         }

         inline void skip_comments()
         {
            if constexpr (::math_expr::core::build_options::kDisableComments)
            {
               return;
            }

            // The following comment styles are supported:
            // 1. // .... \n
            // 2. #  .... \n
            // 3. /* .... */
            struct test
            {
               static inline bool comment_start(const char_t c0, const char_t c1, int& mode, int& incr)
               {
                  mode = 0;
                  if      ('#' == c0)    { mode = 1; incr = 1; }
                  else if ('/' == c0)
                  {
                     if      ('/' == c1) { mode = 1; incr = 2; }
                     else if ('*' == c1) { mode = 2; incr = 2; }
                  }
                  return (0 != mode);
               }

               static inline bool comment_end(const char_t c0, const char_t c1, int& mode)
               {
                  if (
                       ((1 == mode) && ('\n' == c0)) ||
                       ((2 == mode) && ( '*' == c0) && ('/' == c1))
                     )
                  {
                     mode = 0;
                     return true;
                  }
                  else
                     return false;
               }
            };

            int mode      = 0;
            int increment = 0;

            if (is_end(s_itr_))
               return;
            else if (!test::comment_start(*s_itr_, *(s_itr_ + 1), mode, increment))
               return;

            core::char_cptr cmt_start = s_itr_;

            s_itr_ += increment;

            while (!is_end(s_itr_))
            {
               if (core::is_invalid(*s_itr_))
               {
                  token_t t;
                  t.set_error(token::e_error, s_itr_, s_itr_ + 1, base_itr_);
                  token_list_.push_back(t);
                  return;
               }

               if ((1 == mode) && test::comment_end(*s_itr_, 0, mode))
               {
                  ++s_itr_;
                  return;
               }

               if ((2 == mode))
               {
                  if (!is_end((s_itr_ + 1)) && test::comment_end(*s_itr_, *(s_itr_ + 1), mode))
                  {
                     s_itr_ += 2;
                     return;
                  }
               }

               ++s_itr_;
            }

            if (2 == mode)
            {
               token_t t;
               t.set_error(token::e_error, cmt_start, cmt_start + mode, base_itr_);
               token_list_.push_back(t);
            }
         }

         inline bool next_is_digit(const core::char_cptr itr) const
         {
            return ((itr + 1) != s_end_) &&
                   core::is_digit(*(itr + 1));
         }

         inline void scan_token()
         {
            const char_t c = *s_itr_;

            if (core::is_whitespace(c))
            {
               skip_whitespace();
               return;
            }
            else if (is_comment_start(s_itr_))
            {
               skip_comments();
               return;
            }
            else if (core::is_operator_char(c))
            {
               scan_operator();
               return;
            }
            else if (core::is_letter(c))
            {
               scan_symbol();
               return;
            }
            else if (('.' == c) && !next_is_digit(s_itr_))
            {
               scan_operator();
               return;
            }
            else if (core::is_digit(c) || ('.' == c))
            {
               scan_number();
               return;
            }
            else if ('$' == c)
            {
               scan_special_function();
               return;
            }
            #ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
            else if ('\'' == c)
            {
               scan_string();
               return;
            }
            #endif
            else if ('~' == c)
            {
               token_t t;
               t.set_symbol(s_itr_, s_itr_ + 1, base_itr_);
               token_list_.push_back(t);
               ++s_itr_;
               return;
            }
            else
            {
               token_t t;
               t.set_error(token::e_error, s_itr_, s_itr_ + 2, base_itr_);
               token_list_.push_back(t);
               ++s_itr_;
            }
         }

         inline void scan_operator()
         {
            token_t t;

            const char_t c0 = s_itr_[0];

            if (!is_end(s_itr_ + 1))
            {
               const char_t c1 = s_itr_[1];

               if (!is_end(s_itr_ + 2))
               {
                  const char_t c2 = s_itr_[2];

                  if ((c0 == '<') && (c1 == '=') && (c2 == '>'))
                  {
                     t.set_operator(token_t::e_swap, s_itr_, s_itr_ + 3, base_itr_);
                     token_list_.push_back(t);
                     s_itr_ += 3;
                     return;
                  }
               }

               token_t::token_type ttype = token_t::e_none;

               if      ((c0 == '<') && (c1 == '=')) ttype = token_t::e_lte;
               else if ((c0 == '>') && (c1 == '=')) ttype = token_t::e_gte;
               else if ((c0 == '<') && (c1 == '>')) ttype = token_t::e_ne;
               else if ((c0 == '!') && (c1 == '=')) ttype = token_t::e_ne;
               else if ((c0 == '=') && (c1 == '=')) ttype = token_t::e_eq;
               else if ((c0 == ':') && (c1 == '=')) ttype = token_t::e_assign;
               else if ((c0 == '<') && (c1 == '<')) ttype = token_t::e_shl;
               else if ((c0 == '>') && (c1 == '>')) ttype = token_t::e_shr;
               else if ((c0 == '+') && (c1 == '=')) ttype = token_t::e_addass;
               else if ((c0 == '-') && (c1 == '=')) ttype = token_t::e_subass;
               else if ((c0 == '*') && (c1 == '=')) ttype = token_t::e_mulass;
               else if ((c0 == '/') && (c1 == '=')) ttype = token_t::e_divass;
               else if ((c0 == '%') && (c1 == '=')) ttype = token_t::e_modass;

               if (token_t::e_none != ttype)
               {
                  t.set_operator(ttype, s_itr_, s_itr_ + 2, base_itr_);
                  token_list_.push_back(t);
                  s_itr_ += 2;
                  return;
               }
            }

            if ('<' == c0)
               t.set_operator(token_t::e_lt , s_itr_, s_itr_ + 1, base_itr_);
            else if ('>' == c0)
               t.set_operator(token_t::e_gt , s_itr_, s_itr_ + 1, base_itr_);
            else if (';' == c0)
               t.set_operator(token_t::e_eof, s_itr_, s_itr_ + 1, base_itr_);
            else if ('&' == c0)
               t.set_symbol(s_itr_, s_itr_ + 1, base_itr_);
            else if ('|' == c0)
               t.set_symbol(s_itr_, s_itr_ + 1, base_itr_);
            else
               t.set_operator(token_t::token_type(c0), s_itr_, s_itr_ + 1, base_itr_);

            token_list_.push_back(t);
            ++s_itr_;
         }

         inline void scan_symbol()
         {
            core::char_cptr initial_itr = s_itr_;

            while (!is_end(s_itr_))
            {
               if (!core::is_letter_or_digit(*s_itr_) && ('_' != (*s_itr_)))
               {
                  if ('.' != (*s_itr_))
                     break;
                  /*
                     Permit symbols that contain a 'dot'
                     Allowed   : abc.xyz, a123.xyz, abc.123, abc_.xyz a123_.xyz abc._123
                     Disallowed: .abc, abc.<white-space>, abc.<eof>, abc.<operator +,-,*,/...>
                  */
                  if (
                       (s_itr_ != initial_itr)                     &&
                       !is_end(s_itr_ + 1)                         &&
                       !core::is_letter_or_digit(*(s_itr_ + 1)) &&
                       ('_' != (*(s_itr_ + 1)))
                     )
                     break;
               }

               ++s_itr_;
            }

            token_t t;
            t.set_symbol(initial_itr, s_itr_, base_itr_);
            token_list_.push_back(t);
         }

         inline void scan_number()
         {
            /*
               Attempt to match a valid numeric value in one of the following formats:
               (01) 123456
               (02) 123456.
               (03) 123.456
               (04) 123.456e3
               (05) 123.456E3
               (06) 123.456e+3
               (07) 123.456E+3
               (08) 123.456e-3
               (09) 123.456E-3
               (00) .1234
               (11) .1234e3
               (12) .1234E+3
               (13) .1234e+3
               (14) .1234E-3
               (15) .1234e-3
            */

            core::char_cptr initial_itr = s_itr_;
            bool dot_found                 = false;
            bool e_found                   = false;
            bool post_e_sign_found         = false;
            bool post_e_digit_found        = false;
            token_t t;

            while (!is_end(s_itr_))
            {
               if ('.' == (*s_itr_))
               {
                  if (dot_found)
                  {
                     t.set_error(token::e_err_number, initial_itr, s_itr_, base_itr_);
                     token_list_.push_back(t);

                     return;
                  }

                  dot_found = true;
                  ++s_itr_;

                  continue;
               }
               else if ('e' == std::tolower(*s_itr_))
               {
                  const char_t& c = *(s_itr_ + 1);

                  if (is_end(s_itr_ + 1))
                  {
                     t.set_error(token::e_err_number, initial_itr, s_itr_, base_itr_);
                     token_list_.push_back(t);

                     return;
                  }
                  else if (
                            ('+' != c) &&
                            ('-' != c) &&
                            !core::is_digit(c)
                          )
                  {
                     t.set_error(token::e_err_number, initial_itr, s_itr_, base_itr_);
                     token_list_.push_back(t);

                     return;
                  }

                  e_found = true;
                  ++s_itr_;

                  continue;
               }
               else if (e_found && core::is_sign(*s_itr_) && !post_e_digit_found)
               {
                  if (post_e_sign_found)
                  {
                     t.set_error(token::e_err_number, initial_itr, s_itr_, base_itr_);
                     token_list_.push_back(t);

                     return;
                  }

                  post_e_sign_found = true;
                  ++s_itr_;

                  continue;
               }
               else if (e_found && core::is_digit(*s_itr_))
               {
                  post_e_digit_found = true;
                  ++s_itr_;

                  continue;
               }
               else if (('.' != (*s_itr_)) && !core::is_digit(*s_itr_))
                  break;
               else
                  ++s_itr_;
            }

            t.set_numeric(initial_itr, s_itr_, base_itr_);
            token_list_.push_back(t);

            return;
         }

         inline void scan_special_function()
         {
            core::char_cptr initial_itr = s_itr_;
            token_t t;

            // $fdd(x,x,x) = at least 11 chars
            if (std::distance(s_itr_,s_end_) < 11)
            {
               t.set_error(
                  token::e_err_sfunc,
                  initial_itr, std::min(initial_itr + 11, s_end_),
                  base_itr_);
               token_list_.push_back(t);

               return;
            }

            if (
                 !(('$' == *s_itr_)                       &&
                   (core::imatch  ('f',*(s_itr_ + 1))) &&
                   (core::is_digit(*(s_itr_ + 2)))     &&
                   (core::is_digit(*(s_itr_ + 3))))
               )
            {
               t.set_error(
                  token::e_err_sfunc,
                  initial_itr, std::min(initial_itr + 4, s_end_),
                  base_itr_);
               token_list_.push_back(t);

               return;
            }

            s_itr_ += 4; // $fdd = 4chars

            t.set_symbol(initial_itr, s_itr_, base_itr_);
            token_list_.push_back(t);

            return;
         }

         #ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
         inline void scan_string()
         {
            core::char_cptr initial_itr = s_itr_ + 1;
            token_t t;

            if (std::distance(s_itr_,s_end_) < 2)
            {
               t.set_error(token::e_err_string, s_itr_, s_end_, base_itr_);
               token_list_.push_back(t);

               return;
            }

            ++s_itr_;

            bool escaped_found = false;
            bool escaped = false;

            while (!is_end(s_itr_))
            {
               if (!core::is_valid_string_char(*s_itr_))
               {
                  t.set_error(token::e_err_string, initial_itr, s_itr_, base_itr_);
                  token_list_.push_back(t);

                  return;
               }
               else if (!escaped && ('\\' == *s_itr_))
               {
                  escaped_found = true;
                  escaped = true;
                  ++s_itr_;

                  continue;
               }
               else if (!escaped)
               {
                  if ('\'' == *s_itr_)
                     break;
               }
               else if (escaped)
               {
                  if (
                       !is_end(s_itr_) && ('0' == *(s_itr_)) &&
                       ((s_itr_ + 4) <= s_end_)
                     )
                  {
                     const bool x_separator = ('X' == std::toupper(*(s_itr_ + 1)));

                     const bool both_digits = core::is_hex_digit(*(s_itr_ + 2)) &&
                                              core::is_hex_digit(*(s_itr_ + 3)) ;

                     if (!(x_separator && both_digits))
                     {
                        t.set_error(token::e_err_string, initial_itr, s_itr_, base_itr_);
                        token_list_.push_back(t);

                        return;
                     }
                     else
                        s_itr_ += 3;
                  }

                  escaped = false;
               }

               ++s_itr_;
            }

            if (is_end(s_itr_))
            {
               t.set_error(token::e_err_string, initial_itr, s_itr_, base_itr_);
               token_list_.push_back(t);

               return;
            }

            if (!escaped_found)
               t.set_string(initial_itr, s_itr_, base_itr_);
            else
            {
               std::string parsed_string(initial_itr,s_itr_);

               if (!core::cleanup_escapes(parsed_string))
               {
                  t.set_error(token::e_err_string, initial_itr, s_itr_, base_itr_);
                  token_list_.push_back(t);

                  return;
               }

               t.set_string(
                  parsed_string,
                  static_cast<std::size_t>(std::distance(base_itr_,initial_itr)));
            }

            token_list_.push_back(t);
            ++s_itr_;

            return;
         }
         #endif

      private:

         token_list_t       token_list_;
         token_list_itr_t   token_itr_;
         token_list_itr_t   store_token_itr_;
         token_t            eof_token_;
         core::char_cptr base_itr_;
         core::char_cptr s_itr_;
         core::char_cptr s_end_;

         friend class token_scanner;
         friend class token_modifier;
         friend class token_inserter;
         friend class token_joiner;
      }; // class generator

} // namespace math_expr::lexer

#endif
