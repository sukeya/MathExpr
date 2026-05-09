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

#ifndef MATH_EXPR_PARSER_SCOPE_MANAGER_HPP
#define MATH_EXPR_PARSER_SCOPE_MANAGER_HPP

#include "math_expr/core/std_includes.hpp"
#include "math_expr/core/string_utils.hpp"
#include "math_expr/details/expression_node.hpp"
#include "math_expr/details/node_utils.hpp"
#include "math_expr/details/string_nodes.hpp"
#include "math_expr/details/vector_nodes.hpp"

namespace math_expr
{
template <typename T>
struct scope_element
{
    enum class element_type
    {
        e_none,
        e_literal,
        e_variable,
        e_vector,
        e_vecelem,
        e_string
    };

    using vector_holder_t = details::vector_holder<T>;
    using literal_node_ptr = details::literal_node<T>*;
    using variable_node_ptr = details::variable_node<T>*;
    using vector_holder_ptr = vector_holder_t*;
    using expression_node_ptr = details::expression_node<T>*;
#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
    using stringvar_node_ptr = details::string_nodes::stringvar_node<T>*;
#endif

    scope_element()
        : name("???"),
          size(std::numeric_limits<std::size_t>::max()),
          index(std::numeric_limits<std::size_t>::max()),
          depth(std::numeric_limits<std::size_t>::max()),
          ref_count(0),
          ip_index(0),
          type(element_type::e_none),
          active(false),
          var_node(nullptr),
          vec_node(nullptr)
#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
          ,
          str_node(nullptr)
#endif
    {
    }

    bool operator<(const scope_element& se) const
    {
        if (ip_index < se.ip_index)
            return true;
        else if (ip_index > se.ip_index)
            return false;
        else if (depth < se.depth)
            return true;
        else if (depth > se.depth)
            return false;
        else if (index < se.index)
            return true;
        else if (index > se.index)
            return false;
        else
            return (name < se.name);
    }

    void clear()
    {
        name = "???";
        size = std::numeric_limits<std::size_t>::max();
        index = std::numeric_limits<std::size_t>::max();
        depth = std::numeric_limits<std::size_t>::max();
        type = element_type::e_none;
        active = false;
        ref_count = 0;
        ip_index = 0;
        scalar_data.reset();
        vector_data.reset();
#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
        str_data.reset();
#endif
        var_node = nullptr;
        vec_node = nullptr;
#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
        str_node = nullptr;
#endif
    }

    std::string name;
    std::size_t size;
    std::size_t index;
    std::size_t depth;
    std::size_t ref_count;
    std::size_t ip_index;
    element_type type;
    bool active;
    std::unique_ptr<T> scalar_data;
    std::unique_ptr<T[]> vector_data;
#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
    std::unique_ptr<std::string> str_data;
#endif
    expression_node_ptr var_node;
    vector_holder_ptr vec_node;
#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
    stringvar_node_ptr str_node;
#endif
};

template <typename T>
class scope_element_manager
{
   public:
    using scope_element_t = scope_element<T>;
    using expression_node_t = details::expression_node<T>;
    using variable_node_t = details::variable_node<T>;
    using expression_node_ptr = expression_node_t*;
    using variable_node_ptr = variable_node_t*;

    scope_element_manager()
        : scope_depth_(nullptr), input_param_cnt_(0), total_local_symb_size_bytes_(0)
    {
    }

    inline void set_scope_depth(std::size_t& scope_depth)
    {
        scope_depth_ = &scope_depth;
    }

    inline std::size_t size() const
    {
        return element_.size();
    }

    inline bool empty() const
    {
        return element_.empty();
    }

    inline scope_element_t& get_element(const std::size_t& index)
    {
        if (index < element_.size())
            return element_[index];
        else
            return null_element_;
    }

    inline scope_element_t& get_element(
        const std::string& var_name,
        const std::size_t index = std::numeric_limits<std::size_t>::max())
    {
        const std::size_t current_depth = current_scope_depth();

        for (std::size_t i = 0; i < element_.size(); ++i)
        {
            scope_element_t& se = element_[i];

            if (se.depth > current_depth)
                continue;
            else if (core::imatch(se.name, var_name) && (se.index == index))
                return se;
        }

        return null_element_;
    }

    inline scope_element_t& get_active_element(
        const std::string& var_name,
        const std::size_t index = std::numeric_limits<std::size_t>::max())
    {
        const std::size_t current_depth = current_scope_depth();

        for (std::size_t i = 0; i < element_.size(); ++i)
        {
            scope_element_t& se = element_[i];

            if (se.depth > current_depth)
                continue;
            else if (core::imatch(se.name, var_name) && (se.index == index) && (se.active))
                return se;
        }

        return null_element_;
    }

