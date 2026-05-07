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

#ifndef MATH_EXPR_DETAILS_EXPRESSION_NODE_HPP
#define MATH_EXPR_DETAILS_EXPRESSION_NODE_HPP

#include "math_expr/core/numeric/functions.hpp"
#include "math_expr/core/std_includes.hpp"
#include "math_expr/fwd.hpp"
#include "math_expr/details/fwd.hpp"
#include "math_expr/details/node_memory_arena.hpp"
#include "math_expr/core/operator_types.hpp"

namespace math_expr::details
{
template <typename T>
class expression_node : public node_collector_interface<expression_node<T>>,
                        public node_depth_base<expression_node<T>>
{
   public:
    struct allocation_header
    {
        bool arena_backed;
    };

    static constexpr std::size_t k_allocation_header_size =
        ((sizeof(allocation_header) + alignof(std::max_align_t) - 1U) / alignof(std::max_align_t)) *
        alignof(std::max_align_t);

    enum class node_type
    {
        e_none,
        e_null,
        e_constant,
        e_unary,
        e_binary,
        e_binary_ext,
        e_trinary,
        e_quaternary,
        e_vararg,
        e_conditional,
        e_while,
        e_repeat,
        e_for,
        e_switch,
        e_mswitch,
        e_return,
        e_retenv,
        e_variable,
        e_stringvar,
        e_stringconst,
        e_stringvarrng,
        e_cstringvarrng,
        e_strgenrange,
        e_strconcat,
        e_stringvarsize,
        e_strswap,
        e_stringsize,
        e_stringvararg,
        e_function,
        e_vafunction,
        e_genfunction,
        e_strfunction,
        e_strcondition,
        e_strccondition,
        e_add,
        e_sub,
        e_mul,
        e_div,
        e_mod,
        e_pow,
        e_lt,
        e_lte,
        e_gt,
        e_gte,
        e_eq,
        e_ne,
        e_and,
        e_nand,
        e_or,
        e_nor,
        e_xor,
        e_xnor,
        e_in,
        e_like,
        e_ilike,
        e_inranges,
        e_ipow,
        e_ipowinv,
        e_abs,
        e_acos,
        e_acosh,
        e_asin,
        e_asinh,
        e_atan,
        e_atanh,
        e_ceil,
        e_cos,
        e_cosh,
        e_exp,
        e_expm1,
        e_floor,
        e_log,
        e_log10,
        e_log2,
        e_log1p,
        e_neg,
        e_pos,
        e_round,
        e_sin,
        e_sinc,
        e_sinh,
        e_sqrt,
        e_tan,
        e_tanh,
        e_cot,
        e_sec,
        e_csc,
        e_r2d,
        e_d2r,
        e_d2g,
        e_g2d,
        e_notl,
        e_sgn,
        e_erf,
        e_erfc,
        e_ncdf,
        e_frac,
        e_trunc,
        e_uvouv,
        e_vov,
        e_cov,
        e_voc,
        e_vob,
        e_bov,
        e_cob,
        e_boc,
        e_vovov,
        e_vovoc,
        e_vocov,
        e_covov,
        e_covoc,
        e_vovovov,
        e_vovovoc,
        e_vovocov,
        e_vocovov,
        e_covovov,
        e_covocov,
        e_vocovoc,
        e_covovoc,
        e_vococov,
        e_sf3ext,
        e_sf4ext,
        e_nulleq,
        e_strass,
        e_vector,
        e_vecsize,
        e_vecelem,
        e_veccelem,
        e_vecelemrtc,
        e_veccelemrtc,
        e_rbvecelem,
        e_rbvecelemrtc,
        e_rbveccelem,
        e_rbveccelemrtc,
        e_vecinit,
        e_vecvalass,
        e_vecvecass,
        e_vecopvalass,
        e_vecopvecass,
        e_vecfunc,
        e_vecvecswap,
        e_vecvecineq,
        e_vecvalineq,
        e_valvecineq,
        e_vecvecarith,
        e_vecvalarith,
        e_valvecarith,
        e_vecunaryop,
        e_vecondition,
        e_break,
        e_continue,
        e_swap,
        e_assert,
        e_vararg_multi
    };

    using value_type = T;
    using expression_ptr = expression_node<T>*;
    using nci_t = node_collector_interface<expression_node<T>>;
    using noderef_list_t = typename nci_t::noderef_list_t;
    using ndb_t = node_depth_base<expression_node<T>>;

    virtual ~expression_node() {}

    inline virtual T value() const
    {
        return std::numeric_limits<T>::quiet_NaN();
    }

    inline virtual expression_node<T>* branch(const std::size_t& index = 0) const
    {
        return nullptr;
    }

    inline virtual node_type type() const
    {
        return node_type::e_none;
    }

    inline virtual bool valid() const
    {
        return true;
    }

    virtual string_base_node<T>* as_string_base()
    {
        return nullptr;
    }
    virtual range_interface<T>* as_range_iface()
    {
        return nullptr;
    }
    virtual vector_interface<T>* as_vector_iface()
    {
        return nullptr;
    }
    virtual ivariable<T>* as_ivariable()
    {
        return nullptr;
    }
    virtual variable_node<T>* as_variable_node()
    {
        return nullptr;
    }
    virtual sf3_base_node<T>* as_sf3_base()
    {
        return nullptr;
    }
    virtual sf4_base_node<T>* as_sf4_base()
    {
        return nullptr;
    }
    virtual fixed_function_base_node<T>* as_fixed_function_base()
    {
        return nullptr;
    }
    virtual T0oT1_base_node<T>* as_T0oT1_base()
    {
        return nullptr;
    }
    virtual uv_base_node<T>* as_uv_base_node()
    {
        return nullptr;
    }
    virtual vov_base_node<T>* as_vov_base()
    {
        return nullptr;
    }
    virtual cov_base_node<T>* as_cov_base()
    {
        return nullptr;
    }
    virtual voc_base_node<T>* as_voc_base()
    {
        return nullptr;
    }
    virtual vob_base_node<T>* as_vob_base()
    {
        return nullptr;
    }
    virtual bov_base_node<T>* as_bov_base()
    {
        return nullptr;
    }
    virtual cob_base_node<T>* as_cob_base()
    {
        return nullptr;
    }
    virtual boc_base_node<T>* as_boc_base()
    {
        return nullptr;
    }
    virtual scalar_pow_base_node<T>* as_scalar_pow_base()
    {
        return nullptr;
    }
    virtual branch_pow_base_node<T>* as_branch_pow_base()
    {
        return nullptr;
    }
    virtual T0oT1oT2_base_node<T>* as_T0oT1oT2_base()
    {
        return nullptr;
    }
    virtual T0oT1oT2oT3_base_node<T>* as_T0oT1oT2oT3_base()
    {
        return nullptr;
    }
    virtual void release_branch() {}

    static void* operator new(std::size_t size)
    {
        if (auto* arena = active_node_memory_arena(); nullptr != arena)
        {
            if (void* storage =
                    arena->allocate(k_allocation_header_size + size, alignof(std::max_align_t));
                nullptr != storage)
            {
                auto* header = static_cast<allocation_header*>(storage);
                header->arena_backed = true;
                return static_cast<std::byte*>(storage) + k_allocation_header_size;
            }
        }

        void* storage = ::operator new(k_allocation_header_size + size);
        auto* header = static_cast<allocation_header*>(storage);
        header->arena_backed = false;
        return static_cast<std::byte*>(storage) + k_allocation_header_size;
    }

    static void operator delete(void* ptr) noexcept
    {
        if (nullptr == ptr)
        {
            return;
        }

        auto* storage = static_cast<std::byte*>(ptr) - k_allocation_header_size;
        auto* header = reinterpret_cast<allocation_header*>(storage);

        if (!header->arena_backed)
        {
            ::operator delete(storage);
        }
    }

    static void operator delete(void* ptr, std::size_t) noexcept
    {
        operator delete(ptr);
    }

    static void* operator new(std::size_t size, std::align_val_t alignment)
    {
        return ::operator new(size, alignment);
    }

    static void operator delete(void* ptr, std::align_val_t alignment) noexcept
    {
        ::operator delete(ptr, alignment);
    }

    static void operator delete(void* ptr, std::size_t, std::align_val_t alignment) noexcept
    {
        ::operator delete(ptr, alignment);
    }
};  // class expression_node

template <typename T>
inline bool is_generally_string_node(const expression_node<T>* node);

template <typename T>
inline std::enable_if_t<core::numeric::details::is_supported_numeric_type_v<T>, bool> is_true(
    const T v)
{
    return core::numeric::is_true(v);
}

template <typename T>
inline std::enable_if_t<core::numeric::details::is_supported_numeric_type_v<T>, bool> is_false(
    const T v)
{
    return core::numeric::is_false(v);
}

template <typename T>
inline bool is_true(const expression_node<T>* node)
{
    return is_true(node->value());
}

template <typename T>
inline bool is_true(const std::pair<expression_node<T>*, bool>& node)
{
    return is_true(node.first->value());
}

template <typename T>
inline bool is_false(const expression_node<T>* node)
{
    return is_false(node->value());
}

template <typename T>
inline bool is_false(const std::pair<expression_node<T>*, bool>& node)
{
    return is_false(node.first->value());
}

template <typename T>
inline bool is_literal_node(const expression_node<T>* node)
{
    return node && (details::expression_node<T>::node_type::e_constant == node->type());
}

template <typename T>
inline bool is_unary_node(const expression_node<T>* node)
{
    return node && (details::expression_node<T>::node_type::e_unary == node->type());
}

template <typename T>
inline bool is_neg_unary_node(const expression_node<T>* node)
{
    return node && (details::expression_node<T>::node_type::e_neg == node->type());
}

template <typename T>
inline bool is_binary_node(const expression_node<T>* node)
{
    return node && (details::expression_node<T>::node_type::e_binary == node->type());
}

template <typename T>
inline bool is_variable_node(const expression_node<T>* node)
{
    return node && (details::expression_node<T>::node_type::e_variable == node->type());
}

template <typename T>
inline bool is_ivariable_node(const expression_node<T>* node)
{
    return node && (details::expression_node<T>::node_type::e_variable == node->type() ||
                    details::expression_node<T>::node_type::e_vecelem == node->type() ||
                    details::expression_node<T>::node_type::e_veccelem == node->type() ||
                    details::expression_node<T>::node_type::e_vecelemrtc == node->type() ||
                    details::expression_node<T>::node_type::e_veccelemrtc == node->type() ||
                    details::expression_node<T>::node_type::e_rbvecelem == node->type() ||
                    details::expression_node<T>::node_type::e_rbveccelem == node->type() ||
                    details::expression_node<T>::node_type::e_rbvecelemrtc == node->type() ||
                    details::expression_node<T>::node_type::e_rbveccelemrtc == node->type());
}

template <typename T>
inline bool is_vector_elem_node(const expression_node<T>* node)
{
    return node && (details::expression_node<T>::node_type::e_vecelem == node->type());
}

template <typename T>
inline bool is_vector_celem_node(const expression_node<T>* node)
{
    return node && (details::expression_node<T>::node_type::e_veccelem == node->type());
}

template <typename T>
inline bool is_vector_elem_rtc_node(const expression_node<T>* node)
{
    return node && (details::expression_node<T>::node_type::e_vecelemrtc == node->type());
}

template <typename T>
inline bool is_vector_celem_rtc_node(const expression_node<T>* node)
{
    return node && (details::expression_node<T>::node_type::e_veccelemrtc == node->type());
}

template <typename T>
inline bool is_rebasevector_elem_node(const expression_node<T>* node)
{
    return node && (details::expression_node<T>::node_type::e_rbvecelem == node->type());
}

template <typename T>
inline bool is_rebasevector_elem_rtc_node(const expression_node<T>* node)
{
    return node && (details::expression_node<T>::node_type::e_rbvecelemrtc == node->type());
}

template <typename T>
inline bool is_rebasevector_celem_rtc_node(const expression_node<T>* node)
{
    return node && (details::expression_node<T>::node_type::e_rbveccelemrtc == node->type());
}

template <typename T>
inline bool is_rebasevector_celem_node(const expression_node<T>* node)
{
    return node && (details::expression_node<T>::node_type::e_rbveccelem == node->type());
}

template <typename T>
inline bool is_vector_node(const expression_node<T>* node)
{
    return node && (details::expression_node<T>::node_type::e_vector == node->type());
}

template <typename T>
inline bool is_ivector_node(const expression_node<T>* node)
{
    if (node)
    {
        switch (node->type())
        {
            case details::expression_node<T>::node_type::e_vector:
            case details::expression_node<T>::node_type::e_vecvalass:
            case details::expression_node<T>::node_type::e_vecvecass:
            case details::expression_node<T>::node_type::e_vecopvalass:
            case details::expression_node<T>::node_type::e_vecopvecass:
            case details::expression_node<T>::node_type::e_vecvecswap:
            case details::expression_node<T>::node_type::e_vecvecarith:
            case details::expression_node<T>::node_type::e_vecvalarith:
            case details::expression_node<T>::node_type::e_valvecarith:
            case details::expression_node<T>::node_type::e_vecunaryop:
            case details::expression_node<T>::node_type::e_vecondition:
                return true;
            default:
                return false;
        }
    }
    else
        return false;
}

template <typename T>
inline bool amalgamated_vecop(const expression_node<T>* node)
{
    if (node)
    {
        switch (node->type())
        {
            case details::expression_node<T>::node_type::e_vecvecarith:
            case details::expression_node<T>::node_type::e_vecvalarith:
            case details::expression_node<T>::node_type::e_valvecarith:
            case details::expression_node<T>::node_type::e_vecunaryop:
                return true;
            default:
                return false;
        }
    }
    else
        return false;
}

template <typename T>
inline bool is_constant_node(const expression_node<T>* node)
{
    return node && (details::expression_node<T>::node_type::e_constant == node->type() ||
                    details::expression_node<T>::node_type::e_stringconst == node->type());
}

template <typename T>
inline bool is_null_node(const expression_node<T>* node)
{
    return node && (details::expression_node<T>::node_type::e_null == node->type());
}

template <typename T>
inline bool is_break_node(const expression_node<T>* node)
{
    return node && (details::expression_node<T>::node_type::e_break == node->type());
}

template <typename T>
inline bool is_continue_node(const expression_node<T>* node)
{
    return node && (details::expression_node<T>::node_type::e_continue == node->type());
}

template <typename T>
inline bool is_swap_node(const expression_node<T>* node)
{
    return node && (details::expression_node<T>::node_type::e_swap == node->type());
}

template <typename T>
inline bool is_function(const expression_node<T>* node)
{
    return node && (details::expression_node<T>::node_type::e_function == node->type());
}

template <typename T>
inline bool is_vararg_node(const expression_node<T>* node)
{
    return node && (details::expression_node<T>::node_type::e_vararg == node->type() ||
                    details::expression_node<T>::node_type::e_vararg_multi == node->type());
}

template <typename T>
inline bool is_return_node(const expression_node<T>* node)
{
    return node && (details::expression_node<T>::node_type::e_return == node->type());
}

template <typename T>
class unary_node;

template <typename T>
inline bool is_negate_node(const expression_node<T>* node)
{
    if (node && is_unary_node(node))
    {
        return (core::operators::operator_type::neg ==
                static_cast<const unary_node<T>*>(node)->operation());
    }
    else
        return false;
}

template <typename T>
inline bool is_assert_node(const expression_node<T>* node)
{
    return node && (details::expression_node<T>::node_type::e_assert == node->type());
}

template <typename T>
inline bool branch_deletable(const expression_node<T>* node)
{
    return (nullptr != node) && !is_variable_node(node) && !is_string_node(node);
}

template <std::size_t N, typename T>
inline bool all_nodes_valid(expression_node<T>* const (&b)[N])
{
    for (std::size_t i = 0; i < N; ++i)
    {
        if (nullptr == b[i])
            return false;
    }

    return true;
}

template <typename T, typename Allocator, template <typename, typename> class Sequence>
inline bool all_nodes_valid(const Sequence<expression_node<T>*, Allocator>& b)
{
    for (std::size_t i = 0; i < b.size(); ++i)
    {
        if (nullptr == b[i])
            return false;
    }

    return true;
}

template <std::size_t N, typename T>
inline bool all_nodes_variables(expression_node<T>* const (&b)[N])
{
    for (std::size_t i = 0; i < N; ++i)
    {
        if (nullptr == b[i])
            return false;
        else if (!is_variable_node(b[i]))
            return false;
    }

    return true;
}

template <typename T, typename Allocator, template <typename, typename> class Sequence>
inline bool all_nodes_variables(const Sequence<expression_node<T>*, Allocator>& b)
{
    for (std::size_t i = 0; i < b.size(); ++i)
    {
        if (nullptr == b[i])
            return false;
        else if (!is_variable_node(b[i]))
            return false;
    }

    return true;
}

template <typename Node>
class node_collection_destructor
{
   public:
    using nci_t = node_collector_interface<Node>;

