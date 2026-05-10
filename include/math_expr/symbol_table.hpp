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

#ifndef MATH_EXPR_SYMBOL_TABLE_HPP
#define MATH_EXPR_SYMBOL_TABLE_HPP

#include "math_expr/core/std_includes.hpp"
#include "math_expr/core/macros.hpp"
#include "math_expr/core/string_utils.hpp"
#include "math_expr/function_registry.hpp"
#include "math_expr/ifunction.hpp"
#include "math_expr/igeneric_function.hpp"
#include "math_expr/ivararg_function.hpp"
#include "math_expr/variable_registry.hpp"
#include "math_expr/vector_view.hpp"

namespace math_expr
{
template <typename T>
class symbol_table
{
   public:
    enum class symtab_mutability_type
    {
        e_unknown = 0,
        e_mutable = 1,
        e_immutable = 2
    };

    using ff00_functor = T (*)();
    using ff01_functor = T (*)(T);
    using ff02_functor = T (*)(T, T);
    using ff03_functor = T (*)(T, T, T);
    using ff04_functor = T (*)(T, T, T, T);
    using ff05_functor = T (*)(T, T, T, T, T);
    using ff06_functor = T (*)(T, T, T, T, T, T);
    using ff07_functor = T (*)(T, T, T, T, T, T, T);
    using ff08_functor = T (*)(T, T, T, T, T, T, T, T);
    using ff09_functor = T (*)(T, T, T, T, T, T, T, T, T);
    using ff10_functor = T (*)(T, T, T, T, T, T, T, T, T, T);
    using ff11_functor = T (*)(T, T, T, T, T, T, T, T, T, T, T);
    using ff12_functor = T (*)(T, T, T, T, T, T, T, T, T, T, T, T);
    using ff13_functor = T (*)(T, T, T, T, T, T, T, T, T, T, T, T, T);
    using ff14_functor = T (*)(T, T, T, T, T, T, T, T, T, T, T, T, T, T);
    using ff15_functor = T (*)(T, T, T, T, T, T, T, T, T, T, T, T, T, T, T);

   protected:
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

    template <typename Type, typename RawType, bool Owned = true>
    struct type_store
    {
        using expression_ptr = details::expression_node<T>*;
        using variable_node_t = typename details::variable_node<T>;
        using ifunction_t = ifunction<T>;
        using ivararg_function_t = ivararg_function<T>;
        using igeneric_function_t = igeneric_function<T>;
        using vector_t = details::vector_holder<T>;

        using type_t = Type;
        using type_ptr = type_t*;
        using stored_ptr_t = std::conditional_t<Owned, std::unique_ptr<type_t>, type_ptr>;
        using type_pair_t = std::pair<bool, stored_ptr_t>;
        using type_map_t = std::map<std::string, type_pair_t, core::ilesscompare>;
        using tm_itr_t = typename type_map_t::iterator;
        using tm_const_itr_t = typename type_map_t::const_iterator;

        static constexpr std::int32_t lut_size = 256;

        type_map_t map;
        std::size_t size;

        type_store() : size(0) {}

        static inline type_ptr get_raw(const stored_ptr_t& ptr) noexcept
        {
            if constexpr (Owned)
                return ptr.get();
            else
                return ptr;
        }

        inline bool symbol_exists(const std::string& symbol_name) const
        {
            if (symbol_name.empty())
                return false;
            else if (map.end() != map.find(symbol_name))
                return true;
            else
                return false;
        }

        template <typename PtrType>
        inline std::string entity_name(const PtrType& ptr) const
        {
            if (map.empty())
                return std::string();

            tm_const_itr_t itr = map.begin();

            while (map.end() != itr)
            {
                if (get_raw(itr->second.second) == ptr)
                {
                    return itr->first;
                }
                else
                    ++itr;
            }

            return std::string();
        }

        inline bool is_constant(const std::string& symbol_name) const
        {
            if (symbol_name.empty())
                return false;
            else
            {
                const tm_const_itr_t itr = map.find(symbol_name);

                if (map.end() == itr)
                    return false;
                else
                    return (*itr).second.first;
            }
        }

