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

#ifndef MATH_EXPR_RTL_IO_FILE_OPEN_HPP
#define MATH_EXPR_RTL_IO_FILE_OPEN_HPP

#include "math_expr/rtl/io/file/helper.hpp"

namespace math_expr::rtl::io::file
{
template <typename T> class open final : public math_expr::igeneric_function<T>
{
  public:
    using igfun_t = typename math_expr::igeneric_function<T>;
    using parameter_list_t = typename igfun_t::parameter_list_t;
    using generic_type = typename igfun_t::generic_type;
    using string_t = typename generic_type::string_view;

    using igfun_t::operator();

    open() : math_expr::igeneric_function<T>("S|SS")
    {
        details::perform_check<T>();
    }

    inline T operator()(const std::size_t& ps_index, parameter_list_t parameters) override
    {
        const std::string file_name = to_str(string_t(parameters[0]));

        if (file_name.empty())
        {
            return T(0);
        }

        if ((1 == ps_index) && (0 == string_t(parameters[1]).size()))
        {
            return T(0);
        }

        const std::string access = (0 == ps_index) ? "r" : to_str(string_t(parameters[1]));

        details::file_descriptor* fd = new details::file_descriptor(file_name, access);

        if (fd->open())
        {
            return details::encode_handle<T>(fd);
        }
        else
        {
            delete fd;
            return T(0);
        }
    }
};
} // namespace math_expr::rtl::io::file

#endif
