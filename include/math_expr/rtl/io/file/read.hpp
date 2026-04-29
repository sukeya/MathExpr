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

#ifndef MATH_EXPR_RTL_IO_FILE_READ_HPP
#define MATH_EXPR_RTL_IO_FILE_READ_HPP

#include "math_expr/rtl/io/file/helper.hpp"

namespace math_expr::rtl::io::file
{
   template <typename T>
   class read final : public math_expr::igeneric_function<T>
   {
   public:

      typedef typename math_expr::igeneric_function<T> igfun_t;
      typedef typename igfun_t::parameter_list_t    parameter_list_t;
      typedef typename igfun_t::generic_type        generic_type;
      typedef typename generic_type::string_view    string_t;
      typedef typename generic_type::scalar_view    scalar_t;
      typedef typename generic_type::vector_view    vector_t;

      using igfun_t::operator();

      read()
      : igfun_t("TS|TST|TV|TVT")
      { details::perform_check<T>(); }

      inline T operator() (const std::size_t& ps_index, parameter_list_t parameters) override
      {
         details::file_descriptor* fd = details::decode_handle(scalar_t(parameters[0])());

         if (nullptr == fd)
         {
            return T(0);
         }

         switch (ps_index)
         {
            case 0  : {
                         string_t buffer(parameters[1]);
                         const std::size_t amount = buffer.size();
                         return T(fd->read(buffer,amount) ? 1 : 0);
                      }

            case 1  : {
                         string_t buffer(parameters[1]);
                         const std::size_t amount =
                                  std::min(buffer.size(),
                                           static_cast<std::size_t>(scalar_t(parameters[2])()));
                         return T(fd->read(buffer,amount) ? 1 : 0);
                      }

            case 2  : {
                         vector_t vec(parameters[1]);
                         const std::size_t amount = vec.size();
                         return T(fd->read(vec,amount) ? 1 : 0);
                      }

            case 3  : {
                         vector_t vec(parameters[1]);
                         const std::size_t amount =
                                  std::min(vec.size(),
                                           static_cast<std::size_t>(scalar_t(parameters[2])()));
                         return T(fd->read(vec,amount) ? 1 : 0);
                      }
         }

         return T(0);
      }
   };


} // namespace math_expr::rtl::io::file

#endif
