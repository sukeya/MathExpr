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

#ifndef MATH_EXPR_FUNCTION_REGISTRY_HPP
#define MATH_EXPR_FUNCTION_REGISTRY_HPP

#include "math_expr/core/std_includes.hpp"
#include "math_expr/ifunction.hpp"
#include "math_expr/igeneric_function.hpp"
#include "math_expr/ivararg_function.hpp"

namespace math_expr
{
template <typename T, typename FunctionStore, typename VarargFunctionStore,
          typename GenericFunctionStore>
class function_registry
{
   public:
    using function_t = ifunction<T>;
    using vararg_function_t = ivararg_function<T>;
    using generic_function_t = igeneric_function<T>;
    using function_ptr = function_t*;
    using vararg_function_ptr = vararg_function_t*;
    using generic_function_ptr = generic_function_t*;

    template <typename... Args>
    struct freefunc final : public math_expr::ifunction<T>
    {
        using math_expr::ifunction<T>::operator();
        using functor_t = T (*)(Args...);

        explicit freefunc(functor_t ff) : math_expr::ifunction<T>(sizeof...(Args)), f(ff) {}

        inline T operator()(const Args&... args) override
        {
            return f(args...);
        }

        functor_t f;
    };

    function_registry(FunctionStore& function_store, VarargFunctionStore& vararg_function_store,
                      GenericFunctionStore& generic_function_store,
                      GenericFunctionStore& string_function_store,
                      GenericFunctionStore& overload_function_store,
                      std::vector<std::unique_ptr<ifunction<T>>>& free_function_list)
        : function_store(function_store),
          vararg_function_store(vararg_function_store),
          generic_function_store(generic_function_store),
          string_function_store(string_function_store),
          overload_function_store(overload_function_store),
          free_function_list_(free_function_list)
    {
    }

    inline void clear_functions()
    {
        function_store.clear();
        vararg_function_store.clear();
        generic_function_store.clear();
        string_function_store.clear();
        overload_function_store.clear();
        free_function_list_.clear();
    }

    inline std::size_t function_count() const
    {
        return function_store.size;
    }

    inline function_ptr get_function(const std::string& function_name) const
    {
        return function_store.get(function_name);
    }

    inline vararg_function_ptr get_vararg_function(const std::string& function_name) const
    {
        return vararg_function_store.get(function_name);
    }

    inline generic_function_ptr get_generic_function(const std::string& function_name) const
    {
        return generic_function_store.get(function_name);
    }

    inline generic_function_ptr get_string_function(const std::string& function_name) const
    {
        return string_function_store.get(function_name);
    }

    inline generic_function_ptr get_overload_function(const std::string& function_name) const
    {
        return overload_function_store.get(function_name);
    }

    inline bool add_function(const std::string& function_name, function_t& function)
    {
        return function_store.add(function_name, function);
    }

    inline bool add_function(const std::string& function_name, vararg_function_t& function)
    {
        return vararg_function_store.add(function_name, function);
    }

    inline bool add_function(const std::string& function_name, generic_function_t& function)
    {
        switch (function.rtrn_type)
        {
            case generic_function_t::return_type::e_rtrn_scalar:
                return (std::string::npos ==
                        function.parameter_sequence.find_first_not_of("STVZ*?|"))
                           ? generic_function_store.add(function_name, function)
                           : false;

            case generic_function_t::return_type::e_rtrn_string:
                return (std::string::npos ==
                        function.parameter_sequence.find_first_not_of("STVZ*?|"))
                           ? string_function_store.add(function_name, function)
                           : false;

            case generic_function_t::return_type::e_rtrn_overload:
                return (std::string::npos ==
                        function.parameter_sequence.find_first_not_of("STVZ*?|:"))
                           ? overload_function_store.add(function_name, function)
                           : false;
        }

        return false;
    }

