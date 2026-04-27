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

#ifndef MATH_EXPR_CORE_STRING_UTILS_HPP
#define MATH_EXPR_CORE_STRING_UTILS_HPP

namespace math_expr::details
{
      #ifndef math_expr_disable_caseinsensitivity
      inline void case_normalise(std::string& s)
      {
         for (std::size_t i = 0; i < s.size(); ++i)
         {
            s[i] = static_cast<std::string::value_type>(std::tolower(s[i]));
         }
      }

      inline bool imatch(const char_t c1, const char_t c2)
      {
         return std::tolower(c1) == std::tolower(c2);
      }

      inline bool imatch(const std::string& s1, const std::string& s2)
      {
         if (s1.size() == s2.size())
         {
            for (std::size_t i = 0; i < s1.size(); ++i)
            {
               if (std::tolower(s1[i]) != std::tolower(s2[i]))
               {
                  return false;
               }
            }

            return true;
         }

         return false;
      }

      struct ilesscompare
      {
         inline bool operator() (const std::string& s1, const std::string& s2) const
         {
            const std::size_t length = std::min(s1.size(),s2.size());

            for (std::size_t i = 0; i < length; ++i)
            {
               const char_t c1 = static_cast<char_t>(std::tolower(s1[i]));
               const char_t c2 = static_cast<char_t>(std::tolower(s2[i]));

               if (c1 < c2)
                  return true;
               else if (c2 < c1)
                  return false;
            }

            return s1.size() < s2.size();
         }
      };

      #else
      inline void case_normalise(std::string&)
      {}

      inline bool imatch(const char_t c1, const char_t c2)
      {
         return c1 == c2;
      }

      inline bool imatch(const std::string& s1, const std::string& s2)
      {
         return s1 == s2;
      }

      struct ilesscompare
      {
         inline bool operator() (const std::string& s1, const std::string& s2) const
         {
            return s1 < s2;
         }
      };
      #endif

      inline bool is_valid_sf_symbol(const std::string& symbol)
      {
         // Special function: $f12 or $F34
         return (4 == symbol.size())  &&
                ('$' == symbol[0])    &&
                imatch('f',symbol[1]) &&
                is_digit(symbol[2])   &&
                is_digit(symbol[3]);
      }

      inline const char_t& front(const std::string& s)
      {
         return s[0];
      }

      inline const char_t& back(const std::string& s)
      {
         return s[s.size() - 1];
      }

      template <typename SignedType>
      inline std::string to_str_impl(SignedType i)
      {
         if (0 == i)
            return std::string("0");

         std::string result;

         const int sign = (i < 0) ? -1 : 1;

         for ( ; i; i /= 10)
         {
            result += '0' + static_cast<char_t>(sign * (i % 10));
         }

         if (sign < 0)
         {
            result += '-';
         }

         std::reverse(result.begin(), result.end());

         return result;
      }

      inline std::string to_str(int i)
      {
         return to_str_impl(i);
      }

      inline std::string to_str(std::size_t i)
      {
         return to_str_impl(static_cast<long long int>(i));
      }

      inline bool is_hex_digit(const uchar_t digit)
      {
         return (('0' <= digit) && (digit <= '9')) ||
                (('A' <= digit) && (digit <= 'F')) ||
                (('a' <= digit) && (digit <= 'f')) ;
      }

      inline uchar_t hex_to_bin(uchar_t h)
      {
         if (('0' <= h) && (h <= '9'))
            return (h - '0');
         else
            return static_cast<uchar_t>(std::toupper(h) - 'A' + 10);
      }

      template <typename Iterator>
      inline bool parse_hex(Iterator& itr, Iterator end,
                            char_t& result)
      {
         if (
              (end ==  (itr    ))               ||
              (end ==  (itr + 1))               ||
              (end ==  (itr + 2))               ||
              (end ==  (itr + 3))               ||
              ('0' != *(itr    ))               ||
              ('X' != std::toupper(*(itr + 1))) ||
              (!is_hex_digit(*(itr + 2)))       ||
              (!is_hex_digit(*(itr + 3)))
            )
         {
            return false;
         }

         result = static_cast<char_t>(
                     hex_to_bin(static_cast<uchar_t>(*(itr + 2))) << 4 |
                     hex_to_bin(static_cast<uchar_t>(*(itr + 3)))) ;

         return true;
      }

