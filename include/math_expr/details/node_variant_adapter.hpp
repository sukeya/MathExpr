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

    using variant_type =
        std::variant<std::monostate, null_view, literal_view, variable_view, string_view,
                     unary_view, binary_view, function_view, vararg_view, multi_vararg_view,
                     assert_view, sf3ext_view, sf4ext_view, other_view>;

    using hot_variant_type =
        std::variant<std::monostate, literal_view, variable_view, unary_hot_view, binary_hot_view,
                     trinary_hot_view, conditional_hot_view, uv_hot_view, scalar_pow_hot_view,
                     branch_pow_hot_view, unary_branch_hot_view, vov_hot_view, cov_hot_view,
                     voc_hot_view, vob_hot_view, bov_hot_view, cob_hot_view, boc_hot_view,
                     uvouv_hot_view, t0ot1ot2_hot_view, t0ot1ot2ot3_hot_view, scand_hot_view,
                     scor_hot_view, nulleq_hot_view, fallback_view>;

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
                if ((nullptr != node->branch(0)) && (nullptr != node->branch(1)) &&
                    (typeid(*node) == typeid(binary_node_t)))
                {
                    return binary_hot_view{node, static_cast<binary_node_t*>(node)};
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
