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

#ifndef MATH_EXPR_PARSER_RTL_WIRING_HPP
#define MATH_EXPR_PARSER_RTL_WIRING_HPP

#include "math_expr/core/operator_types.hpp"
#include "math_expr/details/node_utils.hpp"
#include "math_expr/details/operator_nodes.hpp"

namespace math_expr
{
template <typename T>
class rtl_wiring
{
   public:
    template <typename BaseOpsMap, typename UnaryOpMap, typename BinaryOpMap,
              typename InvBinaryOpMap, typename Sf3Map, typename Sf4Map>
    static inline void load(BaseOpsMap& base_ops_map, UnaryOpMap& unary_op_map,
                            BinaryOpMap& binary_op_map, InvBinaryOpMap& inv_binary_op_map,
                            Sf3Map& sf3_map, Sf4Map& sf4_map)
    {
        details::load_operations_map(base_ops_map);
        load_unary_operations_map(unary_op_map);
        load_binary_operations_map(binary_op_map);
        load_inv_binary_operations_map(inv_binary_op_map);
        load_sf3_map(sf3_map);
        load_sf4_map(sf4_map);
    }

   private:
    template <typename UnaryOpMap>
    static inline void load_unary_operations_map(UnaryOpMap& m)
    {
#define REGISTER_UNARY_OP(Op, UnaryFunctor) m.insert(std::make_pair(Op, UnaryFunctor<T>::process))

        REGISTER_UNARY_OP(core::operators::operator_type::abs, details::abs_op);
        REGISTER_UNARY_OP(core::operators::operator_type::acos, details::acos_op);
        REGISTER_UNARY_OP(core::operators::operator_type::acosh, details::acosh_op);
        REGISTER_UNARY_OP(core::operators::operator_type::asin, details::asin_op);
        REGISTER_UNARY_OP(core::operators::operator_type::asinh, details::asinh_op);
        REGISTER_UNARY_OP(core::operators::operator_type::atanh, details::atanh_op);
        REGISTER_UNARY_OP(core::operators::operator_type::ceil, details::ceil_op);
        REGISTER_UNARY_OP(core::operators::operator_type::cos, details::cos_op);
        REGISTER_UNARY_OP(core::operators::operator_type::cosh, details::cosh_op);
        REGISTER_UNARY_OP(core::operators::operator_type::exp, details::exp_op);
        REGISTER_UNARY_OP(core::operators::operator_type::expm1, details::expm1_op);
        REGISTER_UNARY_OP(core::operators::operator_type::floor, details::floor_op);
        REGISTER_UNARY_OP(core::operators::operator_type::log, details::log_op);
        REGISTER_UNARY_OP(core::operators::operator_type::log10, details::log10_op);
        REGISTER_UNARY_OP(core::operators::operator_type::log2, details::log2_op);
        REGISTER_UNARY_OP(core::operators::operator_type::log1p, details::log1p_op);
        REGISTER_UNARY_OP(core::operators::operator_type::neg, details::neg_op);
        REGISTER_UNARY_OP(core::operators::operator_type::pos, details::pos_op);
        REGISTER_UNARY_OP(core::operators::operator_type::round, details::round_op);
        REGISTER_UNARY_OP(core::operators::operator_type::sin, details::sin_op);
        REGISTER_UNARY_OP(core::operators::operator_type::sinc, details::sinc_op);
        REGISTER_UNARY_OP(core::operators::operator_type::sinh, details::sinh_op);
        REGISTER_UNARY_OP(core::operators::operator_type::sqrt, details::sqrt_op);
        REGISTER_UNARY_OP(core::operators::operator_type::tan, details::tan_op);
        REGISTER_UNARY_OP(core::operators::operator_type::tanh, details::tanh_op);
        REGISTER_UNARY_OP(core::operators::operator_type::cot, details::cot_op);
        REGISTER_UNARY_OP(core::operators::operator_type::sec, details::sec_op);
        REGISTER_UNARY_OP(core::operators::operator_type::csc, details::csc_op);
        REGISTER_UNARY_OP(core::operators::operator_type::r2d, details::r2d_op);
        REGISTER_UNARY_OP(core::operators::operator_type::d2r, details::d2r_op);
        REGISTER_UNARY_OP(core::operators::operator_type::d2g, details::d2g_op);
        REGISTER_UNARY_OP(core::operators::operator_type::g2d, details::g2d_op);
        REGISTER_UNARY_OP(core::operators::operator_type::notl, details::notl_op);
        REGISTER_UNARY_OP(core::operators::operator_type::sgn, details::sgn_op);
        REGISTER_UNARY_OP(core::operators::operator_type::erf, details::erf_op);
        REGISTER_UNARY_OP(core::operators::operator_type::erfc, details::erfc_op);
        REGISTER_UNARY_OP(core::operators::operator_type::ncdf, details::ncdf_op);
        REGISTER_UNARY_OP(core::operators::operator_type::frac, details::frac_op);
        REGISTER_UNARY_OP(core::operators::operator_type::trunc, details::trunc_op);
#undef REGISTER_UNARY_OP
    }