      inline bool cleanup_escapes(std::string& s)
      {
         typedef std::string::iterator str_itr_t;

         str_itr_t itr1 = s.begin();
         str_itr_t itr2 = s.begin();
         str_itr_t end  = s.end  ();

         std::size_t removal_count  = 0;

         while (end != itr1)
         {
            if ('\\' == (*itr1))
            {
               if (end == ++itr1)
               {
                  return false;
               }
               else if (parse_hex(itr1, end, *itr2))
               {
                  itr1 += 4;
                  itr2 += 1;
                  removal_count += 4;
               }
               else if ('a' == (*itr1)) { (*itr2++) = '\a'; ++itr1; ++removal_count; }
               else if ('b' == (*itr1)) { (*itr2++) = '\b'; ++itr1; ++removal_count; }
               else if ('f' == (*itr1)) { (*itr2++) = '\f'; ++itr1; ++removal_count; }
               else if ('n' == (*itr1)) { (*itr2++) = '\n'; ++itr1; ++removal_count; }
               else if ('r' == (*itr1)) { (*itr2++) = '\r'; ++itr1; ++removal_count; }
               else if ('t' == (*itr1)) { (*itr2++) = '\t'; ++itr1; ++removal_count; }
               else if ('v' == (*itr1)) { (*itr2++) = '\v'; ++itr1; ++removal_count; }
               else if ('0' == (*itr1)) { (*itr2++) = '\0'; ++itr1; ++removal_count; }
               else
               {
                  (*itr2++) = (*itr1++);
                  ++removal_count;
               }

               continue;
            }
            else
               (*itr2++) = (*itr1++);
         }

         if ((removal_count > s.size()) || (0 == removal_count))
            return false;

         s.resize(s.size() - removal_count);

         return true;
      }

      class build_string
      {
      public:

         explicit build_string(const std::size_t& initial_size = 64)
         {
            data_.reserve(initial_size);
         }

         inline build_string& operator << (const std::string& s)
         {
            data_ += s;
            return (*this);
         }

         inline build_string& operator << (char_cptr s)
         {
            data_ += std::string(s);
            return (*this);
         }

         inline operator std::string () const
         {
            return data_;
         }

         inline std::string as_string() const
         {
            return data_;
         }

      private:

         std::string data_;
      };

      static const std::string reserved_words[] =
      {
         "assert",  "break", "case", "continue", "const",  "default",
         "false", "for", "if", "else", "ilike", "in", "like",  "and",
         "nand",  "nor",  "not",  "null",  "or",  "repeat", "return",
         "shl",  "shr",  "swap",  "switch",  "true",  "until", "var",
         "while", "xnor", "xor", "&", "|"
      };

      static const std::size_t reserved_words_size = sizeof(reserved_words) / sizeof(std::string);

      static const std::string reserved_symbols[] =
      {
         "abs", "acos",  "acosh", "and",  "asin", "asinh",  "assert",
         "atan", "atanh",  "atan2", "avg",  "break", "case",  "ceil",
         "clamp", "continue", "const",  "cos", "cosh", "cot",  "csc",
         "default",  "deg2grad", "deg2rad",  "equal", "erf",  "erfc",
         "exp", "expm1", "false", "floor", "for", "frac", "grad2deg",
         "hypot", "iclamp", "if",  "else", "ilike", "in",  "inrange",
         "like",  "log",  "log10", "log2",  "logn",  "log1p", "mand",
         "max", "min",  "mod", "mor",  "mul", "ncdf",  "nand", "nor",
         "not",   "not_equal",   "null",   "or",   "pow",  "rad2deg",
         "repeat", "return", "root", "round", "roundn", "sec", "sgn",
         "shl", "shr", "sin", "sinc", "sinh", "sqrt", "sum",  "swap",
         "switch", "tan",  "tanh", "true",  "trunc", "until",  "var",
         "while", "xnor", "xor", "&", "|"
      };

