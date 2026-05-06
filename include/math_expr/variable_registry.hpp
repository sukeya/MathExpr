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

#ifndef MATH_EXPR_VARIABLE_REGISTRY_HPP
#define MATH_EXPR_VARIABLE_REGISTRY_HPP

#include "math_expr/core/std_includes.hpp"
#include "math_expr/details/string_nodes.hpp"
#include "math_expr/details/vector_nodes.hpp"
#include "math_expr/vector_view.hpp"

namespace math_expr
{
#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
template <typename T, typename VariableStore, typename VectorStore, typename StringStore>
#else
template <typename T, typename VariableStore, typename VectorStore>
#endif
class variable_registry
{
   public:
    using expression_ptr = details::expression_node<T>*;
    using variable_t = details::variable_node<T>;
    using vector_holder_t = details::vector_holder<T>;
    using variable_ptr = variable_t*;
    using vector_holder_ptr = vector_holder_t*;
#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
    using stringvar_t = details::string_nodes::stringvar_node<T>;
    using stringvar_ptr = stringvar_t*;
#endif

#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
    variable_registry(VariableStore& variable_store, VectorStore& vector_store,
                      StringStore& stringvar_store, std::list<T>& local_symbol_list,
                      std::list<std::string>& local_stringvar_list)
        : variable_store(variable_store),
          vector_store(vector_store),
          stringvar_store(stringvar_store),
          local_symbol_list_(local_symbol_list),
          local_stringvar_list_(local_stringvar_list)
    {
    }
#else
    variable_registry(VariableStore& variable_store, VectorStore& vector_store,
                      std::list<T>& local_symbol_list)
        : variable_store(variable_store),
          vector_store(vector_store),
          local_symbol_list_(local_symbol_list)
    {
    }
#endif

    inline void clear_variables(const bool delete_node = true)
    {
        variable_store.clear(delete_node);
    }

    inline void clear_vectors()
    {
        vector_store.clear();
    }

    inline void clear_strings()
    {
#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
        stringvar_store.clear();
        local_stringvar_list_.clear();
#endif
    }

    inline void clear_local_constants()
    {
        local_symbol_list_.clear();
    }

    inline std::size_t variable_count() const
    {
        return variable_store.size;
    }

#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
    inline std::size_t stringvar_count() const
    {
        return stringvar_store.size;
    }
#endif

    inline std::size_t vector_count() const
    {
        return vector_store.size;
    }

    inline variable_ptr get_variable(const std::string& variable_name) const
    {
        return variable_store.get(variable_name);
    }

    inline variable_ptr get_variable(const T& var_ref) const
    {
        return variable_store.get_from_varptr(&var_ref);
    }

#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
    inline stringvar_ptr get_stringvar(const std::string& string_name) const
    {
        return stringvar_store.get(string_name);
    }
#endif

    inline vector_holder_ptr get_vector(const std::string& vector_name) const
    {
        return vector_store.get(vector_name);
    }

    inline T& variable_ref(const std::string& symbol_name)
    {
        return variable_store.type_ref(symbol_name);
    }

#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
    inline std::string& stringvar_ref(const std::string& symbol_name)
    {
        return stringvar_store.type_ref(symbol_name);
    }
#endif

    inline bool is_constant_node(const std::string& symbol_name) const
    {
        return variable_store.is_constant(symbol_name);
    }

#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
    inline bool is_constant_string(const std::string& symbol_name) const
    {
        return stringvar_store.symbol_exists(symbol_name) &&
               stringvar_store.is_constant(symbol_name);
    }
#endif

    inline bool create_variable(const std::string& variable_name, const T& value = T(0))
    {
        local_symbol_list_.push_back(value);
        T& t = local_symbol_list_.back();
        return add_variable(variable_name, t);
    }

#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
    inline bool create_stringvar(const std::string& stringvar_name,
                                 const std::string& value = std::string(""))
    {
        local_stringvar_list_.push_back(value);
        std::string& s = local_stringvar_list_.back();
        return add_stringvar(stringvar_name, s);
    }
#endif

    inline bool add_variable(const std::string& variable_name, T& t, const bool is_constant = false)
    {
        return variable_store.add(variable_name, t, is_constant);
    }

    inline bool add_constant(const std::string& constant_name, const T& value)
    {
        local_symbol_list_.push_back(value);
        T& t = local_symbol_list_.back();
        return add_variable(constant_name, t, true);
    }

#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
    inline bool add_stringvar(const std::string& stringvar_name, std::string& s,
                              const bool is_constant = false)
    {
        return stringvar_store.add(stringvar_name, s, is_constant);
    }
#endif

