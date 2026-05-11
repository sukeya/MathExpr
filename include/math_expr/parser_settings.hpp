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

#ifndef MATH_EXPR_PARSER_SETTINGS_HPP
#define MATH_EXPR_PARSER_SETTINGS_HPP

#include "math_expr/compilation_options.hpp"
#include "math_expr/core/operator_types.hpp"
#include "math_expr/core/string_utils.hpp"

namespace math_expr
{

template <typename T>
class parser;

template <typename T>
class settings_store
{
   private:
    using disabled_entity_set_t = std::set<std::string, core::ilesscompare>;
    using des_itr_t = disabled_entity_set_t::iterator;

   public:
    enum settings_base_funcs
    {
        e_bf_unknown = 0,
        e_bf_abs,
        e_bf_acos,
        e_bf_acosh,
        e_bf_asin,
        e_bf_asinh,
        e_bf_atan,
        e_bf_atan2,
        e_bf_atanh,
        e_bf_avg,
        e_bf_ceil,
        e_bf_clamp,
        e_bf_cos,
        e_bf_cosh,
        e_bf_cot,
        e_bf_csc,
        e_bf_equal,
        e_bf_erf,
        e_bf_erfc,
        e_bf_exp,
        e_bf_expm1,
        e_bf_floor,
        e_bf_frac,
        e_bf_hypot,
        e_bf_iclamp,
        e_bf_like,
        e_bf_log,
        e_bf_log10,
        e_bf_log1p,
        e_bf_log2,
        e_bf_logn,
        e_bf_mand,
        e_bf_max,
        e_bf_min,
        e_bf_mod,
        e_bf_mor,
        e_bf_mul,
        e_bf_ncdf,
        e_bf_pow,
        e_bf_root,
        e_bf_round,
        e_bf_roundn,
        e_bf_sec,
        e_bf_sgn,
        e_bf_sin,
        e_bf_sinc,
        e_bf_sinh,
        e_bf_sqrt,
        e_bf_sum,
        e_bf_swap,
        e_bf_tan,
        e_bf_tanh,
        e_bf_trunc,
        e_bf_not_equal,
        e_bf_inrange,
        e_bf_deg2grad,
        e_bf_deg2rad,
        e_bf_rad2deg,
        e_bf_grad2deg
    };

    enum settings_control_structs
    {
        e_ctrl_unknown = 0,
        e_ctrl_ifelse,
        e_ctrl_switch,
        e_ctrl_for_loop,
        e_ctrl_while_loop,
        e_ctrl_repeat_loop,
        e_ctrl_return
    };

    enum settings_logic_opr
    {
        e_logic_unknown = 0,
        e_logic_and,
        e_logic_nand,
        e_logic_nor,
        e_logic_not,
        e_logic_or,
        e_logic_xnor,
        e_logic_xor,
        e_logic_scand,
        e_logic_scor
    };

    enum settings_arithmetic_opr
    {
        e_arith_unknown = 0,
        e_arith_add,
        e_arith_sub,
        e_arith_mul,
        e_arith_div,
        e_arith_mod,
        e_arith_pow
    };

    enum settings_assignment_opr
    {
        e_assign_unknown = 0,
        e_assign_assign,
        e_assign_addass,
        e_assign_subass,
        e_assign_mulass,
        e_assign_divass,
        e_assign_modass
    };

    enum settings_inequality_opr
    {
        e_ineq_unknown = 0,
        e_ineq_lt,
        e_ineq_lte,
        e_ineq_eq,
        e_ineq_equal,
        e_ineq_ne,
        e_ineq_gte,
        e_ineq_gt
    };

    inline static const compilation_options default_compile_all_opts =
        compilation_options::default_all();

    settings_store(const compilation_options compile_options = default_compile_all_opts)
        : max_stack_depth_(400),
          max_node_depth_(10000),
          max_total_local_symbol_size_bytes_(2000000000),
          max_local_vector_size_(max_total_local_symbol_size_bytes_ / sizeof(T))
    {
        load_compile_options(compile_options);
    }

    settings_store& enable_all_base_functions()
    {
        disabled_func_set_.clear();
        return (*this);
    }

    settings_store& enable_all_control_structures()
    {
        disabled_ctrl_set_.clear();
        return (*this);
    }

