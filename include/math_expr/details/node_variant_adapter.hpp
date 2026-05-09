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

#ifndef MATH_EXPR_DETAILS_NODE_VARIANT_ADAPTER_HPP
#define MATH_EXPR_DETAILS_NODE_VARIANT_ADAPTER_HPP

#include "math_expr/details/node_utils.hpp"

namespace math_expr::details
{
template <typename T>
class node_variant_adapter
{
   public:
    using expression_ptr = expression_node<T>*;
    using null_eq_node_t = null_eq_node<T>;
    using and_binary_ext_node_t = binary_ext_node<T, and_op<T>>;
    using or_binary_ext_node_t = binary_ext_node<T, or_op<T>>;
    using literal_node_t = literal_node<T>;
    using variable_node_t = variable_node<T>;
    using string_base_node_t = string_base_node<T>;
    using string_literal_node_t = string_literal_node<T>;
    using stringvar_node_t = string_nodes::stringvar_node<T>;
    using sf3_base_node_t = sf3_base_node<T>;
    using sf4_base_node_t = sf4_base_node<T>;
    using fixed_function_base_node_t = fixed_function_base_node<T>;
    using unary_node_t = unary_node<T>;
    using binary_node_t = binary_node<T>;
    using trinary_node_t = trinary_node<T>;
    using vov_base_node_t = vov_base_node<T>;
    using cov_base_node_t = cov_base_node<T>;
    using voc_base_node_t = voc_base_node<T>;
    using vob_base_node_t = vob_base_node<T>;
    using bov_base_node_t = bov_base_node<T>;
    using uv_base_node_t = uv_base_node<T>;
    using cob_base_node_t = cob_base_node<T>;
    using boc_base_node_t = boc_base_node<T>;
    using scalar_pow_base_node_t = scalar_pow_base_node<T>;
    using branch_pow_base_node_t = branch_pow_base_node<T>;
    using t0ot1ot2_base_node_t = T0oT1oT2_base_node<T>;
    using t0ot1ot2ot3_base_node_t = T0oT1oT2oT3_base_node<T>;
    using uvouv_node_t = uvouv_node<T>;
    using vector_celem_node_t = vector_celem_node<T>;
    using vector_elem_node_t = vector_elem_node<T>;
    using swap_node_t = swap_node<T>;
    using vector_elem_rtc_node_t = vector_elem_rtc_node<T>;
    using vector_celem_rtc_node_t = vector_celem_rtc_node<T>;
    using rebasevector_elem_node_t = rebasevector_elem_node<T>;
    using rebasevector_celem_node_t = rebasevector_celem_node<T>;
    using rebasevector_elem_rtc_node_t = rebasevector_elem_rtc_node<T>;
    using rebasevector_celem_rtc_node_t = rebasevector_celem_rtc_node<T>;
    using vector_size_node_t = vector_size_node<T>;
    using assignment_node_t = assignment_node<T>;
    using assignment_vec_elem_node_t = assignment_vec_elem_node<T>;
    using assignment_rebasevec_elem_node_t = assignment_rebasevec_elem_node<T>;
    using assignment_rebasevec_celem_node_t = assignment_rebasevec_celem_node<T>;
    using assignment_vec_elem_rtc_node_t = assignment_vec_elem_rtc_node<T>;
    using assignment_rebasevec_elem_rtc_node_t = assignment_rebasevec_elem_rtc_node<T>;
    using assignment_vec_node_t = assignment_vec_node<T>;
    using while_loop_node_t = while_loop_node<T>;
    using while_loop_rtc_node_t = while_loop_rtc_node<T>;
    using repeat_until_loop_node_t = repeat_until_loop_node<T>;
    using repeat_until_loop_rtc_node_t = repeat_until_loop_rtc_node<T>;
    using for_loop_node_t = for_loop_node<T>;
    using for_loop_rtc_node_t = for_loop_rtc_node<T>;
    using switch_node_t = switch_node<T>;
    using multi_switch_node_t = multi_switch_node<T>;
    using assignment_vecvec_node_t = assignment_vecvec_node<T>;
    using vector_init_zero_node_t = vector_init_zero_value_node<T>;
    using vector_init_constfill_node_t = vector_init_single_constvalue_node<T>;
    using vector_init_dynfill_node_t = vector_init_single_value_node<T>;
    using vector_init_iota_cc_node_t = vector_init_iota_constconst_node<T>;
    using vector_init_iota_cnc_node_t = vector_init_iota_constnconst_node<T>;
    using vector_init_iota_ncc_node_t = vector_init_iota_nconstconst_node<T>;
    using vector_init_iota_ncnc_node_t = vector_init_iota_nconstnconst_node<T>;
    using vector_init_general_node_t = vector_initialisation_node<T>;
    using vararg_evaluable_node_t = vararg_evaluable_node<T>;
    using vectorize_evaluable_node_t = vectorize_evaluable_node<T>;

    struct null_view
    {
        expression_ptr node;
    };

    struct literal_view
    {
        expression_ptr node;
        literal_node_t* literal;
    };

    struct variable_view
    {
        expression_ptr node;
        variable_node_t* variable;
    };

    struct string_view
    {
        expression_ptr node;
        string_base_node_t* base;
        stringvar_node_t* mutable_node;
        bool is_const_literal;
    };

    struct unary_view
    {
        expression_ptr node;
    };

    struct binary_view
    {
        expression_ptr node;
    };

    struct function_view
    {
        expression_ptr node;
    };

    struct vararg_view
    {
        expression_ptr node;
    };

    struct multi_vararg_view
    {
        expression_ptr node;
    };

    struct assert_view
    {
        expression_ptr node;
    };

    struct sf3ext_view
    {
        expression_ptr node;
    };

    struct sf4ext_view
    {
        expression_ptr node;
    };

    struct other_view
    {
        expression_ptr node;
    };

    struct unary_hot_view
    {
        expression_ptr node;
        unary_node_t* unary;
    };

    struct binary_hot_view
    {
        expression_ptr node;
        binary_node_t* binary;
    };

    struct trinary_hot_view
    {
        expression_ptr node;
        trinary_node_t* trinary;
    };

    struct sf3_hot_view
    {
        expression_ptr node;
        sf3_base_node_t* sf3;
    };

    struct sf4_hot_view
    {
        expression_ptr node;
        sf4_base_node_t* sf4;
    };

    struct fixed_function_hot_view
    {
        expression_ptr node;
        fixed_function_base_node_t* function;
    };

    struct conditional_hot_view
    {
        expression_ptr node;
    };

    struct scand_hot_view
    {
        expression_ptr node;
    };

    struct scor_hot_view
    {
        expression_ptr node;
    };

    struct uv_hot_view
    {
        expression_ptr node;
        uv_base_node_t* uv;
    };

    struct scalar_pow_hot_view
    {
        expression_ptr node;
        scalar_pow_base_node_t* pow;
    };

    struct branch_pow_hot_view
    {
        expression_ptr node;
        branch_pow_base_node_t* pow;
    };

    struct unary_branch_hot_view
    {
        expression_ptr node;
        core::operators::operator_type operation;
    };

    struct vov_hot_view
    {
        expression_ptr node;
        vov_base_node_t* vov;
    };

    struct cov_hot_view
    {
        expression_ptr node;
        cov_base_node_t* cov;
    };

    struct voc_hot_view
    {
        expression_ptr node;
        voc_base_node_t* voc;
    };

    struct vob_hot_view
    {
        expression_ptr node;
        vob_base_node_t* vob;
    };

    struct bov_hot_view
    {
        expression_ptr node;
        bov_base_node_t* bov;
    };

    struct cob_hot_view
    {
        expression_ptr node;
        cob_base_node_t* cob;
    };

    struct boc_hot_view
    {
        expression_ptr node;
        boc_base_node_t* boc;
    };

    struct uvouv_hot_view
    {
        expression_ptr node;
        uvouv_node_t* uvouv;
    };

    struct t0ot1ot2_hot_view
    {
        expression_ptr node;
        t0ot1ot2_base_node_t* base;
    };

    struct t0ot1ot2ot3_hot_view
    {
        expression_ptr node;
        t0ot1ot2ot3_base_node_t* base;
    };

    struct nulleq_hot_view
    {
        expression_ptr node;
        null_eq_node_t* nulleq;
    };

    struct fallback_view
    {
        expression_ptr node;
    };

    struct vararg_multi_hot_view
    {
        expression_ptr node;
        std::size_t count;
    };

    struct vec_celem_hot_view
    {
        expression_ptr node;
        vector_celem_node_t* celem;
    };

    struct vec_elem_hot_view
    {
        expression_ptr node;
        vector_elem_node_t* elem;
    };

    struct swap_hot_view
    {
        expression_ptr node;
        swap_node_t* swap_ptr;
    };

    struct vec_elem_rtc_hot_view
    {
        expression_ptr node;
        vector_elem_rtc_node_t* rtc;
    };

    struct vec_celem_rtc_hot_view
    {
        expression_ptr node;
        vector_celem_rtc_node_t* rtc;
    };

    struct rbvec_elem_hot_view
    {
        expression_ptr node;
        rebasevector_elem_node_t* rbvec;
    };

    struct rbvec_celem_hot_view
    {
        expression_ptr node;
        rebasevector_celem_node_t* rbvec;
    };

    struct rbvec_elem_rtc_hot_view
    {
        expression_ptr node;
        rebasevector_elem_rtc_node_t* rtc;
    };

    struct rbvec_celem_rtc_hot_view
    {
        expression_ptr node;
        rebasevector_celem_rtc_node_t* rtc;
    };

    struct vecsize_hot_view
    {
        expression_ptr node;
        vector_size_node_t* vecsize;
    };

    struct assign_hot_view
    {
        expression_ptr node;
        assignment_node_t* assign;
    };

    struct assign_vec_elem_hot_view
    {
        expression_ptr node;
        assignment_vec_elem_node_t* assign;
    };

    struct assign_rbvec_elem_hot_view
    {
        expression_ptr node;
        assignment_rebasevec_elem_node_t* assign;
    };

    struct assign_rbvec_celem_hot_view
    {
        expression_ptr node;
        assignment_rebasevec_celem_node_t* assign;
    };

    struct assign_op_hot_view
    {
        expression_ptr node;
        variable_node_t* var;
        core::operators::operator_type read_op;
    };

    struct assign_vec_elem_op_hot_view
    {
        expression_ptr node;
        vector_elem_node_t* elem;
        core::operators::operator_type read_op;
    };

    struct assign_vec_elem_rtc_hot_view
    {
        expression_ptr node;
        assignment_vec_elem_rtc_node_t* assign;
    };

    struct assign_rbvec_elem_rtc_hot_view
    {
        expression_ptr node;
        assignment_rebasevec_elem_rtc_node_t* assign;
    };