    using node_ptr_t = typename nci_t::node_ptr_t;
    using node_pp_t = typename nci_t::node_pp_t;
    using noderef_list_t = typename nci_t::noderef_list_t;

    static void delete_nodes(node_ptr_t& root)
    {
        std::vector<node_pp_t> node_delete_list;
        node_delete_list.reserve(1000);

        collect_nodes(root, node_delete_list);

        for (node_ptr_t* node_pp : node_delete_list)
        {
            core::debug_print("ncd::delete_nodes() - deleting: %p\n",
                              static_cast<const void*>(*node_pp));
            std::unique_ptr<Node>(std::exchange(*node_pp, nullptr));
        }
    }

   private:
    static void collect_nodes(node_ptr_t& root, noderef_list_t& node_delete_list)
    {
        std::deque<node_ptr_t> node_list;
        node_list.push_back(root);
        node_delete_list.push_back(&root);

        noderef_list_t child_node_delete_list;
        child_node_delete_list.reserve(1000);

        while (!node_list.empty())
        {
            node_list.front()->collect_nodes(child_node_delete_list);

            if (!child_node_delete_list.empty())
            {
                for (std::size_t i = 0; i < child_node_delete_list.size(); ++i)
                {
                    node_pp_t& node = child_node_delete_list[i];

                    if (nullptr == (*node))
                    {
                        core::debug_print("ncd::collect_nodes() - null node encountered.\n");
                    }

                    node_list.push_back(*node);
                }

                node_delete_list.insert(node_delete_list.end(), child_node_delete_list.begin(),
                                        child_node_delete_list.end());

                child_node_delete_list.clear();
            }

            node_list.pop_front();
        }

        std::reverse(node_delete_list.begin(), node_delete_list.end());
    }
};

template <typename NodeAllocator, typename T, std::size_t N>
inline void free_all_nodes(NodeAllocator& node_allocator, expression_node<T>* (&b)[N])
{
    for (std::size_t i = 0; i < N; ++i)
    {
        free_node(node_allocator, b[i]);
    }
}

template <typename NodeAllocator, typename T, typename Allocator,
          template <typename, typename> class Sequence>
inline void free_all_nodes(NodeAllocator& node_allocator,
                           Sequence<expression_node<T>*, Allocator>& b)
{
    for (std::size_t i = 0; i < b.size(); ++i)
    {
        free_node(node_allocator, b[i]);
    }

    b.clear();
}

template <typename NodeAllocator, typename T>
inline void free_node(NodeAllocator&, expression_node<T>*& node)
{
    if ((nullptr == node) || is_variable_node(node) || is_string_node(node))
    {
        return;
    }

    node_collection_destructor<expression_node<T>>::delete_nodes(node);
}

template <typename T>
inline void destroy_node(expression_node<T>*& node)
{
    if (nullptr != node)
    {
        node_collection_destructor<expression_node<T>>::delete_nodes(node);
    }
}

template <typename Node>
struct node_depth_base
{
    using node_ptr_t = Node*;
    using nb_pair_t = std::pair<node_ptr_t, bool>;

