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

#ifndef MATH_EXPR_EXPRESSION_HPP
#define MATH_EXPR_EXPRESSION_HPP

#include "math_expr/core/std_includes.hpp"
#include "math_expr/results_context.hpp"
#include "math_expr/symbol_table.hpp"
#include "math_expr/details/node_utils.hpp"

namespace math_expr
{
template <typename T>
class expression
{
   private:
    using expression_ptr = details::expression_node<T>*;
    using vector_holder_ptr = details::vector_holder<T>*;
    using symtab_list_t = std::vector<symbol_table<T>>;

   public:
    struct control_block
    {
        enum class data_type
        {
            e_unknown,
            e_expr,
            e_vecholder,
            e_data,
            e_vecdata,
            e_string
        };

        static std::string to_str(data_type dt)
        {
            switch (dt)
            {
                case data_type::e_unknown:
                    return "e_unknown";
                case data_type::e_expr:
                    return "e_expr";
                case data_type::e_vecholder:
                    return "e_vecholder";
                case data_type::e_data:
                    return "e_data";
                case data_type::e_vecdata:
                    return "e_vecdata";
                case data_type::e_string:
                    return "e_string";
            }

            return "";
        }

        struct data_pack
        {
            data_pack() = default;
            data_pack(data_pack&&) = default;
            data_pack& operator=(data_pack&&) = default;
            data_pack(const data_pack&) = delete;
            data_pack& operator=(const data_pack&) = delete;

            explicit data_pack(std::unique_ptr<details::expression_node<T>> p)
                : owned_(std::move(p)), type(data_type::e_expr), size(0)
            {
            }

            explicit data_pack(std::unique_ptr<details::vector_holder<T>> p)
                : owned_(std::move(p)), type(data_type::e_vecholder), size(0)
            {
            }

            data_pack(std::unique_ptr<T> p, std::size_t sz)
                : owned_(std::move(p)), type(data_type::e_data), size(sz)
            {
            }

            data_pack(std::unique_ptr<T[]> p, std::size_t sz)
                : owned_(std::move(p)), type(data_type::e_vecdata), size(sz)
            {
            }

            data_pack(std::unique_ptr<std::string> p, std::size_t sz)
                : owned_(std::move(p)), type(data_type::e_string), size(sz)
            {
            }

            bool has_data() const noexcept
            {
                return !std::holds_alternative<std::monostate>(owned_);
            }

            template <typename U>
            std::conditional_t<std::is_array_v<U>, std::remove_extent_t<U>*, U*> get()
                const noexcept
            {
                if (const auto* p = std::get_if<std::unique_ptr<U>>(&owned_))
                    return p->get();
                return nullptr;
            }

            data_type type{data_type::e_unknown};
            std::size_t size{0};

           private:
            using owned_t =
                std::variant<std::monostate, std::unique_ptr<details::expression_node<T>>,
                             std::unique_ptr<details::vector_holder<T>>, std::unique_ptr<T>,
                             std::unique_ptr<T[]>, std::unique_ptr<std::string>>;

            owned_t owned_{std::monostate{}};
        };

        using local_data_list_t = std::vector<data_pack>;
        using results_context_t = results_context<T>;

        control_block() : expr(nullptr), retinv_null(false), return_invoked(&retinv_null) {}

        explicit control_block(expression_ptr e)
            : expr(e), retinv_null(false), return_invoked(&retinv_null)
        {
        }

        ~control_block()
        {
            if (expr && details::branch_deletable(expr))
            {
                destroy_node(expr);
            }
        }

        expression_ptr expr;
        local_data_list_t local_data_list;
        std::unique_ptr<results_context_t> results;
        bool retinv_null;
        bool* return_invoked;

        friend class function_compositor<T>;
    };

    inline const std::shared_ptr<control_block>& get_control_block() const
    {
        return control_block_;
    }

    inline const typename control_block::local_data_list_t& local_data_list() const
    {
        if (control_block_)
            return control_block_->local_data_list;

        static typename control_block::local_data_list_t null_local_data_list;
        return null_local_data_list;
    }

   public:
    expression()
    {
        set_expression(new details::null_node<T>());
    }

