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

#ifndef MATH_EXPR_IFUNCTION_HPP
#define MATH_EXPR_IFUNCTION_HPP

#include "math_expr/core/std_includes.hpp"
#include "math_expr/core/macros.hpp"
#include "math_expr/function_traits.hpp"

namespace math_expr
{
template <typename T>
class ifunction : public function_traits
{
   public:
    explicit ifunction(const std::size_t& pc) : param_count(pc) {}

    virtual ~ifunction() {}

#define EMPTY_METHOD_BODY(N)                                                                     \
    {                                                                                            \
        MATH_EXPR_DEBUG(("ifunction::operator() - Operator(" #N ") has not been overridden\n")); \
        return std::numeric_limits<T>::quiet_NaN();                                              \
    }

    inline virtual T operator()() EMPTY_METHOD_BODY(0)

        inline virtual T operator()(const T&) EMPTY_METHOD_BODY(1)

            inline virtual T operator()(const T&, const T&) EMPTY_METHOD_BODY(2)

                inline virtual T operator()(const T&, const T&, const T&) EMPTY_METHOD_BODY(3)

                    inline virtual T operator()(const T&, const T&, const T&,
                                                const T&) EMPTY_METHOD_BODY(4)

                        inline virtual T operator()(const T&, const T&, const T&, const T&,
                                                    const T&) EMPTY_METHOD_BODY(5)

                            inline virtual T operator()(const T&, const T&, const T&, const T&,
                                                        const T&, const T&) EMPTY_METHOD_BODY(6)

                                inline virtual T operator()(const T&, const T&, const T&, const T&,
                                                            const T&, const T&,
                                                            const T&) EMPTY_METHOD_BODY(7)

                                    inline virtual T operator()(const T&, const T&, const T&,
                                                                const T&, const T&, const T&,
                                                                const T&,
                                                                const T&) EMPTY_METHOD_BODY(8)

                                        inline virtual T operator()(const T&, const T&, const T&,
                                                                    const T&, const T&, const T&,
                                                                    const T&, const T&,
                                                                    const T&) EMPTY_METHOD_BODY(9)

                                            inline virtual T operator()(
                                                const T&, const T&, const T&, const T&, const T&,
                                                const T&, const T&, const T&, const T&,
                                                const T&) EMPTY_METHOD_BODY(10)

                                                inline virtual T operator()(
                                                    const T&, const T&, const T&, const T&,
                                                    const T&, const T&, const T&, const T&,
                                                    const T&, const T&,
                                                    const T&) EMPTY_METHOD_BODY(11)

                                                    inline virtual T operator()(
                                                        const T&, const T&, const T&, const T&,
                                                        const T&, const T&, const T&, const T&,
                                                        const T&, const T&, const T&,
                                                        const T&) EMPTY_METHOD_BODY(12)

                                                        inline virtual T operator()(
                                                            const T&, const T&, const T&, const T&,
                                                            const T&, const T&, const T&, const T&,
                                                            const T&, const T&, const T&, const T&,
                                                            const T&) EMPTY_METHOD_BODY(13)

                                                            inline virtual T operator()(
                                                                const T&, const T&, const T&,
                                                                const T&, const T&, const T&,
                                                                const T&, const T&, const T&,
                                                                const T&, const T&, const T&,
                                                                const T&,
                                                                const T&) EMPTY_METHOD_BODY(14)

                                                                inline virtual T operator()(
                                                                    const T&, const T&, const T&,
                                                                    const T&, const T&, const T&,
                                                                    const T&, const T&, const T&,
                                                                    const T&, const T&, const T&,
                                                                    const T&, const T&,
                                                                    const T&) EMPTY_METHOD_BODY(15)

                                                                    inline virtual T operator()(
                                                                        const T&, const T&,
                                                                        const T&, const T&,
                                                                        const T&, const T&,
                                                                        const T&, const T&,
                                                                        const T&, const T&,
                                                                        const T&, const T&,
                                                                        const T&, const T&,
                                                                        const T&,
                                                                        const T&) EMPTY_METHOD_BODY(16)

                                                                        inline virtual T operator()(
                                                                            const T&, const T&,
                                                                            const T&, const T&,
                                                                            const T&, const T&,
                                                                            const T&, const T&,
                                                                            const T&, const T&,
                                                                            const T&, const T&,
                                                                            const T&, const T&,
                                                                            const T&, const T&,
                                                                            const T&) EMPTY_METHOD_BODY(17)

                                                                            inline virtual T
                                                                            operator()(
                                                                                const T&, const T&,
                                                                                const T&, const T&,
                                                                                const T&, const T&,
                                                                                const T&, const T&,
                                                                                const T&, const T&,
                                                                                const T&, const T&,
                                                                                const T&, const T&,
                                                                                const T&, const T&,
                                                                                const T&,
                                                                                const T&) EMPTY_METHOD_BODY(18)

                                                                                inline virtual T
                                                                                operator()(const T&,
                                                                                           const T&,
                                                                                           const T&,
                                                                                           const T&,
                                                                                           const T&,
                                                                                           const T&,
                                                                                           const T&,
                                                                                           const T&,
                                                                                           const T&,
                                                                                           const T&,
                                                                                           const T&,
                                                                                           const T&,
                                                                                           const T&,
                                                                                           const T&,
                                                                                           const T&,
                                                                                           const T&,
                                                                                           const T&,
                                                                                           const T&,
                                                                                           const T&)
                                                                                    EMPTY_METHOD_BODY(
                                                                                        19)

                                                                                        inline virtual T
                                                                                        operator()(
                                                                                            const T&,
                                                                                            const T&,
                                                                                            const T&,
                                                                                            const T&,
                                                                                            const T&,
                                                                                            const T&,
                                                                                            const T&,
                                                                                            const T&,
                                                                                            const T&,
                                                                                            const T&,
                                                                                            const T&,
                                                                                            const T&,
                                                                                            const T&,
                                                                                            const T&,
                                                                                            const T&,
                                                                                            const T&,
                                                                                            const T&,
                                                                                            const T&,
                                                                                            const T&,
                                                                                            const T&)
                                                                                            EMPTY_METHOD_BODY(
                                                                                                20)

#undef EMPTY_METHOD_BODY

                                                                                                std::
                                                                                                    size_t
        param_count;
};

}  // namespace math_expr

#endif