        template <typename Tie, typename RType>
        inline bool add_impl(const std::string& symbol_name, RType t, const bool is_const)
        {
            if (symbol_name.size() > 1)
            {
                for (std::size_t i = 0; i < core::reserved_symbols_size; ++i)
                {
                    if (core::imatch(symbol_name, core::reserved_symbols[i]))
                    {
                        return false;
                    }
                }
            }

            const tm_itr_t itr = map.find(symbol_name);

            if (map.end() == itr)
            {
                map[symbol_name] = Tie::make(t, is_const);
                ++size;
            }

            return true;
        }

        struct tie_array
        {
            static inline type_pair_t make(std::pair<T*, std::size_t> v,
                                           const bool is_const = false)
                requires(Owned)
            {
                return std::make_pair(is_const, std::make_unique<vector_t>(v.first, v.second));
            }
        };

        struct tie_stdvec
        {
            template <typename Allocator>
            static inline type_pair_t make(std::vector<T, Allocator>& v,
                                           const bool is_const = false)
                requires(Owned)
            {
                return std::make_pair(is_const, std::make_unique<vector_t>(v));
            }
        };

        struct tie_vecview
        {
            static inline type_pair_t make(math_expr::vector_view<T>& v,
                                           const bool is_const = false)
                requires(Owned)
            {
                return std::make_pair(is_const, std::make_unique<vector_t>(v));
            }
        };

        struct tie_stddeq
        {
            template <typename Allocator>
            static inline type_pair_t make(std::deque<T, Allocator>& v, const bool is_const = false)
                requires(Owned)
            {
                return std::make_pair(is_const, std::make_unique<vector_t>(v));
            }
        };

        template <std::size_t v_size>
        inline bool add(const std::string& symbol_name, T (&v)[v_size], const bool is_const = false)
            requires(Owned)
        {
            return add_impl<tie_array, std::pair<T*, std::size_t>>(
                symbol_name, std::make_pair(v, v_size), is_const);
        }

        inline bool add(const std::string& symbol_name, T* v, const std::size_t v_size,
                        const bool is_const = false)
            requires(Owned)
        {
            return add_impl<tie_array, std::pair<T*, std::size_t>>(
                symbol_name, std::make_pair(v, v_size), is_const);
        }

        template <typename Allocator>
        inline bool add(const std::string& symbol_name, std::vector<T, Allocator>& v,
                        const bool is_const = false)
            requires(Owned)
        {
            return add_impl<tie_stdvec, std::vector<T, Allocator>&>(symbol_name, v, is_const);
        }

        inline bool add(const std::string& symbol_name, math_expr::vector_view<T>& v,
                        const bool is_const = false)
            requires(Owned)
        {
            return add_impl<tie_vecview, math_expr::vector_view<T>&>(symbol_name, v, is_const);
        }

        template <typename Allocator>
        inline bool add(const std::string& symbol_name, std::deque<T, Allocator>& v,
                        const bool is_const = false)
            requires(Owned)
        {
            return add_impl<tie_stddeq, std::deque<T, Allocator>&>(symbol_name, v, is_const);
        }

        inline bool add(const std::string& symbol_name, RawType& t_, const bool is_const = false)
        {
            struct tie
            {
                static inline auto make(T& t, const bool is_constant = false)
                    requires(Owned)
                {
                    return std::make_pair(is_constant, std::make_unique<variable_node_t>(t));
                }

                static inline auto make(function_t& t, const bool is_constant = false)
                    requires(!Owned)
                {
                    return std::make_pair(is_constant, &t);
                }

                static inline auto make(vararg_function_t& t, const bool is_constant = false)
                    requires(!Owned)
                {
                    return std::make_pair(is_constant, &t);
                }

                static inline auto make(generic_function_t& t, const bool is_constant = false)
                    requires(!Owned)
                {
                    return std::make_pair(is_constant, &t);
                }
            };

            const tm_itr_t itr = map.find(symbol_name);

            if (map.end() == itr)
            {
                map[symbol_name] = tie::make(t_, is_const);
                ++size;
            }

            return true;
        }

        inline type_ptr get(const std::string& symbol_name) const
        {
            const tm_const_itr_t itr = map.find(symbol_name);

            if (map.end() == itr)
                return nullptr;
            else
                return get_raw(itr->second.second);
        }

        template <typename TType, typename TRawType, typename PtrType>
        struct ptr_match
        {
            static inline bool test(const PtrType, const TRawType*)
            {
                return false;
            }
        };

