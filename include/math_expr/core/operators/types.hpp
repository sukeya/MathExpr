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

#ifndef MATH_EXPR_CORE_OPERATORS_TYPES_HPP
#define MATH_EXPR_CORE_OPERATORS_TYPES_HPP

#include "math_expr/core/std_includes.hpp"
#include "math_expr/core/config.hpp"
#include "math_expr/core/macros.hpp"
#include <stdexcept>

namespace math_expr::core::operators
{
enum class operator_type : int
{
    default_op,
    null,
    add,
    sub,
    mul,
    div,
    mod,
    pow,
    atan2,
    min,
    max,
    avg,
    sum,
    prod,
    lt,
    lte,
    eq,
    equal,
    ne,
    nequal,
    gte,
    gt,
    logical_and,
    nand,
    logical_or,
    nor,
    logical_xor,
    xnor,
    mand,
    mor,
    scand,
    scor,
    shr,
    shl,
    abs,
    acos,
    acosh,
    asin,
    asinh,
    atan,
    atanh,
    ceil,
    cos,
    cosh,
    exp,
    expm1,
    floor,
    log,
    log10,
    log2,
    log1p,
    logn,
    neg,
    pos,
    round,
    roundn,
    root,
    sqrt,
    sin,
    sinc,
    sinh,
    sec,
    csc,
    tan,
    tanh,
    cot,
    clamp,
    iclamp,
    inrange,
    sgn,
    r2d,
    d2r,
    d2g,
    g2d,
    hypot,
    notl,
    erf,
    erfc,
    ncdf,
    frac,
    trunc,
    assign,
    addass,
    subass,
    mulass,
    divass,
    modass,
    in,
    like,
    ilike,
    multi,
    smulti,
    swap,

    // Do not add new functions/operators after this point.
    sf00 = 1000,
    sf01 = 1001,
    sf02 = 1002,
    sf03 = 1003,
    sf04 = 1004,
    sf05 = 1005,
    sf06 = 1006,
    sf07 = 1007,
    sf08 = 1008,
    sf09 = 1009,
    sf10 = 1010,
    sf11 = 1011,
    sf12 = 1012,
    sf13 = 1013,
    sf14 = 1014,
    sf15 = 1015,
    sf16 = 1016,
    sf17 = 1017,
    sf18 = 1018,
    sf19 = 1019,
    sf20 = 1020,
    sf21 = 1021,
    sf22 = 1022,
    sf23 = 1023,
    sf24 = 1024,
    sf25 = 1025,
    sf26 = 1026,
    sf27 = 1027,
    sf28 = 1028,
    sf29 = 1029,
    sf30 = 1030,
    sf31 = 1031,
    sf32 = 1032,
    sf33 = 1033,
    sf34 = 1034,
    sf35 = 1035,
    sf36 = 1036,
    sf37 = 1037,
    sf38 = 1038,
    sf39 = 1039,
    sf40 = 1040,
    sf41 = 1041,
    sf42 = 1042,
    sf43 = 1043,
    sf44 = 1044,
    sf45 = 1045,
    sf46 = 1046,
    sf47 = 1047,
    sf48 = 1048,
    sf49 = 1049,
    sf50 = 1050,
    sf51 = 1051,
    sf52 = 1052,
    sf53 = 1053,
    sf54 = 1054,
    sf55 = 1055,
    sf56 = 1056,
    sf57 = 1057,
    sf58 = 1058,
    sf59 = 1059,
    sf60 = 1060,
    sf61 = 1061,
    sf62 = 1062,
    sf63 = 1063,
    sf64 = 1064,
    sf65 = 1065,
    sf66 = 1066,
    sf67 = 1067,
    sf68 = 1068,
    sf69 = 1069,
    sf70 = 1070,
    sf71 = 1071,
    sf72 = 1072,
    sf73 = 1073,
    sf74 = 1074,
    sf75 = 1075,
    sf76 = 1076,
    sf77 = 1077,
    sf78 = 1078,
    sf79 = 1079,
    sf80 = 1080,
    sf81 = 1081,
    sf82 = 1082,
    sf83 = 1083,
    sf84 = 1084,
    sf85 = 1085,
    sf86 = 1086,
    sf87 = 1087,
    sf88 = 1088,
    sf89 = 1089,
    sf90 = 1090,
    sf91 = 1091,
    sf92 = 1092,
    sf93 = 1093,
    sf94 = 1094,
    sf95 = 1095,
    sf96 = 1096,
    sf97 = 1097,
    sf98 = 1098,
    sf99 = 1099,
    sffinal = 1100,
    sf4ext00 = 2000,
    sf4ext01 = 2001,
    sf4ext02 = 2002,
    sf4ext03 = 2003,
    sf4ext04 = 2004,
    sf4ext05 = 2005,
    sf4ext06 = 2006,
    sf4ext07 = 2007,
    sf4ext08 = 2008,
    sf4ext09 = 2009,
    sf4ext10 = 2010,
    sf4ext11 = 2011,
    sf4ext12 = 2012,
    sf4ext13 = 2013,
    sf4ext14 = 2014,
    sf4ext15 = 2015,
    sf4ext16 = 2016,
    sf4ext17 = 2017,
    sf4ext18 = 2018,
    sf4ext19 = 2019,
    sf4ext20 = 2020,
    sf4ext21 = 2021,
    sf4ext22 = 2022,
    sf4ext23 = 2023,
    sf4ext24 = 2024,
    sf4ext25 = 2025,
    sf4ext26 = 2026,
    sf4ext27 = 2027,
    sf4ext28 = 2028,
    sf4ext29 = 2029,
    sf4ext30 = 2030,
    sf4ext31 = 2031,
    sf4ext32 = 2032,
    sf4ext33 = 2033,
    sf4ext34 = 2034,
    sf4ext35 = 2035,
    sf4ext36 = 2036,
    sf4ext37 = 2037,
    sf4ext38 = 2038,
    sf4ext39 = 2039,
    sf4ext40 = 2040,
    sf4ext41 = 2041,
    sf4ext42 = 2042,
    sf4ext43 = 2043,
    sf4ext44 = 2044,
    sf4ext45 = 2045,
    sf4ext46 = 2046,
    sf4ext47 = 2047,
    sf4ext48 = 2048,
    sf4ext49 = 2049,
    sf4ext50 = 2050,
    sf4ext51 = 2051,
    sf4ext52 = 2052,
    sf4ext53 = 2053,
    sf4ext54 = 2054,
    sf4ext55 = 2055,
    sf4ext56 = 2056,
    sf4ext57 = 2057,
    sf4ext58 = 2058,
    sf4ext59 = 2059,
    sf4ext60 = 2060,
    sf4ext61 = 2061
};

inline std::string to_str(const operator_type opr)
{
    switch (opr)
    {
        case operator_type::add:
            return "+";
        case operator_type::sub:
            return "-";
        case operator_type::mul:
            return "*";
        case operator_type::div:
            return "/";
        case operator_type::mod:
            return "%";
        case operator_type::pow:
            return "^";
        case operator_type::assign:
            return ":=";
        case operator_type::addass:
            return "+=";
        case operator_type::subass:
            return "-=";
        case operator_type::mulass:
            return "*=";
        case operator_type::divass:
            return "/=";
        case operator_type::modass:
            return "%=";
        case operator_type::lt:
            return "<";
        case operator_type::lte:
            return "<=";
        case operator_type::eq:
            return "==";
        case operator_type::equal:
            return "=";
        case operator_type::ne:
            return "!=";
        case operator_type::nequal:
            return "<>";
        case operator_type::gte:
            return ">=";
        case operator_type::gt:
            return ">";
        case operator_type::logical_and:
            return "and";
        case operator_type::logical_or:
            return "or";
        case operator_type::logical_xor:
            return "xor";
        case operator_type::nand:
            return "nand";
        case operator_type::nor:
            return "nor";
        case operator_type::xnor:
            return "xnor";
        default:
            return "N/A";
    }
}

struct base_operation_t
{
    base_operation_t(const operator_type t, const unsigned int& np) : type(t), num_params(np) {}