    settings_store& enable_all_logic_ops()
    {
        disabled_logic_set_.clear();
        return (*this);
    }

    settings_store& enable_all_arithmetic_ops()
    {
        disabled_arithmetic_set_.clear();
        return (*this);
    }

    settings_store& enable_all_assignment_ops()
    {
        disabled_assignment_set_.clear();
        return (*this);
    }

    settings_store& enable_all_inequality_ops()
    {
        disabled_inequality_set_.clear();
        return (*this);
    }

    settings_store& enable_local_vardef()
    {
        disable_vardef_ = false;
        return (*this);
    }

    settings_store& enable_commutative_check()
    {
        enable_commutative_check_ = true;
        return (*this);
    }

    settings_store& enable_strength_reduction()
    {
        enable_strength_reduction_ = true;
        return (*this);
    }

    settings_store& disable_all_base_functions()
    {
        for (std::size_t i = 0; i < core::base_function_list_size; ++i)
        {
            disabled_func_set_.insert(std::string(core::base_function_list[i]));
        }
        return (*this);
    }

    settings_store& disable_all_control_structures()
    {
        for (std::size_t i = 0; i < core::cntrl_struct_list_size; ++i)
        {
            disabled_ctrl_set_.insert(std::string(core::cntrl_struct_list[i]));
        }
        return (*this);
    }

    settings_store& disable_all_logic_ops()
    {
        for (std::size_t i = 0; i < core::logic_ops_list_size; ++i)
        {
            disabled_logic_set_.insert(std::string(core::logic_ops_list[i]));
        }
        return (*this);
    }

    settings_store& disable_all_arithmetic_ops()
    {
        for (std::size_t i = 0; i < core::arithmetic_ops_list_size; ++i)
        {
            disabled_arithmetic_set_.insert(std::string(core::arithmetic_ops_list[i]));
        }
        return (*this);
    }

    settings_store& disable_all_assignment_ops()
    {
        for (std::size_t i = 0; i < core::assignment_ops_list_size; ++i)
        {
            disabled_assignment_set_.insert(std::string(core::assignment_ops_list[i]));
        }
        return (*this);
    }

    settings_store& disable_all_inequality_ops()
    {
        for (std::size_t i = 0; i < core::inequality_ops_list_size; ++i)
        {
            disabled_inequality_set_.insert(std::string(core::inequality_ops_list[i]));
        }
        return (*this);
    }

    settings_store& disable_local_vardef()
    {
        disable_vardef_ = true;
        return (*this);
    }

    settings_store& disable_commutative_check()
    {
        enable_commutative_check_ = false;
        return (*this);
    }

    settings_store& disable_strength_reduction()
    {
        enable_strength_reduction_ = false;
        return (*this);
    }

    bool replacer_enabled() const
    {
        return enable_replacer_;
    }
    bool commutative_check_enabled() const
    {
        return enable_commutative_check_;
    }
    bool joiner_enabled() const
    {
        return enable_joiner_;
    }
    bool numeric_check_enabled() const
    {
        return enable_numeric_check_;
    }
    bool bracket_check_enabled() const
    {
        return enable_bracket_check_;
    }
    bool sequence_check_enabled() const
    {
        return enable_sequence_check_;
    }
    bool strength_reduction_enabled() const
    {
        return enable_strength_reduction_;
    }
    bool collect_variables_enabled() const
    {
        return enable_collect_vars_;
    }
    bool collect_functions_enabled() const
    {
        return enable_collect_funcs_;
    }
    bool collect_assignments_enabled() const
    {
        return enable_collect_assings_;
    }
    bool vardef_disabled() const
    {
        return disable_vardef_;
    }
    bool rsrvd_sym_usr_disabled() const
    {
        return disable_rsrvd_sym_usr_;
    }
    bool zero_return_disabled() const
    {
        return disable_zero_return_;
    }

    bool function_enabled(const std::string& function_name) const
    {
        if (disabled_func_set_.empty())
            return true;
        else
            return (disabled_func_set_.end() == disabled_func_set_.find(function_name));
    }

    bool control_struct_enabled(const std::string& control_struct) const
    {
        if (disabled_ctrl_set_.empty())
            return true;
        else
            return (disabled_ctrl_set_.end() == disabled_ctrl_set_.find(control_struct));
    }