    node_depth_base() : depth_set(false), depth(0) {}

    virtual ~node_depth_base() {}

    virtual std::size_t node_depth() const
    {
        return 1;
    }

    std::size_t compute_node_depth(const Node* const& node) const
    {
        if (!depth_set)
        {
            depth = 1 + (node ? node->node_depth() : 0);
            depth_set = true;
        }

        return depth;
    }

    std::size_t compute_node_depth(const nb_pair_t& branch) const
    {
        if (!depth_set)
        {
            depth = 1 + (branch.first ? branch.first->node_depth() : 0);
            depth_set = true;
        }

        return depth;
    }

    template <std::size_t N>
    std::size_t compute_node_depth(const nb_pair_t (&branch)[N]) const
    {
        if (!depth_set)
        {
            depth = 0;

            for (std::size_t i = 0; i < N; ++i)
            {
                if (branch[i].first)
                {
                    depth = std::max(depth.load(), branch[i].first->node_depth());
                }
            }

            depth += 1;
            depth_set = true;
        }

        return depth;
    }

    template <typename BranchType>
    std::size_t max_node_depth(const BranchType& n0, const BranchType& n1) const
    {
        return std::max(compute_node_depth(n0), compute_node_depth(n1));
    }

    template <typename BranchType>
    std::size_t max_node_depth(const BranchType& n0, const BranchType& n1,
                               const BranchType& n2) const
    {
        return std::max(compute_node_depth(n0),
                        std::max(compute_node_depth(n1), compute_node_depth(n2)));
    }

