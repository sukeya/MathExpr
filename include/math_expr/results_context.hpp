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

#ifndef MATH_EXPR_RESULTS_CONTEXT_HPP
#define MATH_EXPR_RESULTS_CONTEXT_HPP

#include "math_expr/core/std_includes.hpp"
#include "math_expr/type_store.hpp"

namespace math_expr
{
#ifndef MATH_EXPR_DISABLE_RETURN_STATEMENT
namespace details
{
template <typename T>
class return_node;
template <typename T>
class return_envelope_node;
}  // namespace details
#endif

template <typename T>
class results_context
{
   public:
    using type_store_t = type_store<T>;
    using scalar_t = typename type_store_t::scalar_view;
    using vector_t = typename type_store_t::vector_view;
    using string_t = typename type_store_t::string_view;

    results_context() : results_available_(false) {}

    inline std::size_t count() const
    {
        if (results_available_)
            return parameter_list_.size();
        else
            return 0;
    }

    inline type_store_t& operator[](const std::size_t& index)
    {
        return parameter_list_[index];
    }

    inline const type_store_t& operator[](const std::size_t& index) const
    {
        return parameter_list_[index];
    }

    inline bool get_scalar(const std::size_t& index, T& out) const
    {
        if ((index < parameter_list_.size()) &&
            (parameter_list_[index].type == type_store_t::store_type::e_scalar))
        {
            const scalar_t scalar(parameter_list_[index]);
            out = scalar();
            return true;
        }

        return false;
    }

    template <typename OutputIterator>
    inline bool get_vector(const std::size_t& index, OutputIterator out_itr) const
    {
        if ((index < parameter_list_.size()) &&
            (parameter_list_[index].type == type_store_t::store_type::e_vector))
        {
            const vector_t vector(parameter_list_[index]);
            for (std::size_t i = 0; i < vector.size(); ++i)
            {
                *(out_itr++) = vector[i];
            }

            return true;
        }

        return false;
    }

    inline bool get_vector(const std::size_t& index, std::vector<T>& out) const
    {
        return get_vector(index, std::back_inserter(out));
    }

    inline bool get_string(const std::size_t& index, std::string& out) const
    {
        if ((index < parameter_list_.size()) &&
            (parameter_list_[index].type == type_store_t::store_type::e_string))
        {
            const string_t str(parameter_list_[index]);
            out.assign(str.begin(), str.size());
            return true;
        }

        return false;
    }

   private:
    inline void clear()
    {
        results_available_ = false;
    }

    using ts_list_t = std::vector<type_store_t>;
    using parameter_list_t = typename type_store_t::parameter_list;

    inline void assign(const parameter_list_t& pl)
    {
        parameter_list_ = pl.parameter_list_;
        results_available_ = true;
    }

    bool results_available_;
    ts_list_t parameter_list_;

#ifndef MATH_EXPR_DISABLE_RETURN_STATEMENT
    friend class details::return_node<T>;
    friend class details::return_envelope_node<T>;
#endif
};

}  // namespace math_expr

#endif