    template <typename BinaryOpMap>
    static inline void load_binary_operations_map(BinaryOpMap& m)
    {
        using value_type = typename BinaryOpMap::value_type;

#define REGISTER_BINARY_OP(Op, BinaryFunctor) m.insert(value_type(Op, BinaryFunctor<T>::process))

        REGISTER_BINARY_OP(core::operators::operator_type::add, details::add_op);
        REGISTER_BINARY_OP(core::operators::operator_type::sub, details::sub_op);
        REGISTER_BINARY_OP(core::operators::operator_type::mul, details::mul_op);
        REGISTER_BINARY_OP(core::operators::operator_type::div, details::div_op);
        REGISTER_BINARY_OP(core::operators::operator_type::mod, details::mod_op);
        REGISTER_BINARY_OP(core::operators::operator_type::pow, details::pow_op);
        REGISTER_BINARY_OP(core::operators::operator_type::lt, details::lt_op);
        REGISTER_BINARY_OP(core::operators::operator_type::lte, details::lte_op);
        REGISTER_BINARY_OP(core::operators::operator_type::gt, details::gt_op);
        REGISTER_BINARY_OP(core::operators::operator_type::gte, details::gte_op);
        REGISTER_BINARY_OP(core::operators::operator_type::eq, details::eq_op);
        REGISTER_BINARY_OP(core::operators::operator_type::ne, details::ne_op);
        REGISTER_BINARY_OP(core::operators::operator_type::logical_and, details::and_op);
        REGISTER_BINARY_OP(core::operators::operator_type::nand, details::nand_op);
        REGISTER_BINARY_OP(core::operators::operator_type::logical_or, details::or_op);
        REGISTER_BINARY_OP(core::operators::operator_type::nor, details::nor_op);
        REGISTER_BINARY_OP(core::operators::operator_type::logical_xor, details::xor_op);
        REGISTER_BINARY_OP(core::operators::operator_type::xnor, details::xnor_op);
#undef REGISTER_BINARY_OP
    }

