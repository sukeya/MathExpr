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

#ifndef MATH_EXPR_DETAILS_LOOP_NODES_HPP
#define MATH_EXPR_DETAILS_LOOP_NODES_HPP

#include "math_expr/loop_runtime_check.hpp"
#include "math_expr/details/literal_nodes.hpp"

namespace math_expr::details
{
#ifndef MATH_EXPR_DISABLE_BREAK_CONTINUE
template <typename T>
class break_exception
{
   public:
    explicit break_exception(const T& v) : value(v) {}

    T value;
};

class continue_exception
{
};

template <typename T>
class break_node final : public expression_node<T>
{
   public:
    using expression_ptr = expression_node<T>*;
    using branch_t = std::pair<expression_ptr, bool>;

    explicit break_node(expression_ptr ret = expression_ptr(0))
    {
        construct_branch_pair(return_, ret);
    }

    inline T value() const override
    {
        const T result =
            return_.first ? return_.first->value() : std::numeric_limits<T>::quiet_NaN();

        throw break_exception<T>(result);

#ifdef __NVCOMPILER
        return std::numeric_limits<T>::quiet_NaN();
#endif
    }

    inline typename expression_node<T>::node_type type() const override
    {
        return expression_node<T>::node_type::e_break;
    }

    void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) override
    {
        expression_node<T>::ndb_t::collect(return_, node_delete_list);
    }

    std::size_t node_depth() const override
    {
        return expression_node<T>::ndb_t::compute_node_depth(return_);
    }

   private:
    branch_t return_;
};

template <typename T>
class continue_node final : public expression_node<T>
{
   public:
    inline T value() const override
    {
        throw continue_exception();
#ifdef __NVCOMPILER
        return std::numeric_limits<T>::quiet_NaN();
#endif
    }

    inline typename expression_node<T>::node_type type() const override
    {
        return expression_node<T>::node_type::e_break;
    }
};
#endif

struct loop_runtime_checker
{
    loop_runtime_checker(loop_runtime_check_ptr loop_runtime_check,
                         loop_types lp_typ = loop_types::e_invalid())
        : iteration_count_(0),
          loop_runtime_check_(loop_runtime_check),
          max_loop_iterations_(loop_runtime_check_->max_loop_iterations),
          loop_type_(lp_typ)
    {
        assert(loop_runtime_check_);
    }

    inline void reset(const std::uint64_t initial_value = 0) const
    {
        iteration_count_ = initial_value;
    }

    inline bool check() const
    {
        assert(loop_runtime_check_);

        if ((++iteration_count_ <= max_loop_iterations_) && loop_runtime_check_->check())
        {
            return true;
        }

        loop_runtime_check::violation_context ctxt;
        ctxt.loop = loop_type_;
        ctxt.violation = loop_runtime_check::violation_type::e_iteration_count;

        loop_runtime_check_->handle_runtime_violation(ctxt);

        return false;
    }

    bool valid() const
    {
        return nullptr != loop_runtime_check_;
    }

    mutable std::uint64_t iteration_count_;
    mutable loop_runtime_check_ptr loop_runtime_check_;
    const std::uint64_t& max_loop_iterations_;
    loop_types loop_type_;
};

template <typename T>
class while_loop_node : public expression_node<T>
{
   public:
    using expression_ptr = expression_node<T>*;
    using branch_t = std::pair<expression_ptr, bool>;

    while_loop_node(expression_ptr condition, expression_ptr loop_body)
    {
        construct_branch_pair(condition_, condition);
        construct_branch_pair(loop_body_, loop_body);
        assert(valid());
    }

    inline T value() const override
    {
        T result = T(0);

        while (is_true(condition_))
        {
            result = loop_body_.first->value();
        }

        return result;
    }

    inline typename expression_node<T>::node_type type() const override
    {
        return expression_node<T>::node_type::e_while;
    }

    inline bool valid() const override
    {
        return condition_.first && condition_.first->valid() && loop_body_.first &&
               loop_body_.first->valid();
    }

    void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) override
    {
        expression_node<T>::ndb_t::collect(condition_, node_delete_list);
        expression_node<T>::ndb_t::collect(loop_body_, node_delete_list);
    }

    std::size_t node_depth() const override
    {
        return expression_node<T>::ndb_t::compute_node_depth(condition_, loop_body_);
    }

   protected:
    branch_t condition_;
    branch_t loop_body_;
};

