/*
 **************************************************************
 *         C++ Mathematical Expression Toolkit Library        *
 *                                                            *
 * Simple Example 02                                          *
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
template <typename T> class expression
{
  private:
    using expression_ptr = details::expression_node<T>*;
    using vector_holder_ptr = details::vector_holder<T>*;
    using symtab_list_t = std::vector<symbol_table<T>>;

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
            data_pack() : pointer(0), type(data_type::e_unknown), size(0) {}

            data_pack(void* ptr, const data_type dt, const std::size_t sz = 0)
                : pointer(ptr), type(dt), size(sz)
            {
            }

            void* pointer;
            data_type type;
            std::size_t size;
        };

        using local_data_list_t = std::vector<data_pack>;
        using results_context_t = results_context<T>;
        using cntrl_blck_ptr_t = control_block*;

        control_block()
            : ref_count(0), expr(0), results(0), retinv_null(false), return_invoked(&retinv_null)
        {
        }

        explicit control_block(expression_ptr e)
            : ref_count(1), expr(e), results(0), retinv_null(false), return_invoked(&retinv_null)
        {
        }

        ~control_block()
        {
            if (expr && details::branch_deletable(expr))
            {
                destroy_node(expr);
            }

            if (!local_data_list.empty())
            {
                for (std::size_t i = 0; i < local_data_list.size(); ++i)
                {
                    switch (local_data_list[i].type)
                    {
                    case data_type::e_expr:
                        delete reinterpret_cast<expression_ptr>(local_data_list[i].pointer);
                        break;

                    case data_type::e_vecholder:
                        delete reinterpret_cast<vector_holder_ptr>(local_data_list[i].pointer);
                        break;

                    case data_type::e_data:
                        delete reinterpret_cast<T*>(local_data_list[i].pointer);
                        break;

                    case data_type::e_vecdata:
                        delete[] reinterpret_cast<T*>(local_data_list[i].pointer);
                        break;

                    case data_type::e_string:
                        delete reinterpret_cast<std::string*>(local_data_list[i].pointer);
                        break;

                    default:
                        break;
                    }
                }
            }

            if (results)
            {
                delete results;
            }
        }

        static inline cntrl_blck_ptr_t create(expression_ptr e)
        {
            return new control_block(e);
        }

        static inline void destroy(cntrl_blck_ptr_t& cntrl_blck)
        {
            if (cntrl_blck)
            {
                if ((0 != cntrl_blck->ref_count) && (0 == --cntrl_blck->ref_count))
                {
                    delete cntrl_blck;
                }

                cntrl_blck = 0;
            }
        }

        std::size_t ref_count;
        expression_ptr expr;
        local_data_list_t local_data_list;
        results_context_t* results;
        bool retinv_null;
        bool* return_invoked;

        friend class function_compositor<T>;
    };

  public:
    expression() : control_block_(0)
    {
        set_expression(new details::null_node<T>());
    }

    expression(const expression<T>& e)
        : control_block_(e.control_block_), symbol_table_list_(e.symbol_table_list_)
    {
        control_block_->ref_count++;
    }

    explicit expression(const symbol_table<T>& symbol_table) : control_block_(0)
    {
        set_expression(new details::null_node<T>());
        symbol_table_list_.push_back(symbol_table);
    }

    inline expression<T>& operator=(const expression<T>& e)
    {
        if (this != &e)
        {
            if (control_block_)
            {
                if ((0 != control_block_->ref_count) && (0 == --control_block_->ref_count))
                {
                    delete control_block_;
                }

                control_block_ = 0;
            }

            control_block_ = e.control_block_;
            control_block_->ref_count++;
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
        return ((0 == control_block_) || (0 == control_block_->expr));
    }

    inline expression<T>& release()
    {
        math_expr::core::dump_ptr("expression::release", this);
        control_block::destroy(control_block_);

        return (*this);
    }

    ~expression()
    {
        control_block::destroy(control_block_);
    }

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
            if (control_block_)
            {
                if (0 == --control_block_->ref_count)
                {
                    delete control_block_;
                }
            }

            control_block_ = control_block::create(expr);
        }
    }

    inline void register_local_var(expression_ptr expr)
    {
        if (expr)
        {
            if (control_block_)
            {
                control_block_->local_data_list.push_back(
                    typename expression<T>::control_block::data_pack(
                        reinterpret_cast<void*>(expr), control_block::data_type::e_expr));
            }
        }
    }

    inline void register_local_var(vector_holder_ptr vec_holder)
    {
        if (vec_holder)
        {
            if (control_block_)
            {
                control_block_->local_data_list.push_back(
                    typename expression<T>::control_block::data_pack(
                        reinterpret_cast<void*>(vec_holder),
                        control_block::data_type::e_vecholder));
            }
        }
    }

    inline void register_local_data(void* data, const std::size_t& size = 0,
                                    const std::size_t data_mode = 0)
    {
        if (data)
        {
            if (control_block_)
            {
                typename control_block::data_type dt = control_block::data_type::e_data;

                switch (data_mode)
                {
                case 0:
                    dt = control_block::data_type::e_data;
                    break;
                case 1:
                    dt = control_block::data_type::e_vecdata;
                    break;
                case 2:
                    dt = control_block::data_type::e_string;
                    break;
                }

                control_block_->local_data_list.push_back(
                    typename expression<T>::control_block::data_pack(reinterpret_cast<void*>(data),
                                                                     dt, size));
            }
        }
    }

    inline const typename control_block::local_data_list_t& local_data_list()
    {
        if (control_block_)
        {
            return control_block_->local_data_list;
        }
        else
        {
            static typename control_block::local_data_list_t null_local_data_list;
            return null_local_data_list;
        }
    }

    inline void register_return_results(results_context_t* rc)
    {
        if (control_block_ && rc)
        {
            control_block_->results = rc;
        }
    }

    inline void set_retinvk(bool* retinvk_ptr)
    {
        if (control_block_)
        {
            control_block_->return_invoked = retinvk_ptr;
        }
    }

    control_block* control_block_;
    symtab_list_t symbol_table_list_;

    friend class parser<T>;
    friend class expression_helper<T>;
    friend class function_compositor<T>;
    template <typename TT> friend bool is_valid(const expression<TT>& expr);
}; // class expression

} // namespace math_expr

#endif