    template <typename InvBinaryOpMap>
    static inline void load_inv_binary_operations_map(InvBinaryOpMap& m)
    {
        using value_type = typename InvBinaryOpMap::value_type;

#define REGISTER_BINARY_OP(Op, BinaryFunctor) m.insert(value_type(BinaryFunctor<T>::process, Op))

        REGISTER_BINARY_OP(core::operators::operator_type::add, details::add_op);
        REGISTER_BINARY_OP(core::operators::operator_type::sub, details::sub_op);
        REGISTER_BINARY_OP(core::operators::operator_type::mul, details::mul_op);
        REGISTER_BINARY_OP(core::operators::operator_type::div, details::div_op);
        REGISTER_BINARY_OP(core::operators::operator_type::mod, details::mod_op);
        REGISTER_BINARY_OP(core::operators::operator_type::pow, details::pow_op);
        REGISTER_BINARY_OP(core::operators::operator_type::lt, details::lt_op);
        REGISTER_BINARY_OP(core::operators::operator_type::lte, details::lte_op);
        REGISTER_BINARY_OP(core::operators::operator_type::gt, details::gt_op);
        REGISTER_BINARY_OP(core::operators::operator_type::gte, details::gte_op);
        REGISTER_BINARY_OP(core::operators::operator_type::eq, details::eq_op);
        REGISTER_BINARY_OP(core::operators::operator_type::ne, details::ne_op);
        REGISTER_BINARY_OP(core::operators::operator_type::logical_and, details::and_op);
        REGISTER_BINARY_OP(core::operators::operator_type::nand, details::nand_op);
        REGISTER_BINARY_OP(core::operators::operator_type::logical_or, details::or_op);
        REGISTER_BINARY_OP(core::operators::operator_type::nor, details::nor_op);
        REGISTER_BINARY_OP(core::operators::operator_type::logical_xor, details::xor_op);
        REGISTER_BINARY_OP(core::operators::operator_type::xnor, details::xnor_op);
#undef REGISTER_BINARY_OP
    }