template <typename T>
class while_loop_rtc_node final : public while_loop_node<T>, public loop_runtime_checker
{
   public:
    using parent_t = while_loop_node<T>;
    using expression_ptr = expression_node<T>*;

    while_loop_rtc_node(expression_ptr condition, expression_ptr loop_body,
                        loop_runtime_check_ptr loop_rt_chk)
        : parent_t(condition, loop_body),
          loop_runtime_checker(loop_rt_chk, loop_types::e_while_loop())
    {
        assert(valid());
    }

    inline T value() const override
    {
        T result = T(0);

        loop_runtime_checker::reset();

        while (is_true(parent_t::condition_) && loop_runtime_checker::check())
        {
            result = parent_t::loop_body_.first->value();
        }

        return result;
    }

    using parent_t::valid;

    bool valid() const override final
    {
        return parent_t::valid() && loop_runtime_checker::valid();
    }
};

template <typename T>
class repeat_until_loop_node : public expression_node<T>
{
   public:
    using expression_ptr = expression_node<T>*;
    using branch_t = std::pair<expression_ptr, bool>;

    repeat_until_loop_node(expression_ptr condition, expression_ptr loop_body)
    {
        construct_branch_pair(condition_, condition);
        construct_branch_pair(loop_body_, loop_body);
        assert(valid());
    }

    inline T value() const override
    {
        T result = T(0);

        do
        {
            result = loop_body_.first->value();
        } while (is_false(condition_.first));

        return result;
    }

    inline typename expression_node<T>::node_type type() const override
    {
        return expression_node<T>::node_type::e_repeat;
    }

    inline bool valid() const override
    {
        return condition_.first && condition_.first->valid() && loop_body_.first &&
               loop_body_.first->valid();
    }

    void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) override
    {
        expression_node<T>::ndb_t::collect(condition_, node_delete_list);
        expression_node<T>::ndb_t::collect(loop_body_, node_delete_list);
    }

    std::size_t node_depth() const override
    {
        return expression_node<T>::ndb_t::compute_node_depth(condition_, loop_body_);
    }

   protected:
    branch_t condition_;
    branch_t loop_body_;
};

template <typename T>
class repeat_until_loop_rtc_node final : public repeat_until_loop_node<T>,
                                         public loop_runtime_checker
{
   public:
    using parent_t = repeat_until_loop_node<T>;
    using expression_ptr = expression_node<T>*;

    repeat_until_loop_rtc_node(expression_ptr condition, expression_ptr loop_body,
                               loop_runtime_check_ptr loop_rt_chk)
        : parent_t(condition, loop_body),
          loop_runtime_checker(loop_rt_chk, loop_types::e_repeat_until_loop())
    {
        assert(valid());
    }

    inline T value() const override
    {
        T result = T(0);

        loop_runtime_checker::reset(1);

        do
        {
            result = parent_t::loop_body_.first->value();
        } while (is_false(parent_t::condition_.first) && loop_runtime_checker::check());

        return result;
    }

    using parent_t::valid;

    inline bool valid() const override final
    {
        return parent_t::valid() && loop_runtime_checker::valid();
    }
};

template <typename T>
class for_loop_node : public expression_node<T>
{
   public:
    using expression_ptr = expression_node<T>*;
    using branch_t = std::pair<expression_ptr, bool>;

    for_loop_node(expression_ptr initialiser, expression_ptr condition, expression_ptr incrementor,
                  expression_ptr loop_body)
    {
        construct_branch_pair(initialiser_, initialiser);
        construct_branch_pair(condition_, condition);
        construct_branch_pair(incrementor_, incrementor);
        construct_branch_pair(loop_body_, loop_body);
        assert(valid());
    }

    inline T value() const override
    {
        T result = T(0);

        if (initialiser_.first)
            initialiser_.first->value();

        if (incrementor_.first)
        {
            while (is_true(condition_))
            {
                result = loop_body_.first->value();
                incrementor_.first->value();
            }
        }
        else
        {
            while (is_true(condition_))
            {
                result = loop_body_.first->value();
            }
        }

        return result;
    }

    inline typename expression_node<T>::node_type type() const override
    {
        return expression_node<T>::node_type::e_for;
    }