    expression(const expression<T>& e)
        : control_block_(e.control_block_), symbol_table_list_(e.symbol_table_list_)
    {
    }

    explicit expression(const symbol_table<T>& symbol_table)
    {
        set_expression(new details::null_node<T>());
        symbol_table_list_.push_back(symbol_table);
    }

    inline expression<T>& operator=(const expression<T>& e)
    {
        if (this != &e)
        {
            control_block_ = e.control_block_;
            symbol_table_list_ = e.symbol_table_list_;
        }

        return *this;
    }

    inline bool operator==(const expression<T>& e) const
    {
        return (this == &e);
    }

    inline bool operator!() const
    {
        return (!control_block_ || (nullptr == control_block_->expr));
    }

    inline expression<T>& release()
    {
        math_expr::core::dump_ptr("expression::release", this);
        control_block_.reset();

        return (*this);
    }

    ~expression() = default;

    inline T value() const
    {
        assert(control_block_);
        assert(control_block_->expr);

        return control_block_->expr->value();
    }

    inline T operator()() const
    {
        return value();
    }

    inline operator T() const
    {
        return value();
    }

    inline operator bool() const
    {
        return details::is_true(value());
    }

    inline bool register_symbol_table(symbol_table<T>& st)
    {
        for (std::size_t i = 0; i < symbol_table_list_.size(); ++i)
        {
            if (st == symbol_table_list_[i])
            {
                return false;
            }
        }

        symbol_table_list_.push_back(st);
        return true;
    }

    inline const symbol_table<T>& get_symbol_table(const std::size_t& index = 0) const
    {
        return symbol_table_list_[index];
    }

    inline symbol_table<T>& get_symbol_table(const std::size_t& index = 0)
    {
        return symbol_table_list_[index];
    }

    std::size_t num_symbol_tables() const
    {
        return symbol_table_list_.size();
    }

    using results_context_t = results_context<T>;

    inline const results_context_t& results() const
    {
        if (control_block_->results)
            return (*control_block_->results);
        else
        {
            static const results_context_t null_results;
            return null_results;
        }
    }

    inline bool return_invoked() const
    {
        assert(control_block_);

        return control_block_ && control_block_->return_invoked &&
               (*control_block_->return_invoked);
    }

   private:
    inline symtab_list_t get_symbol_table_list() const
    {
        return symbol_table_list_;
    }

    inline void set_expression(const expression_ptr expr)
    {
        if (expr)
        {
            control_block_ = std::make_shared<control_block>(expr);
        }
    }

    inline void register_local_var(expression_ptr expr)
    {
        if (expr && control_block_)
        {
            control_block_->local_data_list.emplace_back(
                std::unique_ptr<details::expression_node<T>>(expr));
        }
    }

    inline void register_local_var(vector_holder_ptr vec_holder)
    {
        if (vec_holder && control_block_)
        {
            control_block_->local_data_list.emplace_back(
                std::unique_ptr<details::vector_holder<T>>(vec_holder));
        }
    }

    inline void register_local_data(std::unique_ptr<T> data, const std::size_t size = 0)
    {
        if (data && control_block_)
            control_block_->local_data_list.emplace_back(std::move(data), size);
    }

    inline void register_local_data(std::unique_ptr<T[]> data, const std::size_t size = 0)
    {
        if (data && control_block_)
            control_block_->local_data_list.emplace_back(std::move(data), size);
    }

    inline void register_local_data(std::unique_ptr<std::string> data, const std::size_t size = 0)
    {
        if (data && control_block_)
            control_block_->local_data_list.emplace_back(std::move(data), size);
    }

    inline void register_return_results(results_context_t* rc)
    {
        if (control_block_ && rc)
        {
            control_block_->results = std::unique_ptr<results_context_t>(rc);
        }
    }

    inline void set_retinvk(bool* retinvk_ptr)
    {
        if (control_block_)
        {
            control_block_->return_invoked = retinvk_ptr;
        }
    }

    std::shared_ptr<control_block> control_block_;
    symtab_list_t symbol_table_list_;

    friend class parser<T>;
    template <typename TT>
    friend bool is_valid(const expression<TT>& expr);
};  // class expression

}  // namespace math_expr

#endif