    template <typename Sf3Map>
    static inline void load_sf3_map(Sf3Map& sf3_map)
    {
        using pair_t =
            std::pair<typename Sf3Map::mapped_type::first_type, core::operators::operator_type>;

#define REGISTER_SF3(Op)                     \
    sf3_map[details::sf##Op##_op<T>::id()] = \
        pair_t(details::sf##Op##_op<T>::process, core::operators::operator_type::sf##Op)

        REGISTER_SF3(00);
        REGISTER_SF3(01);
        REGISTER_SF3(02);
        REGISTER_SF3(03);
        REGISTER_SF3(04);
        REGISTER_SF3(05);
        REGISTER_SF3(06);
        REGISTER_SF3(07);
        REGISTER_SF3(08);
        REGISTER_SF3(09);
        REGISTER_SF3(10);
        REGISTER_SF3(11);
        REGISTER_SF3(12);
        REGISTER_SF3(13);
        REGISTER_SF3(14);
        REGISTER_SF3(15);
        REGISTER_SF3(16);
        REGISTER_SF3(17);
        REGISTER_SF3(18);
        REGISTER_SF3(19);
        REGISTER_SF3(20);
        REGISTER_SF3(21);
        REGISTER_SF3(22);
        REGISTER_SF3(23);
        REGISTER_SF3(24);
        REGISTER_SF3(25);
        REGISTER_SF3(26);
        REGISTER_SF3(27);
        REGISTER_SF3(28);
        REGISTER_SF3(29);
        REGISTER_SF3(30);
#undef REGISTER_SF3

#define REGISTER_SF3_EXTID(Id, Op) \
    sf3_map[Id] = pair_t(details::sf##Op##_op<T>::process, core::operators::operator_type::sf##Op)

        REGISTER_SF3_EXTID("(t-t)-t", 23);
#undef REGISTER_SF3_EXTID
    }

    template <typename Sf4Map>
    static inline void load_sf4_map(Sf4Map& sf4_map)
    {
        using pair_t =
            std::pair<typename Sf4Map::mapped_type::first_type, core::operators::operator_type>;

#define REGISTER_SF4(Op)                     \
    sf4_map[details::sf##Op##_op<T>::id()] = \
        pair_t(details::sf##Op##_op<T>::process, core::operators::operator_type::sf##Op)

        REGISTER_SF4(48);
        REGISTER_SF4(49);
        REGISTER_SF4(50);
        REGISTER_SF4(51);
        REGISTER_SF4(52);
        REGISTER_SF4(53);
        REGISTER_SF4(54);
        REGISTER_SF4(55);
        REGISTER_SF4(56);
        REGISTER_SF4(57);
        REGISTER_SF4(58);
        REGISTER_SF4(59);
        REGISTER_SF4(60);
        REGISTER_SF4(61);
        REGISTER_SF4(62);
        REGISTER_SF4(63);
        REGISTER_SF4(64);
        REGISTER_SF4(65);
        REGISTER_SF4(66);
        REGISTER_SF4(67);
        REGISTER_SF4(68);
        REGISTER_SF4(69);
        REGISTER_SF4(70);
        REGISTER_SF4(71);
        REGISTER_SF4(72);
        REGISTER_SF4(73);
        REGISTER_SF4(74);
        REGISTER_SF4(75);
        REGISTER_SF4(76);
        REGISTER_SF4(77);
        REGISTER_SF4(78);
        REGISTER_SF4(79);
        REGISTER_SF4(80);
        REGISTER_SF4(81);
        REGISTER_SF4(82);
        REGISTER_SF4(83);
#undef REGISTER_SF4

#define REGISTER_SF4EXT(Op)                     \
    sf4_map[details::sfext##Op##_op<T>::id()] = \
        pair_t(details::sfext##Op##_op<T>::process, core::operators::operator_type::sf4ext##Op)

        REGISTER_SF4EXT(00);
        REGISTER_SF4EXT(01);
        REGISTER_SF4EXT(02);
        REGISTER_SF4EXT(03);
        REGISTER_SF4EXT(04);
        REGISTER_SF4EXT(05);
        REGISTER_SF4EXT(06);
        REGISTER_SF4EXT(07);
        REGISTER_SF4EXT(08);
        REGISTER_SF4EXT(09);
        REGISTER_SF4EXT(10);
        REGISTER_SF4EXT(11);
        REGISTER_SF4EXT(12);
        REGISTER_SF4EXT(13);
        REGISTER_SF4EXT(14);
        REGISTER_SF4EXT(15);
        REGISTER_SF4EXT(16);
        REGISTER_SF4EXT(17);
        REGISTER_SF4EXT(18);
        REGISTER_SF4EXT(19);
        REGISTER_SF4EXT(20);
        REGISTER_SF4EXT(21);
        REGISTER_SF4EXT(22);
        REGISTER_SF4EXT(23);
        REGISTER_SF4EXT(24);
        REGISTER_SF4EXT(25);
        REGISTER_SF4EXT(26);
        REGISTER_SF4EXT(27);
        REGISTER_SF4EXT(28);
        REGISTER_SF4EXT(29);
        REGISTER_SF4EXT(30);
        REGISTER_SF4EXT(31);
        REGISTER_SF4EXT(32);
        REGISTER_SF4EXT(33);
        REGISTER_SF4EXT(34);
        REGISTER_SF4EXT(35);
        REGISTER_SF4EXT(36);
        REGISTER_SF4EXT(37);
        REGISTER_SF4EXT(38);
        REGISTER_SF4EXT(39);
        REGISTER_SF4EXT(40);
        REGISTER_SF4EXT(41);
        REGISTER_SF4EXT(42);
        REGISTER_SF4EXT(43);
        REGISTER_SF4EXT(44);
        REGISTER_SF4EXT(45);
        REGISTER_SF4EXT(46);
        REGISTER_SF4EXT(47);
        REGISTER_SF4EXT(48);
        REGISTER_SF4EXT(49);
        REGISTER_SF4EXT(50);
        REGISTER_SF4EXT(51);
        REGISTER_SF4EXT(52);
        REGISTER_SF4EXT(53);
        REGISTER_SF4EXT(54);
        REGISTER_SF4EXT(55);
        REGISTER_SF4EXT(56);
        REGISTER_SF4EXT(57);
        REGISTER_SF4EXT(58);
        REGISTER_SF4EXT(59);
        REGISTER_SF4EXT(60);
        REGISTER_SF4EXT(61);
#undef REGISTER_SF4EXT
    }
};
}  // namespace math_expr

#endif
