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

#ifndef MATH_EXPR_RTL_IO_FILE_DETAILS_HANDLE_CODEC_HPP
#define MATH_EXPR_RTL_IO_FILE_DETAILS_HANDLE_CODEC_HPP

#include "math_expr/core/std_includes.hpp"

namespace math_expr::rtl::io::file::details
{
struct file_descriptor;

template <typename T> inline T encode_handle(file_descriptor* fd)
{
    T t = T(0);

    std::memcpy(reinterpret_cast<char*>(&t), reinterpret_cast<const char*>(&fd),
                sizeof(file_descriptor*));

    return t;
}

template <typename T> inline file_descriptor* decode_handle(T v)
{
    file_descriptor* fd = nullptr;

    std::memcpy(reinterpret_cast<char*>(&fd), reinterpret_cast<const char*>(&v),
                sizeof(file_descriptor*));

    return fd;
}

template <typename T> inline void perform_check()
{
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4127)
#endif
    if (sizeof(T) < sizeof(file_descriptor*))
    {
        throw std::runtime_error(
            "math_expr::rtl::io::file - Error - pointer size larger than holder.");
    }
#ifdef _MSC_VER
#pragma warning(pop)
#endif
    assert(sizeof(T) >= sizeof(file_descriptor*));
}
} // namespace math_expr::rtl::io::file::details

#endif