    inline bool add_element(scope_element_t&& se)
    {
        for (std::size_t i = 0; i < element_.size(); ++i)
        {
            scope_element_t& cse = element_[i];

            if (core::imatch(cse.name, se.name) && (cse.depth <= se.depth) &&
                (cse.index == se.index) && (cse.size == se.size) && (cse.type == se.type) &&
                (cse.active))
                return false;
        }

        switch (se.type)
        {
            case scope_element_t::element_type::e_variable:
                total_local_symb_size_bytes_ += sizeof(T);
                break;

            case scope_element_t::element_type::e_literal:
                total_local_symb_size_bytes_ += sizeof(T);
                break;

            case scope_element_t::element_type::e_vector:
                total_local_symb_size_bytes_ += sizeof(T) * se.size;
                break;

            default:
                break;
        }

        element_.push_back(std::move(se));
        std::sort(element_.begin(), element_.end());

        return true;
    }

    inline void deactivate(const std::size_t& scope_depth)
    {
        core::debug_print("deactivate() - Scope depth: %d\n",
                          static_cast<int>(current_scope_depth()));

        for (std::size_t i = 0; i < element_.size(); ++i)
        {
            scope_element_t& se = element_[i];

            if (se.active && (se.depth >= scope_depth))
            {
                core::debug_print("deactivate() - element[%02d] '%s'\n", static_cast<int>(i),
                                  se.name.c_str());

                se.active = false;
            }
        }
    }

    inline void free_element(scope_element_t& se)
    {
        core::debug_print("free_element() - se[%s]\n", se.name.c_str());

        switch (se.type)
        {
            case scope_element_t::element_type::e_literal:
            case scope_element_t::element_type::e_variable:
            case scope_element_t::element_type::e_vecelem:
            {
                std::unique_ptr<expression_node_t> node(std::exchange(se.var_node, nullptr));
                break;
            }

            case scope_element_t::element_type::e_vector:
            {
                std::unique_ptr<typename scope_element_t::vector_holder_t> node(
                    std::exchange(se.vec_node, nullptr));
                break;
            }

#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
            case scope_element_t::element_type::e_string:
            {
                std::unique_ptr<details::string_nodes::stringvar_node<T>> node(
                    std::exchange(se.str_node, nullptr));
                break;
            }
#endif

            default:
                return;
        }

        se.clear();
    }

    inline void cleanup()
    {
        for (std::size_t i = 0; i < element_.size(); ++i)
        {
            free_element(element_[i]);
        }

        element_.clear();

        input_param_cnt_ = 0;
        total_local_symb_size_bytes_ = 0;
    }

    inline std::size_t total_local_symb_size_bytes() const
    {
        return total_local_symb_size_bytes_;
    }

    inline std::size_t next_ip_index()
    {
        return ++input_param_cnt_;
    }

    inline expression_node_ptr get_variable(const T& v)
    {
        for (std::size_t i = 0; i < element_.size(); ++i)
        {
            scope_element_t& se = element_[i];

            if (se.active && se.var_node && details::is_variable_node(se.var_node))
            {
                variable_node_ptr vn = static_cast<variable_node_ptr>(se.var_node);

                if (&(vn->ref()) == (&v))
                {
                    return se.var_node;
                }
            }
        }

        return expression_node_ptr(0);
    }

    inline std::string get_vector_name(const T* data)
    {
        for (std::size_t i = 0; i < element_.size(); ++i)
        {
            scope_element_t& se = element_[i];

            if (se.active && se.vec_node && (se.vec_node->data() == data))
            {
                return se.name;
            }
        }

        return "neo-vector";
    }

   private:
    inline std::size_t current_scope_depth() const
    {
        assert(scope_depth_);
        return *scope_depth_;
    }

    std::size_t* scope_depth_;
    std::vector<scope_element_t> element_;
    scope_element_t null_element_;
    std::size_t input_param_cnt_;
    std::size_t total_local_symb_size_bytes_;
};

template <typename T>
class scope_handler
{
   public:
    explicit scope_handler(std::size_t& scope_depth, scope_element_manager<T>& scope_manager)
        : scope_depth_(scope_depth), scope_manager_(scope_manager)
    {
        ++scope_depth_;
        if constexpr (::math_expr::core::build_options::kEnableDebugging)
        {
            const std::string depth(2 * scope_depth_, '-');
            core::debug_print("%s> Scope Depth: %02d\n", depth.c_str(),
                              static_cast<int>(scope_depth_));
        }
    }

    ~scope_handler()
    {
        scope_manager_.deactivate(scope_depth_);
        --scope_depth_;
        if constexpr (::math_expr::core::build_options::kEnableDebugging)
        {
            const std::string depth(2 * scope_depth_, '-');
            core::debug_print("<%s Scope Depth: %02d\n", depth.c_str(),
                              static_cast<int>(scope_depth_));
        }
    }

   private:
    std::size_t& scope_depth_;
    scope_element_manager<T>& scope_manager_;
};
}  // namespace math_expr

#endif
