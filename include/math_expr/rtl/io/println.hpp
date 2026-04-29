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

#ifndef MATH_EXPR_RTL_IO_PRINTLN_HPP
#define MATH_EXPR_RTL_IO_PRINTLN_HPP

#include "math_expr/rtl/io/print.hpp"

namespace math_expr::rtl::io
{
template <typename T> struct println final : public math_expr::igeneric_function<T>
{
    typedef typename igeneric_function<T>::parameter_list_t parameter_list_t;

    using math_expr::igeneric_function<T>::operator();

    explicit println(const std::string& scalar_format = "%10.5f") : scalar_format_(scalar_format)
    {
        math_expr::enable_zero_parameters(*this);
    }

    inline T operator()(parameter_list_t parameters) override
    {
        details::print_impl<T>::process(scalar_format_, parameters);
        printf("\n");
        return T(0);
    }

    std::string scalar_format_;
};

} // namespace math_expr::rtl::io

#endif
