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

#ifndef MATH_EXPR_PARSER_HPP
#define MATH_EXPR_PARSER_HPP

#include "math_expr/assert_check.hpp"
#include "math_expr/compilation_check.hpp"
#include "math_expr/core/numeric.hpp"
#include "math_expr/core/operator_types.hpp"
#include "math_expr/function_traits.hpp"
#include "math_expr/ifunction.hpp"
#include "math_expr/igeneric_function.hpp"
#include "math_expr/ivararg_function.hpp"
#include "math_expr/lexer/helper.hpp"
#include "math_expr/lexer/parser_helper.hpp"
#include "math_expr/parser_error.hpp"
#include "math_expr/results_context.hpp"
#include "math_expr/stringvar_base.hpp"
#include "math_expr/symbol_table.hpp"
#include "math_expr/type_store.hpp"
#include "math_expr/vector_view.hpp"
#include "math_expr/expression.hpp"
#include "math_expr/details/conditional_nodes.hpp"
#include "math_expr/details/function_nodes.hpp"
#include "math_expr/details/loop_nodes.hpp"
#include "math_expr/details/node_utils.hpp"
#include "math_expr/details/operator_nodes.hpp"
#include "math_expr/details/return_nodes.hpp"
#include "math_expr/details/vector_nodes.hpp"
#include "math_expr/parser_dependent_entity_collector.hpp"
#include "math_expr/parser/fold_passes.hpp"
#include "math_expr/parser/parser_state.hpp"
#include "math_expr/parser_settings.hpp"
#include "math_expr/parser_symbol_types.hpp"
#include "math_expr/parser/symtab_store.hpp"
#include "math_expr/parser_unknown_symbol_resolver.hpp"
#include "math_expr/parser/expression_table.hpp"
#include "math_expr/parser/rtl_wiring.hpp"
#include "math_expr/parser/scope_manager.hpp"
#include "math_expr/parser/control_flow_parser.hpp"
#include "math_expr/parser/dynamic_function_parser.hpp"
#include "math_expr/parser/entity_parser.hpp"
#include "math_expr/parser/function_call_parser.hpp"
#include "math_expr/parser/range_parser.hpp"
#include "math_expr/parser/sequence_parser.hpp"
#include "math_expr/parser/special_case_parser.hpp"
#include "math_expr/parser/symbol_parser.hpp"
#include "math_expr/parser/symbol_resolution_parser.hpp"
#include "math_expr/parser/string_range_parser.hpp"
#include "math_expr/parser/switch_parser.hpp"
#include "math_expr/parser/vararg_parser.hpp"
#include "math_expr/parser/vector_index_parser.hpp"

namespace math_expr
{

/**
 * @brief Parses and synthesizes mathematical expressions.
 *
 * The template parameter `T` is the numeric value type used consistently
 * throughout the parsed expression. Variables, literals, vector elements,
 * and synthesized expression nodes all use this same `T`.
 *
 * For example, `parser<double>` produces nodes such as
 * `details::assignment_vec_op_node<double, ...>`.
 *
 * @tparam T Numeric value type for the entire expression tree.
 */
template <typename T>
class parser : public lexer::parser_helper
{
   private:
    using cref_t = const T&;
    using const_t = const T;
    using F = ifunction<T>;
    using VAF = ivararg_function<T>;
    using GF = igeneric_function<T>;
    using ifunction_t = ifunction<T>;
    using ivararg_function_t = ivararg_function<T>;
    using igeneric_function_t = igeneric_function<T>;
    using expression_node_t = details::expression_node<T>;
    using literal_node_t = details::literal_node<T>;
    using unary_node_t = details::unary_node<T>;
    using binary_node_t = details::binary_node<T>;
    using trinary_node_t = details::trinary_node<T>;
    using quaternary_node_t = details::quaternary_node<T>;
    using conditional_node_t = details::conditional_node<T>;
    using cons_conditional_node_t = details::cons_conditional_node<T>;
    using while_loop_node_t = details::while_loop_node<T>;
    using repeat_until_loop_node_t = details::repeat_until_loop_node<T>;
    using for_loop_node_t = details::for_loop_node<T>;
    using while_loop_rtc_node_t = details::while_loop_rtc_node<T>;
    using repeat_until_loop_rtc_node_t = details::repeat_until_loop_rtc_node<T>;
    using for_loop_rtc_node_t = details::for_loop_rtc_node<T>;
#ifndef MATH_EXPR_DISABLE_BREAK_CONTINUE
    using while_loop_bc_node_t = details::while_loop_bc_node<T>;
    using repeat_until_loop_bc_node_t = details::repeat_until_loop_bc_node<T>;
    using for_loop_bc_node_t = details::for_loop_bc_node<T>;
    using while_loop_bc_rtc_node_t = details::while_loop_bc_rtc_node<T>;
    using repeat_until_loop_bc_rtc_node_t = details::repeat_until_loop_bc_rtc_node<T>;
    using for_loop_bc_rtc_node_t = details::for_loop_bc_rtc_node<T>;
#endif
    using switch_node_t = details::switch_node<T>;
    using variable_node_t = details::variable_node<T>;
    using vector_elem_node_t = details::vector_elem_node<T>;
    using vector_celem_node_t = details::vector_celem_node<T>;
    using vector_elem_rtc_node_t = details::vector_elem_rtc_node<T>;
    using vector_celem_rtc_node_t = details::vector_celem_rtc_node<T>;
    using rebasevector_elem_node_t = details::rebasevector_elem_node<T>;
    using rebasevector_celem_node_t = details::rebasevector_celem_node<T>;
    using rebasevector_elem_rtc_node_t = details::rebasevector_elem_rtc_node<T>;
    using rebasevector_celem_rtc_node_t = details::rebasevector_celem_rtc_node<T>;
    using vector_node_t = details::vector_node<T>;
    using vector_size_node_t = details::vector_size_node<T>;
    using range_t = details::range_pack<T>;
#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
    using stringvar_node_t = details::string_nodes::stringvar_node<T>;
    using string_literal_node_t = details::string_literal_node<T>;
    using string_range_node_t = details::string_nodes::string_range_node<T>;
    using const_string_range_node_t = details::string_nodes::const_string_range_node<T>;
    using generic_string_range_node_t = details::string_nodes::generic_string_range_node<T>;
    using string_concat_node_t = details::string_nodes::string_concat_node<T>;
    using assignment_string_node_t = details::string_nodes::assignment_string_node<T>;
    using assignment_string_range_node_t = details::string_nodes::assignment_string_range_node<T>;
    using conditional_string_node_t = details::string_nodes::conditional_string_node<T>;
    using cons_conditional_str_node_t = details::string_nodes::cons_conditional_str_node<T>;
#endif
    using assignment_node_t = details::assignment_node<T>;
    using assignment_vec_elem_node_t = details::assignment_vec_elem_node<T>;
    using assignment_vec_elem_rtc_node_t = details::assignment_vec_elem_rtc_node<T>;
    using assignment_rebasevec_elem_node_t = details::assignment_rebasevec_elem_node<T>;
    using assignment_rebasevec_elem_rtc_node_t = details::assignment_rebasevec_elem_rtc_node<T>;
    using assignment_rebasevec_celem_node_t = details::assignment_rebasevec_celem_node<T>;
    using assignment_vec_node_t = details::assignment_vec_node<T>;
    using assignment_vecvec_node_t = details::assignment_vecvec_node<T>;
    using conditional_vector_node_t = details::conditional_vector_node<T>;
    using scand_node_t = details::scand_node<T>;
    using scor_node_t = details::scor_node<T>;
    using token_t = lexer::token;
    using expression_node_ptr = expression_node_t*;
    using expression_t = expression<T>;
    using symbol_table_t = symbol_table<T>;
    using symbol_table_list_t = typename expression<T>::symtab_list_t;
    using symtab_store_t = math_expr::symtab_store<T>;
    using vector_holder_t = details::vector_holder<T>;
    using vector_holder_ptr = vector_holder_t*;

    using functor_t = typename core::numeric::functor_t<T>;
    using quaternary_functor_t = typename functor_t::qfunc_t;
    using trinary_functor_t = typename functor_t::tfunc_t;
    using binary_functor_t = typename functor_t::bfunc_t;
    using unary_functor_t = typename functor_t::ufunc_t;

    using operator_t = core::operators::operator_type;

    using unary_op_map_t = std::map<operator_t, unary_functor_t>;
    using binary_op_map_t = std::map<operator_t, binary_functor_t>;
    using trinary_op_map_t = std::map<operator_t, trinary_functor_t>;

    using sf3_map_t = std::map<std::string, std::pair<trinary_functor_t, operator_t>>;
    using sf4_map_t = std::map<std::string, std::pair<quaternary_functor_t, operator_t>>;

    using inv_binary_op_map_t = std::map<binary_functor_t, operator_t>;
    using base_ops_map_t =
        std::multimap<std::string, core::operators::base_operation_t, core::ilesscompare>;
    using disabled_func_set_t = std::set<std::string, core::ilesscompare>;

    using vov_t = details::T0oT1_define<T, cref_t, cref_t>;
    using cov_t = details::T0oT1_define<T, const_t, cref_t>;
    using voc_t = details::T0oT1_define<T, cref_t, const_t>;

    using vovov_t = details::T0oT1oT2_define<T, cref_t, cref_t, cref_t>;
    using vovoc_t = details::T0oT1oT2_define<T, cref_t, cref_t, const_t>;
    using vocov_t = details::T0oT1oT2_define<T, cref_t, const_t, cref_t>;
    using covov_t = details::T0oT1oT2_define<T, const_t, cref_t, cref_t>;
    using covoc_t = details::T0oT1oT2_define<T, const_t, cref_t, const_t>;
    using cocov_t = details::T0oT1oT2_define<T, const_t, const_t, cref_t>;
    using vococ_t = details::T0oT1oT2_define<T, cref_t, const_t, const_t>;

    using vovovov_t = details::T0oT1oT2oT3_define<T, cref_t, cref_t, cref_t, cref_t>;
    using vovovoc_t = details::T0oT1oT2oT3_define<T, cref_t, cref_t, cref_t, const_t>;
    using vovocov_t = details::T0oT1oT2oT3_define<T, cref_t, cref_t, const_t, cref_t>;
    using vocovov_t = details::T0oT1oT2oT3_define<T, cref_t, const_t, cref_t, cref_t>;
    using covovov_t = details::T0oT1oT2oT3_define<T, const_t, cref_t, cref_t, cref_t>;

    using covocov_t = details::T0oT1oT2oT3_define<T, const_t, cref_t, const_t, cref_t>;
    using vocovoc_t = details::T0oT1oT2oT3_define<T, cref_t, const_t, cref_t, const_t>;
    using covovoc_t = details::T0oT1oT2oT3_define<T, const_t, cref_t, cref_t, const_t>;
    using vococov_t = details::T0oT1oT2oT3_define<T, cref_t, const_t, const_t, cref_t>;

    using results_context_t = results_context<T>;
    using expression_table_t = math_expr::expression_table<token_t>;
    using precedence_level = typename expression_table_t::precedence_level;
    using expression_state_t = typename expression_table_t::state_t;
    static constexpr precedence_level default_precedence = precedence_level::e_level00;
    using scope_element = math_expr::scope_element<T>;
    using scope_element_manager = math_expr::scope_element_manager<T>;
    using scope_handler = math_expr::scope_handler<T>;

    using prsrhlpr_t = parser_helper;

    template <typename T_>
    struct halfopen_range_policy
    {
        static inline bool is_within(const T_& v, const T_& begin, const T_& end)
        {
            assert(begin <= end);
            return (begin <= v) && (v < end);
        }

        static inline bool is_less(const T_& v, const T_& begin)
        {
            return (v < begin);
        }

        static inline bool is_greater(const T_& v, const T_& end)
        {
            return (end <= v);
        }

        static inline bool end_inclusive()
        {
            return false;
        }
    };

    template <typename T_>
    struct closed_range_policy
    {
        static inline bool is_within(const T_& v, const T_& begin, const T_& end)
        {
            assert(begin <= end);
            return (begin <= v) && (v <= end);
        }

        static inline bool is_less(const T_& v, const T_& begin)
        {
            return (v < begin);
        }

        static inline bool is_greater(const T_& v, const T_& end)
        {
            return (end < v);
        }

        static inline bool end_inclusive()
        {
            return true;
        }
    };

    template <typename IntervalPointType,
              typename RangePolicy = halfopen_range_policy<IntervalPointType>>
    class interval_container_t
    {
       public:
        using interval_point_t = IntervalPointType;
        using interval_t = std::pair<interval_point_t, interval_point_t>;
        using interval_map_t = std::map<interval_point_t, interval_t>;
        using interval_map_citr_t = typename interval_map_t::const_iterator;

        std::size_t size() const
        {
            return interval_map_.size();
        }

        void reset()
        {
            interval_map_.clear();
        }

        bool in_interval(const interval_point_t point, interval_t& interval) const
        {
            interval_map_citr_t itr = RangePolicy::end_inclusive()
                                          ? interval_map_.lower_bound(point)
                                          : interval_map_.upper_bound(point);

            for (; itr != interval_map_.end(); ++itr)
            {
                const interval_point_t& begin = itr->second.first;
                const interval_point_t& end = itr->second.second;

                if (RangePolicy::is_within(point, begin, end))
                {
                    interval = interval_t(begin, end);
                    return true;
                }
                else if (RangePolicy::is_greater(point, end))
                {
                    break;
                }
            }

            return false;
        }

        bool in_interval(const interval_point_t point) const
        {
            interval_t interval;
            return in_interval(point, interval);
        }

        bool add_interval(const interval_point_t begin, const interval_point_t end)
        {
            if ((end <= begin) || in_interval(begin) || in_interval(end))
            {
                return false;
            }

            interval_map_[end] = std::make_pair(begin, end);

            return true;
        }

        bool add_interval(const interval_t interval)
        {
            return add_interval(interval.first, interval.second);
        }

       private:
        interval_map_t interval_map_;
    };

    class stack_limit_handler
    {
       public:
        using parser_t = parser<T>;

        explicit stack_limit_handler(parser<T>& p) : parser_(p), limit_exceeded_(false)
        {
            if (++parser_.state_.stack_depth > parser_.settings_.max_stack_depth_)
            {
                limit_exceeded_ = true;
                parser_.set_error(make_error(parser_error::error_mode::e_parser,
                                             "ERR000 - Current stack depth " +
                                                 core::to_str(parser_.state_.stack_depth) +
                                                 " exceeds maximum allowed stack depth of " +
                                                 core::to_str(parser_.settings_.max_stack_depth_),
                                             core::error_location()));
            }
        }

        ~stack_limit_handler()
        {
            assert(parser_.state_.stack_depth > 0);
            parser_.state_.stack_depth--;
        }

        bool operator!()
        {
            return limit_exceeded_;
        }

       private:
        stack_limit_handler(const stack_limit_handler&) = delete;
        stack_limit_handler& operator=(const stack_limit_handler&) = delete;

        parser_t& parser_;
        bool limit_exceeded_;
    };

   public:
    using unknown_symbol_resolver = math_expr::unknown_symbol_resolver<T>;
    using collect_type = math_expr::collect_types;
    using symbol_type = math_expr::symbol_type;
    using dependent_entity_collector = math_expr::dependent_entity_collector;
    using settings_store = math_expr::settings_store<T>;

    using settings_t = settings_store;

    explicit parser(const settings_t& settings = settings_t())
        : settings_(settings),
          resolve_unknown_symbol_(false),
          results_context_(nullptr),
          unknown_symbol_resolver_(nullptr),
          sem_(),
          operator_joiner_2_(2),
          operator_joiner_3_(3),
          loop_runtime_check_(nullptr),
          vector_access_runtime_check_(nullptr),
          compilation_check_ptr_(nullptr),
          assert_check_(nullptr)
    {
        sem_.set_scope_depth(state_.scope_depth);
        init_precompilation();

        math_expr::rtl_wiring<T>::load(base_ops_map_, unary_op_map_, binary_op_map_,
                                       inv_binary_op_map_, sf3_map_, sf4_map_);

        expression_generator_.init_synthesize_map();
        expression_generator_.set_parser(*this);
        expression_generator_.set_uom(unary_op_map_);
        expression_generator_.set_bom(binary_op_map_);
        expression_generator_.set_ibom(inv_binary_op_map_);
        expression_generator_.set_sf3m(sf3_map_);
        expression_generator_.set_sf4m(sf4_map_);
        expression_generator_.set_strength_reduction_state(settings_.strength_reduction_enabled());
    }

    ~parser() {}

    inline void init_precompilation()
    {
        dec_.collect_variables() = settings_.collect_variables_enabled();

        dec_.collect_functions() = settings_.collect_functions_enabled();

        dec_.collect_assignments() = settings_.collect_assignments_enabled();

        if (settings_.replacer_enabled())
        {
            symbol_replacer_.clear();
            helper_assembly_.token_modifier_list.clear();
            helper_assembly_.register_modifier(&symbol_replacer_);
        }

        if (settings_.commutative_check_enabled())
        {
            for (std::size_t i = 0; i < core::reserved_words_size; ++i)
            {
                commutative_inserter_.ignore_symbol(std::string(core::reserved_words[i]));
            }

            helper_assembly_.token_inserter_list.clear();
            helper_assembly_.register_inserter(&commutative_inserter_);
        }

        if (settings_.joiner_enabled())
        {
            helper_assembly_.token_joiner_list.clear();
            helper_assembly_.register_joiner(&operator_joiner_2_);
            helper_assembly_.register_joiner(&operator_joiner_3_);
        }

        if (settings_.numeric_check_enabled() || settings_.bracket_check_enabled() ||
            settings_.sequence_check_enabled())
        {
            helper_assembly_.token_scanner_list.clear();

            if (settings_.numeric_check_enabled())
            {
                helper_assembly_.register_scanner(&numeric_checker_);
            }

            if (settings_.bracket_check_enabled())
            {
                helper_assembly_.register_scanner(&bracket_checker_);
            }

            if (settings_.sequence_check_enabled())
            {
                helper_assembly_.register_scanner(&sequence_validator_);
                helper_assembly_.register_scanner(&sequence_validator_3tkns_);
            }
        }
    }

    inline bool compile(const std::string& expression_string, expression<T>& expr)
    {
        state_.reset();
        error_list_.clear();
        brkcnt_list_.clear();
        synthesis_error_.clear();
        immutable_memory_map_.reset();
        immutable_symtok_map_.clear();
        current_state_stack_.clear();
        assert_ids_.clear();
        sem_.cleanup();

        return_cleanup();

        if (!valid_settings())
        {
            return false;
        }

        expression_generator_.set_allocator(node_allocator_);

        if (expression_string.empty())
        {
            set_error(make_error(parser_error::error_mode::e_syntax, "ERR001 - Empty expression!",
                                 core::error_location()));

            return false;
        }

        if (!init(expression_string))
        {
            process_lexer_errors();
            return false;
        }

        if (lexer().empty())
        {
            set_error(make_error(parser_error::error_mode::e_syntax, "ERR002 - Empty expression!",
                                 core::error_location()));

            return false;
        }

        if (halt_compilation_check())
        {
            core::debug_print("halt_compilation_check() - compile checkpoint 0\n");
            sem_.cleanup();
            return false;
        }

        if (!run_assemblies())
        {
            sem_.cleanup();
            return false;
        }

        if (halt_compilation_check())
        {
            core::debug_print("halt_compilation_check() - compile checkpoint 1\n");
            sem_.cleanup();
            return false;
        }

        symtab_store_.symtab_list_ = expr.get_symbol_table_list();
        dec_.clear();

        lexer().begin();

        next_token();

        expression_node_ptr e = parse_corpus();

        if ((nullptr != e) && (token_t::e_eof == current_token().type))
        {
            bool* retinvk_ptr = nullptr;

            if (state_.return_stmt_present)
            {
                dec_.return_present_ = true;

                e = expression_generator_.return_envelope(e, results_context_, retinvk_ptr);
            }

            expr.set_expression(e);
            expr.set_retinvk(retinvk_ptr);

            register_local_vars(expr);
            register_return_results(expr);

            return !(!expr);
        }
        else
        {
            if (error_list_.empty())
            {
                set_error(make_error(parser_error::error_mode::e_syntax, current_token(),
                                     "ERR003 - Invalid expression encountered",
                                     core::error_location()));
            }

            if ((nullptr != e) && branch_deletable(e))
            {
                destroy_node(e);
            }

            dec_.clear();
            sem_.cleanup();
            return_cleanup();
            expr = expression_t();

            return false;
        }
    }

    inline expression_t compile(const std::string& expression_string, symbol_table_t& symtab)
    {
        expression_t expression;
        expression.register_symbol_table(symtab);
        compile(expression_string, expression);
        return expression;
    }

    void process_lexer_errors()
    {
        for (std::size_t i = 0; i < lexer().size(); ++i)
        {
            if (lexer()[i].is_error())
            {
                std::string diagnostic = "ERR004 - ";

                switch (lexer()[i].type)
                {
                    case lexer::token::e_error:
                        diagnostic += "General token error";
                        break;

                    case lexer::token::e_err_symbol:
                        diagnostic += "Symbol error";
                        break;

                    case lexer::token::e_err_number:
                        diagnostic += "Invalid numeric token";
                        break;

                    case lexer::token::e_err_string:
                        diagnostic += "Invalid string token";
                        break;

                    case lexer::token::e_err_sfunc:
                        diagnostic += "Invalid special function token";
                        break;

                    default:
                        diagnostic += "Unknown compiler error";
                }

                set_error(make_error(parser_error::error_mode::e_lexer, lexer()[i],
                                     diagnostic + ": " + lexer()[i].value, core::error_location()));
            }
        }
    }

    inline bool run_assemblies()
    {
        if (settings_.commutative_check_enabled())
        {
            helper_assembly_.run_inserters(lexer());
        }

        if (settings_.joiner_enabled())
        {
            helper_assembly_.run_joiners(lexer());
        }

        if (settings_.replacer_enabled())
        {
            helper_assembly_.run_modifiers(lexer());
        }

        if (settings_.numeric_check_enabled() || settings_.bracket_check_enabled() ||
            settings_.sequence_check_enabled())
        {
            if (!helper_assembly_.run_scanners(lexer()))
            {
                if (helper_assembly_.error_token_scanner)
                {
                    using scanner_kind = lexer::token_scanner::scanner_kind;
                    const scanner_kind sk = helper_assembly_.error_token_scanner->kind();

                    if (sk == scanner_kind::bracket)
                    {
                        auto* bracket_checker_ptr = static_cast<lexer::helper::bracket_checker*>(
                            helper_assembly_.error_token_scanner);
                        set_error(make_error(parser_error::error_mode::e_token,
                                             bracket_checker_ptr->error_token(),
                                             "ERR005 - Mismatched brackets: '" +
                                                 bracket_checker_ptr->error_token().value + "'",
                                             core::error_location()));
                    }
                    else if (sk == scanner_kind::numeric)
                    {
                        auto* numeric_checker_ptr = static_cast<lexer::helper::numeric_checker<T>*>(
                            helper_assembly_.error_token_scanner);
                        for (std::size_t i = 0; i < numeric_checker_ptr->error_count(); ++i)
                        {
                            lexer::token error_token = lexer()[numeric_checker_ptr->error_index(i)];

                            set_error(make_error(
                                parser_error::error_mode::e_token, error_token,
                                "ERR006 - Invalid numeric token: '" + error_token.value + "'",
                                core::error_location()));
                        }

                        if (numeric_checker_ptr->error_count())
                        {
                            numeric_checker_ptr->clear_errors();
                        }
                    }
                    else if (sk == scanner_kind::sequence)
                    {
                        auto* sequence_validator_ptr =
                            static_cast<lexer::helper::sequence_validator*>(
                                helper_assembly_.error_token_scanner);
                        for (std::size_t i = 0; i < sequence_validator_ptr->error_count(); ++i)
                        {
                            std::pair<lexer::token, lexer::token> error_token =
                                sequence_validator_ptr->error(i);

                            set_error(make_error(
                                parser_error::error_mode::e_token, error_token.first,
                                "ERR007 - Invalid token sequence: '" + error_token.first.value +
                                    "' and '" + error_token.second.value + "'",
                                core::error_location()));
                        }

                        if (sequence_validator_ptr->error_count())
                        {
                            sequence_validator_ptr->clear_errors();
                        }
                    }
                    else if (sk == scanner_kind::sequence_3tokens)
                    {
                        auto* sequence_validator3_ptr =
                            static_cast<lexer::helper::sequence_validator_3tokens*>(
                                helper_assembly_.error_token_scanner);
                        for (std::size_t i = 0; i < sequence_validator3_ptr->error_count(); ++i)
                        {
                            std::pair<lexer::token, lexer::token> error_token =
                                sequence_validator3_ptr->error(i);

                            set_error(make_error(
                                parser_error::error_mode::e_token, error_token.first,
                                "ERR008 - Invalid token sequence: '" + error_token.first.value +
                                    "' and '" + error_token.second.value + "'",
                                core::error_location()));
                        }

                        if (sequence_validator3_ptr->error_count())
                        {
                            sequence_validator3_ptr->clear_errors();
                        }
                    }
                }

                return false;
            }
        }

        return true;
    }

    inline settings_store& settings()
    {
        return settings_;
    }

    inline parser_error::type get_error(const std::size_t& index) const
    {
        if (index < error_list_.size())
        {
            return error_list_[index];
        }

        throw std::invalid_argument("parser::get_error() - Invalid error index specified");
    }

    inline std::string error() const
    {
        if (!error_list_.empty())
        {
            return error_list_[0].diagnostic;
        }
        else
            return std::string("No Error");
    }

    inline std::size_t error_count() const
    {
        return error_list_.size();
    }

    inline dependent_entity_collector& dec()
    {
        return dec_;
    }

    inline std::size_t total_local_symbol_size_bytes() const
    {
        return sem_.total_local_symb_size_bytes();
    }

    inline bool replace_symbol(const std::string& old_symbol, const std::string& new_symbol)
    {
        if (!settings_.replacer_enabled())
            return false;
        else if (core::is_reserved_word(old_symbol))
            return false;
        else
            return symbol_replacer_.add_replace(old_symbol, new_symbol, lexer::token::e_symbol);
    }

    inline bool remove_replace_symbol(const std::string& symbol)
    {
        if (!settings_.replacer_enabled())
            return false;
        else if (core::is_reserved_word(symbol))
            return false;
        else
            return symbol_replacer_.remove(symbol);
    }

    inline void enable_unknown_symbol_resolver(unknown_symbol_resolver* usr = nullptr)
    {
        resolve_unknown_symbol_ = true;

        if (usr)
            unknown_symbol_resolver_ = usr;
        else
            unknown_symbol_resolver_ = &default_usr_;
    }

    inline void enable_unknown_symbol_resolver(unknown_symbol_resolver& usr)
    {
        enable_unknown_symbol_resolver(&usr);
    }

    inline void disable_unknown_symbol_resolver()
    {
        resolve_unknown_symbol_ = false;
        unknown_symbol_resolver_ = &default_usr_;
    }

    inline void register_loop_runtime_check(loop_runtime_check& lrtchk)
    {
        loop_runtime_check_ = &lrtchk;
    }

    inline void register_vector_access_runtime_check(vector_access_runtime_check<T>& vartchk)
    {
        vector_access_runtime_check_ = &vartchk;
    }

    inline void register_compilation_timeout_check(compilation_check& compchk)
    {
        compilation_check_ptr_ = &compchk;
    }

    inline void register_assert_check(assert_check& assrt_chck)
    {
        assert_check_ = &assrt_chck;
    }

    inline void clear_loop_runtime_check()
    {
        loop_runtime_check_ = nullptr;
    }

    inline void clear_vector_access_runtime_check()
    {
        vector_access_runtime_check_ = nullptr;
    }

    inline void clear_compilation_timeout_check()
    {
        compilation_check_ptr_ = nullptr;
    }

    inline void clear_assert_check()
    {
        assert_check_ = nullptr;
    }

   private:
    inline bool valid_base_operation(const std::string& symbol) const
    {
        const std::size_t length = symbol.size();

        if ((length < 3) ||  // Shortest base op symbol length
            (length > 9)     // Longest base op symbol length
        )
            return false;
        else
            return settings_.function_enabled(symbol) &&
                   (base_ops_map_.end() != base_ops_map_.find(symbol));
    }

    inline bool valid_vararg_operation(const std::string& symbol) const
    {
        static constexpr std::string_view s_sum = "sum";
        static constexpr std::string_view s_mul = "mul";
        static constexpr std::string_view s_avg = "avg";
        static constexpr std::string_view s_min = "min";
        static constexpr std::string_view s_max = "max";
        static constexpr std::string_view s_mand = "mand";
        static constexpr std::string_view s_mor = "mor";
        static constexpr std::string_view s_multi = "~";
        static constexpr std::string_view s_mswitch = "[*]";

        return (core::imatch(symbol, s_sum) || core::imatch(symbol, s_mul) ||
                core::imatch(symbol, s_avg) || core::imatch(symbol, s_min) ||
                core::imatch(symbol, s_max) || core::imatch(symbol, s_mand) ||
                core::imatch(symbol, s_mor) || core::imatch(symbol, s_multi) ||
                core::imatch(symbol, s_mswitch)) &&
               settings_.function_enabled(symbol);
    }

    bool is_invalid_logic_operation(const core::operators::operator_type operation) const
    {
        return settings_.logic_disabled(operation);
    }

    bool is_invalid_arithmetic_operation(const core::operators::operator_type operation) const
    {
        return settings_.arithmetic_disabled(operation);
    }

    bool is_invalid_assignment_operation(const core::operators::operator_type operation) const
    {
        return settings_.assignment_disabled(operation);
    }

    bool is_invalid_inequality_operation(const core::operators::operator_type operation) const
    {
        return settings_.inequality_disabled(operation);
    }

    inline void next_token()
    {
        std::string ct_str;
        std::size_t ct_pos = 0;

        if constexpr (::math_expr::core::build_options::kEnableDebugging)
        {
            ct_str = current_token().value;
            ct_pos = current_token().position;
        }

        parser_helper::next_token();
        if constexpr (::math_expr::core::build_options::kEnableDebugging)
        {
            const std::string depth(2 * state_.scope_depth, ' ');
            core::debug_print(
                "%s"
                "prev[%s | %04d] --> curr[%s | %04d]  stack_level: %3d\n",
                depth.c_str(), ct_str.c_str(), static_cast<unsigned int>(ct_pos),
                current_token().value.c_str(), static_cast<unsigned int>(current_token().position),
                static_cast<unsigned int>(state_.stack_depth));
        }
    }

    inline expression_node_ptr parse_corpus()
    {
        std::vector<expression_node_ptr> arg_list;
        std::vector<bool> side_effect_list;

        scoped_vec_delete<expression_node_t> svd((*this), arg_list);

        lexer::token begin_token;
        lexer::token end_token;

        for (;;)
        {
            state_.side_effect_present = false;

            begin_token = current_token();

            expression_node_ptr arg = parse_expression();

            if (nullptr == arg)
            {
                if (error_list_.empty())
                {
                    set_error(make_error(parser_error::error_mode::e_syntax, current_token(),
                                         "ERR009 - Invalid expression encountered",
                                         core::error_location()));
                }

                return error_node();
            }
            else
            {
                arg_list.push_back(arg);

                side_effect_list.push_back(state_.side_effect_present);

                end_token = current_token();

                const std::string sub_expr = construct_subexpr(begin_token, end_token);

                core::debug_print("parse_corpus(%02d) Subexpr: %s\n",
                                  static_cast<int>(arg_list.size() - 1), sub_expr.c_str());

                core::debug_print("parse_corpus(%02d) - Side effect present: %s\n",
                                  static_cast<int>(arg_list.size() - 1),
                                  state_.side_effect_present ? "true" : "false");

                core::debug_print("-------------------------------------------------\n");
            }

            if (token_is(token_t::e_eof, prsrhlpr_t::token_advance_mode::e_hold))
            {
                if (lexer().finished())
                    break;
                else
                    next_token();
            }
            else if (!settings_.commutative_check_enabled() &&
                     (current_token().type == token_t::e_symbol ||
                      current_token().type == token_t::e_number ||
                      current_token().type == token_t::e_string ||
                      token_is_bracket(prsrhlpr_t::token_advance_mode::e_hold)))
            {
                set_error(make_error(parser_error::error_mode::e_syntax, current_token(),
                                     "ERR010 - Invalid syntax '" + current_token().value +
                                         "' possible missing operator or context",
                                     core::error_location()));

                return error_node();
            }
        }

        if (!arg_list.empty() && is_return_node(arg_list.back()))
        {
            dec_.final_stmt_return_ = true;
        }

        const expression_node_ptr result = simplify(arg_list, side_effect_list);

        svd.delete_ptr = (nullptr == result);

        return result;
    }

    std::string construct_subexpr(lexer::token& begin_token, lexer::token& end_token,
                                  const bool cleanup_whitespace = true)
    {
        std::string result = lexer().substr(begin_token.position, end_token.position);
        if (cleanup_whitespace)
        {
            for (std::size_t i = 0; i < result.size(); ++i)
            {
                if (core::is_whitespace(result[i]))
                    result[i] = ' ';
            }
        }

        return result;
    }

    inline void push_current_state(const expression_state_t current_state)
    {
        current_state_stack_.push_back(current_state);
    }

    inline void pop_current_state()
    {
        if (!current_state_stack_.empty())
        {
            current_state_stack_.pop_back();
        }
    }

    inline expression_state_t current_state() const
    {
        return (!current_state_stack_.empty()) ? current_state_stack_.back() : expression_state_t();
    }

    inline bool halt_compilation_check()
    {
        compilation_check::compilation_context context;

        if (compilation_check_ptr_ && !compilation_check_ptr_->continue_compilation(context))
        {
            const std::string error_message =
                !context.error_message.empty() ? " Details: " + context.error_message : "";

            set_error(make_error(parser_error::error_mode::e_parser, token_t(),
                                 "ERR011 - Internal compilation check failed." + error_message,
                                 core::error_location()));

            return true;
        }

        return false;
    }

    inline expression_node_ptr parse_expression(
        precedence_level precedence = precedence_level::e_level00)
    {
        if (halt_compilation_check())
        {
            core::debug_print("halt_compilation_check() - parse_expression checkpoint 2\n");
            return error_node();
        }

        stack_limit_handler slh(*this);

        if (!slh)
        {
            return error_node();
        }

        expression_node_ptr expression = parse_branch(precedence);

        if (nullptr == expression)
        {
            return error_node();
        }

        if (token_is(token_t::e_eof, prsrhlpr_t::token_advance_mode::e_hold))
        {
            return expression;
        }

        bool break_loop = false;

        expression_state_t current_state;

        for (;;)
        {
            break_loop = !expression_table_t::resolve(current_token(), current_state);

            if (break_loop)
            {
                parse_pending_string_rangesize(expression);
                break;
            }
            else if (current_state.left < precedence)
                break;

            const lexer::token prev_token = current_token();

            next_token();

            expression_node_ptr right_branch = error_node();
            expression_node_ptr new_expression = error_node();

            if (is_invalid_logic_operation(current_state.operation))
            {
                free_node(node_allocator_, expression);

                set_error(make_error(parser_error::error_mode::e_syntax, prev_token,
                                     "ERR012 - Invalid or disabled logic operation '" +
                                         core::operators::to_str(current_state.operation) + "'",
                                     core::error_location()));

                return error_node();
            }
            else if (is_invalid_arithmetic_operation(current_state.operation))
            {
                free_node(node_allocator_, expression);

                set_error(make_error(parser_error::error_mode::e_syntax, prev_token,
                                     "ERR013 - Invalid or disabled arithmetic operation '" +
                                         core::operators::to_str(current_state.operation) + "'",
                                     core::error_location()));

                return error_node();
            }
            else if (is_invalid_inequality_operation(current_state.operation))
            {
                free_node(node_allocator_, expression);

                set_error(make_error(parser_error::error_mode::e_syntax, prev_token,
                                     "ERR014 - Invalid inequality operation '" +
                                         core::operators::to_str(current_state.operation) + "'",
                                     core::error_location()));

                return error_node();
            }
            else if (is_invalid_assignment_operation(current_state.operation))
            {
                free_node(node_allocator_, expression);

                set_error(make_error(parser_error::error_mode::e_syntax, prev_token,
                                     "ERR015 - Invalid or disabled assignment operation '" +
                                         core::operators::to_str(current_state.operation) + "'",
                                     core::error_location()));

                return error_node();
            }

            if (nullptr != (right_branch = parse_expression(current_state.right)))
            {
                if (details::is_return_node(expression) || details::is_return_node(right_branch))
                {
                    free_node(node_allocator_, expression);
                    free_node(node_allocator_, right_branch);

                    set_error(
                        make_error(parser_error::error_mode::e_syntax, prev_token,
                                   "ERR016 - Return statements cannot be part of sub-expressions",
                                   core::error_location()));

                    return error_node();
                }

                push_current_state(current_state);

                new_expression =
                    expression_generator_(current_state.operation, expression, right_branch);

                pop_current_state();
            }

            if (nullptr == new_expression)
            {
                if (error_list_.empty())
                {
                    set_error(make_error(
                        parser_error::error_mode::e_syntax, prev_token,
                        !synthesis_error_.empty()
                            ? synthesis_error_
                            : "ERR017 - General parsing error at token: '" + prev_token.value + "'",
                        core::error_location()));
                }

                free_node(node_allocator_, expression);
                free_node(node_allocator_, right_branch);

                return error_node();
            }
            else
            {
                if (token_is(token_t::e_ternary, prsrhlpr_t::token_advance_mode::e_hold) &&
                    (precedence_level::e_level00 == precedence))
                {
                    expression = parse_ternary_conditional_statement(new_expression);
                }
                else
                    expression = new_expression;

                parse_pending_string_rangesize(expression);
            }
        }

        if ((nullptr != expression) && (expression->node_depth() > settings_.max_node_depth_))
        {
            set_error(make_error(parser_error::error_mode::e_syntax, current_token(),
                                 "ERR018 - Expression depth of " +
                                     core::to_str(static_cast<int>(expression->node_depth())) +
                                     " exceeds maximum allowed expression depth of " +
                                     core::to_str(static_cast<int>(settings_.max_node_depth_)),
                                 core::error_location()));

            free_node(node_allocator_, expression);

            return error_node();
        }
        else if (!settings_.commutative_check_enabled() &&
                 !core::is_logic_opr(current_token().value) &&
                 (current_state.operation == core::operators::operator_type::default_op) &&
                 (current_token().type == token_t::e_symbol ||
                  current_token().type == token_t::e_number ||
                  current_token().type == token_t::e_string))
        {
            set_error(make_error(parser_error::error_mode::e_syntax, current_token(),
                                 "ERR019 - Invalid syntax '" + current_token().value +
                                     "' possible missing operator or context",
                                 core::error_location()));

            free_node(node_allocator_, expression);

            return error_node();
        }

        return expression;
    }

    bool simplify_unary_negation_branch(expression_node_ptr& node)
    {
        {
            using ubn_t = details::unary_branch_node<T, details::neg_op<T>>;
            ubn_t* n = (node->type() == details::expression_node<T>::node_type::e_neg &&
                        !node->as_uv_base_node())
                           ? static_cast<ubn_t*>(node)
                           : nullptr;

            if (n)
            {
                expression_node_ptr un_r = n->branch(0);
                n->release();
                free_node(node_allocator_, node);
                node = un_r;

                return true;
            }
        }

        {
            using uvn_t = details::unary_variable_node<T, details::neg_op<T>>;

            uvn_t* n = (node->type() == details::expression_node<T>::node_type::e_neg &&
                        node->as_uv_base_node())
                           ? static_cast<uvn_t*>(node)
                           : nullptr;

            if (n)
            {
                const T& v = n->v();
                expression_node_ptr return_node = error_node();

                if ((nullptr != (return_node = symtab_store_.get_variable(v))) ||
                    (nullptr != (return_node = sem_.get_variable(v))))
                {
                    free_node(node_allocator_, node);
                    node = return_node;

                    return true;
                }
                else
                {
                    set_error(make_error(parser_error::error_mode::e_syntax, current_token(),
                                         "ERR020 - Failed to find variable node in symbol table",
                                         core::error_location()));

                    free_node(node_allocator_, node);

                    return false;
                }
            }
        }

        return false;
    }

    static inline expression_node_ptr error_node()
    {
        return nullptr;
    }

    struct scoped_expression_delete
    {
        scoped_expression_delete(parser<T>& pr, expression_node_ptr& expression)
            : delete_ptr(true), parser_(pr), expression_(expression)
        {
        }

        ~scoped_expression_delete()
        {
            if (delete_ptr)
            {
                free_node(parser_.node_allocator_, expression_);
            }
        }

        bool delete_ptr;
        parser<T>& parser_;
        expression_node_ptr& expression_;

       private:
        scoped_expression_delete(const scoped_expression_delete&) = delete;
        scoped_expression_delete& operator=(const scoped_expression_delete&) = delete;
    };

    /**
     * @brief RAII helper that releases parser-owned node pointers on scope exit.
     *
     * This helper is nested in `parser<T>`, so it operates within the parser
     * instance whose numeric value type is `T`. The `Type` parameter denotes
     * the concrete node type being managed.
     *
     * @tparam Type Concrete node type stored by the parser.
     * @tparam N Number of pointers managed by this guard.
     */
    template <typename Type, std::size_t N>
    struct scoped_delete
    {
        using ptr_t = Type*;

        scoped_delete(parser<T>& pr, ptr_t& p) : delete_ptr(true), parser_(pr), p_(&p) {}

        scoped_delete(parser<T>& pr, ptr_t (&p)[N]) : delete_ptr(true), parser_(pr), p_(&p[0]) {}

        ~scoped_delete()
        {
            if (delete_ptr)
            {
                for (std::size_t i = 0; i < N; ++i)
                {
                    free_node(parser_.node_allocator_, p_[i]);
                }
            }
        }

        bool delete_ptr;
        parser<T>& parser_;
        ptr_t* p_;

       private:
        scoped_delete(const scoped_delete<Type, N>&) = delete;
        scoped_delete<Type, N>& operator=(const scoped_delete<Type, N>&) = delete;
    };

    /**
     * @brief RAII helper that releases a deque of parser-owned node pointers.
     *
     * This helper belongs to `parser<T>`, where `T` is the numeric value type
     * used by the surrounding parser and its synthesized expression nodes.
     * `Type` denotes the concrete node type stored in the deque.
     *
     * @tparam Type Concrete node type stored by the parser.
     */
    template <typename Type>
    struct scoped_deq_delete
    {
        using ptr_t = Type*;

        scoped_deq_delete(parser<T>& pr, std::deque<ptr_t>& deq)
            : delete_ptr(true), parser_(pr), deq_(deq)
        {
        }

        ~scoped_deq_delete()
        {
            if (delete_ptr && !deq_.empty())
            {
                for (std::size_t i = 0; i < deq_.size(); ++i)
                {
                    core::debug_print("~scoped_deq_delete() - deleting node: %p\n",
                                      static_cast<const void*>(deq_[i]));
                    free_node(parser_.node_allocator_, deq_[i]);
                }

                deq_.clear();
            }
        }

        bool delete_ptr;
        parser<T>& parser_;
        std::deque<ptr_t>& deq_;

       private:
        scoped_deq_delete(const scoped_deq_delete<Type>&) = delete;
        scoped_deq_delete<Type>& operator=(const scoped_deq_delete<Type>&) = delete;
    };

    /**
     * @brief RAII helper that releases a vector of parser-owned node pointers.
     *
     * This helper belongs to `parser<T>`, where `T` is the numeric value type
     * used by the surrounding parser and its synthesized expression nodes.
     * `Type` denotes the concrete node type stored in the vector.
     *
     * @tparam Type Concrete node type stored by the parser.
     */
    template <typename Type>
    struct scoped_vec_delete
    {
        using ptr_t = Type*;

        scoped_vec_delete(parser<T>& pr, std::vector<ptr_t>& vec)
            : delete_ptr(true), parser_(pr), vec_(vec)
        {
        }

        ~scoped_vec_delete()
        {
            if (delete_ptr && !vec_.empty())
            {
                for (std::size_t i = 0; i < vec_.size(); ++i)
                {
                    core::debug_print("~scoped_vec_delete() - deleting node: %p\n",
                                      static_cast<const void*>(vec_[i]));
                    free_node(parser_.node_allocator_, vec_[i]);
                }

                vec_.clear();
            }
        }

        ptr_t operator[](const std::size_t index)
        {
            return vec_[index];
        }

        bool delete_ptr;
        parser<T>& parser_;
        std::vector<ptr_t>& vec_;

       private:
        scoped_vec_delete(const scoped_vec_delete<Type>&) = delete;
        scoped_vec_delete<Type>& operator=(const scoped_vec_delete<Type>&) = delete;
    };

    struct scoped_bool_negator
    {
        explicit scoped_bool_negator(bool& bb) : b(bb)
        {
            b = !b;
        }

        ~scoped_bool_negator()
        {
            b = !b;
        }

        bool& b;
    };

    struct scoped_bool_or_restorer
    {
        explicit scoped_bool_or_restorer(bool& bb) : b(bb), original_value_(bb) {}

        ~scoped_bool_or_restorer()
        {
            b = b || original_value_;
        }

        bool& b;
        bool original_value_;
    };

    struct scoped_inc_dec
    {
        explicit scoped_inc_dec(std::size_t& v) : v_(v)
        {
            ++v_;
        }

        ~scoped_inc_dec()
        {
            assert(v_ > 0);
            --v_;
        }

        std::size_t& v_;
    };

    struct control_flow_context
    {
        using token_advance_mode = typename prsrhlpr_t::token_advance_mode;

        explicit control_flow_context(parser<T>& parser)
            : parser_(parser),
              settings(parser.settings_),
              state(parser.state_),
              sem(parser.sem_),
              symtab_store(parser.symtab_store_),
              brkcnt_list(parser.brkcnt_list_),
              node_allocator(parser.node_allocator_)
        {
        }

        inline const token_t& current_token() const
        {
            return parser_.current_token();
        }

        inline void next_token()
        {
            parser_.next_token();
        }

        inline bool token_is(const token_t::token_type type,
                             const token_advance_mode mode = token_advance_mode::e_advance)
        {
            return parser_.token_is(type, mode);
        }

        inline bool token_is(const std::string& symbol,
                             const token_advance_mode mode = token_advance_mode::e_advance)
        {
            return parser_.token_is(symbol, mode);
        }

        inline bool token_is_loop(const token_advance_mode mode = token_advance_mode::e_advance)
        {
            return parser_.token_is_loop(mode);
        }

        inline bool token_is_arithmetic_opr(
            const token_advance_mode mode = token_advance_mode::e_advance)
        {
            return parser_.token_is_arithmetic_opr(mode);
        }

        inline bool token_is_right_bracket(
            const token_advance_mode mode = token_advance_mode::e_advance)
        {
            return parser_.token_is_right_bracket(mode);
        }

        inline bool token_is_ineq_opr(const token_advance_mode mode = token_advance_mode::e_advance)
        {
            return parser_.token_is_ineq_opr(mode);
        }

        inline bool peek_token_is(const token_t::token_type type) const
        {
            return parser_.peek_token_is(type);
        }

        inline bool peek_token_is(const std::string& symbol) const
        {
            return parser_.peek_token_is(symbol);
        }

        inline expression_node_ptr parse_expression()
        {
            return parser_.parse_expression();
        }

        inline expression_node_ptr parse_multi_sequence(const std::string& source = "",
                                                        const bool wrap_sequence = false)
        {
            return parser_.parse_multi_sequence(source, wrap_sequence);
        }

        template <typename Sequence1, typename Sequence2>
        inline expression_node_ptr simplify(Sequence1& expression_list, Sequence2& side_effect_list)
        {
            return parser_.simplify(expression_list, side_effect_list);
        }

        inline void set_error(const parser_error::type& error)
        {
            parser_.set_error(error);
        }

        static inline expression_node_ptr error_node()
        {
            return parser<T>::error_node();
        }

        inline void free_node(expression_node_ptr& node)
        {
            details::free_node(node_allocator, node);
        }

        inline expression_node_ptr conditional(expression_node_ptr condition,
                                               expression_node_ptr consequent,
                                               expression_node_ptr alternative)
        {
            return parser_.expression_generator_.conditional(condition, consequent, alternative);
        }

#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
        inline expression_node_ptr conditional_string(expression_node_ptr condition,
                                                      expression_node_ptr consequent,
                                                      expression_node_ptr alternative)
        {
            return parser_.expression_generator_.conditional_string(condition, consequent,
                                                                    alternative);
        }
#endif

        inline expression_node_ptr conditional_vector(expression_node_ptr condition,
                                                      expression_node_ptr consequent,
                                                      expression_node_ptr alternative)
        {
            return parser_.expression_generator_.conditional_vector(condition, consequent,
                                                                    alternative);
        }

        inline expression_node_ptr while_loop(expression_node_ptr condition,
                                              expression_node_ptr branch,
                                              const bool break_or_continue_present)
        {
            return parser_.expression_generator_.while_loop(condition, branch,
                                                            break_or_continue_present);
        }

        inline expression_node_ptr repeat_until_loop(expression_node_ptr condition,
                                                     expression_node_ptr branch,
                                                     const bool break_or_continue_present)
        {
            return parser_.expression_generator_.repeat_until_loop(condition, branch,
                                                                   break_or_continue_present);
        }

        inline expression_node_ptr for_loop(expression_node_ptr initialiser,
                                            expression_node_ptr condition,
                                            expression_node_ptr incrementor,
                                            expression_node_ptr loop_body,
                                            const bool break_or_continue_present)
        {
            return parser_.expression_generator_.for_loop(initialiser, condition, incrementor,
                                                          loop_body, break_or_continue_present);
        }

        inline expression_node_ptr make_null_node()
        {
            return node_allocator.template allocate<details::null_node<T>>();
        }

        inline expression_node_ptr make_variable_node(T& value)
        {
            return node_allocator.template allocate<variable_node_t>(value);
        }

        inline void handle_brkcnt_scope_exit()
        {
            parser_.handle_brkcnt_scope_exit();
        }

        inline void activate_side_effect(const std::string& source)
        {
            state.activate_side_effect(source);
        }

        parser<T>& parser_;
        settings_store& settings;
        parser_state& state;
        scope_element_manager& sem;
        symtab_store_t& symtab_store;
        std::deque<bool>& brkcnt_list;
        details::node_allocator& node_allocator;
    };

    struct switch_context
    {
        using token_advance_mode = typename prsrhlpr_t::token_advance_mode;

        explicit switch_context(parser<T>& parser)
            : parser_(parser), node_allocator(parser.node_allocator_)
        {
        }

        inline const token_t& current_token() const
        {
            return parser_.current_token();
        }

        inline void next_token()
        {
            parser_.next_token();
        }

        inline bool token_is(const token_t::token_type type,
                             const token_advance_mode mode = token_advance_mode::e_advance)
        {
            return parser_.token_is(type, mode);
        }

        inline expression_node_ptr parse_expression()
        {
            return parser_.parse_expression();
        }

        inline expression_node_ptr parse_multi_sequence(const std::string& source = "",
                                                        const bool wrap_sequence = false)
        {
            return parser_.parse_multi_sequence(source, wrap_sequence);
        }

        inline void set_error(const parser_error::type& error)
        {
            parser_.set_error(error);
        }

        static inline expression_node_ptr error_node()
        {
            return parser<T>::error_node();
        }

        inline void free_node(expression_node_ptr& node)
        {
            details::free_node(node_allocator, node);
        }

        inline expression_node_ptr switch_statement(std::vector<expression_node_ptr>& arg_list,
                                                    const bool default_statement_present)
        {
            return parser_.expression_generator_.switch_statement(arg_list,
                                                                  default_statement_present);
        }

        inline expression_node_ptr multi_switch_statement(
            std::vector<expression_node_ptr>& arg_list)
        {
            return parser_.expression_generator_.multi_switch_statement(arg_list);
        }

        inline expression_node_ptr make_nan_literal()
        {
            return node_allocator.template allocate_c<literal_node_t>(
                std::numeric_limits<T>::quiet_NaN());
        }

        parser<T>& parser_;
        details::node_allocator& node_allocator;
    };

    struct vararg_context
    {
        explicit vararg_context(parser<T>& parser)
            : parser_(parser), node_allocator(parser.node_allocator_)
        {
        }

        inline const token_t& current_token() const
        {
            return parser_.current_token();
        }

        inline void next_token()
        {
            parser_.next_token();
        }

        inline bool token_is(const token_t::token_type type)
        {
            return parser_.token_is(type);
        }

        inline expression_node_ptr parse_expression()
        {
            return parser_.parse_expression();
        }

        inline expression_node_ptr parse_multi_sequence()
        {
            return parser_.parse_multi_sequence();
        }

        inline expression_node_ptr parse_multi_switch_statement()
        {
            return parser_.parse_multi_switch_statement();
        }

        inline expression_node_ptr check_block_statement_closure(expression_node_ptr expression)
        {
            return parser_.check_block_statement_closure(expression);
        }

        inline void lodge_symbol(const std::string& symbol, const symbol_type st)
        {
            parser_.lodge_symbol(symbol, st);
        }

        inline void set_error(const parser_error::type& error)
        {
            parser_.set_error(error);
        }

        static inline expression_node_ptr error_node()
        {
            return parser<T>::error_node();
        }

        inline expression_node_ptr vararg_function(const core::operators::operator_type operation,
                                                   std::vector<expression_node_ptr>& arg_list)
        {
            return parser_.expression_generator_.vararg_function(operation, arg_list);
        }

        inline void free_node(expression_node_ptr& node)
        {
            details::free_node(node_allocator, node);
        }

        parser<T>& parser_;
        details::node_allocator& node_allocator;
    };

    struct sequence_context
    {
        using token_advance_mode = typename prsrhlpr_t::token_advance_mode;

        explicit sequence_context(parser<T>& parser)
            : parser_(parser),
              settings(parser.settings_),
              state(parser.state_),
              sem(parser.sem_),
              node_allocator(parser.node_allocator_)
        {
        }

        inline const token_t& current_token() const
        {
            return parser_.current_token();
        }

        inline bool token_is(const token_t::token_type type,
                             const token_advance_mode mode = token_advance_mode::e_advance)
        {
            return parser_.token_is(type, mode);
        }

        inline bool peek_token_is(const token_t::token_type type) const
        {
            return parser_.peek_token_is(type);
        }

        inline expression_node_ptr parse_expression()
        {
            return parser_.parse_expression();
        }

        template <typename Sequence1, typename Sequence2>
        inline expression_node_ptr simplify(Sequence1& expression_list, Sequence2& side_effect_list,
                                            const bool specialise_on_final_type = false)
        {
            return parser_.simplify(expression_list, side_effect_list, specialise_on_final_type);
        }

        inline void set_error(const parser_error::type& error)
        {
            parser_.set_error(error);
        }

        static inline expression_node_ptr error_node()
        {
            return parser<T>::error_node();
        }

        inline expression_node_ptr make_null_node()
        {
            return node_allocator.template allocate<details::null_node<T>>();
        }

        inline void free_node(expression_node_ptr& node)
        {
            details::free_node(node_allocator, node);
        }

        parser<T>& parser_;
        settings_store& settings;
        parser_state& state;
        scope_element_manager& sem;
        details::node_allocator& node_allocator;
    };

    struct range_context
    {
        explicit range_context(parser<T>& parser)
            : parser_(parser), node_allocator(parser.node_allocator_)
        {
        }

        inline const token_t& current_token() const
        {
            return parser_.current_token();
        }

        inline bool token_is(const token_t::token_type type)
        {
            return parser_.token_is(type);
        }

        inline expression_node_ptr parse_expression()
        {
            return parser_.parse_expression();
        }

        inline void set_error(const parser_error::type& error)
        {
            parser_.set_error(error);
        }

        inline void free_node(expression_node_ptr& node)
        {
            details::free_node(node_allocator, node);
        }

        parser<T>& parser_;
        details::node_allocator& node_allocator;
    };

#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
    struct string_range_context
    {
        using token_advance_mode = typename prsrhlpr_t::token_advance_mode;
        using range_t = typename parser<T>::range_t;

        explicit string_range_context(parser<T>& parser)
            : parser_(parser), node_allocator(parser.node_allocator_)
        {
        }

        inline const token_t& current_token() const
        {
            return parser_.current_token();
        }

        inline bool token_is(const token_t::token_type type,
                             const token_advance_mode mode = token_advance_mode::e_advance)
        {
            return parser_.token_is(type, mode);
        }

        inline bool parse_range(range_t& rp, const bool skip_lsqr = false)
        {
            return parser_.parse_range(rp, skip_lsqr);
        }

        inline void set_error(const parser_error::type& error)
        {
            parser_.set_error(error);
        }

        static inline expression_node_ptr error_node()
        {
            return parser<T>::error_node();
        }

        inline expression_node_ptr make_string_size_node(expression_node_ptr expression)
        {
            return node_allocator.template allocate<details::string_nodes::string_size_node<T>>(
                expression);
        }

        inline expression_node_ptr string_range(expression_node_ptr expression, range_t& rp)
        {
            return parser_.expression_generator_(expression, rp);
        }

        inline void free_node(expression_node_ptr& node)
        {
            details::free_node(node_allocator, node);
        }

        inline bool errors_empty() const
        {
            return parser_.error_list_.empty();
        }

        parser<T>& parser_;
        details::node_allocator& node_allocator;
    };
#endif

    struct vector_index_context
    {
        using token_advance_mode = typename prsrhlpr_t::token_advance_mode;
        using vector_interface_t = details::vector_interface<T>;

        explicit vector_index_context(parser<T>& parser)
            : parser_(parser),
              settings(parser.settings_),
              sem(parser.sem_),
              node_allocator(parser.node_allocator_)
        {
        }

        inline const token_t& current_token() const
        {
            return parser_.current_token();
        }

        inline bool token_is(const token_t::token_type type,
                             const token_advance_mode mode = token_advance_mode::e_advance)
        {
            return parser_.token_is(type, mode);
        }

        inline bool peek_token_is(const token_t::token_type type) const
        {
            return parser_.peek_token_is(type);
        }

        inline expression_node_ptr parse_expression()
        {
            return parser_.parse_expression();
        }

        inline void set_error(const parser_error::type& error)
        {
            parser_.set_error(error);
        }

        static inline expression_node_ptr error_node()
        {
            return parser<T>::error_node();
        }

        inline void free_node(expression_node_ptr& node)
        {
            details::free_node(node_allocator, node);
        }

        inline expression_node_ptr synthesize_vector_element(const std::string& vector_name,
                                                             vector_holder_ptr vec,
                                                             expression_node_ptr vec_node,
                                                             expression_node_ptr index_expr)
        {
            return parser_.synthesize_vector_element(vector_name, vec, vec_node, index_expr);
        }

        inline bool errors_empty() const
        {
            return parser_.error_list_.empty();
        }

        parser<T>& parser_;
        settings_store& settings;
        scope_element_manager& sem;
        details::node_allocator& node_allocator;
    };

    struct entity_context
    {
        using token_advance_mode = typename prsrhlpr_t::token_advance_mode;
        using range_t = typename parser<T>::range_t;
        using scope_element_t = math_expr::scope_element<T>;
        using symbol_table_t = typename parser<T>::symbol_table_t;
        using vector_holder_ptr = typename parser<T>::vector_holder_ptr;
        using string_context_t = typename symtab_store_t::string_context;
        using vector_context_t = typename symtab_store_t::vector_context;
#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
        using stringvar_node_ptr = typename parser<T>::stringvar_node_t*;
#endif

        explicit entity_context(parser<T>& parser)
            : parser_(parser),
              state(parser.state_),
              sem(parser.sem_),
              symtab_store(parser.symtab_store_),
              node_allocator(parser.node_allocator_)
        {
        }

        inline const token_t& current_token() const
        {
            return parser_.current_token();
        }

        inline void next_token()
        {
            parser_.next_token();
        }

        inline bool token_is(const token_t::token_type type,
                             const token_advance_mode mode = token_advance_mode::e_advance)
        {
            return parser_.token_is(type, mode);
        }

        inline bool peek_token_is(const token_t::token_type type) const
        {
            return parser_.peek_token_is(type);
        }

        inline bool parse_range(range_t& rp, const bool skip_lsqr = false)
        {
            return parser_.parse_range(rp, skip_lsqr);
        }

        inline expression_node_ptr parse_vector_index(const std::string& vector_name = "")
        {
            return parser_.parse_vector_index(vector_name);
        }

        inline void set_error(const parser_error::type& error)
        {
            parser_.set_error(error);
        }

        static inline expression_node_ptr error_node()
        {
            return parser<T>::error_node();
        }

        inline void free_node(expression_node_ptr& node)
        {
            details::free_node(node_allocator, node);
        }

        inline scope_element_t& get_active_element(const std::string& symbol)
        {
            return sem.get_active_element(symbol);
        }

#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
        inline string_context_t get_string_context(const std::string& symbol) const
        {
            return symtab_store.get_string_context(symbol);
        }

        inline bool is_conststr_stringvar(const std::string& symbol) const
        {
            return symtab_store.is_conststr_stringvar(symbol);
        }

        inline bool is_constant_string(const std::string& symbol) const
        {
            return symtab_store.is_constant_string(symbol);
        }

        inline expression_node_ptr make_string_literal(const std::string& value)
        {
            return parser_.expression_generator_(value);
        }

        inline expression_node_ptr make_const_string_range(const std::string& value, range_t& range)
        {
            return parser_.expression_generator_(value, range);
        }

        inline expression_node_ptr make_string_range(stringvar_node_ptr node, range_t& range)
        {
            return parser_.expression_generator_(node->ref(), range);
        }

        inline expression_node_ptr make_stringvar_size_node(stringvar_node_ptr node)
        {
            return node_allocator.template allocate<details::string_nodes::stringvar_size_node<T>>(
                node->ref());
        }

        inline void lodge_immutable_string_symbol(const lexer::token& token, core::char_cptr begin,
                                                  const std::size_t size)
        {
            parser_.lodge_immutable_symbol(token, parser_.make_memory_range(begin, size));
        }
#endif

        inline vector_context_t get_vector_context(const std::string& vector_name) const
        {
            return symtab_store.get_vector_context(vector_name);
        }

        inline expression_node_ptr make_numeric_literal(const T& value)
        {
            return parser_.expression_generator_(value);
        }

        inline expression_node_ptr make_vector_node(vector_holder_ptr vector_holder)
        {
            return node_allocator.template allocate<vector_node_t>(vector_holder);
        }

        inline expression_node_ptr make_vector_size_node(vector_holder_ptr vector_holder)
        {
            return node_allocator.template allocate<vector_size_node_t>(vector_holder);
        }

        inline expression_node_ptr synthesize_vector_element(const std::string& vector_name,
                                                             vector_holder_ptr vector_holder,
                                                             expression_node_ptr vector_node,
                                                             expression_node_ptr index_expr)
        {
            return parser_.synthesize_vector_element(vector_name, vector_holder, vector_node,
                                                     index_expr);
        }

        inline void lodge_immutable_vector_symbol(const lexer::token& token, const T* begin,
                                                  const std::size_t size)
        {
            parser_.lodge_immutable_symbol(token, parser_.make_memory_range(begin, size));
        }

        inline void lodge_symbol(const std::string& symbol, const symbol_type st)
        {
            parser_.lodge_symbol(symbol, st);
        }

        parser<T>& parser_;
        parser_state& state;
        scope_element_manager& sem;
        symtab_store_t& symtab_store;
        details::node_allocator& node_allocator;
    };

    struct function_call_context
    {
        using token_advance_mode = typename prsrhlpr_t::token_advance_mode;
        using base_ops_map_t = typename parser<T>::base_ops_map_t;

        explicit function_call_context(parser<T>& parser)
            : parser_(parser),
              state(parser.state_),
              base_ops_map(parser.base_ops_map_),
              node_allocator(parser.node_allocator_)
        {
        }

        inline const token_t& current_token() const
        {
            return parser_.current_token();
        }

        inline void next_token()
        {
            parser_.next_token();
        }

        inline bool token_is(const token_t::token_type type,
                             const token_advance_mode mode = token_advance_mode::e_advance)
        {
            return parser_.token_is(type, mode);
        }

        inline expression_node_ptr parse_expression()
        {
            return parser_.parse_expression();
        }

        inline void set_error(const parser_error::type& error)
        {
            parser_.set_error(error);
        }

        static inline expression_node_ptr error_node()
        {
            return parser<T>::error_node();
        }

        inline void free_node(expression_node_ptr& node)
        {
            details::free_node(node_allocator, node);
        }

        template <std::size_t N>
        inline expression_node_ptr function(ifunction<T>* function,
                                            expression_node_ptr (&branch)[N])
        {
            return parser_.expression_generator_.function(function, branch);
        }

        inline expression_node_ptr function(ifunction<T>* function)
        {
            return parser_.expression_generator_.function(function);
        }

        template <std::size_t N>
        inline expression_node_ptr base_operation(const core::operators::operator_type operation,
                                                  expression_node_ptr (&branch)[N])
        {
            return parser_.expression_generator_(operation, branch);
        }

        inline void lodge_symbol(const std::string& symbol, const symbol_type st)
        {
            parser_.lodge_symbol(symbol, st);
        }

        parser<T>& parser_;
        parser_state& state;
        base_ops_map_t& base_ops_map;
        details::node_allocator& node_allocator;
    };

    struct symbol_resolution_context
    {
        using scope_element_t = math_expr::scope_element<T>;
        using symbol_table_t = typename parser<T>::symbol_table_t;
        using variable_context_t = typename symtab_store_t::variable_context;
        using unknown_symbol_resolver_t = typename parser<T>::unknown_symbol_resolver;

        explicit symbol_resolution_context(parser<T>& parser)
            : parser_(parser),
              settings(parser.settings_),
              sem(parser.sem_),
              symtab_store(parser.symtab_store_),
              resolve_unknown_symbol(parser.resolve_unknown_symbol_),
              unknown_symbol_resolver(parser.unknown_symbol_resolver_)
        {
        }

        inline const token_t& current_token() const
        {
            return parser_.current_token();
        }

        inline void next_token()
        {
            parser_.next_token();
        }

        inline void set_error(const parser_error::type& error)
        {
            parser_.set_error(error);
        }

        static inline expression_node_ptr error_node()
        {
            return parser<T>::error_node();
        }

        inline variable_context_t get_variable_context(const std::string& symbol) const
        {
            return symtab_store.get_variable_context(symbol);
        }

        inline bool is_constant_node(const std::string& symbol) const
        {
            return symtab_store.is_constant_node(symbol);
        }

        inline expression_node_ptr make_numeric_literal(const T& value)
        {
            return parser_.expression_generator_(value);
        }

        inline void lodge_immutable_variable_symbol(const lexer::token& token, const T& value)
        {
            parser_.lodge_immutable_symbol(token, parser_.make_memory_range(value));
        }

        inline bool post_variable_process(const std::string& symbol)
        {
            return parser_.post_variable_process(symbol);
        }

        inline void lodge_symbol(const std::string& symbol, const symbol_type st)
        {
            parser_.lodge_symbol(symbol, st);
        }

        inline bool scope_empty() const
        {
            return sem.empty();
        }

        inline scope_element_t& get_active_element(const std::string& symbol)
        {
            return sem.get_active_element(symbol);
        }

#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
        inline bool is_stringvar(const std::string& symbol) const
        {
            return symtab_store.is_stringvar(symbol);
        }

        inline expression_node_ptr parse_string()
        {
            return parser_.parse_string();
        }

        inline igeneric_function<T>* get_string_function(const std::string& symbol) const
        {
            return symtab_store.get_string_function(symbol);
        }

        inline expression_node_ptr parse_string_function_call(igeneric_function<T>* function,
                                                              const std::string& function_name)
        {
            return parser_.parse_string_function_call(function, function_name);
        }

        inline igeneric_function<T>* get_overload_function(const std::string& symbol) const
        {
            return symtab_store.get_overload_function(symbol);
        }

        inline expression_node_ptr parse_overload_function_call(igeneric_function<T>* function,
                                                                const std::string& function_name)
        {
            return parser_.parse_overload_function_call(function, function_name);
        }
#endif

        inline expression_node_ptr parse_vector()
        {
            return parser_.parse_vector();
        }

        inline ifunction<T>* get_function(const std::string& symbol) const
        {
            return symtab_store.get_function(symbol);
        }

        inline expression_node_ptr parse_function_invocation(ifunction<T>* function,
                                                             const std::string& function_name)
        {
            return parser_.parse_function_invocation(function, function_name);
        }

        inline ivararg_function<T>* get_vararg_function(const std::string& symbol) const
        {
            return symtab_store.get_vararg_function(symbol);
        }

        inline expression_node_ptr parse_vararg_function_call(ivararg_function<T>* function,
                                                              const std::string& function_name)
        {
            return parser_.parse_vararg_function_call(function, function_name);
        }

        inline igeneric_function<T>* get_generic_function(const std::string& symbol) const
        {
            return symtab_store.get_generic_function(symbol);
        }

        inline expression_node_ptr parse_generic_function_call(igeneric_function<T>* function,
                                                               const std::string& function_name)
        {
            return parser_.parse_generic_function_call(function, function_name);
        }

        inline bool is_vector(const std::string& symbol) const
        {
            return symtab_store.is_vector(symbol);
        }

        inline symbol_table_t& get_symbol_table()
        {
            return symtab_store.get_symbol_table();
        }

        inline expression_node_ptr get_variable(const std::string& symbol) const
        {
            return symtab_store.get_variable(symbol);
        }

        parser<T>& parser_;
        settings_store& settings;
        scope_element_manager& sem;
        symtab_store_t& symtab_store;
        bool& resolve_unknown_symbol;
        unknown_symbol_resolver_t*& unknown_symbol_resolver;
    };

    struct symbol_context
    {
        explicit symbol_context(parser<T>& parser)
            : parser_(parser),
              settings(parser.settings_),
              sem(parser.sem_),
              symtab_store(parser.symtab_store_)
        {
        }

        inline const token_t& current_token() const
        {
            return parser_.current_token();
        }

        inline void next_token()
        {
            parser_.next_token();
        }

        inline void set_error(const parser_error::type& error)
        {
            parser_.set_error(error);
        }

        static inline expression_node_ptr error_node()
        {
            return parser<T>::error_node();
        }

        inline bool valid_vararg_operation(const std::string& symbol) const
        {
            return parser_.valid_vararg_operation(symbol);
        }

        inline bool valid_base_operation(const std::string& symbol) const
        {
            return parser_.valid_base_operation(symbol);
        }

        inline expression_node_ptr parse_vararg_function()
        {
            return parser_.parse_vararg_function();
        }

        inline expression_node_ptr parse_not_statement()
        {
            return parser_.parse_not_statement();
        }

        inline expression_node_ptr make_numeric_literal(const T& value)
        {
            return parser_.expression_generator_(value);
        }

        inline expression_node_ptr parse_base_operation()
        {
            return parser_.parse_base_operation();
        }

        inline expression_node_ptr parse_conditional_statement()
        {
            return parser_.parse_conditional_statement();
        }

        inline expression_node_ptr check_block_statement_closure(expression_node_ptr expression)
        {
            return parser_.check_block_statement_closure(expression);
        }

        inline expression_node_ptr parse_while_loop()
        {
            return parser_.parse_while_loop();
        }

        inline expression_node_ptr parse_repeat_until_loop()
        {
            return parser_.parse_repeat_until_loop();
        }

        inline expression_node_ptr parse_for_loop()
        {
            return parser_.parse_for_loop();
        }

        inline expression_node_ptr parse_switch_statement()
        {
            return parser_.parse_switch_statement();
        }

        inline expression_node_ptr parse_special_function()
        {
            return parser_.parse_special_function();
        }

        inline expression_node_ptr parse_null_statement()
        {
            return parser_.parse_null_statement();
        }

#ifndef MATH_EXPR_DISABLE_BREAK_CONTINUE
        inline expression_node_ptr parse_break_statement()
        {
            return parser_.parse_break_statement();
        }

        inline expression_node_ptr parse_continue_statement()
        {
            return parser_.parse_continue_statement();
        }
#endif

        inline expression_node_ptr parse_define_var_statement()
        {
            return parser_.parse_define_var_statement();
        }

        inline expression_node_ptr parse_define_constvar_statement()
        {
            return parser_.parse_define_constvar_statement();
        }

        inline expression_node_ptr parse_swap_statement()
        {
            return parser_.parse_swap_statement();
        }

#ifndef MATH_EXPR_DISABLE_RETURN_STATEMENT
        inline expression_node_ptr parse_return_statement()
        {
            return parser_.parse_return_statement();
        }
#endif

        inline expression_node_ptr parse_assert_statement()
        {
            return parser_.parse_assert_statement();
        }

        inline bool symtab_valid() const
        {
            return symtab_store.valid();
        }

        inline bool scope_empty() const
        {
            return sem.empty();
        }

        inline expression_node_ptr parse_symtab_symbol()
        {
            return parser_.parse_symtab_symbol();
        }

        parser<T>& parser_;
        settings_store& settings;
        scope_element_manager& sem;
        symtab_store_t& symtab_store;
    };

    struct special_case_context
    {
        using token_advance_mode = typename prsrhlpr_t::token_advance_mode;

        explicit special_case_context(parser<T>& parser)
            : parser_(parser),
              state(parser.state_),
              brkcnt_list(parser.brkcnt_list_),
              node_allocator(parser.node_allocator_)
        {
        }

        inline const token_t& current_token() const
        {
            return parser_.current_token();
        }

        inline void next_token()
        {
            parser_.next_token();
        }

        inline bool token_is(const token_t::token_type type,
                             const token_advance_mode mode = token_advance_mode::e_advance)
        {
            return parser_.token_is(type, mode);
        }

        inline expression_node_ptr parse_expression()
        {
            return parser_.parse_expression();
        }

        inline void set_error(const parser_error::type& error)
        {
            parser_.set_error(error);
        }

        static inline expression_node_ptr error_node()
        {
            return parser<T>::error_node();
        }

        inline void free_node(expression_node_ptr& node)
        {
            details::free_node(node_allocator, node);
        }

        template <std::size_t N>
        inline expression_node_ptr special_function(const core::operators::operator_type operation,
                                                    expression_node_ptr (&branch)[N])
        {
            return parser_.expression_generator_.special_function(operation, branch);
        }

        inline expression_node_ptr make_null_node()
        {
            return node_allocator.template allocate<details::null_node<T>>();
        }

#ifndef MATH_EXPR_DISABLE_BREAK_CONTINUE
        inline expression_node_ptr make_break_node(expression_node_ptr return_expr)
        {
            return node_allocator.template allocate<details::break_node<T>>(return_expr);
        }

        inline expression_node_ptr make_continue_node()
        {
            return node_allocator.template allocate<details::continue_node<T>>();
        }
#endif

        inline void activate_side_effect(const std::string& source)
        {
            state.activate_side_effect(source);
        }

        parser<T>& parser_;
        parser_state& state;
        std::deque<bool>& brkcnt_list;
        details::node_allocator& node_allocator;
    };

    class type_checker;

    struct dynamic_function_context
    {
        using token_advance_mode = typename prsrhlpr_t::token_advance_mode;
        using type_checker_t = type_checker;

        explicit dynamic_function_context(parser<T>& parser)
            : parser_(parser), state(parser.state_), node_allocator(parser.node_allocator_)
        {
        }

        inline const token_t& current_token() const
        {
            return parser_.current_token();
        }

        inline void next_token()
        {
            parser_.next_token();
        }

        inline bool token_is(const token_t::token_type type,
                             const token_advance_mode mode = token_advance_mode::e_advance)
        {
            return parser_.token_is(type, mode);
        }

        inline expression_node_ptr parse_expression()
        {
            return parser_.parse_expression();
        }

        inline void set_error(const parser_error::type& error)
        {
            parser_.set_error(error);
        }

        static inline expression_node_ptr error_node()
        {
            return parser<T>::error_node();
        }

        inline void free_node(expression_node_ptr& node)
        {
            details::free_node(node_allocator, node);
        }

        inline expression_node_ptr vararg_function_call(ivararg_function<T>* function,
                                                        std::vector<expression_node_ptr>& arg_list)
        {
            return parser_.expression_generator_.vararg_function_call(function, arg_list);
        }

        inline expression_node_ptr generic_function_call(igeneric_function<T>* function,
                                                         std::vector<expression_node_ptr>& arg_list)
        {
            return parser_.expression_generator_.generic_function_call(function, arg_list);
        }

        inline expression_node_ptr generic_function_call(igeneric_function<T>* function,
                                                         std::vector<expression_node_ptr>& arg_list,
                                                         std::size_t param_seq_index)
        {
            return parser_.expression_generator_.generic_function_call(function, arg_list,
                                                                       param_seq_index);
        }

#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
        inline expression_node_ptr string_function_call(igeneric_function<T>* function,
                                                        std::vector<expression_node_ptr>& arg_list)
        {
            return parser_.expression_generator_.string_function_call(function, arg_list);
        }

        inline expression_node_ptr string_function_call(igeneric_function<T>* function,
                                                        std::vector<expression_node_ptr>& arg_list,
                                                        std::size_t param_seq_index)
        {
            return parser_.expression_generator_.string_function_call(function, arg_list,
                                                                      param_seq_index);
        }
#endif

        parser<T>& parser_;
        parser_state& state;
        details::node_allocator& node_allocator;
    };

    inline expression_node_ptr parse_function_invocation(ifunction<T>* function,
                                                         const std::string& function_name)
    {
        function_call_context context(*this);
        return parser_function_call<T>::parse_function_invocation(context, function, function_name);
    }

    template <std::size_t NumberofParameters>
    inline expression_node_ptr parse_function_call(ifunction<T>* function,
                                                   const std::string& function_name)
    {
        function_call_context context(*this);
        return parser_function_call<T>::template parse_function_call<NumberofParameters>(
            context, function, function_name);
    }

    inline expression_node_ptr parse_function_call_0(ifunction<T>* function,
                                                     const std::string& function_name)
    {
        function_call_context context(*this);
        return parser_function_call<T>::parse_function_call_0(context, function, function_name);
    }

    template <std::size_t MaxNumberofParameters>
    inline std::size_t parse_base_function_call(
        expression_node_ptr (&param_list)[MaxNumberofParameters],
        const std::string& function_name = "")
    {
        function_call_context context(*this);
        return parser_function_call<T>::template parse_base_function_call<MaxNumberofParameters>(
            context, param_list, function_name);
    }

    inline expression_node_ptr parse_base_operation()
    {
        function_call_context context(*this);
        return parser_function_call<T>::parse_base_operation(context);
    }

    inline expression_node_ptr parse_conditional_statement_01(expression_node_ptr condition)
    {
        control_flow_context context(*this);
        return parser_control_flow<T>::parse_conditional_statement_01(context, condition);
    }

    inline expression_node_ptr parse_conditional_statement_02(expression_node_ptr condition)
    {
        control_flow_context context(*this);
        return parser_control_flow<T>::parse_conditional_statement_02(context, condition);
    }

    inline expression_node_ptr parse_conditional_statement()
    {
        control_flow_context context(*this);
        return parser_control_flow<T>::parse_conditional_statement(context);
    }

    inline expression_node_ptr parse_ternary_conditional_statement(expression_node_ptr condition)
    {
        control_flow_context context(*this);
        return parser_control_flow<T>::parse_ternary_conditional_statement(context, condition);
    }

    inline expression_node_ptr parse_not_statement()
    {
        if (settings_.logic_disabled("not"))
        {
            set_error(make_error(parser_error::error_mode::e_syntax, current_token(),
                                 "ERR063 - Invalid or disabled logic operation 'not'",
                                 core::error_location()));

            return error_node();
        }

        return parse_base_operation();
    }

    void handle_brkcnt_scope_exit()
    {
        assert(!brkcnt_list_.empty());
        brkcnt_list_.pop_front();
    }

    inline expression_node_ptr parse_while_loop()
    {
        control_flow_context context(*this);
        return parser_control_flow<T>::parse_while_loop(context);
    }

    inline expression_node_ptr parse_repeat_until_loop()
    {
        control_flow_context context(*this);
        return parser_control_flow<T>::parse_repeat_until_loop(context);
    }

    inline expression_node_ptr parse_for_loop()
    {
        control_flow_context context(*this);
        return parser_control_flow<T>::parse_for_loop(context);
    }

    inline expression_node_ptr parse_switch_statement()
    {
        switch_context context(*this);
        return parser_switch<T>::parse_switch_statement(context);
    }

    inline expression_node_ptr parse_multi_switch_statement()
    {
        switch_context context(*this);
        return parser_switch<T>::parse_multi_switch_statement(context);
    }

    inline expression_node_ptr parse_vararg_function()
    {
        vararg_context context(*this);
        return parser_vararg<T>::parse_vararg_function(context);
    }

#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
    inline expression_node_ptr parse_string_range_statement(expression_node_ptr& expression)
    {
        string_range_context context(*this);
        return parser_string_range<T>::parse_string_range_statement(context, expression);
    }
#else
    inline expression_node_ptr parse_string_range_statement(expression_node_ptr&)
    {
        return error_node();
    }
#endif

    inline bool parse_pending_string_rangesize(expression_node_ptr& expression)
    {
#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
        string_range_context context(*this);
        return parser_string_range<T>::parse_pending_string_rangesize(context, expression);
#else
        return false;
#endif
    }

    inline void parse_pending_vector_index_operator(expression_node_ptr& expression)
    {
        vector_index_context context(*this);
        parser_vector_index<T>::parse_pending_vector_index_operator(context, expression);
    }

    template <typename Allocator1, typename Allocator2,
              template <typename, typename> class Sequence>
    inline expression_node_ptr simplify(Sequence<expression_node_ptr, Allocator1>& expression_list,
                                        Sequence<bool, Allocator2>& side_effect_list,
                                        const bool specialise_on_final_type = false)
    {
        if (expression_list.empty())
            return error_node();
        else if (1 == expression_list.size())
            return expression_list[0];

        Sequence<expression_node_ptr, Allocator1> tmp_expression_list;

        core::debug_print("simplify() - expression_list.size: %d  side_effect_list.size(): %d\n",
                          static_cast<int>(expression_list.size()),
                          static_cast<int>(side_effect_list.size()));

        bool return_node_present = false;

        for (std::size_t i = 0; i < (expression_list.size() - 1); ++i)
        {
            if (is_variable_node(expression_list[i]))
                continue;
            else if (is_return_node(expression_list[i]) || is_break_node(expression_list[i]) ||
                     is_continue_node(expression_list[i]))
            {
                tmp_expression_list.push_back(expression_list[i]);

                // Remove all subexpressions after first short-circuit
                // node has been encountered.

                for (std::size_t j = i + 1; j < expression_list.size(); ++j)
                {
                    free_node(node_allocator_, expression_list[j]);
                }

                return_node_present = true;

                break;
            }
            else if (is_constant_node(expression_list[i]) || is_null_node(expression_list[i]) ||
                     !side_effect_list[i])
            {
                free_node(node_allocator_, expression_list[i]);
                continue;
            }
            else
                tmp_expression_list.push_back(expression_list[i]);
        }

        if (!return_node_present)
        {
            tmp_expression_list.push_back(expression_list.back());
        }

        expression_list.swap(tmp_expression_list);

        if (tmp_expression_list.size() > expression_list.size())
        {
            core::debug_print("simplify() - Reduced subexpressions from %d to %d\n",
                              static_cast<int>(tmp_expression_list.size()),
                              static_cast<int>(expression_list.size()));
        }

        if (return_node_present || side_effect_list.back() || (expression_list.size() > 1))
            state_.activate_side_effect("simplify()");

        if (1 == expression_list.size())
            return expression_list[0];
        else if (specialise_on_final_type && is_generally_string_node(expression_list.back()))
            return expression_generator_.vararg_function(core::operators::operator_type::smulti,
                                                         expression_list);
        else
            return expression_generator_.vararg_function(core::operators::operator_type::multi,
                                                         expression_list);
    }

    inline expression_node_ptr parse_multi_sequence(const std::string& source = "",
                                                    const bool enforce_crlbrackets = false)
    {
        sequence_context context(*this);
        return parser_sequence<T>::parse_multi_sequence(context, source, enforce_crlbrackets);
    }

    inline bool parse_range(range_t& rp, const bool skip_lsqr = false)
    {
        range_context context(*this);
        return parser_range<T>::parse_range(context, rp, skip_lsqr);
    }

    inline void lodge_symbol(const std::string& symbol, const symbol_type st)
    {
        dec_.add_symbol(symbol, st);
    }

#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
    inline expression_node_ptr parse_string()
    {
        entity_context context(*this);
        return parser_entity<T>::parse_string(context);
    }
#else
    inline expression_node_ptr parse_string()
    {
        return error_node();
    }
#endif

#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
    inline expression_node_ptr parse_const_string()
    {
        entity_context context(*this);
        return parser_entity<T>::parse_const_string(context);
    }
#else
    inline expression_node_ptr parse_const_string()
    {
        return error_node();
    }
#endif

    inline expression_node_ptr parse_vector_index(const std::string& vector_name = "")
    {
        vector_index_context context(*this);
        return parser_vector_index<T>::parse_vector_index(context, vector_name);
    }

    inline expression_node_ptr parse_vector()
    {
        entity_context context(*this);
        return parser_entity<T>::parse_vector(context);
    }

    inline expression_node_ptr synthesize_vector_element(const std::string& vector_name,
                                                         vector_holder_ptr vec,
                                                         expression_node_ptr vec_node,
                                                         expression_node_ptr index_expr)
    {
        // Perform compile-time range check
        if (details::is_constant_node(index_expr))
        {
            const std::size_t index =
                static_cast<std::size_t>(core::numeric::to_int32(index_expr->value()));
            const std::size_t vec_size = vec->size();

            if (index >= vec_size)
            {
                set_error(make_error(parser_error::error_mode::e_syntax, current_token(),
                                     "ERR126 - Index of " + core::to_str(index) +
                                         " out of range for "
                                         "vector '" +
                                         vector_name + "' of size " + core::to_str(vec_size),
                                     core::error_location()));

                free_node(node_allocator_, vec_node);
                free_node(node_allocator_, index_expr);

                return error_node();
            }
        }

        return expression_generator_.vector_element(vector_name, vec, vec_node, index_expr);
    }

    inline expression_node_ptr parse_vararg_function_call(ivararg_function<T>* vararg_function,
                                                          const std::string& vararg_function_name)
    {
        dynamic_function_context context(*this);
        return parser_dynamic_function<T>::parse_vararg_function_call(context, vararg_function,
                                                                      vararg_function_name);
    }

    class type_checker
    {
       public:
        enum return_type_t
        {
            e_overload = ' ',
            e_numeric = 'T',
            e_string = 'S'
        };

        struct function_prototype_t
        {
            return_type_t return_type;
            std::string param_seq;
        };

        using parser_t = parser<T>;
        using function_definition_list_t = std::vector<function_prototype_t>;

        type_checker(parser_t& p, const std::string& func_name, const std::string& func_prototypes,
                     const return_type_t default_return_type)
            : invalid_state_(true),
              parser_(p),
              function_name_(func_name),
              default_return_type_(default_return_type)
        {
            parse_function_prototypes(func_prototypes);
        }

        bool verify(const std::string& param_seq, std::size_t& pseq_index)
        {
            if (function_definition_list_.empty())
                return true;

            std::vector<std::pair<std::size_t, char>> error_list;

            for (std::size_t i = 0; i < function_definition_list_.size(); ++i)
            {
                core::char_t diff_value = 0;
                std::size_t diff_index = 0;

                const bool result = core::sequence_match(function_definition_list_[i].param_seq,
                                                         param_seq, diff_index, diff_value);

                if (result)
                {
                    pseq_index = i;
                    return true;
                }
                else
                    error_list.push_back(std::make_pair(diff_index, diff_value));
            }

            if (1 == error_list.size())
            {
                parser_.set_error(make_error(
                    parser_error::error_mode::e_syntax, parser_.current_token(),
                    "ERR132 - Failed parameter type check for function '" + function_name_ +
                        "', "
                        "Expected '" +
                        function_definition_list_[0].param_seq + "' call set: '" + param_seq + "'",
                    core::error_location()));
            }
            else
            {
                // find first with largest diff_index;
                std::size_t max_diff_index = 0;

                for (std::size_t i = 1; i < error_list.size(); ++i)
                {
                    if (error_list[i].first > error_list[max_diff_index].first)
                    {
                        max_diff_index = i;
                    }
                }

                parser_.set_error(make_error(
                    parser_error::error_mode::e_syntax, parser_.current_token(),
                    "ERR133 - Failed parameter type check for function '" + function_name_ +
                        "', "
                        "Best match: '" +
                        function_definition_list_[max_diff_index].param_seq + "' call set: '" +
                        param_seq + "'",
                    core::error_location()));
            }

            return false;
        }

        std::size_t paramseq_count() const
        {
            return function_definition_list_.size();
        }

        std::string paramseq(const std::size_t& index) const
        {
            return function_definition_list_[index].param_seq;
        }

        return_type_t return_type(const std::size_t& index) const
        {
            return function_definition_list_[index].return_type;
        }

        bool invalid() const
        {
            return !invalid_state_;
        }

        bool allow_zero_parameters() const
        {
            for (std::size_t i = 0; i < function_definition_list_.size(); ++i)
            {
                if (std::string::npos != function_definition_list_[i].param_seq.find("Z"))
                {
                    return true;
                }
            }

            return false;
        }

       private:
        std::vector<std::string> split_param_seq(const std::string& param_seq,
                                                 const core::char_t delimiter = '|') const
        {
            std::string::const_iterator current_begin = param_seq.begin();
            std::string::const_iterator iter = param_seq.begin();

            std::vector<std::string> result;

            while (iter != param_seq.end())
            {
                if (*iter == delimiter)
                {
                    result.push_back(std::string(current_begin, iter));
                    current_begin = ++iter;
                }
                else
                    ++iter;
            }

            if (current_begin != iter)
            {
                result.push_back(std::string(current_begin, iter));
            }

            return result;
        }

        inline bool is_valid_token(std::string param_seq, function_prototype_t& funcproto) const
        {
            // Determine return type
            funcproto.return_type = default_return_type_;

            if (param_seq.size() > 2)
            {
                if (':' == param_seq[1])
                {
                    // Note: Only overloaded igeneric functions can have return
                    // type definitions.
                    if (type_checker::e_overload != default_return_type_)
                        return false;

                    switch (param_seq[0])
                    {
                        case 'T':
                            funcproto.return_type = type_checker::e_numeric;
                            break;

                        case 'S':
                            funcproto.return_type = type_checker::e_string;
                            break;

                        default:
                            return false;
                    }

                    param_seq.erase(0, 2);
                }
            }

            if ((std::string::npos != param_seq.find("?*")) ||
                (std::string::npos != param_seq.find("**")))
            {
                return false;
            }
            else if ((std::string::npos == param_seq.find_first_not_of("STV*?|")) ||
                     ("Z" == param_seq))
            {
                funcproto.param_seq = param_seq;
                return true;
            }

            return false;
        }

        void parse_function_prototypes(const std::string& func_prototypes)
        {
            if (func_prototypes.empty())
                return;

            std::vector<std::string> param_seq_list = split_param_seq(func_prototypes);

            using param_seq_map_t = std::map<std::string, std::size_t>;
            param_seq_map_t param_seq_map;

            for (std::size_t i = 0; i < param_seq_list.size(); ++i)
            {
                function_prototype_t func_proto;

                if (!is_valid_token(param_seq_list[i], func_proto))
                {
                    invalid_state_ = false;

                    parser_.set_error(
                        make_error(parser_error::error_mode::e_syntax, parser_.current_token(),
                                   "ERR134 - Invalid parameter sequence of '" + param_seq_list[i] +
                                       "' for function: " + function_name_,
                                   core::error_location()));
                    return;
                }

                param_seq_map_t::const_iterator seq_itr = param_seq_map.find(param_seq_list[i]);

                if (param_seq_map.end() != seq_itr)
                {
                    invalid_state_ = false;

                    parser_.set_error(make_error(
                        parser_error::error_mode::e_syntax, parser_.current_token(),
                        "ERR135 - Function '" + function_name_ +
                            "' has a parameter sequence conflict between " + "pseq_idx[" +
                            core::to_str(seq_itr->second) + "] and" + "pseq_idx[" +
                            core::to_str(i) + "] " + "param seq: " + param_seq_list[i],
                        core::error_location()));
                    return;
                }

                function_definition_list_.push_back(func_proto);
            }
        }

        type_checker(const type_checker&) = delete;
        type_checker& operator=(const type_checker&) = delete;

        bool invalid_state_;
        parser_t& parser_;
        std::string function_name_;
        const return_type_t default_return_type_;
        function_definition_list_t function_definition_list_;
    };

    inline expression_node_ptr parse_generic_function_call(igeneric_function<T>* function,
                                                           const std::string& function_name)
    {
        dynamic_function_context context(*this);
        return parser_dynamic_function<T>::parse_generic_function_call(context, function,
                                                                       function_name);
    }

    inline bool parse_igeneric_function_params(std::string& param_type_list,
                                               std::vector<expression_node_ptr>& arg_list,
                                               const std::string& function_name,
                                               igeneric_function<T>* function,
                                               const type_checker& tc)
    {
        dynamic_function_context context(*this);
        return parser_dynamic_function<T>::parse_igeneric_function_params(
            context, param_type_list, arg_list, function_name, function, tc);
    }

#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
    inline expression_node_ptr parse_string_function_call(igeneric_function<T>* function,
                                                          const std::string& function_name)
    {
        dynamic_function_context context(*this);
        return parser_dynamic_function<T>::parse_string_function_call(context, function,
                                                                      function_name);
    }

    inline expression_node_ptr parse_overload_function_call(igeneric_function<T>* function,
                                                            const std::string& function_name)
    {
        dynamic_function_context context(*this);
        return parser_dynamic_function<T>::parse_overload_function_call(context, function,
                                                                        function_name);
    }
#endif

    inline expression_node_ptr parse_special_function()
    {
        special_case_context context(*this);
        return parser_special_case<T>::parse_special_function(context);
    }

    inline expression_node_ptr parse_null_statement()
    {
        special_case_context context(*this);
        return parser_special_case<T>::parse_null_statement(context);
    }

#ifndef MATH_EXPR_DISABLE_BREAK_CONTINUE
    inline expression_node_ptr parse_break_statement()
    {
        special_case_context context(*this);
        return parser_special_case<T>::parse_break_statement(context);
    }

    inline expression_node_ptr parse_continue_statement()
    {
        special_case_context context(*this);
        return parser_special_case<T>::parse_continue_statement(context);
    }
#endif

    inline expression_node_ptr parse_define_vector_statement(const std::string& vec_name)
    {
        expression_node_ptr size_expression_node = error_node();

        if (!token_is(token_t::e_lsqrbracket))
        {
            set_error(make_error(parser_error::error_mode::e_syntax, current_token(),
                                 "ERR157 - Expected '[' as part of vector size definition",
                                 core::error_location()));

            return error_node();
        }
        else if (nullptr == (size_expression_node = parse_expression()))
        {
            set_error(make_error(parser_error::error_mode::e_syntax, current_token(),
                                 "ERR158 - Failed to determine size of vector '" + vec_name + "'",
                                 core::error_location()));

            return error_node();
        }
        else if (!is_constant_node(size_expression_node))
        {
            const bool is_rebaseble_vector =
                (size_expression_node->type() ==
                 details::expression_node<T>::node_type::e_vecsize) &&
                static_cast<details::vector_size_node<T>*>(size_expression_node)
                    ->vec_holder()
                    ->rebaseable();

            free_node(node_allocator_, size_expression_node);

            const std::string error_msg =
                (is_rebaseble_vector)
                    ? std::string(
                          "Rebasable/Resizable vector cannot be used to define the size of vector")
                    : std::string("Expected a constant literal number as size of vector");
            set_error(make_error(parser_error::error_mode::e_syntax, current_token(),
                                 "ERR159 - " + error_msg + " '" + vec_name + "'",
                                 core::error_location()));

            return error_node();
        }

        const T vector_size = size_expression_node->value();

        free_node(node_allocator_, size_expression_node);

        const std::size_t max_vector_size = settings_.max_local_vector_size();

        if ((vector_size <= T(0)) ||
            std::not_equal_to<T>()(T(0), vector_size - core::numeric::trunc(vector_size)) ||
            (static_cast<std::size_t>(vector_size) > max_vector_size))
        {
            set_error(
                make_error(parser_error::error_mode::e_syntax, current_token(),
                           "ERR160 - Invalid vector size. Must be an integer in the "
                           "range [0," +
                               core::to_str(static_cast<std::size_t>(max_vector_size)) +
                               "], size: " + core::to_str(core::numeric::to_int32(vector_size)),
                           core::error_location()));

            return error_node();
        }

        typename symbol_table_t::vector_holder_ptr vec_holder =
            typename symbol_table_t::vector_holder_ptr(0);

        const std::size_t vec_size = static_cast<std::size_t>(core::numeric::to_int32(vector_size));
        const std::size_t predicted_total_lclsymb_size =
            sizeof(T) * vec_size + sem_.total_local_symb_size_bytes();

        if (predicted_total_lclsymb_size > settings().max_total_local_symbol_size_bytes())
        {
            set_error(make_error(parser_error::error_mode::e_syntax, current_token(),
                                 "ERR161 - Adding vector '" + vec_name + "' of size " +
                                     core::to_str(vec_size) +
                                     " bytes "
                                     "will exceed max total local symbol size of: " +
                                     core::to_str(settings().max_total_local_symbol_size_bytes()) +
                                     " bytes, "
                                     "current total size: " +
                                     core::to_str(sem_.total_local_symb_size_bytes()) + " bytes",
                                 core::error_location()));

            return error_node();
        }

        scope_element& se = sem_.get_element(vec_name);

        if (se.name == vec_name)
        {
            if (se.active)
            {
                set_error(
                    make_error(parser_error::error_mode::e_syntax, current_token(),
                               "ERR162 - Illegal redefinition of local vector: '" + vec_name + "'",
                               core::error_location()));

                return error_node();
            }
            else if ((se.size == vec_size) && (scope_element::element_type::e_vector == se.type))
            {
                vec_holder = se.vec_node;
                se.active = true;
                se.depth = state_.scope_depth;
                se.ref_count++;
            }
        }

        if (nullptr == vec_holder)
        {
            scope_element nse;
            nse.name = vec_name;
            nse.active = true;
            nse.ref_count = 1;
            nse.type = scope_element::element_type::e_vector;
            nse.depth = state_.scope_depth;
            nse.size = vec_size;
            nse.vector_data = std::make_unique<T[]>(vec_size);
            nse.vec_node =
                new typename scope_element::vector_holder_t(nse.vector_data.get(), nse.size);

            core::numeric::set_zero_value(nse.vector_data.get(), vec_size);

            if (!sem_.add_element(std::move(nse)))
            {
                set_error(
                    make_error(parser_error::error_mode::e_syntax, current_token(),
                               "ERR163 - Failed to add new local vector '" + vec_name + "' to SEM",
                               core::error_location()));

                sem_.free_element(nse);

                return error_node();
            }

            assert(sem_.total_local_symb_size_bytes() <=
                   settings().max_total_local_symbol_size_bytes());

            vec_holder = nse.vec_node;

            core::debug_print(
                "parse_define_vector_statement() - INFO - Added new local vector: %s[%d]\n",
                nse.name.c_str(), static_cast<int>(nse.size));
        }

        state_.activate_side_effect("parse_define_vector_statement()");

        lodge_symbol(vec_name, symbol_type::e_st_local_vector);

        std::vector<expression_node_ptr> vec_initilizer_list;

        scoped_vec_delete<expression_node_t> svd((*this), vec_initilizer_list);

        bool single_value_initialiser = false;
        bool range_value_initialiser = false;
        bool vec_to_vec_initialiser = false;
        bool null_initialisation = false;

        if (!token_is(token_t::e_rsqrbracket))
        {
            set_error(make_error(parser_error::error_mode::e_syntax, current_token(),
                                 "ERR164 - Expected ']' as part of vector size definition",
                                 core::error_location()));

            return error_node();
        }
        else if (!token_is(token_t::e_eof, prsrhlpr_t::token_advance_mode::e_hold))
        {
            if (!token_is(token_t::e_assign))
            {
                set_error(make_error(parser_error::error_mode::e_syntax, current_token(),
                                     "ERR165 - Expected ':=' as part of vector definition",
                                     core::error_location()));

                return error_node();
            }
            else if (token_is(token_t::e_lsqrbracket))
            {
                expression_node_ptr initialiser_component = parse_expression();

                if (nullptr == initialiser_component)
                {
                    set_error(make_error(parser_error::error_mode::e_syntax, current_token(),
                                         "ERR166 - Failed to parse first component of vector "
                                         "initialiser for vector: " +
                                             vec_name,
                                         core::error_location()));

                    return error_node();
                }

                vec_initilizer_list.push_back(initialiser_component);

                if (token_is(token_t::e_colon))
                {
                    initialiser_component = parse_expression();

                    if (nullptr == initialiser_component)
                    {
                        set_error(make_error(parser_error::error_mode::e_syntax, current_token(),
                                             "ERR167 - Failed to parse second component of vector "
                                             "initialiser for vector: " +
                                                 vec_name,
                                             core::error_location()));

                        return error_node();
                    }

                    vec_initilizer_list.push_back(initialiser_component);
                }

                if (!token_is(token_t::e_rsqrbracket))
                {
                    set_error(
                        make_error(parser_error::error_mode::e_syntax, current_token(),
                                   "ERR168 - Expected ']' to close single value vector initialiser",
                                   core::error_location()));

                    return error_node();
                }

                switch (vec_initilizer_list.size())
                {
                    case 1:
                        single_value_initialiser = true;
                        break;
                    case 2:
                        range_value_initialiser = true;
                        break;
                }
            }
            else if (!token_is(token_t::e_lcrlbracket))
            {
                expression_node_ptr initialiser = error_node();

                // Is this a vector to vector assignment and initialisation?
                if (token_t::e_symbol == current_token().type)
                {
                    // Is it a locally defined vector?
                    const scope_element& lcl_se = sem_.get_active_element(current_token().value);

                    if (scope_element::element_type::e_vector == lcl_se.type)
                    {
                        if (nullptr != (initialiser = parse_expression()))
                            vec_initilizer_list.push_back(initialiser);
                        else
                            return error_node();
                    }
                    // Are we dealing with a user defined vector?
                    else if (symtab_store_.is_vector(current_token().value))
                    {
                        lodge_symbol(current_token().value, symbol_type::e_st_vector);

                        if (nullptr != (initialiser = parse_expression()))
                            vec_initilizer_list.push_back(initialiser);
                        else
                            return error_node();
                    }
                    // Are we dealing with a null initialisation vector definition?
                    else if (token_is(token_t::e_symbol, "null"))
                        null_initialisation = true;
                }

                if (!null_initialisation)
                {
                    if (nullptr == initialiser)
                    {
                        set_error(
                            make_error(parser_error::error_mode::e_syntax, current_token(),
                                       "ERR169 - Expected '{' as part of vector initialiser list",
                                       core::error_location()));

                        return error_node();
                    }
                    else
                        vec_to_vec_initialiser = true;
                }
            }
            else if (!token_is(token_t::e_rcrlbracket))
            {
                for (;;)
                {
                    expression_node_ptr initialiser = parse_expression();

                    if (nullptr == initialiser)
                    {
                        set_error(
                            make_error(parser_error::error_mode::e_syntax, current_token(),
                                       "ERR170 - Expected '{' as part of vector initialiser list",
                                       core::error_location()));

                        return error_node();
                    }
                    else
                        vec_initilizer_list.push_back(initialiser);

                    if (token_is(token_t::e_rcrlbracket))
                        break;

                    const bool is_next_close = peek_token_is(token_t::e_rcrlbracket);

                    if (!token_is(token_t::e_comma) && is_next_close)
                    {
                        set_error(make_error(parser_error::error_mode::e_syntax, current_token(),
                                             "ERR171 - Expected ',' between vector initialisers",
                                             core::error_location()));

                        return error_node();
                    }

                    if (token_is(token_t::e_rcrlbracket))
                        break;
                }
            }

            if (!token_is(token_t::e_rbracket, prsrhlpr_t::token_advance_mode::e_hold) &&
                !token_is(token_t::e_rcrlbracket, prsrhlpr_t::token_advance_mode::e_hold) &&
                !token_is(token_t::e_rsqrbracket, prsrhlpr_t::token_advance_mode::e_hold))
            {
                if (!token_is(token_t::e_eof, prsrhlpr_t::token_advance_mode::e_hold))
                {
                    set_error(make_error(parser_error::error_mode::e_syntax, current_token(),
                                         "ERR172 - Expected ';' at end of vector definition",
                                         core::error_location()));

                    return error_node();
                }
            }

            if (!single_value_initialiser && !range_value_initialiser &&
                (T(vec_initilizer_list.size()) > vector_size))
            {
                set_error(make_error(parser_error::error_mode::e_syntax, current_token(),
                                     "ERR173 - Initialiser list larger than the number of elements "
                                     "in the vector: '" +
                                         vec_name + "'",
                                     core::error_location()));

                return error_node();
            }
        }

        expression_node_ptr result = error_node();

        if ((vec_initilizer_list.size() == 1) && single_value_initialiser)
        {
            if (details::is_constant_node(vec_initilizer_list[0]))
            {
                // vector_init_zero_value_node   var v[10] := [0]
                if (T(0) == vec_initilizer_list[0]->value())
                {
                    result = node_allocator_.allocate<details::vector_init_zero_value_node<T>>(
                        (*vec_holder)[0], vec_size, vec_initilizer_list);
                }
                else
                {
                    // vector_init_single_constvalue_node   var v[10] := [123]
                    result =
                        node_allocator_.allocate<details::vector_init_single_constvalue_node<T>>(
                            (*vec_holder)[0], vec_size, vec_initilizer_list);
                }
            }
            else
            {
                // vector_init_single_value_node   var v[10] := [123 + (x / y)]
                result = node_allocator_.allocate<details::vector_init_single_value_node<T>>(
                    (*vec_holder)[0], vec_size, vec_initilizer_list);
            }
        }
        else if ((vec_initilizer_list.size() == 2) && range_value_initialiser)
        {
            bool base_const = details::is_constant_node(vec_initilizer_list[0]);
            bool inc_const = details::is_constant_node(vec_initilizer_list[1]);

            if (base_const && inc_const)
            {
                // vector_init_single_value_node   var v[10] := [1 : 3.5]
                result = node_allocator_.allocate<details::vector_init_iota_constconst_node<T>>(
                    (*vec_holder)[0], vec_size, vec_initilizer_list);
            }
            else if (base_const && !inc_const)
            {
                // vector_init_single_value_node   var v[10] := [1 : x + y]
                result = node_allocator_.allocate<details::vector_init_iota_constnconst_node<T>>(
                    (*vec_holder)[0], vec_size, vec_initilizer_list);
            }
            else if (!base_const && inc_const)
            {
                // vector_init_single_value_node   var v[10] := [x + y : 3]
                result = node_allocator_.allocate<details::vector_init_iota_nconstconst_node<T>>(
                    (*vec_holder)[0], vec_size, vec_initilizer_list);
            }
            else if (!base_const && !inc_const)
            {
                // vector_init_single_value_node   var v[10] := [x + y :  z / w]
                result = node_allocator_.allocate<details::vector_init_iota_nconstnconst_node<T>>(
                    (*vec_holder)[0], vec_size, vec_initilizer_list);
            }
        }
        else if (null_initialisation)
            result = expression_generator_(T(0.0));
        else if (vec_to_vec_initialiser)
        {
            expression_node_ptr vec_node = node_allocator_.allocate<vector_node_t>(vec_holder);

            result = expression_generator_(core::operators::operator_type::assign, vec_node,
                                           vec_initilizer_list[0]);
        }
        else
        {
            result = node_allocator_.allocate<details::vector_initialisation_node<T>>(
                (*vec_holder)[0], vec_size, vec_initilizer_list, single_value_initialiser);
        }

        svd.delete_ptr = false;

        if (result && result->valid())
        {
            return result;
        }

        details::free_node(node_allocator_, result);

        set_error(
            make_error(parser_error::error_mode::e_synthesis, current_token(),
                       "ERR174 - Failed to generate initialisation node for vector: " + vec_name,
                       core::error_location()));

        return error_node();
    }

#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
    inline expression_node_ptr parse_define_string_statement(
        const std::string& str_name, expression_node_ptr initialisation_expression)
    {
        stringvar_node_t* str_node = nullptr;

        scope_element& se = sem_.get_element(str_name);

        if (se.name == str_name)
        {
            if (se.active)
            {
                set_error(make_error(
                    parser_error::error_mode::e_syntax, current_token(),
                    "ERR175 - Illegal redefinition of local variable: '" + str_name + "'",
                    core::error_location()));

                free_node(node_allocator_, initialisation_expression);

                return error_node();
            }
            else if (scope_element::element_type::e_string == se.type)
            {
                str_node = se.str_node;
                se.active = true;
                se.depth = state_.scope_depth;
                se.ref_count++;
            }
        }

        if (nullptr == str_node)
        {
            scope_element nse;
            nse.name = str_name;
            nse.active = true;
            nse.ref_count = 1;
            nse.type = scope_element::element_type::e_string;
            nse.depth = state_.scope_depth;
            nse.str_data = std::make_unique<std::string>();
            nse.str_node = new stringvar_node_t(*nse.str_data);

            if (!sem_.add_element(std::move(nse)))
            {
                set_error(make_error(
                    parser_error::error_mode::e_syntax, current_token(),
                    "ERR176 - Failed to add new local string variable '" + str_name + "' to SEM",
                    core::error_location()));

                free_node(node_allocator_, initialisation_expression);

                sem_.free_element(nse);

                return error_node();
            }

            assert(sem_.total_local_symb_size_bytes() <=
                   settings().max_total_local_symbol_size_bytes());

            str_node = nse.str_node;

            core::debug_print(
                "parse_define_string_statement() - INFO - Added new local string variable: %s\n",
                nse.name.c_str());
        }

        lodge_symbol(str_name, symbol_type::e_st_local_string);

        state_.activate_side_effect("parse_define_string_statement()");

        expression_node_ptr branch[2] = {0};

        branch[0] = str_node;
        branch[1] = initialisation_expression;

        return expression_generator_(core::operators::operator_type::assign, branch);
    }
#else
    inline expression_node_ptr parse_define_string_statement(const std::string&,
                                                             expression_node_ptr)
    {
        return error_node();
    }
#endif

    inline bool local_variable_is_shadowed(const std::string& symbol)
    {
        const scope_element& se = sem_.get_element(symbol);
        return (se.name == symbol) && se.active;
    }

    inline expression_node_ptr parse_define_var_statement()
    {
        if (settings_.vardef_disabled())
        {
            set_error(make_error(parser_error::error_mode::e_syntax, current_token(),
                                 "ERR177 - Illegal variable definition", core::error_location()));

            return error_node();
        }
        else if (!core::imatch(current_token().value, "var"))
        {
            return error_node();
        }
        else
            next_token();

        const std::string var_name = current_token().value;

        expression_node_ptr initialisation_expression = error_node();

        if (!token_is(token_t::e_symbol))
        {
            set_error(make_error(parser_error::error_mode::e_syntax, current_token(),
                                 "ERR178 - Expected a symbol for variable definition",
                                 core::error_location()));

            return error_node();
        }
        else if (core::is_reserved_symbol(var_name))
        {
            set_error(
                make_error(parser_error::error_mode::e_syntax, current_token(),
                           "ERR179 - Illegal redefinition of reserved keyword: '" + var_name + "'",
                           core::error_location()));

            return error_node();
        }
        else if (symtab_store_.symbol_exists(var_name))
        {
            set_error(make_error(parser_error::error_mode::e_syntax, current_token(),
                                 "ERR180 - Illegal redefinition of variable '" + var_name + "'",
                                 core::error_location()));

            return error_node();
        }
        else if (local_variable_is_shadowed(var_name))
        {
            set_error(
                make_error(parser_error::error_mode::e_syntax, current_token(),
                           "ERR181 - Illegal redefinition of local variable: '" + var_name + "'",
                           core::error_location()));

            return error_node();
        }
        else if (token_is(token_t::e_lsqrbracket, prsrhlpr_t::token_advance_mode::e_hold))
        {
            return parse_define_vector_statement(var_name);
        }
        else if (token_is(token_t::e_lcrlbracket, prsrhlpr_t::token_advance_mode::e_hold))
        {
            return parse_uninitialised_var_statement(var_name);
        }
        else if (token_is(token_t::e_assign))
        {
            if (nullptr == (initialisation_expression = parse_expression()))
            {
                set_error(
                    make_error(parser_error::error_mode::e_syntax, current_token(),
                               "ERR182 - Failed to parse initialisation expression for variable '" +
                                   var_name + "'",
                               core::error_location()));

                return error_node();
            }
        }

        if (!token_is(token_t::e_rbracket, prsrhlpr_t::token_advance_mode::e_hold) &&
            !token_is(token_t::e_rcrlbracket, prsrhlpr_t::token_advance_mode::e_hold) &&
            !token_is(token_t::e_rsqrbracket, prsrhlpr_t::token_advance_mode::e_hold))
        {
            if (!token_is(token_t::e_eof, prsrhlpr_t::token_advance_mode::e_hold))
            {
                set_error(
                    make_error(parser_error::error_mode::e_syntax, current_token(),
                               "ERR183 - Expected ';' after variable '" + var_name + "' definition",
                               core::error_location()));

                free_node(node_allocator_, initialisation_expression);

                return error_node();
            }
        }

        if ((nullptr != initialisation_expression) &&
            details::is_generally_string_node(initialisation_expression))
        {
            return parse_define_string_statement(var_name, initialisation_expression);
        }

        expression_node_ptr var_node = nullptr;

        scope_element& se = sem_.get_element(var_name);

        if (se.name == var_name)
        {
            if (se.active)
            {
                set_error(make_error(
                    parser_error::error_mode::e_syntax, current_token(),
                    "ERR184 - Illegal redefinition of local variable: '" + var_name + "'",
                    core::error_location()));

                free_node(node_allocator_, initialisation_expression);

                return error_node();
            }
            else if (scope_element::element_type::e_variable == se.type)
            {
                var_node = se.var_node;
                se.active = true;
                se.depth = state_.scope_depth;
                se.ref_count++;
            }
        }

        if (nullptr == var_node)
        {
            const std::size_t predicted_total_lclsymb_size =
                sizeof(T) + sem_.total_local_symb_size_bytes();

            if (predicted_total_lclsymb_size > settings().max_total_local_symbol_size_bytes())
            {
                set_error(
                    make_error(parser_error::error_mode::e_syntax, current_token(),
                               "ERR185 - Adding variable '" + var_name +
                                   "' "
                                   "will exceed max total local symbol size of: " +
                                   core::to_str(settings().max_total_local_symbol_size_bytes()) +
                                   " bytes, "
                                   "current total size: " +
                                   core::to_str(sem_.total_local_symb_size_bytes()) + " bytes",
                               core::error_location()));

                free_node(node_allocator_, initialisation_expression);

                return error_node();
            }

            scope_element nse;
            nse.name = var_name;
            nse.active = true;
            nse.ref_count = 1;
            nse.type = scope_element::element_type::e_variable;
            nse.depth = state_.scope_depth;
            nse.scalar_data = std::make_unique<T>(T(0));
            nse.var_node = node_allocator_.allocate<variable_node_t>(*nse.scalar_data);

            if (!sem_.add_element(std::move(nse)))
            {
                set_error(make_error(
                    parser_error::error_mode::e_syntax, current_token(),
                    "ERR186 - Failed to add new local variable '" + var_name + "' to SEM",
                    core::error_location()));

                free_node(node_allocator_, initialisation_expression);

                sem_.free_element(nse);

                return error_node();
            }

            assert(sem_.total_local_symb_size_bytes() <=
                   settings().max_total_local_symbol_size_bytes());

            var_node = nse.var_node;

            core::debug_print(
                "parse_define_var_statement() - INFO - Added new local variable: %s\n",
                nse.name.c_str());
        }

        state_.activate_side_effect("parse_define_var_statement()");

        lodge_symbol(var_name, symbol_type::e_st_local_variable);

        expression_node_ptr branch[2] = {0};

        branch[0] = var_node;
        branch[1] =
            initialisation_expression ? initialisation_expression : expression_generator_(T(0));

        return expression_generator_(core::operators::operator_type::assign, branch);
    }

    inline expression_node_ptr parse_define_constvar_statement()
    {
        if (settings_.vardef_disabled())
        {
            set_error(make_error(parser_error::error_mode::e_syntax, current_token(),
                                 "ERR187 - Illegal const variable definition",
                                 core::error_location()));

            return error_node();
        }
        else if (!token_is("const"))
        {
            set_error(make_error(parser_error::error_mode::e_syntax, current_token(),
                                 "ERR188 - Expected 'const' keyword for const-variable definition",
                                 core::error_location()));

            return error_node();
        }
        else if (!token_is("var"))
        {
            set_error(make_error(parser_error::error_mode::e_syntax, current_token(),
                                 "ERR189 - Expected 'var' keyword for const-variable definition",
                                 core::error_location()));

            return error_node();
        }

        const std::string var_name = current_token().value;

        expression_node_ptr initialisation_expression = error_node();

        if (!token_is(token_t::e_symbol))
        {
            set_error(make_error(parser_error::error_mode::e_syntax, current_token(),
                                 "ERR190 - Expected a symbol for const-variable definition",
                                 core::error_location()));

            return error_node();
        }
        else if (core::is_reserved_symbol(var_name))
        {
            set_error(
                make_error(parser_error::error_mode::e_syntax, current_token(),
                           "ERR191 - Illegal redefinition of reserved keyword: '" + var_name + "'",
                           core::error_location()));

            return error_node();
        }
        else if (symtab_store_.symbol_exists(var_name))
        {
            set_error(make_error(parser_error::error_mode::e_syntax, current_token(),
                                 "ERR192 - Illegal redefinition of variable '" + var_name + "'",
                                 core::error_location()));

            return error_node();
        }
        else if (local_variable_is_shadowed(var_name))
        {
            set_error(
                make_error(parser_error::error_mode::e_syntax, current_token(),
                           "ERR193 - Illegal redefinition of local variable: '" + var_name + "'",
                           core::error_location()));

            return error_node();
        }
        else if (!token_is(token_t::e_assign))
        {
            set_error(make_error(parser_error::error_mode::e_syntax, current_token(),
                                 "ERR194 - Expected assignment operator after const-variable: '" +
                                     var_name + "' definition",
                                 core::error_location()));

            return error_node();
        }
        else if (nullptr == (initialisation_expression = parse_expression()))
        {
            set_error(make_error(
                parser_error::error_mode::e_syntax, current_token(),
                "ERR195 - Failed to parse initialisation expression for const-variable: '" +
                    var_name + "'",
                core::error_location()));

            return error_node();
        }

        if (!details::is_literal_node(initialisation_expression))
        {
            set_error(make_error(parser_error::error_mode::e_syntax, current_token(),
                                 "ERR196 - initialisation expression for const-variable: '" +
                                     var_name + "' must be a constant/literal",
                                 core::error_location()));

            free_node(node_allocator_, initialisation_expression);

            return error_node();
        }

        assert(initialisation_expression);

        const T init_value = initialisation_expression->value();

        free_node(node_allocator_, initialisation_expression);

        expression_node_ptr var_node = nullptr;

        scope_element& se = sem_.get_element(var_name);

        if (se.name == var_name)
        {
            if (se.active)
            {
                set_error(make_error(
                    parser_error::error_mode::e_syntax, current_token(),
                    "ERR197 - Illegal redefinition of local variable: '" + var_name + "'",
                    core::error_location()));

                return error_node();
            }
            else if (scope_element::element_type::e_literal == se.type)
            {
                var_node = se.var_node;
                se.active = true;
                se.depth = state_.scope_depth;
                se.ref_count++;
            }
        }

        if (nullptr == var_node)
        {
            const std::size_t predicted_total_lclsymb_size =
                sizeof(T) + sem_.total_local_symb_size_bytes();

            if (predicted_total_lclsymb_size > settings().max_total_local_symbol_size_bytes())
            {
                set_error(
                    make_error(parser_error::error_mode::e_syntax, current_token(),
                               "ERR198 - Adding variable '" + var_name +
                                   "' "
                                   "will exceed max total local symbol size of: " +
                                   core::to_str(settings().max_total_local_symbol_size_bytes()) +
                                   " bytes, "
                                   "current total size: " +
                                   core::to_str(sem_.total_local_symb_size_bytes()) + " bytes",
                               core::error_location()));

                return error_node();
            }

            scope_element nse;
            nse.name = var_name;
            nse.active = true;
            nse.ref_count = 1;
            nse.type = scope_element::element_type::e_literal;
            nse.depth = state_.scope_depth;
            nse.var_node = node_allocator_.allocate<literal_node_t>(init_value);

            if (!sem_.add_element(std::move(nse)))
            {
                set_error(make_error(
                    parser_error::error_mode::e_syntax, current_token(),
                    "ERR199 - Failed to add new local const-variable '" + var_name + "' to SEM",
                    core::error_location()));

                sem_.free_element(nse);

                return error_node();
            }

            assert(sem_.total_local_symb_size_bytes() <=
                   settings().max_total_local_symbol_size_bytes());

            var_node = nse.var_node;

            core::debug_print(
                "parse_define_constvar_statement() - INFO - Added new local const-variable: %s\n",
                nse.name.c_str());
        }

        state_.activate_side_effect("parse_define_constvar_statement()");

        lodge_symbol(var_name, symbol_type::e_st_local_variable);

        return expression_generator_(var_node->value());
    }

    inline expression_node_ptr parse_uninitialised_var_statement(const std::string& var_name)
    {
        if (!token_is(token_t::e_lcrlbracket) || !token_is(token_t::e_rcrlbracket))
        {
            set_error(make_error(parser_error::error_mode::e_syntax, current_token(),
                                 "ERR200 - Expected a '{}' for uninitialised var definition",
                                 core::error_location()));

            return error_node();
        }
        else if (!token_is(token_t::e_eof, prsrhlpr_t::token_advance_mode::e_hold))
        {
            set_error(make_error(parser_error::error_mode::e_syntax, current_token(),
                                 "ERR201 - Expected ';' after uninitialised variable definition",
                                 core::error_location()));

            return error_node();
        }

        expression_node_ptr var_node = nullptr;

        scope_element& se = sem_.get_element(var_name);

        if (se.name == var_name)
        {
            if (se.active)
            {
                set_error(make_error(
                    parser_error::error_mode::e_syntax, current_token(),
                    "ERR202 - Illegal redefinition of local variable: '" + var_name + "'",
                    core::error_location()));

                return error_node();
            }
            else if (scope_element::element_type::e_variable == se.type)
            {
                var_node = se.var_node;
                se.active = true;
                se.ref_count++;
            }
        }

        if (nullptr == var_node)
        {
            const std::size_t predicted_total_lclsymb_size =
                sizeof(T) + sem_.total_local_symb_size_bytes();

            if (predicted_total_lclsymb_size > settings().max_total_local_symbol_size_bytes())
            {
                set_error(
                    make_error(parser_error::error_mode::e_syntax, current_token(),
                               "ERR203 - Adding variable '" + var_name +
                                   "' "
                                   "will exceed max total local symbol size of: " +
                                   core::to_str(settings().max_total_local_symbol_size_bytes()) +
                                   " bytes, "
                                   "current total size: " +
                                   core::to_str(sem_.total_local_symb_size_bytes()) + " bytes",
                               core::error_location()));

                return error_node();
            }

            scope_element nse;
            nse.name = var_name;
            nse.active = true;
            nse.ref_count = 1;
            nse.type = scope_element::element_type::e_variable;
            nse.depth = state_.scope_depth;
            nse.ip_index = sem_.next_ip_index();
            nse.scalar_data = std::make_unique<T>(T(0));
            nse.var_node = node_allocator_.allocate<variable_node_t>(*nse.scalar_data);

            if (!sem_.add_element(std::move(nse)))
            {
                set_error(make_error(
                    parser_error::error_mode::e_syntax, current_token(),
                    "ERR204 - Failed to add new local variable '" + var_name + "' to SEM",
                    core::error_location()));

                sem_.free_element(nse);

                return error_node();
            }

            assert(sem_.total_local_symb_size_bytes() <=
                   settings().max_total_local_symbol_size_bytes());

            core::debug_print(
                "parse_uninitialised_var_statement() - INFO - Added new local variable: %s\n",
                nse.name.c_str());
        }

        lodge_symbol(var_name, symbol_type::e_st_local_variable);

        state_.activate_side_effect("parse_uninitialised_var_statement()");

        return expression_generator_(T(0));
    }

    inline expression_node_ptr parse_swap_statement()
    {
        if (!core::imatch(current_token().value, "swap"))
        {
            return error_node();
        }
        else
            next_token();

        if (!token_is(token_t::e_lbracket))
        {
            set_error(make_error(parser_error::error_mode::e_syntax, current_token(),
                                 "ERR205 - Expected '(' at start of swap statement",
                                 core::error_location()));

            return error_node();
        }

        expression_node_ptr variable0 = error_node();
        expression_node_ptr variable1 = error_node();

        bool variable0_generated = false;
        bool variable1_generated = false;

        const std::string var0_name = current_token().value;

        if (!token_is(token_t::e_symbol, prsrhlpr_t::token_advance_mode::e_hold))
        {
            set_error(
                make_error(parser_error::error_mode::e_syntax, current_token(),
                           "ERR206 - Expected a symbol for variable or vector element definition",
                           core::error_location()));

            return error_node();
        }
        else if (peek_token_is(token_t::e_lsqrbracket))
        {
            if (nullptr == (variable0 = parse_vector()))
            {
                set_error(
                    make_error(parser_error::error_mode::e_syntax, current_token(),
                               "ERR207 - First parameter to swap is an invalid vector element: '" +
                                   var0_name + "'",
                               core::error_location()));

                return error_node();
            }

            variable0_generated = true;
        }
        else
        {
            if (symtab_store_.is_variable(var0_name))
            {
                variable0 = symtab_store_.get_variable(var0_name);
            }

            const scope_element& se = sem_.get_element(var0_name);

            if ((se.active) && (se.name == var0_name) &&
                (scope_element::element_type::e_variable == se.type))
            {
                variable0 = se.var_node;
            }

            lodge_symbol(var0_name, symbol_type::e_st_variable);

            if (nullptr == variable0)
            {
                set_error(make_error(
                    parser_error::error_mode::e_syntax, current_token(),
                    "ERR208 - First parameter to swap is an invalid variable: '" + var0_name + "'",
                    core::error_location()));

                return error_node();
            }
            else
                next_token();
        }

        if (!token_is(token_t::e_comma))
        {
            set_error(make_error(parser_error::error_mode::e_syntax, current_token(),
                                 "ERR209 - Expected ',' between parameters to swap",
                                 core::error_location()));

            if (variable0_generated)
            {
                free_node(node_allocator_, variable0);
            }

            return error_node();
        }

        const std::string var1_name = current_token().value;

        if (!token_is(token_t::e_symbol, prsrhlpr_t::token_advance_mode::e_hold))
        {
            set_error(
                make_error(parser_error::error_mode::e_syntax, current_token(),
                           "ERR210 - Expected a symbol for variable or vector element definition",
                           core::error_location()));

            if (variable0_generated)
            {
                free_node(node_allocator_, variable0);
            }

            return error_node();
        }
        else if (peek_token_is(token_t::e_lsqrbracket))
        {
            if (nullptr == (variable1 = parse_vector()))
            {
                set_error(
                    make_error(parser_error::error_mode::e_syntax, current_token(),
                               "ERR211 - Second parameter to swap is an invalid vector element: '" +
                                   var1_name + "'",
                               core::error_location()));

                if (variable0_generated)
                {
                    free_node(node_allocator_, variable0);
                }

                return error_node();
            }

            variable1_generated = true;
        }
        else
        {
            if (symtab_store_.is_variable(var1_name))
            {
                variable1 = symtab_store_.get_variable(var1_name);
            }

            const scope_element& se = sem_.get_element(var1_name);

            if ((se.active) && (se.name == var1_name) &&
                (scope_element::element_type::e_variable == se.type))
            {
                variable1 = se.var_node;
            }

            lodge_symbol(var1_name, symbol_type::e_st_variable);

            if (nullptr == variable1)
            {
                set_error(make_error(
                    parser_error::error_mode::e_syntax, current_token(),
                    "ERR212 - Second parameter to swap is an invalid variable: '" + var1_name + "'",
                    core::error_location()));

                if (variable0_generated)
                {
                    free_node(node_allocator_, variable0);
                }

                return error_node();
            }
            else
                next_token();
        }

        if (!token_is(token_t::e_rbracket))
        {
            set_error(make_error(parser_error::error_mode::e_syntax, current_token(),
                                 "ERR213 - Expected ')' at end of swap statement",
                                 core::error_location()));

            if (variable0_generated)
            {
                free_node(node_allocator_, variable0);
            }

            if (variable1_generated)
            {
                free_node(node_allocator_, variable1);
            }

            return error_node();
        }

        using variable_node_ptr = details::variable_node<T>*;

        variable_node_ptr v0 = variable_node_ptr(0);
        variable_node_ptr v1 = variable_node_ptr(0);

        expression_node_ptr result = error_node();

        if ((nullptr != (v0 = static_cast<variable_node_ptr>(variable0->as_variable_node()))) &&
            (nullptr != (v1 = static_cast<variable_node_ptr>(variable1->as_variable_node()))))
        {
            result = node_allocator_.allocate<details::swap_node<T>>(v0, v1);

            if (variable0_generated)
            {
                free_node(node_allocator_, variable0);
            }

            if (variable1_generated)
            {
                free_node(node_allocator_, variable1);
            }
        }
        else
            result = node_allocator_.allocate<details::swap_generic_node<T>>(variable0, variable1);

        state_.activate_side_effect("parse_swap_statement()");

        return result;
    }

#ifndef MATH_EXPR_DISABLE_RETURN_STATEMENT
    inline expression_node_ptr parse_return_statement()
    {
        if (state_.parsing_return_stmt)
        {
            set_error(make_error(parser_error::error_mode::e_syntax, current_token(),
                                 "ERR214 - Return call within a return call is not allowed",
                                 core::error_location()));

            return error_node();
        }

        scoped_bool_negator sbn(state_.parsing_return_stmt);

        std::vector<expression_node_ptr> arg_list;

        scoped_vec_delete<expression_node_t> svd((*this), arg_list);

        if (!core::imatch(current_token().value, "return"))
        {
            return error_node();
        }
        else
            next_token();

        if (!token_is(token_t::e_lsqrbracket))
        {
            set_error(make_error(parser_error::error_mode::e_syntax, current_token(),
                                 "ERR215 - Expected '[' at start of return statement",
                                 core::error_location()));

            return error_node();
        }
        else if (!token_is(token_t::e_rsqrbracket))
        {
            for (;;)
            {
                expression_node_ptr arg = parse_expression();

                if (nullptr == arg)
                    return error_node();

                arg_list.push_back(arg);

                if (token_is(token_t::e_rsqrbracket))
                    break;
                else if (!token_is(token_t::e_comma))
                {
                    set_error(
                        make_error(parser_error::error_mode::e_syntax, current_token(),
                                   "ERR216 - Expected ',' between values during call to return",
                                   core::error_location()));

                    return error_node();
                }
            }
        }
        else if (settings_.zero_return_disabled())
        {
            set_error(make_error(parser_error::error_mode::e_syntax, current_token(),
                                 "ERR217 - Zero parameter return statement not allowed",
                                 core::error_location()));

            return error_node();
        }

        const lexer::token prev_token = current_token();

        if (token_is(token_t::e_rsqrbracket))
        {
            if (!arg_list.empty())
            {
                set_error(make_error(parser_error::error_mode::e_syntax, prev_token,
                                     "ERR218 - Invalid ']' found during return call",
                                     core::error_location()));

                return error_node();
            }
        }

        std::string ret_param_type_list;

        for (std::size_t i = 0; i < arg_list.size(); ++i)
        {
            if (nullptr == arg_list[i])
                return error_node();
            else if (is_ivector_node(arg_list[i]))
                ret_param_type_list += 'V';
            else if (is_generally_string_node(arg_list[i]))
                ret_param_type_list += 'S';
            else
                ret_param_type_list += 'T';
        }

        dec_.retparam_list_.push_back(ret_param_type_list);

        expression_node_ptr result = expression_generator_.return_call(arg_list);

        svd.delete_ptr = (nullptr == result);

        state_.return_stmt_present = true;

        state_.activate_side_effect("parse_return_statement()");

        return result;
    }
#else
    inline expression_node_ptr parse_return_statement()
    {
        return error_node();
    }
#endif

    inline expression_node_ptr parse_assert_statement()
    {
        assert(core::imatch(current_token().value, "assert"));

        if (state_.parsing_assert_stmt)
        {
            set_error(
                make_error(parser_error::error_mode::e_syntax, current_token(),
                           "ERR219 - Assert statement within an assert statement is not allowed",
                           core::error_location()));

            return error_node();
        }

        scoped_bool_negator sbn(state_.parsing_assert_stmt);

        next_token();

        std::vector<expression_node_ptr> assert_arg_list(3, error_node());
        scoped_vec_delete<expression_node_t> svd((*this), assert_arg_list);

        expression_node_ptr& assert_condition = assert_arg_list[0];
        expression_node_ptr& assert_message = assert_arg_list[1];
        expression_node_ptr& assert_id = assert_arg_list[2];

        if (!token_is(token_t::e_lbracket))
        {
            set_error(make_error(parser_error::error_mode::e_syntax, current_token(),
                                 "ERR220 - Expected '(' at start of assert statement",
                                 core::error_location()));

            return error_node();
        }

        const token_t start_token = current_token();

        // Parse the assert condition
        if (nullptr == (assert_condition = parse_expression()))
        {
            set_error(make_error(parser_error::error_mode::e_syntax, current_token(),
                                 "ERR221 - Failed to parse condition for assert statement",
                                 core::error_location()));

            return error_node();
        }

        const token_t end_token = current_token();

        if (!token_is(token_t::e_rbracket))
        {
            if (!token_is(token_t::e_comma))
            {
                set_error(make_error(
                    parser_error::error_mode::e_syntax, current_token(),
                    "ERR222 - Expected ',' between condition and message for assert statement",
                    core::error_location()));

                return error_node();
            }
            // Parse the assert message
            else if ((nullptr == (assert_message = parse_expression())) ||
                     !details::is_generally_string_node(assert_message))
            {
                set_error(make_error(
                    parser_error::error_mode::e_syntax, current_token(),
                    "ERR223 - " +
                        (assert_message
                             ? std::string("Expected string for assert message")
                             : std::string("Failed to parse message for assert statement")),
                    core::error_location()));

                return error_node();
            }
            else if (!token_is(token_t::e_rbracket))
            {
                if (!token_is(token_t::e_comma))
                {
                    set_error(make_error(
                        parser_error::error_mode::e_syntax, current_token(),
                        "ERR224 - Expected ',' between message and ID for assert statement",
                        core::error_location()));

                    return error_node();
                }
                // Parse assert ID
                else if ((nullptr == (assert_id = parse_expression())) ||
                         !details::is_const_string_node(assert_id))
                {
                    set_error(make_error(
                        parser_error::error_mode::e_syntax, current_token(),
                        "ERR225 - " + (assert_id
                                           ? std::string("Expected literal string for assert ID")
                                           : std::string("Failed to parse string for assert ID")),
                        core::error_location()));

                    return error_node();
                }
                else if (!token_is(token_t::e_rbracket))
                {
                    set_error(make_error(parser_error::error_mode::e_syntax, current_token(),
                                         "ERR226 - Expected ')' at start of assert statement",
                                         core::error_location()));

                    return error_node();
                }
            }
        }

        math_expr::assert_check::assert_context context;
        context.condition = lexer().substr(start_token.position, end_token.position);
        context.offet = start_token.position;

        if (nullptr == assert_check_)
        {
            core::debug_print(
                "parse_assert_statement() - assert functionality is disabled. assert "
                "condition: %s\n",
                context.condition.c_str());

            return new details::null_node<T>();
        }

#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
        if (assert_message && details::is_const_string_node(assert_message))
        {
            auto* sbn_msg = assert_message->as_string_base();
            assert(sbn_msg);
            context.message = sbn_msg->str();
        }

        if (assert_id && details::is_const_string_node(assert_id))
        {
            auto* sbn_id = assert_id->as_string_base();
            assert(sbn_id);
            context.id = sbn_id->str();

            if (assert_ids_.end() != assert_ids_.find(context.id))
            {
                set_error(make_error(parser_error::error_mode::e_syntax, current_token(),
                                     "ERR227 - Duplicate assert ID: " + context.id,
                                     core::error_location()));

                return error_node();
            }

            assert_ids_.insert(context.id);
            free_node(node_allocator_, assert_id);
        }
#endif

        expression_node_ptr result_node =
            expression_generator_.assert_call(assert_condition, assert_message, context);

        core::debug_print("parse_assert_statement() - assert condition: [%s]\n",
                          context.condition.c_str());
        core::debug_print("parse_assert_statement() - assert message:   [%s]\n",
                          context.message.c_str());
        core::debug_print("parse_assert_statement() - assert id:        [%s]\n",
                          context.id.c_str());
        core::debug_print("parse_assert_statement() - assert offset:    [%d]\n",
                          static_cast<int>(context.offet));

        if (nullptr == result_node)
        {
            set_error(make_error(parser_error::error_mode::e_syntax, current_token(),
                                 "ERR228 - Failed to synthesize assert", core::error_location()));

            return error_node();
        }

        svd.delete_ptr = false;
        return result_node;
    }

    inline bool post_variable_process(const std::string& symbol)
    {
        if (peek_token_is(token_t::e_lbracket) || peek_token_is(token_t::e_lcrlbracket) ||
            peek_token_is(token_t::e_lsqrbracket))
        {
            if (!settings_.commutative_check_enabled())
            {
                set_error(
                    make_error(parser_error::error_mode::e_syntax, current_token(),
                               "ERR229 - Invalid sequence of variable '" + symbol + "' and bracket",
                               core::error_location()));

                return false;
            }

            lexer().insert_front(token_t::e_mul);
        }

        return true;
    }

    inline bool post_bracket_process(const typename token_t::token_type& token,
                                     expression_node_ptr& branch)
    {
        bool implied_mul = false;

        if (details::is_generally_string_node(branch))
            return true;

        if (details::is_ivector_node(branch))
            return true;

        const lexer::parser_helper::token_advance_mode hold =
            prsrhlpr_t::token_advance_mode::e_hold;

        switch (token)
        {
            case token_t::e_lcrlbracket:
                implied_mul = token_is(token_t::e_lbracket, hold) ||
                              token_is(token_t::e_lcrlbracket, hold) ||
                              token_is(token_t::e_lsqrbracket, hold);
                break;

            case token_t::e_lbracket:
                implied_mul = token_is(token_t::e_lbracket, hold) ||
                              token_is(token_t::e_lcrlbracket, hold) ||
                              token_is(token_t::e_lsqrbracket, hold);
                break;

            case token_t::e_lsqrbracket:
                implied_mul = token_is(token_t::e_lbracket, hold) ||
                              token_is(token_t::e_lcrlbracket, hold) ||
                              token_is(token_t::e_lsqrbracket, hold);
                break;

            default:
                return true;
        }

        if (implied_mul)
        {
            if (!settings_.commutative_check_enabled())
            {
                set_error(make_error(parser_error::error_mode::e_syntax, current_token(),
                                     "ERR230 - Invalid sequence of brackets",
                                     core::error_location()));

                return false;
            }
            else if (token_t::e_eof != current_token().type)
            {
                lexer().insert_front(current_token().type);
                lexer().insert_front(token_t::e_mul);
                next_token();
            }
        }

        return true;
    }

    using interval_t = typename interval_container_t<std::uintptr_t>::interval_t;
    using immutable_memory_map_t = interval_container_t<std::uintptr_t>;
    using immutable_symtok_map_t = std::map<interval_t, token_t>;

    inline interval_t make_memory_range(const T& t)
    {
        const auto addr = reinterpret_cast<std::uintptr_t>(&t);
        return interval_t(addr, addr + sizeof(T));
    }

    inline interval_t make_memory_range(const T* begin, const std::size_t size)
    {
        const auto addr = reinterpret_cast<std::uintptr_t>(begin);
        return interval_t(addr, addr + size * sizeof(T));
    }

    inline interval_t make_memory_range(core::char_cptr begin, const std::size_t size)
    {
        const auto addr = reinterpret_cast<std::uintptr_t>(begin);
        return interval_t(addr, addr + size);
    }

    void lodge_immutable_symbol(const lexer::token& token, const interval_t interval)
    {
        immutable_memory_map_.add_interval(interval);
        immutable_symtok_map_[interval] = token;
    }

    inline expression_node_ptr parse_symtab_symbol()
    {
        symbol_resolution_context context(*this);
        return parser_symbol_resolution<T>::parse_symtab_symbol(context);
    }

    inline expression_node_ptr check_block_statement_closure(expression_node_ptr expression)
    {
        if (expression && ((current_token().type == token_t::e_symbol) ||
                           (current_token().type == token_t::e_number)))
        {
            free_node(node_allocator_, expression);

            set_error(make_error(parser_error::error_mode::e_syntax, current_token(),
                                 "ERR240 - Invalid syntax '" + current_token().value +
                                     "' possible missing operator or context",
                                 core::error_location()));

            return error_node();
        }

        return expression;
    }

    inline expression_node_ptr parse_symbol()
    {
        symbol_context context(*this);
        return parser_symbol<T>::parse_symbol(context);
    }

    inline expression_node_ptr parse_branch(
        precedence_level precedence = precedence_level::e_level00)
    {
        stack_limit_handler slh(*this);

        if (!slh)
        {
            return error_node();
        }

        expression_node_ptr branch = error_node();

        if (token_t::e_number == current_token().type)
        {
            T numeric_value = T(0);

            if (core::numeric::string_to_real(current_token().value, numeric_value))
            {
                expression_node_ptr literal_exp = expression_generator_(numeric_value);

                if (nullptr == literal_exp)
                {
                    set_error(make_error(
                        parser_error::error_mode::e_numeric, current_token(),
                        "ERR242 - Failed generate node for scalar: '" + current_token().value + "'",
                        core::error_location()));

                    return error_node();
                }

                next_token();
                branch = literal_exp;
            }
            else
            {
                set_error(make_error(
                    parser_error::error_mode::e_numeric, current_token(),
                    "ERR243 - Failed to convert '" + current_token().value + "' to a number",
                    core::error_location()));

                return error_node();
            }
        }
        else if (token_t::e_symbol == current_token().type)
        {
            branch = parse_symbol();
        }
#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
        else if (token_t::e_string == current_token().type)
        {
            branch = parse_const_string();
        }
#endif
        else if (token_t::e_lbracket == current_token().type)
        {
            next_token();

            if (nullptr == (branch = parse_expression()))
            {
                return error_node();
            }

            token_is(token_t::e_eof);

            if (!token_is(token_t::e_rbracket))
            {
                set_error(
                    make_error(parser_error::error_mode::e_syntax, current_token(),
                               "ERR244 - Expected ')' instead of: '" + current_token().value + "'",
                               core::error_location()));

                details::free_node(node_allocator_, branch);

                return error_node();
            }
            else if (!post_bracket_process(token_t::e_lbracket, branch))
            {
                details::free_node(node_allocator_, branch);

                return error_node();
            }

            parse_pending_vector_index_operator(branch);
        }
        else if (token_t::e_lsqrbracket == current_token().type)
        {
            next_token();

            if (nullptr == (branch = parse_expression()))
                return error_node();
            else if (!token_is(token_t::e_rsqrbracket))
            {
                set_error(
                    make_error(parser_error::error_mode::e_syntax, current_token(),
                               "ERR245 - Expected ']' instead of: '" + current_token().value + "'",
                               core::error_location()));

                details::free_node(node_allocator_, branch);

                return error_node();
            }
            else if (!post_bracket_process(token_t::e_lsqrbracket, branch))
            {
                details::free_node(node_allocator_, branch);

                return error_node();
            }
        }
        else if (token_t::e_lcrlbracket == current_token().type)
        {
            next_token();

            if (nullptr == (branch = parse_expression()))
                return error_node();
            else if (!token_is(token_t::e_rcrlbracket))
            {
                set_error(
                    make_error(parser_error::error_mode::e_syntax, current_token(),
                               "ERR246 - Expected '}' instead of: '" + current_token().value + "'",
                               core::error_location()));

                details::free_node(node_allocator_, branch);

                return error_node();
            }
            else if (!post_bracket_process(token_t::e_lcrlbracket, branch))
            {
                details::free_node(node_allocator_, branch);

                return error_node();
            }
        }
        else if (token_t::e_sub == current_token().type)
        {
            next_token();
            branch = parse_expression(precedence_level::e_level11);

            if (branch &&
                !(details::is_neg_unary_node(branch) && simplify_unary_negation_branch(branch)))
            {
                expression_node_ptr result =
                    expression_generator_(core::operators::operator_type::neg, branch);

                if (nullptr == result)
                {
                    details::free_node(node_allocator_, branch);

                    return error_node();
                }
                else
                    branch = result;
            }
        }
        else if (token_t::e_add == current_token().type)
        {
            next_token();
            branch = parse_expression(precedence_level::e_level13);
        }
        else if (token_t::e_eof == current_token().type)
        {
            set_error(make_error(parser_error::error_mode::e_syntax, current_token(),
                                 "ERR247 - Premature end of expression[1]",
                                 core::error_location()));

            return error_node();
        }
        else
        {
            set_error(make_error(parser_error::error_mode::e_syntax, current_token(),
                                 "ERR248 - Premature end of expression[2]",
                                 core::error_location()));

            return error_node();
        }

        if (branch && (precedence_level::e_level00 == precedence) &&
            token_is(token_t::e_ternary, prsrhlpr_t::token_advance_mode::e_hold))
        {
            branch = parse_ternary_conditional_statement(branch);
        }

        parse_pending_string_rangesize(branch);

        return branch;
    }

    /**
     * @brief Generates expression nodes for a specific parser numeric type.
     *
     * In normal use, `Type` matches the enclosing `parser<T>` numeric type, so
     * the generated nodes use the same value type as the parser, variables,
     * vectors, and literals handled during parsing.
     *
     * @tparam Type Numeric value type used by generated expression nodes.
     */
    template <typename Type>
    class expression_generator
    {
       public:
        using expression_node_ptr = details::expression_node<Type>*;
        using synthesize_functor_t = expression_node_ptr (*)(
            expression_generator<T>&, const core::operators::operator_type& operation,
            expression_node_ptr (&branch)[2]);
        using synthesize_map_t = std::map<std::string, synthesize_functor_t>;
        using parser_t = typename math_expr::parser<Type>;
        using fold_passes_t = math_expr::fold_passes<Type>;
        using vtype = const Type&;
        using ctype = const Type;

        inline void init_synthesize_map()
        {
#ifndef MATH_EXPR_DISABLE_ENHANCED_FEATURES
            synthesize_map_["(v)o(v)"] = synthesize_vov_expression::process;
            synthesize_map_["(c)o(v)"] = synthesize_cov_expression::process;
            synthesize_map_["(v)o(c)"] = synthesize_voc_expression::process;

#define REGISTER_SYNTHEZIER(S) synthesize_map_[S ::node_type::id()] = S ::process;

            REGISTER_SYNTHEZIER(synthesize_vovov_expression0);
            REGISTER_SYNTHEZIER(synthesize_vovov_expression1);
            REGISTER_SYNTHEZIER(synthesize_vovoc_expression0);
            REGISTER_SYNTHEZIER(synthesize_vovoc_expression1);
            REGISTER_SYNTHEZIER(synthesize_vocov_expression0);
            REGISTER_SYNTHEZIER(synthesize_vocov_expression1);
            REGISTER_SYNTHEZIER(synthesize_covov_expression0);
            REGISTER_SYNTHEZIER(synthesize_covov_expression1);
            REGISTER_SYNTHEZIER(synthesize_covoc_expression0);
            REGISTER_SYNTHEZIER(synthesize_covoc_expression1);
            REGISTER_SYNTHEZIER(synthesize_cocov_expression1);
            REGISTER_SYNTHEZIER(synthesize_vococ_expression0);

            REGISTER_SYNTHEZIER(synthesize_vovovov_expression0);
            REGISTER_SYNTHEZIER(synthesize_vovovoc_expression0);
            REGISTER_SYNTHEZIER(synthesize_vovocov_expression0);
            REGISTER_SYNTHEZIER(synthesize_vocovov_expression0);
            REGISTER_SYNTHEZIER(synthesize_covovov_expression0);
            REGISTER_SYNTHEZIER(synthesize_covocov_expression0);
            REGISTER_SYNTHEZIER(synthesize_vocovoc_expression0);
            REGISTER_SYNTHEZIER(synthesize_covovoc_expression0);
            REGISTER_SYNTHEZIER(synthesize_vococov_expression0);

            REGISTER_SYNTHEZIER(synthesize_vovovov_expression1);
            REGISTER_SYNTHEZIER(synthesize_vovovoc_expression1);
            REGISTER_SYNTHEZIER(synthesize_vovocov_expression1);
            REGISTER_SYNTHEZIER(synthesize_vocovov_expression1);
            REGISTER_SYNTHEZIER(synthesize_covovov_expression1);
            REGISTER_SYNTHEZIER(synthesize_covocov_expression1);
            REGISTER_SYNTHEZIER(synthesize_vocovoc_expression1);
            REGISTER_SYNTHEZIER(synthesize_covovoc_expression1);
            REGISTER_SYNTHEZIER(synthesize_vococov_expression1);

            REGISTER_SYNTHEZIER(synthesize_vovovov_expression2);
            REGISTER_SYNTHEZIER(synthesize_vovovoc_expression2);
            REGISTER_SYNTHEZIER(synthesize_vovocov_expression2);
            REGISTER_SYNTHEZIER(synthesize_vocovov_expression2);
            REGISTER_SYNTHEZIER(synthesize_covovov_expression2);
            REGISTER_SYNTHEZIER(synthesize_covocov_expression2);
            REGISTER_SYNTHEZIER(synthesize_vocovoc_expression2);
            REGISTER_SYNTHEZIER(synthesize_covovoc_expression2);

            REGISTER_SYNTHEZIER(synthesize_vovovov_expression3);
            REGISTER_SYNTHEZIER(synthesize_vovovoc_expression3);
            REGISTER_SYNTHEZIER(synthesize_vovocov_expression3);
            REGISTER_SYNTHEZIER(synthesize_vocovov_expression3);
            REGISTER_SYNTHEZIER(synthesize_covovov_expression3);
            REGISTER_SYNTHEZIER(synthesize_covocov_expression3);
            REGISTER_SYNTHEZIER(synthesize_vocovoc_expression3);
            REGISTER_SYNTHEZIER(synthesize_covovoc_expression3);
            REGISTER_SYNTHEZIER(synthesize_vococov_expression3);

            REGISTER_SYNTHEZIER(synthesize_vovovov_expression4);
            REGISTER_SYNTHEZIER(synthesize_vovovoc_expression4);
            REGISTER_SYNTHEZIER(synthesize_vovocov_expression4);
            REGISTER_SYNTHEZIER(synthesize_vocovov_expression4);
            REGISTER_SYNTHEZIER(synthesize_covovov_expression4);
            REGISTER_SYNTHEZIER(synthesize_covocov_expression4);
            REGISTER_SYNTHEZIER(synthesize_vocovoc_expression4);
            REGISTER_SYNTHEZIER(synthesize_covovoc_expression4);

#undef REGISTER_SYNTHEZIER
#endif
        }

        inline void set_parser(parser_t& p)
        {
            parser_ = &p;
        }

        inline void set_uom(unary_op_map_t& unary_op_map)
        {
            unary_op_map_ = &unary_op_map;
        }

        inline void set_bom(binary_op_map_t& binary_op_map)
        {
            binary_op_map_ = &binary_op_map;
        }

        inline void set_ibom(inv_binary_op_map_t& inv_binary_op_map)
        {
            inv_binary_op_map_ = &inv_binary_op_map;
        }

        inline void set_sf3m(sf3_map_t& sf3_map)
        {
            sf3_map_ = &sf3_map;
        }

        inline void set_sf4m(sf4_map_t& sf4_map)
        {
            sf4_map_ = &sf4_map;
        }

        inline void set_allocator(details::node_allocator& na)
        {
            node_allocator_ = &na;
        }

        inline void set_strength_reduction_state(const bool enabled)
        {
            strength_reduction_enabled_ = enabled;
        }

        inline bool strength_reduction_enabled() const
        {
            return strength_reduction_enabled_;
        }

        inline bool valid_operator(const core::operators::operator_type& operation,
                                   binary_functor_t& bop)
        {
            typename binary_op_map_t::iterator bop_itr = binary_op_map_->find(operation);

            if (binary_op_map_->end() == bop_itr)
                return false;

            bop = bop_itr->second;

            return true;
        }

        inline bool valid_operator(const core::operators::operator_type& operation,
                                   unary_functor_t& uop)
        {
            typename unary_op_map_t::iterator uop_itr = unary_op_map_->find(operation);

            if ((*unary_op_map_).end() == uop_itr)
                return false;

            uop = uop_itr->second;

            return true;
        }

        inline core::operators::operator_type get_operator(const binary_functor_t& bop) const
        {
            return (*inv_binary_op_map_).find(bop)->second;
        }

        inline expression_node_ptr operator()(const Type& v) const
        {
            return node_allocator_->allocate<literal_node_t>(v);
        }

#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
        inline expression_node_ptr operator()(const std::string& s) const
        {
            return node_allocator_->allocate<string_literal_node_t>(s);
        }

        inline expression_node_ptr operator()(std::string& s, range_t& rp) const
        {
            return node_allocator_->allocate_rr<string_range_node_t>(s, rp);
        }

        inline expression_node_ptr operator()(const std::string& s, range_t& rp) const
        {
            return node_allocator_->allocate_tt<const_string_range_node_t>(s, rp);
        }

        inline expression_node_ptr operator()(expression_node_ptr branch, range_t& rp) const
        {
            if (is_generally_string_node(branch))
                return node_allocator_->allocate_tt<generic_string_range_node_t>(branch, rp);
            else
                return error_node();
        }
#endif

        inline bool unary_optimisable(const core::operators::operator_type& operation) const
        {
            return (core::operators::operator_type::abs == operation) ||
                   (core::operators::operator_type::acos == operation) ||
                   (core::operators::operator_type::acosh == operation) ||
                   (core::operators::operator_type::asin == operation) ||
                   (core::operators::operator_type::asinh == operation) ||
                   (core::operators::operator_type::atan == operation) ||
                   (core::operators::operator_type::atanh == operation) ||
                   (core::operators::operator_type::ceil == operation) ||
                   (core::operators::operator_type::cos == operation) ||
                   (core::operators::operator_type::cosh == operation) ||
                   (core::operators::operator_type::exp == operation) ||
                   (core::operators::operator_type::expm1 == operation) ||
                   (core::operators::operator_type::floor == operation) ||
                   (core::operators::operator_type::log == operation) ||
                   (core::operators::operator_type::log10 == operation) ||
                   (core::operators::operator_type::log2 == operation) ||
                   (core::operators::operator_type::log1p == operation) ||
                   (core::operators::operator_type::neg == operation) ||
                   (core::operators::operator_type::pos == operation) ||
                   (core::operators::operator_type::round == operation) ||
                   (core::operators::operator_type::sin == operation) ||
                   (core::operators::operator_type::sinc == operation) ||
                   (core::operators::operator_type::sinh == operation) ||
                   (core::operators::operator_type::sqrt == operation) ||
                   (core::operators::operator_type::tan == operation) ||
                   (core::operators::operator_type::tanh == operation) ||
                   (core::operators::operator_type::cot == operation) ||
                   (core::operators::operator_type::sec == operation) ||
                   (core::operators::operator_type::csc == operation) ||
                   (core::operators::operator_type::r2d == operation) ||
                   (core::operators::operator_type::d2r == operation) ||
                   (core::operators::operator_type::d2g == operation) ||
                   (core::operators::operator_type::g2d == operation) ||
                   (core::operators::operator_type::notl == operation) ||
                   (core::operators::operator_type::sgn == operation) ||
                   (core::operators::operator_type::erf == operation) ||
                   (core::operators::operator_type::erfc == operation) ||
                   (core::operators::operator_type::ncdf == operation) ||
                   (core::operators::operator_type::frac == operation) ||
                   (core::operators::operator_type::trunc == operation);
        }

        inline bool sf3_optimisable(const std::string& sf3id, trinary_functor_t& tfunc) const
        {
            typename sf3_map_t::const_iterator itr = sf3_map_->find(sf3id);

            if (sf3_map_->end() == itr)
                return false;
            else
                tfunc = itr->second.first;

            return true;
        }

        inline bool sf4_optimisable(const std::string& sf4id, quaternary_functor_t& qfunc) const
        {
            typename sf4_map_t::const_iterator itr = sf4_map_->find(sf4id);

            if (sf4_map_->end() == itr)
                return false;
            else
                qfunc = itr->second.first;

            return true;
        }

        inline bool sf3_optimisable(const std::string& sf3id,
                                    core::operators::operator_type& operation) const
        {
            typename sf3_map_t::const_iterator itr = sf3_map_->find(sf3id);

            if (sf3_map_->end() == itr)
                return false;
            else
                operation = itr->second.second;

            return true;
        }

        inline bool sf4_optimisable(const std::string& sf4id,
                                    core::operators::operator_type& operation) const
        {
            typename sf4_map_t::const_iterator itr = sf4_map_->find(sf4id);

            if (sf4_map_->end() == itr)
                return false;
            else
                operation = itr->second.second;

            return true;
        }

        inline expression_node_ptr operator()(const core::operators::operator_type& operation,
                                              expression_node_ptr (&branch)[1])
        {
            if (nullptr == branch[0])
            {
                return error_node();
            }
            else if (details::is_null_node(branch[0]))
            {
                return branch[0];
            }
            else if (details::is_break_node(branch[0]))
            {
                return error_node();
            }
            else if (details::is_continue_node(branch[0]))
            {
                return error_node();
            }
            else if (details::is_constant_node(branch[0]))
            {
                return synthesize_expression<unary_node_t, 1>(operation, branch);
            }
            else if (unary_optimisable(operation) && details::is_variable_node(branch[0]))
            {
                return synthesize_uv_expression(operation, branch);
            }
            else if (unary_optimisable(operation) && details::is_ivector_node(branch[0]))
            {
                return synthesize_uvec_expression(operation, branch);
            }
            else
                return synthesize_unary_expression(operation, branch);
        }

        inline bool is_assignment_operation(const core::operators::operator_type& operation) const
        {
            return ((core::operators::operator_type::addass == operation) ||
                    (core::operators::operator_type::subass == operation) ||
                    (core::operators::operator_type::mulass == operation) ||
                    (core::operators::operator_type::divass == operation) ||
                    (core::operators::operator_type::modass == operation)) &&
                   parser_->settings_.assignment_enabled(operation);
        }

#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
        inline bool valid_string_operation(const core::operators::operator_type& operation) const
        {
            return (core::operators::operator_type::add == operation) ||
                   (core::operators::operator_type::lt == operation) ||
                   (core::operators::operator_type::lte == operation) ||
                   (core::operators::operator_type::gt == operation) ||
                   (core::operators::operator_type::gte == operation) ||
                   (core::operators::operator_type::eq == operation) ||
                   (core::operators::operator_type::ne == operation) ||
                   (core::operators::operator_type::in == operation) ||
                   (core::operators::operator_type::like == operation) ||
                   (core::operators::operator_type::ilike == operation) ||
                   (core::operators::operator_type::assign == operation) ||
                   (core::operators::operator_type::addass == operation) ||
                   (core::operators::operator_type::swap == operation);
        }
#else
        inline bool valid_string_operation(const core::operators::operator_type&) const
        {
            return false;
        }
#endif

        inline std::string to_str(const core::operators::operator_type& operation) const
        {
            switch (operation)
            {
                case core::operators::operator_type::add:
                    return "+";
                case core::operators::operator_type::sub:
                    return "-";
                case core::operators::operator_type::mul:
                    return "*";
                case core::operators::operator_type::div:
                    return "/";
                case core::operators::operator_type::mod:
                    return "%";
                case core::operators::operator_type::pow:
                    return "^";
                case core::operators::operator_type::lt:
                    return "<";
                case core::operators::operator_type::lte:
                    return "<=";
                case core::operators::operator_type::gt:
                    return ">";
                case core::operators::operator_type::gte:
                    return ">=";
                case core::operators::operator_type::eq:
                    return "==";
                case core::operators::operator_type::ne:
                    return "!=";
                case core::operators::operator_type::logical_and:
                    return "and";
                case core::operators::operator_type::nand:
                    return "nand";
                case core::operators::operator_type::logical_or:
                    return "or";
                case core::operators::operator_type::nor:
                    return "nor";
                case core::operators::operator_type::logical_xor:
                    return "xor";
                case core::operators::operator_type::xnor:
                    return "xnor";
                default:
                    return "UNKNOWN";
            }
        }

        inline bool operation_optimisable(const core::operators::operator_type& operation) const
        {
            return (core::operators::operator_type::add == operation) ||
                   (core::operators::operator_type::sub == operation) ||
                   (core::operators::operator_type::mul == operation) ||
                   (core::operators::operator_type::div == operation) ||
                   (core::operators::operator_type::mod == operation) ||
                   (core::operators::operator_type::pow == operation) ||
                   (core::operators::operator_type::lt == operation) ||
                   (core::operators::operator_type::lte == operation) ||
                   (core::operators::operator_type::gt == operation) ||
                   (core::operators::operator_type::gte == operation) ||
                   (core::operators::operator_type::eq == operation) ||
                   (core::operators::operator_type::ne == operation) ||
                   (core::operators::operator_type::logical_and == operation) ||
                   (core::operators::operator_type::nand == operation) ||
                   (core::operators::operator_type::logical_or == operation) ||
                   (core::operators::operator_type::nor == operation) ||
                   (core::operators::operator_type::logical_xor == operation) ||
                   (core::operators::operator_type::xnor == operation);
        }

        inline std::string branch_to_id(expression_node_ptr branch) const
        {
            static const std::string null_str("(null)");
            static const std::string const_str("(c)");
            static const std::string var_str("(v)");
            static const std::string vov_str("(vov)");
            static const std::string cov_str("(cov)");
            static const std::string voc_str("(voc)");
            static const std::string str_str("(s)");
            static const std::string strrng_str("(rngs)");
            static const std::string cs_str("(cs)");
            static const std::string cstrrng_str("(crngs)");

            if (details::is_null_node(branch))
                return null_str;
            else if (details::is_constant_node(branch))
                return const_str;
            else if (details::is_variable_node(branch))
                return var_str;
            else if (details::is_vov_node(branch))
                return vov_str;
            else if (details::is_cov_node(branch))
                return cov_str;
            else if (details::is_voc_node(branch))
                return voc_str;
            else if (details::is_string_node(branch))
                return str_str;
            else if (details::is_const_string_node(branch))
                return cs_str;
            else if (details::is_string_range_node(branch))
                return strrng_str;
            else if (details::is_const_string_range_node(branch))
                return cstrrng_str;
            else if (details::is_t0ot1ot2_node(branch))
            {
                auto* b3 = branch->as_T0oT1oT2_base();
                assert(b3);
                return "(" + static_cast<details::T0oT1oT2_base_node<T>*>(b3)->type_id() + ")";
            }
            else if (details::is_t0ot1ot2ot3_node(branch))
            {
                auto* b4 = branch->as_T0oT1oT2oT3_base();
                assert(b4);
                return "(" + static_cast<details::T0oT1oT2oT3_base_node<T>*>(b4)->type_id() + ")";
            }
            else
                return "ERROR";
        }

        inline std::string branch_to_id(expression_node_ptr (&branch)[2]) const
        {
            return branch_to_id(branch[0]) + std::string("o") + branch_to_id(branch[1]);
        }

        inline bool cov_optimisable(const core::operators::operator_type& operation,
                                    expression_node_ptr (&branch)[2]) const
        {
            if (!operation_optimisable(operation))
                return false;
            else
                return details::is_constant_node(branch[0]) && details::is_variable_node(branch[1]);
        }

        inline bool voc_optimisable(const core::operators::operator_type& operation,
                                    expression_node_ptr (&branch)[2]) const
        {
            if (!operation_optimisable(operation))
                return false;
            else
                return details::is_variable_node(branch[0]) && details::is_constant_node(branch[1]);
        }

        inline bool vov_optimisable(const core::operators::operator_type& operation,
                                    expression_node_ptr (&branch)[2]) const
        {
            if (!operation_optimisable(operation))
                return false;
            else
                return details::is_variable_node(branch[0]) && details::is_variable_node(branch[1]);
        }

        inline bool cob_optimisable(const core::operators::operator_type& operation,
                                    expression_node_ptr (&branch)[2]) const
        {
            if (!operation_optimisable(operation))
                return false;
            else
                return details::is_constant_node(branch[0]) &&
                       !details::is_constant_node(branch[1]);
        }

        inline bool boc_optimisable(const core::operators::operator_type& operation,
                                    expression_node_ptr (&branch)[2]) const
        {
            if (!operation_optimisable(operation))
                return false;
            else
                return !details::is_constant_node(branch[0]) &&
                       details::is_constant_node(branch[1]);
        }

        inline bool cocob_optimisable(const core::operators::operator_type& operation,
                                      expression_node_ptr (&branch)[2]) const
        {
            if ((core::operators::operator_type::add == operation) ||
                (core::operators::operator_type::sub == operation) ||
                (core::operators::operator_type::mul == operation) ||
                (core::operators::operator_type::div == operation))
            {
                return (details::is_constant_node(branch[0]) && details::is_cob_node(branch[1])) ||
                       (details::is_constant_node(branch[1]) && details::is_cob_node(branch[0]));
            }
            else
                return false;
        }

        inline bool coboc_optimisable(const core::operators::operator_type& operation,
                                      expression_node_ptr (&branch)[2]) const
        {
            if ((core::operators::operator_type::add == operation) ||
                (core::operators::operator_type::sub == operation) ||
                (core::operators::operator_type::mul == operation) ||
                (core::operators::operator_type::div == operation))
            {
                return (details::is_constant_node(branch[0]) && details::is_boc_node(branch[1])) ||
                       (details::is_constant_node(branch[1]) && details::is_boc_node(branch[0]));
            }
            else
                return false;
        }

        inline bool uvouv_optimisable(const core::operators::operator_type& operation,
                                      expression_node_ptr (&branch)[2]) const
        {
            if (!operation_optimisable(operation))
                return false;
            else
                return details::is_uv_node(branch[0]) && details::is_uv_node(branch[1]);
        }

        inline bool vob_optimisable(const core::operators::operator_type& operation,
                                    expression_node_ptr (&branch)[2]) const
        {
            if (!operation_optimisable(operation))
                return false;
            else
                return details::is_variable_node(branch[0]) &&
                       !details::is_variable_node(branch[1]);
        }

        inline bool bov_optimisable(const core::operators::operator_type& operation,
                                    expression_node_ptr (&branch)[2]) const
        {
            if (!operation_optimisable(operation))
                return false;
            else
                return !details::is_variable_node(branch[0]) &&
                       details::is_variable_node(branch[1]);
        }

        inline bool binext_optimisable(const core::operators::operator_type& operation,
                                       expression_node_ptr (&branch)[2]) const
        {
            if (!operation_optimisable(operation))
                return false;
            else
                return !details::is_constant_node(branch[0]) ||
                       !details::is_constant_node(branch[1]);
        }

        inline bool is_invalid_assignment_op(const core::operators::operator_type& operation,
                                             expression_node_ptr (&branch)[2]) const
        {
            if (is_assignment_operation(operation))
            {
                const bool b1_is_genstring = details::is_generally_string_node(branch[1]);

                if (details::is_string_node(branch[0]))
                    return !b1_is_genstring;
                else if (details::is_literal_node(branch[0]))
                    return true;
                else
                    return (!details::is_variable_node(branch[0]) &&
                            !details::is_vector_elem_node(branch[0]) &&
                            !details::is_vector_celem_node(branch[0]) &&
                            !details::is_vector_elem_rtc_node(branch[0]) &&
                            !details::is_vector_celem_rtc_node(branch[0]) &&
                            !details::is_rebasevector_elem_node(branch[0]) &&
                            !details::is_rebasevector_celem_node(branch[0]) &&
                            !details::is_rebasevector_elem_rtc_node(branch[0]) &&
                            !details::is_rebasevector_celem_rtc_node(branch[0]) &&
                            !details::is_vector_node(branch[0])) ||
                           b1_is_genstring;
            }
            else
                return false;
        }

        inline bool is_constpow_operation(const core::operators::operator_type& operation,
                                          expression_node_ptr (&branch)[2]) const
        {
            if (!details::is_constant_node(branch[1]) || details::is_constant_node(branch[0]) ||
                details::is_variable_node(branch[0]) || details::is_vector_node(branch[0]) ||
                details::is_generally_string_node(branch[0]))
                return false;

            const Type c = static_cast<details::literal_node<Type>*>(branch[1])->value();

            return cardinal_pow_optimisable(operation, c);
        }

        inline bool is_invalid_break_continue_op(expression_node_ptr (&branch)[2]) const
        {
            return (details::is_break_node(branch[0]) || details::is_break_node(branch[1]) ||
                    details::is_continue_node(branch[0]) || details::is_continue_node(branch[1]));
        }

        inline bool is_invalid_string_op(const core::operators::operator_type& operation,
                                         expression_node_ptr (&branch)[2]) const
        {
            const bool b0_string = is_generally_string_node(branch[0]);
            const bool b1_string = is_generally_string_node(branch[1]);

            bool result = false;

            if (b0_string != b1_string)
                result = true;
            else if (!valid_string_operation(operation) && b0_string && b1_string)
                result = true;

            if (result)
            {
                parser_->set_synthesis_error("Invalid string operation");
            }

            return result;
        }

        inline bool is_invalid_string_op(const core::operators::operator_type& operation,
                                         expression_node_ptr (&branch)[3]) const
        {
            const bool b0_string = is_generally_string_node(branch[0]);
            const bool b1_string = is_generally_string_node(branch[1]);
            const bool b2_string = is_generally_string_node(branch[2]);

            bool result = false;

            if ((b0_string != b1_string) || (b1_string != b2_string))
                result = true;
            else if ((core::operators::operator_type::inrange != operation) && b0_string &&
                     b1_string && b2_string)
                result = true;

            if (result)
            {
                parser_->set_synthesis_error("Invalid string operation");
            }

            return result;
        }

        inline bool is_string_operation(const core::operators::operator_type& operation,
                                        expression_node_ptr (&branch)[2]) const
        {
            const bool b0_string = is_generally_string_node(branch[0]);
            const bool b1_string = is_generally_string_node(branch[1]);

            return (b0_string && b1_string && valid_string_operation(operation));
        }

        inline bool is_string_operation(const core::operators::operator_type& operation,
                                        expression_node_ptr (&branch)[3]) const
        {
            const bool b0_string = is_generally_string_node(branch[0]);
            const bool b1_string = is_generally_string_node(branch[1]);
            const bool b2_string = is_generally_string_node(branch[2]);

            return (b0_string && b1_string && b2_string &&
                    (core::operators::operator_type::inrange == operation));
        }

        inline bool is_shortcircuit_expression(
            const core::operators::operator_type& operation) const
        {
            if constexpr (::math_expr::core::build_options::kDisableScAndOr)
            {
                return false;
            }
            else
            {
                return ((core::operators::operator_type::scand == operation) ||
                        (core::operators::operator_type::scor == operation));
            }
        }

        inline bool is_null_present(expression_node_ptr (&branch)[2]) const
        {
            return (details::is_null_node(branch[0]) || details::is_null_node(branch[1]));
        }

        inline bool is_vector_eqineq_logic_operation(
            const core::operators::operator_type& operation, expression_node_ptr (&branch)[2]) const
        {
            if (!is_ivector_node(branch[0]) && !is_ivector_node(branch[1]))
                return false;
            else
                return ((core::operators::operator_type::lt == operation) ||
                        (core::operators::operator_type::lte == operation) ||
                        (core::operators::operator_type::gt == operation) ||
                        (core::operators::operator_type::gte == operation) ||
                        (core::operators::operator_type::eq == operation) ||
                        (core::operators::operator_type::ne == operation) ||
                        (core::operators::operator_type::equal == operation) ||
                        (core::operators::operator_type::logical_and == operation) ||
                        (core::operators::operator_type::nand == operation) ||
                        (core::operators::operator_type::logical_or == operation) ||
                        (core::operators::operator_type::nor == operation) ||
                        (core::operators::operator_type::logical_xor == operation) ||
                        (core::operators::operator_type::xnor == operation));
        }

        inline bool is_vector_arithmetic_operation(const core::operators::operator_type& operation,
                                                   expression_node_ptr (&branch)[2]) const
        {
            if (!is_ivector_node(branch[0]) && !is_ivector_node(branch[1]))
                return false;
            else
                return ((core::operators::operator_type::add == operation) ||
                        (core::operators::operator_type::sub == operation) ||
                        (core::operators::operator_type::mul == operation) ||
                        (core::operators::operator_type::div == operation) ||
                        (core::operators::operator_type::pow == operation));
        }

        inline expression_node_ptr operator()(const core::operators::operator_type& operation,
                                              expression_node_ptr (&branch)[2])
        {
            if ((nullptr == branch[0]) || (nullptr == branch[1]))
            {
                parser_->set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, parser_->current_state().token,
                    "ERR249 - Invalid branches received for operator '" +
                        core::operators::to_str(operation) + "'",
                    core::error_location()));

                return error_node();
            }
            else if (is_invalid_string_op(operation, branch))
            {
                parser_->set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, parser_->current_state().token,
                    "ERR250 - Invalid branch pair for string operator '" +
                        core::operators::to_str(operation) + "'",
                    core::error_location()));

                return error_node();
            }
            else if (is_invalid_assignment_op(operation, branch))
            {
                parser_->set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, parser_->current_state().token,
                    "ERR251 - Invalid branch pair for assignment operator '" +
                        core::operators::to_str(operation) + "'",
                    core::error_location()));

                return error_node();
            }
            else if (is_invalid_break_continue_op(branch))
            {
                parser_->set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, parser_->current_state().token,
                    "ERR252 - Invalid branch pair for break/continue operator '" +
                        core::operators::to_str(operation) + "'",
                    core::error_location()));

                return error_node();
            }
            else if (core::operators::operator_type::assign == operation)
            {
                return synthesize_assignment_expression(operation, branch);
            }
            else if (core::operators::operator_type::swap == operation)
            {
                return synthesize_swap_expression(branch);
            }
            else if (is_assignment_operation(operation))
            {
                return synthesize_assignment_operation_expression(operation, branch);
            }
            else if (is_vector_eqineq_logic_operation(operation, branch))
            {
                return synthesize_veceqineqlogic_operation_expression(operation, branch);
            }
            else if (is_vector_arithmetic_operation(operation, branch))
            {
                return synthesize_vecarithmetic_operation_expression(operation, branch);
            }
            else if (is_shortcircuit_expression(operation))
            {
                return synthesize_shortcircuit_expression(operation, branch);
            }
            else if (is_string_operation(operation, branch))
            {
                return synthesize_string_expression(operation, branch);
            }
            else if (is_null_present(branch))
            {
                return synthesize_null_expression(operation, branch);
            }
            else if (is_constpow_operation(operation, branch))
            {
                if constexpr (!::math_expr::core::build_options::kDisableCardinalPowOptimisation)
                {
                    return cardinal_pow_optimisation(branch);
                }
            }

            expression_node_ptr result = error_node();

#ifndef MATH_EXPR_DISABLE_ENHANCED_FEATURES
            if (synthesize_expression(operation, branch, result))
            {
                return result;
            }
#endif

            /*
               Possible reductions:
               1. c o cob -> cob
               2. cob o c -> cob
               3. c o boc -> boc
               4. boc o c -> boc
            */
            result = error_node();

            if (cocob_optimisable(operation, branch))
            {
                result = synthesize_cocob_expression::process((*this), operation, branch);
            }
            else if (coboc_optimisable(operation, branch) && (nullptr == result))
            {
                result = synthesize_coboc_expression::process((*this), operation, branch);
            }

            if (result)
                return result;

            if (uvouv_optimisable(operation, branch))
            {
                return synthesize_uvouv_expression(operation, branch);
            }
            else if (vob_optimisable(operation, branch))
            {
                return synthesize_vob_expression::process((*this), operation, branch);
            }
            else if (bov_optimisable(operation, branch))
            {
                return synthesize_bov_expression::process((*this), operation, branch);
            }
            else if (cob_optimisable(operation, branch))
            {
                return synthesize_cob_expression::process((*this), operation, branch);
            }
            else if (boc_optimisable(operation, branch))
            {
                return synthesize_boc_expression::process((*this), operation, branch);
            }
#ifndef MATH_EXPR_DISABLE_ENHANCED_FEATURES
            else if (cov_optimisable(operation, branch))
            {
                return synthesize_cov_expression::process((*this), operation, branch);
            }
#endif
            else if (binext_optimisable(operation, branch))
            {
                return synthesize_binary_ext_expression::process((*this), operation, branch);
            }
            else
                return synthesize_expression<binary_node_t, 2>(operation, branch);
        }

        inline expression_node_ptr operator()(const core::operators::operator_type& operation,
                                              expression_node_ptr (&branch)[3])
        {
            if ((nullptr == branch[0]) || (nullptr == branch[1]) || (nullptr == branch[2]))
            {
                details::free_all_nodes(*node_allocator_, branch);

                parser_->set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, parser_->current_state().token,
                    "ERR253 - Invalid branches operator '" + core::operators::to_str(operation) +
                        "'",
                    core::error_location()));

                return error_node();
            }
            else if (is_invalid_string_op(operation, branch))
            {
                parser_->set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, parser_->current_state().token,
                    "ERR254 - Invalid branches for string operator '" +
                        core::operators::to_str(operation) + "'",
                    core::error_location()));

                return error_node();
            }
            else if (is_string_operation(operation, branch))
            {
                return synthesize_string_expression(operation, branch);
            }
            else
                return synthesize_expression<trinary_node_t, 3>(operation, branch);
        }

        inline expression_node_ptr operator()(const core::operators::operator_type& operation,
                                              expression_node_ptr (&branch)[4])
        {
            return synthesize_expression<quaternary_node_t, 4>(operation, branch);
        }

        inline expression_node_ptr operator()(const core::operators::operator_type& operation,
                                              expression_node_ptr b0)
        {
            expression_node_ptr branch[1] = {b0};
            return (*this)(operation, branch);
        }

        inline expression_node_ptr operator()(const core::operators::operator_type& operation,
                                              expression_node_ptr& b0, expression_node_ptr& b1)
        {
            expression_node_ptr result = error_node();

            if ((nullptr != b0) && (nullptr != b1))
            {
                expression_node_ptr branch[2] = {b0, b1};
                result = expression_generator<Type>::operator()(operation, branch);
                b0 = branch[0];
                b1 = branch[1];
            }

            return result;
        }

        inline expression_node_ptr conditional(expression_node_ptr condition,
                                               expression_node_ptr consequent,
                                               expression_node_ptr alternative) const
        {
            if ((nullptr == condition) || (nullptr == consequent))
            {
                details::free_node(*node_allocator_, condition);
                details::free_node(*node_allocator_, consequent);
                details::free_node(*node_allocator_, alternative);

                const std::string invalid_branches =
                    ((nullptr == condition) ? std::string("condition ") : "") +
                    ((nullptr == consequent) ? std::string("consequent") : "");

                parser_->set_error(parser_error::make_error(
                    parser_error::error_mode::e_parser, parser_->current_state().token,
                    "ERR255 - Invalid " + invalid_branches + " for conditional statement",
                    core::error_location()));

                return error_node();
            }
            // Can the condition be immediately evaluated? if so optimise.
            else if (details::is_constant_node(condition))
            {
                // True branch
                if (details::is_true(condition))
                {
                    details::free_node(*node_allocator_, condition);
                    details::free_node(*node_allocator_, alternative);

                    return consequent;
                }
                // False branch
                else
                {
                    details::free_node(*node_allocator_, condition);
                    details::free_node(*node_allocator_, consequent);

                    if (alternative)
                        return alternative;
                    else
                        return node_allocator_->allocate<details::null_node<T>>();
                }
            }

            expression_node_ptr result = error_node();
            std::string node_name = "Unknown!";

            if ((nullptr != consequent) && (nullptr != alternative))
            {
                result = node_allocator_->allocate<conditional_node_t>(condition, consequent,
                                                                       alternative);
                node_name = "conditional_node_t";
            }
            else
            {
                result = node_allocator_->allocate<cons_conditional_node_t>(condition, consequent);
                node_name = "cons_conditional_node_t";
            }

            if (result && result->valid())
            {
                return result;
            }

            parser_->set_error(parser_error::make_error(
                parser_error::error_mode::e_parser, token_t(),
                "ERR256 - Failed to synthesize node: " + node_name, core::error_location()));

            details::free_node(*node_allocator_, result);
            return error_node();
        }

#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
        inline expression_node_ptr conditional_string(expression_node_ptr condition,
                                                      expression_node_ptr consequent,
                                                      expression_node_ptr alternative) const
        {
            if ((nullptr == condition) || (nullptr == consequent))
            {
                details::free_node(*node_allocator_, condition);
                details::free_node(*node_allocator_, consequent);
                details::free_node(*node_allocator_, alternative);

                const std::string invalid_branches =
                    ((nullptr == condition) ? std::string("condition ") : "") +
                    ((nullptr == consequent) ? std::string("consequent") : "");

                parser_->set_error(parser_error::make_error(
                    parser_error::error_mode::e_parser, parser_->current_state().token,
                    "ERR257 - Invalid " + invalid_branches + " for string conditional statement",
                    core::error_location()));

                return error_node();
            }
            // Can the condition be immediately evaluated? if so optimise.
            else if (details::is_constant_node(condition))
            {
                // True branch
                if (details::is_true(condition))
                {
                    details::free_node(*node_allocator_, condition);
                    details::free_node(*node_allocator_, alternative);

                    return consequent;
                }
                // False branch
                else
                {
                    details::free_node(*node_allocator_, condition);
                    details::free_node(*node_allocator_, consequent);

                    if (alternative)
                        return alternative;
                    else
                        return node_allocator_->allocate_c<details::string_literal_node<Type>>("");
                }
            }
            else if ((nullptr != consequent) && (nullptr != alternative))
            {
                expression_node_ptr result = node_allocator_->allocate<conditional_string_node_t>(
                    condition, consequent, alternative);

                if (result && result->valid())
                {
                    return result;
                }

                parser_->set_error(parser_error::make_error(
                    parser_error::error_mode::e_parser, token_t(),
                    "ERR258 - Failed to synthesize node: conditional_string_node_t",
                    core::error_location()));

                details::free_node(*node_allocator_, result);
            }

            return error_node();
        }
#else
        inline expression_node_ptr conditional_string(expression_node_ptr, expression_node_ptr,
                                                      expression_node_ptr) const
        {
            return error_node();
        }
#endif

        inline expression_node_ptr conditional_vector(expression_node_ptr condition,
                                                      expression_node_ptr consequent,
                                                      expression_node_ptr alternative) const
        {
            if ((nullptr == condition) || (nullptr == consequent))
            {
                details::free_node(*node_allocator_, condition);
                details::free_node(*node_allocator_, consequent);
                details::free_node(*node_allocator_, alternative);

                const std::string invalid_branches =
                    ((nullptr == condition) ? std::string("condition ") : "") +
                    ((nullptr == consequent) ? std::string("consequent") : "");

                parser_->set_error(parser_error::make_error(
                    parser_error::error_mode::e_parser, parser_->current_state().token,
                    "ERR259 - Invalid " + invalid_branches + " for vector conditional statement",
                    core::error_location()));

                return error_node();
            }
            // Can the condition be immediately evaluated? if so optimise.
            else if (details::is_constant_node(condition))
            {
                // True branch
                if (details::is_true(condition))
                {
                    details::free_node(*node_allocator_, condition);
                    details::free_node(*node_allocator_, alternative);

                    return consequent;
                }
                // False branch
                else
                {
                    details::free_node(*node_allocator_, condition);
                    details::free_node(*node_allocator_, consequent);

                    if (alternative)
                        return alternative;
                    else
                        return node_allocator_->allocate<details::null_node<T>>();
                }
            }
            else if ((nullptr != consequent) && (nullptr != alternative))
            {
                return node_allocator_->allocate<conditional_vector_node_t>(condition, consequent,
                                                                            alternative);
            }
            else
                return error_node();
        }

        inline loop_runtime_check_ptr get_loop_runtime_check(const loop_types loop_type) const
        {
            if (parser_->loop_runtime_check_ &&
                (loop_type == (parser_->loop_runtime_check_->loop_set & loop_type)))
            {
                return parser_->loop_runtime_check_;
            }

            return nullptr;
        }

        inline vector_access_runtime_check<T>* get_vector_access_runtime_check() const
        {
            return parser_->vector_access_runtime_check_;
        }

        inline expression_node_ptr while_loop(expression_node_ptr& condition,
                                              expression_node_ptr& branch,
                                              const bool break_continue_present = false) const
        {
            if (!break_continue_present && !parser_->state_.return_stmt_present &&
                details::is_constant_node(condition))
            {
                expression_node_ptr result = error_node();
                if (details::is_true(condition))
                {
                    // Infinite loops are not allowed.

                    parser_->set_error(parser_error::make_error(
                        parser_error::error_mode::e_parser, parser_->current_state().token,
                        "ERR260 - Infinite loop condition without 'break' or 'return' not allowed "
                        "in while-loops",
                        core::error_location()));

                    result = error_node();
                }
                else
                    result = node_allocator_->allocate<details::null_node<Type>>();

                details::free_node(*node_allocator_, condition);
                details::free_node(*node_allocator_, branch);

                return result;
            }
            else if (details::is_null_node(condition))
            {
                details::free_node(*node_allocator_, condition);

                return branch;
            }

            loop_runtime_check_ptr rtc = get_loop_runtime_check(loop_types::e_while_loop());

            if (!break_continue_present)
            {
                if (rtc)
                    return node_allocator_->allocate<while_loop_rtc_node_t>(condition, branch, rtc);
                else
                    return node_allocator_->allocate<while_loop_node_t>(condition, branch);
            }
#ifndef MATH_EXPR_DISABLE_BREAK_CONTINUE
            else
            {
                if (rtc)
                    return node_allocator_->allocate<while_loop_bc_rtc_node_t>(condition, branch,
                                                                               rtc);
                else
                    return node_allocator_->allocate<while_loop_bc_node_t>(condition, branch);
            }
#else
            return error_node();
#endif
        }

        inline expression_node_ptr repeat_until_loop(
            expression_node_ptr& condition, expression_node_ptr& branch,
            const bool break_continue_present = false) const
        {
            if (!break_continue_present && details::is_constant_node(condition))
            {
                if (details::is_true(condition) && details::is_constant_node(branch))
                {
                    free_node(*node_allocator_, condition);

                    return branch;
                }

                details::free_node(*node_allocator_, condition);
                details::free_node(*node_allocator_, branch);

                return error_node();
            }
            else if (details::is_null_node(condition))
            {
                details::free_node(*node_allocator_, condition);

                return branch;
            }

            loop_runtime_check_ptr rtc = get_loop_runtime_check(loop_types::e_repeat_until_loop());

            if (!break_continue_present)
            {
                if (rtc)
                    return node_allocator_->allocate<repeat_until_loop_rtc_node_t>(condition,
                                                                                   branch, rtc);
                else
                    return node_allocator_->allocate<repeat_until_loop_node_t>(condition, branch);
            }
#ifndef MATH_EXPR_DISABLE_BREAK_CONTINUE
            else
            {
                if (rtc)
                    return node_allocator_->allocate<repeat_until_loop_bc_rtc_node_t>(condition,
                                                                                      branch, rtc);
                else
                    return node_allocator_->allocate<repeat_until_loop_bc_node_t>(condition,
                                                                                  branch);
            }
#else
            return error_node();
#endif
        }

        inline expression_node_ptr for_loop(expression_node_ptr& initialiser,
                                            expression_node_ptr& condition,
                                            expression_node_ptr& incrementor,
                                            expression_node_ptr& loop_body,
                                            bool break_continue_present = false) const
        {
            if (!break_continue_present && !parser_->state_.return_stmt_present &&
                details::is_constant_node(condition))
            {
                expression_node_ptr result = error_node();

                if (details::is_true(condition))
                {
                    // Infinite loops are not allowed.

                    parser_->set_error(parser_error::make_error(
                        parser_error::error_mode::e_parser, parser_->current_state().token,
                        "ERR261 - Infinite loop condition without 'break' or 'return' not allowed "
                        "in for-loop",
                        core::error_location()));

                    result = error_node();
                }
                else
                    result = node_allocator_->allocate<details::null_node<Type>>();

                details::free_node(*node_allocator_, initialiser);
                details::free_node(*node_allocator_, condition);
                details::free_node(*node_allocator_, incrementor);
                details::free_node(*node_allocator_, loop_body);

                return result;
            }
            else if (details::is_null_node(condition) || (nullptr == condition))
            {
                details::free_node(*node_allocator_, initialiser);
                details::free_node(*node_allocator_, condition);
                details::free_node(*node_allocator_, incrementor);

                return loop_body;
            }

            loop_runtime_check_ptr rtc = get_loop_runtime_check(loop_types::e_for_loop());

            if (!break_continue_present)
            {
                if (rtc)
                    return node_allocator_->allocate<for_loop_rtc_node_t>(
                        initialiser, condition, incrementor, loop_body, rtc);
                else
                    return node_allocator_->allocate<for_loop_node_t>(initialiser, condition,
                                                                      incrementor, loop_body);
            }
#ifndef MATH_EXPR_DISABLE_BREAK_CONTINUE
            else
            {
                if (rtc)
                    return node_allocator_->allocate<for_loop_bc_rtc_node_t>(
                        initialiser, condition, incrementor, loop_body, rtc);
                else
                    return node_allocator_->allocate<for_loop_bc_node_t>(initialiser, condition,
                                                                         incrementor, loop_body);
            }
#else
            return error_node();
#endif
        }

        template <typename Allocator, template <typename, typename> class Sequence>
        inline expression_node_ptr const_optimise_switch(
            Sequence<expression_node_ptr, Allocator>& arg_list)
        {
            expression_node_ptr result = error_node();

            for (std::size_t i = 0; i < (arg_list.size() / 2); ++i)
            {
                expression_node_ptr condition = arg_list[(2 * i)];
                expression_node_ptr consequent = arg_list[(2 * i) + 1];

                if ((nullptr == result) && details::is_true(condition))
                {
                    result = consequent;
                    break;
                }
            }

            if (nullptr == result)
            {
                result = arg_list.back();
            }

            for (std::size_t i = 0; i < arg_list.size(); ++i)
            {
                expression_node_ptr current_expr = arg_list[i];

                if (current_expr && (current_expr != result))
                {
                    free_node(*node_allocator_, current_expr);
                }
            }

            return result;
        }

        template <typename Allocator, template <typename, typename> class Sequence>
        inline expression_node_ptr const_optimise_mswitch(
            Sequence<expression_node_ptr, Allocator>& arg_list)
        {
            expression_node_ptr result = error_node();

            for (std::size_t i = 0; i < (arg_list.size() / 2); ++i)
            {
                expression_node_ptr condition = arg_list[(2 * i)];
                expression_node_ptr consequent = arg_list[(2 * i) + 1];

                if (details::is_true(condition))
                {
                    result = consequent;
                }
            }

            if (nullptr == result)
            {
                const T zero = T(0);
                result = node_allocator_->allocate<literal_node_t>(zero);
            }

            for (std::size_t i = 0; i < arg_list.size(); ++i)
            {
                expression_node_ptr& current_expr = arg_list[i];

                if (current_expr && (current_expr != result))
                {
                    details::free_node(*node_allocator_, current_expr);
                }
            }

            return result;
        }

        struct switch_nodes
        {
            using arg_list_t = std::vector<std::pair<expression_node_ptr, bool>>;

#define CASE_STMT(N)                            \
    if (is_true(arg[(2 * N)].first))            \
    {                                           \
        return arg[(2 * N) + 1].first->value(); \
    }

            struct switch_impl_1
            {
                static inline T process(const arg_list_t& arg)
                {
                    CASE_STMT(0);

                    assert(arg.size() == ((2 * 1) + 1));

                    return arg.back().first->value();
                }
            };

            struct switch_impl_2
            {
                static inline T process(const arg_list_t& arg)
                {
                    CASE_STMT(0);
                    CASE_STMT(1);

                    assert(arg.size() == ((2 * 2) + 1));

                    return arg.back().first->value();
                }
            };

            struct switch_impl_3
            {
                static inline T process(const arg_list_t& arg)
                {
                    CASE_STMT(0);
                    CASE_STMT(1);
                    CASE_STMT(2);

                    assert(arg.size() == ((2 * 3) + 1));

                    return arg.back().first->value();
                }
            };

            struct switch_impl_4
            {
                static inline T process(const arg_list_t& arg)
                {
                    CASE_STMT(0);
                    CASE_STMT(1);
                    CASE_STMT(2);
                    CASE_STMT(3);

                    assert(arg.size() == ((2 * 4) + 1));

                    return arg.back().first->value();
                }
            };

            struct switch_impl_5
            {
                static inline T process(const arg_list_t& arg)
                {
                    CASE_STMT(0);
                    CASE_STMT(1);
                    CASE_STMT(2);
                    CASE_STMT(3);
                    CASE_STMT(4);

                    assert(arg.size() == ((2 * 5) + 1));

                    return arg.back().first->value();
                }
            };

            struct switch_impl_6
            {
                static inline T process(const arg_list_t& arg)
                {
                    CASE_STMT(0);
                    CASE_STMT(1);
                    CASE_STMT(2);
                    CASE_STMT(3);
                    CASE_STMT(4);
                    CASE_STMT(5);

                    assert(arg.size() == ((2 * 6) + 1));

                    return arg.back().first->value();
                }
            };

            struct switch_impl_7
            {
                static inline T process(const arg_list_t& arg)
                {
                    CASE_STMT(0);
                    CASE_STMT(1);
                    CASE_STMT(2);
                    CASE_STMT(3);
                    CASE_STMT(4);
                    CASE_STMT(5);
                    CASE_STMT(6);

                    assert(arg.size() == ((2 * 7) + 1));

                    return arg.back().first->value();
                }
            };

#undef CASE_STMT
        };

        template <typename Allocator, template <typename, typename> class Sequence>
        inline expression_node_ptr switch_statement(
            Sequence<expression_node_ptr, Allocator>& arg_list,
            const bool default_statement_present)
        {
            if (arg_list.empty())
                return error_node();
            else if (!all_nodes_valid(arg_list) ||
                     (!default_statement_present && (arg_list.size() < 2)))
            {
                details::free_all_nodes(*node_allocator_, arg_list);

                return error_node();
            }
            else if (fold_passes_t::is_constant_foldable(arg_list))
                return const_optimise_switch(arg_list);

            switch ((arg_list.size() - 1) / 2)
            {
#define CASE_STMT(N)                                                                          \
    case N:                                                                                   \
        return node_allocator_                                                                \
            ->allocate<details::switch_n_node<Type, typename switch_nodes::switch_impl_##N>>( \
                arg_list);

                CASE_STMT(1);
                CASE_STMT(2);
                CASE_STMT(3);
                CASE_STMT(4);
                CASE_STMT(5);
                CASE_STMT(6);
                CASE_STMT(7);
#undef CASE_STMT

                default:
                    return node_allocator_->allocate<details::switch_node<Type>>(arg_list);
            }
        }

        template <typename Allocator, template <typename, typename> class Sequence>
        inline expression_node_ptr multi_switch_statement(
            Sequence<expression_node_ptr, Allocator>& arg_list)
        {
            if (!all_nodes_valid(arg_list))
            {
                details::free_all_nodes(*node_allocator_, arg_list);

                return error_node();
            }
            else if (fold_passes_t::is_constant_foldable(arg_list))
                return const_optimise_mswitch(arg_list);
            else
                return node_allocator_->allocate<details::multi_switch_node<Type>>(arg_list);
        }

        inline expression_node_ptr assert_call(expression_node_ptr& assert_condition,
                                               expression_node_ptr& assert_message,
                                               const assert_check::assert_context& context)
        {
            using alloc_type = details::assert_node<Type>;

            expression_node_ptr result = node_allocator_->allocate_rrrr<alloc_type>(
                assert_condition, assert_message, parser_->assert_check_, context);

            if (result && result->valid())
            {
                parser_->state_.activate_side_effect("assert_call()");
                return result;
            }

            details::free_node(*node_allocator_, result);
            details::free_node(*node_allocator_, assert_condition);
            details::free_node(*node_allocator_, assert_message);

            return error_node();
        }

#define UNARY_OPR_SWITCH_STATEMENTS                                      \
    CASE_STMT(core::operators::operator_type::abs, details::abs_op);     \
    CASE_STMT(core::operators::operator_type::acos, details::acos_op);   \
    CASE_STMT(core::operators::operator_type::acosh, details::acosh_op); \
    CASE_STMT(core::operators::operator_type::asin, details::asin_op);   \
    CASE_STMT(core::operators::operator_type::asinh, details::asinh_op); \
    CASE_STMT(core::operators::operator_type::atan, details::atan_op);   \
    CASE_STMT(core::operators::operator_type::atanh, details::atanh_op); \
    CASE_STMT(core::operators::operator_type::ceil, details::ceil_op);   \
    CASE_STMT(core::operators::operator_type::cos, details::cos_op);     \
    CASE_STMT(core::operators::operator_type::cosh, details::cosh_op);   \
    CASE_STMT(core::operators::operator_type::exp, details::exp_op);     \
    CASE_STMT(core::operators::operator_type::expm1, details::expm1_op); \
    CASE_STMT(core::operators::operator_type::floor, details::floor_op); \
    CASE_STMT(core::operators::operator_type::log, details::log_op);     \
    CASE_STMT(core::operators::operator_type::log10, details::log10_op); \
    CASE_STMT(core::operators::operator_type::log2, details::log2_op);   \
    CASE_STMT(core::operators::operator_type::log1p, details::log1p_op); \
    CASE_STMT(core::operators::operator_type::neg, details::neg_op);     \
    CASE_STMT(core::operators::operator_type::pos, details::pos_op);     \
    CASE_STMT(core::operators::operator_type::round, details::round_op); \
    CASE_STMT(core::operators::operator_type::sin, details::sin_op);     \
    CASE_STMT(core::operators::operator_type::sinc, details::sinc_op);   \
    CASE_STMT(core::operators::operator_type::sinh, details::sinh_op);   \
    CASE_STMT(core::operators::operator_type::sqrt, details::sqrt_op);   \
    CASE_STMT(core::operators::operator_type::tan, details::tan_op);     \
    CASE_STMT(core::operators::operator_type::tanh, details::tanh_op);   \
    CASE_STMT(core::operators::operator_type::cot, details::cot_op);     \
    CASE_STMT(core::operators::operator_type::sec, details::sec_op);     \
    CASE_STMT(core::operators::operator_type::csc, details::csc_op);     \
    CASE_STMT(core::operators::operator_type::r2d, details::r2d_op);     \
    CASE_STMT(core::operators::operator_type::d2r, details::d2r_op);     \
    CASE_STMT(core::operators::operator_type::d2g, details::d2g_op);     \
    CASE_STMT(core::operators::operator_type::g2d, details::g2d_op);     \
    CASE_STMT(core::operators::operator_type::notl, details::notl_op);   \
    CASE_STMT(core::operators::operator_type::sgn, details::sgn_op);     \
    CASE_STMT(core::operators::operator_type::erf, details::erf_op);     \
    CASE_STMT(core::operators::operator_type::erfc, details::erfc_op);   \
    CASE_STMT(core::operators::operator_type::ncdf, details::ncdf_op);   \
    CASE_STMT(core::operators::operator_type::frac, details::frac_op);   \
    CASE_STMT(core::operators::operator_type::trunc, details::trunc_op);

        inline expression_node_ptr synthesize_uv_expression(
            const core::operators::operator_type& operation, expression_node_ptr (&branch)[1])
        {
            T& v = static_cast<details::variable_node<T>*>(branch[0])->ref();

            switch (operation)
            {
#define CASE_STMT(op0, op1) \
    case op0:               \
        return node_allocator_->allocate<typename details::unary_variable_node<Type, op1<Type>>>(v);

                UNARY_OPR_SWITCH_STATEMENTS
#undef CASE_STMT
                default:
                    return error_node();
            }
        }

        inline expression_node_ptr synthesize_uvec_expression(
            const core::operators::operator_type& operation, expression_node_ptr (&branch)[1])
        {
            switch (operation)
            {
#define CASE_STMT(op0, op1)                                                                     \
    case op0:                                                                                   \
        return node_allocator_->allocate<typename details::unary_vector_node<Type, op1<Type>>>( \
            operation, branch[0]);

                UNARY_OPR_SWITCH_STATEMENTS
#undef CASE_STMT
                default:
                    return error_node();
            }
        }

        inline expression_node_ptr synthesize_unary_expression(
            const core::operators::operator_type& operation, expression_node_ptr (&branch)[1])
        {
            switch (operation)
            {
#define CASE_STMT(op0, op1)                                                                     \
    case op0:                                                                                   \
        return node_allocator_->allocate<typename details::unary_branch_node<Type, op1<Type>>>( \
            branch[0]);

                UNARY_OPR_SWITCH_STATEMENTS
#undef CASE_STMT
                default:
                    return error_node();
            }
        }

        inline expression_node_ptr const_optimise_sf3(
            const core::operators::operator_type& operation, expression_node_ptr (&branch)[3])
        {
            expression_node_ptr temp_node = error_node();

            switch (operation)
            {
#define CASE_STMT(op)                                                                       \
    case core::operators::operator_type::sf##op:                                            \
        temp_node =                                                                         \
            node_allocator_->allocate<details::sf3_node<Type, details::sf##op##_op<Type>>>( \
                operation, branch);                                                         \
        break;

                CASE_STMT(00);
                CASE_STMT(01);
                CASE_STMT(02);
                CASE_STMT(03);
                CASE_STMT(04);
                CASE_STMT(05);
                CASE_STMT(06);
                CASE_STMT(07);
                CASE_STMT(08);
                CASE_STMT(09);
                CASE_STMT(10);
                CASE_STMT(11);
                CASE_STMT(12);
                CASE_STMT(13);
                CASE_STMT(14);
                CASE_STMT(15);
                CASE_STMT(16);
                CASE_STMT(17);
                CASE_STMT(18);
                CASE_STMT(19);
                CASE_STMT(20);
                CASE_STMT(21);
                CASE_STMT(22);
                CASE_STMT(23);
                CASE_STMT(24);
                CASE_STMT(25);
                CASE_STMT(26);
                CASE_STMT(27);
                CASE_STMT(28);
                CASE_STMT(29);
                CASE_STMT(30);
                CASE_STMT(31);
                CASE_STMT(32);
                CASE_STMT(33);
                CASE_STMT(34);
                CASE_STMT(35);
                CASE_STMT(36);
                CASE_STMT(37);
                CASE_STMT(38);
                CASE_STMT(39);
                CASE_STMT(40);
                CASE_STMT(41);
                CASE_STMT(42);
                CASE_STMT(43);
                CASE_STMT(44);
                CASE_STMT(45);
                CASE_STMT(46);
                CASE_STMT(47);
#undef CASE_STMT
                default:
                    return error_node();
            }

            assert(temp_node);

            const T v = temp_node->value();

            details::free_node(*node_allocator_, temp_node);

            return node_allocator_->allocate<literal_node_t>(v);
        }

        inline expression_node_ptr varnode_optimise_sf3(
            const core::operators::operator_type& operation, expression_node_ptr (&branch)[3])
        {
            using variable_ptr = details::variable_node<Type>*;

            const Type& v0 = static_cast<variable_ptr>(branch[0])->ref();
            const Type& v1 = static_cast<variable_ptr>(branch[1])->ref();
            const Type& v2 = static_cast<variable_ptr>(branch[2])->ref();

            switch (operation)
            {
#define CASE_STMT(op)                            \
    case core::operators::operator_type::sf##op: \
        return node_allocator_                   \
            ->allocate_rrr<details::sf3_var_node<Type, details::sf##op##_op<Type>>>(v0, v1, v2);

                CASE_STMT(00);
                CASE_STMT(01);
                CASE_STMT(02);
                CASE_STMT(03);
                CASE_STMT(04);
                CASE_STMT(05);
                CASE_STMT(06);
                CASE_STMT(07);
                CASE_STMT(08);
                CASE_STMT(09);
                CASE_STMT(10);
                CASE_STMT(11);
                CASE_STMT(12);
                CASE_STMT(13);
                CASE_STMT(14);
                CASE_STMT(15);
                CASE_STMT(16);
                CASE_STMT(17);
                CASE_STMT(18);
                CASE_STMT(19);
                CASE_STMT(20);
                CASE_STMT(21);
                CASE_STMT(22);
                CASE_STMT(23);
                CASE_STMT(24);
                CASE_STMT(25);
                CASE_STMT(26);
                CASE_STMT(27);
                CASE_STMT(28);
                CASE_STMT(29);
                CASE_STMT(30);
                CASE_STMT(31);
                CASE_STMT(32);
                CASE_STMT(33);
                CASE_STMT(34);
                CASE_STMT(35);
                CASE_STMT(36);
                CASE_STMT(37);
                CASE_STMT(38);
                CASE_STMT(39);
                CASE_STMT(40);
                CASE_STMT(41);
                CASE_STMT(42);
                CASE_STMT(43);
                CASE_STMT(44);
                CASE_STMT(45);
                CASE_STMT(46);
                CASE_STMT(47);
#undef CASE_STMT
                default:
                    return error_node();
            }
        }

        inline expression_node_ptr special_function(const core::operators::operator_type& operation,
                                                    expression_node_ptr (&branch)[3])
        {
            if (!all_nodes_valid(branch))
                return error_node();
            else if (fold_passes_t::is_constant_foldable(branch))
                return const_optimise_sf3(operation, branch);
            else if (all_nodes_variables(branch))
                return varnode_optimise_sf3(operation, branch);
            else
            {
                switch (operation)
                {
#define CASE_STMT(op)                                                                          \
    case core::operators::operator_type::sf##op:                                               \
        return node_allocator_->allocate<details::sf3_node<Type, details::sf##op##_op<Type>>>( \
            operation, branch);

                    CASE_STMT(00);
                    CASE_STMT(01);
                    CASE_STMT(02);
                    CASE_STMT(03);
                    CASE_STMT(04);
                    CASE_STMT(05);
                    CASE_STMT(06);
                    CASE_STMT(07);
                    CASE_STMT(08);
                    CASE_STMT(09);
                    CASE_STMT(10);
                    CASE_STMT(11);
                    CASE_STMT(12);
                    CASE_STMT(13);
                    CASE_STMT(14);
                    CASE_STMT(15);
                    CASE_STMT(16);
                    CASE_STMT(17);
                    CASE_STMT(18);
                    CASE_STMT(19);
                    CASE_STMT(20);
                    CASE_STMT(21);
                    CASE_STMT(22);
                    CASE_STMT(23);
                    CASE_STMT(24);
                    CASE_STMT(25);
                    CASE_STMT(26);
                    CASE_STMT(27);
                    CASE_STMT(28);
                    CASE_STMT(29);
                    CASE_STMT(30);
                    CASE_STMT(31);
                    CASE_STMT(32);
                    CASE_STMT(33);
                    CASE_STMT(34);
                    CASE_STMT(35);
                    CASE_STMT(36);
                    CASE_STMT(37);
                    CASE_STMT(38);
                    CASE_STMT(39);
                    CASE_STMT(40);
                    CASE_STMT(41);
                    CASE_STMT(42);
                    CASE_STMT(43);
                    CASE_STMT(44);
                    CASE_STMT(45);
                    CASE_STMT(46);
                    CASE_STMT(47);
#undef CASE_STMT
                    default:
                        return error_node();
                }
            }
        }

        inline expression_node_ptr const_optimise_sf4(
            const core::operators::operator_type& operation, expression_node_ptr (&branch)[4])
        {
            expression_node_ptr temp_node = error_node();

            switch (operation)
            {
#define CASE_STMT(op)                                                                       \
    case core::operators::operator_type::sf##op:                                            \
        temp_node =                                                                         \
            node_allocator_->allocate<details::sf4_node<Type, details::sf##op##_op<Type>>>( \
                operation, branch);                                                         \
        break;

                CASE_STMT(48);
                CASE_STMT(49);
                CASE_STMT(50);
                CASE_STMT(51);
                CASE_STMT(52);
                CASE_STMT(53);
                CASE_STMT(54);
                CASE_STMT(55);
                CASE_STMT(56);
                CASE_STMT(57);
                CASE_STMT(58);
                CASE_STMT(59);
                CASE_STMT(60);
                CASE_STMT(61);
                CASE_STMT(62);
                CASE_STMT(63);
                CASE_STMT(64);
                CASE_STMT(65);
                CASE_STMT(66);
                CASE_STMT(67);
                CASE_STMT(68);
                CASE_STMT(69);
                CASE_STMT(70);
                CASE_STMT(71);
                CASE_STMT(72);
                CASE_STMT(73);
                CASE_STMT(74);
                CASE_STMT(75);
                CASE_STMT(76);
                CASE_STMT(77);
                CASE_STMT(78);
                CASE_STMT(79);
                CASE_STMT(80);
                CASE_STMT(81);
                CASE_STMT(82);
                CASE_STMT(83);
                CASE_STMT(84);
                CASE_STMT(85);
                CASE_STMT(86);
                CASE_STMT(87);
                CASE_STMT(88);
                CASE_STMT(89);
                CASE_STMT(90);
                CASE_STMT(91);
                CASE_STMT(92);
                CASE_STMT(93);
                CASE_STMT(94);
                CASE_STMT(95);
                CASE_STMT(96);
                CASE_STMT(97);
                CASE_STMT(98);
                CASE_STMT(99);
#undef CASE_STMT
                default:
                    return error_node();
            }

            assert(temp_node);

            const T v = temp_node->value();

            details::free_node(*node_allocator_, temp_node);

            return node_allocator_->allocate<literal_node_t>(v);
        }

        inline expression_node_ptr varnode_optimise_sf4(
            const core::operators::operator_type& operation, expression_node_ptr (&branch)[4])
        {
            using variable_ptr = details::variable_node<Type>*;

            const Type& v0 = static_cast<variable_ptr>(branch[0])->ref();
            const Type& v1 = static_cast<variable_ptr>(branch[1])->ref();
            const Type& v2 = static_cast<variable_ptr>(branch[2])->ref();
            const Type& v3 = static_cast<variable_ptr>(branch[3])->ref();

            switch (operation)
            {
#define CASE_STMT(op)                                                                            \
    case core::operators::operator_type::sf##op:                                                 \
        return node_allocator_                                                                   \
            ->allocate_rrrr<details::sf4_var_node<Type, details::sf##op##_op<Type>>>(v0, v1, v2, \
                                                                                     v3);

                CASE_STMT(48);
                CASE_STMT(49);
                CASE_STMT(50);
                CASE_STMT(51);
                CASE_STMT(52);
                CASE_STMT(53);
                CASE_STMT(54);
                CASE_STMT(55);
                CASE_STMT(56);
                CASE_STMT(57);
                CASE_STMT(58);
                CASE_STMT(59);
                CASE_STMT(60);
                CASE_STMT(61);
                CASE_STMT(62);
                CASE_STMT(63);
                CASE_STMT(64);
                CASE_STMT(65);
                CASE_STMT(66);
                CASE_STMT(67);
                CASE_STMT(68);
                CASE_STMT(69);
                CASE_STMT(70);
                CASE_STMT(71);
                CASE_STMT(72);
                CASE_STMT(73);
                CASE_STMT(74);
                CASE_STMT(75);
                CASE_STMT(76);
                CASE_STMT(77);
                CASE_STMT(78);
                CASE_STMT(79);
                CASE_STMT(80);
                CASE_STMT(81);
                CASE_STMT(82);
                CASE_STMT(83);
                CASE_STMT(84);
                CASE_STMT(85);
                CASE_STMT(86);
                CASE_STMT(87);
                CASE_STMT(88);
                CASE_STMT(89);
                CASE_STMT(90);
                CASE_STMT(91);
                CASE_STMT(92);
                CASE_STMT(93);
                CASE_STMT(94);
                CASE_STMT(95);
                CASE_STMT(96);
                CASE_STMT(97);
                CASE_STMT(98);
                CASE_STMT(99);
#undef CASE_STMT
                default:
                    return error_node();
            }
        }

        inline expression_node_ptr special_function(const core::operators::operator_type& operation,
                                                    expression_node_ptr (&branch)[4])
        {
            if (!all_nodes_valid(branch))
                return error_node();
            else if (fold_passes_t::is_constant_foldable(branch))
                return const_optimise_sf4(operation, branch);
            else if (all_nodes_variables(branch))
                return varnode_optimise_sf4(operation, branch);
            switch (operation)
            {
#define CASE_STMT(op)                                                                          \
    case core::operators::operator_type::sf##op:                                               \
        return node_allocator_->allocate<details::sf4_node<Type, details::sf##op##_op<Type>>>( \
            operation, branch);

                CASE_STMT(48);
                CASE_STMT(49);
                CASE_STMT(50);
                CASE_STMT(51);
                CASE_STMT(52);
                CASE_STMT(53);
                CASE_STMT(54);
                CASE_STMT(55);
                CASE_STMT(56);
                CASE_STMT(57);
                CASE_STMT(58);
                CASE_STMT(59);
                CASE_STMT(60);
                CASE_STMT(61);
                CASE_STMT(62);
                CASE_STMT(63);
                CASE_STMT(64);
                CASE_STMT(65);
                CASE_STMT(66);
                CASE_STMT(67);
                CASE_STMT(68);
                CASE_STMT(69);
                CASE_STMT(70);
                CASE_STMT(71);
                CASE_STMT(72);
                CASE_STMT(73);
                CASE_STMT(74);
                CASE_STMT(75);
                CASE_STMT(76);
                CASE_STMT(77);
                CASE_STMT(78);
                CASE_STMT(79);
                CASE_STMT(80);
                CASE_STMT(81);
                CASE_STMT(82);
                CASE_STMT(83);
                CASE_STMT(84);
                CASE_STMT(85);
                CASE_STMT(86);
                CASE_STMT(87);
                CASE_STMT(88);
                CASE_STMT(89);
                CASE_STMT(90);
                CASE_STMT(91);
                CASE_STMT(92);
                CASE_STMT(93);
                CASE_STMT(94);
                CASE_STMT(95);
                CASE_STMT(96);
                CASE_STMT(97);
                CASE_STMT(98);
                CASE_STMT(99);
#undef CASE_STMT
                default:
                    return error_node();
            }
        }

        template <typename Allocator, template <typename, typename> class Sequence>
        inline expression_node_ptr const_optimise_varargfunc(
            const core::operators::operator_type& operation,
            Sequence<expression_node_ptr, Allocator>& arg_list)
        {
            expression_node_ptr temp_node = error_node();

            switch (operation)
            {
#define CASE_STMT(op0, op1)                                                                     \
    case op0:                                                                                   \
        temp_node = node_allocator_->allocate<details::vararg_node<Type, op1<Type>>>(arg_list); \
        break;

                CASE_STMT(core::operators::operator_type::sum, details::vararg_add_op);
                CASE_STMT(core::operators::operator_type::prod, details::vararg_mul_op);
                CASE_STMT(core::operators::operator_type::avg, details::vararg_avg_op);
                CASE_STMT(core::operators::operator_type::min, details::vararg_min_op);
                CASE_STMT(core::operators::operator_type::max, details::vararg_max_op);
                CASE_STMT(core::operators::operator_type::mand, details::vararg_mand_op);
                CASE_STMT(core::operators::operator_type::mor, details::vararg_mor_op);
                CASE_STMT(core::operators::operator_type::multi, details::vararg_multi_op);
#undef CASE_STMT
                default:
                    return error_node();
            }

            const T v = temp_node->value();

            details::free_node(*node_allocator_, temp_node);

            return node_allocator_->allocate<literal_node_t>(v);
        }

        inline bool special_one_parameter_vararg(
            const core::operators::operator_type& operation) const
        {
            return ((core::operators::operator_type::sum == operation) ||
                    (core::operators::operator_type::prod == operation) ||
                    (core::operators::operator_type::avg == operation) ||
                    (core::operators::operator_type::min == operation) ||
                    (core::operators::operator_type::max == operation));
        }

        template <typename Allocator, template <typename, typename> class Sequence>
        inline expression_node_ptr varnode_optimise_varargfunc(
            const core::operators::operator_type& operation,
            Sequence<expression_node_ptr, Allocator>& arg_list)
        {
            switch (operation)
            {
#define CASE_STMT(op0, op1) \
    case op0:               \
        return node_allocator_->allocate<details::vararg_varnode<Type, op1<Type>>>(arg_list);

                CASE_STMT(core::operators::operator_type::sum, details::vararg_add_op);
                CASE_STMT(core::operators::operator_type::prod, details::vararg_mul_op);
                CASE_STMT(core::operators::operator_type::avg, details::vararg_avg_op);
                CASE_STMT(core::operators::operator_type::min, details::vararg_min_op);
                CASE_STMT(core::operators::operator_type::max, details::vararg_max_op);
                CASE_STMT(core::operators::operator_type::mand, details::vararg_mand_op);
                CASE_STMT(core::operators::operator_type::mor, details::vararg_mor_op);
                CASE_STMT(core::operators::operator_type::multi, details::vararg_multi_op);
#undef CASE_STMT
                default:
                    return error_node();
            }
        }

        template <typename Allocator, template <typename, typename> class Sequence>
        inline expression_node_ptr vectorize_func(
            const core::operators::operator_type& operation,
            Sequence<expression_node_ptr, Allocator>& arg_list)
        {
            if (1 == arg_list.size())
            {
                switch (operation)
                {
#define CASE_STMT(op0, op1) \
    case op0:               \
        return node_allocator_->allocate<details::vectorize_node<Type, op1<Type>>>(arg_list[0]);

                    CASE_STMT(core::operators::operator_type::sum, details::vec_add_op)
                    CASE_STMT(core::operators::operator_type::prod, details::vec_mul_op)
                    CASE_STMT(core::operators::operator_type::avg, details::vec_avg_op)
                    CASE_STMT(core::operators::operator_type::min, details::vec_min_op)
                    CASE_STMT(core::operators::operator_type::max, details::vec_max_op)
#undef CASE_STMT
                    default:
                        return error_node();
                }
            }
            else
                return error_node();
        }

        template <typename Allocator, template <typename, typename> class Sequence>
        inline expression_node_ptr vararg_function(
            const core::operators::operator_type& operation,
            Sequence<expression_node_ptr, Allocator>& arg_list)
        {
            if (!all_nodes_valid(arg_list))
            {
                details::free_all_nodes(*node_allocator_, arg_list);

                return error_node();
            }
            else if (fold_passes_t::is_constant_foldable(arg_list))
                return const_optimise_varargfunc(operation, arg_list);
            else if ((1 == arg_list.size()) && details::is_ivector_node(arg_list[0]))
                return vectorize_func(operation, arg_list);
            else if ((1 == arg_list.size()) && special_one_parameter_vararg(operation))
                return arg_list[0];
            else if (all_nodes_variables(arg_list))
                return varnode_optimise_varargfunc(operation, arg_list);

#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
            if (core::operators::operator_type::smulti == operation)
            {
                expression_node_ptr result = node_allocator_->allocate<
                    details::string_nodes::str_vararg_node<Type, details::vararg_multi_op<Type>>>(
                    arg_list);
                if (result && result->valid())
                {
                    return result;
                }

                parser_->set_error(parser_error::make_error(
                    parser_error::error_mode::e_synthesis, token_t(),
                    "ERR262 - Failed to synthesize node: str_vararg_node<vararg_multi_op>",
                    core::error_location()));

                details::free_node(*node_allocator_, result);
            }
            else
#endif
            {
                expression_node_ptr result = error_node();

                switch (operation)
                {
#define CASE_STMT(op0, op1)                                                                  \
    case op0:                                                                                \
        result = node_allocator_->allocate<details::vararg_node<Type, op1<Type>>>(arg_list); \
        break;

                    CASE_STMT(core::operators::operator_type::sum, details::vararg_add_op)
                    CASE_STMT(core::operators::operator_type::prod, details::vararg_mul_op)
                    CASE_STMT(core::operators::operator_type::avg, details::vararg_avg_op)
                    CASE_STMT(core::operators::operator_type::min, details::vararg_min_op)
                    CASE_STMT(core::operators::operator_type::max, details::vararg_max_op)
                    CASE_STMT(core::operators::operator_type::mand, details::vararg_mand_op)
                    CASE_STMT(core::operators::operator_type::mor, details::vararg_mor_op)
                    CASE_STMT(core::operators::operator_type::multi, details::vararg_multi_op)
#undef CASE_STMT
                    default:
                        return error_node();
                }

                if (result && result->valid())
                {
                    return result;
                }

                parser_->set_error(parser_error::make_error(
                    parser_error::error_mode::e_synthesis, token_t(),
                    "ERR263 - Failed to synthesize node: vararg_node", core::error_location()));

                details::free_node(*node_allocator_, result);
            }

            return error_node();
        }

        template <std::size_t N>
        inline expression_node_ptr function(ifunction_t* f, expression_node_ptr (&b)[N])
        {
            using function_N_node_t = typename details::function_N_node<T, ifunction_t, N>;
            expression_node_ptr result = synthesize_expression<function_N_node_t, N>(f, b);

            if (nullptr == result)
                return error_node();
            else
            {
                // Can the function call be completely optimised?
                if (details::is_constant_node(result))
                    return result;
                else if (!all_nodes_valid(b))
                {
                    details::free_node(*node_allocator_, result);
                    std::fill_n(b, N, nullptr);

                    return error_node();
                }
                else if (N != f->param_count)
                {
                    details::free_node(*node_allocator_, result);
                    std::fill_n(b, N, nullptr);

                    return error_node();
                }

                function_N_node_t* func_node_ptr = reinterpret_cast<function_N_node_t*>(result);

                if (!func_node_ptr->init_branches(b))
                {
                    details::free_node(*node_allocator_, result);
                    std::fill_n(b, N, nullptr);

                    return error_node();
                }

                if (result && result->valid())
                {
                    return result;
                }

                parser_->set_error(parser_error::make_error(
                    parser_error::error_mode::e_synthesis, token_t(),
                    "ERR264 - Failed to synthesize node: function_N_node_t",
                    core::error_location()));

                details::free_node(*node_allocator_, result);
                return error_node();
            }
        }

        inline expression_node_ptr function(ifunction_t* f)
        {
            using function_N_node_t = typename details::function_N_node<Type, ifunction_t, 0>;
            return node_allocator_->allocate<function_N_node_t>(f);
        }

        inline expression_node_ptr vararg_function_call(ivararg_function_t* vaf,
                                                        std::vector<expression_node_ptr>& arg_list)
        {
            if (!all_nodes_valid(arg_list))
            {
                details::free_all_nodes(*node_allocator_, arg_list);

                return error_node();
            }

            using alloc_type = details::vararg_function_node<Type, ivararg_function_t>;

            expression_node_ptr result = node_allocator_->allocate<alloc_type>(vaf, arg_list);

            if (!arg_list.empty() && !vaf->has_side_effects() &&
                fold_passes_t::is_constant_foldable(arg_list))
            {
                const Type v = result->value();
                details::free_node(*node_allocator_, result);
                result = node_allocator_->allocate<literal_node_t>(v);
            }

            parser_->state_.activate_side_effect("vararg_function_call()");

            if (result && result->valid())
            {
                return result;
            }

            parser_->set_error(parser_error::make_error(
                parser_error::error_mode::e_synthesis, token_t(),
                "ERR265 - Failed to synthesize node: vararg_function_node<ivararg_function_t>",
                core::error_location()));

            details::free_node(*node_allocator_, result);
            return error_node();
        }

        inline expression_node_ptr generic_function_call(
            igeneric_function_t* gf, std::vector<expression_node_ptr>& arg_list,
            const std::size_t& param_seq_index = std::numeric_limits<std::size_t>::max())
        {
            if (!all_nodes_valid(arg_list))
            {
                details::free_all_nodes(*node_allocator_, arg_list);
                return error_node();
            }

            using alloc_type1 = details::generic_function_node<Type, igeneric_function_t>;
            using alloc_type2 = details::multimode_genfunction_node<Type, igeneric_function_t>;

            const std::size_t no_psi = std::numeric_limits<std::size_t>::max();

            expression_node_ptr result = error_node();
            std::string node_name = "Unknown";

            if (no_psi == param_seq_index)
            {
                result = node_allocator_->allocate<alloc_type1>(arg_list, gf);
                node_name = "generic_function_node<igeneric_function_t>";
            }
            else
            {
                result = node_allocator_->allocate<alloc_type2>(gf, param_seq_index, arg_list);
                node_name = "multimode_genfunction_node<igeneric_function_t>";
            }

            alloc_type1* genfunc_node_ptr = static_cast<alloc_type1*>(result);

            assert(genfunc_node_ptr);

            if (!arg_list.empty() && !gf->has_side_effects() &&
                parser_->state_.type_check_enabled && fold_passes_t::is_constant_foldable(arg_list))
            {
                genfunc_node_ptr->init_branches();

                const Type v = result->value();

                details::free_node(*node_allocator_, result);

                return node_allocator_->allocate<literal_node_t>(v);
            }
            else if (genfunc_node_ptr->init_branches())
            {
                if (result && result->valid())
                {
                    parser_->state_.activate_side_effect("generic_function_call()");
                    return result;
                }

                parser_->set_error(parser_error::make_error(
                    parser_error::error_mode::e_synthesis, token_t(),
                    "ERR266 - Failed to synthesize node: " + node_name, core::error_location()));

                details::free_node(*node_allocator_, result);
                return error_node();
            }
            else
            {
                details::free_node(*node_allocator_, result);
                details::free_all_nodes(*node_allocator_, arg_list);

                return error_node();
            }
        }

#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
        inline expression_node_ptr string_function_call(
            igeneric_function_t* gf, std::vector<expression_node_ptr>& arg_list,
            const std::size_t& param_seq_index = std::numeric_limits<std::size_t>::max())
        {
            if (!all_nodes_valid(arg_list))
            {
                details::free_all_nodes(*node_allocator_, arg_list);
                return error_node();
            }

            using alloc_type1 = details::string_function_node<Type, igeneric_function_t>;
            using alloc_type2 = details::multimode_strfunction_node<Type, igeneric_function_t>;

            const std::size_t no_psi = std::numeric_limits<std::size_t>::max();

            expression_node_ptr result = error_node();
            std::string node_name = "Unknown";

            if (no_psi == param_seq_index)
            {
                result = node_allocator_->allocate<alloc_type1>(gf, arg_list);
                node_name = "string_function_node<igeneric_function_t>";
            }
            else
            {
                result = node_allocator_->allocate<alloc_type2>(gf, param_seq_index, arg_list);
                node_name = "multimode_strfunction_node<igeneric_function_t>";
            }

            alloc_type1* strfunc_node_ptr = static_cast<alloc_type1*>(result);

            assert(strfunc_node_ptr);

            if (!arg_list.empty() && !gf->has_side_effects() &&
                fold_passes_t::is_constant_foldable(arg_list))
            {
                strfunc_node_ptr->init_branches();

                const Type v = result->value();

                details::free_node(*node_allocator_, result);

                return node_allocator_->allocate<literal_node_t>(v);
            }
            else if (strfunc_node_ptr->init_branches())
            {
                if (result && result->valid())
                {
                    parser_->state_.activate_side_effect("string_function_call()");
                    return result;
                }

                parser_->set_error(parser_error::make_error(
                    parser_error::error_mode::e_synthesis, token_t(),
                    "ERR267 - Failed to synthesize node: " + node_name, core::error_location()));

                details::free_node(*node_allocator_, result);
                return error_node();
            }
            else
            {
                details::free_node(*node_allocator_, result);
                details::free_all_nodes(*node_allocator_, arg_list);

                return error_node();
            }
        }
#endif

#ifndef MATH_EXPR_DISABLE_RETURN_STATEMENT
        inline expression_node_ptr return_call(std::vector<expression_node_ptr>& arg_list)
        {
            if (!all_nodes_valid(arg_list))
            {
                details::free_all_nodes(*node_allocator_, arg_list);
                return error_node();
            }

            using alloc_type = details::return_node<Type>;

            expression_node_ptr result =
                node_allocator_->allocate_rr<alloc_type>(arg_list, parser_->results_ctx());

            alloc_type* return_node_ptr = static_cast<alloc_type*>(result);

            assert(return_node_ptr);

            if (return_node_ptr->init_branches())
            {
                if (result && result->valid())
                {
                    parser_->state_.activate_side_effect("return_call()");
                    return result;
                }

                parser_->set_error(parser_error::make_error(
                    parser_error::error_mode::e_synthesis, token_t(),
                    "ERR268 - Failed to synthesize node: return_node", core::error_location()));

                details::free_node(*node_allocator_, result);
                return error_node();
            }
            else
            {
                details::free_node(*node_allocator_, result);
                details::free_all_nodes(*node_allocator_, arg_list);

                return error_node();
            }
        }

        inline expression_node_ptr return_envelope(expression_node_ptr body, results_context_t* rc,
                                                   bool*& return_invoked)
        {
            using alloc_type = details::return_envelope_node<Type>;

            expression_node_ptr result = node_allocator_->allocate_cr<alloc_type>(body, (*rc));

            return_invoked = static_cast<alloc_type*>(result)->retinvk_ptr();

            return result;
        }
#else
        inline expression_node_ptr return_call(std::vector<expression_node_ptr>&)
        {
            return error_node();
        }

        inline expression_node_ptr return_envelope(expression_node_ptr, results_context_t*, bool*&)
        {
            return error_node();
        }
#endif

        inline expression_node_ptr vector_element(const std::string& symbol,
                                                  vector_holder_ptr vector_base,
                                                  expression_node_ptr vec_node,
                                                  expression_node_ptr index)
        {
            expression_node_ptr result = error_node();
            std::string node_name = "Unknown";

            if (details::is_constant_node(index))
            {
                const std::size_t vec_index =
                    static_cast<std::size_t>(core::numeric::to_int64(index->value()));

                details::free_node(*node_allocator_, index);

                if (vec_index >= vector_base->size())
                {
                    parser_->set_error(parser_error::make_error(
                        parser_error::error_mode::e_parser, token_t(),
                        "ERR269 - Index of " + core::to_str(vec_index) +
                            " out of range for "
                            "vector '" +
                            symbol + "' of size " + core::to_str(vector_base->size()),
                        core::error_location()));

                    details::free_node(*node_allocator_, vec_node);

                    return error_node();
                }

                if (vector_base->rebaseable())
                {
                    vector_access_runtime_check<T>* rtc = get_vector_access_runtime_check();

                    result = (rtc) ? node_allocator_->allocate<rebasevector_celem_rtc_node_t>(
                                         vec_node, vec_index, vector_base, rtc)
                                   : node_allocator_->allocate<rebasevector_celem_node_t>(
                                         vec_node, vec_index, vector_base);

                    node_name = (rtc) ? "rebasevector_elem_rtc_node_t" : "rebasevector_elem_node_t";

                    if (result && result->valid())
                    {
                        return result;
                    }

                    parser_->set_error(parser_error::make_error(
                        parser_error::error_mode::e_synthesis, token_t(),
                        "ERR270 - Failed to synthesize node: " + node_name +
                            " for vector: " + symbol,
                        core::error_location()));

                    details::free_node(*node_allocator_, result);
                    return error_node();
                }
                else if (details::is_ivector_node(vec_node) && !details::is_vector_node(vec_node))
                {
                    vector_access_runtime_check<T>* rtc = get_vector_access_runtime_check();

                    result = (rtc) ? node_allocator_->allocate<vector_celem_rtc_node_t>(
                                         vec_node, vec_index, vector_base, rtc)
                                   : node_allocator_->allocate<vector_celem_node_t>(
                                         vec_node, vec_index, vector_base);

                    node_name = (rtc) ? "vector_elem_rtc_node_t" : "vector_elem_node_t";

                    if (result && result->valid())
                    {
                        return result;
                    }

                    parser_->set_error(parser_error::make_error(
                        parser_error::error_mode::e_synthesis, token_t(),
                        "ERR271 - Failed to synthesize node: " + node_name +
                            " for vector: " + symbol,
                        core::error_location()));

                    details::free_node(*node_allocator_, result);
                    return error_node();
                }

                const scope_element& se = parser_->sem_.get_element(symbol, vec_index);

                if (se.index == vec_index)
                {
                    result = se.var_node;
                    details::free_node(*node_allocator_, vec_node);
                }
                else
                {
                    scope_element nse;
                    nse.name = symbol;
                    nse.active = true;
                    nse.ref_count = 1;
                    nse.type = scope_element::element_type::e_vecelem;
                    nse.index = vec_index;
                    nse.depth = parser_->state_.scope_depth;
                    nse.var_node =
                        node_allocator_->allocate<variable_node_t>((*(*vector_base)[vec_index]));

                    if (!parser_->sem_.add_element(std::move(nse)))
                    {
                        parser_->set_synthesis_error(
                            "Failed to add new local vector element to SEM [1]");

                        parser_->sem_.free_element(nse);

                        result = error_node();
                    }

                    assert(parser_->sem_.total_local_symb_size_bytes() <=
                           parser_->settings().max_total_local_symbol_size_bytes());

                    details::free_node(*node_allocator_, vec_node);

                    core::debug_print(
                        "vector_element() - INFO - Added new local vector element: %s\n",
                        nse.name.c_str());

                    parser_->state_.activate_side_effect("vector_element()");

                    result = nse.var_node;
                    node_name = "variable_node_t";
                }
            }
            else
            {
                vector_access_runtime_check<T>* rtc = get_vector_access_runtime_check();

                if (vector_base->rebaseable())
                {
                    result = (rtc) ? node_allocator_->allocate<rebasevector_elem_rtc_node_t>(
                                         vec_node, index, vector_base, rtc)
                                   : node_allocator_->allocate<rebasevector_elem_node_t>(
                                         vec_node, index, vector_base);

                    node_name = (rtc) ? "rebasevector_elem_rtc_node_t" : "rebasevector_elem_node_t";
                }
                else
                {
                    result = rtc ? node_allocator_->allocate<vector_elem_rtc_node_t>(
                                       vec_node, index, vector_base, rtc)
                                 : node_allocator_->allocate<vector_elem_node_t>(vec_node, index,
                                                                                 vector_base);

                    node_name = (rtc) ? "vector_elem_rtc_node_t" : "vector_elem_node_t";
                }
            }

            if (result && result->valid())
            {
                return result;
            }

            parser_->set_error(parser_error::make_error(
                parser_error::error_mode::e_synthesis, token_t(),
                "ERR272 - Failed to synthesize node: " + node_name, core::error_location()));

            details::free_node(*node_allocator_, result);
            return error_node();
        }

       private:
        void lodge_assignment(symbol_type cst, expression_node_ptr node)
        {
            parser_->state_.activate_side_effect("lodge_assignment()");

            if (!parser_->dec_.collect_assignments())
                return;

            std::string symbol_name;

            switch (cst)
            {
                case symbol_type::e_st_variable:
                    symbol_name = parser_->symtab_store_.get_variable_name(node);
                    break;

#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
                case symbol_type::e_st_string:
                    symbol_name = parser_->symtab_store_.get_stringvar_name(node);
                    break;
#endif

                case symbol_type::e_st_vector:
                {
                    using vector_holder_t = details::vector_holder<T>;

                    vector_holder_t& vh = static_cast<vector_node_t*>(node)->vec_holder();

                    symbol_name = parser_->symtab_store_.get_vector_name(&vh);
                }
                break;

                case symbol_type::e_st_vecelem:
                {
                    using vector_holder_t = details::vector_holder<T>;

                    vector_holder_t& vh = static_cast<vector_elem_node_t*>(node)->vec_holder();

                    symbol_name = parser_->symtab_store_.get_vector_name(&vh);

                    cst = symbol_type::e_st_vector;
                }
                break;

                default:
                    return;
            }

            if (!symbol_name.empty())
            {
                parser_->dec_.add_assignment(symbol_name, cst);
            }
        }

        std::uintptr_t base_ptr(expression_node_ptr node)
        {
            if (node)
            {
                switch (node->type())
                {
                    case details::expression_node<T>::node_type::e_variable:
                        return reinterpret_cast<std::uintptr_t>(
                            &static_cast<variable_node_t*>(node)->ref());

                    case details::expression_node<T>::node_type::e_vecelem:
                        return reinterpret_cast<std::uintptr_t>(
                            &static_cast<vector_elem_node_t*>(node)->ref());

                    case details::expression_node<T>::node_type::e_veccelem:
                        return reinterpret_cast<std::uintptr_t>(
                            &static_cast<vector_celem_node_t*>(node)->ref());

                    case details::expression_node<T>::node_type::e_vecelemrtc:
                        return reinterpret_cast<std::uintptr_t>(
                            &static_cast<vector_elem_rtc_node_t*>(node)->ref());

                    case details::expression_node<T>::node_type::e_veccelemrtc:
                        return reinterpret_cast<std::uintptr_t>(
                            &static_cast<vector_celem_rtc_node_t*>(node)->ref());

                    case details::expression_node<T>::node_type::e_rbvecelem:
                        return reinterpret_cast<std::uintptr_t>(
                            &static_cast<rebasevector_elem_node_t*>(node)->ref());

                    case details::expression_node<T>::node_type::e_rbvecelemrtc:
                        return reinterpret_cast<std::uintptr_t>(
                            &static_cast<rebasevector_elem_rtc_node_t*>(node)->ref());

                    case details::expression_node<T>::node_type::e_rbveccelem:
                        return reinterpret_cast<std::uintptr_t>(
                            &static_cast<rebasevector_celem_node_t*>(node)->ref());

                    case details::expression_node<T>::node_type::e_rbveccelemrtc:
                        return reinterpret_cast<std::uintptr_t>(
                            &static_cast<rebasevector_celem_rtc_node_t*>(node)->ref());

                    case details::expression_node<T>::node_type::e_vector:
                        return reinterpret_cast<std::uintptr_t>(
                            static_cast<vector_node_t*>(node)->vec_holder().data());

#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
                    case details::expression_node<T>::node_type::e_stringvar:
                        return reinterpret_cast<std::uintptr_t>(
                            static_cast<stringvar_node_t*>(node)->base());

                    case details::expression_node<T>::node_type::e_stringvarrng:
                        return reinterpret_cast<std::uintptr_t>(
                            static_cast<string_range_node_t*>(node)->base());
#endif
                    default:
                        return 0;
                }
            }

            return 0;
        }

        bool assign_immutable_symbol(expression_node_ptr node)
        {
            interval_t interval;
            const std::uintptr_t baseptr_addr = base_ptr(node);

            core::debug_print("assign_immutable_symbol - base ptr addr: 0x%zx\n",
                              static_cast<std::size_t>(baseptr_addr));

            if (parser_->immutable_memory_map_.in_interval(baseptr_addr, interval))
            {
                typename immutable_symtok_map_t::iterator itr =
                    parser_->immutable_symtok_map_.find(interval);

                if (parser_->immutable_symtok_map_.end() != itr)
                {
                    token_t& token = itr->second;
                    parser_->set_error(
                        parser_error::make_error(parser_error::error_mode::e_parser, token,
                                                 "ERR273 - Symbol '" + token.value +
                                                     "' cannot be assigned-to as it is immutable.",
                                                 core::error_location()));
                }
                else
                    parser_->set_synthesis_error("Unable to assign symbol is immutable.");

                return true;
            }

            return false;
        }

        inline expression_node_ptr synthesize_assignment_expression(
            const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
        {
            if (assign_immutable_symbol(branch[0]))
            {
                return error_node();
            }
            else if (details::is_variable_node(branch[0]))
            {
                lodge_assignment(symbol_type::e_st_variable, branch[0]);
                return synthesize_expression<assignment_node_t, 2>(operation, branch);
            }
            else if (details::is_vector_elem_node(branch[0]) ||
                     details::is_vector_celem_node(branch[0]))
            {
                lodge_assignment(symbol_type::e_st_vecelem, branch[0]);
                return synthesize_expression<assignment_vec_elem_node_t, 2>(operation, branch);
            }
            else if (details::is_vector_elem_rtc_node(branch[0]) ||
                     details::is_vector_celem_rtc_node(branch[0]))
            {
                lodge_assignment(symbol_type::e_st_vecelem, branch[0]);
                return synthesize_expression<assignment_vec_elem_rtc_node_t, 2>(operation, branch);
            }
            else if (details::is_rebasevector_elem_node(branch[0]))
            {
                lodge_assignment(symbol_type::e_st_vecelem, branch[0]);
                return synthesize_expression<assignment_rebasevec_elem_node_t, 2>(operation,
                                                                                  branch);
            }
            else if (details::is_rebasevector_elem_rtc_node(branch[0]))
            {
                lodge_assignment(symbol_type::e_st_vecelem, branch[0]);
                return synthesize_expression<assignment_rebasevec_elem_rtc_node_t, 2>(operation,
                                                                                      branch);
            }
            else if (details::is_rebasevector_celem_node(branch[0]))
            {
                lodge_assignment(symbol_type::e_st_vecelem, branch[0]);
                return synthesize_expression<assignment_rebasevec_celem_node_t, 2>(operation,
                                                                                   branch);
            }
#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
            else if (details::is_string_node(branch[0]))
            {
                lodge_assignment(symbol_type::e_st_string, branch[0]);
                return synthesize_expression<assignment_string_node_t, 2>(operation, branch);
            }
            else if (details::is_string_range_node(branch[0]))
            {
                lodge_assignment(symbol_type::e_st_string, branch[0]);
                return synthesize_expression<assignment_string_range_node_t, 2>(operation, branch);
            }
#endif
            else if (details::is_vector_node(branch[0]))
            {
                lodge_assignment(symbol_type::e_st_vector, branch[0]);

                if (details::is_ivector_node(branch[1]))
                    return synthesize_expression<assignment_vecvec_node_t, 2>(operation, branch);
                else
                    return synthesize_expression<assignment_vec_node_t, 2>(operation, branch);
            }
            else if (details::is_literal_node(branch[0]))
            {
                parser_->set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, parser_->current_state().token,
                    "ERR274 - Cannot assign value to const variable", core::error_location()));

                return error_node();
            }
            else
            {
                parser_->set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, parser_->current_state().token,
                    "ERR275 - Invalid branches for assignment operator '" +
                        core::operators::to_str(operation) + "'",
                    core::error_location()));

                return error_node();
            }
        }

        inline expression_node_ptr synthesize_assignment_operation_expression(
            const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
        {
            if (assign_immutable_symbol(branch[0]))
            {
                return error_node();
            }

            expression_node_ptr result = error_node();
            std::string node_name = "Unknown";

            if (details::is_variable_node(branch[0]))
            {
                lodge_assignment(symbol_type::e_st_variable, branch[0]);

                switch (operation)
                {
#define CASE_STMT(op0, op1)                                                                     \
    case op0:                                                                                   \
        result =                                                                                \
            node_allocator_                                                                     \
                ->template allocate_rrr<typename details::assignment_op_node<Type, op1<Type>>>( \
                    operation, branch[0], branch[1]);                                           \
        node_name = "assignment_op_node";                                                       \
        break;

                    CASE_STMT(core::operators::operator_type::addass, details::add_op)
                    CASE_STMT(core::operators::operator_type::subass, details::sub_op)
                    CASE_STMT(core::operators::operator_type::mulass, details::mul_op)
                    CASE_STMT(core::operators::operator_type::divass, details::div_op)
                    CASE_STMT(core::operators::operator_type::modass, details::mod_op)
#undef CASE_STMT
                    default:
                        return error_node();
                }
            }
            else if (details::is_vector_elem_node(branch[0]))
            {
                lodge_assignment(symbol_type::e_st_vecelem, branch[0]);

                switch (operation)
                {
#define CASE_STMT(op0, op1)                                                                       \
    case op0:                                                                                     \
        result = node_allocator_->template allocate_rrr<                                          \
            typename details::assignment_vec_elem_op_node<Type, op1<Type>>>(operation, branch[0], \
                                                                            branch[1]);           \
        node_name = "assignment_vec_elem_op_node";                                                \
        break;

                    CASE_STMT(core::operators::operator_type::addass, details::add_op)
                    CASE_STMT(core::operators::operator_type::subass, details::sub_op)
                    CASE_STMT(core::operators::operator_type::mulass, details::mul_op)
                    CASE_STMT(core::operators::operator_type::divass, details::div_op)
                    CASE_STMT(core::operators::operator_type::modass, details::mod_op)
#undef CASE_STMT
                    default:
                        return error_node();
                }
            }
            else if (details::is_vector_elem_rtc_node(branch[0]))
            {
                lodge_assignment(symbol_type::e_st_vecelem, branch[0]);

                switch (operation)
                {
#define CASE_STMT(op0, op1)                                                      \
    case op0:                                                                    \
        result = node_allocator_->template allocate_rrr<                         \
            typename details::assignment_vec_elem_op_rtc_node<Type, op1<Type>>>( \
            operation, branch[0], branch[1]);                                    \
        node_name = "assignment_vec_elem_op_rtc_node";                           \
        break;

                    CASE_STMT(core::operators::operator_type::addass, details::add_op)
                    CASE_STMT(core::operators::operator_type::subass, details::sub_op)
                    CASE_STMT(core::operators::operator_type::mulass, details::mul_op)
                    CASE_STMT(core::operators::operator_type::divass, details::div_op)
                    CASE_STMT(core::operators::operator_type::modass, details::mod_op)
#undef CASE_STMT
                    default:
                        return error_node();
                }
            }
            else if (details::is_vector_celem_rtc_node(branch[0]))
            {
                lodge_assignment(symbol_type::e_st_vecelem, branch[0]);

                switch (operation)
                {
#define CASE_STMT(op0, op1)                                                       \
    case op0:                                                                     \
        result = node_allocator_->template allocate_rrr<                          \
            typename details::assignment_vec_celem_op_rtc_node<Type, op1<Type>>>( \
            operation, branch[0], branch[1]);                                     \
        node_name = "assignment_vec_celem_op_rtc_node";                           \
        break;

                    CASE_STMT(core::operators::operator_type::addass, details::add_op)
                    CASE_STMT(core::operators::operator_type::subass, details::sub_op)
                    CASE_STMT(core::operators::operator_type::mulass, details::mul_op)
                    CASE_STMT(core::operators::operator_type::divass, details::div_op)
                    CASE_STMT(core::operators::operator_type::modass, details::mod_op)
#undef CASE_STMT
                    default:
                        return error_node();
                }
            }
            else if (details::is_rebasevector_elem_node(branch[0]))
            {
                lodge_assignment(symbol_type::e_st_vecelem, branch[0]);

                switch (operation)
                {
#define CASE_STMT(op0, op1)                                                        \
    case op0:                                                                      \
        result = node_allocator_->template allocate_rrr<                           \
            typename details::assignment_rebasevec_elem_op_node<Type, op1<Type>>>( \
            operation, branch[0], branch[1]);                                      \
        node_name = "assignment_rebasevec_elem_op_node";                           \
        break;

                    CASE_STMT(core::operators::operator_type::addass, details::add_op)
                    CASE_STMT(core::operators::operator_type::subass, details::sub_op)
                    CASE_STMT(core::operators::operator_type::mulass, details::mul_op)
                    CASE_STMT(core::operators::operator_type::divass, details::div_op)
                    CASE_STMT(core::operators::operator_type::modass, details::mod_op)
#undef CASE_STMT
                    default:
                        return error_node();
                }
            }
            else if (details::is_rebasevector_celem_node(branch[0]))
            {
                lodge_assignment(symbol_type::e_st_vecelem, branch[0]);

                switch (operation)
                {
#define CASE_STMT(op0, op1)                                                         \
    case op0:                                                                       \
        result = node_allocator_->template allocate_rrr<                            \
            typename details::assignment_rebasevec_celem_op_node<Type, op1<Type>>>( \
            operation, branch[0], branch[1]);                                       \
        node_name = "assignment_rebasevec_celem_op_node";                           \
        break;

                    CASE_STMT(core::operators::operator_type::addass, details::add_op)
                    CASE_STMT(core::operators::operator_type::subass, details::sub_op)
                    CASE_STMT(core::operators::operator_type::mulass, details::mul_op)
                    CASE_STMT(core::operators::operator_type::divass, details::div_op)
                    CASE_STMT(core::operators::operator_type::modass, details::mod_op)
#undef CASE_STMT
                    default:
                        return error_node();
                }
            }
            else if (details::is_rebasevector_elem_rtc_node(branch[0]))
            {
                lodge_assignment(symbol_type::e_st_vecelem, branch[0]);

                switch (operation)
                {
#define CASE_STMT(op0, op1)                                                            \
    case op0:                                                                          \
        result = node_allocator_->template allocate_rrr<                               \
            typename details::assignment_rebasevec_elem_op_rtc_node<Type, op1<Type>>>( \
            operation, branch[0], branch[1]);                                          \
        node_name = "assignment_rebasevec_elem_op_rtc_node";                           \
        break;

                    CASE_STMT(core::operators::operator_type::addass, details::add_op)
                    CASE_STMT(core::operators::operator_type::subass, details::sub_op)
                    CASE_STMT(core::operators::operator_type::mulass, details::mul_op)
                    CASE_STMT(core::operators::operator_type::divass, details::div_op)
                    CASE_STMT(core::operators::operator_type::modass, details::mod_op)
#undef CASE_STMT
                    default:
                        return error_node();
                }
            }
            else if (details::is_rebasevector_celem_rtc_node(branch[0]))
            {
                lodge_assignment(symbol_type::e_st_vecelem, branch[0]);

                switch (operation)
                {
#define CASE_STMT(op0, op1)                                                             \
    case op0:                                                                           \
        result = node_allocator_->template allocate_rrr<                                \
            typename details::assignment_rebasevec_celem_op_rtc_node<Type, op1<Type>>>( \
            operation, branch[0], branch[1]);                                           \
        node_name = "assignment_rebasevec_celem_op_rtc_node";                           \
        break;

                    CASE_STMT(core::operators::operator_type::addass, details::add_op)
                    CASE_STMT(core::operators::operator_type::subass, details::sub_op)
                    CASE_STMT(core::operators::operator_type::mulass, details::mul_op)
                    CASE_STMT(core::operators::operator_type::divass, details::div_op)
                    CASE_STMT(core::operators::operator_type::modass, details::mod_op)
#undef CASE_STMT
                    default:
                        return error_node();
                }
            }
            else if (details::is_vector_node(branch[0]))
            {
                lodge_assignment(symbol_type::e_st_vector, branch[0]);

                if (details::is_ivector_node(branch[1]))
                {
                    switch (operation)
                    {
#define CASE_STMT(op0, op1)                                                                     \
    case op0:                                                                                   \
        result = node_allocator_->template allocate_rrr<                                        \
            typename details::assignment_vecvec_op_node<Type, op1<Type>>>(operation, branch[0], \
                                                                          branch[1]);           \
        node_name = "assignment_rebasevec_celem_op_node";                                       \
        break;

                        CASE_STMT(core::operators::operator_type::addass, details::add_op)
                        CASE_STMT(core::operators::operator_type::subass, details::sub_op)
                        CASE_STMT(core::operators::operator_type::mulass, details::mul_op)
                        CASE_STMT(core::operators::operator_type::divass, details::div_op)
                        CASE_STMT(core::operators::operator_type::modass, details::mod_op)
#undef CASE_STMT
                        default:
                            return error_node();
                    }
                }
                else
                {
                    switch (operation)
                    {
#define CASE_STMT(op0, op1)                                                                  \
    case op0:                                                                                \
        result = node_allocator_->template allocate_rrr<                                     \
            typename details::assignment_vec_op_node<Type, op1<Type>>>(operation, branch[0], \
                                                                       branch[1]);           \
        node_name = "assignment_vec_op_node";                                                \
        break;

                        CASE_STMT(core::operators::operator_type::addass, details::add_op)
                        CASE_STMT(core::operators::operator_type::subass, details::sub_op)
                        CASE_STMT(core::operators::operator_type::mulass, details::mul_op)
                        CASE_STMT(core::operators::operator_type::divass, details::div_op)
                        CASE_STMT(core::operators::operator_type::modass, details::mod_op)
#undef CASE_STMT
                        default:
                            return error_node();
                    }
                }
            }
#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
            else if ((core::operators::operator_type::addass == operation) &&
                     details::is_string_node(branch[0]))
            {
                using addass_t = details::string_nodes::assignment_string_node<
                    T, details::string_nodes::asn_addassignment>;

                lodge_assignment(symbol_type::e_st_string, branch[0]);

                result = synthesize_expression<addass_t, 2>(operation, branch);
                node_name = "assignment_string_node<T,details::string_nodes::asn_addassignment>";
            }
#endif
            else
            {
                parser_->set_error(parser_error::make_error(
                    parser_error::error_mode::e_syntax, parser_->current_state().token,
                    "ERR276 - Invalid branches for assignment operator '" +
                        core::operators::to_str(operation) + "'",
                    core::error_location()));

                return error_node();
            }

            if (result && result->valid())
            {
                return result;
            }

            parser_->set_error(parser_error::make_error(
                parser_error::error_mode::e_synthesis, token_t(),
                "ERR277 - Failed to synthesize node: " + node_name, core::error_location()));

            details::free_node(*node_allocator_, result);
            return error_node();
        }

        inline expression_node_ptr synthesize_veceqineqlogic_operation_expression(
            const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
        {
            const bool is_b0_ivec = details::is_ivector_node(branch[0]);
            const bool is_b1_ivec = details::is_ivector_node(branch[1]);

#define BATCH_EQINEQ_LOGIC_CASE                                             \
    CASE_STMT(core::operators::operator_type::lt, details::lt_op)           \
    CASE_STMT(core::operators::operator_type::lte, details::lte_op)         \
    CASE_STMT(core::operators::operator_type::gt, details::gt_op)           \
    CASE_STMT(core::operators::operator_type::gte, details::gte_op)         \
    CASE_STMT(core::operators::operator_type::eq, details::eq_op)           \
    CASE_STMT(core::operators::operator_type::ne, details::ne_op)           \
    CASE_STMT(core::operators::operator_type::equal, details::equal_op)     \
    CASE_STMT(core::operators::operator_type::logical_and, details::and_op) \
    CASE_STMT(core::operators::operator_type::nand, details::nand_op)       \
    CASE_STMT(core::operators::operator_type::logical_or, details::or_op)   \
    CASE_STMT(core::operators::operator_type::nor, details::nor_op)         \
    CASE_STMT(core::operators::operator_type::logical_xor, details::xor_op) \
    CASE_STMT(core::operators::operator_type::xnor, details::xnor_op)

            expression_node_ptr result = error_node();
            std::string node_name = "Unknown";

            if (is_b0_ivec && is_b1_ivec)
            {
                switch (operation)
                {
#define CASE_STMT(op0, op1)                                                                        \
    case op0:                                                                                      \
        result =                                                                                   \
            node_allocator_                                                                        \
                ->template allocate_rrr<typename details::vec_binop_vecvec_node<Type, op1<Type>>>( \
                    operation, branch[0], branch[1]);                                              \
        node_name = "vec_binop_vecvec_node";                                                       \
        break;

                    BATCH_EQINEQ_LOGIC_CASE
#undef CASE_STMT
                    default:
                        return error_node();
                }
            }
            else if (is_b0_ivec && !is_b1_ivec)
            {
                switch (operation)
                {
#define CASE_STMT(op0, op1)                                                                        \
    case op0:                                                                                      \
        result =                                                                                   \
            node_allocator_                                                                        \
                ->template allocate_rrr<typename details::vec_binop_vecval_node<Type, op1<Type>>>( \
                    operation, branch[0], branch[1]);                                              \
        node_name = "vec_binop_vecval_node";                                                       \
        break;

                    BATCH_EQINEQ_LOGIC_CASE
#undef CASE_STMT
                    default:
                        return error_node();
                }
            }
            else if (!is_b0_ivec && is_b1_ivec)
            {
                switch (operation)
                {
#define CASE_STMT(op0, op1)                                                                        \
    case op0:                                                                                      \
        result =                                                                                   \
            node_allocator_                                                                        \
                ->template allocate_rrr<typename details::vec_binop_valvec_node<Type, op1<Type>>>( \
                    operation, branch[0], branch[1]);                                              \
        node_name = "vec_binop_valvec_node";                                                       \
        break;

                    BATCH_EQINEQ_LOGIC_CASE
#undef CASE_STMT
                    default:
                        return error_node();
                }
            }
            else
                return error_node();

            if (result && result->valid())
            {
                return result;
            }

            parser_->set_error(parser_error::make_error(
                parser_error::error_mode::e_synthesis, token_t(),
                "ERR278 - Failed to synthesize node: " + node_name, core::error_location()));

            details::free_node(*node_allocator_, result);
            return error_node();

#undef BATCH_EQINEQ_LOGIC_CASE
        }

        inline expression_node_ptr synthesize_vecarithmetic_operation_expression(
            const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
        {
            const bool is_b0_ivec = details::is_ivector_node(branch[0]);
            const bool is_b1_ivec = details::is_ivector_node(branch[1]);

#define VECTOR_OPS                                                  \
    CASE_STMT(core::operators::operator_type::add, details::add_op) \
    CASE_STMT(core::operators::operator_type::sub, details::sub_op) \
    CASE_STMT(core::operators::operator_type::mul, details::mul_op) \
    CASE_STMT(core::operators::operator_type::div, details::div_op) \
    CASE_STMT(core::operators::operator_type::mod, details::mod_op)

            expression_node_ptr result = error_node();
            std::string node_name = "Unknown";

            if (is_b0_ivec && is_b1_ivec)
            {
                switch (operation)
                {
#define CASE_STMT(op0, op1)                                                                        \
    case op0:                                                                                      \
        result =                                                                                   \
            node_allocator_                                                                        \
                ->template allocate_rrr<typename details::vec_binop_vecvec_node<Type, op1<Type>>>( \
                    operation, branch[0], branch[1]);                                              \
        node_name = "vec_binop_vecvec_node";                                                       \
        break;

                    VECTOR_OPS CASE_STMT(core::operators::operator_type::pow, details::pow_op)
#undef CASE_STMT
                        default : return error_node();
                }
            }
            else if (is_b0_ivec && !is_b1_ivec)
            {
                switch (operation)
                {
#define CASE_STMT(op0, op1)                                                                        \
    case op0:                                                                                      \
        result =                                                                                   \
            node_allocator_                                                                        \
                ->template allocate_rrr<typename details::vec_binop_vecval_node<Type, op1<Type>>>( \
                    operation, branch[0], branch[1]);                                              \
        node_name = "vec_binop_vecval_node(b0ivec,!b1ivec)";                                       \
        break;

                    VECTOR_OPS CASE_STMT(core::operators::operator_type::pow, details::pow_op)
#undef CASE_STMT
                        default : return error_node();
                }
            }
            else if (!is_b0_ivec && is_b1_ivec)
            {
                switch (operation)
                {
#define CASE_STMT(op0, op1)                                                                        \
    case op0:                                                                                      \
        result =                                                                                   \
            node_allocator_                                                                        \
                ->template allocate_rrr<typename details::vec_binop_valvec_node<Type, op1<Type>>>( \
                    operation, branch[0], branch[1]);                                              \
        node_name = "vec_binop_vecval_node(!b0ivec,b1ivec)";                                       \
        break;

                    VECTOR_OPS
#undef CASE_STMT
                    default:
                        return error_node();
                }
            }
            else
                return error_node();

            if (result && result->valid())
            {
                return result;
            }

            parser_->set_error(parser_error::make_error(
                parser_error::error_mode::e_synthesis, token_t(),
                "ERR279 - Failed to synthesize node: " + node_name, core::error_location()));

            details::free_node(*node_allocator_, result);
            return error_node();

#undef VECTOR_OPS
        }

        inline expression_node_ptr synthesize_swap_expression(expression_node_ptr (&branch)[2])
        {
            const bool v0_is_ivar = details::is_ivariable_node(branch[0]);
            const bool v1_is_ivar = details::is_ivariable_node(branch[1]);

            const bool v0_is_ivec = details::is_ivector_node(branch[0]);
            const bool v1_is_ivec = details::is_ivector_node(branch[1]);

#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
            const bool v0_is_str = details::is_generally_string_node(branch[0]);
            const bool v1_is_str = details::is_generally_string_node(branch[1]);
#endif

            expression_node_ptr result = error_node();
            std::string node_name = "Unknown";

            if (v0_is_ivar && v1_is_ivar)
            {
                using variable_node_ptr = details::variable_node<T>*;

                variable_node_ptr v0 = variable_node_ptr(0);
                variable_node_ptr v1 = variable_node_ptr(0);

                if ((nullptr !=
                     (v0 = static_cast<variable_node_ptr>(branch[0]->as_variable_node()))) &&
                    (nullptr !=
                     (v1 = static_cast<variable_node_ptr>(branch[1]->as_variable_node()))))
                {
                    result = node_allocator_->allocate<details::swap_node<T>>(v0, v1);
                    node_name = "swap_node";
                }
                else
                {
                    result = node_allocator_->allocate<details::swap_generic_node<T>>(branch[0],
                                                                                      branch[1]);
                    node_name = "swap_generic_node";
                }
            }
            else if (v0_is_ivec && v1_is_ivec)
            {
                result =
                    node_allocator_->allocate<details::swap_vecvec_node<T>>(branch[0], branch[1]);
                node_name = "swap_vecvec_node";
            }
#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
            else if (v0_is_str && v1_is_str)
            {
                if (is_string_node(branch[0]) && is_string_node(branch[1]))
                {
                    result = node_allocator_->allocate<details::string_nodes::swap_string_node<T>>(
                        branch[0], branch[1]);
                    node_name = "swap_string_node";
                }
                else
                {
                    result =
                        node_allocator_->allocate<details::string_nodes::swap_genstrings_node<T>>(
                            branch[0], branch[1]);
                    node_name = "swap_genstrings_node";
                }
            }
#endif
            else
            {
                parser_->set_synthesis_error(
                    "Only variables, strings, vectors or vector elements can be swapped");
                return error_node();
            }

            if (result && result->valid())
            {
                parser_->state_.activate_side_effect("synthesize_swap_expression()");
                return result;
            }

            parser_->set_error(parser_error::make_error(
                parser_error::error_mode::e_synthesis, token_t(),
                "ERR280 - Failed to synthesize node: " + node_name, core::error_location()));

            details::free_node(*node_allocator_, result);
            return error_node();
        }

        inline expression_node_ptr synthesize_shortcircuit_expression(
            const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
        {
            if constexpr (::math_expr::core::build_options::kDisableScAndOr)
            {
                return error_node();
            }

            expression_node_ptr result = error_node();

            if (details::is_constant_node(branch[0]))
            {
                if ((core::operators::operator_type::scand == operation) &&
                    details::is_false(branch[0]))
                    result = node_allocator_->allocate_c<literal_node_t>(core::numeric::false_v<T>);
                else if ((core::operators::operator_type::scor == operation) &&
                         details::is_true(branch[0]))
                    result = node_allocator_->allocate_c<literal_node_t>(core::numeric::true_v<T>);
            }

            if (details::is_constant_node(branch[1]) && (nullptr == result))
            {
                if ((core::operators::operator_type::scand == operation) &&
                    details::is_false(branch[1]))
                    result = node_allocator_->allocate_c<literal_node_t>(core::numeric::false_v<T>);
                else if ((core::operators::operator_type::scor == operation) &&
                         details::is_true(branch[1]))
                    result = node_allocator_->allocate_c<literal_node_t>(core::numeric::true_v<T>);
            }

            if (result)
            {
                details::free_node(*node_allocator_, branch[0]);
                details::free_node(*node_allocator_, branch[1]);

                return result;
            }
            else if (core::operators::operator_type::scand == operation)
            {
                return synthesize_expression<scand_node_t, 2>(operation, branch);
            }
            else if (core::operators::operator_type::scor == operation)
            {
                return synthesize_expression<scor_node_t, 2>(operation, branch);
            }
            else
                return error_node();
        }

#define BASIC_OPR_SWITCH_STATEMENTS                                 \
    CASE_STMT(core::operators::operator_type::add, details::add_op) \
    CASE_STMT(core::operators::operator_type::sub, details::sub_op) \
    CASE_STMT(core::operators::operator_type::mul, details::mul_op) \
    CASE_STMT(core::operators::operator_type::div, details::div_op) \
    CASE_STMT(core::operators::operator_type::mod, details::mod_op) \
    CASE_STMT(core::operators::operator_type::pow, details::pow_op)

#define EXTENDED_OPR_SWITCH_STATEMENTS                                      \
    CASE_STMT(core::operators::operator_type::lt, details::lt_op)           \
    CASE_STMT(core::operators::operator_type::lte, details::lte_op)         \
    CASE_STMT(core::operators::operator_type::gt, details::gt_op)           \
    CASE_STMT(core::operators::operator_type::gte, details::gte_op)         \
    CASE_STMT(core::operators::operator_type::eq, details::eq_op)           \
    CASE_STMT(core::operators::operator_type::ne, details::ne_op)           \
    CASE_STMT(core::operators::operator_type::logical_and, details::and_op) \
    CASE_STMT(core::operators::operator_type::nand, details::nand_op)       \
    CASE_STMT(core::operators::operator_type::logical_or, details::or_op)   \
    CASE_STMT(core::operators::operator_type::nor, details::nor_op)         \
    CASE_STMT(core::operators::operator_type::logical_xor, details::xor_op) \
    CASE_STMT(core::operators::operator_type::xnor, details::xnor_op)

#ifndef MATH_EXPR_DISABLE_CARDINAL_POW_OPTIMISATION
        /**
         * @brief Synthesizes specialized power nodes for small integer exponents.
         *
         * The optimized nodes that are allocated here are instantiated with the
         * enclosing parser's numeric type `T`, which is the value type used
         * throughout the expression tree.
         *
         * @tparam TType Input value category passed to the power node factory.
         * @tparam IPowNode Power-node template instantiated with parser type `T`.
         */
        template <typename TType, template <typename, typename> class IPowNode>
        inline expression_node_ptr cardinal_pow_optimisation_impl(const TType& v,
                                                                  const unsigned int& p)
        {
            switch (p)
            {
#define CASE_STMT(cp) \
    case cp:          \
        return node_allocator_->allocate<IPowNode<T, core::numeric::fast_exp<T, cp>>>(v);

                CASE_STMT(1);
                CASE_STMT(2);
                CASE_STMT(3);
                CASE_STMT(4);
                CASE_STMT(5);
                CASE_STMT(6);
                CASE_STMT(7);
                CASE_STMT(8);
                CASE_STMT(9);
                CASE_STMT(10);
                CASE_STMT(11);
                CASE_STMT(12);
                CASE_STMT(13);
                CASE_STMT(14);
                CASE_STMT(15);
                CASE_STMT(16);
                CASE_STMT(17);
                CASE_STMT(18);
                CASE_STMT(19);
                CASE_STMT(20);
                CASE_STMT(21);
                CASE_STMT(22);
                CASE_STMT(23);
                CASE_STMT(24);
                CASE_STMT(25);
                CASE_STMT(26);
                CASE_STMT(27);
                CASE_STMT(28);
                CASE_STMT(29);
                CASE_STMT(30);
                CASE_STMT(31);
                CASE_STMT(32);
                CASE_STMT(33);
                CASE_STMT(34);
                CASE_STMT(35);
                CASE_STMT(36);
                CASE_STMT(37);
                CASE_STMT(38);
                CASE_STMT(39);
                CASE_STMT(40);
                CASE_STMT(41);
                CASE_STMT(42);
                CASE_STMT(43);
                CASE_STMT(44);
                CASE_STMT(45);
                CASE_STMT(46);
                CASE_STMT(47);
                CASE_STMT(48);
                CASE_STMT(49);
                CASE_STMT(50);
                CASE_STMT(51);
                CASE_STMT(52);
                CASE_STMT(53);
                CASE_STMT(54);
                CASE_STMT(55);
                CASE_STMT(56);
                CASE_STMT(57);
                CASE_STMT(58);
                CASE_STMT(59);
                CASE_STMT(60);
#undef CASE_STMT
                default:
                    return error_node();
            }
        }

        inline expression_node_ptr cardinal_pow_optimisation(const T& v, const T& c)
        {
            const bool not_recipricol = (c >= T(0));
            const unsigned int p =
                static_cast<unsigned int>(core::numeric::to_int32(core::numeric::abs(c)));

            if (0 == p)
                return node_allocator_->allocate_c<literal_node_t>(T(1));
            else if (std::equal_to<T>()(T(2), c))
            {
                return node_allocator_
                    ->template allocate_rr<typename details::vov_node<Type, details::mul_op<Type>>>(
                        v, v);
            }
            else
            {
                if (not_recipricol)
                    return cardinal_pow_optimisation_impl<T, details::ipow_node>(v, p);
                else
                    return cardinal_pow_optimisation_impl<T, details::ipowinv_node>(v, p);
            }
        }

        inline bool cardinal_pow_optimisable(const core::operators::operator_type& operation,
                                             const T& c) const
        {
            return (core::operators::operator_type::pow == operation) &&
                   (core::numeric::abs(c) <= T(60)) && core::numeric::is_integer(c);
        }

        inline expression_node_ptr cardinal_pow_optimisation(expression_node_ptr (&branch)[2])
        {
            const Type c = static_cast<details::literal_node<Type>*>(branch[1])->value();
            const bool not_recipricol = (c >= T(0));
            const unsigned int p =
                static_cast<unsigned int>(core::numeric::to_int32(core::numeric::abs(c)));

            node_allocator_->free(branch[1]);

            if (0 == p)
            {
                details::free_all_nodes(*node_allocator_, branch);

                return node_allocator_->allocate_c<literal_node_t>(T(1));
            }
            else if (not_recipricol)
                return cardinal_pow_optimisation_impl<expression_node_ptr, details::bipow_node>(
                    branch[0], p);
            else
                return cardinal_pow_optimisation_impl<expression_node_ptr, details::bipowinv_node>(
                    branch[0], p);
        }
#else
        inline expression_node_ptr cardinal_pow_optimisation(T&, const T&)
        {
            return error_node();
        }

        inline bool cardinal_pow_optimisable(const core::operators::operator_type&, const T&)
        {
            return false;
        }

        inline expression_node_ptr cardinal_pow_optimisation(expression_node_ptr (&)[2])
        {
            return error_node();
        }
#endif

        struct synthesize_binary_ext_expression
        {
            static inline expression_node_ptr process(
                expression_generator<Type>& expr_gen,
                const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
            {
                const bool left_neg = is_neg_unary_node(branch[0]);
                const bool right_neg = is_neg_unary_node(branch[1]);

                if (left_neg && right_neg)
                {
                    if ((core::operators::operator_type::add == operation) ||
                        (core::operators::operator_type::sub == operation) ||
                        (core::operators::operator_type::mul == operation) ||
                        (core::operators::operator_type::div == operation))
                    {
                        if (!expr_gen.parser_->simplify_unary_negation_branch(branch[0]) ||
                            !expr_gen.parser_->simplify_unary_negation_branch(branch[1]))
                        {
                            details::free_all_nodes(*expr_gen.node_allocator_, branch);

                            return error_node();
                        }
                    }

                    switch (operation)
                    {
                            // -f(x + 1) + -g(y + 1) --> -(f(x + 1) + g(y + 1))
                        case core::operators::operator_type::add:
                            return expr_gen(
                                core::operators::operator_type::neg,
                                expr_gen.node_allocator_->template allocate<
                                    typename details::binary_ext_node<Type, details::add_op<Type>>>(
                                    branch[0], branch[1]));

                            // -f(x + 1) - -g(y + 1) --> g(y + 1) - f(x + 1)
                        case core::operators::operator_type::sub:
                            return expr_gen.node_allocator_->template allocate<
                                typename details::binary_ext_node<Type, details::sub_op<Type>>>(
                                branch[1], branch[0]);

                        default:
                            break;
                    }
                }
                else if (left_neg && !right_neg)
                {
                    if ((core::operators::operator_type::add == operation) ||
                        (core::operators::operator_type::sub == operation) ||
                        (core::operators::operator_type::mul == operation) ||
                        (core::operators::operator_type::div == operation))
                    {
                        if (!expr_gen.parser_->simplify_unary_negation_branch(branch[0]))
                        {
                            details::free_all_nodes(*expr_gen.node_allocator_, branch);

                            return error_node();
                        }

                        switch (operation)
                        {
                                // -f(x + 1) + g(y + 1) --> g(y + 1) - f(x + 1)
                            case core::operators::operator_type::add:
                                return expr_gen.node_allocator_->template allocate<
                                    typename details::binary_ext_node<Type, details::sub_op<Type>>>(
                                    branch[1], branch[0]);

                                // -f(x + 1) - g(y + 1) --> -(f(x + 1) + g(y + 1))
                            case core::operators::operator_type::sub:
                                return expr_gen(
                                    core::operators::operator_type::neg,
                                    expr_gen.node_allocator_
                                        ->template allocate<typename details::binary_ext_node<
                                            Type, details::add_op<Type>>>(branch[0], branch[1]));

                                // -f(x + 1) * g(y + 1) --> -(f(x + 1) * g(y + 1))
                            case core::operators::operator_type::mul:
                                return expr_gen(
                                    core::operators::operator_type::neg,
                                    expr_gen.node_allocator_
                                        ->template allocate<typename details::binary_ext_node<
                                            Type, details::mul_op<Type>>>(branch[0], branch[1]));

                                // -f(x + 1) / g(y + 1) --> -(f(x + 1) / g(y + 1))
                            case core::operators::operator_type::div:
                                return expr_gen(
                                    core::operators::operator_type::neg,
                                    expr_gen.node_allocator_
                                        ->template allocate<typename details::binary_ext_node<
                                            Type, details::div_op<Type>>>(branch[0], branch[1]));

                            default:
                                return error_node();
                        }
                    }
                }
                else if (!left_neg && right_neg)
                {
                    if ((core::operators::operator_type::add == operation) ||
                        (core::operators::operator_type::sub == operation) ||
                        (core::operators::operator_type::mul == operation) ||
                        (core::operators::operator_type::div == operation))
                    {
                        if (!expr_gen.parser_->simplify_unary_negation_branch(branch[1]))
                        {
                            details::free_all_nodes(*expr_gen.node_allocator_, branch);

                            return error_node();
                        }

                        switch (operation)
                        {
                                // f(x + 1) + -g(y + 1) --> f(x + 1) - g(y + 1)
                            case core::operators::operator_type::add:
                                return expr_gen.node_allocator_->template allocate<
                                    typename details::binary_ext_node<Type, details::sub_op<Type>>>(
                                    branch[0], branch[1]);

                                // f(x + 1) - - g(y + 1) --> f(x + 1) + g(y + 1)
                            case core::operators::operator_type::sub:
                                return expr_gen.node_allocator_->template allocate<
                                    typename details::binary_ext_node<Type, details::add_op<Type>>>(
                                    branch[0], branch[1]);

                                // f(x + 1) * -g(y + 1) --> -(f(x + 1) * g(y + 1))
                            case core::operators::operator_type::mul:
                                return expr_gen(
                                    core::operators::operator_type::neg,
                                    expr_gen.node_allocator_
                                        ->template allocate<typename details::binary_ext_node<
                                            Type, details::mul_op<Type>>>(branch[0], branch[1]));

                                // f(x + 1) / -g(y + 1) --> -(f(x + 1) / g(y + 1))
                            case core::operators::operator_type::div:
                                return expr_gen(
                                    core::operators::operator_type::neg,
                                    expr_gen.node_allocator_
                                        ->template allocate<typename details::binary_ext_node<
                                            Type, details::div_op<Type>>>(branch[0], branch[1]));

                            default:
                                return error_node();
                        }
                    }
                }

                switch (operation)
                {
#define CASE_STMT(op0, op1)                                                                    \
    case op0:                                                                                  \
        return expr_gen.node_allocator_                                                        \
            ->template allocate<typename details::binary_ext_node<Type, op1<Type>>>(branch[0], \
                                                                                    branch[1]);

                    BASIC_OPR_SWITCH_STATEMENTS EXTENDED_OPR_SWITCH_STATEMENTS
#undef CASE_STMT
                        default : return error_node();
                }
            }
        };

        struct synthesize_vob_expression
        {
            static inline expression_node_ptr process(
                expression_generator<Type>& expr_gen,
                const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
            {
                const Type& v = static_cast<details::variable_node<Type>*>(branch[0])->ref();

#ifndef MATH_EXPR_DISABLE_ENHANCED_FEATURES
                if (details::is_sf3ext_node(branch[1]))
                {
                    expression_node_ptr result = error_node();

                    const bool synthesis_result =
                        synthesize_sf4ext_expression::template compile_right<vtype>(
                            expr_gen, v, operation, branch[1], result);

                    if (synthesis_result)
                    {
                        details::free_node(*expr_gen.node_allocator_, branch[1]);
                        return result;
                    }
                }
#endif

                if ((core::operators::operator_type::mul == operation) ||
                    (core::operators::operator_type::div == operation))
                {
                    if (details::is_uv_node(branch[1]))
                    {
                        using uvbn_ptr_t = details::uv_base_node<Type>*;

                        core::operators::operator_type o =
                            static_cast<uvbn_ptr_t>(branch[1])->operation();

                        if (core::operators::operator_type::neg == o)
                        {
                            const Type& v1 = static_cast<uvbn_ptr_t>(branch[1])->v();

                            details::free_node(*expr_gen.node_allocator_, branch[1]);

                            switch (operation)
                            {
                                case core::operators::operator_type::mul:
                                    return expr_gen(
                                        core::operators::operator_type::neg,
                                        expr_gen.node_allocator_
                                            ->template allocate_rr<typename details::vov_node<
                                                Type, details::mul_op<Type>>>(v, v1));

                                case core::operators::operator_type::div:
                                    return expr_gen(
                                        core::operators::operator_type::neg,
                                        expr_gen.node_allocator_
                                            ->template allocate_rr<typename details::vov_node<
                                                Type, details::div_op<Type>>>(v, v1));

                                default:
                                    break;
                            }
                        }
                    }
                }

                switch (operation)
                {
#define CASE_STMT(op0, op1)             \
    case op0:                           \
        return expr_gen.node_allocator_ \
            ->template allocate_rc<typename details::vob_node<Type, op1<Type>>>(v, branch[1]);

                    BASIC_OPR_SWITCH_STATEMENTS EXTENDED_OPR_SWITCH_STATEMENTS
#undef CASE_STMT
                        default : return error_node();
                }
            }
        };

        struct synthesize_bov_expression
        {
            static inline expression_node_ptr process(
                expression_generator<Type>& expr_gen,
                const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
            {
                const Type& v = static_cast<details::variable_node<Type>*>(branch[1])->ref();

#ifndef MATH_EXPR_DISABLE_ENHANCED_FEATURES
                if (details::is_sf3ext_node(branch[0]))
                {
                    expression_node_ptr result = error_node();

                    const bool synthesis_result =
                        synthesize_sf4ext_expression::template compile_left<vtype>(
                            expr_gen, v, operation, branch[0], result);

                    if (synthesis_result)
                    {
                        details::free_node(*expr_gen.node_allocator_, branch[0]);

                        return result;
                    }
                }
#endif

                if ((core::operators::operator_type::add == operation) ||
                    (core::operators::operator_type::sub == operation) ||
                    (core::operators::operator_type::mul == operation) ||
                    (core::operators::operator_type::div == operation))
                {
                    if (details::is_uv_node(branch[0]))
                    {
                        using uvbn_ptr_t = details::uv_base_node<Type>*;

                        core::operators::operator_type o =
                            static_cast<uvbn_ptr_t>(branch[0])->operation();

                        if (core::operators::operator_type::neg == o)
                        {
                            const Type& v0 = static_cast<uvbn_ptr_t>(branch[0])->v();

                            details::free_node(*expr_gen.node_allocator_, branch[0]);

                            switch (operation)
                            {
                                case core::operators::operator_type::add:
                                    return expr_gen.node_allocator_->template allocate_rr<
                                        typename details::vov_node<Type, details::sub_op<Type>>>(
                                        v, v0);

                                case core::operators::operator_type::sub:
                                    return expr_gen(
                                        core::operators::operator_type::neg,
                                        expr_gen.node_allocator_
                                            ->template allocate_rr<typename details::vov_node<
                                                Type, details::add_op<Type>>>(v0, v));

                                case core::operators::operator_type::mul:
                                    return expr_gen(
                                        core::operators::operator_type::neg,
                                        expr_gen.node_allocator_
                                            ->template allocate_rr<typename details::vov_node<
                                                Type, details::mul_op<Type>>>(v0, v));

                                case core::operators::operator_type::div:
                                    return expr_gen(
                                        core::operators::operator_type::neg,
                                        expr_gen.node_allocator_
                                            ->template allocate_rr<typename details::vov_node<
                                                Type, details::div_op<Type>>>(v0, v));
                                default:
                                    break;
                            }
                        }
                    }
                }

                switch (operation)
                {
#define CASE_STMT(op0, op1)             \
    case op0:                           \
        return expr_gen.node_allocator_ \
            ->template allocate_cr<typename details::bov_node<Type, op1<Type>>>(branch[0], v);

                    BASIC_OPR_SWITCH_STATEMENTS EXTENDED_OPR_SWITCH_STATEMENTS
#undef CASE_STMT
                        default : return error_node();
                }
            }
        };

        struct synthesize_cob_expression
        {
            static inline expression_node_ptr process(
                expression_generator<Type>& expr_gen,
                const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
            {
                const Type c = static_cast<details::literal_node<Type>*>(branch[0])->value();

                details::free_node(*expr_gen.node_allocator_, branch[0]);

                if (std::equal_to<T>()(T(0), c) &&
                    (core::operators::operator_type::mul == operation))
                {
                    details::free_node(*expr_gen.node_allocator_, branch[1]);

                    return expr_gen(T(0));
                }
                else if (std::equal_to<T>()(T(0), c) &&
                         (core::operators::operator_type::div == operation))
                {
                    details::free_node(*expr_gen.node_allocator_, branch[1]);

                    return expr_gen(T(0));
                }
                else if (std::equal_to<T>()(T(0), c) &&
                         (core::operators::operator_type::add == operation))
                    return branch[1];
                else if (std::equal_to<T>()(T(1), c) &&
                         (core::operators::operator_type::mul == operation))
                    return branch[1];

                if (details::is_cob_node(branch[1]))
                {
                    // Simplify expressions of the form:
                    // 1. (1 * (2 * (3 * (4 * (5 * (6 * (7 * (8 * (9 + x))))))))) --> 40320 * (9 +
                    // x)
                    // 2. (1 + (2 + (3 + (4 + (5 + (6 + (7 + (8 + (9 + x))))))))) --> 45 + x
                    if ((core::operators::operator_type::mul == operation) ||
                        (core::operators::operator_type::add == operation))
                    {
                        details::cob_base_node<Type>* cobnode =
                            static_cast<details::cob_base_node<Type>*>(branch[1]);

                        if (operation == cobnode->operation())
                        {
                            switch (operation)
                            {
                                case core::operators::operator_type::add:
                                    cobnode->set_c(c + cobnode->c());
                                    break;
                                case core::operators::operator_type::mul:
                                    cobnode->set_c(c * cobnode->c());
                                    break;
                                default:
                                    return error_node();
                            }

                            return cobnode;
                        }
                    }

                    if (operation == core::operators::operator_type::mul)
                    {
                        details::cob_base_node<Type>* cobnode =
                            static_cast<details::cob_base_node<Type>*>(branch[1]);
                        core::operators::operator_type cob_opr = cobnode->operation();

                        if ((core::operators::operator_type::div == cob_opr) ||
                            (core::operators::operator_type::mul == cob_opr))
                        {
                            switch (cob_opr)
                            {
                                case core::operators::operator_type::div:
                                    cobnode->set_c(c * cobnode->c());
                                    break;
                                case core::operators::operator_type::mul:
                                    cobnode->set_c(cobnode->c() / c);
                                    break;
                                default:
                                    return error_node();
                            }

                            return cobnode;
                        }
                    }
                    else if (operation == core::operators::operator_type::div)
                    {
                        details::cob_base_node<Type>* cobnode =
                            static_cast<details::cob_base_node<Type>*>(branch[1]);
                        core::operators::operator_type cob_opr = cobnode->operation();

                        if ((core::operators::operator_type::div == cob_opr) ||
                            (core::operators::operator_type::mul == cob_opr))
                        {
                            details::expression_node<Type>* new_cobnode = error_node();

                            switch (cob_opr)
                            {
                                case core::operators::operator_type::div:
                                    new_cobnode = expr_gen.node_allocator_->template allocate_tt<
                                        typename details::cob_node<Type, details::mul_op<Type>>>(
                                        c / cobnode->c(), cobnode->move_branch(0));
                                    break;

                                case core::operators::operator_type::mul:
                                    new_cobnode = expr_gen.node_allocator_->template allocate_tt<
                                        typename details::cob_node<Type, details::div_op<Type>>>(
                                        c / cobnode->c(), cobnode->move_branch(0));
                                    break;

                                default:
                                    return error_node();
                            }

                            details::free_node(*expr_gen.node_allocator_, branch[1]);

                            return new_cobnode;
                        }
                    }
                }
#ifndef MATH_EXPR_DISABLE_ENHANCED_FEATURES
                else if (details::is_sf3ext_node(branch[1]))
                {
                    expression_node_ptr result = error_node();

                    const bool synthesis_result =
                        synthesize_sf4ext_expression::template compile_right<ctype>(
                            expr_gen, c, operation, branch[1], result);

                    if (synthesis_result)
                    {
                        details::free_node(*expr_gen.node_allocator_, branch[1]);

                        return result;
                    }
                }
#endif

                switch (operation)
                {
#define CASE_STMT(op0, op1)             \
    case op0:                           \
        return expr_gen.node_allocator_ \
            ->template allocate_tt<typename details::cob_node<Type, op1<Type>>>(c, branch[1]);

                    BASIC_OPR_SWITCH_STATEMENTS EXTENDED_OPR_SWITCH_STATEMENTS
#undef CASE_STMT
                        default : return error_node();
                }
            }
        };

        struct synthesize_boc_expression
        {
            static inline expression_node_ptr process(
                expression_generator<Type>& expr_gen,
                const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
            {
                const Type c = static_cast<details::literal_node<Type>*>(branch[1])->value();

                details::free_node(*(expr_gen.node_allocator_), branch[1]);

                if (std::equal_to<T>()(T(0), c) &&
                    (core::operators::operator_type::mul == operation))
                {
                    details::free_node(*expr_gen.node_allocator_, branch[0]);

                    return expr_gen(T(0));
                }
                else if (std::equal_to<T>()(T(0), c) &&
                         (core::operators::operator_type::div == operation))
                {
                    details::free_node(*expr_gen.node_allocator_, branch[0]);

                    return expr_gen(std::numeric_limits<T>::quiet_NaN());
                }
                else if (std::equal_to<T>()(T(0), c) &&
                         (core::operators::operator_type::add == operation))
                    return branch[0];
                else if (std::equal_to<T>()(T(1), c) &&
                         (core::operators::operator_type::mul == operation))
                    return branch[0];

                if (details::is_boc_node(branch[0]))
                {
                    // Simplify expressions of the form:
                    // 1. (((((((((x + 9) * 8) * 7) * 6) * 5) * 4) * 3) * 2) * 1) --> (x + 9) *
                    // 40320
                    // 2. (((((((((x + 9) + 8) + 7) + 6) + 5) + 4) + 3) + 2) + 1) --> x + 45
                    if ((core::operators::operator_type::mul == operation) ||
                        (core::operators::operator_type::add == operation))
                    {
                        details::boc_base_node<Type>* bocnode =
                            static_cast<details::boc_base_node<Type>*>(branch[0]);

                        if (operation == bocnode->operation())
                        {
                            switch (operation)
                            {
                                case core::operators::operator_type::add:
                                    bocnode->set_c(c + bocnode->c());
                                    break;
                                case core::operators::operator_type::mul:
                                    bocnode->set_c(c * bocnode->c());
                                    break;
                                default:
                                    return error_node();
                            }

                            return bocnode;
                        }
                    }
                    else if (operation == core::operators::operator_type::div)
                    {
                        details::boc_base_node<Type>* bocnode =
                            static_cast<details::boc_base_node<Type>*>(branch[0]);
                        core::operators::operator_type boc_opr = bocnode->operation();

                        if ((core::operators::operator_type::div == boc_opr) ||
                            (core::operators::operator_type::mul == boc_opr))
                        {
                            switch (boc_opr)
                            {
                                case core::operators::operator_type::div:
                                    bocnode->set_c(c * bocnode->c());
                                    break;
                                case core::operators::operator_type::mul:
                                    bocnode->set_c(bocnode->c() / c);
                                    break;
                                default:
                                    return error_node();
                            }

                            return bocnode;
                        }
                    }
                    else if (operation == core::operators::operator_type::pow)
                    {
                        // (v ^ c0) ^ c1 --> v ^(c0 * c1)
                        details::boc_base_node<Type>* bocnode =
                            static_cast<details::boc_base_node<Type>*>(branch[0]);
                        core::operators::operator_type boc_opr = bocnode->operation();

                        if (core::operators::operator_type::pow == boc_opr)
                        {
                            bocnode->set_c(bocnode->c() * c);

                            return bocnode;
                        }
                    }
                }

#ifndef MATH_EXPR_DISABLE_ENHANCED_FEATURES
                if (details::is_sf3ext_node(branch[0]))
                {
                    expression_node_ptr result = error_node();

                    const bool synthesis_result =
                        synthesize_sf4ext_expression::template compile_left<ctype>(
                            expr_gen, c, operation, branch[0], result);

                    if (synthesis_result)
                    {
                        free_node(*expr_gen.node_allocator_, branch[0]);

                        return result;
                    }
                }
#endif

                switch (operation)
                {
#define CASE_STMT(op0, op1)             \
    case op0:                           \
        return expr_gen.node_allocator_ \
            ->template allocate_cr<typename details::boc_node<Type, op1<Type>>>(branch[0], c);

                    BASIC_OPR_SWITCH_STATEMENTS EXTENDED_OPR_SWITCH_STATEMENTS
#undef CASE_STMT
                        default : return error_node();
                }
            }
        };

        struct synthesize_cocob_expression
        {
            static inline expression_node_ptr process(
                expression_generator<Type>& expr_gen,
                const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
            {
                expression_node_ptr result = error_node();

                // (cob) o c --> cob
                if (details::is_cob_node(branch[0]))
                {
                    details::cob_base_node<Type>* cobnode =
                        static_cast<details::cob_base_node<Type>*>(branch[0]);

                    const Type c = static_cast<details::literal_node<Type>*>(branch[1])->value();

                    if (std::equal_to<T>()(T(0), c) &&
                        (core::operators::operator_type::mul == operation))
                    {
                        details::free_node(*expr_gen.node_allocator_, branch[0]);
                        details::free_node(*expr_gen.node_allocator_, branch[1]);

                        return expr_gen(T(0));
                    }
                    else if (std::equal_to<T>()(T(0), c) &&
                             (core::operators::operator_type::div == operation))
                    {
                        details::free_node(*expr_gen.node_allocator_, branch[0]);
                        details::free_node(*expr_gen.node_allocator_, branch[1]);

                        return expr_gen(T(std::numeric_limits<T>::quiet_NaN()));
                    }
                    else if (std::equal_to<T>()(T(0), c) &&
                             (core::operators::operator_type::add == operation))
                    {
                        details::free_node(*expr_gen.node_allocator_, branch[1]);

                        return branch[0];
                    }
                    else if (std::equal_to<T>()(T(1), c) &&
                             (core::operators::operator_type::mul == operation))
                    {
                        details::free_node(*expr_gen.node_allocator_, branch[1]);

                        return branch[0];
                    }
                    else if (std::equal_to<T>()(T(1), c) &&
                             (core::operators::operator_type::div == operation))
                    {
                        details::free_node(*expr_gen.node_allocator_, branch[1]);

                        return branch[0];
                    }

                    const bool op_addsub =
                        (core::operators::operator_type::add == cobnode->operation()) ||
                        (core::operators::operator_type::sub == cobnode->operation());

                    if (op_addsub)
                    {
                        switch (operation)
                        {
                            case core::operators::operator_type::add:
                                cobnode->set_c(cobnode->c() + c);
                                break;
                            case core::operators::operator_type::sub:
                                cobnode->set_c(cobnode->c() - c);
                                break;
                            default:
                                return error_node();
                        }

                        result = cobnode;
                    }
                    else if (core::operators::operator_type::mul == cobnode->operation())
                    {
                        switch (operation)
                        {
                            case core::operators::operator_type::mul:
                                cobnode->set_c(cobnode->c() * c);
                                break;
                            case core::operators::operator_type::div:
                                cobnode->set_c(cobnode->c() / c);
                                break;
                            default:
                                return error_node();
                        }

                        result = cobnode;
                    }
                    else if (core::operators::operator_type::div == cobnode->operation())
                    {
                        if (core::operators::operator_type::mul == operation)
                        {
                            cobnode->set_c(cobnode->c() * c);
                            result = cobnode;
                        }
                        else if (core::operators::operator_type::div == operation)
                        {
                            result = expr_gen.node_allocator_->template allocate_tt<
                                typename details::cob_node<Type, details::div_op<Type>>>(
                                cobnode->c() / c, cobnode->move_branch(0));

                            details::free_node(*expr_gen.node_allocator_, branch[0]);
                        }
                    }

                    if (result)
                    {
                        details::free_node(*expr_gen.node_allocator_, branch[1]);
                    }
                }

                // c o (cob) --> cob
                else if (details::is_cob_node(branch[1]))
                {
                    details::cob_base_node<Type>* cobnode =
                        static_cast<details::cob_base_node<Type>*>(branch[1]);

                    const Type c = static_cast<details::literal_node<Type>*>(branch[0])->value();

                    if (std::equal_to<T>()(T(0), c) &&
                        (core::operators::operator_type::mul == operation))
                    {
                        details::free_node(*expr_gen.node_allocator_, branch[0]);
                        details::free_node(*expr_gen.node_allocator_, branch[1]);

                        return expr_gen(T(0));
                    }
                    else if (std::equal_to<T>()(T(0), c) &&
                             (core::operators::operator_type::div == operation))
                    {
                        details::free_node(*expr_gen.node_allocator_, branch[0]);
                        details::free_node(*expr_gen.node_allocator_, branch[1]);

                        return expr_gen(T(0));
                    }
                    else if (std::equal_to<T>()(T(0), c) &&
                             (core::operators::operator_type::add == operation))
                    {
                        details::free_node(*expr_gen.node_allocator_, branch[0]);

                        return branch[1];
                    }
                    else if (std::equal_to<T>()(T(1), c) &&
                             (core::operators::operator_type::mul == operation))
                    {
                        details::free_node(*expr_gen.node_allocator_, branch[0]);

                        return branch[1];
                    }

                    if (core::operators::operator_type::add == cobnode->operation())
                    {
                        if (core::operators::operator_type::add == operation)
                        {
                            cobnode->set_c(c + cobnode->c());
                            result = cobnode;
                        }
                        else if (core::operators::operator_type::sub == operation)
                        {
                            result = expr_gen.node_allocator_->template allocate_tt<
                                typename details::cob_node<Type, details::sub_op<Type>>>(
                                c - cobnode->c(), cobnode->move_branch(0));

                            details::free_node(*expr_gen.node_allocator_, branch[1]);
                        }
                    }
                    else if (core::operators::operator_type::sub == cobnode->operation())
                    {
                        if (core::operators::operator_type::add == operation)
                        {
                            cobnode->set_c(c + cobnode->c());
                            result = cobnode;
                        }
                        else if (core::operators::operator_type::sub == operation)
                        {
                            result = expr_gen.node_allocator_->template allocate_tt<
                                typename details::cob_node<Type, details::add_op<Type>>>(
                                c - cobnode->c(), cobnode->move_branch(0));

                            details::free_node(*expr_gen.node_allocator_, branch[1]);
                        }
                    }
                    else if (core::operators::operator_type::mul == cobnode->operation())
                    {
                        if (core::operators::operator_type::mul == operation)
                        {
                            cobnode->set_c(c * cobnode->c());
                            result = cobnode;
                        }
                        else if (core::operators::operator_type::div == operation)
                        {
                            result = expr_gen.node_allocator_->template allocate_tt<
                                typename details::cob_node<Type, details::div_op<Type>>>(
                                c / cobnode->c(), cobnode->move_branch(0));

                            details::free_node(*expr_gen.node_allocator_, branch[1]);
                        }
                    }
                    else if (core::operators::operator_type::div == cobnode->operation())
                    {
                        if (core::operators::operator_type::mul == operation)
                        {
                            cobnode->set_c(c * cobnode->c());
                            result = cobnode;
                        }
                        else if (core::operators::operator_type::div == operation)
                        {
                            result = expr_gen.node_allocator_->template allocate_tt<
                                typename details::cob_node<Type, details::mul_op<Type>>>(
                                c / cobnode->c(), cobnode->move_branch(0));

                            details::free_node(*expr_gen.node_allocator_, branch[1]);
                        }
                    }

                    if (result)
                    {
                        details::free_node(*expr_gen.node_allocator_, branch[0]);
                    }
                }

                return result;
            }
        };

        struct synthesize_coboc_expression
        {
            static inline expression_node_ptr process(
                expression_generator<Type>& expr_gen,
                const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
            {
                expression_node_ptr result = error_node();

                // (boc) o c --> boc
                if (details::is_boc_node(branch[0]))
                {
                    details::boc_base_node<Type>* bocnode =
                        static_cast<details::boc_base_node<Type>*>(branch[0]);

                    const Type c = static_cast<details::literal_node<Type>*>(branch[1])->value();

                    if (core::operators::operator_type::add == bocnode->operation())
                    {
                        switch (operation)
                        {
                            case core::operators::operator_type::add:
                                bocnode->set_c(bocnode->c() + c);
                                break;
                            case core::operators::operator_type::sub:
                                bocnode->set_c(bocnode->c() - c);
                                break;
                            default:
                                return error_node();
                        }

                        result = bocnode;
                    }
                    else if (core::operators::operator_type::mul == bocnode->operation())
                    {
                        switch (operation)
                        {
                            case core::operators::operator_type::mul:
                                bocnode->set_c(bocnode->c() * c);
                                break;
                            case core::operators::operator_type::div:
                                bocnode->set_c(bocnode->c() / c);
                                break;
                            default:
                                return error_node();
                        }

                        result = bocnode;
                    }
                    else if (core::operators::operator_type::sub == bocnode->operation())
                    {
                        if (core::operators::operator_type::add == operation)
                        {
                            result = expr_gen.node_allocator_->template allocate_tt<
                                typename details::boc_node<Type, details::add_op<Type>>>(
                                bocnode->move_branch(0), c - bocnode->c());

                            details::free_node(*expr_gen.node_allocator_, branch[0]);
                        }
                        else if (core::operators::operator_type::sub == operation)
                        {
                            bocnode->set_c(bocnode->c() + c);
                            result = bocnode;
                        }
                    }
                    else if (core::operators::operator_type::div == bocnode->operation())
                    {
                        switch (operation)
                        {
                            case core::operators::operator_type::div:
                                bocnode->set_c(bocnode->c() * c);
                                break;
                            case core::operators::operator_type::mul:
                                bocnode->set_c(bocnode->c() / c);
                                break;
                            default:
                                return error_node();
                        }

                        result = bocnode;
                    }

                    if (result)
                    {
                        details::free_node(*expr_gen.node_allocator_, branch[1]);
                    }
                }

                // c o (boc) --> boc
                else if (details::is_boc_node(branch[1]))
                {
                    details::boc_base_node<Type>* bocnode =
                        static_cast<details::boc_base_node<Type>*>(branch[1]);

                    const Type c = static_cast<details::literal_node<Type>*>(branch[0])->value();

                    if (core::operators::operator_type::add == bocnode->operation())
                    {
                        if (core::operators::operator_type::add == operation)
                        {
                            bocnode->set_c(c + bocnode->c());
                            result = bocnode;
                        }
                        else if (core::operators::operator_type::sub == operation)
                        {
                            result = expr_gen.node_allocator_->template allocate_tt<
                                typename details::cob_node<Type, details::sub_op<Type>>>(
                                c - bocnode->c(), bocnode->move_branch(0));

                            details::free_node(*expr_gen.node_allocator_, branch[1]);
                        }
                    }
                    else if (core::operators::operator_type::sub == bocnode->operation())
                    {
                        if (core::operators::operator_type::add == operation)
                        {
                            result = expr_gen.node_allocator_->template allocate_tt<
                                typename details::boc_node<Type, details::add_op<Type>>>(
                                bocnode->move_branch(0), c - bocnode->c());

                            details::free_node(*expr_gen.node_allocator_, branch[1]);
                        }
                        else if (core::operators::operator_type::sub == operation)
                        {
                            result = expr_gen.node_allocator_->template allocate_tt<
                                typename details::cob_node<Type, details::sub_op<Type>>>(
                                c + bocnode->c(), bocnode->move_branch(0));

                            details::free_node(*expr_gen.node_allocator_, branch[1]);
                        }
                    }
                    else if (core::operators::operator_type::mul == bocnode->operation())
                    {
                        if (core::operators::operator_type::mul == operation)
                        {
                            bocnode->set_c(c * bocnode->c());
                            result = bocnode;
                        }
                        else if (core::operators::operator_type::div == operation)
                        {
                            result = expr_gen.node_allocator_->template allocate_tt<
                                typename details::cob_node<Type, details::div_op<Type>>>(
                                c / bocnode->c(), bocnode->move_branch(0));

                            details::free_node(*expr_gen.node_allocator_, branch[1]);
                        }
                    }
                    else if (core::operators::operator_type::div == bocnode->operation())
                    {
                        if (core::operators::operator_type::mul == operation)
                        {
                            bocnode->set_c(bocnode->c() / c);
                            result = bocnode;
                        }
                        else if (core::operators::operator_type::div == operation)
                        {
                            result = expr_gen.node_allocator_->template allocate_tt<
                                typename details::cob_node<Type, details::div_op<Type>>>(
                                c * bocnode->c(), bocnode->move_branch(0));

                            details::free_node(*expr_gen.node_allocator_, branch[1]);
                        }
                    }

                    if (result)
                    {
                        details::free_node(*expr_gen.node_allocator_, branch[0]);
                    }
                }

                return result;
            }
        };

#ifndef MATH_EXPR_DISABLE_ENHANCED_FEATURES
        inline bool synthesize_expression(const core::operators::operator_type& operation,
                                          expression_node_ptr (&branch)[2],
                                          expression_node_ptr& result)
        {
            result = error_node();

            if (!operation_optimisable(operation))
                return false;

            const std::string node_id = branch_to_id(branch);

            const typename synthesize_map_t::iterator itr = synthesize_map_.find(node_id);

            if (synthesize_map_.end() != itr)
            {
                result = itr->second((*this), operation, branch);

                return true;
            }
            else
                return false;
        }

        struct synthesize_vov_expression
        {
            static inline expression_node_ptr process(
                expression_generator<Type>& expr_gen,
                const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
            {
                const Type& v1 = static_cast<details::variable_node<Type>*>(branch[0])->ref();
                const Type& v2 = static_cast<details::variable_node<Type>*>(branch[1])->ref();

                switch (operation)
                {
#define CASE_STMT(op0, op1)             \
    case op0:                           \
        return expr_gen.node_allocator_ \
            ->template allocate_rr<typename details::vov_node<Type, op1<Type>>>(v1, v2);

                    BASIC_OPR_SWITCH_STATEMENTS EXTENDED_OPR_SWITCH_STATEMENTS
#undef CASE_STMT
                        default : return error_node();
                }
            }
        };

        struct synthesize_cov_expression
        {
            static inline expression_node_ptr process(
                expression_generator<Type>& expr_gen,
                const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
            {
                const Type c = static_cast<details::literal_node<Type>*>(branch[0])->value();
                const Type& v = static_cast<details::variable_node<Type>*>(branch[1])->ref();

                details::free_node(*(expr_gen.node_allocator_), branch[0]);

                if (std::equal_to<T>()(T(0), c) &&
                    (core::operators::operator_type::mul == operation))
                    return expr_gen(T(0));
                else if (std::equal_to<T>()(T(0), c) &&
                         (core::operators::operator_type::div == operation))
                    return expr_gen(T(0));
                else if (std::equal_to<T>()(T(0), c) &&
                         (core::operators::operator_type::add == operation))
                    return static_cast<details::variable_node<Type>*>(branch[1]);
                else if (std::equal_to<T>()(T(1), c) &&
                         (core::operators::operator_type::mul == operation))
                    return static_cast<details::variable_node<Type>*>(branch[1]);

                switch (operation)
                {
#define CASE_STMT(op0, op1)             \
    case op0:                           \
        return expr_gen.node_allocator_ \
            ->template allocate_cr<typename details::cov_node<Type, op1<Type>>>(c, v);

                    BASIC_OPR_SWITCH_STATEMENTS EXTENDED_OPR_SWITCH_STATEMENTS
#undef CASE_STMT
                        default : return error_node();
                }
            }
        };

        struct synthesize_voc_expression
        {
            static inline expression_node_ptr process(
                expression_generator<Type>& expr_gen,
                const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
            {
                const Type& v = static_cast<details::variable_node<Type>*>(branch[0])->ref();
                const Type c = static_cast<details::literal_node<Type>*>(branch[1])->value();

                details::free_node(*(expr_gen.node_allocator_), branch[1]);

                if (expr_gen.cardinal_pow_optimisable(operation, c))
                {
                    if (std::equal_to<T>()(T(1), c))
                        return branch[0];
                    else
                        return expr_gen.cardinal_pow_optimisation(v, c);
                }
                else if (std::equal_to<T>()(T(0), c) &&
                         (core::operators::operator_type::mul == operation))
                    return expr_gen(T(0));
                else if (std::equal_to<T>()(T(0), c) &&
                         (core::operators::operator_type::div == operation))
                    return expr_gen(std::numeric_limits<T>::quiet_NaN());
                else if (std::equal_to<T>()(T(0), c) &&
                         (core::operators::operator_type::add == operation))
                    return static_cast<details::variable_node<Type>*>(branch[0]);
                else if (std::equal_to<T>()(T(1), c) &&
                         (core::operators::operator_type::mul == operation))
                    return static_cast<details::variable_node<Type>*>(branch[0]);
                else if (std::equal_to<T>()(T(1), c) &&
                         (core::operators::operator_type::div == operation))
                    return static_cast<details::variable_node<Type>*>(branch[0]);

                switch (operation)
                {
#define CASE_STMT(op0, op1)             \
    case op0:                           \
        return expr_gen.node_allocator_ \
            ->template allocate_rc<typename details::voc_node<Type, op1<Type>>>(v, c);

                    BASIC_OPR_SWITCH_STATEMENTS EXTENDED_OPR_SWITCH_STATEMENTS
#undef CASE_STMT
                        default : return error_node();
                }
            }
        };

        struct synthesize_sf3ext_expression
        {
            template <typename T0, typename T1, typename T2>
            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const core::operators::operator_type& sf3opr,
                                                      T0 t0, T1 t1, T2 t2)
            {
                switch (sf3opr)
                {
#define CASE_STMT(op)                                                                         \
    case core::operators::operator_type::sf##op:                                              \
        return details::T0oT1oT2_sf3ext<T, T0, T1, T2, details::sf##op##_op<Type>>::allocate( \
            *(expr_gen.node_allocator_), t0, t1, t2);

                    CASE_STMT(00);
                    CASE_STMT(01);
                    CASE_STMT(02);
                    CASE_STMT(03);
                    CASE_STMT(04);
                    CASE_STMT(05);
                    CASE_STMT(06);
                    CASE_STMT(07);
                    CASE_STMT(08);
                    CASE_STMT(09);
                    CASE_STMT(10);
                    CASE_STMT(11);
                    CASE_STMT(12);
                    CASE_STMT(13);
                    CASE_STMT(14);
                    CASE_STMT(15);
                    CASE_STMT(16);
                    CASE_STMT(17);
                    CASE_STMT(18);
                    CASE_STMT(19);
                    CASE_STMT(20);
                    CASE_STMT(21);
                    CASE_STMT(22);
                    CASE_STMT(23);
                    CASE_STMT(24);
                    CASE_STMT(25);
                    CASE_STMT(26);
                    CASE_STMT(27);
                    CASE_STMT(28);
                    CASE_STMT(29);
                    CASE_STMT(30);
#undef CASE_STMT
                    default:
                        return error_node();
                }
            }

            template <typename T0, typename T1, typename T2>
            static inline bool compile(expression_generator<Type>& expr_gen, const std::string& id,
                                       T0 t0, T1 t1, T2 t2, expression_node_ptr& result)
            {
                core::operators::operator_type sf3opr;

                if (!expr_gen.sf3_optimisable(id, sf3opr))
                    return false;
                else
                    result = synthesize_sf3ext_expression::template process<T0, T1, T2>(
                        expr_gen, sf3opr, t0, t1, t2);

                return true;
            }
        };

        struct synthesize_sf4ext_expression
        {
            template <typename T0, typename T1, typename T2, typename T3>
            static inline expression_node_ptr process(expression_generator<Type>& expr_gen,
                                                      const core::operators::operator_type& sf4opr,
                                                      T0 t0, T1 t1, T2 t2, T3 t3)
            {
                switch (sf4opr)
                {
#define CASE_STMT0(op)                                                                         \
    case core::operators::operator_type::sf##op:                                               \
        return details::T0oT1oT2oT3_sf4ext<Type, T0, T1, T2, T3, details::sf##op##_op<Type>>:: \
            allocate(*(expr_gen.node_allocator_), t0, t1, t2, t3);

#define CASE_STMT1(op)                                                                            \
    case core::operators::operator_type::sf4ext##op:                                              \
        return details::T0oT1oT2oT3_sf4ext<Type, T0, T1, T2, T3, details::sfext##op##_op<Type>>:: \
            allocate(*(expr_gen.node_allocator_), t0, t1, t2, t3)

                    CASE_STMT0(48);
                    CASE_STMT0(49);
                    CASE_STMT0(50);
                    CASE_STMT0(51);
                    CASE_STMT0(52);
                    CASE_STMT0(53);
                    CASE_STMT0(54);
                    CASE_STMT0(55);
                    CASE_STMT0(56);
                    CASE_STMT0(57);
                    CASE_STMT0(58);
                    CASE_STMT0(59);
                    CASE_STMT0(60);
                    CASE_STMT0(61);
                    CASE_STMT0(62);
                    CASE_STMT0(63);
                    CASE_STMT0(64);
                    CASE_STMT0(65);
                    CASE_STMT0(66);
                    CASE_STMT0(67);
                    CASE_STMT0(68);
                    CASE_STMT0(69);
                    CASE_STMT0(70);
                    CASE_STMT0(71);
                    CASE_STMT0(72);
                    CASE_STMT0(73);
                    CASE_STMT0(74);
                    CASE_STMT0(75);
                    CASE_STMT0(76);
                    CASE_STMT0(77);
                    CASE_STMT0(78);
                    CASE_STMT0(79);
                    CASE_STMT0(80);
                    CASE_STMT0(81);
                    CASE_STMT0(82);
                    CASE_STMT0(83);

                    CASE_STMT1(00);
                    CASE_STMT1(01);
                    CASE_STMT1(02);
                    CASE_STMT1(03);
                    CASE_STMT1(04);
                    CASE_STMT1(05);
                    CASE_STMT1(06);
                    CASE_STMT1(07);
                    CASE_STMT1(08);
                    CASE_STMT1(09);
                    CASE_STMT1(10);
                    CASE_STMT1(11);
                    CASE_STMT1(12);
                    CASE_STMT1(13);
                    CASE_STMT1(14);
                    CASE_STMT1(15);
                    CASE_STMT1(16);
                    CASE_STMT1(17);
                    CASE_STMT1(18);
                    CASE_STMT1(19);
                    CASE_STMT1(20);
                    CASE_STMT1(21);
                    CASE_STMT1(22);
                    CASE_STMT1(23);
                    CASE_STMT1(24);
                    CASE_STMT1(25);
                    CASE_STMT1(26);
                    CASE_STMT1(27);
                    CASE_STMT1(28);
                    CASE_STMT1(29);
                    CASE_STMT1(30);
                    CASE_STMT1(31);
                    CASE_STMT1(32);
                    CASE_STMT1(33);
                    CASE_STMT1(34);
                    CASE_STMT1(35);
                    CASE_STMT1(36);
                    CASE_STMT1(37);
                    CASE_STMT1(38);
                    CASE_STMT1(39);
                    CASE_STMT1(40);
                    CASE_STMT1(41);
                    CASE_STMT1(42);
                    CASE_STMT1(43);
                    CASE_STMT1(44);
                    CASE_STMT1(45);
                    CASE_STMT1(46);
                    CASE_STMT1(47);
                    CASE_STMT1(48);
                    CASE_STMT1(49);
                    CASE_STMT1(50);
                    CASE_STMT1(51);
                    CASE_STMT1(52);
                    CASE_STMT1(53);
                    CASE_STMT1(54);
                    CASE_STMT1(55);
                    CASE_STMT1(56);
                    CASE_STMT1(57);
                    CASE_STMT1(58);
                    CASE_STMT1(59);
                    CASE_STMT1(60);
                    CASE_STMT1(61);

#undef CASE_STMT0
#undef CASE_STMT1
                    default:
                        return error_node();
                }
            }

            template <typename T0, typename T1, typename T2, typename T3>
            static inline bool compile(expression_generator<Type>& expr_gen, const std::string& id,
                                       T0 t0, T1 t1, T2 t2, T3 t3, expression_node_ptr& result)
            {
                core::operators::operator_type sf4opr;

                if (!expr_gen.sf4_optimisable(id, sf4opr))
                    return false;
                else
                    result = synthesize_sf4ext_expression::template process<T0, T1, T2, T3>(
                        expr_gen, sf4opr, t0, t1, t2, t3);

                return true;
            }

            // T o (sf3ext)
            template <typename ExternalType>
            static inline bool compile_right(expression_generator<Type>& expr_gen, ExternalType t,
                                             const core::operators::operator_type& operation,
                                             expression_node_ptr& sf3node,
                                             expression_node_ptr& result)
            {
                if (!details::is_sf3ext_node(sf3node))
                    return false;

                using sf3ext_base_ptr = details::T0oT1oT2_base_node<Type>*;

                sf3ext_base_ptr n = static_cast<sf3ext_base_ptr>(sf3node);
                const std::string id = "t" + expr_gen.to_str(operation) + "(" + n->type_id() + ")";

                switch (n->type())
                {
                    case details::expression_node<Type>::node_type::e_covoc:
                        return compile_right_impl<typename covoc_t::sf3_type_node, ExternalType,
                                                  ctype, vtype, ctype>(expr_gen, id, t, sf3node,
                                                                       result);

                    case details::expression_node<Type>::node_type::e_covov:
                        return compile_right_impl<typename covov_t::sf3_type_node, ExternalType,
                                                  ctype, vtype, vtype>(expr_gen, id, t, sf3node,
                                                                       result);

                    case details::expression_node<Type>::node_type::e_vocov:
                        return compile_right_impl<typename vocov_t::sf3_type_node, ExternalType,
                                                  vtype, ctype, vtype>(expr_gen, id, t, sf3node,
                                                                       result);

                    case details::expression_node<Type>::node_type::e_vovoc:
                        return compile_right_impl<typename vovoc_t::sf3_type_node, ExternalType,
                                                  vtype, vtype, ctype>(expr_gen, id, t, sf3node,
                                                                       result);

                    case details::expression_node<Type>::node_type::e_vovov:
                        return compile_right_impl<typename vovov_t::sf3_type_node, ExternalType,
                                                  vtype, vtype, vtype>(expr_gen, id, t, sf3node,
                                                                       result);

                    default:
                        return false;
                }
            }

            // (sf3ext) o T
            template <typename ExternalType>
            static inline bool compile_left(expression_generator<Type>& expr_gen, ExternalType t,
                                            const core::operators::operator_type& operation,
                                            expression_node_ptr& sf3node,
                                            expression_node_ptr& result)
            {
                if (!details::is_sf3ext_node(sf3node))
                    return false;

                using sf3ext_base_ptr = details::T0oT1oT2_base_node<Type>*;

                sf3ext_base_ptr n = static_cast<sf3ext_base_ptr>(sf3node);

                const std::string id = "(" + n->type_id() + ")" + expr_gen.to_str(operation) + "t";

                switch (n->type())
                {
                    case details::expression_node<Type>::node_type::e_covoc:
                        return compile_left_impl<typename covoc_t::sf3_type_node, ExternalType,
                                                 ctype, vtype, ctype>(expr_gen, id, t, sf3node,
                                                                      result);

                    case details::expression_node<Type>::node_type::e_covov:
                        return compile_left_impl<typename covov_t::sf3_type_node, ExternalType,
                                                 ctype, vtype, vtype>(expr_gen, id, t, sf3node,
                                                                      result);

                    case details::expression_node<Type>::node_type::e_vocov:
                        return compile_left_impl<typename vocov_t::sf3_type_node, ExternalType,
                                                 vtype, ctype, vtype>(expr_gen, id, t, sf3node,
                                                                      result);

                    case details::expression_node<Type>::node_type::e_vovoc:
                        return compile_left_impl<typename vovoc_t::sf3_type_node, ExternalType,
                                                 vtype, vtype, ctype>(expr_gen, id, t, sf3node,
                                                                      result);

                    case details::expression_node<Type>::node_type::e_vovov:
                        return compile_left_impl<typename vovov_t::sf3_type_node, ExternalType,
                                                 vtype, vtype, vtype>(expr_gen, id, t, sf3node,
                                                                      result);

                    default:
                        return false;
                }
            }

            template <typename SF3TypeNode, typename ExternalType, typename T0, typename T1,
                      typename T2>
            static inline bool compile_right_impl(expression_generator<Type>& expr_gen,
                                                  const std::string& id, ExternalType t,
                                                  expression_node_ptr& node,
                                                  expression_node_ptr& result)
            {
                SF3TypeNode* n = static_cast<SF3TypeNode*>(node);

                if (n)
                {
                    T0 t0 = n->t0();
                    T1 t1 = n->t1();
                    T2 t2 = n->t2();

                    return synthesize_sf4ext_expression::template compile<ExternalType, T0, T1, T2>(
                        expr_gen, id, t, t0, t1, t2, result);
                }
                else
                    return false;
            }

            template <typename SF3TypeNode, typename ExternalType, typename T0, typename T1,
                      typename T2>
            static inline bool compile_left_impl(expression_generator<Type>& expr_gen,
                                                 const std::string& id, ExternalType t,
                                                 expression_node_ptr& node,
                                                 expression_node_ptr& result)
            {
                SF3TypeNode* n = static_cast<SF3TypeNode*>(node);

                if (n)
                {
                    T0 t0 = n->t0();
                    T1 t1 = n->t1();
                    T2 t2 = n->t2();

                    return synthesize_sf4ext_expression::template compile<T0, T1, T2, ExternalType>(
                        expr_gen, id, t0, t1, t2, t, result);
                }
                else
                    return false;
            }
        };

        struct synthesize_vovov_expression0
        {
            using node_type = typename vovov_t::type0;
            using sf3_type = typename vovov_t::sf3_type;

            static inline expression_node_ptr process(
                expression_generator<Type>& expr_gen,
                const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
            {
                // (v0 o0 v1) o1 (v2)
                const details::vov_base_node<Type>* vov =
                    static_cast<details::vov_base_node<Type>*>(branch[0]);
                const Type& v0 = vov->v0();
                const Type& v1 = vov->v1();
                const Type& v2 = static_cast<details::variable_node<Type>*>(branch[1])->ref();
                const core::operators::operator_type o0 = vov->operation();
                const core::operators::operator_type o1 = operation;

                details::free_node(*(expr_gen.node_allocator_), branch[0]);

                expression_node_ptr result = error_node();

                if (expr_gen.parser_->settings_.strength_reduction_enabled())
                {
                    // (v0 / v1) / v2 --> (vovov) v0 / (v1 * v2)
                    if ((core::operators::operator_type::div == o0) &&
                        (core::operators::operator_type::div == o1))
                    {
                        const bool synthesis_result =
                            synthesize_sf3ext_expression::template compile<vtype, vtype, vtype>(
                                expr_gen, "t/(t*t)", v0, v1, v2, result);

                        core::debug_print("(v0 / v1) / v2 --> (vovov) v0 / (v1 * v2)\n");

                        return (synthesis_result) ? result : error_node();
                    }
                }

                const bool synthesis_result =
                    synthesize_sf3ext_expression::template compile<vtype, vtype, vtype>(
                        expr_gen, id(expr_gen, o0, o1), v0, v1, v2, result);

                if (synthesis_result)
                    return result;

                binary_functor_t f0 = nullptr;
                binary_functor_t f1 = nullptr;

                if (!expr_gen.valid_operator(o0, f0))
                    return error_node();
                else if (!expr_gen.valid_operator(o1, f1))
                    return error_node();
                else
                    return node_type::allocate(*(expr_gen.node_allocator_), v0, v1, v2, f0, f1);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const core::operators::operator_type o0,
                                         const core::operators::operator_type o1)
            {
                return core::build_string()
                       << "(t" << expr_gen.to_str(o0) << "t)" << expr_gen.to_str(o1) << "t";
            }
        };

        struct synthesize_vovov_expression1
        {
            using node_type = typename vovov_t::type1;
            using sf3_type = typename vovov_t::sf3_type;

            static inline expression_node_ptr process(
                expression_generator<Type>& expr_gen,
                const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
            {
                // (v0) o0 (v1 o1 v2)
                const details::vov_base_node<Type>* vov =
                    static_cast<details::vov_base_node<Type>*>(branch[1]);
                const Type& v0 = static_cast<details::variable_node<Type>*>(branch[0])->ref();
                const Type& v1 = vov->v0();
                const Type& v2 = vov->v1();
                const core::operators::operator_type o0 = operation;
                const core::operators::operator_type o1 = vov->operation();

                details::free_node(*(expr_gen.node_allocator_), branch[1]);

                expression_node_ptr result = error_node();

                if (expr_gen.parser_->settings_.strength_reduction_enabled())
                {
                    // v0 / (v1 / v2) --> (vovov) (v0 * v2) / v1
                    if ((core::operators::operator_type::div == o0) &&
                        (core::operators::operator_type::div == o1))
                    {
                        const bool synthesis_result =
                            synthesize_sf3ext_expression::template compile<vtype, vtype, vtype>(
                                expr_gen, "(t*t)/t", v0, v2, v1, result);

                        core::debug_print("v0 / (v1 / v2) --> (vovov) (v0 * v2) / v1\n");

                        return (synthesis_result) ? result : error_node();
                    }
                }

                const bool synthesis_result =
                    synthesize_sf3ext_expression::template compile<vtype, vtype, vtype>(
                        expr_gen, id(expr_gen, o0, o1), v0, v1, v2, result);

                if (synthesis_result)
                    return result;

                binary_functor_t f0 = nullptr;
                binary_functor_t f1 = nullptr;

                if (!expr_gen.valid_operator(o0, f0))
                    return error_node();
                else if (!expr_gen.valid_operator(o1, f1))
                    return error_node();
                else
                    return node_type::allocate(*(expr_gen.node_allocator_), v0, v1, v2, f0, f1);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const core::operators::operator_type o0,
                                         const core::operators::operator_type o1)
            {
                return core::build_string()
                       << "t" << expr_gen.to_str(o0) << "(t" << expr_gen.to_str(o1) << "t)";
            }
        };

        struct synthesize_vovoc_expression0
        {
            using node_type = typename vovoc_t::type0;
            using sf3_type = typename vovoc_t::sf3_type;

            static inline expression_node_ptr process(
                expression_generator<Type>& expr_gen,
                const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
            {
                // (v0 o0 v1) o1 (c)
                const details::vov_base_node<Type>* vov =
                    static_cast<details::vov_base_node<Type>*>(branch[0]);
                const Type& v0 = vov->v0();
                const Type& v1 = vov->v1();
                const Type c = static_cast<details::literal_node<Type>*>(branch[1])->value();
                const core::operators::operator_type o0 = vov->operation();
                const core::operators::operator_type o1 = operation;

                details::free_node(*(expr_gen.node_allocator_), branch[0]);
                details::free_node(*(expr_gen.node_allocator_), branch[1]);

                expression_node_ptr result = error_node();

                if (expr_gen.parser_->settings_.strength_reduction_enabled())
                {
                    // (v0 / v1) / c --> (vovoc) v0 / (v1 * c)
                    if ((core::operators::operator_type::div == o0) &&
                        (core::operators::operator_type::div == o1))
                    {
                        const bool synthesis_result =
                            synthesize_sf3ext_expression::template compile<vtype, vtype, ctype>(
                                expr_gen, "t/(t*t)", v0, v1, c, result);

                        core::debug_print("(v0 / v1) / c --> (vovoc) v0 / (v1 * c)\n");

                        return (synthesis_result) ? result : error_node();
                    }
                }

                const bool synthesis_result =
                    synthesize_sf3ext_expression::template compile<vtype, vtype, ctype>(
                        expr_gen, id(expr_gen, o0, o1), v0, v1, c, result);

                if (synthesis_result)
                    return result;

                binary_functor_t f0 = nullptr;
                binary_functor_t f1 = nullptr;

                if (!expr_gen.valid_operator(o0, f0))
                    return error_node();
                else if (!expr_gen.valid_operator(o1, f1))
                    return error_node();
                else
                    return node_type::allocate(*(expr_gen.node_allocator_), v0, v1, c, f0, f1);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const core::operators::operator_type o0,
                                         const core::operators::operator_type o1)
            {
                return core::build_string()
                       << "(t" << expr_gen.to_str(o0) << "t)" << expr_gen.to_str(o1) << "t";
            }
        };

        struct synthesize_vovoc_expression1
        {
            using node_type = typename vovoc_t::type1;
            using sf3_type = typename vovoc_t::sf3_type;

            static inline expression_node_ptr process(
                expression_generator<Type>& expr_gen,
                const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
            {
                // (v0) o0 (v1 o1 c)
                const details::voc_base_node<Type>* voc =
                    static_cast<const details::voc_base_node<Type>*>(branch[1]);
                const Type& v0 = static_cast<details::variable_node<Type>*>(branch[0])->ref();
                const Type& v1 = voc->v();
                const Type c = voc->c();
                const core::operators::operator_type o0 = operation;
                const core::operators::operator_type o1 = voc->operation();

                details::free_node(*(expr_gen.node_allocator_), branch[1]);

                expression_node_ptr result = error_node();

                if (expr_gen.parser_->settings_.strength_reduction_enabled())
                {
                    // v0 / (v1 / c) --> (vocov) (v0 * c) / v1
                    if ((core::operators::operator_type::div == o0) &&
                        (core::operators::operator_type::div == o1))
                    {
                        const bool synthesis_result =
                            synthesize_sf3ext_expression::template compile<vtype, ctype, vtype>(
                                expr_gen, "(t*t)/t", v0, c, v1, result);

                        core::debug_print("v0 / (v1 / c) --> (vocov) (v0 * c) / v1\n");

                        return (synthesis_result) ? result : error_node();
                    }
                }

                const bool synthesis_result =
                    synthesize_sf3ext_expression::template compile<vtype, vtype, ctype>(
                        expr_gen, id(expr_gen, o0, o1), v0, v1, c, result);

                if (synthesis_result)
                    return result;

                binary_functor_t f0 = nullptr;
                binary_functor_t f1 = nullptr;

                if (!expr_gen.valid_operator(o0, f0))
                    return error_node();
                else if (!expr_gen.valid_operator(o1, f1))
                    return error_node();
                else
                    return node_type::allocate(*(expr_gen.node_allocator_), v0, v1, c, f0, f1);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const core::operators::operator_type o0,
                                         const core::operators::operator_type o1)
            {
                return core::build_string()
                       << "t" << expr_gen.to_str(o0) << "(t" << expr_gen.to_str(o1) << "t)";
            }
        };

        struct synthesize_vocov_expression0
        {
            using node_type = typename vocov_t::type0;
            using sf3_type = typename vocov_t::sf3_type;

            static inline expression_node_ptr process(
                expression_generator<Type>& expr_gen,
                const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
            {
                // (v0 o0 c) o1 (v1)
                const details::voc_base_node<Type>* voc =
                    static_cast<details::voc_base_node<Type>*>(branch[0]);
                const Type& v0 = voc->v();
                const Type c = voc->c();
                const Type& v1 = static_cast<details::variable_node<Type>*>(branch[1])->ref();
                const core::operators::operator_type o0 = voc->operation();
                const core::operators::operator_type o1 = operation;

                details::free_node(*(expr_gen.node_allocator_), branch[0]);

                expression_node_ptr result = error_node();

                if (expr_gen.parser_->settings_.strength_reduction_enabled())
                {
                    // (v0 / c) / v1 --> (vovoc) v0 / (v1 * c)
                    if ((core::operators::operator_type::div == o0) &&
                        (core::operators::operator_type::div == o1))
                    {
                        const bool synthesis_result =
                            synthesize_sf3ext_expression::template compile<vtype, vtype, ctype>(
                                expr_gen, "t/(t*t)", v0, v1, c, result);

                        core::debug_print("(v0 / c) / v1 --> (vovoc) v0 / (v1 * c)\n");

                        return (synthesis_result) ? result : error_node();
                    }
                }

                const bool synthesis_result =
                    synthesize_sf3ext_expression::template compile<vtype, ctype, vtype>(
                        expr_gen, id(expr_gen, o0, o1), v0, c, v1, result);

                if (synthesis_result)
                    return result;

                binary_functor_t f0 = nullptr;
                binary_functor_t f1 = nullptr;

                if (!expr_gen.valid_operator(o0, f0))
                    return error_node();
                else if (!expr_gen.valid_operator(o1, f1))
                    return error_node();
                else
                    return node_type::allocate(*(expr_gen.node_allocator_), v0, c, v1, f0, f1);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const core::operators::operator_type o0,
                                         const core::operators::operator_type o1)
            {
                return core::build_string()
                       << "(t" << expr_gen.to_str(o0) << "t)" << expr_gen.to_str(o1) << "t";
            }
        };

        struct synthesize_vocov_expression1
        {
            using node_type = typename vocov_t::type1;
            using sf3_type = typename vocov_t::sf3_type;

            static inline expression_node_ptr process(
                expression_generator<Type>& expr_gen,
                const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
            {
                // (v0) o0 (c o1 v1)
                const details::cov_base_node<Type>* cov =
                    static_cast<details::cov_base_node<Type>*>(branch[1]);
                const Type& v0 = static_cast<details::variable_node<Type>*>(branch[0])->ref();
                const Type c = cov->c();
                const Type& v1 = cov->v();
                const core::operators::operator_type o0 = operation;
                const core::operators::operator_type o1 = cov->operation();

                details::free_node(*(expr_gen.node_allocator_), branch[1]);

                expression_node_ptr result = error_node();

                if (expr_gen.parser_->settings_.strength_reduction_enabled())
                {
                    // v0 / (c / v1) --> (vovoc) (v0 * v1) / c
                    if ((core::operators::operator_type::div == o0) &&
                        (core::operators::operator_type::div == o1))
                    {
                        const bool synthesis_result =
                            synthesize_sf3ext_expression::template compile<vtype, vtype, ctype>(
                                expr_gen, "(t*t)/t", v0, v1, c, result);

                        core::debug_print("v0 / (c / v1) --> (vovoc) (v0 * v1) / c\n");

                        return (synthesis_result) ? result : error_node();
                    }
                }

                const bool synthesis_result =
                    synthesize_sf3ext_expression::template compile<vtype, ctype, vtype>(
                        expr_gen, id(expr_gen, o0, o1), v0, c, v1, result);

                if (synthesis_result)
                    return result;

                binary_functor_t f0 = nullptr;
                binary_functor_t f1 = nullptr;

                if (!expr_gen.valid_operator(o0, f0))
                    return error_node();
                else if (!expr_gen.valid_operator(o1, f1))
                    return error_node();
                else
                    return node_type::allocate(*(expr_gen.node_allocator_), v0, c, v1, f0, f1);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const core::operators::operator_type o0,
                                         const core::operators::operator_type o1)
            {
                return core::build_string()
                       << "t" << expr_gen.to_str(o0) << "(t" << expr_gen.to_str(o1) << "t)";
            }
        };

        struct synthesize_covov_expression0
        {
            using node_type = typename covov_t::type0;
            using sf3_type = typename covov_t::sf3_type;

            static inline expression_node_ptr process(
                expression_generator<Type>& expr_gen,
                const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
            {
                // (c o0 v0) o1 (v1)
                const details::cov_base_node<Type>* cov =
                    static_cast<details::cov_base_node<Type>*>(branch[0]);
                const Type c = cov->c();
                const Type& v0 = cov->v();
                const Type& v1 = static_cast<details::variable_node<Type>*>(branch[1])->ref();
                const core::operators::operator_type o0 = cov->operation();
                const core::operators::operator_type o1 = operation;

                details::free_node(*(expr_gen.node_allocator_), branch[0]);

                expression_node_ptr result = error_node();

                if (expr_gen.parser_->settings_.strength_reduction_enabled())
                {
                    // (c / v0) / v1 --> (covov) c / (v0 * v1)
                    if ((core::operators::operator_type::div == o0) &&
                        (core::operators::operator_type::div == o1))
                    {
                        const bool synthesis_result =
                            synthesize_sf3ext_expression::template compile<ctype, vtype, vtype>(
                                expr_gen, "t/(t*t)", c, v0, v1, result);

                        core::debug_print("(c / v0) / v1 --> (covov) c / (v0 * v1)\n");

                        return (synthesis_result) ? result : error_node();
                    }
                }

                const bool synthesis_result =
                    synthesize_sf3ext_expression::template compile<ctype, vtype, vtype>(
                        expr_gen, id(expr_gen, o0, o1), c, v0, v1, result);

                if (synthesis_result)
                    return result;

                binary_functor_t f0 = nullptr;
                binary_functor_t f1 = nullptr;

                if (!expr_gen.valid_operator(o0, f0))
                    return error_node();
                else if (!expr_gen.valid_operator(o1, f1))
                    return error_node();
                else
                    return node_type::allocate(*(expr_gen.node_allocator_), c, v0, v1, f0, f1);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const core::operators::operator_type o0,
                                         const core::operators::operator_type o1)
            {
                return core::build_string()
                       << "(t" << expr_gen.to_str(o0) << "t)" << expr_gen.to_str(o1) << "t";
            }
        };

        struct synthesize_covov_expression1
        {
            using node_type = typename covov_t::type1;
            using sf3_type = typename covov_t::sf3_type;

            static inline expression_node_ptr process(
                expression_generator<Type>& expr_gen,
                const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
            {
                // (c) o0 (v0 o1 v1)
                const details::vov_base_node<Type>* vov =
                    static_cast<details::vov_base_node<Type>*>(branch[1]);
                const Type c = static_cast<details::literal_node<Type>*>(branch[0])->value();
                const Type& v0 = vov->v0();
                const Type& v1 = vov->v1();
                const core::operators::operator_type o0 = operation;
                const core::operators::operator_type o1 = vov->operation();

                details::free_node(*(expr_gen.node_allocator_), branch[0]);
                details::free_node(*(expr_gen.node_allocator_), branch[1]);

                expression_node_ptr result = error_node();

                if (expr_gen.parser_->settings_.strength_reduction_enabled())
                {
                    // c / (v0 / v1) --> (covov) (c * v1) / v0
                    if ((core::operators::operator_type::div == o0) &&
                        (core::operators::operator_type::div == o1))
                    {
                        const bool synthesis_result =
                            synthesize_sf3ext_expression::template compile<ctype, vtype, vtype>(
                                expr_gen, "(t*t)/t", c, v1, v0, result);

                        core::debug_print("c / (v0 / v1) --> (covov) (c * v1) / v0\n");

                        return (synthesis_result) ? result : error_node();
                    }
                }

                const bool synthesis_result =
                    synthesize_sf3ext_expression::template compile<ctype, vtype, vtype>(
                        expr_gen, id(expr_gen, o0, o1), c, v0, v1, result);

                if (synthesis_result)
                    return result;

                binary_functor_t f0 = nullptr;
                binary_functor_t f1 = nullptr;

                if (!expr_gen.valid_operator(o0, f0))
                    return error_node();
                else if (!expr_gen.valid_operator(o1, f1))
                    return error_node();
                else
                    return node_type::allocate(*(expr_gen.node_allocator_), c, v0, v1, f0, f1);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const core::operators::operator_type o0,
                                         const core::operators::operator_type o1)
            {
                return core::build_string()
                       << "t" << expr_gen.to_str(o0) << "(t" << expr_gen.to_str(o1) << "t)";
            }
        };

        struct synthesize_covoc_expression0
        {
            using node_type = typename covoc_t::type0;
            using sf3_type = typename covoc_t::sf3_type;

            static inline expression_node_ptr process(
                expression_generator<Type>& expr_gen,
                const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
            {
                // (c0 o0 v) o1 (c1)
                const details::cov_base_node<Type>* cov =
                    static_cast<details::cov_base_node<Type>*>(branch[0]);
                const Type c0 = cov->c();
                const Type& v = cov->v();
                const Type c1 = static_cast<details::literal_node<Type>*>(branch[1])->value();
                const core::operators::operator_type o0 = cov->operation();
                const core::operators::operator_type o1 = operation;

                details::free_node(*(expr_gen.node_allocator_), branch[0]);
                details::free_node(*(expr_gen.node_allocator_), branch[1]);

                expression_node_ptr result = error_node();

                if (expr_gen.parser_->settings_.strength_reduction_enabled())
                {
                    // (c0 + v) + c1 --> (cov) (c0 + c1) + v
                    if ((core::operators::operator_type::add == o0) &&
                        (core::operators::operator_type::add == o1))
                    {
                        core::debug_print("(c0 + v) + c1 --> (cov) (c0 + c1) + v\n");

                        return expr_gen.node_allocator_->template allocate_cr<
                            typename details::cov_node<Type, details::add_op<Type>>>(c0 + c1, v);
                    }
                    // (c0 + v) - c1 --> (cov) (c0 - c1) + v
                    else if ((core::operators::operator_type::add == o0) &&
                             (core::operators::operator_type::sub == o1))
                    {
                        core::debug_print("(c0 + v) - c1 --> (cov) (c0 - c1) + v\n");

                        return expr_gen.node_allocator_->template allocate_cr<
                            typename details::cov_node<Type, details::add_op<Type>>>(c0 - c1, v);
                    }
                    // (c0 - v) + c1 --> (cov) (c0 + c1) - v
                    else if ((core::operators::operator_type::sub == o0) &&
                             (core::operators::operator_type::add == o1))
                    {
                        core::debug_print("(c0 - v) + c1 --> (cov) (c0 + c1) - v\n");

                        return expr_gen.node_allocator_->template allocate_cr<
                            typename details::cov_node<Type, details::sub_op<Type>>>(c0 + c1, v);
                    }
                    // (c0 - v) - c1 --> (cov) (c0 - c1) - v
                    else if ((core::operators::operator_type::sub == o0) &&
                             (core::operators::operator_type::sub == o1))
                    {
                        core::debug_print("(c0 - v) - c1 --> (cov) (c0 - c1) - v\n");

                        return expr_gen.node_allocator_->template allocate_cr<
                            typename details::cov_node<Type, details::sub_op<Type>>>(c0 - c1, v);
                    }
                    // (c0 * v) * c1 --> (cov) (c0 * c1) * v
                    else if ((core::operators::operator_type::mul == o0) &&
                             (core::operators::operator_type::mul == o1))
                    {
                        core::debug_print("(c0 * v) * c1 --> (cov) (c0 * c1) * v\n");

                        return expr_gen.node_allocator_->template allocate_cr<
                            typename details::cov_node<Type, details::mul_op<Type>>>(c0 * c1, v);
                    }
                    // (c0 * v) / c1 --> (cov) (c0 / c1) * v
                    else if ((core::operators::operator_type::mul == o0) &&
                             (core::operators::operator_type::div == o1))
                    {
                        core::debug_print("(c0 * v) / c1 --> (cov) (c0 / c1) * v\n");

                        return expr_gen.node_allocator_->template allocate_cr<
                            typename details::cov_node<Type, details::mul_op<Type>>>(c0 / c1, v);
                    }
                    // (c0 / v) * c1 --> (cov) (c0 * c1) / v
                    else if ((core::operators::operator_type::div == o0) &&
                             (core::operators::operator_type::mul == o1))
                    {
                        core::debug_print("(c0 / v) * c1 --> (cov) (c0 * c1) / v\n");

                        return expr_gen.node_allocator_->template allocate_cr<
                            typename details::cov_node<Type, details::div_op<Type>>>(c0 * c1, v);
                    }
                    // (c0 / v) / c1 --> (cov) (c0 / c1) / v
                    else if ((core::operators::operator_type::div == o0) &&
                             (core::operators::operator_type::div == o1))
                    {
                        core::debug_print("(c0 / v) / c1 --> (cov) (c0 / c1) / v\n");

                        return expr_gen.node_allocator_->template allocate_cr<
                            typename details::cov_node<Type, details::div_op<Type>>>(c0 / c1, v);
                    }
                }

                const bool synthesis_result =
                    synthesize_sf3ext_expression::template compile<ctype, vtype, ctype>(
                        expr_gen, id(expr_gen, o0, o1), c0, v, c1, result);

                if (synthesis_result)
                    return result;

                binary_functor_t f0 = nullptr;
                binary_functor_t f1 = nullptr;

                if (!expr_gen.valid_operator(o0, f0))
                    return error_node();
                else if (!expr_gen.valid_operator(o1, f1))
                    return error_node();
                else
                    return node_type::allocate(*(expr_gen.node_allocator_), c0, v, c1, f0, f1);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const core::operators::operator_type o0,
                                         const core::operators::operator_type o1)
            {
                return core::build_string()
                       << "(t" << expr_gen.to_str(o0) << "t)" << expr_gen.to_str(o1) << "t";
            }
        };

        struct synthesize_covoc_expression1
        {
            using node_type = typename covoc_t::type1;
            using sf3_type = typename covoc_t::sf3_type;

            static inline expression_node_ptr process(
                expression_generator<Type>& expr_gen,
                const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
            {
                // (c0) o0 (v o1 c1)
                const details::voc_base_node<Type>* voc =
                    static_cast<details::voc_base_node<Type>*>(branch[1]);
                const Type c0 = static_cast<details::literal_node<Type>*>(branch[0])->value();
                const Type& v = voc->v();
                const Type c1 = voc->c();
                const core::operators::operator_type o0 = operation;
                const core::operators::operator_type o1 = voc->operation();

                details::free_node(*(expr_gen.node_allocator_), branch[0]);
                details::free_node(*(expr_gen.node_allocator_), branch[1]);

                expression_node_ptr result = error_node();

                if (expr_gen.parser_->settings_.strength_reduction_enabled())
                {
                    // (c0) + (v + c1) --> (cov) (c0 + c1) + v
                    if ((core::operators::operator_type::add == o0) &&
                        (core::operators::operator_type::add == o1))
                    {
                        core::debug_print("(c0) + (v + c1) --> (cov) (c0 + c1) + v\n");

                        return expr_gen.node_allocator_->template allocate_cr<
                            typename details::cov_node<Type, details::add_op<Type>>>(c0 + c1, v);
                    }
                    // (c0) + (v - c1) --> (cov) (c0 - c1) + v
                    else if ((core::operators::operator_type::add == o0) &&
                             (core::operators::operator_type::sub == o1))
                    {
                        core::debug_print("(c0) + (v - c1) --> (cov) (c0 - c1) + v\n");

                        return expr_gen.node_allocator_->template allocate_cr<
                            typename details::cov_node<Type, details::add_op<Type>>>(c0 - c1, v);
                    }
                    // (c0) - (v + c1) --> (cov) (c0 - c1) - v
                    else if ((core::operators::operator_type::sub == o0) &&
                             (core::operators::operator_type::add == o1))
                    {
                        core::debug_print("(c0) - (v + c1) --> (cov) (c0 - c1) - v\n");

                        return expr_gen.node_allocator_->template allocate_cr<
                            typename details::cov_node<Type, details::sub_op<Type>>>(c0 - c1, v);
                    }
                    // (c0) - (v - c1) --> (cov) (c0 + c1) - v
                    else if ((core::operators::operator_type::sub == o0) &&
                             (core::operators::operator_type::sub == o1))
                    {
                        core::debug_print("(c0) - (v - c1) --> (cov) (c0 + c1) - v\n");

                        return expr_gen.node_allocator_->template allocate_cr<
                            typename details::cov_node<Type, details::sub_op<Type>>>(c0 + c1, v);
                    }
                    // (c0) * (v * c1) --> (voc) v * (c0 * c1)
                    else if ((core::operators::operator_type::mul == o0) &&
                             (core::operators::operator_type::mul == o1))
                    {
                        core::debug_print("(c0) * (v * c1) --> (voc) v * (c0 * c1)\n");

                        return expr_gen.node_allocator_->template allocate_cr<
                            typename details::cov_node<Type, details::mul_op<Type>>>(c0 * c1, v);
                    }
                    // (c0) * (v / c1) --> (cov) (c0 / c1) * v
                    else if ((core::operators::operator_type::mul == o0) &&
                             (core::operators::operator_type::div == o1))
                    {
                        core::debug_print("(c0) * (v / c1) --> (cov) (c0 / c1) * v\n");

                        return expr_gen.node_allocator_->template allocate_cr<
                            typename details::cov_node<Type, details::mul_op<Type>>>(c0 / c1, v);
                    }
                    // (c0) / (v * c1) --> (cov) (c0 / c1) / v
                    else if ((core::operators::operator_type::div == o0) &&
                             (core::operators::operator_type::mul == o1))
                    {
                        core::debug_print("(c0) / (v * c1) --> (cov) (c0 / c1) / v\n");

                        return expr_gen.node_allocator_->template allocate_cr<
                            typename details::cov_node<Type, details::div_op<Type>>>(c0 / c1, v);
                    }
                    // (c0) / (v / c1) --> (cov) (c0 * c1) / v
                    else if ((core::operators::operator_type::div == o0) &&
                             (core::operators::operator_type::div == o1))
                    {
                        core::debug_print("(c0) / (v / c1) --> (cov) (c0 * c1) / v\n");

                        return expr_gen.node_allocator_->template allocate_cr<
                            typename details::cov_node<Type, details::div_op<Type>>>(c0 * c1, v);
                    }
                }

                const bool synthesis_result =
                    synthesize_sf3ext_expression::template compile<ctype, vtype, ctype>(
                        expr_gen, id(expr_gen, o0, o1), c0, v, c1, result);

                if (synthesis_result)
                    return result;

                binary_functor_t f0 = nullptr;
                binary_functor_t f1 = nullptr;

                if (!expr_gen.valid_operator(o0, f0))
                    return error_node();
                else if (!expr_gen.valid_operator(o1, f1))
                    return error_node();
                else
                    return node_type::allocate(*(expr_gen.node_allocator_), c0, v, c1, f0, f1);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const core::operators::operator_type o0,
                                         const core::operators::operator_type o1)
            {
                return core::build_string()
                       << "t" << expr_gen.to_str(o0) << "(t" << expr_gen.to_str(o1) << "t)";
            }
        };

        struct synthesize_cocov_expression0
        {
            using node_type = typename cocov_t::type0;
            static inline expression_node_ptr process(expression_generator<Type>&,
                                                      const core::operators::operator_type&,
                                                      expression_node_ptr (&)[2])
            {
                // (c0 o0 c1) o1 (v) - Not possible.
                return error_node();
            }
        };

        struct synthesize_cocov_expression1
        {
            using node_type = typename cocov_t::type1;
            using sf3_type = typename cocov_t::sf3_type;

            static inline expression_node_ptr process(
                expression_generator<Type>& expr_gen,
                const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
            {
                // (c0) o0 (c1 o1 v)
                const details::cov_base_node<Type>* cov =
                    static_cast<details::cov_base_node<Type>*>(branch[1]);
                const Type c0 = static_cast<details::literal_node<Type>*>(branch[0])->value();
                const Type c1 = cov->c();
                const Type& v = cov->v();
                const core::operators::operator_type o0 = operation;
                const core::operators::operator_type o1 = cov->operation();

                details::free_node(*(expr_gen.node_allocator_), branch[0]);
                details::free_node(*(expr_gen.node_allocator_), branch[1]);

                expression_node_ptr result = error_node();

                if (expr_gen.parser_->settings_.strength_reduction_enabled())
                {
                    // (c0) + (c1 + v) --> (cov) (c0 + c1) + v
                    if ((core::operators::operator_type::add == o0) &&
                        (core::operators::operator_type::add == o1))
                    {
                        core::debug_print("(c0) + (c1 + v) --> (cov) (c0 + c1) + v\n");

                        return expr_gen.node_allocator_->template allocate_cr<
                            typename details::cov_node<Type, details::add_op<Type>>>(c0 + c1, v);
                    }
                    // (c0) + (c1 - v) --> (cov) (c0 + c1) - v
                    else if ((core::operators::operator_type::add == o0) &&
                             (core::operators::operator_type::sub == o1))
                    {
                        core::debug_print("(c0) + (c1 - v) --> (cov) (c0 + c1) - v\n");

                        return expr_gen.node_allocator_->template allocate_cr<
                            typename details::cov_node<Type, details::sub_op<Type>>>(c0 + c1, v);
                    }
                    // (c0) - (c1 + v) --> (cov) (c0 - c1) - v
                    else if ((core::operators::operator_type::sub == o0) &&
                             (core::operators::operator_type::add == o1))
                    {
                        core::debug_print("(c0) - (c1 + v) --> (cov) (c0 - c1) - v\n");

                        return expr_gen.node_allocator_->template allocate_cr<
                            typename details::cov_node<Type, details::sub_op<Type>>>(c0 - c1, v);
                    }
                    // (c0) - (c1 - v) --> (cov) (c0 - c1) + v
                    else if ((core::operators::operator_type::sub == o0) &&
                             (core::operators::operator_type::sub == o1))
                    {
                        core::debug_print("(c0) - (c1 - v) --> (cov) (c0 - c1) + v\n");

                        return expr_gen.node_allocator_->template allocate_cr<
                            typename details::cov_node<Type, details::add_op<Type>>>(c0 - c1, v);
                    }
                    // (c0) * (c1 * v) --> (cov) (c0 * c1) * v
                    else if ((core::operators::operator_type::mul == o0) &&
                             (core::operators::operator_type::mul == o1))
                    {
                        core::debug_print("(c0) * (c1 * v) --> (cov) (c0 * c1) * v\n");

                        return expr_gen.node_allocator_->template allocate_cr<
                            typename details::cov_node<Type, details::mul_op<Type>>>(c0 * c1, v);
                    }
                    // (c0) * (c1 / v) --> (cov) (c0 * c1) / v
                    else if ((core::operators::operator_type::mul == o0) &&
                             (core::operators::operator_type::div == o1))
                    {
                        core::debug_print("(c0) * (c1 / v) --> (cov) (c0 * c1) / v\n");

                        return expr_gen.node_allocator_->template allocate_cr<
                            typename details::cov_node<Type, details::div_op<Type>>>(c0 * c1, v);
                    }
                    // (c0) / (c1 * v) --> (cov) (c0 / c1) / v
                    else if ((core::operators::operator_type::div == o0) &&
                             (core::operators::operator_type::mul == o1))
                    {
                        core::debug_print("(c0) / (c1 * v) --> (cov) (c0 / c1) / v\n");

                        return expr_gen.node_allocator_->template allocate_cr<
                            typename details::cov_node<Type, details::div_op<Type>>>(c0 / c1, v);
                    }
                    // (c0) / (c1 / v) --> (cov) (c0 / c1) * v
                    else if ((core::operators::operator_type::div == o0) &&
                             (core::operators::operator_type::div == o1))
                    {
                        core::debug_print("(c0) / (c1 / v) --> (cov) (c0 / c1) * v\n");

                        return expr_gen.node_allocator_->template allocate_cr<
                            typename details::cov_node<Type, details::mul_op<Type>>>(c0 / c1, v);
                    }
                }

                const bool synthesis_result =
                    synthesize_sf3ext_expression::template compile<ctype, ctype, vtype>(
                        expr_gen, id(expr_gen, o0, o1), c0, c1, v, result);

                if (synthesis_result)
                    return result;

                binary_functor_t f0 = nullptr;
                binary_functor_t f1 = nullptr;

                if (!expr_gen.valid_operator(o0, f0))
                    return error_node();
                else if (!expr_gen.valid_operator(o1, f1))
                    return error_node();
                else
                    return node_type::allocate(*(expr_gen.node_allocator_), c0, c1, v, f0, f1);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const core::operators::operator_type o0,
                                         const core::operators::operator_type o1)
            {
                return core::build_string()
                       << "t" << expr_gen.to_str(o0) << "(t" << expr_gen.to_str(o1) << "t)";
            }
        };

        struct synthesize_vococ_expression0
        {
            using node_type = typename vococ_t::type0;
            using sf3_type = typename vococ_t::sf3_type;

            static inline expression_node_ptr process(
                expression_generator<Type>& expr_gen,
                const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
            {
                // (v o0 c0) o1 (c1)
                const details::voc_base_node<Type>* voc =
                    static_cast<details::voc_base_node<Type>*>(branch[0]);
                const Type& v = voc->v();
                const Type& c0 = voc->c();
                const Type& c1 = static_cast<details::literal_node<Type>*>(branch[1])->value();
                const core::operators::operator_type o0 = voc->operation();
                const core::operators::operator_type o1 = operation;

                details::free_node(*(expr_gen.node_allocator_), branch[0]);
                details::free_node(*(expr_gen.node_allocator_), branch[1]);

                expression_node_ptr result = error_node();

                if (expr_gen.parser_->settings_.strength_reduction_enabled())
                {
                    // (v + c0) + c1 --> (voc) v + (c0 + c1)
                    if ((core::operators::operator_type::add == o0) &&
                        (core::operators::operator_type::add == o1))
                    {
                        core::debug_print("(v + c0) + c1 --> (voc) v + (c0 + c1)\n");

                        return expr_gen.node_allocator_->template allocate_rc<
                            typename details::voc_node<Type, details::add_op<Type>>>(v, c0 + c1);
                    }
                    // (v + c0) - c1 --> (voc) v + (c0 - c1)
                    else if ((core::operators::operator_type::add == o0) &&
                             (core::operators::operator_type::sub == o1))
                    {
                        core::debug_print("(v + c0) - c1 --> (voc) v + (c0 - c1)\n");

                        return expr_gen.node_allocator_->template allocate_rc<
                            typename details::voc_node<Type, details::add_op<Type>>>(v, c0 - c1);
                    }
                    // (v - c0) + c1 --> (voc) v - (c0 + c1)
                    else if ((core::operators::operator_type::sub == o0) &&
                             (core::operators::operator_type::add == o1))
                    {
                        core::debug_print("(v - c0) + c1 --> (voc) v - (c0 + c1)\n");

                        return expr_gen.node_allocator_->template allocate_rc<
                            typename details::voc_node<Type, details::add_op<Type>>>(v, c1 - c0);
                    }
                    // (v - c0) - c1 --> (voc) v - (c0 + c1)
                    else if ((core::operators::operator_type::sub == o0) &&
                             (core::operators::operator_type::sub == o1))
                    {
                        core::debug_print("(v - c0) - c1 --> (voc) v - (c0 + c1)\n");

                        return expr_gen.node_allocator_->template allocate_rc<
                            typename details::voc_node<Type, details::sub_op<Type>>>(v, c0 + c1);
                    }
                    // (v * c0) * c1 --> (voc) v * (c0 * c1)
                    else if ((core::operators::operator_type::mul == o0) &&
                             (core::operators::operator_type::mul == o1))
                    {
                        core::debug_print("(v * c0) * c1 --> (voc) v * (c0 * c1)\n");

                        return expr_gen.node_allocator_->template allocate_rc<
                            typename details::voc_node<Type, details::mul_op<Type>>>(v, c0 * c1);
                    }
                    // (v * c0) / c1 --> (voc) v * (c0 / c1)
                    else if ((core::operators::operator_type::mul == o0) &&
                             (core::operators::operator_type::div == o1))
                    {
                        core::debug_print("(v * c0) / c1 --> (voc) v * (c0 / c1)\n");

                        return expr_gen.node_allocator_->template allocate_rc<
                            typename details::voc_node<Type, details::mul_op<Type>>>(v, c0 / c1);
                    }
                    // (v / c0) * c1 --> (voc) v * (c1 / c0)
                    else if ((core::operators::operator_type::div == o0) &&
                             (core::operators::operator_type::mul == o1))
                    {
                        core::debug_print("(v / c0) * c1 --> (voc) v * (c1 / c0)\n");

                        return expr_gen.node_allocator_->template allocate_rc<
                            typename details::voc_node<Type, details::mul_op<Type>>>(v, c1 / c0);
                    }
                    // (v / c0) / c1 --> (voc) v / (c0 * c1)
                    else if ((core::operators::operator_type::div == o0) &&
                             (core::operators::operator_type::div == o1))
                    {
                        core::debug_print("(v / c0) / c1 --> (voc) v / (c0 * c1)\n");

                        return expr_gen.node_allocator_->template allocate_rc<
                            typename details::voc_node<Type, details::div_op<Type>>>(v, c0 * c1);
                    }
                    // (v ^ c0) ^ c1 --> (voc) v ^ (c0 * c1)
                    else if ((core::operators::operator_type::pow == o0) &&
                             (core::operators::operator_type::pow == o1))
                    {
                        core::debug_print("(v ^ c0) ^ c1 --> (voc) v ^ (c0 * c1)\n");

                        return expr_gen.node_allocator_->template allocate_rc<
                            typename details::voc_node<Type, details::pow_op<Type>>>(v, c0 * c1);
                    }
                }

                const bool synthesis_result =
                    synthesize_sf3ext_expression::template compile<vtype, ctype, ctype>(
                        expr_gen, id(expr_gen, o0, o1), v, c0, c1, result);

                if (synthesis_result)
                    return result;

                binary_functor_t f0 = nullptr;
                binary_functor_t f1 = nullptr;

                if (!expr_gen.valid_operator(o0, f0))
                    return error_node();
                else if (!expr_gen.valid_operator(o1, f1))
                    return error_node();
                else
                    return node_type::allocate(*(expr_gen.node_allocator_), v, c0, c1, f0, f1);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const core::operators::operator_type o0,
                                         const core::operators::operator_type o1)
            {
                return core::build_string()
                       << "(t" << expr_gen.to_str(o0) << "t)" << expr_gen.to_str(o1) << "t";
            }
        };

        struct synthesize_vococ_expression1
        {
            using node_type = typename vococ_t::type0;

            static inline expression_node_ptr process(expression_generator<Type>&,
                                                      const core::operators::operator_type&,
                                                      expression_node_ptr (&)[2])
            {
                // (v) o0 (c0 o1 c1) - Not possible.
                core::debug_print("(v) o0 (c0 o1 c1) - Not possible.\n");
                return error_node();
            }
        };

        struct synthesize_vovovov_expression0
        {
            using node_type = typename vovovov_t::type0;
            using sf4_type = typename vovovov_t::sf4_type;
            using T0 = typename node_type::T0;
            using T1 = typename node_type::T1;
            using T2 = typename node_type::T2;
            using T3 = typename node_type::T3;

            static inline expression_node_ptr process(
                expression_generator<Type>& expr_gen,
                const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
            {
                // (v0 o0 v1) o1 (v2 o2 v3)
                const details::vov_base_node<Type>* vov0 =
                    static_cast<details::vov_base_node<Type>*>(branch[0]);
                const details::vov_base_node<Type>* vov1 =
                    static_cast<details::vov_base_node<Type>*>(branch[1]);
                const Type& v0 = vov0->v0();
                const Type& v1 = vov0->v1();
                const Type& v2 = vov1->v0();
                const Type& v3 = vov1->v1();
                const core::operators::operator_type o0 = vov0->operation();
                const core::operators::operator_type o1 = operation;
                const core::operators::operator_type o2 = vov1->operation();

                details::free_node(*(expr_gen.node_allocator_), branch[0]);
                details::free_node(*(expr_gen.node_allocator_), branch[1]);

                expression_node_ptr result = error_node();

                if (expr_gen.parser_->settings_.strength_reduction_enabled())
                {
                    // (v0 / v1) * (v2 / v3) --> (vovovov) (v0 * v2) / (v1 * v3)
                    if ((core::operators::operator_type::div == o0) &&
                        (core::operators::operator_type::mul == o1) &&
                        (core::operators::operator_type::div == o2))
                    {
                        const bool synthesis_result =
                            synthesize_sf4ext_expression::template compile<vtype, vtype, vtype,
                                                                           vtype>(
                                expr_gen, "(t*t)/(t*t)", v0, v2, v1, v3, result);

                        core::debug_print(
                            "(v0 / v1) * (v2 / v3) --> (vovovov) (v0 * v2) / (v1 * v3)\n");

                        return (synthesis_result) ? result : error_node();
                    }
                    // (v0 / v1) / (v2 / v3) --> (vovovov) (v0 * v3) / (v1 * v2)
                    else if ((core::operators::operator_type::div == o0) &&
                             (core::operators::operator_type::div == o1) &&
                             (core::operators::operator_type::div == o2))
                    {
                        const bool synthesis_result =
                            synthesize_sf4ext_expression::template compile<vtype, vtype, vtype,
                                                                           vtype>(
                                expr_gen, "(t*t)/(t*t)", v0, v3, v1, v2, result);

                        core::debug_print(
                            "(v0 / v1) / (v2 / v3) --> (vovovov) (v0 * v3) / (v1 * v2)\n");

                        return (synthesis_result) ? result : error_node();
                    }
                    // (v0 + v1) / (v2 / v3) --> (vovovov) (v0 + v1) * (v3 / v2)
                    else if ((core::operators::operator_type::add == o0) &&
                             (core::operators::operator_type::div == o1) &&
                             (core::operators::operator_type::div == o2))
                    {
                        const bool synthesis_result =
                            synthesize_sf4ext_expression::template compile<vtype, vtype, vtype,
                                                                           vtype>(
                                expr_gen, "(t+t)*(t/t)", v0, v1, v3, v2, result);

                        core::debug_print(
                            "(v0 + v1) / (v2 / v3) --> (vovovov) (v0 + v1) * (v3 / v2)\n");

                        return (synthesis_result) ? result : error_node();
                    }
                    // (v0 - v1) / (v2 / v3) --> (vovovov) (v0 + v1) * (v3 / v2)
                    else if ((core::operators::operator_type::sub == o0) &&
                             (core::operators::operator_type::div == o1) &&
                             (core::operators::operator_type::div == o2))
                    {
                        const bool synthesis_result =
                            synthesize_sf4ext_expression::template compile<vtype, vtype, vtype,
                                                                           vtype>(
                                expr_gen, "(t-t)*(t/t)", v0, v1, v3, v2, result);

                        core::debug_print(
                            "(v0 - v1) / (v2 / v3) --> (vovovov) (v0 - v1) * (v3 / v2)\n");

                        return (synthesis_result) ? result : error_node();
                    }
                    // (v0 * v1) / (v2 / v3) --> (vovovov) ((v0 * v1) * v3) / v2
                    else if ((core::operators::operator_type::mul == o0) &&
                             (core::operators::operator_type::div == o1) &&
                             (core::operators::operator_type::div == o2))
                    {
                        const bool synthesis_result =
                            synthesize_sf4ext_expression::template compile<vtype, vtype, vtype,
                                                                           vtype>(
                                expr_gen, "((t*t)*t)/t", v0, v1, v3, v2, result);

                        core::debug_print(
                            "(v0 * v1) / (v2 / v3) --> (vovovov) ((v0 * v1) * v3) / v2\n");

                        return (synthesis_result) ? result : error_node();
                    }
                }

                const bool synthesis_result =
                    synthesize_sf4ext_expression::template compile<T0, T1, T2, T3>(
                        expr_gen, id(expr_gen, o0, o1, o2), v0, v1, v2, v3, result);

                if (synthesis_result)
                    return result;

                binary_functor_t f0 = nullptr;
                binary_functor_t f1 = nullptr;
                binary_functor_t f2 = nullptr;

                if (!expr_gen.valid_operator(o0, f0))
                    return error_node();
                else if (!expr_gen.valid_operator(o1, f1))
                    return error_node();
                else if (!expr_gen.valid_operator(o2, f2))
                    return error_node();
                else
                    return node_type::allocate(*(expr_gen.node_allocator_), v0, v1, v2, v3, f0, f1,
                                               f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const core::operators::operator_type o0,
                                         const core::operators::operator_type o1,
                                         const core::operators::operator_type o2)
            {
                return core::build_string()
                       << "(t" << expr_gen.to_str(o0) << "t)" << expr_gen.to_str(o1) << "(t"
                       << expr_gen.to_str(o2) << "t)";
            }
        };

        struct synthesize_vovovoc_expression0
        {
            using node_type = typename vovovoc_t::type0;
            using sf4_type = typename vovovoc_t::sf4_type;
            using T0 = typename node_type::T0;
            using T1 = typename node_type::T1;
            using T2 = typename node_type::T2;
            using T3 = typename node_type::T3;

            static inline expression_node_ptr process(
                expression_generator<Type>& expr_gen,
                const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
            {
                // (v0 o0 v1) o1 (v2 o2 c)
                const details::vov_base_node<Type>* vov =
                    static_cast<details::vov_base_node<Type>*>(branch[0]);
                const details::voc_base_node<Type>* voc =
                    static_cast<details::voc_base_node<Type>*>(branch[1]);
                const Type& v0 = vov->v0();
                const Type& v1 = vov->v1();
                const Type& v2 = voc->v();
                const Type c = voc->c();
                const core::operators::operator_type o0 = vov->operation();
                const core::operators::operator_type o1 = operation;
                const core::operators::operator_type o2 = voc->operation();

                details::free_node(*(expr_gen.node_allocator_), branch[0]);
                details::free_node(*(expr_gen.node_allocator_), branch[1]);

                expression_node_ptr result = error_node();

                if (expr_gen.parser_->settings_.strength_reduction_enabled())
                {
                    // (v0 / v1) * (v2 / c) --> (vovovoc) (v0 * v2) / (v1 * c)
                    if ((core::operators::operator_type::div == o0) &&
                        (core::operators::operator_type::mul == o1) &&
                        (core::operators::operator_type::div == o2))
                    {
                        const bool synthesis_result =
                            synthesize_sf4ext_expression::template compile<vtype, vtype, vtype,
                                                                           ctype>(
                                expr_gen, "(t*t)/(t*t)", v0, v2, v1, c, result);

                        core::debug_print(
                            "(v0 / v1) * (v2 / c) --> (vovovoc) (v0 * v2) / (v1 * c)\n");

                        return (synthesis_result) ? result : error_node();
                    }
                    // (v0 / v1) / (v2 / c) --> (vocovov) (v0 * c) / (v1 * v2)
                    if ((core::operators::operator_type::div == o0) &&
                        (core::operators::operator_type::div == o1) &&
                        (core::operators::operator_type::div == o2))
                    {
                        const bool synthesis_result =
                            synthesize_sf4ext_expression::template compile<vtype, ctype, vtype,
                                                                           vtype>(
                                expr_gen, "(t*t)/(t*t)", v0, c, v1, v2, result);

                        core::debug_print(
                            "(v0 / v1) / (v2 / c) --> (vocovov) (v0 * c) / (v1 * v2)\n");

                        return (synthesis_result) ? result : error_node();
                    }
                }

                const bool synthesis_result =
                    synthesize_sf4ext_expression::template compile<T0, T1, T2, T3>(
                        expr_gen, id(expr_gen, o0, o1, o2), v0, v1, v2, c, result);

                if (synthesis_result)
                    return result;

                binary_functor_t f0 = nullptr;
                binary_functor_t f1 = nullptr;
                binary_functor_t f2 = nullptr;

                if (!expr_gen.valid_operator(o0, f0))
                    return error_node();
                else if (!expr_gen.valid_operator(o1, f1))
                    return error_node();
                else if (!expr_gen.valid_operator(o2, f2))
                    return error_node();
                else
                    return node_type::allocate(*(expr_gen.node_allocator_), v0, v1, v2, c, f0, f1,
                                               f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const core::operators::operator_type o0,
                                         const core::operators::operator_type o1,
                                         const core::operators::operator_type o2)
            {
                return core::build_string()
                       << "(t" << expr_gen.to_str(o0) << "t)" << expr_gen.to_str(o1) << "(t"
                       << expr_gen.to_str(o2) << "t)";
            }
        };

        struct synthesize_vovocov_expression0
        {
            using node_type = typename vovocov_t::type0;
            using sf4_type = typename vovocov_t::sf4_type;
            using T0 = typename node_type::T0;
            using T1 = typename node_type::T1;
            using T2 = typename node_type::T2;
            using T3 = typename node_type::T3;

            static inline expression_node_ptr process(
                expression_generator<Type>& expr_gen,
                const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
            {
                // (v0 o0 v1) o1 (c o2 v2)
                const details::vov_base_node<Type>* vov =
                    static_cast<details::vov_base_node<Type>*>(branch[0]);
                const details::cov_base_node<Type>* cov =
                    static_cast<details::cov_base_node<Type>*>(branch[1]);
                const Type& v0 = vov->v0();
                const Type& v1 = vov->v1();
                const Type& v2 = cov->v();
                const Type c = cov->c();
                const core::operators::operator_type o0 = vov->operation();
                const core::operators::operator_type o1 = operation;
                const core::operators::operator_type o2 = cov->operation();

                details::free_node(*(expr_gen.node_allocator_), branch[0]);
                details::free_node(*(expr_gen.node_allocator_), branch[1]);

                expression_node_ptr result = error_node();

                if (expr_gen.parser_->settings_.strength_reduction_enabled())
                {
                    // (v0 / v1) * (c / v2) --> (vocovov) (v0 * c) / (v1 * v2)
                    if ((core::operators::operator_type::div == o0) &&
                        (core::operators::operator_type::mul == o1) &&
                        (core::operators::operator_type::div == o2))
                    {
                        const bool synthesis_result =
                            synthesize_sf4ext_expression::template compile<vtype, ctype, vtype,
                                                                           vtype>(
                                expr_gen, "(t*t)/(t*t)", v0, c, v1, v2, result);

                        core::debug_print(
                            "(v0 / v1) * (c / v2) --> (vocovov) (v0 * c) / (v1 * v2)\n");

                        return (synthesis_result) ? result : error_node();
                    }
                    // (v0 / v1) / (c / v2) --> (vovovoc) (v0 * v2) / (v1 * c)
                    if ((core::operators::operator_type::div == o0) &&
                        (core::operators::operator_type::div == o1) &&
                        (core::operators::operator_type::div == o2))
                    {
                        const bool synthesis_result =
                            synthesize_sf4ext_expression::template compile<vtype, vtype, vtype,
                                                                           ctype>(
                                expr_gen, "(t*t)/(t*t)", v0, v2, v1, c, result);

                        core::debug_print(
                            "(v0 / v1) / (c / v2) --> (vovovoc) (v0 * v2) / (v1 * c)\n");

                        return (synthesis_result) ? result : error_node();
                    }
                }

                const bool synthesis_result =
                    synthesize_sf4ext_expression::template compile<T0, T1, T2, T3>(
                        expr_gen, id(expr_gen, o0, o1, o2), v0, v1, c, v2, result);

                if (synthesis_result)
                    return result;

                binary_functor_t f0 = nullptr;
                binary_functor_t f1 = nullptr;
                binary_functor_t f2 = nullptr;

                if (!expr_gen.valid_operator(o0, f0))
                    return error_node();
                else if (!expr_gen.valid_operator(o1, f1))
                    return error_node();
                else if (!expr_gen.valid_operator(o2, f2))
                    return error_node();
                else
                    return node_type::allocate(*(expr_gen.node_allocator_), v0, v1, c, v2, f0, f1,
                                               f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const core::operators::operator_type o0,
                                         const core::operators::operator_type o1,
                                         const core::operators::operator_type o2)
            {
                return core::build_string()
                       << "(t" << expr_gen.to_str(o0) << "t)" << expr_gen.to_str(o1) << "(t"
                       << expr_gen.to_str(o2) << "t)";
            }
        };

        struct synthesize_vocovov_expression0
        {
            using node_type = typename vocovov_t::type0;
            using sf4_type = typename vocovov_t::sf4_type;
            using T0 = typename node_type::T0;
            using T1 = typename node_type::T1;
            using T2 = typename node_type::T2;
            using T3 = typename node_type::T3;

            static inline expression_node_ptr process(
                expression_generator<Type>& expr_gen,
                const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
            {
                // (v0 o0 c) o1 (v1 o2 v2)
                const details::voc_base_node<Type>* voc =
                    static_cast<details::voc_base_node<Type>*>(branch[0]);
                const details::vov_base_node<Type>* vov =
                    static_cast<details::vov_base_node<Type>*>(branch[1]);
                const Type c = voc->c();
                const Type& v0 = voc->v();
                const Type& v1 = vov->v0();
                const Type& v2 = vov->v1();
                const core::operators::operator_type o0 = voc->operation();
                const core::operators::operator_type o1 = operation;
                const core::operators::operator_type o2 = vov->operation();

                details::free_node(*(expr_gen.node_allocator_), branch[0]);
                details::free_node(*(expr_gen.node_allocator_), branch[1]);

                expression_node_ptr result = error_node();

                if (expr_gen.parser_->settings_.strength_reduction_enabled())
                {
                    // (v0 / c) * (v1 / v2) --> (vovocov) (v0 * v1) / (c * v2)
                    if ((core::operators::operator_type::div == o0) &&
                        (core::operators::operator_type::mul == o1) &&
                        (core::operators::operator_type::div == o2))
                    {
                        const bool synthesis_result =
                            synthesize_sf4ext_expression::template compile<vtype, vtype, ctype,
                                                                           vtype>(
                                expr_gen, "(t*t)/(t*t)", v0, v1, c, v2, result);

                        core::debug_print(
                            "(v0 / c) * (v1 / v2) --> (vovocov) (v0 * v1) / (c * v2)\n");

                        return (synthesis_result) ? result : error_node();
                    }
                    // (v0 / c) / (v1 / v2) --> (vovocov) (v0 * v2) / (c * v1)
                    if ((core::operators::operator_type::div == o0) &&
                        (core::operators::operator_type::div == o1) &&
                        (core::operators::operator_type::div == o2))
                    {
                        const bool synthesis_result =
                            synthesize_sf4ext_expression::template compile<vtype, vtype, ctype,
                                                                           vtype>(
                                expr_gen, "(t*t)/(t*t)", v0, v2, c, v1, result);

                        core::debug_print(
                            "(v0 / c) / (v1 / v2) --> (vovocov) (v0 * v2) / (c * v1)\n");

                        return (synthesis_result) ? result : error_node();
                    }
                }

                const bool synthesis_result =
                    synthesize_sf4ext_expression::template compile<T0, T1, T2, T3>(
                        expr_gen, id(expr_gen, o0, o1, o2), v0, c, v1, v2, result);

                if (synthesis_result)
                    return result;

                binary_functor_t f0 = nullptr;
                binary_functor_t f1 = nullptr;
                binary_functor_t f2 = nullptr;

                if (!expr_gen.valid_operator(o0, f0))
                    return error_node();
                else if (!expr_gen.valid_operator(o1, f1))
                    return error_node();
                else if (!expr_gen.valid_operator(o2, f2))
                    return error_node();
                else
                    return node_type::allocate(*(expr_gen.node_allocator_), v0, c, v1, v2, f0, f1,
                                               f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const core::operators::operator_type o0,
                                         const core::operators::operator_type o1,
                                         const core::operators::operator_type o2)
            {
                return core::build_string()
                       << "(t" << expr_gen.to_str(o0) << "t)" << expr_gen.to_str(o1) << "(t"
                       << expr_gen.to_str(o2) << "t)";
            }
        };

        struct synthesize_covovov_expression0
        {
            using node_type = typename covovov_t::type0;
            using sf4_type = typename covovov_t::sf4_type;
            using T0 = typename node_type::T0;
            using T1 = typename node_type::T1;
            using T2 = typename node_type::T2;
            using T3 = typename node_type::T3;

            static inline expression_node_ptr process(
                expression_generator<Type>& expr_gen,
                const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
            {
                // (c o0 v0) o1 (v1 o2 v2)
                const details::cov_base_node<Type>* cov =
                    static_cast<details::cov_base_node<Type>*>(branch[0]);
                const details::vov_base_node<Type>* vov =
                    static_cast<details::vov_base_node<Type>*>(branch[1]);
                const Type c = cov->c();
                const Type& v0 = cov->v();
                const Type& v1 = vov->v0();
                const Type& v2 = vov->v1();
                const core::operators::operator_type o0 = cov->operation();
                const core::operators::operator_type o1 = operation;
                const core::operators::operator_type o2 = vov->operation();

                details::free_node(*(expr_gen.node_allocator_), branch[0]);
                details::free_node(*(expr_gen.node_allocator_), branch[1]);

                expression_node_ptr result = error_node();

                if (expr_gen.parser_->settings_.strength_reduction_enabled())
                {
                    // (c / v0) * (v1 / v2) --> (covovov) (c * v1) / (v0 * v2)
                    if ((core::operators::operator_type::div == o0) &&
                        (core::operators::operator_type::mul == o1) &&
                        (core::operators::operator_type::div == o2))
                    {
                        const bool synthesis_result =
                            synthesize_sf4ext_expression::template compile<ctype, vtype, vtype,
                                                                           vtype>(
                                expr_gen, "(t*t)/(t*t)", c, v1, v0, v2, result);

                        core::debug_print(
                            "(c / v0) * (v1 / v2) --> (covovov) (c * v1) / (v0 * v2)\n");

                        return (synthesis_result) ? result : error_node();
                    }
                    // (c / v0) / (v1 / v2) --> (covovov) (c * v2) / (v0 * v1)
                    if ((core::operators::operator_type::div == o0) &&
                        (core::operators::operator_type::div == o1) &&
                        (core::operators::operator_type::div == o2))
                    {
                        const bool synthesis_result =
                            synthesize_sf4ext_expression::template compile<ctype, vtype, vtype,
                                                                           vtype>(
                                expr_gen, "(t*t)/(t*t)", c, v2, v0, v1, result);

                        core::debug_print(
                            "(c / v0) / (v1 / v2) --> (covovov) (c * v2) / (v0 * v1)\n");

                        return (synthesis_result) ? result : error_node();
                    }
                }

                const bool synthesis_result =
                    synthesize_sf4ext_expression::template compile<T0, T1, T2, T3>(
                        expr_gen, id(expr_gen, o0, o1, o2), c, v0, v1, v2, result);

                if (synthesis_result)
                    return result;

                binary_functor_t f0 = nullptr;
                binary_functor_t f1 = nullptr;
                binary_functor_t f2 = nullptr;

                if (!expr_gen.valid_operator(o0, f0))
                    return error_node();
                else if (!expr_gen.valid_operator(o1, f1))
                    return error_node();
                else if (!expr_gen.valid_operator(o2, f2))
                    return error_node();
                else
                    return node_type::allocate(*(expr_gen.node_allocator_), c, v0, v1, v2, f0, f1,
                                               f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const core::operators::operator_type o0,
                                         const core::operators::operator_type o1,
                                         const core::operators::operator_type o2)
            {
                return core::build_string()
                       << "(t" << expr_gen.to_str(o0) << "t)" << expr_gen.to_str(o1) << "(t"
                       << expr_gen.to_str(o2) << "t)";
            }
        };

        struct synthesize_covocov_expression0
        {
            using node_type = typename covocov_t::type0;
            using sf4_type = typename covocov_t::sf4_type;
            using T0 = typename node_type::T0;
            using T1 = typename node_type::T1;
            using T2 = typename node_type::T2;
            using T3 = typename node_type::T3;

            static inline expression_node_ptr process(
                expression_generator<Type>& expr_gen,
                const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
            {
                // (c0 o0 v0) o1 (c1 o2 v1)
                const details::cov_base_node<Type>* cov0 =
                    static_cast<details::cov_base_node<Type>*>(branch[0]);
                const details::cov_base_node<Type>* cov1 =
                    static_cast<details::cov_base_node<Type>*>(branch[1]);
                const Type c0 = cov0->c();
                const Type& v0 = cov0->v();
                const Type c1 = cov1->c();
                const Type& v1 = cov1->v();
                const core::operators::operator_type o0 = cov0->operation();
                const core::operators::operator_type o1 = operation;
                const core::operators::operator_type o2 = cov1->operation();

                details::free_node(*(expr_gen.node_allocator_), branch[0]);
                details::free_node(*(expr_gen.node_allocator_), branch[1]);

                expression_node_ptr result = error_node();

                if (expr_gen.parser_->settings_.strength_reduction_enabled())
                {
                    // (c0 + v0) + (c1 + v1) --> (covov) (c0 + c1) + v0 + v1
                    if ((core::operators::operator_type::add == o0) &&
                        (core::operators::operator_type::add == o1) &&
                        (core::operators::operator_type::add == o2))
                    {
                        const bool synthesis_result =
                            synthesize_sf3ext_expression::template compile<ctype, vtype, vtype>(
                                expr_gen, "(t+t)+t", (c0 + c1), v0, v1, result);

                        core::debug_print(
                            "(c0 + v0) + (c1 + v1) --> (covov) (c0 + c1) + v0 + v1\n");

                        return (synthesis_result) ? result : error_node();
                    }
                    // (c0 + v0) - (c1 + v1) --> (covov) (c0 - c1) + v0 - v1
                    else if ((core::operators::operator_type::add == o0) &&
                             (core::operators::operator_type::sub == o1) &&
                             (core::operators::operator_type::add == o2))
                    {
                        const bool synthesis_result =
                            synthesize_sf3ext_expression::template compile<ctype, vtype, vtype>(
                                expr_gen, "(t+t)-t", (c0 - c1), v0, v1, result);

                        core::debug_print(
                            "(c0 + v0) - (c1 + v1) --> (covov) (c0 - c1) + v0 - v1\n");

                        return (synthesis_result) ? result : error_node();
                    }
                    // (c0 - v0) - (c1 - v1) --> (covov) (c0 - c1) - v0 + v1
                    else if ((core::operators::operator_type::sub == o0) &&
                             (core::operators::operator_type::sub == o1) &&
                             (core::operators::operator_type::sub == o2))
                    {
                        const bool synthesis_result =
                            synthesize_sf3ext_expression::template compile<ctype, vtype, vtype>(
                                expr_gen, "(t-t)+t", (c0 - c1), v0, v1, result);

                        core::debug_print(
                            "(c0 - v0) - (c1 - v1) --> (covov) (c0 - c1) - v0 + v1\n");

                        return (synthesis_result) ? result : error_node();
                    }
                    // (c0 * v0) * (c1 * v1) --> (covov) (c0 * c1) * v0 * v1
                    else if ((core::operators::operator_type::mul == o0) &&
                             (core::operators::operator_type::mul == o1) &&
                             (core::operators::operator_type::mul == o2))
                    {
                        const bool synthesis_result =
                            synthesize_sf3ext_expression::template compile<ctype, vtype, vtype>(
                                expr_gen, "(t*t)*t", (c0 * c1), v0, v1, result);

                        core::debug_print(
                            "(c0 * v0) * (c1 * v1) --> (covov) (c0 * c1) * v0 * v1\n");

                        return (synthesis_result) ? result : error_node();
                    }
                    // (c0 * v0) / (c1 * v1) --> (covov) (c0 / c1) * (v0 / v1)
                    else if ((core::operators::operator_type::mul == o0) &&
                             (core::operators::operator_type::div == o1) &&
                             (core::operators::operator_type::mul == o2))
                    {
                        const bool synthesis_result =
                            synthesize_sf3ext_expression::template compile<ctype, vtype, vtype>(
                                expr_gen, "(t*t)/t", (c0 / c1), v0, v1, result);

                        core::debug_print(
                            "(c0 * v0) / (c1 * v1) --> (covov) (c0 / c1) * (v0 / v1)\n");

                        return (synthesis_result) ? result : error_node();
                    }
                    // (c0 / v0) * (c1 / v1) --> (covov) (c0 * c1) / (v0 * v1)
                    else if ((core::operators::operator_type::div == o0) &&
                             (core::operators::operator_type::mul == o1) &&
                             (core::operators::operator_type::div == o2))
                    {
                        const bool synthesis_result =
                            synthesize_sf3ext_expression::template compile<ctype, vtype, vtype>(
                                expr_gen, "t/(t*t)", (c0 * c1), v0, v1, result);

                        core::debug_print(
                            "(c0 / v0) * (c1 / v1) --> (covov) (c0 * c1) / (v0 * v1)\n");

                        return (synthesis_result) ? result : error_node();
                    }
                    // (c0 / v0) / (c1 / v1) --> (covov) ((c0 / c1) * v1) / v0
                    else if ((core::operators::operator_type::div == o0) &&
                             (core::operators::operator_type::div == o1) &&
                             (core::operators::operator_type::div == o2))
                    {
                        const bool synthesis_result =
                            synthesize_sf3ext_expression::template compile<ctype, vtype, vtype>(
                                expr_gen, "(t*t)/t", (c0 / c1), v1, v0, result);

                        core::debug_print(
                            "(c0 / v0) / (c1 / v1) --> (covov) ((c0 / c1) * v1) / v0\n");

                        return (synthesis_result) ? result : error_node();
                    }
                    // (c0 * v0) / (c1 / v1) --> (covov) (c0 / c1) * (v0 * v1)
                    else if ((core::operators::operator_type::mul == o0) &&
                             (core::operators::operator_type::div == o1) &&
                             (core::operators::operator_type::div == o2))
                    {
                        const bool synthesis_result =
                            synthesize_sf3ext_expression::template compile<ctype, vtype, vtype>(
                                expr_gen, "t*(t*t)", (c0 / c1), v0, v1, result);

                        core::debug_print(
                            "(c0 * v0) / (c1 / v1) --> (covov) (c0 / c1) * (v0 * v1)\n");

                        return (synthesis_result) ? result : error_node();
                    }
                    // (c0 / v0) / (c1 * v1) --> (covov) (c0 / c1) / (v0 * v1)
                    else if ((core::operators::operator_type::div == o0) &&
                             (core::operators::operator_type::div == o1) &&
                             (core::operators::operator_type::mul == o2))
                    {
                        const bool synthesis_result =
                            synthesize_sf3ext_expression::template compile<ctype, vtype, vtype>(
                                expr_gen, "t/(t*t)", (c0 / c1), v0, v1, result);

                        core::debug_print(
                            "(c0 / v0) / (c1 * v1) --> (covov) (c0 / c1) / (v0 * v1)\n");

                        return (synthesis_result) ? result : error_node();
                    }
                    // (c * v0) +/- (c * v1) --> (covov) c * (v0 +/- v1)
                    else if ((std::equal_to<T>()(c0, c1)) &&
                             (core::operators::operator_type::mul == o0) &&
                             (core::operators::operator_type::mul == o2) &&
                             ((core::operators::operator_type::add == o1) ||
                              (core::operators::operator_type::sub == o1)))
                    {
                        std::string specfunc;

                        switch (o1)
                        {
                            case core::operators::operator_type::add:
                                specfunc = "t*(t+t)";
                                break;
                            case core::operators::operator_type::sub:
                                specfunc = "t*(t-t)";
                                break;
                            default:
                                return error_node();
                        }

                        const bool synthesis_result =
                            synthesize_sf3ext_expression::template compile<ctype, vtype, vtype>(
                                expr_gen, specfunc, c0, v0, v1, result);

                        core::debug_print("(c * v0) +/- (c * v1) --> (covov) c * (v0 +/- v1)\n");

                        return (synthesis_result) ? result : error_node();
                    }
                }

                const bool synthesis_result =
                    synthesize_sf4ext_expression::template compile<T0, T1, T2, T3>(
                        expr_gen, id(expr_gen, o0, o1, o2), c0, v0, c1, v1, result);

                if (synthesis_result)
                    return result;

                binary_functor_t f0 = nullptr;
                binary_functor_t f1 = nullptr;
                binary_functor_t f2 = nullptr;

                if (!expr_gen.valid_operator(o0, f0))
                    return error_node();
                else if (!expr_gen.valid_operator(o1, f1))
                    return error_node();
                else if (!expr_gen.valid_operator(o2, f2))
                    return error_node();
                else
                    return node_type::allocate(*(expr_gen.node_allocator_), c0, v0, c1, v1, f0, f1,
                                               f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const core::operators::operator_type o0,
                                         const core::operators::operator_type o1,
                                         const core::operators::operator_type o2)
            {
                return core::build_string()
                       << "(t" << expr_gen.to_str(o0) << "t)" << expr_gen.to_str(o1) << "(t"
                       << expr_gen.to_str(o2) << "t)";
            }
        };

        struct synthesize_vocovoc_expression0
        {
            using node_type = typename vocovoc_t::type0;
            using sf4_type = typename vocovoc_t::sf4_type;
            using T0 = typename node_type::T0;
            using T1 = typename node_type::T1;
            using T2 = typename node_type::T2;
            using T3 = typename node_type::T3;

            static inline expression_node_ptr process(
                expression_generator<Type>& expr_gen,
                const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
            {
                // (v0 o0 c0) o1 (v1 o2 c1)
                const details::voc_base_node<Type>* voc0 =
                    static_cast<details::voc_base_node<Type>*>(branch[0]);
                const details::voc_base_node<Type>* voc1 =
                    static_cast<details::voc_base_node<Type>*>(branch[1]);
                const Type c0 = voc0->c();
                const Type& v0 = voc0->v();
                const Type c1 = voc1->c();
                const Type& v1 = voc1->v();
                const core::operators::operator_type o0 = voc0->operation();
                const core::operators::operator_type o1 = operation;
                const core::operators::operator_type o2 = voc1->operation();

                details::free_node(*(expr_gen.node_allocator_), branch[0]);
                details::free_node(*(expr_gen.node_allocator_), branch[1]);

                expression_node_ptr result = error_node();

                if (expr_gen.parser_->settings_.strength_reduction_enabled())
                {
                    // (v0 + c0) + (v1 + c1) --> (covov) (c0 + c1) + v0 + v1
                    if ((core::operators::operator_type::add == o0) &&
                        (core::operators::operator_type::add == o1) &&
                        (core::operators::operator_type::add == o2))
                    {
                        const bool synthesis_result =
                            synthesize_sf3ext_expression::template compile<ctype, vtype, vtype>(
                                expr_gen, "(t+t)+t", (c0 + c1), v0, v1, result);

                        core::debug_print(
                            "(v0 + c0) + (v1 + c1) --> (covov) (c0 + c1) + v0 + v1\n");

                        return (synthesis_result) ? result : error_node();
                    }
                    // (v0 + c0) - (v1 + c1) --> (covov) (c0 - c1) + v0 - v1
                    else if ((core::operators::operator_type::add == o0) &&
                             (core::operators::operator_type::sub == o1) &&
                             (core::operators::operator_type::add == o2))
                    {
                        const bool synthesis_result =
                            synthesize_sf3ext_expression::template compile<ctype, vtype, vtype>(
                                expr_gen, "(t+t)-t", (c0 - c1), v0, v1, result);

                        core::debug_print(
                            "(v0 + c0) - (v1 + c1) --> (covov) (c0 - c1) + v0 - v1\n");

                        return (synthesis_result) ? result : error_node();
                    }
                    // (v0 - c0) - (v1 - c1) --> (covov) (c1 - c0) + v0 - v1
                    else if ((core::operators::operator_type::sub == o0) &&
                             (core::operators::operator_type::sub == o1) &&
                             (core::operators::operator_type::sub == o2))
                    {
                        const bool synthesis_result =
                            synthesize_sf3ext_expression::template compile<ctype, vtype, vtype>(
                                expr_gen, "(t+t)-t", (c1 - c0), v0, v1, result);

                        core::debug_print(
                            "(v0 - c0) - (v1 - c1) --> (covov) (c1 - c0) + v0 - v1\n");

                        return (synthesis_result) ? result : error_node();
                    }
                    // (v0 * c0) * (v1 * c1) --> (covov) (c0 * c1) * v0 * v1
                    else if ((core::operators::operator_type::mul == o0) &&
                             (core::operators::operator_type::mul == o1) &&
                             (core::operators::operator_type::mul == o2))
                    {
                        const bool synthesis_result =
                            synthesize_sf3ext_expression::template compile<ctype, vtype, vtype>(
                                expr_gen, "(t*t)*t", (c0 * c1), v0, v1, result);

                        core::debug_print(
                            "(v0 * c0) * (v1 * c1) --> (covov) (c0 * c1) * v0 * v1\n");

                        return (synthesis_result) ? result : error_node();
                    }
                    // (v0 * c0) / (v1 * c1) --> (covov) (c0 / c1) * (v0 / v1)
                    else if ((core::operators::operator_type::mul == o0) &&
                             (core::operators::operator_type::div == o1) &&
                             (core::operators::operator_type::mul == o2))
                    {
                        const bool synthesis_result =
                            synthesize_sf3ext_expression::template compile<ctype, vtype, vtype>(
                                expr_gen, "(t*t)/t", (c0 / c1), v0, v1, result);

                        core::debug_print(
                            "(v0 * c0) / (v1 * c1) --> (covov) (c0 / c1) * (v0 / v1)\n");

                        return (synthesis_result) ? result : error_node();
                    }
                    // (v0 / c0) * (v1 / c1) --> (covov) (1 / (c0 * c1)) * v0 * v1
                    else if ((core::operators::operator_type::div == o0) &&
                             (core::operators::operator_type::mul == o1) &&
                             (core::operators::operator_type::div == o2))
                    {
                        const bool synthesis_result =
                            synthesize_sf3ext_expression::template compile<ctype, vtype, vtype>(
                                expr_gen, "(t*t)*t", Type(1) / (c0 * c1), v0, v1, result);

                        core::debug_print(
                            "(v0 / c0) * (v1 / c1) --> (covov) (1 / (c0 * c1)) * v0 * v1\n");

                        return (synthesis_result) ? result : error_node();
                    }
                    // (v0 / c0) / (v1 / c1) --> (covov) ((c1 / c0) * v0) / v1
                    else if ((core::operators::operator_type::div == o0) &&
                             (core::operators::operator_type::div == o1) &&
                             (core::operators::operator_type::div == o2))
                    {
                        const bool synthesis_result =
                            synthesize_sf3ext_expression::template compile<ctype, vtype, vtype>(
                                expr_gen, "(t*t)/t", (c1 / c0), v0, v1, result);

                        core::debug_print(
                            "(v0 / c0) / (v1 / c1) --> (covov) ((c1 / c0) * v0) / v1\n");

                        return (synthesis_result) ? result : error_node();
                    }
                    // (v0 * c0) / (v1 / c1) --> (covov) (c0 * c1) * (v0 / v1)
                    else if ((core::operators::operator_type::mul == o0) &&
                             (core::operators::operator_type::div == o1) &&
                             (core::operators::operator_type::div == o2))
                    {
                        const bool synthesis_result =
                            synthesize_sf3ext_expression::template compile<ctype, vtype, vtype>(
                                expr_gen, "t*(t/t)", (c0 * c1), v0, v1, result);

                        core::debug_print(
                            "(v0 * c0) / (v1 / c1) --> (covov) (c0 * c1) * (v0 / v1)\n");

                        return (synthesis_result) ? result : error_node();
                    }
                    // (v0 / c0) / (v1 * c1) --> (covov) (1 / (c0 * c1)) * v0 / v1
                    else if ((core::operators::operator_type::div == o0) &&
                             (core::operators::operator_type::div == o1) &&
                             (core::operators::operator_type::mul == o2))
                    {
                        const bool synthesis_result =
                            synthesize_sf3ext_expression::template compile<ctype, vtype, vtype>(
                                expr_gen, "t*(t/t)", Type(1) / (c0 * c1), v0, v1, result);

                        core::debug_print(
                            "(v0 / c0) / (v1 * c1) --> (covov) (1 / (c0 * c1)) * v0 / v1\n");

                        return (synthesis_result) ? result : error_node();
                    }
                    // (v0 / c0) * (v1 + c1) --> (vocovoc) (v0 * (1 / c0)) * (v1 + c1)
                    else if ((core::operators::operator_type::div == o0) &&
                             (core::operators::operator_type::mul == o1) &&
                             (core::operators::operator_type::add == o2))
                    {
                        const bool synthesis_result =
                            synthesize_sf4ext_expression::template compile<vtype, ctype, vtype,
                                                                           ctype>(
                                expr_gen, "(t*t)*(t+t)", v0, T(1) / c0, v1, c1, result);

                        core::debug_print(
                            "(v0 / c0) * (v1 + c1) --> (vocovoc) (v0 * (1 / c0)) * (v1 + c1)\n");

                        return (synthesis_result) ? result : error_node();
                    }
                    // (v0 / c0) * (v1 - c1) --> (vocovoc) (v0 * (1 / c0)) * (v1 - c1)
                    else if ((core::operators::operator_type::div == o0) &&
                             (core::operators::operator_type::mul == o1) &&
                             (core::operators::operator_type::sub == o2))
                    {
                        const bool synthesis_result =
                            synthesize_sf4ext_expression::template compile<vtype, ctype, vtype,
                                                                           ctype>(
                                expr_gen, "(t*t)*(t-t)", v0, T(1) / c0, v1, c1, result);

                        core::debug_print(
                            "(v0 / c0) * (v1 - c1) --> (vocovoc) (v0 * (1 / c0)) * (v1 - c1)\n");

                        return (synthesis_result) ? result : error_node();
                    }
                    // (v0 * c) +/- (v1 * c) --> (covov) c * (v0 +/- v1)
                    else if ((std::equal_to<T>()(c0, c1)) &&
                             (core::operators::operator_type::mul == o0) &&
                             (core::operators::operator_type::mul == o2) &&
                             ((core::operators::operator_type::add == o1) ||
                              (core::operators::operator_type::sub == o1)))
                    {
                        std::string specfunc;

                        switch (o1)
                        {
                            case core::operators::operator_type::add:
                                specfunc = "t*(t+t)";
                                break;
                            case core::operators::operator_type::sub:
                                specfunc = "t*(t-t)";
                                break;
                            default:
                                return error_node();
                        }

                        const bool synthesis_result =
                            synthesize_sf3ext_expression::template compile<ctype, vtype, vtype>(
                                expr_gen, specfunc, c0, v0, v1, result);

                        core::debug_print("(v0 * c) +/- (v1 * c) --> (covov) c * (v0 +/- v1)\n");

                        return (synthesis_result) ? result : error_node();
                    }
                    // (v0 / c) +/- (v1 / c) --> (vovoc) (v0 +/- v1) / c
                    else if ((std::equal_to<T>()(c0, c1)) &&
                             (core::operators::operator_type::div == o0) &&
                             (core::operators::operator_type::div == o2) &&
                             ((core::operators::operator_type::add == o1) ||
                              (core::operators::operator_type::sub == o1)))
                    {
                        std::string specfunc;

                        switch (o1)
                        {
                            case core::operators::operator_type::add:
                                specfunc = "(t+t)/t";
                                break;
                            case core::operators::operator_type::sub:
                                specfunc = "(t-t)/t";
                                break;
                            default:
                                return error_node();
                        }

                        const bool synthesis_result =
                            synthesize_sf3ext_expression::template compile<vtype, vtype, ctype>(
                                expr_gen, specfunc, v0, v1, c0, result);

                        core::debug_print("(v0 / c) +/- (v1 / c) --> (vovoc) (v0 +/- v1) / c\n");

                        return (synthesis_result) ? result : error_node();
                    }
                }

                const bool synthesis_result =
                    synthesize_sf4ext_expression::template compile<T0, T1, T2, T3>(
                        expr_gen, id(expr_gen, o0, o1, o2), v0, c0, v1, c1, result);

                if (synthesis_result)
                    return result;

                binary_functor_t f0 = nullptr;
                binary_functor_t f1 = nullptr;
                binary_functor_t f2 = nullptr;

                if (!expr_gen.valid_operator(o0, f0))
                    return error_node();
                else if (!expr_gen.valid_operator(o1, f1))
                    return error_node();
                else if (!expr_gen.valid_operator(o2, f2))
                    return error_node();
                else
                    return node_type::allocate(*(expr_gen.node_allocator_), v0, c0, v1, c1, f0, f1,
                                               f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const core::operators::operator_type o0,
                                         const core::operators::operator_type o1,
                                         const core::operators::operator_type o2)
            {
                return core::build_string()
                       << "(t" << expr_gen.to_str(o0) << "t)" << expr_gen.to_str(o1) << "(t"
                       << expr_gen.to_str(o2) << "t)";
            }
        };

        struct synthesize_covovoc_expression0
        {
            using node_type = typename covovoc_t::type0;
            using sf4_type = typename covovoc_t::sf4_type;
            using T0 = typename node_type::T0;
            using T1 = typename node_type::T1;
            using T2 = typename node_type::T2;
            using T3 = typename node_type::T3;

            static inline expression_node_ptr process(
                expression_generator<Type>& expr_gen,
                const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
            {
                // (c0 o0 v0) o1 (v1 o2 c1)
                const details::cov_base_node<Type>* cov =
                    static_cast<details::cov_base_node<Type>*>(branch[0]);
                const details::voc_base_node<Type>* voc =
                    static_cast<details::voc_base_node<Type>*>(branch[1]);
                const Type c0 = cov->c();
                const Type& v0 = cov->v();
                const Type c1 = voc->c();
                const Type& v1 = voc->v();
                const core::operators::operator_type o0 = cov->operation();
                const core::operators::operator_type o1 = operation;
                const core::operators::operator_type o2 = voc->operation();

                details::free_node(*(expr_gen.node_allocator_), branch[0]);
                details::free_node(*(expr_gen.node_allocator_), branch[1]);

                expression_node_ptr result = error_node();

                if (expr_gen.parser_->settings_.strength_reduction_enabled())
                {
                    // (c0 + v0) + (v1 + c1) --> (covov) (c0 + c1) + v0 + v1
                    if ((core::operators::operator_type::add == o0) &&
                        (core::operators::operator_type::add == o1) &&
                        (core::operators::operator_type::add == o2))
                    {
                        const bool synthesis_result =
                            synthesize_sf3ext_expression::template compile<ctype, vtype, vtype>(
                                expr_gen, "(t+t)+t", (c0 + c1), v0, v1, result);

                        core::debug_print(
                            "(c0 + v0) + (v1 + c1) --> (covov) (c0 + c1) + v0 + v1\n");

                        return (synthesis_result) ? result : error_node();
                    }
                    // (c0 + v0) - (v1 + c1) --> (covov) (c0 - c1) + v0 - v1
                    else if ((core::operators::operator_type::add == o0) &&
                             (core::operators::operator_type::sub == o1) &&
                             (core::operators::operator_type::add == o2))
                    {
                        const bool synthesis_result =
                            synthesize_sf3ext_expression::template compile<ctype, vtype, vtype>(
                                expr_gen, "(t+t)-t", (c0 - c1), v0, v1, result);

                        core::debug_print(
                            "(c0 + v0) - (v1 + c1) --> (covov) (c0 - c1) + v0 - v1\n");

                        return (synthesis_result) ? result : error_node();
                    }
                    // (c0 - v0) - (v1 - c1) --> (covov) (c0 + c1) - v0 - v1
                    else if ((core::operators::operator_type::sub == o0) &&
                             (core::operators::operator_type::sub == o1) &&
                             (core::operators::operator_type::sub == o2))
                    {
                        const bool synthesis_result =
                            synthesize_sf3ext_expression::template compile<ctype, vtype, vtype>(
                                expr_gen, "t-(t+t)", (c0 + c1), v0, v1, result);

                        core::debug_print(
                            "(c0 - v0) - (v1 - c1) --> (covov) (c0 + c1) - v0 - v1\n");

                        return (synthesis_result) ? result : error_node();
                    }
                    // (c0 * v0) * (v1 * c1) --> (covov) (c0 * c1) * v0 * v1
                    else if ((core::operators::operator_type::mul == o0) &&
                             (core::operators::operator_type::mul == o1) &&
                             (core::operators::operator_type::mul == o2))
                    {
                        const bool synthesis_result =
                            synthesize_sf3ext_expression::template compile<ctype, vtype, vtype>(
                                expr_gen, "(t*t)*t", (c0 * c1), v0, v1, result);

                        core::debug_print(
                            "(c0 * v0) * (v1 * c1) --> (covov) (c0 * c1) * v0 * v1\n");

                        return (synthesis_result) ? result : error_node();
                    }
                    // (c0 * v0) / (v1 * c1) --> (covov) (c0 / c1) * (v0 / v1)
                    else if ((core::operators::operator_type::mul == o0) &&
                             (core::operators::operator_type::div == o1) &&
                             (core::operators::operator_type::mul == o2))
                    {
                        const bool synthesis_result =
                            synthesize_sf3ext_expression::template compile<ctype, vtype, vtype>(
                                expr_gen, "(t*t)/t", (c0 / c1), v0, v1, result);

                        core::debug_print(
                            "(c0 * v0) / (v1 * c1) --> (covov) (c0 / c1) * (v0 / v1)\n");

                        return (synthesis_result) ? result : error_node();
                    }
                    // (c0 / v0) * (v1 / c1) --> (covov) (c0 / c1) * (v1 / v0)
                    else if ((core::operators::operator_type::div == o0) &&
                             (core::operators::operator_type::mul == o1) &&
                             (core::operators::operator_type::div == o2))
                    {
                        const bool synthesis_result =
                            synthesize_sf3ext_expression::template compile<ctype, vtype, vtype>(
                                expr_gen, "t*(t/t)", (c0 / c1), v1, v0, result);

                        core::debug_print(
                            "(c0 / v0) * (v1 / c1) --> (covov) (c0 / c1) * (v1 / v0)\n");

                        return (synthesis_result) ? result : error_node();
                    }
                    // (c0 / v0) / (v1 / c1) --> (covov) (c0 * c1) / (v0 * v1)
                    else if ((core::operators::operator_type::div == o0) &&
                             (core::operators::operator_type::div == o1) &&
                             (core::operators::operator_type::div == o2))
                    {
                        const bool synthesis_result =
                            synthesize_sf3ext_expression::template compile<ctype, vtype, vtype>(
                                expr_gen, "t/(t*t)", (c0 * c1), v0, v1, result);

                        core::debug_print(
                            "(c0 / v0) / (v1 / c1) --> (covov) (c0 * c1) / (v0 * v1)\n");

                        return (synthesis_result) ? result : error_node();
                    }
                    // (c0 * v0) / (v1 / c1) --> (covov) (c0 * c1) * (v0 / v1)
                    else if ((core::operators::operator_type::mul == o0) &&
                             (core::operators::operator_type::div == o1) &&
                             (core::operators::operator_type::div == o2))
                    {
                        const bool synthesis_result =
                            synthesize_sf3ext_expression::template compile<ctype, vtype, vtype>(
                                expr_gen, "(t*t)/t", (c0 * c1), v0, v1, result);

                        core::debug_print(
                            "(c0 * v0) / (v1 / c1) --> (covov) (c0 * c1) * (v0 / v1)\n");

                        return (synthesis_result) ? result : error_node();
                    }
                    // (c0 / v0) / (v1 * c1) --> (covov) (c0 / c1) / (v0 * v1)
                    else if ((core::operators::operator_type::div == o0) &&
                             (core::operators::operator_type::div == o1) &&
                             (core::operators::operator_type::mul == o2))
                    {
                        const bool synthesis_result =
                            synthesize_sf3ext_expression::template compile<ctype, vtype, vtype>(
                                expr_gen, "t/(t*t)", (c0 / c1), v0, v1, result);

                        core::debug_print(
                            "(c0 / v0) / (v1 * c1) --> (covov) (c0 / c1) / (v0 * v1)\n");

                        return (synthesis_result) ? result : error_node();
                    }
                    // (c * v0) +/- (v1 * c) --> (covov) c * (v0 +/- v1)
                    else if ((std::equal_to<T>()(c0, c1)) &&
                             (core::operators::operator_type::mul == o0) &&
                             (core::operators::operator_type::mul == o2) &&
                             ((core::operators::operator_type::add == o1) ||
                              (core::operators::operator_type::sub == o1)))
                    {
                        std::string specfunc;

                        switch (o1)
                        {
                            case core::operators::operator_type::add:
                                specfunc = "t*(t+t)";
                                break;
                            case core::operators::operator_type::sub:
                                specfunc = "t*(t-t)";
                                break;
                            default:
                                return error_node();
                        }

                        const bool synthesis_result =
                            synthesize_sf3ext_expression::template compile<ctype, vtype, vtype>(
                                expr_gen, specfunc, c0, v0, v1, result);

                        core::debug_print("(c * v0) +/- (v1 * c) --> (covov) c * (v0 +/- v1)\n");

                        return (synthesis_result) ? result : error_node();
                    }
                }

                const bool synthesis_result =
                    synthesize_sf4ext_expression::template compile<T0, T1, T2, T3>(
                        expr_gen, id(expr_gen, o0, o1, o2), c0, v0, v1, c1, result);

                if (synthesis_result)
                    return result;

                binary_functor_t f0 = nullptr;
                binary_functor_t f1 = nullptr;
                binary_functor_t f2 = nullptr;

                if (!expr_gen.valid_operator(o0, f0))
                    return error_node();
                else if (!expr_gen.valid_operator(o1, f1))
                    return error_node();
                else if (!expr_gen.valid_operator(o2, f2))
                    return error_node();
                else
                    return node_type::allocate(*(expr_gen.node_allocator_), c0, v0, v1, c1, f0, f1,
                                               f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const core::operators::operator_type o0,
                                         const core::operators::operator_type o1,
                                         const core::operators::operator_type o2)
            {
                return core::build_string()
                       << "(t" << expr_gen.to_str(o0) << "t)" << expr_gen.to_str(o1) << "(t"
                       << expr_gen.to_str(o2) << "t)";
            }
        };

        struct synthesize_vococov_expression0
        {
            using node_type = typename vococov_t::type0;
            using sf4_type = typename vococov_t::sf4_type;
            using T0 = typename node_type::T0;
            using T1 = typename node_type::T1;
            using T2 = typename node_type::T2;
            using T3 = typename node_type::T3;

            static inline expression_node_ptr process(
                expression_generator<Type>& expr_gen,
                const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
            {
                // (v0 o0 c0) o1 (c1 o2 v1)
                const details::voc_base_node<Type>* voc =
                    static_cast<details::voc_base_node<Type>*>(branch[0]);
                const details::cov_base_node<Type>* cov =
                    static_cast<details::cov_base_node<Type>*>(branch[1]);
                const Type c0 = voc->c();
                const Type& v0 = voc->v();
                const Type c1 = cov->c();
                const Type& v1 = cov->v();
                const core::operators::operator_type o0 = voc->operation();
                const core::operators::operator_type o1 = operation;
                const core::operators::operator_type o2 = cov->operation();

                details::free_node(*(expr_gen.node_allocator_), branch[0]);
                details::free_node(*(expr_gen.node_allocator_), branch[1]);

                expression_node_ptr result = error_node();

                if (expr_gen.parser_->settings_.strength_reduction_enabled())
                {
                    // (v0 + c0) + (c1 + v1) --> (covov) (c0 + c1) + v0 + v1
                    if ((core::operators::operator_type::add == o0) &&
                        (core::operators::operator_type::add == o1) &&
                        (core::operators::operator_type::add == o2))
                    {
                        const bool synthesis_result =
                            synthesize_sf3ext_expression::template compile<ctype, vtype, vtype>(
                                expr_gen, "(t+t)+t", (c0 + c1), v0, v1, result);

                        core::debug_print(
                            "(v0 + c0) + (c1 + v1) --> (covov) (c0 + c1) + v0 + v1\n");

                        return (synthesis_result) ? result : error_node();
                    }
                    // (v0 + c0) - (c1 + v1) --> (covov) (c0 - c1) + v0 - v1
                    else if ((core::operators::operator_type::add == o0) &&
                             (core::operators::operator_type::sub == o1) &&
                             (core::operators::operator_type::add == o2))
                    {
                        const bool synthesis_result =
                            synthesize_sf3ext_expression::template compile<ctype, vtype, vtype>(
                                expr_gen, "(t+t)-t", (c0 - c1), v0, v1, result);

                        core::debug_print(
                            "(v0 + c0) - (c1 + v1) --> (covov) (c0 - c1) + v0 - v1\n");

                        return (synthesis_result) ? result : error_node();
                    }
                    // (v0 - c0) - (c1 - v1) --> (vovoc) v0 + v1 - (c1 + c0)
                    else if ((core::operators::operator_type::sub == o0) &&
                             (core::operators::operator_type::sub == o1) &&
                             (core::operators::operator_type::sub == o2))
                    {
                        const bool synthesis_result =
                            synthesize_sf3ext_expression::template compile<vtype, vtype, ctype>(
                                expr_gen, "(t+t)-t", v0, v1, (c1 + c0), result);

                        core::debug_print(
                            "(v0 - c0) - (c1 - v1) --> (vovoc) v0 + v1 - (c1 + c0)\n");

                        return (synthesis_result) ? result : error_node();
                    }
                    // (v0 * c0) * (c1 * v1) --> (covov) (c0 * c1) * v0 * v1
                    else if ((core::operators::operator_type::mul == o0) &&
                             (core::operators::operator_type::mul == o1) &&
                             (core::operators::operator_type::mul == o2))
                    {
                        const bool synthesis_result =
                            synthesize_sf3ext_expression::template compile<ctype, vtype, vtype>(
                                expr_gen, "(t*t)*t", (c0 * c1), v0, v1, result);

                        core::debug_print(
                            "(v0 * c0) * (c1 * v1) --> (covov) (c0 * c1) * v0 * v1\n");

                        return (synthesis_result) ? result : error_node();
                    }
                    // (v0 * c0) / (c1 * v1) --> (covov) (c0 / c1) * (v0 * v1)
                    else if ((core::operators::operator_type::mul == o0) &&
                             (core::operators::operator_type::div == o1) &&
                             (core::operators::operator_type::mul == o2))
                    {
                        const bool synthesis_result =
                            synthesize_sf3ext_expression::template compile<ctype, vtype, vtype>(
                                expr_gen, "(t*t)/t", (c0 / c1), v0, v1, result);

                        core::debug_print(
                            "(v0 * c0) / (c1 * v1) --> (covov) (c0 / c1) * (v0 * v1)\n");

                        return (synthesis_result) ? result : error_node();
                    }
                    // (v0 / c0) * (c1 / v1) --> (covov) (c1 / c0) * (v0 / v1)
                    else if ((core::operators::operator_type::div == o0) &&
                             (core::operators::operator_type::mul == o1) &&
                             (core::operators::operator_type::div == o2))
                    {
                        const bool synthesis_result =
                            synthesize_sf3ext_expression::template compile<ctype, vtype, vtype>(
                                expr_gen, "(t*t)/t", (c1 / c0), v0, v1, result);

                        core::debug_print(
                            "(v0 / c0) * (c1 / v1) --> (covov) (c1 / c0) * (v0 / v1)\n");

                        return (synthesis_result) ? result : error_node();
                    }
                    // (v0 * c0) / (c1 / v1) --> (covov) (c0 / c1) * (v0 * v1)
                    else if ((core::operators::operator_type::mul == o0) &&
                             (core::operators::operator_type::div == o1) &&
                             (core::operators::operator_type::div == o2))
                    {
                        const bool synthesis_result =
                            synthesize_sf3ext_expression::template compile<ctype, vtype, vtype>(
                                expr_gen, "(t*t)*t", (c0 / c1), v0, v1, result);

                        core::debug_print(
                            "(v0 * c0) / (c1 / v1) --> (covov) (c0 / c1) * (v0 * v1)\n");

                        return (synthesis_result) ? result : error_node();
                    }
                    // (v0 / c0) / (c1 * v1) --> (covov) (1 / (c0 * c1)) * (v0 / v1)
                    else if ((core::operators::operator_type::div == o0) &&
                             (core::operators::operator_type::div == o1) &&
                             (core::operators::operator_type::mul == o2))
                    {
                        const bool synthesis_result =
                            synthesize_sf3ext_expression::template compile<ctype, vtype, vtype>(
                                expr_gen, "(t*t)/t", Type(1) / (c0 * c1), v0, v1, result);

                        core::debug_print(
                            "(v0 / c0) / (c1 * v1) --> (covov) (1 / (c0 * c1)) * (v0 / v1)\n");

                        return (synthesis_result) ? result : error_node();
                    }
                    // (v0 / c0) / (c1 / v1) --> (vovoc) (v0 * v1) * (1 / (c0 * c1))
                    else if ((core::operators::operator_type::div == o0) &&
                             (core::operators::operator_type::div == o1) &&
                             (core::operators::operator_type::div == o2))
                    {
                        const bool synthesis_result =
                            synthesize_sf3ext_expression::template compile<vtype, vtype, ctype>(
                                expr_gen, "(t*t)*t", v0, v1, Type(1) / (c0 * c1), result);

                        core::debug_print(
                            "(v0 / c0) / (c1 / v1) --> (vovoc) (v0 * v1) * (1 / (c0 * c1))\n");

                        return (synthesis_result) ? result : error_node();
                    }
                    // (v0 * c) +/- (c * v1) --> (covov) c * (v0 +/- v1)
                    else if ((std::equal_to<T>()(c0, c1)) &&
                             (core::operators::operator_type::mul == o0) &&
                             (core::operators::operator_type::mul == o2) &&
                             ((core::operators::operator_type::add == o1) ||
                              (core::operators::operator_type::sub == o1)))
                    {
                        std::string specfunc;

                        switch (o1)
                        {
                            case core::operators::operator_type::add:
                                specfunc = "t*(t+t)";
                                break;
                            case core::operators::operator_type::sub:
                                specfunc = "t*(t-t)";
                                break;
                            default:
                                return error_node();
                        }

                        const bool synthesis_result =
                            synthesize_sf3ext_expression::template compile<ctype, vtype, vtype>(
                                expr_gen, specfunc, c0, v0, v1, result);

                        core::debug_print("(v0 * c) +/- (c * v1) --> (covov) c * (v0 +/- v1)\n");

                        return (synthesis_result) ? result : error_node();
                    }
                }

                const bool synthesis_result =
                    synthesize_sf4ext_expression::template compile<T0, T1, T2, T3>(
                        expr_gen, id(expr_gen, o0, o1, o2), v0, c0, c1, v1, result);

                if (synthesis_result)
                    return result;

                binary_functor_t f0 = nullptr;
                binary_functor_t f1 = nullptr;
                binary_functor_t f2 = nullptr;

                if (!expr_gen.valid_operator(o0, f0))
                    return error_node();
                else if (!expr_gen.valid_operator(o1, f1))
                    return error_node();
                else if (!expr_gen.valid_operator(o2, f2))
                    return error_node();
                else
                    return node_type::allocate(*(expr_gen.node_allocator_), v0, c0, c1, v1, f0, f1,
                                               f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const core::operators::operator_type o0,
                                         const core::operators::operator_type o1,
                                         const core::operators::operator_type o2)
            {
                return core::build_string()
                       << "(t" << expr_gen.to_str(o0) << "t)" << expr_gen.to_str(o1) << "(t"
                       << expr_gen.to_str(o2) << "t)";
            }
        };

        struct synthesize_vovovov_expression1
        {
            using node_type = typename vovovov_t::type1;
            using sf4_type = typename vovovov_t::sf4_type;
            using T0 = typename node_type::T0;
            using T1 = typename node_type::T1;
            using T2 = typename node_type::T2;
            using T3 = typename node_type::T3;

            static inline expression_node_ptr process(
                expression_generator<Type>& expr_gen,
                const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
            {
                // v0 o0 (v1 o1 (v2 o2 v3))
                using lcl_vovov_t = typename synthesize_vovov_expression1::node_type;

                const lcl_vovov_t* vovov = static_cast<const lcl_vovov_t*>(branch[1]);
                const Type& v0 = static_cast<details::variable_node<Type>*>(branch[0])->ref();
                const Type& v1 = vovov->t0();
                const Type& v2 = vovov->t1();
                const Type& v3 = vovov->t2();
                const core::operators::operator_type o0 = operation;
                const core::operators::operator_type o1 = expr_gen.get_operator(vovov->f0());
                const core::operators::operator_type o2 = expr_gen.get_operator(vovov->f1());

                binary_functor_t f0 = nullptr;
                binary_functor_t f1 = vovov->f0();
                binary_functor_t f2 = vovov->f1();

                details::free_node(*(expr_gen.node_allocator_), branch[1]);

                expression_node_ptr result = error_node();

                const bool synthesis_result =
                    synthesize_sf4ext_expression::template compile<T0, T1, T2, T3>(
                        expr_gen, id(expr_gen, o0, o1, o2), v0, v1, v2, v3, result);

                if (synthesis_result)
                    return result;
                else if (!expr_gen.valid_operator(o0, f0))
                    return error_node();

                core::debug_print("v0 o0 (v1 o1 (v2 o2 v3))\n");

                return node_type::allocate(*(expr_gen.node_allocator_), v0, v1, v2, v3, f0, f1, f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const core::operators::operator_type o0,
                                         const core::operators::operator_type o1,
                                         const core::operators::operator_type o2)
            {
                return core::build_string()
                       << "t" << expr_gen.to_str(o0) << "(t" << expr_gen.to_str(o1) << "(t"
                       << expr_gen.to_str(o2) << "t))";
            }
        };

        struct synthesize_vovovoc_expression1
        {
            using node_type = typename vovovoc_t::type1;
            using sf4_type = typename vovovoc_t::sf4_type;
            using T0 = typename node_type::T0;
            using T1 = typename node_type::T1;
            using T2 = typename node_type::T2;
            using T3 = typename node_type::T3;

            static inline expression_node_ptr process(
                expression_generator<Type>& expr_gen,
                const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
            {
                // v0 o0 (v1 o1 (v2 o2 c))
                using lcl_vovoc_t = typename synthesize_vovoc_expression1::node_type;

                const lcl_vovoc_t* vovoc = static_cast<const lcl_vovoc_t*>(branch[1]);
                const Type& v0 = static_cast<details::variable_node<Type>*>(branch[0])->ref();
                const Type& v1 = vovoc->t0();
                const Type& v2 = vovoc->t1();
                const Type c = vovoc->t2();
                const core::operators::operator_type o0 = operation;
                const core::operators::operator_type o1 = expr_gen.get_operator(vovoc->f0());
                const core::operators::operator_type o2 = expr_gen.get_operator(vovoc->f1());

                binary_functor_t f0 = nullptr;
                binary_functor_t f1 = vovoc->f0();
                binary_functor_t f2 = vovoc->f1();

                details::free_node(*(expr_gen.node_allocator_), branch[1]);

                expression_node_ptr result = error_node();

                const bool synthesis_result =
                    synthesize_sf4ext_expression::template compile<T0, T1, T2, T3>(
                        expr_gen, id(expr_gen, o0, o1, o2), v0, v1, v2, c, result);

                if (synthesis_result)
                    return result;
                else if (!expr_gen.valid_operator(o0, f0))
                    return error_node();

                core::debug_print("v0 o0 (v1 o1 (v2 o2 c))\n");

                return node_type::allocate(*(expr_gen.node_allocator_), v0, v1, v2, c, f0, f1, f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const core::operators::operator_type o0,
                                         const core::operators::operator_type o1,
                                         const core::operators::operator_type o2)
            {
                return core::build_string()
                       << "t" << expr_gen.to_str(o0) << "(t" << expr_gen.to_str(o1) << "(t"
                       << expr_gen.to_str(o2) << "t))";
            }
        };

        struct synthesize_vovocov_expression1
        {
            using node_type = typename vovocov_t::type1;
            using sf4_type = typename vovocov_t::sf4_type;
            using T0 = typename node_type::T0;
            using T1 = typename node_type::T1;
            using T2 = typename node_type::T2;
            using T3 = typename node_type::T3;

            static inline expression_node_ptr process(
                expression_generator<Type>& expr_gen,
                const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
            {
                // v0 o0 (v1 o1 (c o2 v2))
                using lcl_vocov_t = typename synthesize_vocov_expression1::node_type;

                const lcl_vocov_t* vocov = static_cast<const lcl_vocov_t*>(branch[1]);
                const Type& v0 = static_cast<details::variable_node<Type>*>(branch[0])->ref();
                const Type& v1 = vocov->t0();
                const Type c = vocov->t1();
                const Type& v2 = vocov->t2();
                const core::operators::operator_type o0 = operation;
                const core::operators::operator_type o1 = expr_gen.get_operator(vocov->f0());
                const core::operators::operator_type o2 = expr_gen.get_operator(vocov->f1());

                binary_functor_t f0 = nullptr;
                binary_functor_t f1 = vocov->f0();
                binary_functor_t f2 = vocov->f1();

                details::free_node(*(expr_gen.node_allocator_), branch[1]);

                expression_node_ptr result = error_node();

                const bool synthesis_result =
                    synthesize_sf4ext_expression::template compile<T0, T1, T2, T3>(
                        expr_gen, id(expr_gen, o0, o1, o2), v0, v1, c, v2, result);

                if (synthesis_result)
                    return result;
                if (!expr_gen.valid_operator(o0, f0))
                    return error_node();

                core::debug_print("v0 o0 (v1 o1 (c o2 v2))\n");

                return node_type::allocate(*(expr_gen.node_allocator_), v0, v1, c, v2, f0, f1, f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const core::operators::operator_type o0,
                                         const core::operators::operator_type o1,
                                         const core::operators::operator_type o2)
            {
                return core::build_string()
                       << "t" << expr_gen.to_str(o0) << "(t" << expr_gen.to_str(o1) << "(t"
                       << expr_gen.to_str(o2) << "t))";
            }
        };

        struct synthesize_vocovov_expression1
        {
            using node_type = typename vocovov_t::type1;
            using sf4_type = typename vocovov_t::sf4_type;
            using T0 = typename node_type::T0;
            using T1 = typename node_type::T1;
            using T2 = typename node_type::T2;
            using T3 = typename node_type::T3;

            static inline expression_node_ptr process(
                expression_generator<Type>& expr_gen,
                const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
            {
                // v0 o0 (c o1 (v1 o2 v2))
                using lcl_covov_t = typename synthesize_covov_expression1::node_type;

                const lcl_covov_t* covov = static_cast<const lcl_covov_t*>(branch[1]);
                const Type& v0 = static_cast<details::variable_node<Type>*>(branch[0])->ref();
                const Type c = covov->t0();
                const Type& v1 = covov->t1();
                const Type& v2 = covov->t2();
                const core::operators::operator_type o0 = operation;
                const core::operators::operator_type o1 = expr_gen.get_operator(covov->f0());
                const core::operators::operator_type o2 = expr_gen.get_operator(covov->f1());

                binary_functor_t f0 = nullptr;
                binary_functor_t f1 = covov->f0();
                binary_functor_t f2 = covov->f1();

                details::free_node(*(expr_gen.node_allocator_), branch[1]);

                expression_node_ptr result = error_node();

                const bool synthesis_result =
                    synthesize_sf4ext_expression::template compile<T0, T1, T2, T3>(
                        expr_gen, id(expr_gen, o0, o1, o2), v0, c, v1, v2, result);

                if (synthesis_result)
                    return result;
                else if (!expr_gen.valid_operator(o0, f0))
                    return error_node();

                core::debug_print("v0 o0 (c o1 (v1 o2 v2))\n");

                return node_type::allocate(*(expr_gen.node_allocator_), v0, c, v1, v2, f0, f1, f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const core::operators::operator_type o0,
                                         const core::operators::operator_type o1,
                                         const core::operators::operator_type o2)
            {
                return core::build_string()
                       << "t" << expr_gen.to_str(o0) << "(t" << expr_gen.to_str(o1) << "(t"
                       << expr_gen.to_str(o2) << "t))";
            }
        };

        struct synthesize_covovov_expression1
        {
            using node_type = typename covovov_t::type1;
            using sf4_type = typename covovov_t::sf4_type;
            using T0 = typename node_type::T0;
            using T1 = typename node_type::T1;
            using T2 = typename node_type::T2;
            using T3 = typename node_type::T3;

            static inline expression_node_ptr process(
                expression_generator<Type>& expr_gen,
                const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
            {
                // c o0 (v0 o1 (v1 o2 v2))
                using lcl_vovov_t = typename synthesize_vovov_expression1::node_type;

                const lcl_vovov_t* vovov = static_cast<const lcl_vovov_t*>(branch[1]);
                const Type c = static_cast<details::literal_node<Type>*>(branch[0])->value();
                const Type& v0 = vovov->t0();
                const Type& v1 = vovov->t1();
                const Type& v2 = vovov->t2();
                const core::operators::operator_type o0 = operation;
                const core::operators::operator_type o1 = expr_gen.get_operator(vovov->f0());
                const core::operators::operator_type o2 = expr_gen.get_operator(vovov->f1());

                binary_functor_t f0 = nullptr;
                binary_functor_t f1 = vovov->f0();
                binary_functor_t f2 = vovov->f1();

                details::free_node(*(expr_gen.node_allocator_), branch[0]);
                details::free_node(*(expr_gen.node_allocator_), branch[1]);

                expression_node_ptr result = error_node();

                const bool synthesis_result =
                    synthesize_sf4ext_expression::template compile<T0, T1, T2, T3>(
                        expr_gen, id(expr_gen, o0, o1, o2), c, v0, v1, v2, result);

                if (synthesis_result)
                    return result;
                if (!expr_gen.valid_operator(o0, f0))
                    return error_node();

                core::debug_print("c o0 (v0 o1 (v1 o2 v2))\n");

                return node_type::allocate(*(expr_gen.node_allocator_), c, v0, v1, v2, f0, f1, f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const core::operators::operator_type o0,
                                         const core::operators::operator_type o1,
                                         const core::operators::operator_type o2)
            {
                return core::build_string()
                       << "t" << expr_gen.to_str(o0) << "(t" << expr_gen.to_str(o1) << "(t"
                       << expr_gen.to_str(o2) << "t))";
            }
        };

        struct synthesize_covocov_expression1
        {
            using node_type = typename covocov_t::type1;
            using sf4_type = typename covocov_t::sf4_type;
            using T0 = typename node_type::T0;
            using T1 = typename node_type::T1;
            using T2 = typename node_type::T2;
            using T3 = typename node_type::T3;

            static inline expression_node_ptr process(
                expression_generator<Type>& expr_gen,
                const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
            {
                // c0 o0 (v0 o1 (c1 o2 v1))
                using lcl_vocov_t = typename synthesize_vocov_expression1::node_type;

                const lcl_vocov_t* vocov = static_cast<const lcl_vocov_t*>(branch[1]);
                const Type c0 = static_cast<details::literal_node<Type>*>(branch[0])->value();
                const Type& v0 = vocov->t0();
                const Type c1 = vocov->t1();
                const Type& v1 = vocov->t2();
                const core::operators::operator_type o0 = operation;
                const core::operators::operator_type o1 = expr_gen.get_operator(vocov->f0());
                const core::operators::operator_type o2 = expr_gen.get_operator(vocov->f1());

                binary_functor_t f0 = nullptr;
                binary_functor_t f1 = vocov->f0();
                binary_functor_t f2 = vocov->f1();

                details::free_node(*(expr_gen.node_allocator_), branch[0]);
                details::free_node(*(expr_gen.node_allocator_), branch[1]);

                expression_node_ptr result = error_node();

                const bool synthesis_result =
                    synthesize_sf4ext_expression::template compile<T0, T1, T2, T3>(
                        expr_gen, id(expr_gen, o0, o1, o2), c0, v0, c1, v1, result);

                if (synthesis_result)
                    return result;
                else if (!expr_gen.valid_operator(o0, f0))
                    return error_node();

                core::debug_print("c0 o0 (v0 o1 (c1 o2 v1))\n");

                return node_type::allocate(*(expr_gen.node_allocator_), c0, v0, c1, v1, f0, f1, f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const core::operators::operator_type o0,
                                         const core::operators::operator_type o1,
                                         const core::operators::operator_type o2)
            {
                return core::build_string()
                       << "t" << expr_gen.to_str(o0) << "(t" << expr_gen.to_str(o1) << "(t"
                       << expr_gen.to_str(o2) << "t))";
            }
        };

        struct synthesize_vocovoc_expression1
        {
            using node_type = typename vocovoc_t::type1;
            using sf4_type = typename vocovoc_t::sf4_type;
            using T0 = typename node_type::T0;
            using T1 = typename node_type::T1;
            using T2 = typename node_type::T2;
            using T3 = typename node_type::T3;

            static inline expression_node_ptr process(
                expression_generator<Type>& expr_gen,
                const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
            {
                // v0 o0 (c0 o1 (v1 o2 c2))
                using lcl_covoc_t = typename synthesize_covoc_expression1::node_type;

                const lcl_covoc_t* covoc = static_cast<const lcl_covoc_t*>(branch[1]);
                const Type& v0 = static_cast<details::variable_node<Type>*>(branch[0])->ref();
                const Type c0 = covoc->t0();
                const Type& v1 = covoc->t1();
                const Type c1 = covoc->t2();
                const core::operators::operator_type o0 = operation;
                const core::operators::operator_type o1 = expr_gen.get_operator(covoc->f0());
                const core::operators::operator_type o2 = expr_gen.get_operator(covoc->f1());

                binary_functor_t f0 = nullptr;
                binary_functor_t f1 = covoc->f0();
                binary_functor_t f2 = covoc->f1();

                details::free_node(*(expr_gen.node_allocator_), branch[1]);

                expression_node_ptr result = error_node();

                const bool synthesis_result =
                    synthesize_sf4ext_expression::template compile<T0, T1, T2, T3>(
                        expr_gen, id(expr_gen, o0, o1, o2), v0, c0, v1, c1, result);

                if (synthesis_result)
                    return result;
                else if (!expr_gen.valid_operator(o0, f0))
                    return error_node();

                core::debug_print("v0 o0 (c0 o1 (v1 o2 c2))\n");

                return node_type::allocate(*(expr_gen.node_allocator_), v0, c0, v1, c1, f0, f1, f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const core::operators::operator_type o0,
                                         const core::operators::operator_type o1,
                                         const core::operators::operator_type o2)
            {
                return core::build_string()
                       << "t" << expr_gen.to_str(o0) << "(t" << expr_gen.to_str(o1) << "(t"
                       << expr_gen.to_str(o2) << "t))";
            }
        };

        struct synthesize_covovoc_expression1
        {
            using node_type = typename covovoc_t::type1;
            using sf4_type = typename covovoc_t::sf4_type;
            using T0 = typename node_type::T0;
            using T1 = typename node_type::T1;
            using T2 = typename node_type::T2;
            using T3 = typename node_type::T3;
            static inline expression_node_ptr process(
                expression_generator<Type>& expr_gen,
                const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
            {
                // c0 o0 (v0 o1 (v1 o2 c1))
                using lcl_vovoc_t = typename synthesize_vovoc_expression1::node_type;

                const lcl_vovoc_t* vovoc = static_cast<const lcl_vovoc_t*>(branch[1]);
                const Type c0 = static_cast<details::literal_node<Type>*>(branch[0])->value();
                const Type& v0 = vovoc->t0();
                const Type& v1 = vovoc->t1();
                const Type c1 = vovoc->t2();
                const core::operators::operator_type o0 = operation;
                const core::operators::operator_type o1 = expr_gen.get_operator(vovoc->f0());
                const core::operators::operator_type o2 = expr_gen.get_operator(vovoc->f1());

                binary_functor_t f0 = nullptr;
                binary_functor_t f1 = vovoc->f0();
                binary_functor_t f2 = vovoc->f1();

                details::free_node(*(expr_gen.node_allocator_), branch[0]);
                details::free_node(*(expr_gen.node_allocator_), branch[1]);

                expression_node_ptr result = error_node();

                const bool synthesis_result =
                    synthesize_sf4ext_expression::template compile<T0, T1, T2, T3>(
                        expr_gen, id(expr_gen, o0, o1, o2), c0, v0, v1, c1, result);

                if (synthesis_result)
                    return result;
                else if (!expr_gen.valid_operator(o0, f0))
                    return error_node();

                core::debug_print("c0 o0 (v0 o1 (v1 o2 c1))\n");

                return node_type::allocate(*(expr_gen.node_allocator_), c0, v0, v1, c1, f0, f1, f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const core::operators::operator_type o0,
                                         const core::operators::operator_type o1,
                                         const core::operators::operator_type o2)
            {
                return core::build_string()
                       << "t" << expr_gen.to_str(o0) << "(t" << expr_gen.to_str(o1) << "(t"
                       << expr_gen.to_str(o2) << "t))";
            }
        };

        struct synthesize_vococov_expression1
        {
            using node_type = typename vococov_t::type1;
            using sf4_type = typename vococov_t::sf4_type;
            using T0 = typename node_type::T0;
            using T1 = typename node_type::T1;
            using T2 = typename node_type::T2;
            using T3 = typename node_type::T3;

            static inline expression_node_ptr process(
                expression_generator<Type>& expr_gen,
                const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
            {
                // v0 o0 (c0 o1 (c1 o2 v1))
                using lcl_cocov_t = typename synthesize_cocov_expression1::node_type;

                const lcl_cocov_t* cocov = static_cast<const lcl_cocov_t*>(branch[1]);
                const Type& v0 = static_cast<details::variable_node<Type>*>(branch[0])->ref();
                const Type c0 = cocov->t0();
                const Type c1 = cocov->t1();
                const Type& v1 = cocov->t2();
                const core::operators::operator_type o0 = operation;
                const core::operators::operator_type o1 = expr_gen.get_operator(cocov->f0());
                const core::operators::operator_type o2 = expr_gen.get_operator(cocov->f1());

                binary_functor_t f0 = nullptr;
                binary_functor_t f1 = cocov->f0();
                binary_functor_t f2 = cocov->f1();

                details::free_node(*(expr_gen.node_allocator_), branch[1]);

                expression_node_ptr result = error_node();

                const bool synthesis_result =
                    synthesize_sf4ext_expression::template compile<T0, T1, T2, T3>(
                        expr_gen, id(expr_gen, o0, o1, o2), v0, c0, c1, v1, result);

                if (synthesis_result)
                    return result;
                else if (!expr_gen.valid_operator(o0, f0))
                    return error_node();

                core::debug_print("v0 o0 (c0 o1 (c1 o2 v1))\n");

                return node_type::allocate(*(expr_gen.node_allocator_), v0, c0, c1, v1, f0, f1, f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const core::operators::operator_type o0,
                                         const core::operators::operator_type o1,
                                         const core::operators::operator_type o2)
            {
                return core::build_string()
                       << "t" << expr_gen.to_str(o0) << "(t" << expr_gen.to_str(o1) << "(t"
                       << expr_gen.to_str(o2) << "t))";
            }
        };

        struct synthesize_vovovov_expression2
        {
            using node_type = typename vovovov_t::type2;
            using sf4_type = typename vovovov_t::sf4_type;
            using T0 = typename node_type::T0;
            using T1 = typename node_type::T1;
            using T2 = typename node_type::T2;
            using T3 = typename node_type::T3;

            static inline expression_node_ptr process(
                expression_generator<Type>& expr_gen,
                const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
            {
                // v0 o0 ((v1 o1 v2) o2 v3)
                using lcl_vovov_t = typename synthesize_vovov_expression0::node_type;

                const lcl_vovov_t* vovov = static_cast<const lcl_vovov_t*>(branch[1]);
                const Type& v0 = static_cast<details::variable_node<Type>*>(branch[0])->ref();
                const Type& v1 = vovov->t0();
                const Type& v2 = vovov->t1();
                const Type& v3 = vovov->t2();
                const core::operators::operator_type o0 = operation;
                const core::operators::operator_type o1 = expr_gen.get_operator(vovov->f0());
                const core::operators::operator_type o2 = expr_gen.get_operator(vovov->f1());

                binary_functor_t f0 = nullptr;
                binary_functor_t f1 = vovov->f0();
                binary_functor_t f2 = vovov->f1();

                details::free_node(*(expr_gen.node_allocator_), branch[1]);

                expression_node_ptr result = error_node();

                const bool synthesis_result =
                    synthesize_sf4ext_expression::template compile<T0, T1, T2, T3>(
                        expr_gen, id(expr_gen, o0, o1, o2), v0, v1, v2, v3, result);

                if (synthesis_result)
                    return result;
                else if (!expr_gen.valid_operator(o0, f0))
                    return error_node();

                core::debug_print("v0 o0 ((v1 o1 v2) o2 v3)\n");

                return node_type::allocate(*(expr_gen.node_allocator_), v0, v1, v2, v3, f0, f1, f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const core::operators::operator_type o0,
                                         const core::operators::operator_type o1,
                                         const core::operators::operator_type o2)
            {
                return core::build_string()
                       << "t" << expr_gen.to_str(o0) << "((t" << expr_gen.to_str(o1) << "t)"
                       << expr_gen.to_str(o2) << "t)";
            }
        };

        struct synthesize_vovovoc_expression2
        {
            using node_type = typename vovovoc_t::type2;
            using sf4_type = typename vovovoc_t::sf4_type;
            using T0 = typename node_type::T0;
            using T1 = typename node_type::T1;
            using T2 = typename node_type::T2;
            using T3 = typename node_type::T3;

            static inline expression_node_ptr process(
                expression_generator<Type>& expr_gen,
                const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
            {
                // v0 o0 ((v1 o1 v2) o2 c)
                using lcl_vovoc_t = typename synthesize_vovoc_expression0::node_type;

                const lcl_vovoc_t* vovoc = static_cast<const lcl_vovoc_t*>(branch[1]);
                const Type& v0 = static_cast<details::variable_node<Type>*>(branch[0])->ref();
                const Type& v1 = vovoc->t0();
                const Type& v2 = vovoc->t1();
                const Type c = vovoc->t2();
                const core::operators::operator_type o0 = operation;
                const core::operators::operator_type o1 = expr_gen.get_operator(vovoc->f0());
                const core::operators::operator_type o2 = expr_gen.get_operator(vovoc->f1());

                binary_functor_t f0 = nullptr;
                binary_functor_t f1 = vovoc->f0();
                binary_functor_t f2 = vovoc->f1();

                details::free_node(*(expr_gen.node_allocator_), branch[1]);

                expression_node_ptr result = error_node();

                const bool synthesis_result =
                    synthesize_sf4ext_expression::template compile<T0, T1, T2, T3>(
                        expr_gen, id(expr_gen, o0, o1, o2), v0, v1, v2, c, result);

                if (synthesis_result)
                    return result;
                else if (!expr_gen.valid_operator(o0, f0))
                    return error_node();

                core::debug_print("v0 o0 ((v1 o1 v2) o2 c)\n");

                return node_type::allocate(*(expr_gen.node_allocator_), v0, v1, v2, c, f0, f1, f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const core::operators::operator_type o0,
                                         const core::operators::operator_type o1,
                                         const core::operators::operator_type o2)
            {
                return core::build_string()
                       << "t" << expr_gen.to_str(o0) << "((t" << expr_gen.to_str(o1) << "t)"
                       << expr_gen.to_str(o2) << "t)";
            }
        };

        struct synthesize_vovocov_expression2
        {
            using node_type = typename vovocov_t::type2;
            using sf4_type = typename vovocov_t::sf4_type;
            using T0 = typename node_type::T0;
            using T1 = typename node_type::T1;
            using T2 = typename node_type::T2;
            using T3 = typename node_type::T3;

            static inline expression_node_ptr process(
                expression_generator<Type>& expr_gen,
                const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
            {
                // v0 o0 ((v1 o1 c) o2 v2)
                using lcl_vocov_t = typename synthesize_vocov_expression0::node_type;

                const lcl_vocov_t* vocov = static_cast<const lcl_vocov_t*>(branch[1]);
                const Type& v0 = static_cast<details::variable_node<Type>*>(branch[0])->ref();
                const Type& v1 = vocov->t0();
                const Type c = vocov->t1();
                const Type& v2 = vocov->t2();
                const core::operators::operator_type o0 = operation;
                const core::operators::operator_type o1 = expr_gen.get_operator(vocov->f0());
                const core::operators::operator_type o2 = expr_gen.get_operator(vocov->f1());

                binary_functor_t f0 = nullptr;
                binary_functor_t f1 = vocov->f0();
                binary_functor_t f2 = vocov->f1();

                details::free_node(*(expr_gen.node_allocator_), branch[1]);

                expression_node_ptr result = error_node();

                const bool synthesis_result =
                    synthesize_sf4ext_expression::template compile<T0, T1, T2, T3>(
                        expr_gen, id(expr_gen, o0, o1, o2), v0, v1, c, v2, result);

                if (synthesis_result)
                    return result;
                else if (!expr_gen.valid_operator(o0, f0))
                    return error_node();

                core::debug_print("v0 o0 ((v1 o1 c) o2 v2)\n");

                return node_type::allocate(*(expr_gen.node_allocator_), v0, v1, c, v2, f0, f1, f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const core::operators::operator_type o0,
                                         const core::operators::operator_type o1,
                                         const core::operators::operator_type o2)
            {
                return core::build_string()
                       << "t" << expr_gen.to_str(o0) << "((t" << expr_gen.to_str(o1) << "t)"
                       << expr_gen.to_str(o2) << "t)";
            }
        };

        struct synthesize_vocovov_expression2
        {
            using node_type = typename vocovov_t::type2;
            using sf4_type = typename vocovov_t::sf4_type;
            using T0 = typename node_type::T0;
            using T1 = typename node_type::T1;
            using T2 = typename node_type::T2;
            using T3 = typename node_type::T3;

            static inline expression_node_ptr process(
                expression_generator<Type>& expr_gen,
                const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
            {
                // v0 o0 ((c o1 v1) o2 v2)
                using lcl_covov_t = typename synthesize_covov_expression0::node_type;

                const lcl_covov_t* covov = static_cast<const lcl_covov_t*>(branch[1]);
                const Type& v0 = static_cast<details::variable_node<Type>*>(branch[0])->ref();
                const Type c = covov->t0();
                const Type& v1 = covov->t1();
                const Type& v2 = covov->t2();
                const core::operators::operator_type o0 = operation;
                const core::operators::operator_type o1 = expr_gen.get_operator(covov->f0());
                const core::operators::operator_type o2 = expr_gen.get_operator(covov->f1());

                binary_functor_t f0 = nullptr;
                binary_functor_t f1 = covov->f0();
                binary_functor_t f2 = covov->f1();

                details::free_node(*(expr_gen.node_allocator_), branch[1]);

                expression_node_ptr result = error_node();

                const bool synthesis_result =
                    synthesize_sf4ext_expression::template compile<T0, T1, T2, T3>(
                        expr_gen, id(expr_gen, o0, o1, o2), v0, c, v1, v2, result);

                if (synthesis_result)
                    return result;
                else if (!expr_gen.valid_operator(o0, f0))
                    return error_node();

                core::debug_print("v0 o0 ((c o1 v1) o2 v2)\n");

                return node_type::allocate(*(expr_gen.node_allocator_), v0, c, v1, v2, f0, f1, f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const core::operators::operator_type o0,
                                         const core::operators::operator_type o1,
                                         const core::operators::operator_type o2)
            {
                return core::build_string()
                       << "t" << expr_gen.to_str(o0) << "((t" << expr_gen.to_str(o1) << "t)"
                       << expr_gen.to_str(o2) << "t)";
            }
        };

        struct synthesize_covovov_expression2
        {
            using node_type = typename covovov_t::type2;
            using sf4_type = typename covovov_t::sf4_type;
            using T0 = typename node_type::T0;
            using T1 = typename node_type::T1;
            using T2 = typename node_type::T2;
            using T3 = typename node_type::T3;

            static inline expression_node_ptr process(
                expression_generator<Type>& expr_gen,
                const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
            {
                // c o0 ((v1 o1 v2) o2 v3)
                using lcl_vovov_t = typename synthesize_vovov_expression0::node_type;

                const lcl_vovov_t* vovov = static_cast<const lcl_vovov_t*>(branch[1]);
                const Type c = static_cast<details::literal_node<Type>*>(branch[0])->value();
                const Type& v0 = vovov->t0();
                const Type& v1 = vovov->t1();
                const Type& v2 = vovov->t2();
                const core::operators::operator_type o0 = operation;
                const core::operators::operator_type o1 = expr_gen.get_operator(vovov->f0());
                const core::operators::operator_type o2 = expr_gen.get_operator(vovov->f1());

                binary_functor_t f0 = nullptr;
                binary_functor_t f1 = vovov->f0();
                binary_functor_t f2 = vovov->f1();

                details::free_node(*(expr_gen.node_allocator_), branch[0]);
                details::free_node(*(expr_gen.node_allocator_), branch[1]);

                expression_node_ptr result = error_node();

                const bool synthesis_result =
                    synthesize_sf4ext_expression::template compile<T0, T1, T2, T3>(
                        expr_gen, id(expr_gen, o0, o1, o2), c, v0, v1, v2, result);

                if (synthesis_result)
                    return result;
                else if (!expr_gen.valid_operator(o0, f0))
                    return error_node();

                core::debug_print("c o0 ((v1 o1 v2) o2 v3)\n");

                return node_type::allocate(*(expr_gen.node_allocator_), c, v0, v1, v2, f0, f1, f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const core::operators::operator_type o0,
                                         const core::operators::operator_type o1,
                                         const core::operators::operator_type o2)
            {
                return core::build_string()
                       << "t" << expr_gen.to_str(o0) << "((t" << expr_gen.to_str(o1) << "t)"
                       << expr_gen.to_str(o2) << "t)";
            }
        };

        struct synthesize_covocov_expression2
        {
            using node_type = typename covocov_t::type2;
            using sf4_type = typename covocov_t::sf4_type;
            using T0 = typename node_type::T0;
            using T1 = typename node_type::T1;
            using T2 = typename node_type::T2;
            using T3 = typename node_type::T3;

            static inline expression_node_ptr process(
                expression_generator<Type>& expr_gen,
                const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
            {
                // c0 o0 ((v0 o1 c1) o2 v1)
                using lcl_vocov_t = typename synthesize_vocov_expression0::node_type;

                const lcl_vocov_t* vocov = static_cast<const lcl_vocov_t*>(branch[1]);
                const Type c0 = static_cast<details::literal_node<Type>*>(branch[0])->value();
                const Type& v0 = vocov->t0();
                const Type c1 = vocov->t1();
                const Type& v1 = vocov->t2();
                const core::operators::operator_type o0 = operation;
                const core::operators::operator_type o1 = expr_gen.get_operator(vocov->f0());
                const core::operators::operator_type o2 = expr_gen.get_operator(vocov->f1());

                binary_functor_t f0 = nullptr;
                binary_functor_t f1 = vocov->f0();
                binary_functor_t f2 = vocov->f1();

                details::free_node(*(expr_gen.node_allocator_), branch[0]);
                details::free_node(*(expr_gen.node_allocator_), branch[1]);

                expression_node_ptr result = error_node();

                const bool synthesis_result =
                    synthesize_sf4ext_expression::template compile<T0, T1, T2, T3>(
                        expr_gen, id(expr_gen, o0, o1, o2), c0, v0, c1, v1, result);

                if (synthesis_result)
                    return result;
                else if (!expr_gen.valid_operator(o0, f0))
                    return error_node();

                core::debug_print("c0 o0 ((v0 o1 c1) o2 v1)\n");

                return node_type::allocate(*(expr_gen.node_allocator_), c0, v0, c1, v1, f0, f1, f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const core::operators::operator_type o0,
                                         const core::operators::operator_type o1,
                                         const core::operators::operator_type o2)
            {
                return core::build_string()
                       << "t" << expr_gen.to_str(o0) << "((t" << expr_gen.to_str(o1) << "t)"
                       << expr_gen.to_str(o2) << "t)";
            }
        };

        struct synthesize_vocovoc_expression2
        {
            using node_type = typename vocovoc_t::type2;
            using sf4_type = typename vocovoc_t::sf4_type;
            using T0 = typename node_type::T0;
            using T1 = typename node_type::T1;
            using T2 = typename node_type::T2;
            using T3 = typename node_type::T3;

            static inline expression_node_ptr process(
                expression_generator<Type>& expr_gen,
                const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
            {
                // v0 o0 ((c0 o1 v1) o2 c1)
                using lcl_covoc_t = typename synthesize_covoc_expression0::node_type;

                const lcl_covoc_t* covoc = static_cast<const lcl_covoc_t*>(branch[1]);
                const Type& v0 = static_cast<details::variable_node<Type>*>(branch[0])->ref();
                const Type c0 = covoc->t0();
                const Type& v1 = covoc->t1();
                const Type c1 = covoc->t2();
                const core::operators::operator_type o0 = operation;
                const core::operators::operator_type o1 = expr_gen.get_operator(covoc->f0());
                const core::operators::operator_type o2 = expr_gen.get_operator(covoc->f1());

                binary_functor_t f0 = nullptr;
                binary_functor_t f1 = covoc->f0();
                binary_functor_t f2 = covoc->f1();

                details::free_node(*(expr_gen.node_allocator_), branch[1]);

                expression_node_ptr result = error_node();

                const bool synthesis_result =
                    synthesize_sf4ext_expression::template compile<T0, T1, T2, T3>(
                        expr_gen, id(expr_gen, o0, o1, o2), v0, c0, v1, c1, result);

                if (synthesis_result)
                    return result;
                else if (!expr_gen.valid_operator(o0, f0))
                    return error_node();

                core::debug_print("v0 o0 ((c0 o1 v1) o2 c1)\n");

                return node_type::allocate(*(expr_gen.node_allocator_), v0, c0, v1, c1, f0, f1, f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const core::operators::operator_type o0,
                                         const core::operators::operator_type o1,
                                         const core::operators::operator_type o2)
            {
                return core::build_string()
                       << "t" << expr_gen.to_str(o0) << "((t" << expr_gen.to_str(o1) << "t)"
                       << expr_gen.to_str(o2) << "t)";
            }
        };

        struct synthesize_covovoc_expression2
        {
            using node_type = typename covovoc_t::type2;
            using sf4_type = typename covovoc_t::sf4_type;
            using T0 = typename node_type::T0;
            using T1 = typename node_type::T1;
            using T2 = typename node_type::T2;
            using T3 = typename node_type::T3;

            static inline expression_node_ptr process(
                expression_generator<Type>& expr_gen,
                const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
            {
                // c0 o0 ((v0 o1 v1) o2 c1)
                using lcl_vovoc_t = typename synthesize_vovoc_expression0::node_type;

                const lcl_vovoc_t* vovoc = static_cast<const lcl_vovoc_t*>(branch[1]);
                const Type c0 = static_cast<details::literal_node<Type>*>(branch[0])->value();
                const Type& v0 = vovoc->t0();
                const Type& v1 = vovoc->t1();
                const Type c1 = vovoc->t2();
                const core::operators::operator_type o0 = operation;
                const core::operators::operator_type o1 = expr_gen.get_operator(vovoc->f0());
                const core::operators::operator_type o2 = expr_gen.get_operator(vovoc->f1());

                binary_functor_t f0 = nullptr;
                binary_functor_t f1 = vovoc->f0();
                binary_functor_t f2 = vovoc->f1();

                details::free_node(*(expr_gen.node_allocator_), branch[0]);
                details::free_node(*(expr_gen.node_allocator_), branch[1]);

                expression_node_ptr result = error_node();

                const bool synthesis_result =
                    synthesize_sf4ext_expression::template compile<T0, T1, T2, T3>(
                        expr_gen, id(expr_gen, o0, o1, o2), c0, v0, v1, c1, result);

                if (synthesis_result)
                    return result;
                else if (!expr_gen.valid_operator(o0, f0))
                    return error_node();

                core::debug_print("c0 o0 ((v0 o1 v1) o2 c1)\n");

                return node_type::allocate(*(expr_gen.node_allocator_), c0, v0, v1, c1, f0, f1, f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const core::operators::operator_type o0,
                                         const core::operators::operator_type o1,
                                         const core::operators::operator_type o2)
            {
                return core::build_string()
                       << "t" << expr_gen.to_str(o0) << "((t" << expr_gen.to_str(o1) << "t)"
                       << expr_gen.to_str(o2) << "t)";
            }
        };

        struct synthesize_vococov_expression2
        {
            using node_type = typename vococov_t::type2;
            static inline expression_node_ptr process(expression_generator<Type>&,
                                                      const core::operators::operator_type&,
                                                      expression_node_ptr (&)[2])
            {
                // v0 o0 ((c0 o1 c1) o2 v1) - Not possible
                core::debug_print("v0 o0 ((c0 o1 c1) o2 v1) - Not possible\n");
                return error_node();
            }

            static inline std::string id(expression_generator<Type>&,
                                         const core::operators::operator_type,
                                         const core::operators::operator_type,
                                         const core::operators::operator_type)
            {
                return "INVALID";
            }
        };

        struct synthesize_vovovov_expression3
        {
            using node_type = typename vovovov_t::type3;
            using sf4_type = typename vovovov_t::sf4_type;
            using T0 = typename node_type::T0;
            using T1 = typename node_type::T1;
            using T2 = typename node_type::T2;
            using T3 = typename node_type::T3;

            static inline expression_node_ptr process(
                expression_generator<Type>& expr_gen,
                const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
            {
                // ((v0 o0 v1) o1 v2) o2 v3
                using lcl_vovov_t = typename synthesize_vovov_expression0::node_type;

                const lcl_vovov_t* vovov = static_cast<const lcl_vovov_t*>(branch[0]);
                const Type& v0 = vovov->t0();
                const Type& v1 = vovov->t1();
                const Type& v2 = vovov->t2();
                const Type& v3 = static_cast<details::variable_node<Type>*>(branch[1])->ref();
                const core::operators::operator_type o0 = expr_gen.get_operator(vovov->f0());
                const core::operators::operator_type o1 = expr_gen.get_operator(vovov->f1());
                const core::operators::operator_type o2 = operation;

                binary_functor_t f0 = vovov->f0();
                binary_functor_t f1 = vovov->f1();
                binary_functor_t f2 = nullptr;

                details::free_node(*(expr_gen.node_allocator_), branch[0]);

                expression_node_ptr result = error_node();

                const bool synthesis_result =
                    synthesize_sf4ext_expression::template compile<T0, T1, T2, T3>(
                        expr_gen, id(expr_gen, o0, o1, o2), v0, v1, v2, v3, result);

                if (synthesis_result)
                    return result;
                else if (!expr_gen.valid_operator(o2, f2))
                    return error_node();

                core::debug_print("((v0 o0 v1) o1 v2) o2 v3\n");

                return node_type::allocate(*(expr_gen.node_allocator_), v0, v1, v2, v3, f0, f1, f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const core::operators::operator_type o0,
                                         const core::operators::operator_type o1,
                                         const core::operators::operator_type o2)
            {
                return core::build_string()
                       << "((t" << expr_gen.to_str(o0) << "t)" << expr_gen.to_str(o1) << "t)"
                       << expr_gen.to_str(o2) << "t";
            }
        };

        struct synthesize_vovovoc_expression3
        {
            using node_type = typename vovovoc_t::type3;
            using sf4_type = typename vovovoc_t::sf4_type;
            using T0 = typename node_type::T0;
            using T1 = typename node_type::T1;
            using T2 = typename node_type::T2;
            using T3 = typename node_type::T3;

            static inline expression_node_ptr process(
                expression_generator<Type>& expr_gen,
                const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
            {
                // ((v0 o0 v1) o1 v2) o2 c
                using lcl_vovov_t = typename synthesize_vovov_expression0::node_type;

                const lcl_vovov_t* vovov = static_cast<const lcl_vovov_t*>(branch[0]);
                const Type& v0 = vovov->t0();
                const Type& v1 = vovov->t1();
                const Type& v2 = vovov->t2();
                const Type c = static_cast<details::literal_node<Type>*>(branch[1])->value();
                const core::operators::operator_type o0 = expr_gen.get_operator(vovov->f0());
                const core::operators::operator_type o1 = expr_gen.get_operator(vovov->f1());
                const core::operators::operator_type o2 = operation;

                binary_functor_t f0 = vovov->f0();
                binary_functor_t f1 = vovov->f1();
                binary_functor_t f2 = nullptr;

                details::free_node(*(expr_gen.node_allocator_), branch[0]);
                details::free_node(*(expr_gen.node_allocator_), branch[1]);

                expression_node_ptr result = error_node();

                const bool synthesis_result =
                    synthesize_sf4ext_expression::template compile<T0, T1, T2, T3>(
                        expr_gen, id(expr_gen, o0, o1, o2), v0, v1, v2, c, result);

                if (synthesis_result)
                    return result;
                else if (!expr_gen.valid_operator(o2, f2))
                    return error_node();

                core::debug_print("((v0 o0 v1) o1 v2) o2 c\n");

                return node_type::allocate(*(expr_gen.node_allocator_), v0, v1, v2, c, f0, f1, f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const core::operators::operator_type o0,
                                         const core::operators::operator_type o1,
                                         const core::operators::operator_type o2)
            {
                return core::build_string()
                       << "((t" << expr_gen.to_str(o0) << "t)" << expr_gen.to_str(o1) << "t)"
                       << expr_gen.to_str(o2) << "t";
            }
        };

        struct synthesize_vovocov_expression3
        {
            using node_type = typename vovocov_t::type3;
            using sf4_type = typename vovocov_t::sf4_type;
            using T0 = typename node_type::T0;
            using T1 = typename node_type::T1;
            using T2 = typename node_type::T2;
            using T3 = typename node_type::T3;

            static inline expression_node_ptr process(
                expression_generator<Type>& expr_gen,
                const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
            {
                // ((v0 o0 v1) o1 c) o2 v2
                using lcl_vovoc_t = typename synthesize_vovoc_expression0::node_type;

                const lcl_vovoc_t* vovoc = static_cast<const lcl_vovoc_t*>(branch[0]);
                const Type& v0 = vovoc->t0();
                const Type& v1 = vovoc->t1();
                const Type c = vovoc->t2();
                const Type& v2 = static_cast<details::variable_node<Type>*>(branch[1])->ref();
                const core::operators::operator_type o0 = expr_gen.get_operator(vovoc->f0());
                const core::operators::operator_type o1 = expr_gen.get_operator(vovoc->f1());
                const core::operators::operator_type o2 = operation;

                binary_functor_t f0 = vovoc->f0();
                binary_functor_t f1 = vovoc->f1();
                binary_functor_t f2 = nullptr;

                details::free_node(*(expr_gen.node_allocator_), branch[0]);

                expression_node_ptr result = error_node();

                const bool synthesis_result =
                    synthesize_sf4ext_expression::template compile<T0, T1, T2, T3>(
                        expr_gen, id(expr_gen, o0, o1, o2), v0, v1, c, v2, result);

                if (synthesis_result)
                    return result;
                else if (!expr_gen.valid_operator(o2, f2))
                    return error_node();

                core::debug_print("((v0 o0 v1) o1 c) o2 v2\n");

                return node_type::allocate(*(expr_gen.node_allocator_), v0, v1, c, v2, f0, f1, f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const core::operators::operator_type o0,
                                         const core::operators::operator_type o1,
                                         const core::operators::operator_type o2)
            {
                return core::build_string()
                       << "((t" << expr_gen.to_str(o0) << "t)" << expr_gen.to_str(o1) << "t)"
                       << expr_gen.to_str(o2) << "t";
            }
        };

        struct synthesize_vocovov_expression3
        {
            using node_type = typename vocovov_t::type3;
            using sf4_type = typename vocovov_t::sf4_type;
            using T0 = typename node_type::T0;
            using T1 = typename node_type::T1;
            using T2 = typename node_type::T2;
            using T3 = typename node_type::T3;

            static inline expression_node_ptr process(
                expression_generator<Type>& expr_gen,
                const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
            {
                // ((v0 o0 c) o1 v1) o2 v2
                using lcl_vocov_t = typename synthesize_vocov_expression0::node_type;

                const lcl_vocov_t* vocov = static_cast<const lcl_vocov_t*>(branch[0]);
                const Type& v0 = vocov->t0();
                const Type c = vocov->t1();
                const Type& v1 = vocov->t2();
                const Type& v2 = static_cast<details::variable_node<Type>*>(branch[1])->ref();
                const core::operators::operator_type o0 = expr_gen.get_operator(vocov->f0());
                const core::operators::operator_type o1 = expr_gen.get_operator(vocov->f1());
                const core::operators::operator_type o2 = operation;

                binary_functor_t f0 = vocov->f0();
                binary_functor_t f1 = vocov->f1();
                binary_functor_t f2 = nullptr;

                details::free_node(*(expr_gen.node_allocator_), branch[0]);

                expression_node_ptr result = error_node();

                const bool synthesis_result =
                    synthesize_sf4ext_expression::template compile<T0, T1, T2, T3>(
                        expr_gen, id(expr_gen, o0, o1, o2), v0, c, v1, v2, result);

                if (synthesis_result)
                    return result;
                else if (!expr_gen.valid_operator(o2, f2))
                    return error_node();

                core::debug_print("((v0 o0 c) o1 v1) o2 v2\n");

                return node_type::allocate(*(expr_gen.node_allocator_), v0, c, v1, v2, f0, f1, f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const core::operators::operator_type o0,
                                         const core::operators::operator_type o1,
                                         const core::operators::operator_type o2)
            {
                return core::build_string()
                       << "((t" << expr_gen.to_str(o0) << "t)" << expr_gen.to_str(o1) << "t)"
                       << expr_gen.to_str(o2) << "t";
            }
        };

        struct synthesize_covovov_expression3
        {
            using node_type = typename covovov_t::type3;
            using sf4_type = typename covovov_t::sf4_type;
            using T0 = typename node_type::T0;
            using T1 = typename node_type::T1;
            using T2 = typename node_type::T2;
            using T3 = typename node_type::T3;

            static inline expression_node_ptr process(
                expression_generator<Type>& expr_gen,
                const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
            {
                // ((c o0 v0) o1 v1) o2 v2
                using lcl_covov_t = typename synthesize_covov_expression0::node_type;

                const lcl_covov_t* covov = static_cast<const lcl_covov_t*>(branch[0]);
                const Type c = covov->t0();
                const Type& v0 = covov->t1();
                const Type& v1 = covov->t2();
                const Type& v2 = static_cast<details::variable_node<Type>*>(branch[1])->ref();
                const core::operators::operator_type o0 = expr_gen.get_operator(covov->f0());
                const core::operators::operator_type o1 = expr_gen.get_operator(covov->f1());
                const core::operators::operator_type o2 = operation;

                binary_functor_t f0 = covov->f0();
                binary_functor_t f1 = covov->f1();
                binary_functor_t f2 = nullptr;

                details::free_node(*(expr_gen.node_allocator_), branch[0]);

                expression_node_ptr result = error_node();

                const bool synthesis_result =
                    synthesize_sf4ext_expression::template compile<T0, T1, T2, T3>(
                        expr_gen, id(expr_gen, o0, o1, o2), c, v0, v1, v2, result);

                if (synthesis_result)
                    return result;
                else if (!expr_gen.valid_operator(o2, f2))
                    return error_node();

                core::debug_print("((c o0 v0) o1 v1) o2 v2\n");

                return node_type::allocate(*(expr_gen.node_allocator_), c, v0, v1, v2, f0, f1, f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const core::operators::operator_type o0,
                                         const core::operators::operator_type o1,
                                         const core::operators::operator_type o2)
            {
                return core::build_string()
                       << "((t" << expr_gen.to_str(o0) << "t)" << expr_gen.to_str(o1) << "t)"
                       << expr_gen.to_str(o2) << "t";
            }
        };

        struct synthesize_covocov_expression3
        {
            using node_type = typename covocov_t::type3;
            using sf4_type = typename covocov_t::sf4_type;
            using T0 = typename node_type::T0;
            using T1 = typename node_type::T1;
            using T2 = typename node_type::T2;
            using T3 = typename node_type::T3;

            static inline expression_node_ptr process(
                expression_generator<Type>& expr_gen,
                const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
            {
                // ((c0 o0 v0) o1 c1) o2 v1
                using lcl_covoc_t = typename synthesize_covoc_expression0::node_type;

                const lcl_covoc_t* covoc = static_cast<const lcl_covoc_t*>(branch[0]);
                const Type c0 = covoc->t0();
                const Type& v0 = covoc->t1();
                const Type c1 = covoc->t2();
                const Type& v1 = static_cast<details::variable_node<Type>*>(branch[1])->ref();
                const core::operators::operator_type o0 = expr_gen.get_operator(covoc->f0());
                const core::operators::operator_type o1 = expr_gen.get_operator(covoc->f1());
                const core::operators::operator_type o2 = operation;

                binary_functor_t f0 = covoc->f0();
                binary_functor_t f1 = covoc->f1();
                binary_functor_t f2 = nullptr;

                details::free_node(*(expr_gen.node_allocator_), branch[0]);

                expression_node_ptr result = error_node();

                const bool synthesis_result =
                    synthesize_sf4ext_expression::template compile<T0, T1, T2, T3>(
                        expr_gen, id(expr_gen, o0, o1, o2), c0, v0, c1, v1, result);

                if (synthesis_result)
                    return result;
                else if (!expr_gen.valid_operator(o2, f2))
                    return error_node();

                core::debug_print("((c0 o0 v0) o1 c1) o2 v1\n");

                return node_type::allocate(*(expr_gen.node_allocator_), c0, v0, c1, v1, f0, f1, f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const core::operators::operator_type o0,
                                         const core::operators::operator_type o1,
                                         const core::operators::operator_type o2)
            {
                return core::build_string()
                       << "((t" << expr_gen.to_str(o0) << "t)" << expr_gen.to_str(o1) << "t)"
                       << expr_gen.to_str(o2) << "t";
            }
        };

        struct synthesize_vocovoc_expression3
        {
            using node_type = typename vocovoc_t::type3;
            using sf4_type = typename vocovoc_t::sf4_type;
            using T0 = typename node_type::T0;
            using T1 = typename node_type::T1;
            using T2 = typename node_type::T2;
            using T3 = typename node_type::T3;

            static inline expression_node_ptr process(
                expression_generator<Type>& expr_gen,
                const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
            {
                // ((v0 o0 c0) o1 v1) o2 c1
                using lcl_vocov_t = typename synthesize_vocov_expression0::node_type;

                const lcl_vocov_t* vocov = static_cast<const lcl_vocov_t*>(branch[0]);
                const Type& v0 = vocov->t0();
                const Type c0 = vocov->t1();
                const Type& v1 = vocov->t2();
                const Type c1 = static_cast<details::literal_node<Type>*>(branch[1])->value();
                const core::operators::operator_type o0 = expr_gen.get_operator(vocov->f0());
                const core::operators::operator_type o1 = expr_gen.get_operator(vocov->f1());
                const core::operators::operator_type o2 = operation;

                binary_functor_t f0 = vocov->f0();
                binary_functor_t f1 = vocov->f1();
                binary_functor_t f2 = nullptr;

                details::free_node(*(expr_gen.node_allocator_), branch[0]);
                details::free_node(*(expr_gen.node_allocator_), branch[1]);

                expression_node_ptr result = error_node();

                const bool synthesis_result =
                    synthesize_sf4ext_expression::template compile<T0, T1, T2, T3>(
                        expr_gen, id(expr_gen, o0, o1, o2), v0, c0, v1, c1, result);

                if (synthesis_result)
                    return result;
                else if (!expr_gen.valid_operator(o2, f2))
                    return error_node();

                core::debug_print("((v0 o0 c0) o1 v1) o2 c1\n");

                return node_type::allocate(*(expr_gen.node_allocator_), v0, c0, v1, c1, f0, f1, f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const core::operators::operator_type o0,
                                         const core::operators::operator_type o1,
                                         const core::operators::operator_type o2)
            {
                return core::build_string()
                       << "((t" << expr_gen.to_str(o0) << "t)" << expr_gen.to_str(o1) << "t)"
                       << expr_gen.to_str(o2) << "t";
            }
        };

        struct synthesize_covovoc_expression3
        {
            using node_type = typename covovoc_t::type3;
            using sf4_type = typename covovoc_t::sf4_type;
            using T0 = typename node_type::T0;
            using T1 = typename node_type::T1;
            using T2 = typename node_type::T2;
            using T3 = typename node_type::T3;

            static inline expression_node_ptr process(
                expression_generator<Type>& expr_gen,
                const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
            {
                // ((c0 o0 v0) o1 v1) o2 c1
                using lcl_covov_t = typename synthesize_covov_expression0::node_type;

                const lcl_covov_t* covov = static_cast<const lcl_covov_t*>(branch[0]);
                const Type c0 = covov->t0();
                const Type& v0 = covov->t1();
                const Type& v1 = covov->t2();
                const Type c1 = static_cast<details::literal_node<Type>*>(branch[1])->value();
                const core::operators::operator_type o0 = expr_gen.get_operator(covov->f0());
                const core::operators::operator_type o1 = expr_gen.get_operator(covov->f1());
                const core::operators::operator_type o2 = operation;

                binary_functor_t f0 = covov->f0();
                binary_functor_t f1 = covov->f1();
                binary_functor_t f2 = nullptr;

                details::free_node(*(expr_gen.node_allocator_), branch[0]);
                details::free_node(*(expr_gen.node_allocator_), branch[1]);

                expression_node_ptr result = error_node();

                const bool synthesis_result =
                    synthesize_sf4ext_expression::template compile<T0, T1, T2, T3>(
                        expr_gen, id(expr_gen, o0, o1, o2), c0, v0, v1, c1, result);

                if (synthesis_result)
                    return result;
                else if (!expr_gen.valid_operator(o2, f2))
                    return error_node();

                core::debug_print("((c0 o0 v0) o1 v1) o2 c1\n");

                return node_type::allocate(*(expr_gen.node_allocator_), c0, v0, v1, c1, f0, f1, f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const core::operators::operator_type o0,
                                         const core::operators::operator_type o1,
                                         const core::operators::operator_type o2)
            {
                return core::build_string()
                       << "((t" << expr_gen.to_str(o0) << "t)" << expr_gen.to_str(o1) << "t)"
                       << expr_gen.to_str(o2) << "t";
            }
        };

        struct synthesize_vococov_expression3
        {
            using node_type = typename vococov_t::type3;
            using sf4_type = typename vococov_t::sf4_type;
            using T0 = typename node_type::T0;
            using T1 = typename node_type::T1;
            using T2 = typename node_type::T2;
            using T3 = typename node_type::T3;

            static inline expression_node_ptr process(
                expression_generator<Type>& expr_gen,
                const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
            {
                // ((v0 o0 c0) o1 c1) o2 v1
                using lcl_vococ_t = typename synthesize_vococ_expression0::node_type;

                const lcl_vococ_t* vococ = static_cast<const lcl_vococ_t*>(branch[0]);
                const Type& v0 = vococ->t0();
                const Type c0 = vococ->t1();
                const Type c1 = vococ->t2();
                const Type& v1 = static_cast<details::variable_node<Type>*>(branch[1])->ref();
                const core::operators::operator_type o0 = expr_gen.get_operator(vococ->f0());
                const core::operators::operator_type o1 = expr_gen.get_operator(vococ->f1());
                const core::operators::operator_type o2 = operation;

                binary_functor_t f0 = vococ->f0();
                binary_functor_t f1 = vococ->f1();
                binary_functor_t f2 = nullptr;

                details::free_node(*(expr_gen.node_allocator_), branch[0]);

                expression_node_ptr result = error_node();

                const bool synthesis_result =
                    synthesize_sf4ext_expression::template compile<T0, T1, T2, T3>(
                        expr_gen, id(expr_gen, o0, o1, o2), v0, c0, c1, v1, result);

                if (synthesis_result)
                    return result;
                else if (!expr_gen.valid_operator(o2, f2))
                    return error_node();

                core::debug_print("((v0 o0 c0) o1 c1) o2 v1\n");

                return node_type::allocate(*(expr_gen.node_allocator_), v0, c0, c1, v1, f0, f1, f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const core::operators::operator_type o0,
                                         const core::operators::operator_type o1,
                                         const core::operators::operator_type o2)
            {
                return core::build_string()
                       << "((t" << expr_gen.to_str(o0) << "t)" << expr_gen.to_str(o1) << "t)"
                       << expr_gen.to_str(o2) << "t";
            }
        };

        struct synthesize_vovovov_expression4
        {
            using node_type = typename vovovov_t::type4;
            using sf4_type = typename vovovov_t::sf4_type;
            using T0 = typename node_type::T0;
            using T1 = typename node_type::T1;
            using T2 = typename node_type::T2;
            using T3 = typename node_type::T3;

            static inline expression_node_ptr process(
                expression_generator<Type>& expr_gen,
                const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
            {
                // (v0 o0 (v1 o1 v2)) o2 v3
                using lcl_vovov_t = typename synthesize_vovov_expression1::node_type;

                const lcl_vovov_t* vovov = static_cast<const lcl_vovov_t*>(branch[0]);
                const Type& v0 = vovov->t0();
                const Type& v1 = vovov->t1();
                const Type& v2 = vovov->t2();
                const Type& v3 = static_cast<details::variable_node<Type>*>(branch[1])->ref();
                const core::operators::operator_type o0 = expr_gen.get_operator(vovov->f0());
                const core::operators::operator_type o1 = expr_gen.get_operator(vovov->f1());
                const core::operators::operator_type o2 = operation;

                binary_functor_t f0 = vovov->f0();
                binary_functor_t f1 = vovov->f1();
                binary_functor_t f2 = nullptr;

                details::free_node(*(expr_gen.node_allocator_), branch[0]);

                expression_node_ptr result = error_node();

                const bool synthesis_result =
                    synthesize_sf4ext_expression::template compile<T0, T1, T2, T3>(
                        expr_gen, id(expr_gen, o0, o1, o2), v0, v1, v2, v3, result);

                if (synthesis_result)
                    return result;
                else if (!expr_gen.valid_operator(o2, f2))
                    return error_node();

                core::debug_print("(v0 o0 (v1 o1 v2)) o2 v3\n");

                return node_type::allocate(*(expr_gen.node_allocator_), v0, v1, v2, v3, f0, f1, f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const core::operators::operator_type o0,
                                         const core::operators::operator_type o1,
                                         const core::operators::operator_type o2)
            {
                return core::build_string()
                       << "(t" << expr_gen.to_str(o0) << "(t" << expr_gen.to_str(o1) << "t)"
                       << expr_gen.to_str(o2) << "t";
            }
        };

        struct synthesize_vovovoc_expression4
        {
            using node_type = typename vovovoc_t::type4;
            using sf4_type = typename vovovoc_t::sf4_type;
            using T0 = typename node_type::T0;
            using T1 = typename node_type::T1;
            using T2 = typename node_type::T2;
            using T3 = typename node_type::T3;

            static inline expression_node_ptr process(
                expression_generator<Type>& expr_gen,
                const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
            {
                // ((v0 o0 (v1 o1 v2)) o2 c)
                using lcl_vovov_t = typename synthesize_vovov_expression1::node_type;

                const lcl_vovov_t* vovov = static_cast<const lcl_vovov_t*>(branch[0]);
                const Type& v0 = vovov->t0();
                const Type& v1 = vovov->t1();
                const Type& v2 = vovov->t2();
                const Type c = static_cast<details::literal_node<Type>*>(branch[1])->value();
                const core::operators::operator_type o0 = expr_gen.get_operator(vovov->f0());
                const core::operators::operator_type o1 = expr_gen.get_operator(vovov->f1());
                const core::operators::operator_type o2 = operation;

                binary_functor_t f0 = vovov->f0();
                binary_functor_t f1 = vovov->f1();
                binary_functor_t f2 = nullptr;

                details::free_node(*(expr_gen.node_allocator_), branch[0]);
                details::free_node(*(expr_gen.node_allocator_), branch[1]);

                expression_node_ptr result = error_node();

                const bool synthesis_result =
                    synthesize_sf4ext_expression::template compile<T0, T1, T2, T3>(
                        expr_gen, id(expr_gen, o0, o1, o2), v0, v1, v2, c, result);

                if (synthesis_result)
                    return result;
                else if (!expr_gen.valid_operator(o2, f2))
                    return error_node();

                core::debug_print("((v0 o0 (v1 o1 v2)) o2 c)\n");

                return node_type::allocate(*(expr_gen.node_allocator_), v0, v1, v2, c, f0, f1, f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const core::operators::operator_type o0,
                                         const core::operators::operator_type o1,
                                         const core::operators::operator_type o2)
            {
                return core::build_string()
                       << "(t" << expr_gen.to_str(o0) << "(t" << expr_gen.to_str(o1) << "t)"
                       << expr_gen.to_str(o2) << "t";
            }
        };

        struct synthesize_vovocov_expression4
        {
            using node_type = typename vovocov_t::type4;
            using sf4_type = typename vovocov_t::sf4_type;
            using T0 = typename node_type::T0;
            using T1 = typename node_type::T1;
            using T2 = typename node_type::T2;
            using T3 = typename node_type::T3;

            static inline expression_node_ptr process(
                expression_generator<Type>& expr_gen,
                const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
            {
                // ((v0 o0 (v1 o1 c)) o2 v1)
                using lcl_vovoc_t = typename synthesize_vovoc_expression1::node_type;

                const lcl_vovoc_t* vovoc = static_cast<const lcl_vovoc_t*>(branch[0]);
                const Type& v0 = vovoc->t0();
                const Type& v1 = vovoc->t1();
                const Type c = vovoc->t2();
                const Type& v2 = static_cast<details::variable_node<Type>*>(branch[1])->ref();
                const core::operators::operator_type o0 = expr_gen.get_operator(vovoc->f0());
                const core::operators::operator_type o1 = expr_gen.get_operator(vovoc->f1());
                const core::operators::operator_type o2 = operation;

                binary_functor_t f0 = vovoc->f0();
                binary_functor_t f1 = vovoc->f1();
                binary_functor_t f2 = nullptr;

                details::free_node(*(expr_gen.node_allocator_), branch[0]);

                expression_node_ptr result = error_node();

                const bool synthesis_result =
                    synthesize_sf4ext_expression::template compile<T0, T1, T2, T3>(
                        expr_gen, id(expr_gen, o0, o1, o2), v0, v1, c, v2, result);

                if (synthesis_result)
                    return result;
                else if (!expr_gen.valid_operator(o2, f2))
                    return error_node();

                core::debug_print("((v0 o0 (v1 o1 c)) o2 v1)\n");

                return node_type::allocate(*(expr_gen.node_allocator_), v0, v1, c, v2, f0, f1, f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const core::operators::operator_type o0,
                                         const core::operators::operator_type o1,
                                         const core::operators::operator_type o2)
            {
                return core::build_string()
                       << "(t" << expr_gen.to_str(o0) << "(t" << expr_gen.to_str(o1) << "t)"
                       << expr_gen.to_str(o2) << "t";
            }
        };

        struct synthesize_vocovov_expression4
        {
            using node_type = typename vocovov_t::type4;
            using sf4_type = typename vocovov_t::sf4_type;
            using T0 = typename node_type::T0;
            using T1 = typename node_type::T1;
            using T2 = typename node_type::T2;
            using T3 = typename node_type::T3;

            static inline expression_node_ptr process(
                expression_generator<Type>& expr_gen,
                const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
            {
                // ((v0 o0 (c o1 v1)) o2 v2)
                using lcl_vocov_t = typename synthesize_vocov_expression1::node_type;

                const lcl_vocov_t* vocov = static_cast<const lcl_vocov_t*>(branch[0]);
                const Type& v0 = vocov->t0();
                const Type c = vocov->t1();
                const Type& v1 = vocov->t2();
                const Type& v2 = static_cast<details::variable_node<Type>*>(branch[1])->ref();
                const core::operators::operator_type o0 = expr_gen.get_operator(vocov->f0());
                const core::operators::operator_type o1 = expr_gen.get_operator(vocov->f1());
                const core::operators::operator_type o2 = operation;

                binary_functor_t f0 = vocov->f0();
                binary_functor_t f1 = vocov->f1();
                binary_functor_t f2 = nullptr;

                details::free_node(*(expr_gen.node_allocator_), branch[0]);
                expression_node_ptr result = error_node();

                const bool synthesis_result =
                    synthesize_sf4ext_expression::template compile<T0, T1, T2, T3>(
                        expr_gen, id(expr_gen, o0, o1, o2), v0, c, v1, v2, result);

                if (synthesis_result)
                    return result;
                else if (!expr_gen.valid_operator(o2, f2))
                    return error_node();

                core::debug_print("((v0 o0 (c o1 v1)) o2 v2)\n");

                return node_type::allocate(*(expr_gen.node_allocator_), v0, c, v1, v2, f0, f1, f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const core::operators::operator_type o0,
                                         const core::operators::operator_type o1,
                                         const core::operators::operator_type o2)
            {
                return core::build_string()
                       << "(t" << expr_gen.to_str(o0) << "(t" << expr_gen.to_str(o1) << "t)"
                       << expr_gen.to_str(o2) << "t";
            }
        };

        struct synthesize_covovov_expression4
        {
            using node_type = typename covovov_t::type4;
            using sf4_type = typename covovov_t::sf4_type;
            using T0 = typename node_type::T0;
            using T1 = typename node_type::T1;
            using T2 = typename node_type::T2;
            using T3 = typename node_type::T3;

            static inline expression_node_ptr process(
                expression_generator<Type>& expr_gen,
                const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
            {
                // ((c o0 (v0 o1 v1)) o2 v2)
                using lcl_covov_t = typename synthesize_covov_expression1::node_type;

                const lcl_covov_t* covov = static_cast<const lcl_covov_t*>(branch[0]);
                const Type c = covov->t0();
                const Type& v0 = covov->t1();
                const Type& v1 = covov->t2();
                const Type& v2 = static_cast<details::variable_node<Type>*>(branch[1])->ref();
                const core::operators::operator_type o0 = expr_gen.get_operator(covov->f0());
                const core::operators::operator_type o1 = expr_gen.get_operator(covov->f1());
                const core::operators::operator_type o2 = operation;

                binary_functor_t f0 = covov->f0();
                binary_functor_t f1 = covov->f1();
                binary_functor_t f2 = nullptr;

                details::free_node(*(expr_gen.node_allocator_), branch[0]);

                expression_node_ptr result = error_node();

                const bool synthesis_result =
                    synthesize_sf4ext_expression::template compile<T0, T1, T2, T3>(
                        expr_gen, id(expr_gen, o0, o1, o2), c, v0, v1, v2, result);

                if (synthesis_result)
                    return result;
                else if (!expr_gen.valid_operator(o2, f2))
                    return error_node();

                core::debug_print("((c o0 (v0 o1 v1)) o2 v2)\n");

                return node_type::allocate(*(expr_gen.node_allocator_), c, v0, v1, v2, f0, f1, f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const core::operators::operator_type o0,
                                         const core::operators::operator_type o1,
                                         const core::operators::operator_type o2)
            {
                return core::build_string()
                       << "(t" << expr_gen.to_str(o0) << "(t" << expr_gen.to_str(o1) << "t)"
                       << expr_gen.to_str(o2) << "t";
            }
        };

        struct synthesize_covocov_expression4
        {
            using node_type = typename covocov_t::type4;
            using sf4_type = typename covocov_t::sf4_type;
            using T0 = typename node_type::T0;
            using T1 = typename node_type::T1;
            using T2 = typename node_type::T2;
            using T3 = typename node_type::T3;

            static inline expression_node_ptr process(
                expression_generator<Type>& expr_gen,
                const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
            {
                // ((c0 o0 (v0 o1 c1)) o2 v1)
                using lcl_covoc_t = typename synthesize_covoc_expression1::node_type;

                const lcl_covoc_t* covoc = static_cast<const lcl_covoc_t*>(branch[0]);
                const Type c0 = covoc->t0();
                const Type& v0 = covoc->t1();
                const Type c1 = covoc->t2();
                const Type& v1 = static_cast<details::variable_node<Type>*>(branch[1])->ref();
                const core::operators::operator_type o0 = expr_gen.get_operator(covoc->f0());
                const core::operators::operator_type o1 = expr_gen.get_operator(covoc->f1());
                const core::operators::operator_type o2 = operation;

                binary_functor_t f0 = covoc->f0();
                binary_functor_t f1 = covoc->f1();
                binary_functor_t f2 = nullptr;

                details::free_node(*(expr_gen.node_allocator_), branch[0]);

                expression_node_ptr result = error_node();

                const bool synthesis_result =
                    synthesize_sf4ext_expression::template compile<T0, T1, T2, T3>(
                        expr_gen, id(expr_gen, o0, o1, o2), c0, v0, c1, v1, result);

                if (synthesis_result)
                    return result;
                else if (!expr_gen.valid_operator(o2, f2))
                    return error_node();

                core::debug_print("((c0 o0 (v0 o1 c1)) o2 v1)\n");

                return node_type::allocate(*(expr_gen.node_allocator_), c0, v0, c1, v1, f0, f1, f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const core::operators::operator_type o0,
                                         const core::operators::operator_type o1,
                                         const core::operators::operator_type o2)
            {
                return core::build_string()
                       << "(t" << expr_gen.to_str(o0) << "(t" << expr_gen.to_str(o1) << "t)"
                       << expr_gen.to_str(o2) << "t";
            }
        };

        struct synthesize_vocovoc_expression4
        {
            using node_type = typename vocovoc_t::type4;
            using sf4_type = typename vocovoc_t::sf4_type;
            using T0 = typename node_type::T0;
            using T1 = typename node_type::T1;
            using T2 = typename node_type::T2;
            using T3 = typename node_type::T3;

            static inline expression_node_ptr process(
                expression_generator<Type>& expr_gen,
                const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
            {
                // ((v0 o0 (c0 o1 v1)) o2 c1)
                using lcl_vocov_t = typename synthesize_vocov_expression1::node_type;

                const lcl_vocov_t* vocov = static_cast<const lcl_vocov_t*>(branch[0]);
                const Type& v0 = vocov->t0();
                const Type c0 = vocov->t1();
                const Type& v1 = vocov->t2();
                const Type c1 = static_cast<details::literal_node<Type>*>(branch[1])->value();
                const core::operators::operator_type o0 = expr_gen.get_operator(vocov->f0());
                const core::operators::operator_type o1 = expr_gen.get_operator(vocov->f1());
                const core::operators::operator_type o2 = operation;

                binary_functor_t f0 = vocov->f0();
                binary_functor_t f1 = vocov->f1();
                binary_functor_t f2 = nullptr;

                details::free_node(*(expr_gen.node_allocator_), branch[0]);
                details::free_node(*(expr_gen.node_allocator_), branch[1]);

                expression_node_ptr result = error_node();

                const bool synthesis_result =
                    synthesize_sf4ext_expression::template compile<T0, T1, T2, T3>(
                        expr_gen, id(expr_gen, o0, o1, o2), v0, c0, v1, c1, result);

                if (synthesis_result)
                    return result;
                else if (!expr_gen.valid_operator(o2, f2))
                    return error_node();

                core::debug_print("((v0 o0 (c0 o1 v1)) o2 c1)\n");

                return node_type::allocate(*(expr_gen.node_allocator_), v0, c0, v1, c1, f0, f1, f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const core::operators::operator_type o0,
                                         const core::operators::operator_type o1,
                                         const core::operators::operator_type o2)
            {
                return core::build_string()
                       << "(t" << expr_gen.to_str(o0) << "(t" << expr_gen.to_str(o1) << "t)"
                       << expr_gen.to_str(o2) << "t";
            }
        };

        struct synthesize_covovoc_expression4
        {
            using node_type = typename covovoc_t::type4;
            using sf4_type = typename covovoc_t::sf4_type;
            using T0 = typename node_type::T0;
            using T1 = typename node_type::T1;
            using T2 = typename node_type::T2;
            using T3 = typename node_type::T3;

            static inline expression_node_ptr process(
                expression_generator<Type>& expr_gen,
                const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
            {
                // ((c0 o0 (v0 o1 v1)) o2 c1)
                using lcl_covov_t = typename synthesize_covov_expression1::node_type;

                const lcl_covov_t* covov = static_cast<const lcl_covov_t*>(branch[0]);
                const Type c0 = covov->t0();
                const Type& v0 = covov->t1();
                const Type& v1 = covov->t2();
                const Type c1 = static_cast<details::literal_node<Type>*>(branch[1])->value();
                const core::operators::operator_type o0 = expr_gen.get_operator(covov->f0());
                const core::operators::operator_type o1 = expr_gen.get_operator(covov->f1());
                const core::operators::operator_type o2 = operation;

                binary_functor_t f0 = covov->f0();
                binary_functor_t f1 = covov->f1();
                binary_functor_t f2 = nullptr;

                details::free_node(*(expr_gen.node_allocator_), branch[0]);
                details::free_node(*(expr_gen.node_allocator_), branch[1]);

                expression_node_ptr result = error_node();

                const bool synthesis_result =
                    synthesize_sf4ext_expression::template compile<T0, T1, T2, T3>(
                        expr_gen, id(expr_gen, o0, o1, o2), c0, v0, v1, c1, result);

                if (synthesis_result)
                    return result;
                else if (!expr_gen.valid_operator(o2, f2))
                    return error_node();

                core::debug_print("((c0 o0 (v0 o1 v1)) o2 c1)\n");

                return node_type::allocate(*(expr_gen.node_allocator_), c0, v0, v1, c1, f0, f1, f2);
            }

            static inline std::string id(expression_generator<Type>& expr_gen,
                                         const core::operators::operator_type o0,
                                         const core::operators::operator_type o1,
                                         const core::operators::operator_type o2)
            {
                return core::build_string()
                       << "(t" << expr_gen.to_str(o0) << "(t" << expr_gen.to_str(o1) << "t)"
                       << expr_gen.to_str(o2) << "t";
            }
        };

        struct synthesize_vococov_expression4
        {
            using node_type = typename vococov_t::type4;
            static inline expression_node_ptr process(expression_generator<Type>&,
                                                      const core::operators::operator_type&,
                                                      expression_node_ptr (&)[2])
            {
                // ((v0 o0 (c0 o1 c1)) o2 v1) - Not possible
                core::debug_print("((v0 o0 (c0 o1 c1)) o2 v1) - Not possible\n");
                return error_node();
            }

            static inline std::string id(expression_generator<Type>&,
                                         const core::operators::operator_type,
                                         const core::operators::operator_type,
                                         const core::operators::operator_type)
            {
                return "INVALID";
            }
        };
#endif

        inline expression_node_ptr synthesize_uvouv_expression(
            const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
        {
            // Definition: uv o uv
            core::operators::operator_type o0 =
                static_cast<details::uv_base_node<Type>*>(branch[0])->operation();
            core::operators::operator_type o1 =
                static_cast<details::uv_base_node<Type>*>(branch[1])->operation();
            const Type& v0 = static_cast<details::uv_base_node<Type>*>(branch[0])->v();
            const Type& v1 = static_cast<details::uv_base_node<Type>*>(branch[1])->v();
            unary_functor_t u0 = nullptr;
            unary_functor_t u1 = nullptr;
            binary_functor_t f = nullptr;

            if (!valid_operator(o0, u0))
                return error_node();
            else if (!valid_operator(o1, u1))
                return error_node();
            else if (!valid_operator(operation, f))
                return error_node();

            expression_node_ptr result = error_node();

            if ((core::operators::operator_type::neg == o0) &&
                (core::operators::operator_type::neg == o1))
            {
                switch (operation)
                {
                    // (-v0 + -v1) --> -(v0 + v1)
                    case core::operators::operator_type::add:
                        result = (*this)(
                            core::operators::operator_type::neg,
                            node_allocator_->allocate_rr<
                                typename details::vov_node<Type, details::add_op<Type>>>(v0, v1));
                        core::debug_print("(-v0 + -v1) --> -(v0 + v1)\n");
                        break;

                    // (-v0 - -v1) --> (v1 - v0)
                    case core::operators::operator_type::sub:
                        result = node_allocator_->allocate_rr<
                            typename details::vov_node<Type, details::sub_op<Type>>>(v1, v0);
                        core::debug_print("(-v0 - -v1) --> (v1 - v0)\n");
                        break;

                    // (-v0 * -v1) --> (v0 * v1)
                    case core::operators::operator_type::mul:
                        result = node_allocator_->allocate_rr<
                            typename details::vov_node<Type, details::mul_op<Type>>>(v0, v1);
                        core::debug_print("(-v0 * -v1) --> (v0 * v1)\n");
                        break;

                    // (-v0 / -v1) --> (v0 / v1)
                    case core::operators::operator_type::div:
                        result = node_allocator_->allocate_rr<
                            typename details::vov_node<Type, details::div_op<Type>>>(v0, v1);
                        core::debug_print("(-v0 / -v1) --> (v0 / v1)\n");
                        break;

                    default:
                        break;
                }
            }

            if (nullptr == result)
            {
                result = node_allocator_->allocate_rrrrr<typename details::uvouv_node<Type>>(
                    v0, v1, u0, u1, f);
            }

            details::free_all_nodes(*node_allocator_, branch);
            return result;
        }

#undef BASIC_OPR_SWITCH_STATEMENTS
#undef EXTENDED_OPR_SWITCH_STATEMENTS
#undef UNARY_OPR_SWITCH_STATEMENTS

#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES

#define STRING_OPR_SWITCH_STATEMENTS                                   \
    CASE_STMT(core::operators::operator_type::lt, details::lt_op);     \
    CASE_STMT(core::operators::operator_type::lte, details::lte_op);   \
    CASE_STMT(core::operators::operator_type::gt, details::gt_op);     \
    CASE_STMT(core::operators::operator_type::gte, details::gte_op);   \
    CASE_STMT(core::operators::operator_type::eq, details::eq_op);     \
    CASE_STMT(core::operators::operator_type::ne, details::ne_op);     \
    CASE_STMT(core::operators::operator_type::in, details::in_op);     \
    CASE_STMT(core::operators::operator_type::like, details::like_op); \
    CASE_STMT(core::operators::operator_type::ilike, details::ilike_op);

        template <typename T0, typename T1>
        inline expression_node_ptr synthesize_str_xrox_expression_impl(
            const core::operators::operator_type& opr, T0 s0, T1 s1, range_t rp0)
        {
            switch (opr)
            {
#define CASE_STMT(op0, op1)                                                                    \
    case op0:                                                                                  \
        return node_allocator_->allocate_ttt<                                                  \
            typename details::str_xrox_node<Type, T0, T1, range_t, op1<Type>>, T0, T1>(s0, s1, \
                                                                                       rp0);

                STRING_OPR_SWITCH_STATEMENTS
#undef CASE_STMT
                default:
                    return error_node();
            }
        }

        template <typename T0, typename T1>
        inline expression_node_ptr synthesize_str_xoxr_expression_impl(
            const core::operators::operator_type& opr, T0 s0, T1 s1, range_t rp1)
        {
            switch (opr)
            {
#define CASE_STMT(op0, op1)                                                                    \
    case op0:                                                                                  \
        return node_allocator_->allocate_ttt<                                                  \
            typename details::str_xoxr_node<Type, T0, T1, range_t, op1<Type>>, T0, T1>(s0, s1, \
                                                                                       rp1);

                STRING_OPR_SWITCH_STATEMENTS
#undef CASE_STMT
                default:
                    return error_node();
            }
        }

        template <typename T0, typename T1>
        inline expression_node_ptr synthesize_str_xroxr_expression_impl(
            const core::operators::operator_type& opr, T0 s0, T1 s1, range_t rp0, range_t rp1)
        {
            switch (opr)
            {
#define CASE_STMT(op0, op1)                                                                     \
    case op0:                                                                                   \
        return node_allocator_->allocate_tttt<                                                  \
            typename details::str_xroxr_node<Type, T0, T1, range_t, op1<Type>>, T0, T1>(s0, s1, \
                                                                                        rp0, rp1);

                STRING_OPR_SWITCH_STATEMENTS
#undef CASE_STMT
                default:
                    return error_node();
            }
        }

        template <typename T0, typename T1>
        inline expression_node_ptr synthesize_sos_expression_impl(
            const core::operators::operator_type& opr, T0 s0, T1 s1)
        {
            switch (opr)
            {
#define CASE_STMT(op0, op1)    \
    case op0:                  \
        return node_allocator_ \
            ->allocate_tt<typename details::sos_node<Type, T0, T1, op1<Type>>, T0, T1>(s0, s1);

                STRING_OPR_SWITCH_STATEMENTS
#undef CASE_STMT
                default:
                    return error_node();
            }
        }

        inline expression_node_ptr synthesize_sos_expression(
            const core::operators::operator_type& opr, expression_node_ptr (&branch)[2])
        {
            std::string& s0 =
                static_cast<details::string_nodes::stringvar_node<Type>*>(branch[0])->ref();
            std::string& s1 =
                static_cast<details::string_nodes::stringvar_node<Type>*>(branch[1])->ref();

            return synthesize_sos_expression_impl<std::string&, std::string&>(opr, s0, s1);
        }

        inline expression_node_ptr synthesize_sros_expression(
            const core::operators::operator_type& opr, expression_node_ptr (&branch)[2])
        {
            std::string& s0 =
                static_cast<details::string_nodes::string_range_node<Type>*>(branch[0])->ref();
            std::string& s1 =
                static_cast<details::string_nodes::stringvar_node<Type>*>(branch[1])->ref();
            range_t rp0 =
                static_cast<details::string_nodes::string_range_node<Type>*>(branch[0])->range();

            static_cast<details::string_nodes::string_range_node<Type>*>(branch[0])
                ->range_ref()
                .clear();

            details::free_node(*node_allocator_, branch[0]);

            return synthesize_str_xrox_expression_impl<std::string&, std::string&>(opr, s0, s1,
                                                                                   rp0);
        }

        inline expression_node_ptr synthesize_sosr_expression(
            const core::operators::operator_type& opr, expression_node_ptr (&branch)[2])
        {
            std::string& s0 =
                static_cast<details::string_nodes::stringvar_node<Type>*>(branch[0])->ref();
            std::string& s1 =
                static_cast<details::string_nodes::string_range_node<Type>*>(branch[1])->ref();
            range_t rp1 =
                static_cast<details::string_nodes::string_range_node<Type>*>(branch[1])->range();

            static_cast<details::string_nodes::string_range_node<Type>*>(branch[1])
                ->range_ref()
                .clear();

            details::free_node(*node_allocator_, branch[1]);

            return synthesize_str_xoxr_expression_impl<std::string&, std::string&>(opr, s0, s1,
                                                                                   rp1);
        }

        inline expression_node_ptr synthesize_socsr_expression(
            const core::operators::operator_type& opr, expression_node_ptr (&branch)[2])
        {
            std::string& s0 =
                static_cast<details::string_nodes::stringvar_node<Type>*>(branch[0])->ref();
            std::string s1 =
                static_cast<details::string_nodes::const_string_range_node<Type>*>(branch[1])
                    ->str();
            range_t rp1 =
                static_cast<details::string_nodes::const_string_range_node<Type>*>(branch[1])
                    ->range();

            static_cast<details::string_nodes::const_string_range_node<Type>*>(branch[1])
                ->range_ref()
                .clear();

            details::free_node(*node_allocator_, branch[1]);

            return synthesize_str_xoxr_expression_impl<std::string&, const std::string>(opr, s0, s1,
                                                                                        rp1);
        }

        inline expression_node_ptr synthesize_srosr_expression(
            const core::operators::operator_type& opr, expression_node_ptr (&branch)[2])
        {
            std::string& s0 =
                static_cast<details::string_nodes::string_range_node<Type>*>(branch[0])->ref();
            std::string& s1 =
                static_cast<details::string_nodes::string_range_node<Type>*>(branch[1])->ref();
            range_t rp0 =
                static_cast<details::string_nodes::string_range_node<Type>*>(branch[0])->range();
            range_t rp1 =
                static_cast<details::string_nodes::string_range_node<Type>*>(branch[1])->range();

            static_cast<details::string_nodes::string_range_node<Type>*>(branch[0])
                ->range_ref()
                .clear();
            static_cast<details::string_nodes::string_range_node<Type>*>(branch[1])
                ->range_ref()
                .clear();

            details::free_node(*node_allocator_, branch[0]);
            details::free_node(*node_allocator_, branch[1]);

            return synthesize_str_xroxr_expression_impl<std::string&, std::string&>(opr, s0, s1,
                                                                                    rp0, rp1);
        }

        inline expression_node_ptr synthesize_socs_expression(
            const core::operators::operator_type& opr, expression_node_ptr (&branch)[2])
        {
            std::string& s0 =
                static_cast<details::string_nodes::stringvar_node<Type>*>(branch[0])->ref();
            std::string s1 = static_cast<details::string_literal_node<Type>*>(branch[1])->str();

            details::free_node(*node_allocator_, branch[1]);

            return synthesize_sos_expression_impl<std::string&, const std::string>(opr, s0, s1);
        }

        inline expression_node_ptr synthesize_csos_expression(
            const core::operators::operator_type& opr, expression_node_ptr (&branch)[2])
        {
            std::string s0 = static_cast<details::string_literal_node<Type>*>(branch[0])->str();
            std::string& s1 =
                static_cast<details::string_nodes::stringvar_node<Type>*>(branch[1])->ref();

            details::free_node(*node_allocator_, branch[0]);

            return synthesize_sos_expression_impl<const std::string, std::string&>(opr, s0, s1);
        }

        inline expression_node_ptr synthesize_csosr_expression(
            const core::operators::operator_type& opr, expression_node_ptr (&branch)[2])
        {
            std::string s0 = static_cast<details::string_literal_node<Type>*>(branch[0])->str();
            std::string& s1 =
                static_cast<details::string_nodes::string_range_node<Type>*>(branch[1])->ref();
            range_t rp1 =
                static_cast<details::string_nodes::string_range_node<Type>*>(branch[1])->range();

            static_cast<details::string_nodes::string_range_node<Type>*>(branch[1])
                ->range_ref()
                .clear();

            details::free_node(*node_allocator_, branch[0]);
            details::free_node(*node_allocator_, branch[1]);

            return synthesize_str_xoxr_expression_impl<const std::string, std::string&>(opr, s0, s1,
                                                                                        rp1);
        }

        inline expression_node_ptr synthesize_srocs_expression(
            const core::operators::operator_type& opr, expression_node_ptr (&branch)[2])
        {
            std::string& s0 =
                static_cast<details::string_nodes::string_range_node<Type>*>(branch[0])->ref();
            std::string s1 = static_cast<details::string_literal_node<Type>*>(branch[1])->str();
            range_t rp0 =
                static_cast<details::string_nodes::string_range_node<Type>*>(branch[0])->range();

            static_cast<details::string_nodes::string_range_node<Type>*>(branch[0])
                ->range_ref()
                .clear();

            details::free_node(*node_allocator_, branch[0]);
            details::free_node(*node_allocator_, branch[1]);

            return synthesize_str_xrox_expression_impl<std::string&, const std::string>(opr, s0, s1,
                                                                                        rp0);
        }

        inline expression_node_ptr synthesize_srocsr_expression(
            const core::operators::operator_type& opr, expression_node_ptr (&branch)[2])
        {
            std::string& s0 =
                static_cast<details::string_nodes::string_range_node<Type>*>(branch[0])->ref();
            std::string s1 =
                static_cast<details::string_nodes::const_string_range_node<Type>*>(branch[1])
                    ->str();
            range_t rp0 =
                static_cast<details::string_nodes::string_range_node<Type>*>(branch[0])->range();
            range_t rp1 =
                static_cast<details::string_nodes::const_string_range_node<Type>*>(branch[1])
                    ->range();

            static_cast<details::string_nodes::string_range_node<Type>*>(branch[0])
                ->range_ref()
                .clear();
            static_cast<details::string_nodes::const_string_range_node<Type>*>(branch[1])
                ->range_ref()
                .clear();

            details::free_node(*node_allocator_, branch[0]);
            details::free_node(*node_allocator_, branch[1]);

            return synthesize_str_xroxr_expression_impl<std::string&, const std::string>(
                opr, s0, s1, rp0, rp1);
        }

        inline expression_node_ptr synthesize_csocs_expression(
            const core::operators::operator_type& opr, expression_node_ptr (&branch)[2])
        {
            const std::string s0 =
                static_cast<details::string_literal_node<Type>*>(branch[0])->str();
            const std::string s1 =
                static_cast<details::string_literal_node<Type>*>(branch[1])->str();

            expression_node_ptr result = error_node();

            if (core::operators::operator_type::add == opr)
                result = node_allocator_->allocate_c<details::string_literal_node<Type>>(s0 + s1);
            else if (core::operators::operator_type::in == opr)
                result = node_allocator_->allocate_c<details::literal_node<Type>>(
                    details::in_op<Type>::process(s0, s1));
            else if (core::operators::operator_type::like == opr)
                result = node_allocator_->allocate_c<details::literal_node<Type>>(
                    details::like_op<Type>::process(s0, s1));
            else if (core::operators::operator_type::ilike == opr)
                result = node_allocator_->allocate_c<details::literal_node<Type>>(
                    details::ilike_op<Type>::process(s0, s1));
            else
            {
                expression_node_ptr temp =
                    synthesize_sos_expression_impl<const std::string, const std::string>(opr, s0,
                                                                                         s1);

                const Type v = temp->value();

                details::free_node(*node_allocator_, temp);

                result = node_allocator_->allocate<literal_node_t>(v);
            }

            details::free_all_nodes(*node_allocator_, branch);

            return result;
        }

        inline expression_node_ptr synthesize_csocsr_expression(
            const core::operators::operator_type& opr, expression_node_ptr (&branch)[2])
        {
            const std::string s0 =
                static_cast<details::string_literal_node<Type>*>(branch[0])->str();
            std::string s1 =
                static_cast<details::string_nodes::const_string_range_node<Type>*>(branch[1])
                    ->str();
            range_t rp1 =
                static_cast<details::string_nodes::const_string_range_node<Type>*>(branch[1])
                    ->range();

            static_cast<details::string_nodes::const_string_range_node<Type>*>(branch[1])
                ->range_ref()
                .clear();

            details::free_node(*node_allocator_, branch[0]);
            details::free_node(*node_allocator_, branch[1]);

            return synthesize_str_xoxr_expression_impl<const std::string, const std::string>(
                opr, s0, s1, rp1);
        }

        inline expression_node_ptr synthesize_csros_expression(
            const core::operators::operator_type& opr, expression_node_ptr (&branch)[2])
        {
            std::string s0 =
                static_cast<details::string_nodes::const_string_range_node<Type>*>(branch[0])
                    ->str();
            std::string& s1 =
                static_cast<details::string_nodes::stringvar_node<Type>*>(branch[1])->ref();
            range_t rp0 =
                static_cast<details::string_nodes::const_string_range_node<Type>*>(branch[0])
                    ->range();

            static_cast<details::string_nodes::const_string_range_node<Type>*>(branch[0])
                ->range_ref()
                .clear();

            details::free_node(*node_allocator_, branch[0]);

            return synthesize_str_xrox_expression_impl<const std::string, std::string&>(opr, s0, s1,
                                                                                        rp0);
        }

        inline expression_node_ptr synthesize_csrosr_expression(
            const core::operators::operator_type& opr, expression_node_ptr (&branch)[2])
        {
            const std::string s0 =
                static_cast<details::string_nodes::const_string_range_node<Type>*>(branch[0])
                    ->str();
            std::string& s1 =
                static_cast<details::string_nodes::string_range_node<Type>*>(branch[1])->ref();
            const range_t rp0 =
                static_cast<details::string_nodes::const_string_range_node<Type>*>(branch[0])
                    ->range();
            const range_t rp1 =
                static_cast<details::string_nodes::string_range_node<Type>*>(branch[1])->range();

            static_cast<details::string_nodes::const_string_range_node<Type>*>(branch[0])
                ->range_ref()
                .clear();
            static_cast<details::string_nodes::string_range_node<Type>*>(branch[1])
                ->range_ref()
                .clear();

            details::free_node(*node_allocator_, branch[0]);
            details::free_node(*node_allocator_, branch[1]);

            return synthesize_str_xroxr_expression_impl<const std::string, std::string&>(
                opr, s0, s1, rp0, rp1);
        }

        inline expression_node_ptr synthesize_csrocs_expression(
            const core::operators::operator_type& opr, expression_node_ptr (&branch)[2])
        {
            const std::string s0 =
                static_cast<details::string_nodes::const_string_range_node<Type>*>(branch[0])
                    ->str();
            const std::string s1 =
                static_cast<details::string_literal_node<Type>*>(branch[1])->str();
            const range_t rp0 =
                static_cast<details::string_nodes::const_string_range_node<Type>*>(branch[0])
                    ->range();

            static_cast<details::string_nodes::const_string_range_node<Type>*>(branch[0])
                ->range_ref()
                .clear();

            details::free_all_nodes(*node_allocator_, branch);

            return synthesize_str_xrox_expression_impl<const std::string, std::string>(opr, s0, s1,
                                                                                       rp0);
        }

        inline expression_node_ptr synthesize_csrocsr_expression(
            const core::operators::operator_type& opr, expression_node_ptr (&branch)[2])
        {
            const std::string s0 =
                static_cast<details::string_nodes::const_string_range_node<Type>*>(branch[0])
                    ->str();
            const std::string s1 =
                static_cast<details::string_nodes::const_string_range_node<Type>*>(branch[1])
                    ->str();
            const range_t rp0 =
                static_cast<details::string_nodes::const_string_range_node<Type>*>(branch[0])
                    ->range();
            const range_t rp1 =
                static_cast<details::string_nodes::const_string_range_node<Type>*>(branch[1])
                    ->range();

            static_cast<details::string_nodes::const_string_range_node<Type>*>(branch[0])
                ->range_ref()
                .clear();
            static_cast<details::string_nodes::const_string_range_node<Type>*>(branch[1])
                ->range_ref()
                .clear();

            details::free_all_nodes(*node_allocator_, branch);

            return synthesize_str_xroxr_expression_impl<const std::string, const std::string>(
                opr, s0, s1, rp0, rp1);
        }

        inline expression_node_ptr synthesize_strogen_expression(
            const core::operators::operator_type& opr, expression_node_ptr (&branch)[2])
        {
            switch (opr)
            {
#define CASE_STMT(op0, op1)                                                                       \
    case op0:                                                                                     \
        return node_allocator_->allocate_ttt<typename details::str_sogens_node<Type, op1<Type>>>( \
            opr, branch[0], branch[1]);

                STRING_OPR_SWITCH_STATEMENTS
#undef CASE_STMT
                default:
                    return error_node();
            }
        }

#undef STRING_OPR_SWITCH_STATEMENTS
#endif

#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
        inline expression_node_ptr synthesize_string_expression(
            const core::operators::operator_type& opr, expression_node_ptr (&branch)[2])
        {
            if ((nullptr == branch[0]) || (nullptr == branch[1]))
            {
                details::free_all_nodes(*node_allocator_, branch);

                return error_node();
            }

            const bool b0_is_s = details::is_string_node(branch[0]);
            const bool b0_is_cs = details::is_const_string_node(branch[0]);
            const bool b0_is_sr = details::is_string_range_node(branch[0]);
            const bool b0_is_csr = details::is_const_string_range_node(branch[0]);

            const bool b1_is_s = details::is_string_node(branch[1]);
            const bool b1_is_cs = details::is_const_string_node(branch[1]);
            const bool b1_is_sr = details::is_string_range_node(branch[1]);
            const bool b1_is_csr = details::is_const_string_range_node(branch[1]);

            const bool b0_is_gen = details::is_string_assignment_node(branch[0]) ||
                                   details::is_genricstring_range_node(branch[0]) ||
                                   details::is_string_concat_node(branch[0]) ||
                                   details::is_string_function_node(branch[0]) ||
                                   details::is_string_condition_node(branch[0]) ||
                                   details::is_string_ccondition_node(branch[0]) ||
                                   details::is_string_vararg_node(branch[0]);

            const bool b1_is_gen = details::is_string_assignment_node(branch[1]) ||
                                   details::is_genricstring_range_node(branch[1]) ||
                                   details::is_string_concat_node(branch[1]) ||
                                   details::is_string_function_node(branch[1]) ||
                                   details::is_string_condition_node(branch[1]) ||
                                   details::is_string_ccondition_node(branch[1]) ||
                                   details::is_string_vararg_node(branch[1]);

            if (core::operators::operator_type::add == opr)
            {
                if (!b0_is_cs || !b1_is_cs)
                {
                    return synthesize_expression<string_concat_node_t, 2>(opr, branch);
                }
            }

            if (b0_is_gen || b1_is_gen)
            {
                return synthesize_strogen_expression(opr, branch);
            }
            else if (b0_is_s)
            {
                if (b1_is_s)
                    return synthesize_sos_expression(opr, branch);
                else if (b1_is_cs)
                    return synthesize_socs_expression(opr, branch);
                else if (b1_is_sr)
                    return synthesize_sosr_expression(opr, branch);
                else if (b1_is_csr)
                    return synthesize_socsr_expression(opr, branch);
            }
            else if (b0_is_cs)
            {
                if (b1_is_s)
                    return synthesize_csos_expression(opr, branch);
                else if (b1_is_cs)
                    return synthesize_csocs_expression(opr, branch);
                else if (b1_is_sr)
                    return synthesize_csosr_expression(opr, branch);
                else if (b1_is_csr)
                    return synthesize_csocsr_expression(opr, branch);
            }
            else if (b0_is_sr)
            {
                if (b1_is_s)
                    return synthesize_sros_expression(opr, branch);
                else if (b1_is_sr)
                    return synthesize_srosr_expression(opr, branch);
                else if (b1_is_cs)
                    return synthesize_srocs_expression(opr, branch);
                else if (b1_is_csr)
                    return synthesize_srocsr_expression(opr, branch);
            }
            else if (b0_is_csr)
            {
                if (b1_is_s)
                    return synthesize_csros_expression(opr, branch);
                else if (b1_is_sr)
                    return synthesize_csrosr_expression(opr, branch);
                else if (b1_is_cs)
                    return synthesize_csrocs_expression(opr, branch);
                else if (b1_is_csr)
                    return synthesize_csrocsr_expression(opr, branch);
            }

            return error_node();
        }
#else
        inline expression_node_ptr synthesize_string_expression(
            const core::operators::operator_type&, expression_node_ptr (&branch)[2])
        {
            details::free_all_nodes(*node_allocator_, branch);
            return error_node();
        }
#endif

#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
        inline expression_node_ptr synthesize_string_expression(
            const core::operators::operator_type& opr, expression_node_ptr (&branch)[3])
        {
            if (core::operators::operator_type::inrange != opr)
                return error_node();
            else if ((nullptr == branch[0]) || (nullptr == branch[1]) || (nullptr == branch[2]))
            {
                details::free_all_nodes(*node_allocator_, branch);

                return error_node();
            }
            else if (details::is_const_string_node(branch[0]) &&
                     details::is_const_string_node(branch[1]) &&
                     details::is_const_string_node(branch[2]))
            {
                const std::string s0 =
                    static_cast<details::string_literal_node<Type>*>(branch[0])->str();
                const std::string s1 =
                    static_cast<details::string_literal_node<Type>*>(branch[1])->str();
                const std::string s2 =
                    static_cast<details::string_literal_node<Type>*>(branch[2])->str();

                const Type v = (((s0 <= s1) && (s1 <= s2)) ? Type(1) : Type(0));

                details::free_all_nodes(*node_allocator_, branch);

                return node_allocator_->allocate_c<details::literal_node<Type>>(v);
            }
            else if (details::is_string_node(branch[0]) && details::is_string_node(branch[1]) &&
                     details::is_string_node(branch[2]))
            {
                std::string& s0 =
                    static_cast<details::string_nodes::stringvar_node<Type>*>(branch[0])->ref();
                std::string& s1 =
                    static_cast<details::string_nodes::stringvar_node<Type>*>(branch[1])->ref();
                std::string& s2 =
                    static_cast<details::string_nodes::stringvar_node<Type>*>(branch[2])->ref();

                using inrange_t =
                    typename details::sosos_node<Type, std::string&, std::string&, std::string&,
                                                 details::inrange_op<Type>>;

                return node_allocator_
                    ->allocate_type<inrange_t, std::string&, std::string&, std::string&>(s0, s1,
                                                                                         s2);
            }
            else if (details::is_const_string_node(branch[0]) &&
                     details::is_string_node(branch[1]) && details::is_const_string_node(branch[2]))
            {
                std::string s0 = static_cast<details::string_literal_node<Type>*>(branch[0])->str();
                std::string& s1 =
                    static_cast<details::string_nodes::stringvar_node<Type>*>(branch[1])->ref();
                std::string s2 = static_cast<details::string_literal_node<Type>*>(branch[2])->str();

                using inrange_t =
                    typename details::sosos_node<Type, std::string, std::string&, std::string,
                                                 details::inrange_op<Type>>;

                details::free_node(*node_allocator_, branch[0]);
                details::free_node(*node_allocator_, branch[2]);

                return node_allocator_
                    ->allocate_type<inrange_t, std::string, std::string&, std::string>(s0, s1, s2);
            }
            else if (details::is_string_node(branch[0]) &&
                     details::is_const_string_node(branch[1]) && details::is_string_node(branch[2]))
            {
                std::string& s0 =
                    static_cast<details::string_nodes::stringvar_node<Type>*>(branch[0])->ref();
                std::string s1 = static_cast<details::string_literal_node<Type>*>(branch[1])->str();
                std::string& s2 =
                    static_cast<details::string_nodes::stringvar_node<Type>*>(branch[2])->ref();

                using inrange_t =
                    typename details::sosos_node<Type, std::string&, std::string, std::string&,
                                                 details::inrange_op<Type>>;

                details::free_node(*node_allocator_, branch[1]);

                return node_allocator_
                    ->allocate_type<inrange_t, std::string&, std::string, std::string&>(s0, s1, s2);
            }
            else if (details::is_string_node(branch[0]) && details::is_string_node(branch[1]) &&
                     details::is_const_string_node(branch[2]))
            {
                std::string& s0 =
                    static_cast<details::string_nodes::stringvar_node<Type>*>(branch[0])->ref();
                std::string& s1 =
                    static_cast<details::string_nodes::stringvar_node<Type>*>(branch[1])->ref();
                std::string s2 = static_cast<details::string_literal_node<Type>*>(branch[2])->str();

                using inrange_t =
                    typename details::sosos_node<Type, std::string&, std::string&, std::string,
                                                 details::inrange_op<Type>>;

                details::free_node(*node_allocator_, branch[2]);

                return node_allocator_
                    ->allocate_type<inrange_t, std::string&, std::string&, std::string>(s0, s1, s2);
            }
            else if (details::is_const_string_node(branch[0]) &&
                     details::is_string_node(branch[1]) && details::is_string_node(branch[2]))
            {
                std::string s0 = static_cast<details::string_literal_node<Type>*>(branch[0])->str();
                std::string& s1 =
                    static_cast<details::string_nodes::stringvar_node<Type>*>(branch[1])->ref();
                std::string& s2 =
                    static_cast<details::string_nodes::stringvar_node<Type>*>(branch[2])->ref();

                using inrange_t =
                    typename details::sosos_node<Type, std::string, std::string&, std::string&,
                                                 details::inrange_op<Type>>;

                details::free_node(*node_allocator_, branch[0]);

                return node_allocator_
                    ->allocate_type<inrange_t, std::string, std::string&, std::string&>(s0, s1, s2);
            }
            else
                return error_node();
        }
#else
        inline expression_node_ptr synthesize_string_expression(
            const core::operators::operator_type&, expression_node_ptr (&branch)[3])
        {
            details::free_all_nodes(*node_allocator_, branch);
            return error_node();
        }
#endif

        inline expression_node_ptr synthesize_null_expression(
            const core::operators::operator_type& operation, expression_node_ptr (&branch)[2])
        {
            /*
             Note: The following are the type promotion rules
             that relate to operations that include 'null':
             0. null ==/!=     null --> true false
             1. null operation null --> null
             2. x    ==/!=     null --> true/false
             3. null ==/!=     x    --> true/false
             4. x   operation  null --> x
             5. null operation x    --> x
            */

            using nulleq_node_t = typename details::null_eq_node<T>;

            const bool b0_null = details::is_null_node(branch[0]);
            const bool b1_null = details::is_null_node(branch[1]);

            if (b0_null && b1_null)
            {
                expression_node_ptr result = error_node();

                if (core::operators::operator_type::eq == operation)
                    result = node_allocator_->allocate_c<literal_node_t>(T(1));
                else if (core::operators::operator_type::ne == operation)
                    result = node_allocator_->allocate_c<literal_node_t>(T(0));

                if (result)
                {
                    details::free_node(*node_allocator_, branch[0]);
                    details::free_node(*node_allocator_, branch[1]);

                    return result;
                }

                details::free_node(*node_allocator_, branch[1]);

                return branch[0];
            }
            else if (core::operators::operator_type::eq == operation)
            {
                expression_node_ptr result =
                    node_allocator_->allocate_rc<nulleq_node_t>(branch[b0_null ? 1 : 0], true);

                details::free_node(*node_allocator_, branch[b0_null ? 0 : 1]);

                return result;
            }
            else if (core::operators::operator_type::ne == operation)
            {
                expression_node_ptr result =
                    node_allocator_->allocate_rc<nulleq_node_t>(branch[b0_null ? 1 : 0], false);

                details::free_node(*node_allocator_, branch[b0_null ? 0 : 1]);

                return result;
            }
            else if (b0_null)
            {
                details::free_node(*node_allocator_, branch[0]);
                branch[0] = branch[1];
                branch[1] = error_node();
            }
            else if (b1_null)
            {
                details::free_node(*node_allocator_, branch[1]);
                branch[1] = error_node();
            }

            if ((core::operators::operator_type::add == operation) ||
                (core::operators::operator_type::sub == operation) ||
                (core::operators::operator_type::mul == operation) ||
                (core::operators::operator_type::div == operation) ||
                (core::operators::operator_type::mod == operation) ||
                (core::operators::operator_type::pow == operation))
            {
                return branch[0];
            }

            details::free_node(*node_allocator_, branch[0]);

            if ((core::operators::operator_type::lt == operation) ||
                (core::operators::operator_type::lte == operation) ||
                (core::operators::operator_type::gt == operation) ||
                (core::operators::operator_type::gte == operation) ||
                (core::operators::operator_type::logical_and == operation) ||
                (core::operators::operator_type::nand == operation) ||
                (core::operators::operator_type::logical_or == operation) ||
                (core::operators::operator_type::nor == operation) ||
                (core::operators::operator_type::logical_xor == operation) ||
                (core::operators::operator_type::xnor == operation) ||
                (core::operators::operator_type::in == operation) ||
                (core::operators::operator_type::like == operation) ||
                (core::operators::operator_type::ilike == operation))
            {
                return node_allocator_->allocate_c<literal_node_t>(T(0));
            }

            return node_allocator_->allocate<details::null_node<Type>>();
        }

        template <typename NodeType, std::size_t N>
        inline expression_node_ptr synthesize_expression(
            const core::operators::operator_type& operation, expression_node_ptr (&branch)[N])
        {
            if ((core::operators::operator_type::in == operation) ||
                (core::operators::operator_type::like == operation) ||
                (core::operators::operator_type::ilike == operation))
            {
                free_all_nodes(*node_allocator_, branch);

                return error_node();
            }
            else if (!details::all_nodes_valid<N>(branch))
            {
                free_all_nodes(*node_allocator_, branch);

                return error_node();
            }
            else if ((core::operators::operator_type::default_op != operation))
            {
                // Attempt simple constant folding optimisation.
                expression_node_ptr expression_point =
                    node_allocator_->allocate<NodeType>(operation, branch);

                if (fold_passes_t::is_constant_foldable(branch))
                {
                    const Type v = expression_point->value();
                    details::free_node(*node_allocator_, expression_point);

                    return node_allocator_->allocate<literal_node_t>(v);
                }

                if (expression_point && expression_point->valid())
                {
                    return expression_point;
                }

                parser_->set_error(parser_error::make_error(
                    parser_error::error_mode::e_parser, token_t(),
                    "ERR281 - Failed to synthesize node: NodeType", core::error_location()));

                details::free_node(*node_allocator_, expression_point);
            }

            return error_node();
        }

        template <typename NodeType, std::size_t N>
        inline expression_node_ptr synthesize_expression(F* f, expression_node_ptr (&branch)[N])
        {
            if (!details::all_nodes_valid<N>(branch))
            {
                free_all_nodes(*node_allocator_, branch);

                return error_node();
            }

            using function_N_node_t = typename details::function_N_node<T, ifunction_t, N>;

            // Attempt simple constant folding optimisation.

            expression_node_ptr expression_point = node_allocator_->allocate<NodeType>(f);
            function_N_node_t* func_node_ptr = static_cast<function_N_node_t*>(expression_point);

            if (nullptr == func_node_ptr)
            {
                free_all_nodes(*node_allocator_, branch);

                return error_node();
            }
            else
                func_node_ptr->init_branches(branch);

            if (fold_passes_t::is_constant_foldable(branch) && !f->has_side_effects())
            {
                Type v = expression_point->value();
                details::free_node(*node_allocator_, expression_point);

                return node_allocator_->allocate<literal_node_t>(v);
            }

            parser_->state_.activate_side_effect("synthesize_expression(function<NT,N>)");

            return expression_point;
        }

        bool strength_reduction_enabled_;
        details::node_allocator* node_allocator_;
        synthesize_map_t synthesize_map_;
        unary_op_map_t* unary_op_map_;
        binary_op_map_t* binary_op_map_;
        inv_binary_op_map_t* inv_binary_op_map_;
        sf3_map_t* sf3_map_;
        sf4_map_t* sf4_map_;
        parser_t* parser_;
    };  // class expression_generator

    inline void set_error(const parser_error::type& error_type)
    {
        error_list_.push_back(error_type);
    }

    inline void remove_last_error()
    {
        if (!error_list_.empty())
        {
            error_list_.pop_back();
        }
    }

    inline void set_synthesis_error(const std::string& synthesis_error_message)
    {
        if (synthesis_error_.empty())
        {
            synthesis_error_ = synthesis_error_message;
        }
    }

    inline void register_local_vars(expression<T>& e)
    {
        for (std::size_t i = 0; i < sem_.size(); ++i)
        {
            scope_element& se = sem_.get_element(i);

            core::debug_print("register_local_vars() - se[%s]\n", se.name.c_str());

            if ((scope_element::element_type::e_variable == se.type) ||
                (scope_element::element_type::e_literal == se.type) ||
                (scope_element::element_type::e_vecelem == se.type))
            {
                if (se.var_node)
                {
                    e.register_local_var(se.var_node);
                }

                if (se.scalar_data)
                {
                    e.register_local_data(std::move(se.scalar_data), 1);
                }
            }
            else if (scope_element::element_type::e_vector == se.type)
            {
                if (se.vec_node)
                {
                    e.register_local_var(se.vec_node);
                }

                if (se.vector_data)
                {
                    e.register_local_data(std::move(se.vector_data), se.size);
                }
            }
#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
            else if (scope_element::element_type::e_string == se.type)
            {
                if (se.str_node)
                {
                    e.register_local_var(se.str_node);
                }

                if (se.str_data)
                {
                    e.register_local_data(std::move(se.str_data), se.size);
                }
            }
#endif

            se.var_node = nullptr;
            se.vec_node = nullptr;
#ifndef MATH_EXPR_DISABLE_STRING_CAPABILITIES
            se.str_node = nullptr;
#endif
            se.ref_count = 0;
            se.active = false;
        }
    }

    inline void register_return_results(expression<T>& e)
    {
        e.register_return_results(results_context_);
        results_context_ = 0;
    }

    inline results_context_t& results_ctx()
    {
        if (nullptr == results_context_)
        {
            results_context_ = new results_context_t();
        }

        return (*results_context_);
    }

    inline void return_cleanup()
    {
#ifndef MATH_EXPR_DISABLE_RETURN_STATEMENT
        if (results_context_)
        {
            delete results_context_;
            results_context_ = 0;
        }

        state_.return_stmt_present = false;
#endif
    }

    inline bool valid_settings()
    {
        const std::size_t max_local_vector_size_bytes =
            sizeof(T) * settings_.max_local_vector_size();

        if (max_local_vector_size_bytes > settings_.max_total_local_symbol_size_bytes())
        {
            set_error(make_error(
                parser_error::error_mode::e_parser,
                "ERR282 - Max local vector size of " + core::to_str(max_local_vector_size_bytes) +
                    " bytes "
                    "is larger than max total local symbol size of " +
                    core::to_str(settings_.max_total_local_symbol_size_bytes()) + " bytes",
                core::error_location()));

            return false;
        }

        return true;
    }

   private:
    parser(const parser<T>&) = delete;
    parser<T>& operator=(const parser<T>&) = delete;

    settings_store settings_;
    expression_generator<T> expression_generator_;
    details::node_allocator node_allocator_;
    symtab_store_t symtab_store_;
    dependent_entity_collector dec_;
    std::deque<parser_error::type> error_list_;
    std::deque<bool> brkcnt_list_;
    parser_state state_;
    bool resolve_unknown_symbol_;
    results_context_t* results_context_;
    unknown_symbol_resolver* unknown_symbol_resolver_;
    unknown_symbol_resolver default_usr_;
    base_ops_map_t base_ops_map_;
    unary_op_map_t unary_op_map_;
    binary_op_map_t binary_op_map_;
    inv_binary_op_map_t inv_binary_op_map_;
    sf3_map_t sf3_map_;
    sf4_map_t sf4_map_;
    std::string synthesis_error_;
    scope_element_manager sem_;
    std::vector<expression_state_t> current_state_stack_;

    immutable_memory_map_t immutable_memory_map_;
    immutable_symtok_map_t immutable_symtok_map_;

    lexer::helper::helper_assembly helper_assembly_;

    lexer::helper::commutative_inserter commutative_inserter_;
    lexer::helper::operator_joiner operator_joiner_2_;
    lexer::helper::operator_joiner operator_joiner_3_;
    lexer::helper::symbol_replacer symbol_replacer_;
    lexer::helper::bracket_checker bracket_checker_;
    lexer::helper::numeric_checker<T> numeric_checker_;
    lexer::helper::sequence_validator sequence_validator_;
    lexer::helper::sequence_validator_3tokens sequence_validator_3tkns_;

    loop_runtime_check_ptr loop_runtime_check_;
    vector_access_runtime_check<T>* vector_access_runtime_check_;
    compilation_check_ptr compilation_check_ptr_;
    assert_check_ptr assert_check_;
    std::set<std::string> assert_ids_;

    template <typename ParserType>
    friend void details::disable_type_checking(ParserType& p);
};  // class parser

}  // namespace math_expr

#endif
