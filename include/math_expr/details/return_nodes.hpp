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

#ifndef MATH_EXPR_DETAILS_RETURN_NODES_HPP
#define MATH_EXPR_DETAILS_RETURN_NODES_HPP

#include "math_expr/results_context.hpp"
#include "math_expr/details/function_nodes.hpp"

namespace math_expr::details
{
class return_exception
{
};

template <typename T>
class null_igenfunc
{
   public:
    virtual ~null_igenfunc() {}

    using generic_type = type_store<T>;
    using parameter_list_t = typename generic_type::parameter_list;

    inline virtual T operator()(parameter_list_t)
    {
        return std::numeric_limits<T>::quiet_NaN();
    }
};

#ifndef MATH_EXPR_DISABLE_RETURN_STATEMENT
template <typename T>
class return_node final : public generic_function_node<T, null_igenfunc<T>>
{
   public:
    using results_context_t = results_context<T>;
    using igeneric_function_t = null_igenfunc<T>;
    using igeneric_function_ptr = igeneric_function_t*;
    using gen_function_t = generic_function_node<T, igeneric_function_t>;

    return_node(const std::vector<typename gen_function_t::expression_ptr>& arg_list,
                results_context_t& rc)
        : gen_function_t(arg_list), results_context_(&rc)
    {
        assert(valid());
    }

    inline T value() const override
    {
        if (gen_function_t::populate_value_list())
        {
            prepare_typestore_list();

            using parameter_list_t = typename type_store<T>::parameter_list;

            results_context_->assign(parameter_list_t(gen_function_t::typestore_list_));

            throw return_exception();
        }

        return std::numeric_limits<T>::quiet_NaN();
    }

    inline typename expression_node<T>::node_type type() const override
    {
        return expression_node<T>::node_type::e_return;
    }

    inline bool valid() const override
    {
        return results_context_;
    }

   private:
    void prepare_typestore_list() const
    {
        for (std::size_t i = 0; i < gen_function_t::typestore_list_.size(); ++i)
        {
            typename gen_function_t::type_store_t& ts = gen_function_t::typestore_list_[i];

            if (ts.ivec)
            {
                ts.size = ts.ivec->size();
            }
        }
    }

    results_context_t* results_context_;
};

template <typename T>
class return_envelope_node final : public expression_node<T>
{
   public:
    using expression_ptr = expression_node<T>*;
    using results_context_t = results_context<T>;
    using branch_t = std::pair<expression_ptr, bool>;

    return_envelope_node(expression_ptr body, results_context_t& rc)
        : results_context_(&rc), return_invoked_(false)
    {
        construct_branch_pair(body_, body);
        assert(valid());
    }

    inline T value() const override
    {
        try
        {
            return_invoked_ = false;
            results_context_->clear();

            return body_.first->value();
        }
        catch (const return_exception&)
        {
            return_invoked_ = true;

            return std::numeric_limits<T>::quiet_NaN();
        }
    }

    inline typename expression_node<T>::node_type type() const override
    {
        return expression_node<T>::node_type::e_retenv;
    }

    inline bool valid() const override
    {
        return results_context_ && body_.first;
    }

    inline bool* retinvk_ptr()
    {
        return &return_invoked_;
    }

    void reset_state() const
    {
        return_invoked_ = false;
        results_context_->clear();
    }
    expression_node<T>* body_node() const
    {
        return body_.first;
    }

    void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) override
    {
        expression_node<T>::ndb_t::collect(body_, node_delete_list);
    }

    std::size_t node_depth() const override
    {
        return expression_node<T>::ndb_t::compute_node_depth(body_);
    }

   private:
    results_context_t* results_context_;
    mutable bool return_invoked_;
    branch_t body_;
};
#endif

#define MATH_EXPR_DEFINE_UNARY_OP(OpName)                        \
    template <typename T>                                        \
    struct OpName##_op                                           \
    {                                                            \
        using Type = typename core::numeric::functor_t<T>::Type; \
        using node_t = typename expression_node<T>::node_type;   \
                                                                 \
        static inline T process(Type v)                          \
        {                                                        \
            return core::numeric::OpName(v);                     \
        }                                                        \
                                                                 \
        static inline node_t type()                              \
        {                                                        \
            return expression_node<T>::node_type::e_##OpName;    \
        }                                                        \
                                                                 \
        static inline core::operators::operator_type operation() \
        {                                                        \
            return core::operators::operator_type::OpName;       \
        }                                                        \
    }

MATH_EXPR_DEFINE_UNARY_OP(abs);
MATH_EXPR_DEFINE_UNARY_OP(acos);
MATH_EXPR_DEFINE_UNARY_OP(acosh);
MATH_EXPR_DEFINE_UNARY_OP(asin);
MATH_EXPR_DEFINE_UNARY_OP(asinh);
MATH_EXPR_DEFINE_UNARY_OP(atan);
MATH_EXPR_DEFINE_UNARY_OP(atanh);
MATH_EXPR_DEFINE_UNARY_OP(ceil);
MATH_EXPR_DEFINE_UNARY_OP(cos);
MATH_EXPR_DEFINE_UNARY_OP(cosh);
MATH_EXPR_DEFINE_UNARY_OP(cot);
MATH_EXPR_DEFINE_UNARY_OP(csc);
MATH_EXPR_DEFINE_UNARY_OP(d2g);
MATH_EXPR_DEFINE_UNARY_OP(d2r);
MATH_EXPR_DEFINE_UNARY_OP(erf);
MATH_EXPR_DEFINE_UNARY_OP(erfc);
MATH_EXPR_DEFINE_UNARY_OP(exp);
MATH_EXPR_DEFINE_UNARY_OP(expm1);
MATH_EXPR_DEFINE_UNARY_OP(floor);
MATH_EXPR_DEFINE_UNARY_OP(frac);
MATH_EXPR_DEFINE_UNARY_OP(g2d);
MATH_EXPR_DEFINE_UNARY_OP(log);
MATH_EXPR_DEFINE_UNARY_OP(log10);
MATH_EXPR_DEFINE_UNARY_OP(log2);
MATH_EXPR_DEFINE_UNARY_OP(log1p);
MATH_EXPR_DEFINE_UNARY_OP(ncdf);
MATH_EXPR_DEFINE_UNARY_OP(neg);
MATH_EXPR_DEFINE_UNARY_OP(notl);
MATH_EXPR_DEFINE_UNARY_OP(pos);
MATH_EXPR_DEFINE_UNARY_OP(r2d);
MATH_EXPR_DEFINE_UNARY_OP(round);
MATH_EXPR_DEFINE_UNARY_OP(sec);
MATH_EXPR_DEFINE_UNARY_OP(sgn);
MATH_EXPR_DEFINE_UNARY_OP(sin);
MATH_EXPR_DEFINE_UNARY_OP(sinc);
MATH_EXPR_DEFINE_UNARY_OP(sinh);
MATH_EXPR_DEFINE_UNARY_OP(sqrt);
MATH_EXPR_DEFINE_UNARY_OP(tan);
MATH_EXPR_DEFINE_UNARY_OP(tanh);
MATH_EXPR_DEFINE_UNARY_OP(trunc);
#undef MATH_EXPR_DEFINE_UNARY_OP

}  // namespace math_expr::details

#endif
