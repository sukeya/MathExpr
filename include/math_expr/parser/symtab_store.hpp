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

#ifndef MATH_EXPR_PARSER_SYMTAB_STORE_HPP
#define MATH_EXPR_PARSER_SYMTAB_STORE_HPP

#include "math_expr/core/std_includes.hpp"
#include "math_expr/symbol_table.hpp"

namespace math_expr
{
template <typename T>
struct symtab_store
{
    using symbol_table_t = symbol_table<T>;
    using symbol_table_list_t = std::vector<symbol_table_t>;
    using expression_node_ptr = details::expression_node<T>*;
    using variable_ptr = details::variable_node<T>*;
    using vector_holder_t = details::vector_holder<T>;
    using vector_holder_ptr = vector_holder_t*;
    using function_ptr = ifunction<T>*;
    using vararg_function_ptr = ivararg_function<T>*;
    using generic_function_ptr = igeneric_function<T>*;

    struct variable_context
    {
        variable_context() : symbol_table(nullptr), variable(nullptr) {}

        const symbol_table_t* symbol_table;
        variable_ptr variable;
    };

    struct vector_context
    {
        vector_context() : symbol_table(nullptr), vector_holder(nullptr) {}

        const symbol_table_t* symbol_table;
        vector_holder_ptr vector_holder;
    };

    inline bool empty() const
    {
        return symtab_list_.empty();
    }

    inline void clear()
    {
        symtab_list_.clear();
    }

    inline bool valid() const
    {
        if (!empty())
        {
            for (std::size_t i = 0; i < symtab_list_.size(); ++i)
            {
                if (symtab_list_[i].valid())
                    return true;
            }
        }

        return false;
    }

    inline variable_context get_variable_context(const std::string& variable_name) const
    {
        variable_context result;

        for (std::size_t i = 0; i < symtab_list_.size(); ++i)
        {
            if (!symtab_list_[i].valid())
                continue;

            result.variable = symtab_list_[i].get_variable(variable_name);
            if (result.variable)
            {
                result.symbol_table = &symtab_list_[i];
                break;
            }
        }

        return result;
    }

    inline variable_ptr get_variable(const std::string& variable_name) const
    {
        variable_ptr result = nullptr;

        for (std::size_t i = 0; i < symtab_list_.size(); ++i)
        {
            if (!symtab_list_[i].valid())
                continue;

            result = symtab_list_[i].get_variable(variable_name);
            if (result)
                break;
        }

        return result;
    }

    inline variable_ptr get_variable(const T& var_ref) const
    {
        variable_ptr result = nullptr;

        for (std::size_t i = 0; i < symtab_list_.size(); ++i)
        {
            if (!symtab_list_[i].valid())
                continue;

            result = symtab_list_[i].get_variable(var_ref);
            if (result)
                break;
        }

        return result;
    }

    inline function_ptr get_function(const std::string& function_name) const
    {
        function_ptr result = nullptr;

        for (std::size_t i = 0; i < symtab_list_.size(); ++i)
        {
            if (!symtab_list_[i].valid())
                continue;

            result = symtab_list_[i].get_function(function_name);
            if (result)
                break;
        }

        return result;
    }

    inline vararg_function_ptr get_vararg_function(const std::string& function_name) const
    {
        vararg_function_ptr result = nullptr;

        for (std::size_t i = 0; i < symtab_list_.size(); ++i)
        {
            if (!symtab_list_[i].valid())
                continue;

            result = symtab_list_[i].get_vararg_function(function_name);
            if (result)
                break;
        }

        return result;
    }

    inline generic_function_ptr get_generic_function(const std::string& function_name) const
    {
        generic_function_ptr result = nullptr;

        for (std::size_t i = 0; i < symtab_list_.size(); ++i)
        {
            if (!symtab_list_[i].valid())
                continue;

            result = symtab_list_[i].get_generic_function(function_name);
            if (result)
                break;
        }

        return result;
    }

    inline generic_function_ptr get_string_function(const std::string& function_name) const
    {
        generic_function_ptr result = nullptr;

        for (std::size_t i = 0; i < symtab_list_.size(); ++i)
        {
            if (!symtab_list_[i].valid())
                continue;

            result = symtab_list_[i].get_string_function(function_name);
            if (result)
                break;
        }

        return result;
    }

