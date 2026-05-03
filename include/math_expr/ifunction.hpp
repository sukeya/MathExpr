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

    inline virtual T operator()()
    {
        return ifunction_unimplemented<0>();
    }

    inline virtual T operator()(const T&)
    {
        return ifunction_unimplemented<1>();
    }

    inline virtual T operator()(const T&, const T&)
    {
        return ifunction_unimplemented<2>();
    }

    inline virtual T operator()(const T&, const T&, const T&)
    {
        return ifunction_unimplemented<3>();
    }

    inline virtual T operator()(const T&, const T&, const T&, const T&)
    {
        return ifunction_unimplemented<4>();
    }

    inline virtual T operator()(const T&, const T&, const T&, const T&, const T&)
    {
        return ifunction_unimplemented<5>();
    }

    inline virtual T operator()(const T&, const T&, const T&, const T&, const T&, const T&)
    {
        return ifunction_unimplemented<6>();
    }

    inline virtual T operator()(const T&, const T&, const T&, const T&, const T&, const T&,
                                const T&)
    {
        return ifunction_unimplemented<7>();
    }

    inline virtual T operator()(const T&, const T&, const T&, const T&, const T&, const T&,
                                const T&, const T&)
    {
        return ifunction_unimplemented<8>();
    }

    inline virtual T operator()(const T&, const T&, const T&, const T&, const T&, const T&,
                                const T&, const T&, const T&)
    {
        return ifunction_unimplemented<9>();
    }

    inline virtual T operator()(const T&, const T&, const T&, const T&, const T&, const T&,
                                const T&, const T&, const T&, const T&)
    {
        return ifunction_unimplemented<10>();
    }

    inline virtual T operator()(const T&, const T&, const T&, const T&, const T&, const T&,
                                const T&, const T&, const T&, const T&, const T&)
    {
        return ifunction_unimplemented<11>();
    }

    inline virtual T operator()(const T&, const T&, const T&, const T&, const T&, const T&,
                                const T&, const T&, const T&, const T&, const T&, const T&)
    {
        return ifunction_unimplemented<12>();
    }

    inline virtual T operator()(const T&, const T&, const T&, const T&, const T&, const T&,
                                const T&, const T&, const T&, const T&, const T&, const T&,
                                const T&)
    {
        return ifunction_unimplemented<13>();
    }

    inline virtual T operator()(const T&, const T&, const T&, const T&, const T&, const T&,
                                const T&, const T&, const T&, const T&, const T&, const T&,
                                const T&, const T&)
    {
        return ifunction_unimplemented<14>();
    }

    inline virtual T operator()(const T&, const T&, const T&, const T&, const T&, const T&,
                                const T&, const T&, const T&, const T&, const T&, const T&,
                                const T&, const T&, const T&)
    {
        return ifunction_unimplemented<15>();
    }

    inline virtual T operator()(const T&, const T&, const T&, const T&, const T&, const T&,
                                const T&, const T&, const T&, const T&, const T&, const T&,
                                const T&, const T&, const T&, const T&)
    {
        return ifunction_unimplemented<16>();
    }

    inline virtual T operator()(const T&, const T&, const T&, const T&, const T&, const T&,
                                const T&, const T&, const T&, const T&, const T&, const T&,
                                const T&, const T&, const T&, const T&, const T&)
    {
        return ifunction_unimplemented<17>();
    }

    inline virtual T operator()(const T&, const T&, const T&, const T&, const T&, const T&,
                                const T&, const T&, const T&, const T&, const T&, const T&,
                                const T&, const T&, const T&, const T&, const T&, const T&)
    {
        return ifunction_unimplemented<18>();
    }

    inline virtual T operator()(const T&, const T&, const T&, const T&, const T&, const T&,
                                const T&, const T&, const T&, const T&, const T&, const T&,
                                const T&, const T&, const T&, const T&, const T&, const T&,
                                const T&)
    {
        return ifunction_unimplemented<19>();
    }

    inline virtual T operator()(const T&, const T&, const T&, const T&, const T&, const T&,
                                const T&, const T&, const T&, const T&, const T&, const T&,
                                const T&, const T&, const T&, const T&, const T&, const T&,
                                const T&, const T&)
    {
        return ifunction_unimplemented<20>();
    }

    std::size_t param_count;

   private:
    template <int N>
    inline T ifunction_unimplemented() noexcept
    {
        core::debug_print("ifunction::operator() - Operator(%d) has not been overridden\n", N);
        return std::numeric_limits<T>::quiet_NaN();
    }
};

}  // namespace math_expr

#endif
