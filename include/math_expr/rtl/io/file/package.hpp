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

#ifndef MATH_EXPR_RTL_IO_FILE_PACKAGE_HPP
#define MATH_EXPR_RTL_IO_FILE_PACKAGE_HPP

#ifndef MATH_EXPR_DISABLE_RTL_IO_FILE

#include "math_expr/symbol_table.hpp"
#include "math_expr/rtl/io/file/close.hpp"
#include "math_expr/rtl/io/file/eof.hpp"
#include "math_expr/rtl/io/file/getline.hpp"
#include "math_expr/rtl/io/file/open.hpp"
#include "math_expr/rtl/io/file/read.hpp"
#include "math_expr/rtl/io/file/write.hpp"

namespace math_expr::rtl::io::file
{
template <typename T> struct package
{
    open<T> o;
    close<T> c;
    write<T> w;
    read<T> r;
    getline<T> g;
    eof<T> e;

    bool register_package(math_expr::symbol_table<T>& symtab)
    {
#define math_expr_register_function(FunctionName, FunctionType)                                    \
    if (!symtab.add_function(FunctionName, FunctionType))                                          \
    {                                                                                              \
        math_expr_debug(                                                                           \
            ("math_expr::rtl::io::file::register_package - Failed to add function: %s\n",          \
             FunctionName));                                                                       \
        return false;                                                                              \
    }

        math_expr_register_function("open", o) math_expr_register_function("close", c)
            math_expr_register_function("write", w) math_expr_register_function("read", r)
                math_expr_register_function("getline", g) math_expr_register_function("eof", e)
#undef math_expr_register_function

                    return true;
    }
};
} // namespace math_expr::rtl::io::file

#endif
#endif