    bool logic_enabled(const std::string& logic_operation) const
    {
        if (disabled_logic_set_.empty())
            return true;
        else
            return (disabled_logic_set_.end() == disabled_logic_set_.find(logic_operation));
    }

    bool arithmetic_enabled(const core::operators::operator_type& arithmetic_operation) const
    {
        if (disabled_logic_set_.empty())
            return true;
        else
            return disabled_arithmetic_set_.end() ==
                   disabled_arithmetic_set_.find(arith_opr_to_string(arithmetic_operation));
    }

    bool assignment_enabled(const core::operators::operator_type& assignment) const
    {
        if (disabled_assignment_set_.empty())
            return true;
        else
            return disabled_assignment_set_.end() ==
                   disabled_assignment_set_.find(assign_opr_to_string(assignment));
    }

    bool inequality_enabled(const core::operators::operator_type& inequality) const
    {
        if (disabled_inequality_set_.empty())
            return true;
        else
            return disabled_inequality_set_.end() ==
                   disabled_inequality_set_.find(inequality_opr_to_string(inequality));
    }

    bool function_disabled(const std::string& function_name) const
    {
        if (disabled_func_set_.empty())
            return false;
        else
            return (disabled_func_set_.end() != disabled_func_set_.find(function_name));
    }

    bool control_struct_disabled(const std::string& control_struct) const
    {
        if (disabled_ctrl_set_.empty())
            return false;
        else
            return (disabled_ctrl_set_.end() != disabled_ctrl_set_.find(control_struct));
    }

    bool logic_disabled(const std::string& logic_operation) const
    {
        if (disabled_logic_set_.empty())
            return false;
        else
            return (disabled_logic_set_.end() != disabled_logic_set_.find(logic_operation));
    }

    bool assignment_disabled(const core::operators::operator_type assignment_operation) const
    {
        if (disabled_assignment_set_.empty())
            return false;
        else
            return disabled_assignment_set_.end() !=
                   disabled_assignment_set_.find(assign_opr_to_string(assignment_operation));
    }

    bool logic_disabled(const core::operators::operator_type logic_operation) const
    {
        if (disabled_logic_set_.empty())
            return false;
        else
            return disabled_logic_set_.end() !=
                   disabled_logic_set_.find(logic_opr_to_string(logic_operation));
    }

    bool arithmetic_disabled(const core::operators::operator_type arithmetic_operation) const
    {
        if (disabled_arithmetic_set_.empty())
            return false;
        else
            return disabled_arithmetic_set_.end() !=
                   disabled_arithmetic_set_.find(arith_opr_to_string(arithmetic_operation));
    }

    bool inequality_disabled(const core::operators::operator_type& inequality) const
    {
        if (disabled_inequality_set_.empty())
            return false;
        else
            return disabled_inequality_set_.end() !=
                   disabled_inequality_set_.find(inequality_opr_to_string(inequality));
    }

    settings_store& disable_base_function(const settings_base_funcs bf)
    {
        if ((e_bf_unknown != bf) &&
            (static_cast<std::size_t>(bf) < (core::base_function_list_size + 1)))
        {
            disabled_func_set_.insert(std::string(core::base_function_list[bf - 1]));
        }

        return (*this);
    }

    settings_store& disable_control_structure(const settings_control_structs ctrl_struct)
    {
        if ((e_ctrl_unknown != ctrl_struct) &&
            (static_cast<std::size_t>(ctrl_struct) < (core::cntrl_struct_list_size + 1)))
        {
            disabled_ctrl_set_.insert(std::string(core::cntrl_struct_list[ctrl_struct - 1]));
        }

        return (*this);
    }

    settings_store& disable_logic_operation(const settings_logic_opr logic)
    {
        if ((e_logic_unknown != logic) &&
            (static_cast<std::size_t>(logic) < (core::logic_ops_list_size + 1)))
        {
            disabled_logic_set_.insert(std::string(core::logic_ops_list[logic - 1]));
        }

        return (*this);
    }

    settings_store& disable_arithmetic_operation(const settings_arithmetic_opr arithmetic)
    {
        if ((e_arith_unknown != arithmetic) &&
            (static_cast<std::size_t>(arithmetic) < (core::arithmetic_ops_list_size + 1)))
        {
            disabled_arithmetic_set_.insert(std::string(core::arithmetic_ops_list[arithmetic - 1]));
        }

        return (*this);
    }