    operator_type type;
    unsigned int num_params;
};

struct loop_unroll
{
    static constexpr unsigned int loop_batch_size =
        ::math_expr::core::build_options::kDisableSuperscalarUnroll ? 4u : 16u;

    explicit loop_unroll(const std::size_t& vsize)
        : remainder(vsize % loop_batch_size), upper_bound(static_cast<int>(vsize - remainder))
    {
    }

    template <class F>
    void foreach_batch(F&& f)
    {
#pragma unroll
        for (unsigned int i = 0; i < loop_batch_size; ++i)
        {
            f(i);
        }
    }

    template <class F>
    void foreach_remainder(F&& f)
    {
        if (remainder < 0)
        {
            throw std::runtime_error("loop_unroll::foreach() - Invalid remainder");
        }

        if constexpr (!::math_expr::core::build_options::kDisableSuperscalarUnroll)
        {
            switch (remainder)
            {
                case 15:
                    f();
                    [[fallthrough]];
                case 14:
                    f();
                    [[fallthrough]];
                case 13:
                    f();
                    [[fallthrough]];
                case 12:
                    f();
                    [[fallthrough]];
                case 11:
                    f();
                    [[fallthrough]];
                case 10:
                    f();
                    [[fallthrough]];
                case 9:
                    f();
                    [[fallthrough]];
                case 8:
                    f();
                    [[fallthrough]];
                case 7:
                    f();
                    [[fallthrough]];
                case 6:
                    f();
                    [[fallthrough]];
                case 5:
                    f();
                    [[fallthrough]];
                case 4:
                    f();
                    [[fallthrough]];
                case 3:
                    f();
                    [[fallthrough]];
                case 2:
                    f();
                    [[fallthrough]];
                case 1:
                    f();
                    break;
                case 0:
                    break;
            }
        }
        else
        {
            switch (remainder)
            {
                case 3:
                    f();
                    [[fallthrough]];
                case 2:
                    f();
                    [[fallthrough]];
                case 1:
                    f();
                    break;
                case 0:
                    break;
            }
        }
    }

    int remainder;
    int upper_bound;
};

}  // namespace math_expr::core::operators

#endif