    struct assign_rbvec_elem_op_hot_view
    {
        expression_ptr node;
        rebasevector_elem_node_t* elem;
        core::operators::operator_type read_op;
    };

    struct assign_rbvec_celem_op_hot_view
    {
        expression_ptr node;
        rebasevector_celem_node_t* elem;
        core::operators::operator_type read_op;
    };

    struct assign_vec_elem_op_rtc_hot_view
    {
        expression_ptr node;
        vector_elem_rtc_node_t* elem;
        core::operators::operator_type read_op;
    };

    struct assign_vec_celem_op_rtc_hot_view
    {
        expression_ptr node;
        vector_celem_rtc_node_t* elem;
        core::operators::operator_type read_op;
    };

    struct assign_rbvec_elem_op_rtc_hot_view
    {
        expression_ptr node;
        rebasevector_elem_rtc_node_t* elem;
        core::operators::operator_type read_op;
    };

    struct assign_rbvec_celem_op_rtc_hot_view
    {
        expression_ptr node;
        rebasevector_celem_rtc_node_t* elem;
        core::operators::operator_type read_op;
    };

    struct assign_vec_scalar_hot_view
    {
        expression_ptr node;
        assignment_vec_node_t* assign;
    };

    struct assign_vec_scalar_op_hot_view
    {
        expression_ptr node;
        core::operators::operator_type read_op;
    };

    struct while_hot_view
    {
        expression_ptr node;
        while_loop_node_t* loop;
    };

    struct while_rtc_hot_view
    {
        expression_ptr node;
        while_loop_rtc_node_t* loop;
    };

    struct repeat_until_hot_view
    {
        expression_ptr node;
        repeat_until_loop_node_t* loop;
    };

    struct repeat_until_rtc_hot_view
    {
        expression_ptr node;
        repeat_until_loop_rtc_node_t* loop;
    };

    struct for_hot_view
    {
        expression_ptr node;
        for_loop_node_t* loop;
    };

    struct for_rtc_hot_view
    {
        expression_ptr node;
        for_loop_rtc_node_t* loop;
    };

    struct switch_hot_view
    {
        expression_ptr node;
        switch_node_t* sw;
    };

    struct multi_switch_hot_view
    {
        expression_ptr node;
        multi_switch_node_t* sw;
    };

    struct assign_vecvec_hot_view
    {
        expression_ptr node;
        assignment_vecvec_node_t* assign;
    };

    struct assign_vecvec_op_hot_view
    {
        expression_ptr node;
        core::operators::operator_type read_op;
    };

    struct vecinit_zero_hot_view
    {
        expression_ptr node;
        T* vec_base;
        std::size_t vec_size;
    };

    struct vecinit_constfill_hot_view
    {
        expression_ptr node;
        T* vec_base;
        std::size_t vec_size;
        T fill_value;
    };

    struct vecinit_dynfill_hot_view
    {
        expression_ptr node;
        T* vec_base;
        std::size_t vec_size;
        expression_ptr init_child;
    };

    struct vecinit_iota_cc_hot_view
    {
        expression_ptr node;
        T* vec_base;
        std::size_t vec_size;
        T base_val;
        T increment_val;
    };

    struct vecinit_iota_cnc_hot_view
    {
        expression_ptr node;
        T* vec_base;
        std::size_t vec_size;
        T base_val;
        expression_ptr increment_child;
    };

    struct vecinit_iota_ncc_hot_view
    {
        expression_ptr node;
        T* vec_base;
        std::size_t vec_size;
        expression_ptr base_child;
        expression_ptr increment_child;
    };

    struct vecinit_iota_ncnc_hot_view
    {
        expression_ptr node;
        T* vec_base;
        std::size_t vec_size;
        expression_ptr base_child;
        expression_ptr increment_child;
    };

    struct vecinit_general_hot_view
    {
        expression_ptr node;
        vector_init_general_node_t* vinit;
    };

    struct vec_binop_vecvec_hot_view
    {
        expression_ptr node;
        core::operators::operator_type operation;
    };

    struct vec_binop_vecval_hot_view
    {
        expression_ptr node;
        core::operators::operator_type operation;
    };

    struct vec_binop_valvec_hot_view
    {
        expression_ptr node;
        core::operators::operator_type operation;
    };

    struct unary_vec_hot_view
    {
        expression_ptr node;
        core::operators::operator_type operation;
    };

    struct vararg_evaluable_hot_view
    {
        expression_ptr node;
        vararg_evaluable_node_t* fn;
    };

    struct vecfunc_hot_view
    {
        expression_ptr node;
        vectorize_evaluable_node_t* fn;
    };

    using variant_type =
        std::variant<std::monostate, null_view, literal_view, variable_view, string_view,
                     unary_view, binary_view, function_view, vararg_view, multi_vararg_view,
                     assert_view, sf3ext_view, sf4ext_view, other_view>;

    using hot_variant_type = std::variant<
        std::monostate, literal_view, variable_view, unary_hot_view, binary_hot_view,
        trinary_hot_view, sf3_hot_view, sf4_hot_view, fixed_function_hot_view, conditional_hot_view,
        uv_hot_view, scalar_pow_hot_view, branch_pow_hot_view, unary_branch_hot_view, vov_hot_view,
        cov_hot_view, voc_hot_view, vob_hot_view, bov_hot_view, cob_hot_view, boc_hot_view,
        uvouv_hot_view, t0ot1ot2_hot_view, t0ot1ot2ot3_hot_view, scand_hot_view, scor_hot_view,
        nulleq_hot_view, vararg_multi_hot_view, vec_celem_hot_view, vec_elem_hot_view,
        swap_hot_view, vec_elem_rtc_hot_view, vec_celem_rtc_hot_view, rbvec_elem_hot_view,
        rbvec_celem_hot_view, rbvec_elem_rtc_hot_view, rbvec_celem_rtc_hot_view, vecsize_hot_view,
        assign_hot_view, assign_vec_elem_hot_view, assign_rbvec_elem_hot_view,
        assign_rbvec_celem_hot_view, assign_op_hot_view, assign_vec_elem_op_hot_view,
        assign_vec_elem_rtc_hot_view, assign_rbvec_elem_rtc_hot_view, assign_rbvec_elem_op_hot_view,
        assign_rbvec_celem_op_hot_view, assign_vec_elem_op_rtc_hot_view,
        assign_vec_celem_op_rtc_hot_view, assign_rbvec_elem_op_rtc_hot_view,
        assign_rbvec_celem_op_rtc_hot_view, assign_vec_scalar_hot_view,
        assign_vec_scalar_op_hot_view, while_hot_view, while_rtc_hot_view, repeat_until_hot_view,
        repeat_until_rtc_hot_view, for_hot_view, for_rtc_hot_view, switch_hot_view,
        multi_switch_hot_view, assign_vecvec_hot_view, assign_vecvec_op_hot_view,
        vecinit_zero_hot_view, vecinit_constfill_hot_view, vecinit_dynfill_hot_view,
        vecinit_iota_cc_hot_view, vecinit_iota_cnc_hot_view, vecinit_iota_ncc_hot_view,
        vecinit_iota_ncnc_hot_view, vecinit_general_hot_view, vec_binop_vecvec_hot_view,
        vec_binop_vecval_hot_view, vec_binop_valvec_hot_view, unary_vec_hot_view,
        vararg_evaluable_hot_view, vecfunc_hot_view, fallback_view>;

    static inline std::optional<core::operators::operator_type> unary_branch_operation(
        const typename expression_node<T>::node_type type)
    {
        using node_type = typename expression_node<T>::node_type;
        using operator_type = core::operators::operator_type;

        switch (type)
        {
            case node_type::e_abs:
                return operator_type::abs;
            case node_type::e_acos:
                return operator_type::acos;
            case node_type::e_acosh:
                return operator_type::acosh;
            case node_type::e_asin:
                return operator_type::asin;
            case node_type::e_asinh:
                return operator_type::asinh;
            case node_type::e_atan:
                return operator_type::atan;
            case node_type::e_atanh:
                return operator_type::atanh;
            case node_type::e_ceil:
                return operator_type::ceil;
            case node_type::e_cos:
                return operator_type::cos;
            case node_type::e_cosh:
                return operator_type::cosh;
            case node_type::e_exp:
                return operator_type::exp;
            case node_type::e_expm1:
                return operator_type::expm1;
            case node_type::e_floor:
                return operator_type::floor;
            case node_type::e_log:
                return operator_type::log;
            case node_type::e_log10:
                return operator_type::log10;
            case node_type::e_log2:
                return operator_type::log2;
            case node_type::e_log1p:
                return operator_type::log1p;
            case node_type::e_neg:
                return operator_type::neg;
            case node_type::e_pos:
                return operator_type::pos;
            case node_type::e_round:
                return operator_type::round;
            case node_type::e_sin:
                return operator_type::sin;
            case node_type::e_sinc:
                return operator_type::sinc;
            case node_type::e_sinh:
                return operator_type::sinh;
            case node_type::e_sqrt:
                return operator_type::sqrt;
            case node_type::e_tan:
                return operator_type::tan;
            case node_type::e_tanh:
                return operator_type::tanh;
            case node_type::e_cot:
                return operator_type::cot;
            case node_type::e_sec:
                return operator_type::sec;
            case node_type::e_csc:
                return operator_type::csc;
            case node_type::e_r2d:
                return operator_type::r2d;
            case node_type::e_d2r:
                return operator_type::d2r;
            case node_type::e_d2g:
                return operator_type::d2g;
            case node_type::e_g2d:
                return operator_type::g2d;
            case node_type::e_notl:
                return operator_type::notl;
            case node_type::e_sgn:
                return operator_type::sgn;
            case node_type::e_erf:
                return operator_type::erf;
            case node_type::e_erfc:
                return operator_type::erfc;
            case node_type::e_ncdf:
                return operator_type::ncdf;
            case node_type::e_frac:
                return operator_type::frac;
            case node_type::e_trunc:
                return operator_type::trunc;
            default:
                return std::nullopt;
        }
    }

    static inline std::optional<core::operators::operator_type> compound_to_read_op(
        const core::operators::operator_type op)
    {
        using op_t = core::operators::operator_type;
        switch (op)
        {
            case op_t::addass:
                return op_t::add;
            case op_t::subass:
                return op_t::sub;
            case op_t::mulass:
                return op_t::mul;
            case op_t::divass:
                return op_t::div;
            case op_t::modass:
                return op_t::mod;
            default:
                return std::nullopt;
        }
    }