      static const std::size_t reserved_symbols_size = sizeof(reserved_symbols) / sizeof(std::string);

      static const std::string base_function_list[] =
      {
         "abs", "acos",  "acosh", "asin",  "asinh", "atan",  "atanh",
         "atan2",  "avg",  "ceil",  "clamp",  "cos",  "cosh",  "cot",
         "csc",  "equal",  "erf",  "erfc",  "exp",  "expm1", "floor",
         "frac", "hypot", "iclamp",  "like", "log", "log10",  "log2",
         "logn", "log1p", "mand", "max", "min", "mod", "mor",  "mul",
         "ncdf",  "pow",  "root",  "round",  "roundn",  "sec", "sgn",
         "sin", "sinc", "sinh", "sqrt", "sum", "swap", "tan", "tanh",
         "trunc",  "not_equal",  "inrange",  "deg2grad",   "deg2rad",
         "rad2deg", "grad2deg"
      };

      static const std::size_t base_function_list_size = sizeof(base_function_list) / sizeof(std::string);

      static const std::string logic_ops_list[] =
      {
         "and", "nand", "nor", "not", "or",  "xnor", "xor", "&", "|"
      };

      static const std::size_t logic_ops_list_size = sizeof(logic_ops_list) / sizeof(std::string);

      static const std::string cntrl_struct_list[] =
      {
         "if", "switch", "for", "while", "repeat", "return"
      };

      static const std::size_t cntrl_struct_list_size = sizeof(cntrl_struct_list) / sizeof(std::string);

      static const std::string arithmetic_ops_list[] =
      {
         "+", "-", "*", "/", "%", "^"
      };

      static const std::size_t arithmetic_ops_list_size = sizeof(arithmetic_ops_list) / sizeof(std::string);

      static const std::string assignment_ops_list[] =
      {
         ":=", "+=", "-=",
         "*=", "/=", "%="
      };

      static const std::size_t assignment_ops_list_size = sizeof(assignment_ops_list) / sizeof(std::string);

      static const std::string inequality_ops_list[] =
      {
         "<",  "<=", "==",
         "=",  "!=", "<>",
         ">=",  ">"
      };

      static const std::size_t inequality_ops_list_size = sizeof(inequality_ops_list) / sizeof(std::string);

      inline bool is_reserved_word(const std::string& symbol)
      {
         for (std::size_t i = 0; i < reserved_words_size; ++i)
         {
            if (imatch(symbol, reserved_words[i]))
            {
               return true;
            }
         }

         return false;
      }

      inline bool is_reserved_symbol(const std::string& symbol)
      {
         for (std::size_t i = 0; i < reserved_symbols_size; ++i)
         {
            if (imatch(symbol, reserved_symbols[i]))
            {
               return true;
            }
         }

         return false;
      }

      inline bool is_base_function(const std::string& function_name)
      {
         for (std::size_t i = 0; i < base_function_list_size; ++i)
         {
            if (imatch(function_name, base_function_list[i]))
            {
               return true;
            }
         }

         return false;
      }

      inline bool is_control_struct(const std::string& cntrl_strct)
      {
         for (std::size_t i = 0; i < cntrl_struct_list_size; ++i)
         {
            if (imatch(cntrl_strct, cntrl_struct_list[i]))
            {
               return true;
            }
         }

         return false;
      }

      inline bool is_logic_opr(const std::string& lgc_opr)
      {
         for (std::size_t i = 0; i < logic_ops_list_size; ++i)
         {
            if (imatch(lgc_opr, logic_ops_list[i]))
            {
               return true;
            }
         }

         return false;
      }

      struct cs_match
      {
         static inline bool cmp(const char_t c0, const char_t c1)
         {
            return (c0 == c1);
         }
      };

      struct cis_match
      {
         static inline bool cmp(const char_t c0, const char_t c1)
         {
            return (std::tolower(c0) == std::tolower(c1));
         }
      };