    template <typename... Args>
    inline bool add_function(const std::string& function_name, T (*function)(Args...))
    {
        free_function_list_.push_back(std::make_unique<freefunc<Args...>>(function));
        return add_function(function_name, *free_function_list_.back());
    }

    inline bool remove_function(const std::string& function_name)
    {
        return function_store.remove(function_name);
    }

    inline bool remove_vararg_function(const std::string& function_name)
    {
        return vararg_function_store.remove(function_name);
    }

    template <typename Allocator, template <typename, typename> class Sequence>
    inline std::size_t get_function_list(Sequence<std::string, Allocator>& function_list) const
    {
        std::vector<std::string> function_names;
        std::size_t count = 0;

        count += function_store.get_list(function_names);
        count += vararg_function_store.get_list(function_names);
        count += generic_function_store.get_list(function_names);
        count += string_function_store.get_list(function_names);
        count += overload_function_store.get_list(function_names);

        std::set<std::string> function_set;

        for (std::size_t i = 0; i < function_names.size(); ++i)
        {
            function_set.insert(function_names[i]);
        }

        std::copy(function_set.begin(), function_set.end(), std::back_inserter(function_list));

        return count;
    }

    inline std::vector<std::string> get_function_list() const
    {
        std::vector<std::string> result;
        get_function_list(result);
        return result;
    }

    inline bool symbol_exists(const std::string& symbol_name) const
    {
        return function_store.symbol_exists(symbol_name) ||
               vararg_function_store.symbol_exists(symbol_name) ||
               generic_function_store.symbol_exists(symbol_name) ||
               string_function_store.symbol_exists(symbol_name) ||
               overload_function_store.symbol_exists(symbol_name);
    }

    inline bool is_function(const std::string& function_name) const
    {
        return function_store.symbol_exists(function_name);
    }

    inline bool is_vararg_function(const std::string& function_name) const
    {
        return vararg_function_store.symbol_exists(function_name);
    }

    inline void load_from(const function_registry& registry)
    {
        {
            std::vector<std::string> name_list;
            registry.function_store.get_list(name_list);

            for (std::size_t i = 0; i < name_list.size(); ++i)
            {
                math_expr::ifunction<T>& ifunc = *registry.get_function(name_list[i]);
                add_function(name_list[i], ifunc);
            }
        }

        {
            std::vector<std::string> name_list;
            registry.vararg_function_store.get_list(name_list);

            for (std::size_t i = 0; i < name_list.size(); ++i)
            {
                math_expr::ivararg_function<T>& ivafunc =
                    *registry.get_vararg_function(name_list[i]);
                add_function(name_list[i], ivafunc);
            }
        }

        {
            std::vector<std::string> name_list;
            registry.generic_function_store.get_list(name_list);

            for (std::size_t i = 0; i < name_list.size(); ++i)
            {
                math_expr::igeneric_function<T>& ifunc =
                    *registry.get_generic_function(name_list[i]);
                add_function(name_list[i], ifunc);
            }
        }

        {
            std::vector<std::string> name_list;
            registry.string_function_store.get_list(name_list);

            for (std::size_t i = 0; i < name_list.size(); ++i)
            {
                math_expr::igeneric_function<T>& ifunc =
                    *registry.get_string_function(name_list[i]);
                add_function(name_list[i], ifunc);
            }
        }

        {
            std::vector<std::string> name_list;
            registry.overload_function_store.get_list(name_list);

            for (std::size_t i = 0; i < name_list.size(); ++i)
            {
                math_expr::igeneric_function<T>& ifunc =
                    *registry.get_overload_function(name_list[i]);
                add_function(name_list[i], ifunc);
            }
        }
    }

    FunctionStore& function_store;
    VarargFunctionStore& vararg_function_store;
    GenericFunctionStore& generic_function_store;
    GenericFunctionStore& string_function_store;
    GenericFunctionStore& overload_function_store;
    std::vector<std::unique_ptr<ifunction<T>>>& free_function_list_;
};
}  // namespace math_expr

#endif