    inline bool valid() const override
    {
        return condition_.first && loop_body_.first;
    }

    void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) override
    {
        expression_node<T>::ndb_t::collect(initialiser_, node_delete_list);
        expression_node<T>::ndb_t::collect(condition_, node_delete_list);
        expression_node<T>::ndb_t::collect(incrementor_, node_delete_list);
        expression_node<T>::ndb_t::collect(loop_body_, node_delete_list);
    }

    std::size_t node_depth() const override
    {
        return expression_node<T>::ndb_t::compute_node_depth(initialiser_, condition_, incrementor_,
                                                             loop_body_);
    }

   protected:
    branch_t initialiser_;
    branch_t condition_;
    branch_t incrementor_;
    branch_t loop_body_;
};

template <typename T>
class for_loop_rtc_node final : public for_loop_node<T>, public loop_runtime_checker
{
   public:
    using parent_t = for_loop_node<T>;
    using expression_ptr = expression_node<T>*;

    for_loop_rtc_node(expression_ptr initialiser, expression_ptr condition,
                      expression_ptr incrementor, expression_ptr loop_body,
                      loop_runtime_check_ptr loop_rt_chk)
        : parent_t(initialiser, condition, incrementor, loop_body),
          loop_runtime_checker(loop_rt_chk, loop_types::e_for_loop())
    {
        assert(valid());
    }

    inline T value() const override
    {
        T result = T(0);

        loop_runtime_checker::reset();

        if (parent_t::initialiser_.first)
            parent_t::initialiser_.first->value();

        if (parent_t::incrementor_.first)
        {
            while (is_true(parent_t::condition_) && loop_runtime_checker::check())
            {
                result = parent_t::loop_body_.first->value();
                parent_t::incrementor_.first->value();
            }
        }
        else
        {
            while (is_true(parent_t::condition_) && loop_runtime_checker::check())
            {
                result = parent_t::loop_body_.first->value();
            }
        }

        return result;
    }

    using parent_t::valid;

    inline bool valid() const override final
    {
        return parent_t::valid() && loop_runtime_checker::valid();
    }
};

#ifndef MATH_EXPR_DISABLE_BREAK_CONTINUE
template <typename T>
class while_loop_bc_node : public while_loop_node<T>
{
   public:
    using parent_t = while_loop_node<T>;
    using expression_ptr = expression_node<T>*;

    while_loop_bc_node(expression_ptr condition, expression_ptr loop_body)
        : parent_t(condition, loop_body)
    {
        assert(parent_t::valid());
    }

    inline T value() const override
    {
        T result = T(0);

        while (is_true(parent_t::condition_))
        {
            try
            {
                result = parent_t::loop_body_.first->value();
            }
            catch (const break_exception<T>& e)
            {
                return e.value;
            }
            catch (const continue_exception&)
            {
            }
        }

        return result;
    }
};

template <typename T>
class while_loop_bc_rtc_node final : public while_loop_bc_node<T>, public loop_runtime_checker
{
   public:
    using parent_t = while_loop_bc_node<T>;
    using expression_ptr = expression_node<T>*;

    while_loop_bc_rtc_node(expression_ptr condition, expression_ptr loop_body,
                           loop_runtime_check_ptr loop_rt_chk)
        : parent_t(condition, loop_body),
          loop_runtime_checker(loop_rt_chk, loop_types::e_while_loop())
    {
        assert(valid());
    }

    inline T value() const override
    {
        T result = T(0);

        loop_runtime_checker::reset();

        while (is_true(parent_t::condition_) && loop_runtime_checker::check())
        {
            try
            {
                result = parent_t::loop_body_.first->value();
            }
            catch (const break_exception<T>& e)
            {
                return e.value;
            }
            catch (const continue_exception&)
            {
            }
        }

        return result;
    }

    using parent_t::valid;

    inline bool valid() const override final
    {
        return parent_t::valid() && loop_runtime_checker::valid();
    }
};

template <typename T>
class repeat_until_loop_bc_node : public repeat_until_loop_node<T>
{
   public:
    using parent_t = repeat_until_loop_node<T>;
    using expression_ptr = expression_node<T>*;

    repeat_until_loop_bc_node(expression_ptr condition, expression_ptr loop_body)
        : parent_t(condition, loop_body)
    {
        assert(parent_t::valid());
    }