    inline generic_function_ptr get_overload_function(const std::string& function_name) const
    {
        generic_function_ptr result = nullptr;

        for (std::size_t i = 0; i < symtab_list_.size(); ++i)
        {
            if (!symtab_list_[i].valid())
                continue;

            result = symtab_list_[i].get_overload_function(function_name);
            if (result)
                break;
        }

        return result;
    }

    inline vector_context get_vector_context(const std::string& vector_name) const
    {
        vector_context result;

        for (std::size_t i = 0; i < symtab_list_.size(); ++i)
        {
            if (!symtab_list_[i].valid())
                continue;

            result.vector_holder = symtab_list_[i].get_vector(vector_name);
            if (result.vector_holder)
            {
                result.symbol_table = &symtab_list_[i];
                break;
            }
        }

        return result;
    }

    inline vector_holder_ptr get_vector(const std::string& vector_name) const
    {
        vector_holder_ptr result = nullptr;

        for (std::size_t i = 0; i < symtab_list_.size(); ++i)
        {
            if (!symtab_list_[i].valid())
                continue;

            result = symtab_list_[i].get_vector(vector_name);
            if (result)
                break;
        }

        return result;
    }

    inline bool is_constant_node(const std::string& symbol_name) const
    {
        for (std::size_t i = 0; i < symtab_list_.size(); ++i)
        {
            if (!symtab_list_[i].valid())
                continue;

            if (symtab_list_[i].is_constant_node(symbol_name))
                return true;
        }

        return false;
    }

    inline bool symbol_exists(const std::string& symbol) const
    {
        for (std::size_t i = 0; i < symtab_list_.size(); ++i)
        {
            if (!symtab_list_[i].valid())
                continue;

            if (symtab_list_[i].symbol_exists(symbol))
                return true;
        }

        return false;
    }

    inline bool is_variable(const std::string& variable_name) const
    {
        for (std::size_t i = 0; i < symtab_list_.size(); ++i)
        {
            if (!symtab_list_[i].valid())
                continue;
            if (symtab_list_[i].is_variable(variable_name))
                return true;
        }

        return false;
    }

    inline bool is_function(const std::string& function_name) const
    {
        for (std::size_t i = 0; i < symtab_list_.size(); ++i)
        {
            if (!symtab_list_[i].valid())
                continue;
            if (symtab_list_[i].is_function(function_name))
                return true;
        }

        return false;
    }

    inline bool is_vararg_function(const std::string& function_name) const
    {
        for (std::size_t i = 0; i < symtab_list_.size(); ++i)
        {
            if (!symtab_list_[i].valid())
                continue;
            if (symtab_list_[i].is_vararg_function(function_name))
                return true;
        }

        return false;
    }

    inline bool is_vector(const std::string& vector_name) const
    {
        for (std::size_t i = 0; i < symtab_list_.size(); ++i)
        {
            if (!symtab_list_[i].valid())
                continue;
            if (symtab_list_[i].is_vector(vector_name))
                return true;
        }

        return false;
    }

    inline std::string get_variable_name(const expression_node_ptr& ptr) const
    {
        for (std::size_t i = 0; i < symtab_list_.size(); ++i)
        {
            if (!symtab_list_[i].valid())
                continue;

            std::string name = symtab_list_[i].get_variable_name(ptr);
            if (!name.empty())
                return name;
        }

        return std::string();
    }

    inline std::string get_vector_name(const vector_holder_ptr& ptr) const
    {
        for (std::size_t i = 0; i < symtab_list_.size(); ++i)
        {
            if (!symtab_list_[i].valid())
                continue;

            std::string name = symtab_list_[i].get_vector_name(ptr);
            if (!name.empty())
                return name;
        }

        return std::string();
    }

    inline symbol_table_t& get_symbol_table(const std::size_t& index = 0)
    {
        return symtab_list_[index];
    }

    symbol_table_list_t symtab_list_;
};
}  // namespace math_expr

#endif