    static inline variant_type classify(expression_ptr node)
    {
        if (nullptr == node)
        {
            return std::monostate{};
        }
        else if (is_null_node(node))
        {
            return null_view{node};
        }
        else if (is_variable_node(node))
        {
            return variable_view{node, static_cast<variable_node_t*>(node)};
        }
        else if (is_generally_string_node(node))
        {
            return string_view{
                node, static_cast<string_base_node_t*>(node->as_string_base()),
                is_string_node(node) ? static_cast<stringvar_node_t*>(node) : nullptr,
                is_const_string_node(node)};
        }
        else if (is_literal_node(node))
        {
            return literal_view{node, static_cast<literal_node_t*>(node)};
        }
        else if (is_unary_node(node))
        {
            return unary_view{node};
        }
        else if (is_binary_node(node))
        {
            return binary_view{node};
        }
        else if (is_function(node))
        {
            return function_view{node};
        }
        else if (node->type() == expression_node<T>::node_type::e_vararg_multi)
        {
            return multi_vararg_view{node};
        }
        else if (is_vararg_node(node))
        {
            return vararg_view{node};
        }
        else if (is_assert_node(node))
        {
            return assert_view{node};
        }
        else if (is_sf3ext_node(node))
        {
            return sf3ext_view{node};
        }
        else if (is_sf4ext_node(node))
        {
            return sf4ext_view{node};
        }

        return other_view{node};
    }