        template <typename TType, typename TRawType>
        struct ptr_match<TType, TRawType, variable_node_t*>
        {
            static inline bool test(const variable_node_t* p, const TRawType* ptr)
            {
                core::debug_print("ptr_match::test() - %p <--> %p\n",
                                  static_cast<const void*>(&(p->ref())),
                                  static_cast<const void*>(ptr));
                return (&(p->ref()) == ptr);
            }
        };

        inline type_ptr get_from_varptr(const RawType* ptr) const
        {
            tm_const_itr_t itr = map.begin();

            while (map.end() != itr)
            {
                type_ptr ret_ptr = get_raw(itr->second.second);

                if (ptr_match<Type, RawType, type_ptr>::test(ret_ptr, ptr))
                {
                    return ret_ptr;
                }

                ++itr;
            }

            return nullptr;
        }

        inline bool remove(const std::string& symbol_name, const bool delete_node = true)
        {
            const tm_itr_t itr = map.find(symbol_name);

            if (map.end() != itr)
            {
                if constexpr (Owned)
                {
                    if (!delete_node)
                        itr->second.second.release();
                }

                map.erase(itr);
                --size;

                return true;
            }
            else
                return false;
        }

        inline RawType& type_ref(const std::string& symbol_name)
        {
            struct init_type
            {
                static inline double set(double)
                {
                    return (0.0);
                }
                static inline double set(long double)
                {
                    return (0.0);
                }
                static inline float set(float)
                {
                    return (0.0f);
                }
                static inline std::string set(std::string&)
                {
                    return std::string("");
                }
            };

            static RawType null_type = init_type::set(RawType());

            const tm_const_itr_t itr = map.find(symbol_name);

            if (map.end() == itr)
                return null_type;
            else
                return itr->second.second->ref();
        }

        inline void clear(const bool delete_node = true)
        {
            if (!map.empty())
            {
                if constexpr (Owned)
                {
                    if (!delete_node)
                    {
                        for (auto& [name, type_pair] : map)
                        {
                            static_cast<void>(name);
                            type_pair.second.release();
                        }
                    }
                }

                map.clear();
            }

            size = 0;
        }

        template <typename Allocator, template <typename, typename> class Sequence>
        inline std::size_t get_list(
            Sequence<std::pair<std::string, RawType>, Allocator>& list) const
        {
            std::size_t count = 0;

            if (!map.empty())
            {
                tm_const_itr_t itr = map.begin();
                tm_const_itr_t end = map.end();

                while (end != itr)
                {
                    list.push_back(std::make_pair((*itr).first, itr->second.second->ref()));
                    ++itr;
                    ++count;
                }
            }

            return count;
        }

        template <typename Allocator, template <typename, typename> class Sequence>
        inline std::size_t get_list(Sequence<std::string, Allocator>& vlist) const
        {
            std::size_t count = 0;

            if (!map.empty())
            {
                tm_const_itr_t itr = map.begin();
                tm_const_itr_t end = map.end();

                while (end != itr)
                {
                    vlist.push_back((*itr).first);
                    ++itr;
                    ++count;
                }
            }

            return count;
        }
    };

    using expression_ptr = details::expression_node<T>*;
    using variable_t = typename details::variable_node<T>;
    using vector_holder_t = typename details::vector_holder<T>;
    using variable_ptr = variable_t*;
    using function_t = ifunction<T>;
    using vararg_function_t = ivararg_function<T>;
    using generic_function_t = igeneric_function<T>;
    using function_ptr = function_t*;
    using vararg_function_ptr = vararg_function_t*;
    using generic_function_ptr = generic_function_t*;
    using variable_store_t = type_store<variable_t, T>;
    using function_store_t = type_store<function_t, function_t, false>;
    using vararg_function_store_t = type_store<vararg_function_t, vararg_function_t, false>;
    using generic_function_store_t = type_store<generic_function_t, generic_function_t, false>;
    using vector_store_t = type_store<vector_holder_t, vector_holder_t>;
    using variable_registry_t = math_expr::variable_registry<T, variable_store_t, vector_store_t>;
    using function_registry_t =
        math_expr::function_registry<T, function_store_t, vararg_function_store_t,
                                     generic_function_store_t>;

    static constexpr std::size_t lut_size = 256;

