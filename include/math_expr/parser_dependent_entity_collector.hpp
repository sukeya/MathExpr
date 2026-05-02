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

#ifndef MATH_EXPR_PARSER_DEPENDENT_ENTITY_COLLECTOR_HPP
#define MATH_EXPR_PARSER_DEPENDENT_ENTITY_COLLECTOR_HPP

#include "math_expr/core/string_utils.hpp"
#include "math_expr/parser_symbol_types.hpp"

namespace math_expr
{

template <typename T>
class parser;

class dependent_entity_collector
{
   public:
    using symbol_t = std::pair<std::string, symbol_type>;
    using symbol_list_t = std::vector<symbol_t>;

    explicit dependent_entity_collector(const collect_types options = collect_types::e_none())
        : options_(options),
          collect_variables_(options_.is_variables()),
          collect_functions_(options_.is_functions()),
          collect_assignments_(options_.is_assignments()),
          return_present_(false),
          final_stmt_return_(false)
    {
    }

    template <typename Allocator, template <typename, typename> class Sequence>
    inline std::size_t symbols(Sequence<symbol_t, Allocator>& symbols_list)
    {
        if (!collect_variables_ && !collect_functions_)
            return 0;
        else if (symbol_name_list_.empty())
            return 0;

        for (std::size_t i = 0; i < symbol_name_list_.size(); ++i)
        {
            core::case_normalise(symbol_name_list_[i].first);
        }

        std::sort(symbol_name_list_.begin(), symbol_name_list_.end());

        std::unique_copy(symbol_name_list_.begin(), symbol_name_list_.end(),
                         std::back_inserter(symbols_list));

        return symbols_list.size();
    }

    template <typename Allocator, template <typename, typename> class Sequence>
    inline std::size_t assignment_symbols(Sequence<symbol_t, Allocator>& assignment_list)
    {
        if (!collect_assignments_)
            return 0;
        else if (assignment_name_list_.empty())
            return 0;

        for (std::size_t i = 0; i < assignment_name_list_.size(); ++i)
        {
            core::case_normalise(assignment_name_list_[i].first);
        }

        std::sort(assignment_name_list_.begin(), assignment_name_list_.end());

        std::unique_copy(assignment_name_list_.begin(), assignment_name_list_.end(),
                         std::back_inserter(assignment_list));

        return assignment_list.size();
    }

    void clear()
    {
        symbol_name_list_.clear();
        assignment_name_list_.clear();
        retparam_list_.clear();
        return_present_ = false;
        final_stmt_return_ = false;
    }

    bool& collect_variables()
    {
        return collect_variables_;
    }

    bool& collect_functions()
    {
        return collect_functions_;
    }

    bool& collect_assignments()
    {
        return collect_assignments_;
    }

    bool return_present() const
    {
        return return_present_;
    }

    bool final_stmt_return() const
    {
        return final_stmt_return_;
    }

    using retparam_list_t = std::vector<std::string>;

    retparam_list_t return_param_type_list() const
    {
        return retparam_list_;
    }

   private:
    inline void add_symbol(const std::string& symbol, const symbol_type st)
    {
        switch (st)
        {
            case symbol_type::e_st_variable:
            case symbol_type::e_st_vector:
            case symbol_type::e_st_string:
            case symbol_type::e_st_local_variable:
            case symbol_type::e_st_local_vector:
            case symbol_type::e_st_local_string:
                if (collect_variables_)
                    symbol_name_list_.push_back(std::make_pair(symbol, st));
                break;

            case symbol_type::e_st_function:
                if (collect_functions_)
                    symbol_name_list_.push_back(std::make_pair(symbol, st));
                break;

            default:
                return;
        }
    }

    inline void add_assignment(const std::string& symbol, const symbol_type st)
    {
        switch (st)
        {
            case symbol_type::e_st_variable:
            case symbol_type::e_st_vector:
            case symbol_type::e_st_string:
                if (collect_assignments_)
                    assignment_name_list_.push_back(std::make_pair(symbol, st));
                break;

            default:
                return;
        }
    }

    collect_types options_;
    bool collect_variables_;
    bool collect_functions_;
    bool collect_assignments_;
    bool return_present_;
    bool final_stmt_return_;
    symbol_list_t symbol_name_list_;
    symbol_list_t assignment_name_list_;
    retparam_list_t retparam_list_;

    template <typename T>
    friend class parser;
};

}  // namespace math_expr

#endif