    settings_store& disable_assignment_operation(const settings_assignment_opr assignment)
    {
        if ((e_assign_unknown != assignment) &&
            (static_cast<std::size_t>(assignment) < (core::assignment_ops_list_size + 1)))
        {
            disabled_assignment_set_.insert(std::string(core::assignment_ops_list[assignment - 1]));
        }

        return (*this);
    }

    settings_store& disable_inequality_operation(const settings_inequality_opr inequality)
    {
        if ((e_ineq_unknown != inequality) &&
            (static_cast<std::size_t>(inequality) < (core::inequality_ops_list_size + 1)))
        {
            disabled_inequality_set_.insert(std::string(core::inequality_ops_list[inequality - 1]));
        }

        return (*this);
    }

    settings_store& enable_base_function(const settings_base_funcs bf)
    {
        if ((e_bf_unknown != bf) &&
            (static_cast<std::size_t>(bf) < (core::base_function_list_size + 1)))
        {
            const des_itr_t itr =
                disabled_func_set_.find(std::string(core::base_function_list[bf - 1]));

            if (disabled_func_set_.end() != itr)
            {
                disabled_func_set_.erase(itr);
            }
        }

        return (*this);
    }

    settings_store& enable_control_structure(const settings_control_structs ctrl_struct)
    {
        if ((e_ctrl_unknown != ctrl_struct) &&
            (static_cast<std::size_t>(ctrl_struct) < (core::cntrl_struct_list_size + 1)))
        {
            const des_itr_t itr =
                disabled_ctrl_set_.find(std::string(core::cntrl_struct_list[ctrl_struct - 1]));

            if (disabled_ctrl_set_.end() != itr)
            {
                disabled_ctrl_set_.erase(itr);
            }
        }

        return (*this);
    }

    settings_store& enable_logic_operation(const settings_logic_opr logic)
    {
        if ((e_logic_unknown != logic) &&
            (static_cast<std::size_t>(logic) < (core::logic_ops_list_size + 1)))
        {
            const des_itr_t itr =
                disabled_logic_set_.find(std::string(core::logic_ops_list[logic - 1]));

            if (disabled_logic_set_.end() != itr)
            {
                disabled_logic_set_.erase(itr);
            }
        }

        return (*this);
    }

    settings_store& enable_arithmetic_operation(const settings_arithmetic_opr arithmetic)
    {
        if ((e_arith_unknown != arithmetic) &&
            (static_cast<std::size_t>(arithmetic) < (core::arithmetic_ops_list_size + 1)))
        {
            const des_itr_t itr = disabled_arithmetic_set_.find(
                std::string(core::arithmetic_ops_list[arithmetic - 1]));

            if (disabled_arithmetic_set_.end() != itr)
            {
                disabled_arithmetic_set_.erase(itr);
            }
        }

        return (*this);
    }

    settings_store& enable_assignment_operation(const settings_assignment_opr assignment)
    {
        if ((e_assign_unknown != assignment) &&
            (static_cast<std::size_t>(assignment) < (core::assignment_ops_list_size + 1)))
        {
            const des_itr_t itr = disabled_assignment_set_.find(
                std::string(core::assignment_ops_list[assignment - 1]));

            if (disabled_assignment_set_.end() != itr)
            {
                disabled_assignment_set_.erase(itr);
            }
        }

        return (*this);
    }

    settings_store& enable_inequality_operation(const settings_inequality_opr inequality)
    {
        if ((e_ineq_unknown != inequality) &&
            (static_cast<std::size_t>(inequality) < (core::inequality_ops_list_size + 1)))
        {
            const des_itr_t itr = disabled_inequality_set_.find(
                std::string(core::inequality_ops_list[inequality - 1]));

            if (disabled_inequality_set_.end() != itr)
            {
                disabled_inequality_set_.erase(itr);
            }
        }

        return (*this);
    }

    void set_max_stack_depth(const std::size_t max_stack_depth)
    {
        max_stack_depth_ = max_stack_depth;
    }

    void set_max_node_depth(const std::size_t max_node_depth)
    {
        max_node_depth_ = max_node_depth;
    }

    void set_max_local_vector_size(const std::size_t max_local_vector_size)
    {
        max_local_vector_size_ = max_local_vector_size;
    }