    static_assert(requires(variable_store_t& store, const std::string& name, T& value) {
        store.add(name, value);
    });
    static_assert(requires(function_store_t& store, const std::string& name, function_t& function) {
        store.add(name, function);
    });
    static_assert(!requires(function_store_t& store, const std::string& name, T& value) {
        store.add(name, value);
    });
    static_assert(!requires(function_store_t& store, const std::string& name,
                            std::vector<T>& values) { store.add(name, values); });

    // Symbol Table Holder
    struct control_block
    {
        struct st_data
        {
            variable_store_t variable_store;
            function_store_t function_store;
            vararg_function_store_t vararg_function_store;
            generic_function_store_t generic_function_store;
            generic_function_store_t string_function_store;
            generic_function_store_t overload_function_store;
            vector_store_t vector_store;
            std::list<T> local_symbol_list_;
            std::list<std::string> local_stringvar_list_;
            std::vector<std::unique_ptr<ifunction<T>>> free_function_list_;
            variable_registry_t variable_registry;
            function_registry_t function_registry;
            std::set<std::string> reserved_symbol_table_;

            st_data()
                : variable_registry(variable_store, vector_store, local_symbol_list_),
                  function_registry(function_store, vararg_function_store, generic_function_store,
                                    string_function_store, overload_function_store,
                                    free_function_list_)
            {
                for (std::size_t i = 0; i < core::reserved_words_size; ++i)
                {
                    reserved_symbol_table_.insert(std::string(core::reserved_words[i]));
                }

                for (std::size_t i = 0; i < core::reserved_symbols_size; ++i)
                {
                    reserved_symbol_table_.insert(std::string(core::reserved_symbols[i]));
                }
            }

            inline bool is_reserved_symbol(const std::string& symbol) const
            {
                return (reserved_symbol_table_.end() != reserved_symbol_table_.find(symbol));
            }
        };

        control_block()
            : data_(std::make_unique<st_data>()), mutability_(symtab_mutability_type::e_mutable)
        {
        }

        void set_mutability(const symtab_mutability_type mutability)
        {
            mutability_ = mutability;
        }

        std::unique_ptr<st_data> data_;
        symtab_mutability_type mutability_;
    };

    using local_data_t = typename control_block::st_data;

   public:
    explicit symbol_table(
        const symtab_mutability_type mutability = symtab_mutability_type::e_mutable)
        : control_block_(std::make_shared<control_block>())
    {
        control_block_->set_mutability(mutability);
        clear();
    }

    ~symbol_table()
    {
        math_expr::core::dump_ptr("~symbol_table", this);
    }

    symbol_table(const symbol_table<T>& st) : control_block_(st.control_block_) {}

    inline symbol_table<T>& operator=(const symbol_table<T>& st)
    {
        if (this != &st)
        {
            control_block_ = st.control_block_;
        }

        return (*this);
    }

    inline bool operator==(const symbol_table<T>& st) const
    {
        return (this == &st) || (control_block_.get() == st.control_block_.get());
    }

    inline symtab_mutability_type mutability() const
    {
        return valid() ? control_block_->mutability_ : symtab_mutability_type::e_unknown;
    }

    inline void clear_variables(const bool delete_node = true)
    {
        local_data().variable_registry.clear_variables(delete_node);
    }

    inline void clear_functions()
    {
        local_data().function_registry.clear_functions();
    }

    inline void clear_strings()
    {
        local_data().variable_registry.clear_strings();
    }

    inline void clear_vectors()
    {
        local_data().variable_registry.clear_vectors();
    }

    inline void clear_local_constants()
    {
        local_data().variable_registry.clear_local_constants();
    }

    inline void clear()
    {
        if (!valid())
            return;
        clear_variables();
        clear_functions();
        clear_strings();
        clear_vectors();
        clear_local_constants();
    }

    inline std::size_t variable_count() const
    {
        if (valid())
            return local_data().variable_registry.variable_count();
        else
            return 0;
    }

    inline std::size_t function_count() const
    {
        if (valid())
            return local_data().function_registry.function_count();
        else
            return 0;
    }

    inline std::size_t vector_count() const
    {
        if (valid())
            return local_data().variable_registry.vector_count();
        else
            return 0;
    }

    inline variable_ptr get_variable(const std::string& variable_name) const
    {
        if (!valid())
            return nullptr;
        else if (!valid_symbol(variable_name))
            return nullptr;
        else
            return local_data().variable_registry.get_variable(variable_name);
    }

    inline variable_ptr get_variable(const T& var_ref) const
    {
        if (!valid())
            return nullptr;
        else
            return local_data().variable_registry.get_variable(var_ref);
    }

