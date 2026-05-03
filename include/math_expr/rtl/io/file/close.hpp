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

#ifndef MATH_EXPR_RTL_IO_FILE_CLOSE_HPP
#define MATH_EXPR_RTL_IO_FILE_CLOSE_HPP

#include "math_expr/rtl/io/file/helper.hpp"

namespace math_expr::rtl::io::file
{
template <typename T>
struct close final : public math_expr::ifunction<T>
{
    using math_expr::ifunction<T>::operator();

    close() : math_expr::ifunction<T>(1)
    {
        details::perform_check<T>();
    }

    inline T operator()(const T& v) override
    {
        details::file_descriptor* fd = details::decode_handle(v);

        if (nullptr == fd)
        {
            return T(0);
        }

        const bool result = fd->close();

        delete fd;

        return result ? T(1) : T(0);
    }
};

}  // namespace math_expr::rtl::io::file

#endif