    void set_max_total_local_symbol_size_bytes(const std::size_t max_total_lcl_symb_size)
    {
        max_total_local_symbol_size_bytes_ = max_total_lcl_symb_size;
    }

    std::size_t max_stack_depth() const
    {
        return max_stack_depth_;
    }

    std::size_t max_node_depth() const
    {
        return max_node_depth_;
    }

    std::size_t max_local_vector_size() const
    {
        return max_local_vector_size_;
    }

    std::size_t max_total_local_symbol_size_bytes() const
    {
        return max_total_local_symbol_size_bytes_;
    }

   private:
    void load_compile_options(const compilation_options compile_options)
    {
        enable_replacer_ = compile_options.is_replacer();
        enable_joiner_ = compile_options.is_joiner();
        enable_numeric_check_ = compile_options.is_numeric_check();
        enable_bracket_check_ = compile_options.is_bracket_check();
        enable_sequence_check_ = compile_options.is_sequence_check();
        enable_commutative_check_ = compile_options.is_commutative_check();
        enable_strength_reduction_ = compile_options.is_strength_reduction();
        enable_collect_vars_ = compile_options.is_collect_vars();
        enable_collect_funcs_ = compile_options.is_collect_funcs();
        enable_collect_assings_ = compile_options.is_collect_assings();
        disable_vardef_ = compile_options.is_disable_vardef();
        disable_rsrvd_sym_usr_ = compile_options.is_disable_usr_on_rsrvd();
        disable_zero_return_ = compile_options.is_disable_zero_return();
    }

    std::string assign_opr_to_string(core::operators::operator_type opr) const
    {
        switch (opr)
        {
            case core::operators::operator_type::assign:
                return ":=";
            case core::operators::operator_type::addass:
                return "+=";
            case core::operators::operator_type::subass:
                return "-=";
            case core::operators::operator_type::mulass:
                return "*=";
            case core::operators::operator_type::divass:
                return "/=";
            case core::operators::operator_type::modass:
                return "%=";
            default:
                return "";
        }
    }

    std::string arith_opr_to_string(core::operators::operator_type opr) const
    {
        switch (opr)
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
            default:
                return "";
        }
    }

    std::string inequality_opr_to_string(core::operators::operator_type opr) const
    {
        switch (opr)
        {
            case core::operators::operator_type::lt:
                return "<";
            case core::operators::operator_type::lte:
                return "<=";
            case core::operators::operator_type::eq:
                return "==";
            case core::operators::operator_type::equal:
                return "=";
            case core::operators::operator_type::ne:
                return "!=";
            case core::operators::operator_type::gte:
                return ">=";
            case core::operators::operator_type::gt:
                return ">";
            default:
                return "";
        }
    }

    std::string logic_opr_to_string(core::operators::operator_type opr) const
    {
        switch (opr)
        {
            case core::operators::operator_type::logical_and:
                return "and";
            case core::operators::operator_type::logical_or:
                return "or";
            case core::operators::operator_type::logical_xor:
                return "xor";
            case core::operators::operator_type::nand:
                return "nand";
            case core::operators::operator_type::nor:
                return "nor";
            case core::operators::operator_type::xnor:
                return "xnor";
            case core::operators::operator_type::notl:
                return "not";
            default:
                return "";
        }
    }

    bool enable_replacer_;
    bool enable_joiner_;
    bool enable_numeric_check_;
    bool enable_bracket_check_;
    bool enable_sequence_check_;
    bool enable_commutative_check_;
    bool enable_strength_reduction_;
    bool enable_collect_vars_;
    bool enable_collect_funcs_;
    bool enable_collect_assings_;
    bool disable_vardef_;
    bool disable_rsrvd_sym_usr_;
    bool disable_zero_return_;

    disabled_entity_set_t disabled_func_set_;
    disabled_entity_set_t disabled_ctrl_set_;
    disabled_entity_set_t disabled_logic_set_;
    disabled_entity_set_t disabled_arithmetic_set_;
    disabled_entity_set_t disabled_assignment_set_;
    disabled_entity_set_t disabled_inequality_set_;

    std::size_t max_stack_depth_;
    std::size_t max_node_depth_;
    std::size_t max_total_local_symbol_size_bytes_;
    std::size_t max_local_vector_size_;

    friend class parser<T>;
};

}  // namespace math_expr

#endif