      template <typename Iterator, typename Compare>
      inline bool match_impl(const Iterator pattern_begin,
                             const Iterator pattern_end  ,
                             const Iterator data_begin   ,
                             const Iterator data_end     ,
                             const typename std::iterator_traits<Iterator>::value_type& zero_or_more,
                             const typename std::iterator_traits<Iterator>::value_type& exactly_one )
      {
         typedef typename std::iterator_traits<Iterator>::value_type type;

         const Iterator null_itr(0);

         Iterator p_itr  = pattern_begin;
         Iterator d_itr  = data_begin;
         Iterator np_itr = null_itr;
         Iterator nd_itr = null_itr;

         for ( ; ; )
         {
            if (p_itr != pattern_end)
            {
               const type c = *(p_itr);

               if ((data_end != d_itr) && (Compare::cmp(c,*(d_itr)) || (exactly_one == c)))
               {
                  ++d_itr;
                  ++p_itr;
                  continue;
               }
               else if (zero_or_more == c)
               {
                  while ((pattern_end != p_itr) && (zero_or_more == *(p_itr)))
                  {
                     ++p_itr;
                  }

                  const type d = *(p_itr);

                  while ((data_end != d_itr) && !(Compare::cmp(d,*(d_itr)) || (exactly_one == d)))
                  {
                     ++d_itr;
                  }

                  // set backtrack iterators
                  np_itr = p_itr - 1;
                  nd_itr = d_itr + 1;

                  continue;
               }
            }
            else if (data_end == d_itr)
               break;

            if ((data_end == d_itr) || (null_itr == nd_itr))
                return false;

            p_itr = np_itr;
            d_itr = nd_itr;
         }

         return true;
      }

      inline bool wc_match(const std::string& wild_card,
                           const std::string& str)
      {
         return match_impl<char_cptr,cs_match>
                (
                   wild_card.data(),
                   wild_card.data() + wild_card.size(),
                   str.data(),
                   str.data() + str.size(),
                   '*', '?'
                );
      }

      inline bool wc_imatch(const std::string& wild_card,
                            const std::string& str)
      {
         return match_impl<char_cptr,cis_match>
                (
                   wild_card.data(),
                   wild_card.data() + wild_card.size(),
                   str.data(),
                   str.data() + str.size(),
                   '*', '?'
                );
      }

      inline bool sequence_match(const std::string& pattern,
                                 const std::string& str,
                                 std::size_t&       diff_index,
                                 char_t&            diff_value)
      {
         if (str.empty())
         {
            return ("Z" == pattern);
         }
         else if ('*' == pattern[0])
            return false;

         typedef std::string::const_iterator itr_t;

         itr_t p_itr = pattern.begin();
         itr_t s_itr = str    .begin();

         const itr_t p_end = pattern.end();
         const itr_t s_end = str    .end();

         while ((s_end != s_itr) && (p_end != p_itr))
         {
            if ('*' == (*p_itr))
            {
               const char_t target = static_cast<char_t>(std::toupper(*(p_itr - 1)));

               if ('*' == target)
               {
                  diff_index = static_cast<std::size_t>(std::distance(str.begin(),s_itr));
                  diff_value = static_cast<char_t>(std::toupper(*p_itr));

                  return false;
               }
               else
                  ++p_itr;

               while (s_itr != s_end)
               {
                  if (target != std::toupper(*s_itr))
                     break;
                  else
                     ++s_itr;
               }

               continue;
            }
            else if (
                      ('?' != *p_itr) &&
                      std::toupper(*p_itr) != std::toupper(*s_itr)
                    )
            {
               diff_index = static_cast<std::size_t>(std::distance(str.begin(),s_itr));
               diff_value = static_cast<char_t>(std::toupper(*p_itr));

               return false;
            }

            ++p_itr;
            ++s_itr;
         }

         return (
                  (s_end == s_itr) &&
                  (
                    (p_end ==  p_itr) ||
                    ('*'   == *p_itr)
                  )
                );
      }

} // namespace math_expr::details

#endif