    template <typename BranchType>
    std::size_t max_node_depth(const BranchType& n0, const BranchType& n1, const BranchType& n2,
                               const BranchType& n3) const
    {
        return std::max(std::max(compute_node_depth(n0), compute_node_depth(n1)),
                        std::max(compute_node_depth(n2), compute_node_depth(n3)));
    }

    template <typename BranchType>
    std::size_t compute_node_depth(const BranchType& n0, const BranchType& n1) const
    {
        if (!depth_set)
        {
            depth = 1 + max_node_depth(n0, n1);
            depth_set = true;
        }

        return depth;
    }

    template <typename BranchType>
    std::size_t compute_node_depth(const BranchType& n0, const BranchType& n1,
                                   const BranchType& n2) const
    {
        if (!depth_set)
        {
            depth = 1 + max_node_depth(n0, n1, n2);
            depth_set = true;
        }

        return depth;
    }

    template <typename BranchType>
    std::size_t compute_node_depth(const BranchType& n0, const BranchType& n1, const BranchType& n2,
                                   const BranchType& n3) const
    {
        if (!depth_set)
        {
            depth = 1 + max_node_depth(n0, n1, n2, n3);
            depth_set = true;
        }

        return depth;
    }

    template <typename Allocator, template <typename, typename> class Sequence>
    std::size_t compute_node_depth(const Sequence<node_ptr_t, Allocator>& branch_list) const
    {
        if (!depth_set)
        {
            for (std::size_t i = 0; i < branch_list.size(); ++i)
            {
                if (branch_list[i])
                {
                    depth = std::max(depth.load(), compute_node_depth(branch_list[i]));
                }
            }

            depth_set = true;
        }

        return depth;
    }