    inline function_ptr get_function(const std::string& function_name) const
    {
        if (!valid())
            return nullptr;
        else if (!valid_function(function_name))
            return nullptr;
        else
            return local_data().function_registry.get_function(function_name);
    }

    inline vararg_function_ptr get_vararg_function(const std::string& vararg_function_name) const
    {
        if (!valid())
            return nullptr;
        else if (!valid_function(vararg_function_name))
            return nullptr;
        else
            return local_data().function_registry.get_vararg_function(vararg_function_name);
    }

    inline generic_function_ptr get_generic_function(const std::string& function_name) const
    {
        if (!valid())
            return nullptr;
        else if (!valid_function(function_name))
            return nullptr;
        else
            return local_data().function_registry.get_generic_function(function_name);
    }

    inline generic_function_ptr get_string_function(const std::string& function_name) const
    {
        if (!valid())
            return nullptr;
        else if (!valid_function(function_name))
            return nullptr;
        else
            return local_data().function_registry.get_string_function(function_name);
    }

    inline generic_function_ptr get_overload_function(const std::string& function_name) const
    {
        if (!valid())
            return nullptr;
        else if (!valid_function(function_name))
            return nullptr;
        else
            return local_data().function_registry.get_overload_function(function_name);
    }

    using vector_holder_ptr = vector_holder_t*;

    inline vector_holder_ptr get_vector(const std::string& vector_name) const
    {
        if (!valid())
            return nullptr;
        else if (!valid_symbol(vector_name))
            return nullptr;
        else
            return local_data().variable_registry.get_vector(vector_name);
    }

    inline T& variable_ref(const std::string& symbol_name)
    {
        thread_local T null_var = T(0);
        if (!valid())
            return null_var;
        else if (!valid_symbol(symbol_name))
            return null_var;
        else
            return local_data().variable_registry.variable_ref(symbol_name);
    }

    inline bool is_constant_node(const std::string& symbol_name) const
    {
        if (!valid())
            return false;
        else if (!valid_symbol(symbol_name))
            return false;
        else
            return local_data().variable_registry.is_constant_node(symbol_name);
    }

    inline bool create_variable(const std::string& variable_name, const T& value = T(0))
    {
        if (!valid())
            return false;
        else if (!valid_symbol(variable_name))
            return false;
        else if (symbol_exists(variable_name))
            return false;

        return local_data().variable_registry.create_variable(variable_name, value);
    }

    inline bool add_variable(const std::string& variable_name, T& t, const bool is_constant = false)
    {
        if (!valid())
            return false;
        else if (!valid_symbol(variable_name))
            return false;
        else if (symbol_exists(variable_name))
            return false;
        else
            return local_data().variable_registry.add_variable(variable_name, t, is_constant);
    }

    inline bool add_constant(const std::string& constant_name, const T& value)
    {
        if (!valid())
            return false;
        else if (!valid_symbol(constant_name))
            return false;
        else if (symbol_exists(constant_name))
            return false;

        return local_data().variable_registry.add_constant(constant_name, value);
    }

    inline bool add_function(const std::string& function_name, function_t& function)
    {
        if (!valid())
            return false;
        else if (!valid_symbol(function_name))
            return false;
        else if (symbol_exists(function_name))
            return false;
        else
            return local_data().function_registry.add_function(function_name, function);
    }

    inline bool add_function(const std::string& vararg_function_name,
                             vararg_function_t& vararg_function)
    {
        if (!valid())
            return false;
        else if (!valid_symbol(vararg_function_name))
            return false;
        else if (symbol_exists(vararg_function_name))
            return false;
        else
            return local_data().function_registry.add_function(vararg_function_name,
                                                               vararg_function);
    }

    inline bool add_function(const std::string& function_name, generic_function_t& function)
    {
        if (!valid())
            return false;
        else if (!valid_symbol(function_name))
            return false;
        else if (symbol_exists(function_name))
            return false;
        else
            return local_data().function_registry.add_function(function_name, function);
    }