    static inline hot_variant_type classify_hot(expression_ptr node)
    {
        if (nullptr == node)
        {
            return std::monostate{};
        }

        if (auto* uv = node->as_uv_base_node(); nullptr != uv)
        {
            return uv_hot_view{node, uv};
        }
        else if (auto* pow = node->as_scalar_pow_base(); nullptr != pow)
        {
            return scalar_pow_hot_view{node, pow};
        }
        else if (auto* pow = node->as_branch_pow_base();
                 (nullptr != pow) && (nullptr != node->branch(0)))
        {
            return branch_pow_hot_view{node, pow};
        }
        else if (const auto operation = unary_branch_operation(node->type());
                 operation.has_value() && (nullptr != node->branch(0)))
        {
            return unary_branch_hot_view{node, *operation};
        }
        else if (node->type() == expression_node<T>::node_type::e_uvouv)
        {
            return uvouv_hot_view{node, static_cast<uvouv_node_t*>(node)};
        }
        else if ((node->type() == expression_node<T>::node_type::e_binary) &&
                 ((core::operators::operator_type::logical_and ==
                   static_cast<binary_node_t*>(node)->operation()) ||
                  (core::operators::operator_type::scand ==
                   static_cast<binary_node_t*>(node)->operation())) &&
                 (nullptr != node->branch(0)) && (nullptr != node->branch(1)))
        {
            return scand_hot_view{node};
        }
        else if ((node->type() == expression_node<T>::node_type::e_binary) &&
                 ((core::operators::operator_type::logical_or ==
                   static_cast<binary_node_t*>(node)->operation()) ||
                  (core::operators::operator_type::scor ==
                   static_cast<binary_node_t*>(node)->operation())) &&
                 (nullptr != node->branch(0)) && (nullptr != node->branch(1)))
        {
            return scor_hot_view{node};
        }
        else if ((node->type() == expression_node<T>::node_type::e_binary_ext) &&
                 (nullptr != dynamic_cast<and_binary_ext_node_t*>(node)) &&
                 (nullptr != node->branch(0)) && (nullptr != node->branch(1)))
        {
            return scand_hot_view{node};
        }
        else if ((node->type() == expression_node<T>::node_type::e_binary_ext) &&
                 (nullptr != dynamic_cast<or_binary_ext_node_t*>(node)) &&
                 (nullptr != node->branch(0)) && (nullptr != node->branch(1)))
        {
            return scor_hot_view{node};
        }
        else if ((node->type() == expression_node<T>::node_type::e_nulleq) &&
                 (nullptr != node->branch(0)))
        {
            return nulleq_hot_view{node, static_cast<null_eq_node_t*>(node)};
        }
        else if (auto* sf3 = node->as_sf3_base(); nullptr != sf3)
        {
            return sf3_hot_view{node, sf3};
        }
        else if (auto* sf4 = node->as_sf4_base(); nullptr != sf4)
        {
            return sf4_hot_view{node, sf4};
        }
        else if (auto* function = node->as_fixed_function_base(); nullptr != function)
        {
            return fixed_function_hot_view{node, function};
        }
        else if (auto* base = node->as_T0oT1oT2_base();
                 (nullptr != base) && (3 == base->operand_count()) &&
                 (((nullptr != base->binary_functor(0)) && (nullptr != base->binary_functor(1))) ||
                  (nullptr != base->ternary_functor())))
        {
            return t0ot1ot2_hot_view{node, base};
        }
        else if (auto* base = node->as_T0oT1oT2oT3_base();
                 (nullptr != base) && (4 == base->operand_count()) &&
                 (((nullptr != base->binary_functor(0)) && (nullptr != base->binary_functor(1)) &&
                   (nullptr != base->binary_functor(2))) ||
                  (nullptr != base->quaternary_functor())))
        {
            return t0ot1ot2ot3_hot_view{node, base};
        }
        else if (auto* vov = node->as_vov_base(); nullptr != vov)
        {
            return vov_hot_view{node, vov};
        }
        else if (auto* cov = node->as_cov_base(); nullptr != cov)
        {
            return cov_hot_view{node, cov};
        }
        else if (auto* voc = node->as_voc_base(); nullptr != voc)
        {
            return voc_hot_view{node, voc};
        }
        else if (auto* vob = node->as_vob_base(); nullptr != vob)
        {
            return vob_hot_view{node, vob};
        }
        else if (auto* bov = node->as_bov_base(); nullptr != bov)
        {
            return bov_hot_view{node, bov};
        }
        else if (auto* cob = node->as_cob_base(); nullptr != cob)
        {
            return cob_hot_view{node, cob};
        }
        else if (auto* boc = node->as_boc_base(); nullptr != boc)
        {
            return boc_hot_view{node, boc};
        }

        switch (node->type())
        {
            case expression_node<T>::node_type::e_constant:
                return literal_view{node, static_cast<literal_node_t*>(node)};

            case expression_node<T>::node_type::e_variable:
                return variable_view{node, static_cast<variable_node_t*>(node)};

            case expression_node<T>::node_type::e_unary:
                if ((nullptr != node->branch(0)) && (typeid(*node) == typeid(unary_node_t)))
                {
                    return unary_hot_view{node, static_cast<unary_node_t*>(node)};
                }
                return fallback_view{node};

            case expression_node<T>::node_type::e_binary:
                if ((nullptr != node->branch(0)) && (nullptr != node->branch(1)))
                {
                    if (typeid(*node) == typeid(binary_node_t))
                    {
                        return binary_hot_view{node, static_cast<binary_node_t*>(node)};
                    }
                    if (auto* assign = dynamic_cast<assignment_node_t*>(node); nullptr != assign)
                    {
                        return assign_hot_view{node, assign};
                    }
                    if (auto* assign = dynamic_cast<assignment_vec_elem_node_t*>(node);
                        nullptr != assign)
                    {
                        return assign_vec_elem_hot_view{node, assign};
                    }
                    if (auto* assign = dynamic_cast<assignment_rebasevec_elem_node_t*>(node);
                        nullptr != assign)
                    {
                        return assign_rbvec_elem_hot_view{node, assign};
                    }
                    if (auto* assign = dynamic_cast<assignment_rebasevec_celem_node_t*>(node);
                        nullptr != assign)
                    {
                        return assign_rbvec_celem_hot_view{node, assign};
                    }
                    if (auto* assign = dynamic_cast<assignment_vec_elem_rtc_node_t*>(node);
                        nullptr != assign)
                    {
                        return assign_vec_elem_rtc_hot_view{node, assign};
                    }
                    if (auto* assign = dynamic_cast<assignment_rebasevec_elem_rtc_node_t*>(node);
                        nullptr != assign)
                    {
                        return assign_rbvec_elem_rtc_hot_view{node, assign};
                    }
                    const auto op = static_cast<binary_node_t*>(node)->operation();
                    if (const auto read_op = compound_to_read_op(op); read_op.has_value())
                    {
                        if (is_variable_node(node->branch(0)))
                        {
                            return assign_op_hot_view{
                                node, static_cast<variable_node_t*>(node->branch(0)), *read_op};
                        }
                        if (is_vector_elem_node(node->branch(0)))
                        {
                            return assign_vec_elem_op_hot_view{
                                node, static_cast<vector_elem_node_t*>(node->branch(0)), *read_op};
                        }
                        if (is_rebasevector_elem_node(node->branch(0)))
                        {
                            return assign_rbvec_elem_op_hot_view{
                                node, static_cast<rebasevector_elem_node_t*>(node->branch(0)),
                                *read_op};
                        }
                        if (is_rebasevector_celem_node(node->branch(0)))
                        {
                            return assign_rbvec_celem_op_hot_view{
                                node, static_cast<rebasevector_celem_node_t*>(node->branch(0)),
                                *read_op};
                        }
                        if (is_vector_elem_rtc_node(node->branch(0)))
                        {
                            return assign_vec_elem_op_rtc_hot_view{
                                node, static_cast<vector_elem_rtc_node_t*>(node->branch(0)),
                                *read_op};
                        }
                        if (is_vector_celem_rtc_node(node->branch(0)))
                        {
                            return assign_vec_celem_op_rtc_hot_view{
                                node, static_cast<vector_celem_rtc_node_t*>(node->branch(0)),
                                *read_op};
                        }
                        if (is_rebasevector_elem_rtc_node(node->branch(0)))
                        {
                            return assign_rbvec_elem_op_rtc_hot_view{
                                node, static_cast<rebasevector_elem_rtc_node_t*>(node->branch(0)),
                                *read_op};
                        }
                        if (is_rebasevector_celem_rtc_node(node->branch(0)))
                        {
                            return assign_rbvec_celem_op_rtc_hot_view{
                                node, static_cast<rebasevector_celem_rtc_node_t*>(node->branch(0)),
                                *read_op};
                        }
                    }
                }
                return fallback_view{node};

            case expression_node<T>::node_type::e_trinary:
                if ((nullptr != node->branch(0)) && (nullptr != node->branch(1)) &&
                    (nullptr != node->branch(2)) && (typeid(*node) == typeid(trinary_node_t)))
                {
                    return trinary_hot_view{node, static_cast<trinary_node_t*>(node)};
                }
                return fallback_view{node};

            case expression_node<T>::node_type::e_conditional:
                if ((nullptr != node->branch(0)) && (nullptr != node->branch(1)))
                {
                    return conditional_hot_view{node};
                }
                return fallback_view{node};

            case expression_node<T>::node_type::e_vararg_multi:
            {
                const std::size_t count = node->arg_size();
                if (count > 0)
                {
                    return vararg_multi_hot_view{node, count};
                }
                return fallback_view{node};
            }

            case expression_node<T>::node_type::e_veccelem:
                return vec_celem_hot_view{node, static_cast<vector_celem_node_t*>(node)};

            case expression_node<T>::node_type::e_vecelem:
                return vec_elem_hot_view{node, static_cast<vector_elem_node_t*>(node)};

            case expression_node<T>::node_type::e_swap:
                if (typeid(*node) == typeid(swap_node_t))
                {
                    return swap_hot_view{node, static_cast<swap_node_t*>(node)};
                }
                return fallback_view{node};

            case expression_node<T>::node_type::e_vecelemrtc:
                return vec_elem_rtc_hot_view{node, static_cast<vector_elem_rtc_node_t*>(node)};

            case expression_node<T>::node_type::e_veccelemrtc:
                return vec_celem_rtc_hot_view{node, static_cast<vector_celem_rtc_node_t*>(node)};

            case expression_node<T>::node_type::e_rbvecelem:
                return rbvec_elem_hot_view{node, static_cast<rebasevector_elem_node_t*>(node)};

            case expression_node<T>::node_type::e_rbveccelem:
                return rbvec_celem_hot_view{node, static_cast<rebasevector_celem_node_t*>(node)};

            case expression_node<T>::node_type::e_rbvecelemrtc:
                return rbvec_elem_rtc_hot_view{node,
                                               static_cast<rebasevector_elem_rtc_node_t*>(node)};

            case expression_node<T>::node_type::e_rbveccelemrtc:
                return rbvec_celem_rtc_hot_view{node,
                                                static_cast<rebasevector_celem_rtc_node_t*>(node)};

            case expression_node<T>::node_type::e_vecsize:
                return vecsize_hot_view{node, static_cast<vector_size_node_t*>(node)};

            case expression_node<T>::node_type::e_vecvalass:
                if (typeid(*node) == typeid(assignment_vec_node_t))
                    return assign_vec_scalar_hot_view{node,
                                                      static_cast<assignment_vec_node_t*>(node)};
                return fallback_view{node};

            case expression_node<T>::node_type::e_vecopvalass:
            {
                const auto op = static_cast<binary_node_t*>(node)->operation();
                if (const auto read_op = compound_to_read_op(op); read_op.has_value())
                {
                    if (nullptr != node->as_vector_iface())
                        return assign_vec_scalar_op_hot_view{node, *read_op};
                }
                return fallback_view{node};
            }

            case expression_node<T>::node_type::e_vecopvecass:
            {
                const auto op = static_cast<binary_node_t*>(node)->operation();
                if (const auto read_op = compound_to_read_op(op); read_op.has_value())
                {
                    if (nullptr != node->as_vector_iface())
                        return assign_vecvec_op_hot_view{node, *read_op};
                }
                return fallback_view{node};
            }

            case expression_node<T>::node_type::e_while:
                if (typeid(*node) == typeid(while_loop_node_t))
                    return while_hot_view{node, static_cast<while_loop_node_t*>(node)};
                if (typeid(*node) == typeid(while_loop_rtc_node_t))
                    return while_rtc_hot_view{node, static_cast<while_loop_rtc_node_t*>(node)};
                return fallback_view{node};

            case expression_node<T>::node_type::e_repeat:
                if (typeid(*node) == typeid(repeat_until_loop_node_t))
                    return repeat_until_hot_view{node,
                                                 static_cast<repeat_until_loop_node_t*>(node)};
                if (typeid(*node) == typeid(repeat_until_loop_rtc_node_t))
                    return repeat_until_rtc_hot_view{
                        node, static_cast<repeat_until_loop_rtc_node_t*>(node)};
                return fallback_view{node};

            case expression_node<T>::node_type::e_for:
                if (typeid(*node) == typeid(for_loop_node_t))
                    return for_hot_view{node, static_cast<for_loop_node_t*>(node)};
                if (typeid(*node) == typeid(for_loop_rtc_node_t))
                    return for_rtc_hot_view{node, static_cast<for_loop_rtc_node_t*>(node)};
                return fallback_view{node};

            case expression_node<T>::node_type::e_switch:
                return switch_hot_view{node, static_cast<switch_node_t*>(node)};

            case expression_node<T>::node_type::e_mswitch:
                if (typeid(*node) == typeid(multi_switch_node_t))
                    return multi_switch_hot_view{node, static_cast<multi_switch_node_t*>(node)};
                return fallback_view{node};

            case expression_node<T>::node_type::e_vecvecass:
            {
                if (typeid(*node) != typeid(assignment_vecvec_node_t))
                    return fallback_view{node};
                auto* n = static_cast<assignment_vecvec_node_t*>(node);
                if (n->is_src_ivec())
                    return fallback_view{node};
                return assign_vecvec_hot_view{node, n};
            }

            case expression_node<T>::node_type::e_vecinit:
            {
                if (typeid(*node) == typeid(vector_init_zero_node_t))
                {
                    auto* n = static_cast<vector_init_zero_node_t*>(node);
                    return vecinit_zero_hot_view{node, n->vec_base(), n->vec_size()};
                }
                if (typeid(*node) == typeid(vector_init_constfill_node_t))
                {
                    auto* n = static_cast<vector_init_constfill_node_t*>(node);
                    return vecinit_constfill_hot_view{node, n->vec_base(), n->vec_size(),
                                                      n->init_value()};
                }
                if (typeid(*node) == typeid(vector_init_dynfill_node_t))
                {
                    auto* n = static_cast<vector_init_dynfill_node_t*>(node);
                    return vecinit_dynfill_hot_view{node, n->vec_base(), n->vec_size(),
                                                    n->init_child()};
                }
                if (typeid(*node) == typeid(vector_init_iota_cc_node_t))
                {
                    auto* n = static_cast<vector_init_iota_cc_node_t*>(node);
                    return vecinit_iota_cc_hot_view{node, n->vec_base(), n->vec_size(),
                                                    n->base_val(), n->increment_val()};
                }
                if (typeid(*node) == typeid(vector_init_iota_cnc_node_t))
                {
                    auto* n = static_cast<vector_init_iota_cnc_node_t*>(node);
                    return vecinit_iota_cnc_hot_view{node, n->vec_base(), n->vec_size(),
                                                     n->base_val(), n->increment_child()};
                }
                if (typeid(*node) == typeid(vector_init_iota_ncc_node_t))
                {
                    auto* n = static_cast<vector_init_iota_ncc_node_t*>(node);
                    return vecinit_iota_ncc_hot_view{node, n->vec_base(), n->vec_size(),
                                                     n->base_child(), n->increment_child()};
                }
                if (typeid(*node) == typeid(vector_init_iota_ncnc_node_t))
                {
                    auto* n = static_cast<vector_init_iota_ncnc_node_t*>(node);
                    return vecinit_iota_ncnc_hot_view{node, n->vec_base(), n->vec_size(),
                                                      n->base_child(), n->increment_child()};
                }
                if (typeid(*node) == typeid(vector_init_general_node_t))
                {
                    auto* n = static_cast<vector_init_general_node_t*>(node);
                    return vecinit_general_hot_view{node, n};
                }
                return fallback_view{node};
            }

            case expression_node<T>::node_type::e_vararg:
            {
                auto* fn = dynamic_cast<vararg_evaluable_node_t*>(node);
                if (fn && fn->arg_count() > 0)
                    return vararg_evaluable_hot_view{node, fn};
                return fallback_view{node};
            }

            case expression_node<T>::node_type::e_vafunction:
            {
                auto* fn = dynamic_cast<vararg_evaluable_node_t*>(node);
                if (fn && fn->arg_count() > 0)
                    return vararg_evaluable_hot_view{node, fn};
                return fallback_view{node};
            }

            case expression_node<T>::node_type::e_vecvecarith:
            {
                auto* vi = node->as_vector_iface();
                if (nullptr == vi || vi->vec()->vec_holder().rebaseable())
                    return fallback_view{node};
                auto* b0 = node->branch(0);
                auto* b1 = node->branch(1);
                if (nullptr == b0 || nullptr == b1)
                    return fallback_view{node};
                auto* vi0 = b0->as_vector_iface();
                auto* vi1 = b1->as_vector_iface();
                if (nullptr == vi0 || nullptr == vi1)
                    return fallback_view{node};
                if (vi0->vec()->vec_holder().rebaseable())
                    return fallback_view{node};
                if (vi1->vec()->vec_holder().rebaseable())
                    return fallback_view{node};
                return vec_binop_vecvec_hot_view{node,
                                                 static_cast<binary_node_t*>(node)->operation()};
            }

            case expression_node<T>::node_type::e_vecvalarith:
            {
                auto* vi = node->as_vector_iface();
                if (nullptr == vi || vi->vec()->vec_holder().rebaseable())
                    return fallback_view{node};
                auto* b0 = node->branch(0);
                auto* b1 = node->branch(1);
                if (nullptr == b0 || nullptr == b1)
                    return fallback_view{node};
                const auto op = static_cast<binary_node_t*>(node)->operation();
                auto* vi0 = b0->as_vector_iface();
                auto* vi1 = b1->as_vector_iface();
                if (vi0 != nullptr && vi1 == nullptr)
                {
                    if (vi0->vec()->vec_holder().rebaseable())
                        return fallback_view{node};
                    return vec_binop_vecval_hot_view{node, op};
                }
                if (vi0 == nullptr && vi1 != nullptr)
                {
                    if (vi1->vec()->vec_holder().rebaseable())
                        return fallback_view{node};
                    return vec_binop_valvec_hot_view{node, op};
                }
                return fallback_view{node};
            }

            case expression_node<T>::node_type::e_vecunaryop:
            {
                auto* vi = node->as_vector_iface();
                if (nullptr == vi || vi->vec()->vec_holder().rebaseable())
                    return fallback_view{node};
                auto* b0 = node->branch(0);
                if (nullptr == b0)
                    return fallback_view{node};
                auto* vi0 = b0->as_vector_iface();
                if (nullptr == vi0 || vi0->vec()->vec_holder().rebaseable())
                    return fallback_view{node};
                return unary_vec_hot_view{node, static_cast<unary_node_t*>(node)->operation()};
            }

            case expression_node<T>::node_type::e_vecfunc:
            {
                auto* fn = dynamic_cast<vectorize_evaluable_node_t*>(node);
                if (fn && fn->ivec())
                    return vecfunc_hot_view{node, fn};
                return fallback_view{node};
            }

            default:
                return fallback_view{node};
        }
    }

    template <typename Alternative>
    static inline bool holds(expression_ptr node)
    {
        return std::holds_alternative<Alternative>(classify(node));
    }

    template <typename Visitor>
    static inline decltype(auto) visit(expression_ptr node, Visitor&& visitor)
    {
        return std::visit(std::forward<Visitor>(visitor), classify(node));
    }