    template <typename Allocator, template <typename, typename> class Sequence>
    std::size_t compute_node_depth(const Sequence<nb_pair_t, Allocator>& branch_list) const
    {
        if (!depth_set)
        {
            for (std::size_t i = 0; i < branch_list.size(); ++i)
            {
                if (branch_list[i].first)
                {
                    depth = std::max(depth.load(), compute_node_depth(branch_list[i].first));
                }
            }

            depth_set = true;
        }

        return depth;
    }

    mutable std::atomic<bool> depth_set;
    mutable std::atomic<std::size_t> depth;

    template <typename NodeSequence>
    void collect(node_ptr_t const& node, const bool deletable, NodeSequence& delete_node_list) const
    {
        if ((nullptr != node) && deletable)
        {
            delete_node_list.push_back(const_cast<node_ptr_t*>(&node));
        }
    }

    template <typename NodeSequence>
    void collect(const nb_pair_t& branch, NodeSequence& delete_node_list) const
    {
        collect(branch.first, branch.second, delete_node_list);
    }

    template <typename NodeSequence>
    void collect(Node*& node, NodeSequence& delete_node_list) const
    {
        collect(node, branch_deletable(node), delete_node_list);
    }

    template <std::size_t N, typename NodeSequence>
    void collect(const nb_pair_t (&branch)[N], NodeSequence& delete_node_list) const
    {
        for (std::size_t i = 0; i < N; ++i)
        {
            collect(branch[i].first, branch[i].second, delete_node_list);
        }
    }