    template <std::size_t N>
    inline bool add_vector(const std::string& vector_name, T (&v)[N])
    {
        return vector_store.add(vector_name, v);
    }

    inline bool add_vector(const std::string& vector_name, T* v, const std::size_t& v_size)
    {
        return vector_store.add(vector_name, v, v_size);
    }

    template <typename Allocator>
    inline bool add_vector(const std::string& vector_name, std::vector<T, Allocator>& v)
    {
        return vector_store.add(vector_name, v);
    }

    inline bool add_vector(const std::string& vector_name, math_expr::vector_view<T>& v)
    {
        return vector_store.add(vector_name, v);
    }

    inline bool remove_variable(const std::string& variable_name, const bool delete_node = true)
    {
        return variable_store.remove(variable_name, delete_node);
    }

#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
    inline bool remove_stringvar(const std::string& string_name)
    {
        return stringvar_store.remove(string_name);
    }
#endif

    inline bool remove_vector(const std::string& vector_name)
    {
        return vector_store.remove(vector_name);
    }

    template <typename Allocator, template <typename, typename> class Sequence>
    inline std::size_t get_variable_list(
        Sequence<std::pair<std::string, T>, Allocator>& vlist) const
    {
        return variable_store.get_list(vlist);
    }

    template <typename Allocator, template <typename, typename> class Sequence>
    inline std::size_t get_variable_list(Sequence<std::string, Allocator>& vlist) const
    {
        return variable_store.get_list(vlist);
    }

#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
    template <typename Allocator, template <typename, typename> class Sequence>
    inline std::size_t get_stringvar_list(
        Sequence<std::pair<std::string, std::string>, Allocator>& svlist) const
    {
        return stringvar_store.get_list(svlist);
    }

    template <typename Allocator, template <typename, typename> class Sequence>
    inline std::size_t get_stringvar_list(Sequence<std::string, Allocator>& svlist) const
    {
        return stringvar_store.get_list(svlist);
    }
#endif

    template <typename Allocator, template <typename, typename> class Sequence>
    inline std::size_t get_vector_list(Sequence<std::string, Allocator>& vec_list) const
    {
        return vector_store.get_list(vec_list);
    }

    inline bool symbol_exists(const std::string& symbol_name) const
    {
        if (variable_store.symbol_exists(symbol_name))
            return true;
#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
        else if (stringvar_store.symbol_exists(symbol_name))
            return true;
#endif
        else
            return vector_store.symbol_exists(symbol_name);
    }

    inline bool is_variable(const std::string& variable_name) const
    {
        return variable_store.symbol_exists(variable_name);
    }

#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
    inline bool is_stringvar(const std::string& stringvar_name) const
    {
        return stringvar_store.symbol_exists(stringvar_name);
    }

    inline bool is_conststr_stringvar(const std::string& symbol_name) const
    {
        return stringvar_store.symbol_exists(symbol_name) &&
               (stringvar_store.symbol_exists(symbol_name) ||
                stringvar_store.is_constant(symbol_name));
    }
#endif

    inline bool is_vector(const std::string& vector_name) const
    {
        return vector_store.symbol_exists(vector_name);
    }

    inline std::string get_variable_name(const expression_ptr& ptr) const
    {
        return variable_store.entity_name(ptr);
    }

    inline std::string get_vector_name(const vector_holder_ptr& ptr) const
    {
        return vector_store.entity_name(ptr);
    }

#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
    inline std::string get_stringvar_name(const expression_ptr& ptr) const
    {
        return stringvar_store.entity_name(ptr);
    }

    inline std::string get_conststr_stringvar_name(const expression_ptr& ptr) const
    {
        return stringvar_store.entity_name(ptr);
    }
#endif

    inline void load_variables_from(const variable_registry& registry)
    {
        std::vector<std::string> name_list;
        registry.variable_store.get_list(name_list);

        for (std::size_t i = 0; i < name_list.size(); ++i)
        {
            T& variable = registry.get_variable(name_list[i])->ref();
            add_variable(name_list[i], variable);
        }
    }

    inline void load_vectors_from(const variable_registry& registry)
    {
        std::vector<std::string> name_list;
        registry.vector_store.get_list(name_list);

        for (std::size_t i = 0; i < name_list.size(); ++i)
        {
            vector_holder_t& vecholder = *registry.get_vector(name_list[i]);
            add_vector(name_list[i], vecholder.data(), vecholder.size());
        }
    }

    VariableStore& variable_store;
    VectorStore& vector_store;
#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
    StringStore& stringvar_store;
#endif
    std::list<T>& local_symbol_list_;
#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
    std::list<std::string>& local_stringvar_list_;
#endif
};
}  // namespace math_expr

#endif