    static inline literal_node_t* literal(expression_ptr node)
    {
        const variant_type node_variant = classify(node);
        const auto* view = std::get_if<literal_view>(&node_variant);
        return view ? view->literal : nullptr;
    }

    static inline variable_node_t* variable(expression_ptr node)
    {
        const variant_type node_variant = classify(node);
        const auto* view = std::get_if<variable_view>(&node_variant);
        return view ? view->variable : nullptr;
    }

    static inline string_base_node_t* string_base(expression_ptr node)
    {
        const variant_type node_variant = classify(node);
        const auto* view = std::get_if<string_view>(&node_variant);
        return view ? view->base : nullptr;
    }

    static inline stringvar_node_t* string_variable(expression_ptr node)
    {
        const variant_type node_variant = classify(node);
        const auto* view = std::get_if<string_view>(&node_variant);
        return view ? view->mutable_node : nullptr;
    }

    static inline string_literal_node_t* const_string(expression_ptr node)
    {
        const variant_type node_variant = classify(node);
        const auto* view = std::get_if<string_view>(&node_variant);
        return (view && view->is_const_literal) ? static_cast<string_literal_node_t*>(view->node)
                                                : nullptr;
    }

    static inline bool is_string_variable(expression_ptr node)
    {
        return nullptr != string_variable(node);
    }

    static inline bool is_const_string(expression_ptr node)
    {
        return nullptr != const_string(node);
    }

    static inline bool is_null(expression_ptr node)
    {
        return holds<null_view>(node);
    }

    static inline bool is_literal(expression_ptr node)
    {
        return nullptr != literal(node);
    }

    static inline bool is_variable(expression_ptr node)
    {
        return nullptr != variable(node);
    }

    static inline expression_ptr branch(expression_ptr node, const std::size_t index = 0)
    {
        return node ? node->branch(index) : nullptr;
    }

    static inline uv_base_node_t* unary_variable_base(expression_ptr node)
    {
        return node ? node->as_uv_base_node() : nullptr;
    }

    static inline vov_base_node_t* vov_base(expression_ptr node)
    {
        return node ? node->as_vov_base() : nullptr;
    }

    static inline cov_base_node_t* cov_base(expression_ptr node)
    {
        return node ? node->as_cov_base() : nullptr;
    }

    static inline voc_base_node_t* voc_base(expression_ptr node)
    {
        return node ? node->as_voc_base() : nullptr;
    }

    static inline vob_base_node_t* vob_base(expression_ptr node)
    {
        return node ? node->as_vob_base() : nullptr;
    }

    static inline bov_base_node_t* bov_base(expression_ptr node)
    {
        return node ? node->as_bov_base() : nullptr;
    }

    static inline cob_base_node_t* cob_base(expression_ptr node)
    {
        return node ? node->as_cob_base() : nullptr;
    }

    static inline boc_base_node_t* boc_base(expression_ptr node)
    {
        return node ? node->as_boc_base() : nullptr;
    }

    static inline expression_ptr move_cob_branch(expression_ptr node, const std::size_t index = 0)
    {
        auto* base = cob_base(node);
        return base ? base->move_branch(index) : nullptr;
    }

    static inline expression_ptr move_boc_branch(expression_ptr node, const std::size_t index = 0)
    {
        auto* base = boc_base(node);
        return base ? base->move_branch(index) : nullptr;
    }