    template <typename Allocator, template <typename, typename> class Sequence,
              typename NodeSequence>
    void collect(const Sequence<nb_pair_t, Allocator>& branch, NodeSequence& delete_node_list) const
    {
        for (std::size_t i = 0; i < branch.size(); ++i)
        {
            collect(branch[i].first, branch[i].second, delete_node_list);
        }
    }

    template <typename Allocator, template <typename, typename> class Sequence,
              typename NodeSequence>
    void collect(const Sequence<node_ptr_t, Allocator>& branch_list,
                 NodeSequence& delete_node_list) const
    {
        for (std::size_t i = 0; i < branch_list.size(); ++i)
        {
            collect(branch_list[i], branch_deletable(branch_list[i]), delete_node_list);
        }
    }

    template <typename Boolean, typename AllocatorT, typename AllocatorB,
              template <typename, typename> class Sequence, typename NodeSequence>
    void collect(const Sequence<node_ptr_t, AllocatorT>& branch_list,
                 const Sequence<Boolean, AllocatorB>& branch_deletable_list,
                 NodeSequence& delete_node_list) const
    {
        for (std::size_t i = 0; i < branch_list.size(); ++i)
        {
            collect(branch_list[i], branch_deletable_list[i], delete_node_list);
        }
    }
};

template <typename Type>
class vector_holder
{
   private:
    using value_type = Type;
    using value_ptr = value_type*;
    using const_value_ptr = const value_ptr;
    using vector_holder_t = vector_holder<Type>;