    template <typename... Args>
    inline bool add_function(const std::string& function_name, T (*function)(Args...))
    {
        if (!valid())
            return false;
        if (!valid_symbol(function_name))
            return false;
        if (symbol_exists(function_name))
            return false;
        return local_data().function_registry.add_function(function_name, function);
    }

#define MATH_EXPR_DEFINE_FREEFUNCTION(NN)                              \
    inline bool add_function(const std::string& n, ff##NN##_functor f) \
    {                                                                  \
        return add_function_impl_(n, f);                               \
    }

    MATH_EXPR_DEFINE_FREEFUNCTION(00);
    MATH_EXPR_DEFINE_FREEFUNCTION(01);
    MATH_EXPR_DEFINE_FREEFUNCTION(02);
    MATH_EXPR_DEFINE_FREEFUNCTION(03);
    MATH_EXPR_DEFINE_FREEFUNCTION(04);
    MATH_EXPR_DEFINE_FREEFUNCTION(05);
    MATH_EXPR_DEFINE_FREEFUNCTION(06);
    MATH_EXPR_DEFINE_FREEFUNCTION(07);
    MATH_EXPR_DEFINE_FREEFUNCTION(08);
    MATH_EXPR_DEFINE_FREEFUNCTION(09);
    MATH_EXPR_DEFINE_FREEFUNCTION(10);
    MATH_EXPR_DEFINE_FREEFUNCTION(11);
    MATH_EXPR_DEFINE_FREEFUNCTION(12);
    MATH_EXPR_DEFINE_FREEFUNCTION(13);
    MATH_EXPR_DEFINE_FREEFUNCTION(14);
    MATH_EXPR_DEFINE_FREEFUNCTION(15);
#undef MATH_EXPR_DEFINE_FREEFUNCTION

    inline bool add_reserved_function(const std::string& function_name, function_t& function)
    {
        if (!valid())
            return false;
        else if (!valid_symbol(function_name, false))
            return false;
        else if (symbol_exists(function_name, false))
            return false;
        else
            return local_data().function_registry.add_function(function_name, function);
    }

    inline bool add_reserved_function(const std::string& vararg_function_name,
                                      vararg_function_t& vararg_function)
    {
        if (!valid())
            return false;
        else if (!valid_symbol(vararg_function_name, false))
            return false;
        else if (symbol_exists(vararg_function_name, false))
            return false;
        else
            return local_data().function_registry.add_function(vararg_function_name,
                                                               vararg_function);
    }

    inline bool add_reserved_function(const std::string& function_name,
                                      generic_function_t& function)
    {
        if (!valid())
            return false;
        else if (!valid_symbol(function_name, false))
            return false;
        else if (symbol_exists(function_name, false))
            return false;
        else
            return local_data().function_registry.add_function(function_name, function);
    }

    template <typename... Args>
    inline bool add_reserved_function(const std::string& function_name, T (*function)(Args...))
    {
        if (!valid())
            return false;
        if (!valid_symbol(function_name, false))
            return false;
        if (symbol_exists(function_name, false))
            return false;
        return local_data().function_registry.add_function(function_name, function);
    }

#define MATH_EXPR_DEFINE_RESERVED_FREEFUNCTION(NN)                              \
    inline bool add_reserved_function(const std::string& n, ff##NN##_functor f) \
    {                                                                           \
        return add_reserved_function_impl_(n, f);                               \
    }

    MATH_EXPR_DEFINE_RESERVED_FREEFUNCTION(00);
    MATH_EXPR_DEFINE_RESERVED_FREEFUNCTION(01);
    MATH_EXPR_DEFINE_RESERVED_FREEFUNCTION(02);
    MATH_EXPR_DEFINE_RESERVED_FREEFUNCTION(03);
    MATH_EXPR_DEFINE_RESERVED_FREEFUNCTION(04);
    MATH_EXPR_DEFINE_RESERVED_FREEFUNCTION(05);
    MATH_EXPR_DEFINE_RESERVED_FREEFUNCTION(06);
    MATH_EXPR_DEFINE_RESERVED_FREEFUNCTION(07);
    MATH_EXPR_DEFINE_RESERVED_FREEFUNCTION(08);
    MATH_EXPR_DEFINE_RESERVED_FREEFUNCTION(09);
    MATH_EXPR_DEFINE_RESERVED_FREEFUNCTION(10);
    MATH_EXPR_DEFINE_RESERVED_FREEFUNCTION(11);
    MATH_EXPR_DEFINE_RESERVED_FREEFUNCTION(12);
    MATH_EXPR_DEFINE_RESERVED_FREEFUNCTION(13);
    MATH_EXPR_DEFINE_RESERVED_FREEFUNCTION(14);
    MATH_EXPR_DEFINE_RESERVED_FREEFUNCTION(15);
#undef MATH_EXPR_DEFINE_RESERVED_FREEFUNCTION

    template <std::size_t N>
    inline bool add_vector(const std::string& vector_name, T (&v)[N])
    {
        if (!valid())
            return false;
        else if (!valid_symbol(vector_name))
            return false;
        else if (symbol_exists(vector_name))
            return false;
        else
            return local_data().variable_registry.add_vector(vector_name, v);
    }

    inline bool add_vector(const std::string& vector_name, T* v, const std::size_t& v_size)
    {
        if (!valid())
            return false;
        else if (!valid_symbol(vector_name))
            return false;
        else if (symbol_exists(vector_name))
            return false;
        else if (0 == v_size)
            return false;
        else
            return local_data().variable_registry.add_vector(vector_name, v, v_size);
    }

    template <typename Allocator>
    inline bool add_vector(const std::string& vector_name, std::vector<T, Allocator>& v)
    {
        if (!valid())
            return false;
        else if (!valid_symbol(vector_name))
            return false;
        else if (symbol_exists(vector_name))
            return false;
        else if (0 == v.size())
            return false;
        else
            return local_data().variable_registry.add_vector(vector_name, v);
    }

    inline bool add_vector(const std::string& vector_name, math_expr::vector_view<T>& v)
    {
        if (!valid())
            return false;
        else if (!valid_symbol(vector_name))
            return false;
        else if (symbol_exists(vector_name))
            return false;
        else if (0 == v.size())
            return false;
        else
            return local_data().variable_registry.add_vector(vector_name, v);
    }

    inline bool remove_variable(const std::string& variable_name, const bool delete_node = true)
    {
        if (!valid())
            return false;
        else
            return local_data().variable_registry.remove_variable(variable_name, delete_node);
    }

    inline bool remove_function(const std::string& function_name)
    {
        if (!valid())
            return false;
        else
            return local_data().function_registry.remove_function(function_name);
    }

    inline bool remove_vararg_function(const std::string& vararg_function_name)
    {
        if (!valid())
            return false;
        else
            return local_data().function_registry.remove_vararg_function(vararg_function_name);
    }

    inline bool remove_vector(const std::string& vector_name)
    {
        if (!valid())
            return false;
        else
            return local_data().variable_registry.remove_vector(vector_name);
    }

    inline bool add_constants()
    {
        return add_pi() && add_epsilon() && add_infinity();
    }

    inline bool add_pi()
    {
        static constexpr T local_pi = core::numeric::const_pi<T>();
        return add_constant("pi", local_pi);
    }

    inline bool add_epsilon()
    {
        static constexpr T local_epsilon = core::numeric::details::epsilon_type<T>::value();
        return add_constant("epsilon", local_epsilon);
    }

    inline bool add_infinity()
    {
        static constexpr T local_infinity = std::numeric_limits<T>::infinity();
        return add_constant("inf", local_infinity);
    }

    template <typename Package>
    inline bool add_package(Package& package)
    {
        return package.register_package(*this);
    }

    template <typename Allocator, template <typename, typename> class Sequence>
    inline std::size_t get_variable_list(
        Sequence<std::pair<std::string, T>, Allocator>& vlist) const
    {
        if (!valid())
            return 0;
        else
            return local_data().variable_registry.get_variable_list(vlist);
    }

    template <typename Allocator, template <typename, typename> class Sequence>
    inline std::size_t get_variable_list(Sequence<std::string, Allocator>& vlist) const
    {
        if (!valid())
            return 0;
        else
            return local_data().variable_registry.get_variable_list(vlist);
    }

    template <typename Allocator, template <typename, typename> class Sequence>
    inline std::size_t get_vector_list(Sequence<std::string, Allocator>& vec_list) const
    {
        if (!valid())
            return 0;
        else
            return local_data().variable_registry.get_vector_list(vec_list);
    }

    template <typename Allocator, template <typename, typename> class Sequence>
    inline std::size_t get_function_list(Sequence<std::string, Allocator>& function_list) const
    {
        if (!valid())
            return 0;
        return local_data().function_registry.get_function_list(function_list);
    }

    inline std::vector<std::string> get_function_list() const
    {
        std::vector<std::string> result;
        get_function_list(result);
        return result;
    }

    inline bool symbol_exists(const std::string& symbol_name,
                              const bool check_reserved_symb = true) const
    {
        /*
           Function will return true if symbol_name exists as either a
           reserved symbol, variable, stringvar, vector or function name
           in any of the type stores.
        */
        if (!valid())
            return false;
        else if (local_data().variable_registry.symbol_exists(symbol_name))
            return true;
        else if (local_data().function_registry.symbol_exists(symbol_name))
            return true;
        else if (check_reserved_symb && local_data().is_reserved_symbol(symbol_name))
            return true;
        else
            return false;
    }

    inline bool is_variable(const std::string& variable_name) const
    {
        if (!valid())
            return false;
        else
            return local_data().variable_registry.is_variable(variable_name);
    }

    inline bool is_function(const std::string& function_name) const
    {
        if (!valid())
            return false;
        else
            return local_data().function_registry.is_function(function_name);
    }

    inline bool is_vararg_function(const std::string& vararg_function_name) const
    {
        if (!valid())
            return false;
        else
            return local_data().function_registry.is_vararg_function(vararg_function_name);
    }

    inline bool is_vector(const std::string& vector_name) const
    {
        if (!valid())
            return false;
        else
            return local_data().variable_registry.is_vector(vector_name);
    }

    inline std::string get_variable_name(const expression_ptr& ptr) const
    {
        return local_data().variable_registry.get_variable_name(ptr);
    }

    inline std::string get_vector_name(const vector_holder_ptr& ptr) const
    {
        return local_data().variable_registry.get_vector_name(ptr);
    }

    inline bool valid() const
    {
        // Symbol table sanity check.
        return control_block_ && control_block_->data_;
    }

    inline void load_from(const symbol_table<T>& st)
    {
        local_data().function_registry.load_from(st.local_data().function_registry);
    }

    inline void load_variables_from(const symbol_table<T>& st)
    {
        local_data().variable_registry.load_variables_from(st.local_data().variable_registry);
    }

    inline void load_vectors_from(const symbol_table<T>& st)
    {
        local_data().variable_registry.load_vectors_from(st.local_data().variable_registry);
    }

   private:
    template <typename... Args>
    inline bool add_function_impl_(const std::string& function_name, T (*function)(Args...))
    {
        if (!valid())
            return false;
        if (!valid_symbol(function_name))
            return false;
        if (symbol_exists(function_name))
            return false;
        return local_data().function_registry.add_function(function_name, function);
    }

    template <typename... Args>
    inline bool add_reserved_function_impl_(const std::string& function_name,
                                            T (*function)(Args...))
    {
        if (!valid())
            return false;
        if (!valid_symbol(function_name, false))
            return false;
        if (symbol_exists(function_name, false))
            return false;
        return local_data().function_registry.add_function(function_name, function);
    }

    inline bool valid_symbol(const std::string& symbol, const bool check_reserved_symb = true) const
    {
        if (symbol.empty())
            return false;
        else if (!core::is_letter(symbol[0]))
            return false;
        else if (symbol.size() > 1)
        {
            for (std::size_t i = 1; i < symbol.size(); ++i)
            {
                if (!core::is_letter_or_digit(symbol[i]) && ('_' != symbol[i]))
                {
                    if ((i < (symbol.size() - 1)) && ('.' == symbol[i]))
                        continue;
                    else
                        return false;
                }
            }
        }

        return (check_reserved_symb) ? (!local_data().is_reserved_symbol(symbol)) : true;
    }

    inline bool valid_function(const std::string& symbol) const
    {
        if (symbol.empty())
            return false;
        else if (!core::is_letter(symbol[0]))
            return false;
        else if (symbol.size() > 1)
        {
            for (std::size_t i = 1; i < symbol.size(); ++i)
            {
                if (!core::is_letter_or_digit(symbol[i]) && ('_' != symbol[i]))
                {
                    if ((i < (symbol.size() - 1)) && ('.' == symbol[i]))
                        continue;
                    else
                        return false;
                }
            }
        }

        return true;
    }

    inline local_data_t& local_data()
    {
        return *(control_block_->data_);
    }

    inline const local_data_t& local_data() const
    {
        return *(control_block_->data_);
    }

    std::shared_ptr<control_block> control_block_;

    friend class parser<T>;
};  // class symbol_table

template <typename T>
class function_compositor;

}  // namespace math_expr

#endif