    static inline T value(expression_ptr node)
    {
        struct visitor
        {
            static inline T resolve(const t0ot1ot2_base_node_t* base, const std::size_t index)
            {
                return base->operand_is_reference(index) ? *base->operand_reference(index)
                                                         : base->operand_value(index);
            }

            static inline T resolve(const t0ot1ot2ot3_base_node_t* base, const std::size_t index)
            {
                return base->operand_is_reference(index) ? *base->operand_reference(index)
                                                         : base->operand_value(index);
            }

            static inline T trinary_value(trinary_node_t* trinary)
            {
                const T arg0 = node_variant_adapter::value(trinary->branch(0));
                const T arg1 = node_variant_adapter::value(trinary->branch(1));
                const T arg2 = node_variant_adapter::value(trinary->branch(2));

                switch (trinary->operation())
                {
                    case core::operators::operator_type::inrange:
                        return (arg1 < arg0) ? core::numeric::false_v<T>
                                             : ((arg1 > arg2) ? core::numeric::false_v<T>
                                                              : core::numeric::true_v<T>);

                    case core::operators::operator_type::clamp:
                        return (arg1 < arg0) ? arg0 : (arg1 > arg2 ? arg2 : arg1);

                    case core::operators::operator_type::iclamp:
                        if ((arg1 <= arg0) || (arg1 >= arg2))
                            return arg1;
                        else
                            return ((T(2) * arg1 <= (arg2 + arg0)) ? arg0 : arg2);

                    default:
                        return trinary->value();
                }
            }

            T operator()(std::monostate) const
            {
                return std::numeric_limits<T>::quiet_NaN();
            }

            T operator()(const literal_view& view) const
            {
                return view.literal->value();
            }

            T operator()(const variable_view& view) const
            {
                return view.variable->ref();
            }

            T operator()(const unary_hot_view& view) const
            {
                return core::operators::process<T>(
                    view.unary->operation(), node_variant_adapter::value(view.unary->branch(0)));
            }

            T operator()(const binary_hot_view& view) const
            {
                return core::operators::process<T>(
                    view.binary->operation(), node_variant_adapter::value(view.binary->branch(0)),
                    node_variant_adapter::value(view.binary->branch(1)));
            }

            T operator()(const trinary_hot_view& view) const
            {
                return trinary_value(view.trinary);
            }

            T operator()(const sf3_hot_view& view) const
            {
                return view.sf3->functor()(node_variant_adapter::value(view.sf3->branch(0)),
                                           node_variant_adapter::value(view.sf3->branch(1)),
                                           node_variant_adapter::value(view.sf3->branch(2)));
            }

            T operator()(const sf4_hot_view& view) const
            {
                return view.sf4->functor()(node_variant_adapter::value(view.sf4->branch(0)),
                                           node_variant_adapter::value(view.sf4->branch(1)),
                                           node_variant_adapter::value(view.sf4->branch(2)),
                                           node_variant_adapter::value(view.sf4->branch(3)));
            }

            T operator()(const fixed_function_hot_view& view) const
            {
                std::array<T, 20> values{};
                for (std::size_t i = 0; i < view.function->arity(); ++i)
                {
                    values[i] = node_variant_adapter::value(view.function->branch(i));
                }
                return view.function->evaluate_values(values.data());
            }

            T operator()(const conditional_hot_view& view) const
            {
                const T condition =
                    node_variant_adapter::value(node_variant_adapter::branch(view.node, 0));
                if (is_true(condition))
                {
                    return node_variant_adapter::value(node_variant_adapter::branch(view.node, 1));
                }

                expression_ptr alternative = node_variant_adapter::branch(view.node, 2);
                return alternative ? node_variant_adapter::value(alternative)
                                   : std::numeric_limits<T>::quiet_NaN();
            }

            T operator()(const uv_hot_view& view) const
            {
                return core::operators::process<T>(view.uv->operation(), view.uv->v());
            }

            T operator()(const scalar_pow_hot_view& view) const
            {
                const T base = view.pow->pow_function()(view.pow->v());
                return view.pow->reciprocal() ? (T(1) / base) : base;
            }

            T operator()(const branch_pow_hot_view& view) const
            {
                const T base =
                    view.pow->pow_function()(node_variant_adapter::value(view.pow->branch(0)));
                return view.pow->reciprocal() ? (T(1) / base) : base;
            }

            T operator()(const unary_branch_hot_view& view) const
            {
                return core::operators::process<T>(
                    view.operation,
                    node_variant_adapter::value(node_variant_adapter::branch(view.node)));
            }

            T operator()(const vov_hot_view& view) const
            {
                return core::operators::process<T>(view.vov->operation(), view.vov->v0(),
                                                   view.vov->v1());
            }

            T operator()(const cov_hot_view& view) const
            {
                return core::operators::process<T>(view.cov->operation(), view.cov->c(),
                                                   view.cov->v());
            }

            T operator()(const voc_hot_view& view) const
            {
                return core::operators::process<T>(view.voc->operation(), view.voc->v(),
                                                   view.voc->c());
            }

            T operator()(const vob_hot_view& view) const
            {
                return core::operators::process<T>(
                    view.vob->operation(), view.vob->v(),
                    node_variant_adapter::value(node_variant_adapter::branch(view.node)));
            }

            T operator()(const bov_hot_view& view) const
            {
                return core::operators::process<T>(
                    view.bov->operation(),
                    node_variant_adapter::value(node_variant_adapter::branch(view.node)),
                    view.bov->v());
            }

            T operator()(const cob_hot_view& view) const
            {
                return core::operators::process<T>(
                    view.cob->operation(), view.cob->c(),
                    node_variant_adapter::value(node_variant_adapter::branch(view.node)));
            }

            T operator()(const boc_hot_view& view) const
            {
                return core::operators::process<T>(
                    view.boc->operation(),
                    node_variant_adapter::value(node_variant_adapter::branch(view.node)),
                    view.boc->c());
            }

            T operator()(const uvouv_hot_view& view) const
            {
                return view.uvouv->f()(view.uvouv->u0()(view.uvouv->v0()),
                                       view.uvouv->u1()(view.uvouv->v1()));
            }

            T operator()(const t0ot1ot2_hot_view& view) const
            {
                const T t0 = resolve(view.base, 0);
                const T t1 = resolve(view.base, 1);
                const T t2 = resolve(view.base, 2);

                if (nullptr != view.base->ternary_functor())
                {
                    return view.base->ternary_functor()(t0, t1, t2);
                }

                switch (view.base->mode_index())
                {
                    case 0:
                        return view.base->binary_functor(1)(view.base->binary_functor(0)(t0, t1),
                                                            t2);
                    case 1:
                        return view.base->binary_functor(0)(t0,
                                                            view.base->binary_functor(1)(t1, t2));
                    default:
                        return view.node->value();
                }
            }

            T operator()(const t0ot1ot2ot3_hot_view& view) const
            {
                const T t0 = resolve(view.base, 0);
                const T t1 = resolve(view.base, 1);
                const T t2 = resolve(view.base, 2);
                const T t3 = resolve(view.base, 3);

                if (nullptr != view.base->quaternary_functor())
                {
                    return view.base->quaternary_functor()(t0, t1, t2, t3);
                }

                switch (view.base->mode_index())
                {
                    case 0:
                        return view.base->binary_functor(1)(view.base->binary_functor(0)(t0, t1),
                                                            view.base->binary_functor(2)(t2, t3));
                    case 1:
                        return view.base->binary_functor(0)(
                            t0,
                            view.base->binary_functor(1)(t1, view.base->binary_functor(2)(t2, t3)));
                    case 2:
                        return view.base->binary_functor(0)(
                            t0,
                            view.base->binary_functor(2)(view.base->binary_functor(1)(t1, t2), t3));
                    case 3:
                        return view.base->binary_functor(2)(
                            view.base->binary_functor(1)(view.base->binary_functor(0)(t0, t1), t2),
                            t3);
                    case 4:
                        return view.base->binary_functor(2)(
                            view.base->binary_functor(0)(t0, view.base->binary_functor(1)(t1, t2)),
                            t3);
                    default:
                        return view.node->value();
                }
            }

            T operator()(const scand_hot_view& view) const
            {
                if (!is_true(
                        node_variant_adapter::value(node_variant_adapter::branch(view.node, 0))))
                {
                    return core::numeric::false_v<T>;
                }

                return is_true(
                           node_variant_adapter::value(node_variant_adapter::branch(view.node, 1)))
                           ? core::numeric::true_v<T>
                           : core::numeric::false_v<T>;
            }

            T operator()(const scor_hot_view& view) const
            {
                if (is_true(
                        node_variant_adapter::value(node_variant_adapter::branch(view.node, 0))))
                {
                    return core::numeric::true_v<T>;
                }

                return is_true(
                           node_variant_adapter::value(node_variant_adapter::branch(view.node, 1)))
                           ? core::numeric::true_v<T>
                           : core::numeric::false_v<T>;
            }

            T operator()(const nulleq_hot_view& view) const
            {
                const T value =
                    node_variant_adapter::value(node_variant_adapter::branch(view.node, 0));
                const bool result = core::numeric::is_nan(value);
                if (result)
                {
                    return view.nulleq->equality() ? core::numeric::true_v<T>
                                                   : core::numeric::false_v<T>;
                }

                return view.nulleq->equality() ? core::numeric::false_v<T>
                                               : core::numeric::true_v<T>;
            }

            T operator()(const vararg_multi_hot_view& view) const
            {
                for (std::size_t i = 0; i + 1 < view.count; ++i)
                {
                    node_variant_adapter::value(view.node->branch(i));
                }
                return node_variant_adapter::value(view.node->branch(view.count - 1));
            }

            T operator()(const vec_celem_hot_view& view) const
            {
                node_variant_adapter::value(view.celem->vec_branch());
                return *(view.celem->vec_data() + view.celem->elem_idx());
            }

            T operator()(const vec_elem_hot_view& view) const
            {
                node_variant_adapter::value(view.elem->vec_branch());
                const auto idx = core::numeric::to_uint64(
                    node_variant_adapter::value(view.elem->index_branch()));
                return *(view.elem->vec_data() + idx);
            }

            T operator()(const swap_hot_view& view) const
            {
                std::swap(view.swap_ptr->var0_ptr()->ref(), view.swap_ptr->var1_ptr()->ref());
                return view.swap_ptr->var1_ptr()->ref();
            }

            T operator()(const vec_elem_rtc_hot_view& view) const
            {
                const std::uint64_t index =
                    core::numeric::to_uint64(node_variant_adapter::value(view.rtc->index_branch()));
                node_variant_adapter::value(view.rtc->vec_branch());

                if (index <= view.rtc->max_idx())
                {
                    return *(view.rtc->holder()->data() + index);
                }

                typename vector_access_runtime_check<T>::violation_context context;
                context.base_ptr = view.rtc->vec_data();
                context.end_ptr = view.rtc->vec_data() + view.rtc->holder()->size();
                context.access_ptr = view.rtc->vec_data() + index;
                context.type_size = sizeof(T);

                T* result_ptr = view.rtc->rt_check()->handle_runtime_violation(context)
                                    ? context.access_ptr
                                    : view.rtc->vec_data();
                return *result_ptr;
            }

            T operator()(const vec_celem_rtc_hot_view& view) const
            {
                node_variant_adapter::value(view.rtc->vec_branch());
                const std::size_t index = view.rtc->elem_idx();

                if (index <= view.rtc->max_idx())
                {
                    return *(view.rtc->holder()->data() + index);
                }

                typename vector_access_runtime_check<T>::violation_context context;
                context.base_ptr = view.rtc->vec_data();
                context.end_ptr = view.rtc->vec_data() + view.rtc->holder()->size();
                context.access_ptr = view.rtc->vec_data() + index;
                context.type_size = sizeof(T);

                T* result_ptr = view.rtc->rt_check()->handle_runtime_violation(context)
                                    ? context.access_ptr
                                    : view.rtc->vec_data();
                return *result_ptr;
            }

            T operator()(const rbvec_elem_hot_view& view) const
            {
                node_variant_adapter::value(view.rbvec->vec_branch());
                const auto idx = core::numeric::to_uint64(
                    node_variant_adapter::value(view.rbvec->index_branch()));
                return *(view.rbvec->holder()->data() + idx);
            }

            T operator()(const rbvec_celem_hot_view& view) const
            {
                node_variant_adapter::value(view.rbvec->vec_branch());
                return *(view.rbvec->holder()->data() + view.rbvec->elem_idx());
            }

            T operator()(const rbvec_elem_rtc_hot_view& view) const
            {
                node_variant_adapter::value(view.rtc->vec_branch());
                const std::uint64_t index =
                    core::numeric::to_uint64(node_variant_adapter::value(view.rtc->index_branch()));

                if (index <= view.rtc->holder()->size() - 1)
                {
                    return *(view.rtc->holder()->data() + index);
                }

                typename vector_access_runtime_check<T>::violation_context context;
                context.base_ptr = view.rtc->holder()->data();
                context.end_ptr = view.rtc->holder()->data() + view.rtc->holder()->size();
                context.access_ptr = view.rtc->holder()->data() + index;
                context.type_size = sizeof(T);

                T* result_ptr = view.rtc->rt_check()->handle_runtime_violation(context)
                                    ? context.access_ptr
                                    : view.rtc->holder()->data();
                return *result_ptr;
            }

            T operator()(const rbvec_celem_rtc_hot_view& view) const
            {
                node_variant_adapter::value(view.rtc->vec_branch());
                const std::size_t index = view.rtc->elem_idx();

                if (index <= view.rtc->holder()->size() - 1)
                {
                    return *(view.rtc->holder()->data() + index);
                }

                typename vector_access_runtime_check<T>::violation_context context;
                context.base_ptr = view.rtc->vec_data();
                context.end_ptr = view.rtc->vec_data() + view.rtc->holder()->size();
                context.access_ptr = view.rtc->vec_data() + index;
                context.type_size = sizeof(T);

                T* result_ptr = view.rtc->rt_check()->handle_runtime_violation(context)
                                    ? context.access_ptr
                                    : view.rtc->vec_data();
                return *result_ptr;
            }

            T operator()(const vecsize_hot_view& view) const
            {
                return static_cast<T>(view.vecsize->vec_holder()->size());
            }

            T operator()(const assign_hot_view& view) const
            {
                T& ref = view.assign->var_node()->ref();
                ref = node_variant_adapter::value(node_variant_adapter::branch(view.node, 1));
                return ref;
            }

            T operator()(const assign_vec_elem_hot_view& view) const
            {
                const vector_elem_node_t* elem = view.assign->elem_node_ptr();
                node_variant_adapter::value(elem->vec_branch());
                const auto idx =
                    core::numeric::to_uint64(node_variant_adapter::value(elem->index_branch()));
                T& ref = *(elem->vec_data() + idx);
                ref = node_variant_adapter::value(node_variant_adapter::branch(view.node, 1));
                return ref;
            }

            T operator()(const assign_rbvec_elem_hot_view& view) const
            {
                const rebasevector_elem_node_t* elem = view.assign->rbvec_elem_node_ptr();
                node_variant_adapter::value(elem->vec_branch());
                const auto idx =
                    core::numeric::to_uint64(node_variant_adapter::value(elem->index_branch()));
                T& ref = *(elem->holder()->data() + idx);
                ref = node_variant_adapter::value(node_variant_adapter::branch(view.node, 1));
                return ref;
            }

            T operator()(const assign_rbvec_celem_hot_view& view) const
            {
                const rebasevector_celem_node_t* elem = view.assign->rbvec_celem_node_ptr();
                node_variant_adapter::value(elem->vec_branch());
                T& ref = *(elem->holder()->data() + elem->elem_idx());
                ref = node_variant_adapter::value(node_variant_adapter::branch(view.node, 1));
                return ref;
            }

            T operator()(const assign_op_hot_view& view) const
            {
                T& v = view.var->ref();
                v = core::operators::process<T>(
                    view.read_op, v,
                    node_variant_adapter::value(node_variant_adapter::branch(view.node, 1)));
                return v;
            }

            T operator()(const assign_vec_elem_op_hot_view& view) const
            {
                node_variant_adapter::value(view.elem->vec_branch());
                const auto idx = core::numeric::to_uint64(
                    node_variant_adapter::value(view.elem->index_branch()));
                T& ref = *(view.elem->vec_data() + idx);
                ref = core::operators::process<T>(
                    view.read_op, ref,
                    node_variant_adapter::value(node_variant_adapter::branch(view.node, 1)));
                return ref;
            }

            T operator()(const assign_vec_elem_rtc_hot_view& view) const
            {
                const vector_elem_rtc_node_t* elem = view.assign->elem_rtc_node_ptr();
                node_variant_adapter::value(elem->vec_branch());
                const auto idx =
                    core::numeric::to_uint64(node_variant_adapter::value(elem->index_branch()));
                T* ptr;
                if (idx <= elem->max_idx())
                {
                    ptr = elem->holder()->data() + idx;
                }
                else
                {
                    typename vector_access_runtime_check<T>::violation_context context;
                    context.base_ptr = elem->vec_data();
                    context.end_ptr = elem->vec_data() + elem->holder()->size();
                    context.access_ptr = elem->vec_data() + idx;
                    context.type_size = sizeof(T);
                    ptr = elem->rt_check()->handle_runtime_violation(context) ? context.access_ptr
                                                                              : elem->vec_data();
                }
                T& ref = *ptr;
                ref = node_variant_adapter::value(node_variant_adapter::branch(view.node, 1));
                return ref;
            }

            T operator()(const assign_rbvec_elem_rtc_hot_view& view) const
            {
                const rebasevector_elem_rtc_node_t* elem = view.assign->rbvec_elem_rtc_node_ptr();
                node_variant_adapter::value(elem->vec_branch());
                const auto idx =
                    core::numeric::to_uint64(node_variant_adapter::value(elem->index_branch()));
                T* ptr;
                if (idx <= elem->holder()->size() - 1)
                {
                    ptr = elem->holder()->data() + idx;
                }
                else
                {
                    typename vector_access_runtime_check<T>::violation_context context;
                    context.base_ptr = elem->holder()->data();
                    context.end_ptr = elem->holder()->data() + elem->holder()->size();
                    context.access_ptr = elem->holder()->data() + idx;
                    context.type_size = sizeof(T);
                    ptr = elem->rt_check()->handle_runtime_violation(context)
                              ? context.access_ptr
                              : elem->holder()->data();
                }
                T& ref = *ptr;
                ref = node_variant_adapter::value(node_variant_adapter::branch(view.node, 1));
                return ref;
            }

            T operator()(const assign_rbvec_elem_op_hot_view& view) const
            {
                node_variant_adapter::value(view.elem->vec_branch());
                const auto idx = core::numeric::to_uint64(
                    node_variant_adapter::value(view.elem->index_branch()));
                T& ref = *(view.elem->holder()->data() + idx);
                ref = core::operators::process<T>(
                    view.read_op, ref,
                    node_variant_adapter::value(node_variant_adapter::branch(view.node, 1)));
                return ref;
            }

            T operator()(const assign_rbvec_celem_op_hot_view& view) const
            {
                node_variant_adapter::value(view.elem->vec_branch());
                T& ref = *(view.elem->holder()->data() + view.elem->elem_idx());
                ref = core::operators::process<T>(
                    view.read_op, ref,
                    node_variant_adapter::value(node_variant_adapter::branch(view.node, 1)));
                return ref;
            }

            T operator()(const assign_vec_elem_op_rtc_hot_view& view) const
            {
                node_variant_adapter::value(view.elem->vec_branch());
                const auto idx = core::numeric::to_uint64(
                    node_variant_adapter::value(view.elem->index_branch()));
                T* ptr;
                if (idx <= view.elem->max_idx())
                {
                    ptr = view.elem->holder()->data() + idx;
                }
                else
                {
                    typename vector_access_runtime_check<T>::violation_context context;
                    context.base_ptr = view.elem->vec_data();
                    context.end_ptr = view.elem->vec_data() + view.elem->holder()->size();
                    context.access_ptr = view.elem->vec_data() + idx;
                    context.type_size = sizeof(T);
                    ptr = view.elem->rt_check()->handle_runtime_violation(context)
                              ? context.access_ptr
                              : view.elem->vec_data();
                }
                T& ref = *ptr;
                ref = core::operators::process<T>(
                    view.read_op, ref,
                    node_variant_adapter::value(node_variant_adapter::branch(view.node, 1)));
                return ref;
            }

            T operator()(const assign_vec_celem_op_rtc_hot_view& view) const
            {
                node_variant_adapter::value(view.elem->vec_branch());
                T* ptr;
                if (view.elem->elem_idx() <= view.elem->max_idx())
                {
                    ptr = view.elem->holder()->data() + view.elem->elem_idx();
                }
                else
                {
                    typename vector_access_runtime_check<T>::violation_context context;
                    context.base_ptr = view.elem->vec_data();
                    context.end_ptr = view.elem->vec_data() + view.elem->holder()->size();
                    context.access_ptr = view.elem->vec_data() + view.elem->elem_idx();
                    context.type_size = sizeof(T);
                    ptr = view.elem->rt_check()->handle_runtime_violation(context)
                              ? context.access_ptr
                              : view.elem->vec_data();
                }
                T& ref = *ptr;
                ref = core::operators::process<T>(
                    view.read_op, ref,
                    node_variant_adapter::value(node_variant_adapter::branch(view.node, 1)));
                return ref;
            }

            T operator()(const assign_rbvec_elem_op_rtc_hot_view& view) const
            {
                node_variant_adapter::value(view.elem->vec_branch());
                const auto idx = core::numeric::to_uint64(
                    node_variant_adapter::value(view.elem->index_branch()));
                T* ptr;
                if (idx <= view.elem->holder()->size() - 1)
                {
                    ptr = view.elem->holder()->data() + idx;
                }
                else
                {
                    typename vector_access_runtime_check<T>::violation_context context;
                    context.base_ptr = view.elem->holder()->data();
                    context.end_ptr = view.elem->holder()->data() + view.elem->holder()->size();
                    context.access_ptr = view.elem->holder()->data() + idx;
                    context.type_size = sizeof(T);
                    ptr = view.elem->rt_check()->handle_runtime_violation(context)
                              ? context.access_ptr
                              : view.elem->holder()->data();
                }
                T& ref = *ptr;
                ref = core::operators::process<T>(
                    view.read_op, ref,
                    node_variant_adapter::value(node_variant_adapter::branch(view.node, 1)));
                return ref;
            }

            T operator()(const assign_rbvec_celem_op_rtc_hot_view& view) const
            {
                node_variant_adapter::value(view.elem->vec_branch());
                T* ptr;
                if (view.elem->elem_idx() <= view.elem->holder()->size() - 1)
                {
                    ptr = view.elem->holder()->data() + view.elem->elem_idx();
                }
                else
                {
                    typename vector_access_runtime_check<T>::violation_context context;
                    context.base_ptr = view.elem->vec_data();
                    context.end_ptr = view.elem->vec_data() + view.elem->holder()->size();
                    context.access_ptr = view.elem->vec_data() + view.elem->elem_idx();
                    context.type_size = sizeof(T);
                    ptr = view.elem->rt_check()->handle_runtime_violation(context)
                              ? context.access_ptr
                              : view.elem->vec_data();
                }
                T& ref = *ptr;
                ref = core::operators::process<T>(
                    view.read_op, ref,
                    node_variant_adapter::value(node_variant_adapter::branch(view.node, 1)));
                return ref;
            }

            T operator()(const assign_vec_scalar_hot_view& view) const
            {
                const T v = node_variant_adapter::value(node_variant_adapter::branch(view.node, 1));
                T* vec = view.assign->vds().data();
                const std::size_t sz = view.assign->vec_holder_ptr()->size();
                core::operators::loop_unroll lud(sz);
                const T* upper_bound = vec + lud.upper_bound;
                while (vec < upper_bound)
                {
                    lud.foreach_batch([v, &vec](unsigned int i) { vec[i] = v; });
                    vec += lud.loop_batch_size;
                }
                lud.foreach_remainder([&vec, v]() { *vec++ = v; });
                return view.assign->vds().data()[0];
            }

            T operator()(const assign_vec_scalar_op_hot_view& view) const
            {
                auto* vi = view.node->as_vector_iface();
                const T v = node_variant_adapter::value(node_variant_adapter::branch(view.node, 1));
                T* vec = vi->vds().data();
                const std::size_t sz = vi->size();
                core::operators::loop_unroll lud(sz);
                const T* upper_bound = vec + lud.upper_bound;
                while (vec < upper_bound)
                {
                    lud.foreach_batch(
                        [&](unsigned int i)
                        { vec[i] = core::operators::process<T>(view.read_op, vec[i], v); });
                    vec += lud.loop_batch_size;
                }
                lud.foreach_remainder(
                    [&vec, v, this, &view]()
                    {
                        *vec = core::operators::process<T>(view.read_op, *vec, v);
                        ++vec;
                    });
                return vi->vds().data()[0];
            }

            T operator()(const while_hot_view& view) const
            {
                T result{};
                while (is_true(node_variant_adapter::value(view.loop->condition_branch())))
                    result = node_variant_adapter::value(view.loop->body_branch());
                return result;
            }

            T operator()(const while_rtc_hot_view& view) const
            {
                T result{};
                view.loop->rt_checker()->reset();
                while (is_true(node_variant_adapter::value(view.loop->condition_branch())) &&
                       view.loop->rt_checker()->check())
                    result = node_variant_adapter::value(view.loop->body_branch());
                return result;
            }

            T operator()(const repeat_until_hot_view& view) const
            {
                T result{};
                do
                {
                    result = node_variant_adapter::value(view.loop->body_branch());
                } while (is_false(node_variant_adapter::value(view.loop->condition_branch())));
                return result;
            }

            T operator()(const repeat_until_rtc_hot_view& view) const
            {
                T result{};
                view.loop->rt_checker()->reset(1);
                do
                {
                    result = node_variant_adapter::value(view.loop->body_branch());
                } while (is_false(node_variant_adapter::value(view.loop->condition_branch())) &&
                         view.loop->rt_checker()->check());
                return result;
            }

            T operator()(const for_hot_view& view) const
            {
                T result{};
                if (view.loop->initialiser_branch())
                    node_variant_adapter::value(view.loop->initialiser_branch());
                if (view.loop->incrementor_branch())
                {
                    while (is_true(node_variant_adapter::value(view.loop->condition_branch())))
                    {
                        result = node_variant_adapter::value(view.loop->body_branch());
                        node_variant_adapter::value(view.loop->incrementor_branch());
                    }
                }
                else
                {
                    while (is_true(node_variant_adapter::value(view.loop->condition_branch())))
                        result = node_variant_adapter::value(view.loop->body_branch());
                }
                return result;
            }

            T operator()(const for_rtc_hot_view& view) const
            {
                T result{};
                view.loop->rt_checker()->reset();
                if (view.loop->initialiser_branch())
                    node_variant_adapter::value(view.loop->initialiser_branch());
                if (view.loop->incrementor_branch())
                {
                    while (is_true(node_variant_adapter::value(view.loop->condition_branch())) &&
                           view.loop->rt_checker()->check())
                    {
                        result = node_variant_adapter::value(view.loop->body_branch());
                        node_variant_adapter::value(view.loop->incrementor_branch());
                    }
                }
                else
                {
                    while (is_true(node_variant_adapter::value(view.loop->condition_branch())) &&
                           view.loop->rt_checker()->check())
                        result = node_variant_adapter::value(view.loop->body_branch());
                }
                return result;
            }

            T operator()(const switch_hot_view& view) const
            {
                const auto& args = view.sw->arg_list();
                const std::size_t upper_bound = args.size() - 1;
                for (std::size_t i = 0; i < upper_bound; i += 2)
                {
                    if (is_true(node_variant_adapter::value(args[i].first)))
                        return node_variant_adapter::value(args[i + 1].first);
                }
                return node_variant_adapter::value(args[upper_bound].first);
            }

            T operator()(const multi_switch_hot_view& view) const
            {
                const auto& args = view.sw->arg_list();
                T result{};
                for (std::size_t i = 0; i + 1 < args.size(); i += 2)
                {
                    if (is_true(node_variant_adapter::value(args[i].first)))
                        result = node_variant_adapter::value(args[i + 1].first);
                }
                return result;
            }

            T operator()(const assign_vecvec_hot_view& view) const
            {
                node_variant_adapter::value(node_variant_adapter::branch(view.node, 1));
                const std::size_t sz = std::min(view.assign->vec0_ptr()->vec_holder().size(),
                                                view.assign->vec1_ptr()->vec_holder().size());
                T* vec0 = view.assign->vec0_ptr()->vds().data();
                T* vec1 = view.assign->vec1_ptr()->vds().data();
                core::operators::loop_unroll lud(sz);
                const T* upper_bound = vec0 + lud.upper_bound;
                while (vec0 < upper_bound)
                {
                    lud.foreach_batch([&vec0, &vec1](unsigned int i) { vec0[i] = vec1[i]; });
                    vec0 += lud.loop_batch_size;
                    vec1 += lud.loop_batch_size;
                }
                lud.foreach_remainder([&vec0, &vec1]() { *vec0++ = *vec1++; });
                return view.assign->vec0_ptr()->vds().data()[0];
            }

            T operator()(const assign_vecvec_op_hot_view& view) const
            {
                node_variant_adapter::value(node_variant_adapter::branch(view.node, 0));
                node_variant_adapter::value(node_variant_adapter::branch(view.node, 1));
                auto* vi0 = view.node->as_vector_iface();
                auto* vi1 = node_variant_adapter::branch(view.node, 1)->as_vector_iface();
                T* vec0 = vi0->vds().data();
                const T* vec1 = vi1->vds().data();
                const std::size_t sz = vi0->size();
                core::operators::loop_unroll lud(sz);
                const T* upper_bound = vec0 + lud.upper_bound;
                while (vec0 < upper_bound)
                {
                    lud.foreach_batch(
                        [&](unsigned int i)
                        { vec0[i] = core::operators::process<T>(view.read_op, vec0[i], vec1[i]); });
                    vec0 += lud.loop_batch_size;
                    vec1 += lud.loop_batch_size;
                }
                lud.foreach_remainder(
                    [&vec0, &vec1, this, &view]()
                    {
                        *vec0 = core::operators::process<T>(view.read_op, *vec0, *vec1);
                        ++vec0;
                        ++vec1;
                    });
                return vi0->vds().data()[0];
            }

            T operator()(const vecinit_zero_hot_view& view) const
            {
                core::numeric::set_zero_value(view.vec_base, view.vec_size);
                return *view.vec_base;
            }

            T operator()(const vecinit_constfill_hot_view& view) const
            {
                for (std::size_t i = 0; i < view.vec_size; ++i)
                    *(view.vec_base + i) = view.fill_value;
                return *view.vec_base;
            }

            T operator()(const vecinit_dynfill_hot_view& view) const
            {
                const T v = node_variant_adapter::value(view.init_child);
                for (std::size_t i = 0; i < view.vec_size; ++i) *(view.vec_base + i) = v;
                return *view.vec_base;
            }

            T operator()(const vecinit_iota_cc_hot_view& view) const
            {
                T v = view.base_val;
                for (std::size_t i = 0; i < view.vec_size; ++i, v += view.increment_val)
                    *(view.vec_base + i) = v;
                return *view.vec_base;
            }

            T operator()(const vecinit_iota_cnc_hot_view& view) const
            {
                T v = view.base_val;
                expression_node<T>& increment = *view.increment_child;
                for (std::size_t i = 0; i < view.vec_size; ++i, v += increment.value())
                    *(view.vec_base + i) = v;
                return *view.vec_base;
            }

            T operator()(const vecinit_iota_ncc_hot_view& view) const
            {
                T v = node_variant_adapter::value(view.base_child);
                const T incr = view.increment_child->value();
                for (std::size_t i = 0; i < view.vec_size; ++i, v += incr) *(view.vec_base + i) = v;
                return *view.vec_base;
            }

            T operator()(const vecinit_iota_ncnc_hot_view& view) const
            {
                T v = node_variant_adapter::value(view.base_child);
                expression_node<T>& increment = *view.increment_child;
                for (std::size_t i = 0; i < view.vec_size; ++i, v += increment.value())
                    *(view.vec_base + i) = v;
                return *view.vec_base;
            }

            T operator()(const vecinit_general_hot_view& view) const
            {
                const auto& list = view.vinit->initialiser_list();
                if (view.vinit->is_single_value())
                {
                    const T v = list.empty() ? T(0) : node_variant_adapter::value(list[0]);
                    for (std::size_t i = 0; i < view.vinit->vec_size(); ++i)
                        *(view.vinit->vec_base() + i) = v;
                }
                else
                {
                    const std::size_t n = list.size();
                    for (std::size_t i = 0; i < n; ++i)
                        *(view.vinit->vec_base() + i) = node_variant_adapter::value(list[i]);
                    if (n < view.vinit->vec_size())
                        core::numeric::set_zero_value(view.vinit->vec_base() + n,
                                                      view.vinit->vec_size() - n);
                }
                return *view.vinit->vec_base();
            }

            T operator()(const vararg_evaluable_hot_view& view) const
            {
                const std::size_t n = view.fn->arg_count();
                std::vector<T> vals(n);
                for (std::size_t i = 0; i < n; ++i)
                    vals[i] = node_variant_adapter::value(view.fn->arg_at(i));
                return view.fn->process_values(vals);
            }

            T operator()(const vec_binop_vecvec_hot_view& view) const
            {
                node_variant_adapter::value(node_variant_adapter::branch(view.node, 0));
                node_variant_adapter::value(node_variant_adapter::branch(view.node, 1));
                auto* vi = view.node->as_vector_iface();
                auto* vi0 = node_variant_adapter::branch(view.node, 0)->as_vector_iface();
                auto* vi1 = node_variant_adapter::branch(view.node, 1)->as_vector_iface();
                const T* vec0 = vi0->vds().data();
                const T* vec1 = vi1->vds().data();
                T* vec_out = vi->vds().data();
                const std::size_t sz = std::min(vi0->size(), vi1->size());
                core::operators::loop_unroll lud(sz);
                const T* upper_bound = vec0 + lud.upper_bound;
                while (vec0 < upper_bound)
                {
                    lud.foreach_batch(
                        [&](unsigned int i)
                        {
                            vec_out[i] =
                                core::operators::process<T>(view.operation, vec0[i], vec1[i]);
                        });
                    vec0 += lud.loop_batch_size;
                    vec1 += lud.loop_batch_size;
                    vec_out += lud.loop_batch_size;
                }
                lud.foreach_remainder(
                    [&]()
                    {
                        *vec_out = core::operators::process<T>(view.operation, *vec0, *vec1);
                        ++vec0;
                        ++vec1;
                        ++vec_out;
                    });
                return vi->vds().data()[0];
            }

            T operator()(const vec_binop_vecval_hot_view& view) const
            {
                node_variant_adapter::value(node_variant_adapter::branch(view.node, 0));
                const T scalar =
                    node_variant_adapter::value(node_variant_adapter::branch(view.node, 1));
                auto* vi = view.node->as_vector_iface();
                auto* vi0 = node_variant_adapter::branch(view.node, 0)->as_vector_iface();
                const T* vec0 = vi0->vds().data();
                T* vec_out = vi->vds().data();
                const std::size_t sz = vi0->size();
                core::operators::loop_unroll lud(sz);
                const T* upper_bound = vec0 + lud.upper_bound;
                while (vec0 < upper_bound)
                {
                    lud.foreach_batch(
                        [&](unsigned int i)
                        {
                            vec_out[i] =
                                core::operators::process<T>(view.operation, vec0[i], scalar);
                        });
                    vec0 += lud.loop_batch_size;
                    vec_out += lud.loop_batch_size;
                }
                lud.foreach_remainder(
                    [&]()
                    {
                        *vec_out = core::operators::process<T>(view.operation, *vec0, scalar);
                        ++vec0;
                        ++vec_out;
                    });
                return vi->vds().data()[0];
            }

            T operator()(const vec_binop_valvec_hot_view& view) const
            {
                const T scalar =
                    node_variant_adapter::value(node_variant_adapter::branch(view.node, 0));
                node_variant_adapter::value(node_variant_adapter::branch(view.node, 1));
                auto* vi = view.node->as_vector_iface();
                auto* vi1 = node_variant_adapter::branch(view.node, 1)->as_vector_iface();
                const T* vec1 = vi1->vds().data();
                T* vec_out = vi->vds().data();
                const std::size_t sz = vi1->size();
                core::operators::loop_unroll lud(sz);
                const T* upper_bound = vec1 + lud.upper_bound;
                while (vec1 < upper_bound)
                {
                    lud.foreach_batch(
                        [&](unsigned int i)
                        {
                            vec_out[i] =
                                core::operators::process<T>(view.operation, scalar, vec1[i]);
                        });
                    vec1 += lud.loop_batch_size;
                    vec_out += lud.loop_batch_size;
                }
                lud.foreach_remainder(
                    [&]()
                    {
                        *vec_out = core::operators::process<T>(view.operation, scalar, *vec1);
                        ++vec1;
                        ++vec_out;
                    });
                return vi->vds().data()[0];
            }

            T operator()(const unary_vec_hot_view& view) const
            {
                node_variant_adapter::value(node_variant_adapter::branch(view.node, 0));
                auto* vi = view.node->as_vector_iface();
                auto* vi0 = node_variant_adapter::branch(view.node, 0)->as_vector_iface();
                const T* vec0 = vi0->vds().data();
                T* vec_out = vi->vds().data();
                const std::size_t sz = vi0->size();
                core::operators::loop_unroll lud(sz);
                const T* upper_bound = vec0 + lud.upper_bound;
                while (vec0 < upper_bound)
                {
                    lud.foreach_batch(
                        [&](unsigned int i)
                        { vec_out[i] = core::operators::process<T>(view.operation, vec0[i]); });
                    vec0 += lud.loop_batch_size;
                    vec_out += lud.loop_batch_size;
                }
                lud.foreach_remainder(
                    [&]()
                    {
                        *vec_out = core::operators::process<T>(view.operation, *vec0);
                        ++vec0;
                        ++vec_out;
                    });
                return vi->vds().data()[0];
            }

            T operator()(const vecfunc_hot_view& view) const
            {
                node_variant_adapter::value(view.fn->vec_branch());
                return view.fn->proc_fn()(view.fn->ivec());
            }

            T operator()(const fallback_view& view) const
            {
                return view.node->value();
            }
        };

        return std::visit(visitor{}, classify_hot(node));
    }

    static inline std::size_t node_depth(expression_ptr node)
    {
        return node ? node->node_depth() : std::size_t(0);
    }

    static inline void release_branch(expression_ptr node)
    {
        if (node)
        {
            node->release_branch();
        }
    }

    static inline t0ot1ot2_base_node_t* t0ot1ot2_base(expression_ptr node)
    {
        return node ? node->as_T0oT1oT2_base() : nullptr;
    }

    static inline t0ot1ot2ot3_base_node_t* t0ot1ot2ot3_base(expression_ptr node)
    {
        return node ? node->as_T0oT1oT2oT3_base() : nullptr;
    }
};
}  // namespace math_expr::details

#endif
