/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
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

#ifndef MATH_EXPR_RTL_IO_FILE_WRITE_HPP
#define MATH_EXPR_RTL_IO_FILE_WRITE_HPP

#include "math_expr/rtl/io/file/helper.hpp"

namespace math_expr::rtl::io::file
{
template <typename T>
class write final : public math_expr::igeneric_function<T>
{
   public:
    using igfun_t = typename math_expr::igeneric_function<T>;
    using parameter_list_t = typename igfun_t::parameter_list_t;
    using generic_type = typename igfun_t::generic_type;
    using string_t = typename generic_type::string_view;
    using scalar_t = typename generic_type::scalar_view;
    using vector_t = typename generic_type::vector_view;

    using igfun_t::operator();

    write() : igfun_t("TS|TST|TV|TVT")
    {
        details::perform_check<T>();
    }

    inline T operator()(const std::size_t& ps_index, parameter_list_t parameters) override
    {
        details::file_descriptor* fd = details::decode_handle(scalar_t(parameters[0])());

        if (nullptr == fd)
        {
            return T(0);
        }

        switch (ps_index)
        {
            case 0:
            {
                const string_t buffer(parameters[1]);
                const std::size_t amount = buffer.size();
                return T(fd->write(buffer, amount) ? 1 : 0);
            }

            case 1:
            {
                const string_t buffer(parameters[1]);
                const std::size_t amount =
                    std::min(buffer.size(), static_cast<std::size_t>(scalar_t(parameters[2])()));
                return T(fd->write(buffer, amount) ? 1 : 0);
            }

            case 2:
            {
                const vector_t vec(parameters[1]);
                const std::size_t amount = vec.size();
                return T(fd->write(vec, amount) ? 1 : 0);
            }

            case 3:
            {
                const vector_t vec(parameters[1]);
                const std::size_t amount =
                    std::min(vec.size(), static_cast<std::size_t>(scalar_t(parameters[2])()));
                return T(fd->write(vec, amount) ? 1 : 0);
            }
        }

        return T(0);
    }
};

}  // namespace math_expr::rtl::io::file

#endif