    inline T value() const override
    {
        T result = T(0);

        do
        {
            try
            {
                result = parent_t::loop_body_.first->value();
            }
            catch (const break_exception<T>& e)
            {
                return e.value;
            }
            catch (const continue_exception&)
            {
            }
        } while (is_false(parent_t::condition_.first));

        return result;
    }
};

template <typename T>
class repeat_until_loop_bc_rtc_node final : public repeat_until_loop_bc_node<T>,
                                            public loop_runtime_checker
{
   public:
    using parent_t = repeat_until_loop_bc_node<T>;
    using expression_ptr = expression_node<T>*;

    repeat_until_loop_bc_rtc_node(expression_ptr condition, expression_ptr loop_body,
                                  loop_runtime_check_ptr loop_rt_chk)
        : parent_t(condition, loop_body),
          loop_runtime_checker(loop_rt_chk, loop_types::e_repeat_until_loop())
    {
        assert(valid());
    }

    inline T value() const override
    {
        T result = T(0);

        loop_runtime_checker::reset();

        do
        {
            try
            {
                result = parent_t::loop_body_.first->value();
            }
            catch (const break_exception<T>& e)
            {
                return e.value;
            }
            catch (const continue_exception&)
            {
            }
        } while (is_false(parent_t::condition_.first) && loop_runtime_checker::check());

        return result;
    }

    using parent_t::valid;

    inline bool valid() const override final
    {
        return parent_t::valid() && loop_runtime_checker::valid();
    }
};

template <typename T>
class for_loop_bc_node : public for_loop_node<T>
{
   public:
    using parent_t = for_loop_node<T>;
    using expression_ptr = expression_node<T>*;

    for_loop_bc_node(expression_ptr initialiser, expression_ptr condition,
                     expression_ptr incrementor, expression_ptr loop_body)
        : parent_t(initialiser, condition, incrementor, loop_body)
    {
        assert(parent_t::valid());
    }

    inline T value() const override
    {
        T result = T(0);

        if (parent_t::initialiser_.first)
            parent_t::initialiser_.first->value();

        if (parent_t::incrementor_.first)
        {
            while (is_true(parent_t::condition_))
            {
                try
                {
                    result = parent_t::loop_body_.first->value();
                }
                catch (const break_exception<T>& e)
                {
                    return e.value;
                }
                catch (const continue_exception&)
                {
                }

                parent_t::incrementor_.first->value();
            }
        }
        else
        {
            while (is_true(parent_t::condition_))
            {
                try
                {
                    result = parent_t::loop_body_.first->value();
                }
                catch (const break_exception<T>& e)
                {
                    return e.value;
                }
                catch (const continue_exception&)
                {
                }
            }
        }

        return result;
    }
};

template <typename T>
class for_loop_bc_rtc_node final : public for_loop_bc_node<T>, public loop_runtime_checker
{
   public:
    using parent_t = for_loop_bc_node<T>;
    using expression_ptr = expression_node<T>*;

    for_loop_bc_rtc_node(expression_ptr initialiser, expression_ptr condition,
                         expression_ptr incrementor, expression_ptr loop_body,
                         loop_runtime_check_ptr loop_rt_chk)
        : parent_t(initialiser, condition, incrementor, loop_body),
          loop_runtime_checker(loop_rt_chk, loop_types::e_for_loop())
    {
        assert(valid());
    }

    inline T value() const override
    {
        T result = T(0);

        loop_runtime_checker::reset();

        if (parent_t::initialiser_.first)
            parent_t::initialiser_.first->value();

        if (parent_t::incrementor_.first)
        {
            while (is_true(parent_t::condition_) && loop_runtime_checker::check())
            {
                try
                {
                    result = parent_t::loop_body_.first->value();
                }
                catch (const break_exception<T>& e)
                {
                    return e.value;
                }
                catch (const continue_exception&)
                {
                }

                parent_t::incrementor_.first->value();
            }
        }
        else
        {
            while (is_true(parent_t::condition_) && loop_runtime_checker::check())
            {
                try
                {
                    result = parent_t::loop_body_.first->value();
                }
                catch (const break_exception<T>& e)
                {
                    return e.value;
                }
                catch (const continue_exception&)
                {
                }
            }
        }

        return result;
    }

