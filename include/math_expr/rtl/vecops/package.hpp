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

#ifndef MATH_EXPR_RTL_VECOPS_PACKAGE_HPP
#define MATH_EXPR_RTL_VECOPS_PACKAGE_HPP

#ifndef MATH_EXPR_DISABLE_RTL_VECOPS

#include "math_expr/symbol_table.hpp"
#include "math_expr/rtl/vecops/all_false.hpp"
#include "math_expr/rtl/vecops/all_true.hpp"
#include "math_expr/rtl/vecops/any_false.hpp"
#include "math_expr/rtl/vecops/any_true.hpp"
#include "math_expr/rtl/vecops/assign.hpp"
#include "math_expr/rtl/vecops/axpbsy.hpp"
#include "math_expr/rtl/vecops/axpbsyz.hpp"
#include "math_expr/rtl/vecops/axpby.hpp"
#include "math_expr/rtl/vecops/axpbyz.hpp"
#include "math_expr/rtl/vecops/axpbz.hpp"
#include "math_expr/rtl/vecops/axpy.hpp"
#include "math_expr/rtl/vecops/axpyz.hpp"
#include "math_expr/rtl/vecops/copy.hpp"
#include "math_expr/rtl/vecops/count.hpp"
#include "math_expr/rtl/vecops/diff.hpp"
#include "math_expr/rtl/vecops/dot.hpp"
#include "math_expr/rtl/vecops/dotk.hpp"
#include "math_expr/rtl/vecops/iota.hpp"
#include "math_expr/rtl/vecops/max_elemwise.hpp"
#include "math_expr/rtl/vecops/min_elemwise.hpp"
#include "math_expr/rtl/vecops/nthelement.hpp"
#include "math_expr/rtl/vecops/reverse.hpp"
#include "math_expr/rtl/vecops/rol.hpp"
#include "math_expr/rtl/vecops/ror.hpp"
#include "math_expr/rtl/vecops/select.hpp"
#include "math_expr/rtl/vecops/shift_left.hpp"
#include "math_expr/rtl/vecops/shift_right.hpp"
#include "math_expr/rtl/vecops/sort.hpp"
#include "math_expr/rtl/vecops/sumk.hpp"
#include "math_expr/rtl/vecops/threshold_above.hpp"
#include "math_expr/rtl/vecops/threshold_below.hpp"

namespace math_expr::rtl::vecops
{
template <typename T>
struct package
{
    all_true<T> at;
    all_false<T> af;
    any_true<T> nt;
    any_false<T> nf;
    count<T> c;
    copy<T> cp;
    rol<T> rl;
    ror<T> rr;
    reverse<T> rev;
    shift_left<T> sl;
    shift_right<T> sr;
    sort<T> st;
    nthelement<T> ne;
    assign<T> an;
    iota<T> ia;
    sumk<T> sk;
    axpy<T> b1_axpy;
    axpby<T> b1_axpby;
    axpyz<T> b1_axpyz;
    axpbyz<T> b1_axpbyz;
    axpbsy<T> b1_axpbsy;
    axpbsyz<T> b1_axpbsyz;
    axpbz<T> b1_axpbz;
    diff<T> df;
    dot<T> dt;
    dotk<T> dtk;
    threshold_above<T> ta;
    threshold_below<T> tb;
    min_elemwise<T> miew;
    max_elemwise<T> maew;
    select<T> slct;

    bool register_package(math_expr::symbol_table<T>& symtab)
    {
#define MATH_EXPR_REGISTER_FUNCTION(FunctionName, FunctionType)                        \
    if (!symtab.add_function(FunctionName, FunctionType))                              \
    {                                                                                  \
        core::debug_print(                                                             \
            "math_expr::rtl::vecops::register_package - Failed to add function: %s\n", \
            FunctionName);                                                             \
        return false;                                                                  \
    }

        MATH_EXPR_REGISTER_FUNCTION("all_true", at);
        MATH_EXPR_REGISTER_FUNCTION("all_false", af);
        MATH_EXPR_REGISTER_FUNCTION("any_true", nt);
        MATH_EXPR_REGISTER_FUNCTION("any_false", nf);
        MATH_EXPR_REGISTER_FUNCTION("count", c);
        MATH_EXPR_REGISTER_FUNCTION("copy", cp);
        MATH_EXPR_REGISTER_FUNCTION("rotate_left", rl);
        MATH_EXPR_REGISTER_FUNCTION("rol", rl);
        MATH_EXPR_REGISTER_FUNCTION("rotate_right", rr);
        MATH_EXPR_REGISTER_FUNCTION("ror", rr);
        MATH_EXPR_REGISTER_FUNCTION("reverse", rev);
        MATH_EXPR_REGISTER_FUNCTION("shftl", sl);
        MATH_EXPR_REGISTER_FUNCTION("shftr", sr);
        MATH_EXPR_REGISTER_FUNCTION("sort", st);
        MATH_EXPR_REGISTER_FUNCTION("nth_element", ne);
        MATH_EXPR_REGISTER_FUNCTION("assign", an);
        MATH_EXPR_REGISTER_FUNCTION("iota", ia);
        MATH_EXPR_REGISTER_FUNCTION("sumk", sk);
        MATH_EXPR_REGISTER_FUNCTION("axpy", b1_axpy);
        MATH_EXPR_REGISTER_FUNCTION("axpby", b1_axpby);
        MATH_EXPR_REGISTER_FUNCTION("axpyz", b1_axpyz);
        MATH_EXPR_REGISTER_FUNCTION("axpbyz", b1_axpbyz);
        MATH_EXPR_REGISTER_FUNCTION("axpbsy", b1_axpbsy);
        MATH_EXPR_REGISTER_FUNCTION("axpbsyz", b1_axpbsyz);
        MATH_EXPR_REGISTER_FUNCTION("axpbz", b1_axpbz);
        MATH_EXPR_REGISTER_FUNCTION("diff", df);
        MATH_EXPR_REGISTER_FUNCTION("dot", dt);
        MATH_EXPR_REGISTER_FUNCTION("dotk", dtk);
        MATH_EXPR_REGISTER_FUNCTION("threshold_above", ta);
        MATH_EXPR_REGISTER_FUNCTION("threshold_below", tb);
        MATH_EXPR_REGISTER_FUNCTION("min_elemwise", miew);
        MATH_EXPR_REGISTER_FUNCTION("max_elemwise", maew);
        MATH_EXPR_REGISTER_FUNCTION("select", slct);

#undef MATH_EXPR_REGISTER_FUNCTION

        return true;
    }
};

}  // namespace math_expr::rtl::vecops

#endif

#endif