    class vector_holder_base
    {
       public:
        virtual ~vector_holder_base() {}

        inline value_ptr operator[](const std::size_t& index) const
        {
            return value_at(index);
        }

        inline std::size_t size() const
        {
            return vector_size();
        }

        inline std::size_t base_size() const
        {
            return vector_base_size();
        }

        inline value_ptr data() const
        {
            return value_at(0);
        }

        virtual inline bool rebaseable() const
        {
            return false;
        }

        virtual void set_ref(value_ptr*) {}

        virtual void remove_ref(value_ptr*) {}

        virtual void set_size_ref(std::size_t*) {}

        virtual void remove_size_ref(std::size_t*) {}

        virtual vector_view<Type>* rebaseable_instance()
        {
            return nullptr;
        }

       protected:
        virtual value_ptr value_at(const std::size_t&) const = 0;
        virtual std::size_t vector_size() const = 0;
        virtual std::size_t vector_base_size() const = 0;
    };

    class array_vector_impl final : public vector_holder_base
    {
       public:
        array_vector_impl(const Type* vec, const std::size_t& vec_size) : vec_(vec), size_(vec_size)
        {
        }

       protected:
        value_ptr value_at(const std::size_t& index) const override
        {
            assert(index < size_);
            return const_cast<const_value_ptr>(vec_ + index);
        }

        std::size_t vector_size() const override
        {
            return size_;
        }

        std::size_t vector_base_size() const override
        {
            return vector_size();
        }

       private:
        array_vector_impl(const array_vector_impl&) = delete;
        array_vector_impl& operator=(const array_vector_impl&) = delete;

        const Type* vec_;
        const std::size_t size_;
    };

    template <typename Allocator, template <typename, typename> class Sequence>
    class sequence_vector_impl final : public vector_holder_base
    {
       public:
        using sequence_t = Sequence<Type, Allocator>;

        explicit sequence_vector_impl(sequence_t& seq) : sequence_(seq) {}

       protected:
        value_ptr value_at(const std::size_t& index) const override
        {
            assert(index < sequence_.size());
            return (&sequence_[index]);
        }

        std::size_t vector_size() const override
        {
            return sequence_.size();
        }

        std::size_t vector_base_size() const override
        {
            return vector_size();
        }

       private:
        sequence_vector_impl(const sequence_vector_impl&) = delete;
        sequence_vector_impl& operator=(const sequence_vector_impl&) = delete;

        sequence_t& sequence_;
    };

    class vector_view_impl final : public vector_holder_base
    {
       public:
        using vector_view_t = math_expr::vector_view<Type>;

        explicit vector_view_impl(vector_view_t& vec_view) : vec_view_(vec_view)
        {
            assert(vec_view_.size() > 0);
        }

        void set_ref(value_ptr* ref) override
        {
            vec_view_.set_ref(ref);
        }

        void remove_ref(value_ptr* ref) override
        {
            vec_view_.remove_ref(ref);
        }

        bool rebaseable() const override
        {
            return true;
        }

        vector_view<Type>* rebaseable_instance() override
        {
            return &vec_view_;
        }