    using parent_t::valid;

    inline bool valid() const override final
    {
        return parent_t::valid() && loop_runtime_checker::valid();
    }
};
#endif

template <typename T>
class switch_node : public expression_node<T>
{
   public:
    using expression_ptr = expression_node<T>*;
    using branch_t = std::pair<expression_ptr, bool>;

    template <typename Allocator, template <typename, typename> class Sequence>
    explicit switch_node(const Sequence<expression_ptr, Allocator>& arg_list)
    {
        if (1 != (arg_list.size() & 1))
            return;

        arg_list_.resize(arg_list.size());

        for (std::size_t i = 0; i < arg_list.size(); ++i)
        {
            if (arg_list[i] && arg_list[i]->valid())
            {
                construct_branch_pair(arg_list_[i], arg_list[i]);
            }
            else
            {
                arg_list_.clear();
                return;
            }
        }

        assert(valid());
    }

    inline T value() const override
    {
        const std::size_t upper_bound = (arg_list_.size() - 1);

        for (std::size_t i = 0; i < upper_bound; i += 2)
        {
            expression_ptr condition = arg_list_[i].first;
            expression_ptr consequent = arg_list_[i + 1].first;

            if (is_true(condition))
            {
                return consequent->value();
            }
        }

        return arg_list_[upper_bound].first->value();
    }

    inline typename expression_node<T>::node_type type() const override final
    {
        return expression_node<T>::node_type::e_switch;
    }

    inline bool valid() const override
    {
        return !arg_list_.empty();
    }

    void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) override
    {
        expression_node<T>::ndb_t::collect(arg_list_, node_delete_list);
    }

    std::size_t node_depth() const override final
    {
        return expression_node<T>::ndb_t::compute_node_depth(arg_list_);
    }

   protected:
    std::vector<branch_t> arg_list_;
};

template <typename T, typename Switch_N>
class switch_n_node final : public switch_node<T>
{
   public:
    using expression_ptr = expression_node<T>*;

    template <typename Allocator, template <typename, typename> class Sequence>
    explicit switch_n_node(const Sequence<expression_ptr, Allocator>& arg_list)
        : switch_node<T>(arg_list)
    {
    }

    inline T value() const override
    {
        return Switch_N::process(switch_node<T>::arg_list_);
    }
};

template <typename T>
class multi_switch_node final : public expression_node<T>
{
   public:
    using expression_ptr = expression_node<T>*;
    using branch_t = std::pair<expression_ptr, bool>;

    template <typename Allocator, template <typename, typename> class Sequence>
    explicit multi_switch_node(const Sequence<expression_ptr, Allocator>& arg_list)
    {
        if (0 != (arg_list.size() & 1))
            return;

        arg_list_.resize(arg_list.size());

        for (std::size_t i = 0; i < arg_list.size(); ++i)
        {
            if (arg_list[i] && arg_list[i]->valid())
            {
                construct_branch_pair(arg_list_[i], arg_list[i]);
            }
            else
            {
                arg_list_.clear();
                return;
            }
        }

        assert(valid());
    }

    inline T value() const override
    {
        const std::size_t upper_bound = (arg_list_.size() - 1);

        T result = T(0);

        for (std::size_t i = 0; i < upper_bound; i += 2)
        {
            expression_ptr condition = arg_list_[i].first;
            expression_ptr consequent = arg_list_[i + 1].first;

            if (is_true(condition))
            {
                result = consequent->value();
            }
        }

        return result;
    }

    inline typename expression_node<T>::node_type type() const override
    {
        return expression_node<T>::node_type::e_mswitch;
    }

    inline bool valid() const override
    {
        return !arg_list_.empty() && (0 == (arg_list_.size() % 2));
    }

    void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) override
    {
        expression_node<T>::ndb_t::collect(arg_list_, node_delete_list);
    }

    std::size_t node_depth() const override final
    {
        return expression_node<T>::ndb_t::compute_node_depth(arg_list_);
    }

   private:
    std::vector<branch_t> arg_list_;
};

template <typename T>
class ivariable
{
   public:
    virtual ~ivariable() {}

    virtual T& ref() = 0;
    virtual const T& ref() const = 0;
};

}  // namespace math_expr::details

#endif
