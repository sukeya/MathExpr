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

#ifndef MATH_EXPR_CORE_OPERATORS_DETAILS_PROCESS_HPP
#define MATH_EXPR_CORE_OPERATORS_DETAILS_PROCESS_HPP

#include "math_expr/core/numeric.hpp"
#include "math_expr/core/operators/types.hpp"

namespace math_expr::core::operators::details
{
template <typename T> inline T process_impl(const operator_type operation, const T arg)
{
    switch (operation)
    {
    case operator_type::abs:
        return ::math_expr::core::numeric::abs(arg);
    case operator_type::acos:
        return ::math_expr::core::numeric::acos(arg);
    case operator_type::acosh:
        return ::math_expr::core::numeric::acosh(arg);
    case operator_type::asin:
        return ::math_expr::core::numeric::asin(arg);
    case operator_type::asinh:
        return ::math_expr::core::numeric::asinh(arg);
    case operator_type::atan:
        return ::math_expr::core::numeric::atan(arg);
    case operator_type::atanh:
        return ::math_expr::core::numeric::atanh(arg);
    case operator_type::ceil:
        return ::math_expr::core::numeric::ceil(arg);
    case operator_type::cos:
        return ::math_expr::core::numeric::cos(arg);
    case operator_type::cosh:
        return ::math_expr::core::numeric::cosh(arg);
    case operator_type::exp:
        return ::math_expr::core::numeric::exp(arg);
    case operator_type::expm1:
        return ::math_expr::core::numeric::expm1(arg);
    case operator_type::floor:
        return ::math_expr::core::numeric::floor(arg);
    case operator_type::log:
        return ::math_expr::core::numeric::log(arg);
    case operator_type::log10:
        return ::math_expr::core::numeric::log10(arg);
    case operator_type::log2:
        return ::math_expr::core::numeric::log2(arg);
    case operator_type::log1p:
        return ::math_expr::core::numeric::log1p(arg);
    case operator_type::neg:
        return ::math_expr::core::numeric::neg(arg);
    case operator_type::pos:
        return ::math_expr::core::numeric::pos(arg);
    case operator_type::round:
        return ::math_expr::core::numeric::round(arg);
    case operator_type::sin:
        return ::math_expr::core::numeric::sin(arg);
    case operator_type::sinc:
        return ::math_expr::core::numeric::sinc(arg);
    case operator_type::sinh:
        return ::math_expr::core::numeric::sinh(arg);
    case operator_type::sqrt:
        return ::math_expr::core::numeric::sqrt(arg);
    case operator_type::tan:
        return ::math_expr::core::numeric::tan(arg);
    case operator_type::tanh:
        return ::math_expr::core::numeric::tanh(arg);
    case operator_type::cot:
        return ::math_expr::core::numeric::cot(arg);
    case operator_type::sec:
        return ::math_expr::core::numeric::sec(arg);
    case operator_type::csc:
        return ::math_expr::core::numeric::csc(arg);
    case operator_type::r2d:
        return ::math_expr::core::numeric::r2d(arg);
    case operator_type::d2r:
        return ::math_expr::core::numeric::d2r(arg);
    case operator_type::d2g:
        return ::math_expr::core::numeric::d2g(arg);
    case operator_type::g2d:
        return ::math_expr::core::numeric::g2d(arg);
    case operator_type::notl:
        return ::math_expr::core::numeric::notl(arg);
    case operator_type::sgn:
        return ::math_expr::core::numeric::sgn(arg);
    case operator_type::erf:
        return ::math_expr::core::numeric::erf(arg);
    case operator_type::erfc:
        return ::math_expr::core::numeric::erfc(arg);
    case operator_type::ncdf:
        return ::math_expr::core::numeric::ncdf(arg);
    case operator_type::frac:
        return ::math_expr::core::numeric::frac(arg);
    case operator_type::trunc:
        return ::math_expr::core::numeric::trunc(arg);

    default:
        math_expr_debug(
            ("::math_expr::core::numeric::details::process_impl<T> - Invalid unary operation.\n"));
        return std::numeric_limits<T>::quiet_NaN();
    }
}

template <typename T>
inline T process_impl(const operator_type operation, const T arg0, const T arg1)
{
    switch (operation)
    {
    case operator_type::add:
        return (arg0 + arg1);
    case operator_type::sub:
        return (arg0 - arg1);
    case operator_type::mul:
        return (arg0 * arg1);
    case operator_type::div:
        return (arg0 / arg1);
    case operator_type::mod:
        return ::math_expr::core::numeric::modulus<T>(arg0, arg1);
    case operator_type::pow:
        return ::math_expr::core::numeric::pow<T>(arg0, arg1);
    case operator_type::atan2:
        return ::math_expr::core::numeric::atan2<T>(arg0, arg1);
    case operator_type::min:
        return std::min<T>(arg0, arg1);
    case operator_type::max:
        return std::max<T>(arg0, arg1);
    case operator_type::logn:
        return ::math_expr::core::numeric::logn<T>(arg0, arg1);
    case operator_type::lt:
        return (arg0 < arg1) ? T(1) : T(0);
    case operator_type::lte:
        return (arg0 <= arg1) ? T(1) : T(0);
    case operator_type::eq:
        return std::equal_to<T>()(arg0, arg1) ? T(1) : T(0);
    case operator_type::ne:
        return std::not_equal_to<T>()(arg0, arg1) ? T(1) : T(0);
    case operator_type::gte:
        return (arg0 >= arg1) ? T(1) : T(0);
    case operator_type::gt:
        return (arg0 > arg1) ? T(1) : T(0);
    case operator_type::logical_and:
        return ::math_expr::core::numeric::and_opr<T>(arg0, arg1);
    case operator_type::nand:
        return ::math_expr::core::numeric::nand_opr<T>(arg0, arg1);
    case operator_type::logical_or:
        return ::math_expr::core::numeric::or_opr<T>(arg0, arg1);
    case operator_type::nor:
        return ::math_expr::core::numeric::nor_opr<T>(arg0, arg1);
    case operator_type::logical_xor:
        return ::math_expr::core::numeric::xor_opr<T>(arg0, arg1);
    case operator_type::xnor:
        return ::math_expr::core::numeric::xnor_opr<T>(arg0, arg1);
    case operator_type::root:
        return ::math_expr::core::numeric::root<T>(arg0, arg1);
    case operator_type::roundn:
        return ::math_expr::core::numeric::roundn<T>(arg0, arg1);
    case operator_type::equal:
        return ::math_expr::core::numeric::equal<T>(arg0, arg1);
    case operator_type::nequal:
        return ::math_expr::core::numeric::nequal<T>(arg0, arg1);
    case operator_type::hypot:
        return ::math_expr::core::numeric::hypot<T>(arg0, arg1);
    case operator_type::shr:
        return ::math_expr::core::numeric::shr<T>(arg0, arg1);
    case operator_type::shl:
        return ::math_expr::core::numeric::shl<T>(arg0, arg1);

    default:
        math_expr_debug(
            ("::math_expr::core::numeric::details::process_impl<T> - Invalid binary operation.\n"));
        return std::numeric_limits<T>::quiet_NaN();
    }
}

} // namespace math_expr::core::operators::details

#endif