       protected:
        value_ptr value_at(const std::size_t& index) const override
        {
            assert(index < vec_view_.size());
            return (&vec_view_[index]);
        }

        std::size_t vector_size() const override
        {
            return vec_view_.size();
        }

        std::size_t vector_base_size() const override
        {
            return vec_view_.base_size();
        }

       private:
        vector_view_impl(const vector_view_impl&) = delete;
        vector_view_impl& operator=(const vector_view_impl&) = delete;

        vector_view_t& vec_view_;
    };

    class resizable_vector_impl final : public vector_holder_base
    {
       public:
        resizable_vector_impl(vector_holder& vec_view_holder, const Type* vec,
                              const std::size_t& vec_size)
            : vec_(vec), size_(vec_size), vec_view_holder_(*vec_view_holder.rebaseable_instance())
        {
            assert(vec_view_holder.rebaseable_instance());
            assert(size_ <= vector_base_size());
        }

        virtual ~resizable_vector_impl() override {}

       protected:
        value_ptr value_at(const std::size_t& index) const override
        {
            assert(index < vector_size());
            return const_cast<const_value_ptr>(vec_ + index);
        }

        std::size_t vector_size() const override
        {
            return vec_view_holder_.size();
        }

        std::size_t vector_base_size() const override
        {
            return vec_view_holder_.base_size();
        }

        bool rebaseable() const override
        {
            return true;
        }

        virtual vector_view<Type>* rebaseable_instance() override
        {
            return &vec_view_holder_;
        }

       private:
        resizable_vector_impl(const resizable_vector_impl&) = delete;
        resizable_vector_impl& operator=(const resizable_vector_impl&) = delete;

        const Type* vec_;
        const std::size_t size_;
        vector_view<Type>& vec_view_holder_;
    };

   public:
    using vds_t = typename core::vec_data_store<Type>;

    vector_holder(Type* vec, const std::size_t& vec_size)
        : vector_holder_base_(new (buffer) array_vector_impl(vec, vec_size))
    {
    }

    explicit vector_holder(const vds_t& vds)
        : vector_holder_base_(new (buffer) array_vector_impl(vds.data(), vds.size()))
    {
    }

    template <typename Allocator>
    explicit vector_holder(std::vector<Type, Allocator>& vec)
        : vector_holder_base_(new (buffer) sequence_vector_impl<Allocator, std::vector>(vec))
    {
    }

    explicit vector_holder(math_expr::vector_view<Type>& vec)
        : vector_holder_base_(new (buffer) vector_view_impl(vec))
    {
    }

    explicit vector_holder(vector_holder_t& vec_holder, const vds_t& vds)
        : vector_holder_base_(new (buffer)
                                  resizable_vector_impl(vec_holder, vds.data(), vds.size()))
    {
    }

    inline value_ptr operator[](const std::size_t& index) const
    {
        return (*vector_holder_base_)[index];
    }

    inline std::size_t size() const
    {
        return vector_holder_base_->size();
    }

    inline std::size_t base_size() const
    {
        return vector_holder_base_->base_size();
    }

    inline value_ptr data() const
    {
        return vector_holder_base_->data();
    }

    void set_ref(value_ptr* ref)
    {
        if (rebaseable())
        {
            vector_holder_base_->set_ref(ref);
        }
    }

    void set_size_ref(std::size_t* ref)
    {
        if (rebaseable())
        {
            vector_holder_base_->set_size_ref(ref);
        }
    }

    void remove_ref(value_ptr* ref)
    {
        if (rebaseable())
        {
            vector_holder_base_->remove_ref(ref);
        }
    }

    void remove_size_ref(std::size_t* ref)
    {
        if (rebaseable())
        {
            vector_holder_base_->remove_size_ref(ref);
        }
    }

    bool rebaseable() const
    {
        return vector_holder_base_->rebaseable();
    }

    vector_view<Type>* rebaseable_instance()
    {
        return vector_holder_base_->rebaseable_instance();
    }

   private:
    vector_holder(const vector_holder<Type>&) = delete;
    vector_holder<Type>& operator=(const vector_holder<Type>&) = delete;

    mutable vector_holder_base* vector_holder_base_;
    core::uchar_t buffer[64];
};

}  // namespace math_expr::details

#endif
