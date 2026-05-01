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

#define math_expr_define_unary_op(OpName)                        \
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

math_expr_define_unary_op(abs);
math_expr_define_unary_op(acos);
math_expr_define_unary_op(acosh);
math_expr_define_unary_op(asin);
math_expr_define_unary_op(asinh);
math_expr_define_unary_op(atan);
math_expr_define_unary_op(atanh);
math_expr_define_unary_op(ceil);
math_expr_define_unary_op(cos);
math_expr_define_unary_op(cosh);
math_expr_define_unary_op(cot);
math_expr_define_unary_op(csc);
math_expr_define_unary_op(d2g);
math_expr_define_unary_op(d2r);
math_expr_define_unary_op(erf);
math_expr_define_unary_op(erfc);
math_expr_define_unary_op(exp);
math_expr_define_unary_op(expm1);
math_expr_define_unary_op(floor);
math_expr_define_unary_op(frac);
math_expr_define_unary_op(g2d);
math_expr_define_unary_op(log);
math_expr_define_unary_op(log10);
math_expr_define_unary_op(log2);
math_expr_define_unary_op(log1p);
math_expr_define_unary_op(ncdf);
math_expr_define_unary_op(neg);
math_expr_define_unary_op(notl);
math_expr_define_unary_op(pos);
math_expr_define_unary_op(r2d);
math_expr_define_unary_op(round);
math_expr_define_unary_op(sec);
math_expr_define_unary_op(sgn);
math_expr_define_unary_op(sin);
math_expr_define_unary_op(sinc);
math_expr_define_unary_op(sinh);
math_expr_define_unary_op(sqrt);
math_expr_define_unary_op(tan);
math_expr_define_unary_op(tanh);
math_expr_define_unary_op(trunc);
#undef math_expr_define_unary_op

}  // namespace math_expr::details

#endif
