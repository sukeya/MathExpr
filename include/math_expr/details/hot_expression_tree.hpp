#ifndef MATH_EXPR_DETAILS_HOT_EXPRESSION_TREE_HPP
#define MATH_EXPR_DETAILS_HOT_EXPRESSION_TREE_HPP

#include "math_expr/details/node_variant_adapter.hpp"

namespace math_expr::details
{
template <typename T>
class hot_expression_tree
{
   public:
    using expression_ptr = expression_node<T>*;
    using node_variant_adapter_t = node_variant_adapter<T>;
    using node_index_t = std::uint32_t;

    struct literal_data
    {
        T value;
    };

    struct variable_data
    {
        const T* ref;
    };

    struct unary_data
    {
        core::operators::operator_type operation;
        node_index_t child;
    };

    struct binary_data
    {
        core::operators::operator_type operation;
        node_index_t lhs;
        node_index_t rhs;
    };

    struct trinary_data
    {
        core::operators::operator_type operation;
        node_index_t arg0;
        node_index_t arg1;
        node_index_t arg2;
    };

    struct sf3_data
    {
        using trinary_functor_t = typename core::numeric::functor_t<T>::tfunc_t;

        trinary_functor_t functor;
        node_index_t arg0;
        node_index_t arg1;
        node_index_t arg2;
    };

    struct sf4_data
    {
        using quaternary_functor_t = typename core::numeric::functor_t<T>::qfunc_t;

        quaternary_functor_t functor;
        node_index_t arg0;
        node_index_t arg1;
        node_index_t arg2;
        node_index_t arg3;
    };

    struct fixed_function_data
    {
        fixed_function_base_node<T>* function;
        std::array<node_index_t, 20> args;
        std::uint8_t arity;
    };

    struct conditional_data
    {
        node_index_t condition;
        node_index_t consequent;
        node_index_t alternative;
        bool has_alternative;
    };

    struct scand_data
    {
        node_index_t lhs;
        node_index_t rhs;
    };

    struct scor_data
    {
        node_index_t lhs;
        node_index_t rhs;
    };

    struct uv_data
    {
        core::operators::operator_type operation;
        const T* ref;
    };

    struct scalar_pow_data
    {
        using powfun_t = typename node_variant_adapter_t::scalar_pow_base_node_t::powfun_t;

        powfun_t pow_function;
        bool reciprocal;
        const T* ref;
    };

    struct branch_pow_data
    {
        using powfun_t = typename node_variant_adapter_t::branch_pow_base_node_t::powfun_t;

        powfun_t pow_function;
        bool reciprocal;
        node_index_t child;
    };

    struct unary_branch_data
    {
        core::operators::operator_type operation;
        node_index_t child;
    };

    struct vov_data
    {
        core::operators::operator_type operation;
        const T* lhs;
        const T* rhs;
    };

    struct cov_data
    {
        core::operators::operator_type operation;
        T c;
        const T* v;
    };

    struct voc_data
    {
        core::operators::operator_type operation;
        const T* v;
        T c;
    };

    struct vob_data
    {
        core::operators::operator_type operation;
        const T* v;
        node_index_t child;
    };

    struct bov_data
    {
        core::operators::operator_type operation;
        node_index_t child;
        const T* v;
    };

    struct cob_data
    {
        core::operators::operator_type operation;
        T c;
        node_index_t child;
    };

    struct boc_data
    {
        core::operators::operator_type operation;
        node_index_t child;
        T c;
    };

    struct uvouv_data
    {
        using functor_t = typename core::numeric::functor_t<T>;
        using unary_functor_t = typename functor_t::ufunc_t;
        using binary_functor_t = typename functor_t::bfunc_t;

        const T* v0;
        const T* v1;
        unary_functor_t u0;
        unary_functor_t u1;
        binary_functor_t f;
    };

    struct operand_slot
    {
        const T* ref;
        T value;
        bool is_ref;
    };

    struct t0ot1ot2_data
    {
        using binary_functor_t = typename core::numeric::functor_t<T>::bfunc_t;
        using trinary_functor_t = typename core::numeric::functor_t<T>::tfunc_t;

        std::array<operand_slot, 3> operands;
        binary_functor_t f0;
        binary_functor_t f1;
        trinary_functor_t tf;
        std::uint8_t mode;
        bool use_trinary_functor;
    };

    struct t0ot1ot2ot3_data
    {
        using binary_functor_t = typename core::numeric::functor_t<T>::bfunc_t;
        using quaternary_functor_t = typename core::numeric::functor_t<T>::qfunc_t;

        std::array<operand_slot, 4> operands;
        binary_functor_t f0;
        binary_functor_t f1;
        binary_functor_t f2;
        quaternary_functor_t qf;
        std::uint8_t mode;
        bool use_quaternary_functor;
    };

    struct nulleq_data
    {
        node_index_t child;
        bool equality;
    };

    struct fallback_subtree_data
    {
        expression_ptr node;
    };

    struct vararg_multi_data
    {
        std::vector<node_index_t> args;
    };

    struct vec_celem_data
    {
        T* vector_base;
        std::size_t index;
        node_index_t vec_node;
    };

    struct vec_elem_data
    {
        T* vector_base;
        node_index_t vec_node;
        node_index_t index_child;
    };

    struct swap_data
    {
        T* var0;
        T* var1;
    };

    struct vec_elem_rtc_data
    {
        using vector_holder_t = vector_holder<T>;

        vector_holder_t* holder;
        T* vector_base;
        std::size_t max_vector_index;
        vector_access_runtime_check<T>* rt_check;
        node_index_t vec_node;
        node_index_t index_child;
    };

    struct vec_celem_rtc_data
    {
        using vector_holder_t = vector_holder<T>;

        vector_holder_t* holder;
        T* vector_base;
        std::size_t index;
        std::size_t max_vector_index;
        vector_access_runtime_check<T>* rt_check;
        node_index_t vec_node;
    };

    struct rbvec_elem_data
    {
        using vector_holder_t = vector_holder<T>;

        vector_holder_t* holder;
        node_index_t vec_node;
        node_index_t index_child;
    };

    struct rbvec_celem_data
    {
        using vector_holder_t = vector_holder<T>;

        vector_holder_t* holder;
        std::size_t index;
        node_index_t vec_node;
    };

    using node_data_t =
        std::variant<literal_data, variable_data, unary_data, binary_data, trinary_data, sf3_data,
                     sf4_data, fixed_function_data, uv_data, conditional_data, scand_data,
                     scor_data, scalar_pow_data, branch_pow_data, unary_branch_data, vov_data,
                     cov_data, voc_data, vob_data, bov_data, cob_data, boc_data, uvouv_data,
                     t0ot1ot2_data, t0ot1ot2ot3_data, nulleq_data, vararg_multi_data,
                     vec_celem_data, vec_elem_data, swap_data, vec_elem_rtc_data,
                     vec_celem_rtc_data, rbvec_elem_data, rbvec_celem_data, fallback_subtree_data>;

    struct node
    {
        node_data_t data;
    };

    static std::unique_ptr<hot_expression_tree> try_build(expression_ptr root)
    {
        auto tree = std::unique_ptr<hot_expression_tree>(new hot_expression_tree());
        const std::optional<node_index_t> root_index = tree->append(root);
        if (!root_index.has_value())
        {
            return nullptr;
        }

        tree->root_ = *root_index;
        return tree;
    }

    T value() const
    {
        return evaluate(root_);
    }

    std::size_t size() const
    {
        return nodes_.size();
    }

   private:
    hot_expression_tree() = default;

    static operand_slot make_operand_slot(const bool is_ref, const T* ref, const T value)
    {
        return operand_slot{ref, value, is_ref};
    }

    static T resolve_operand(const operand_slot& slot)
    {
        return slot.is_ref ? *slot.ref : slot.value;
    }

    static T eval_trinary(const trinary_data& data, const T arg0, const T arg1, const T arg2)
    {
        switch (data.operation)
        {
            case core::operators::operator_type::inrange:
                return (arg1 < arg0)
                           ? core::numeric::false_v<T>
                           : ((arg1 > arg2) ? core::numeric::false_v<T> : core::numeric::true_v<T>);

            case core::operators::operator_type::clamp:
                return (arg1 < arg0) ? arg0 : (arg1 > arg2 ? arg2 : arg1);

            case core::operators::operator_type::iclamp:
                if ((arg1 <= arg0) || (arg1 >= arg2))
                    return arg1;
                else
                    return ((T(2) * arg1 <= (arg2 + arg0)) ? arg0 : arg2);

            default:
                return std::numeric_limits<T>::quiet_NaN();
        }
    }

    static T eval_t0ot1ot2(const t0ot1ot2_data& data)
    {
        const T t0 = resolve_operand(data.operands[0]);
        const T t1 = resolve_operand(data.operands[1]);
        const T t2 = resolve_operand(data.operands[2]);

        if (data.use_trinary_functor)
        {
            return data.tf(t0, t1, t2);
        }

        switch (data.mode)
        {
            case 0:
                return data.f1(data.f0(t0, t1), t2);
            case 1:
                return data.f0(t0, data.f1(t1, t2));
            default:
                return std::numeric_limits<T>::quiet_NaN();
        }
    }

    static T eval_t0ot1ot2ot3(const t0ot1ot2ot3_data& data)
    {
        const T t0 = resolve_operand(data.operands[0]);
        const T t1 = resolve_operand(data.operands[1]);
        const T t2 = resolve_operand(data.operands[2]);
        const T t3 = resolve_operand(data.operands[3]);

        if (data.use_quaternary_functor)
        {
            return data.qf(t0, t1, t2, t3);
        }

        switch (data.mode)
        {
            case 0:
                return data.f1(data.f0(t0, t1), data.f2(t2, t3));
            case 1:
                return data.f0(t0, data.f1(t1, data.f2(t2, t3)));
            case 2:
                return data.f0(t0, data.f2(data.f1(t1, t2), t3));
            case 3:
                return data.f2(data.f1(data.f0(t0, t1), t2), t3);
            case 4:
                return data.f2(data.f0(t0, data.f1(t1, t2)), t3);
            default:
                return std::numeric_limits<T>::quiet_NaN();
        }
    }

    T evaluate(const node_index_t index) const
    {
        struct visitor
        {
            const hot_expression_tree& tree;

            T operator()(const literal_data& data) const
            {
                return data.value;
            }

            T operator()(const variable_data& data) const
            {
                return *data.ref;
            }

            T operator()(const unary_data& data) const
            {
                return core::operators::process<T>(data.operation, tree.evaluate(data.child));
            }

            T operator()(const binary_data& data) const
            {
                return core::operators::process<T>(data.operation, tree.evaluate(data.lhs),
                                                   tree.evaluate(data.rhs));
            }

            T operator()(const trinary_data& data) const
            {
                return hot_expression_tree::eval_trinary(data, tree.evaluate(data.arg0),
                                                         tree.evaluate(data.arg1),
                                                         tree.evaluate(data.arg2));
            }

            T operator()(const sf3_data& data) const
            {
                return data.functor(tree.evaluate(data.arg0), tree.evaluate(data.arg1),
                                    tree.evaluate(data.arg2));
            }

            T operator()(const sf4_data& data) const
            {
                return data.functor(tree.evaluate(data.arg0), tree.evaluate(data.arg1),
                                    tree.evaluate(data.arg2), tree.evaluate(data.arg3));
            }

            T operator()(const fixed_function_data& data) const
            {
                std::array<T, 20> values{};
                for (std::size_t i = 0; i < data.arity; ++i)
                {
                    values[i] = tree.evaluate(data.args[i]);
                }
                return data.function->evaluate_values(values.data());
            }

            T operator()(const conditional_data& data) const
            {
                if (is_true(tree.evaluate(data.condition)))
                {
                    return tree.evaluate(data.consequent);
                }

                return data.has_alternative ? tree.evaluate(data.alternative)
                                            : std::numeric_limits<T>::quiet_NaN();
            }

            T operator()(const scand_data& data) const
            {
                if (!is_true(tree.evaluate(data.lhs)))
                {
                    return core::numeric::false_v<T>;
                }

                return is_true(tree.evaluate(data.rhs)) ? core::numeric::true_v<T>
                                                        : core::numeric::false_v<T>;
            }

            T operator()(const scor_data& data) const
            {
                if (is_true(tree.evaluate(data.lhs)))
                {
                    return core::numeric::true_v<T>;
                }

                return is_true(tree.evaluate(data.rhs)) ? core::numeric::true_v<T>
                                                        : core::numeric::false_v<T>;
            }

            T operator()(const uv_data& data) const
            {
                return core::operators::process<T>(data.operation, *data.ref);
            }

            T operator()(const scalar_pow_data& data) const
            {
                const T base = data.pow_function(*data.ref);
                return data.reciprocal ? (T(1) / base) : base;
            }

            T operator()(const branch_pow_data& data) const
            {
                const T base = data.pow_function(tree.evaluate(data.child));
                return data.reciprocal ? (T(1) / base) : base;
            }

            T operator()(const unary_branch_data& data) const
            {
                return core::operators::process<T>(data.operation, tree.evaluate(data.child));
            }

            T operator()(const vov_data& data) const
            {
                return core::operators::process<T>(data.operation, *data.lhs, *data.rhs);
            }

            T operator()(const cov_data& data) const
            {
                return core::operators::process<T>(data.operation, data.c, *data.v);
            }

            T operator()(const voc_data& data) const
            {
                return core::operators::process<T>(data.operation, *data.v, data.c);
            }

            T operator()(const vob_data& data) const
            {
                return core::operators::process<T>(data.operation, *data.v,
                                                   tree.evaluate(data.child));
            }

            T operator()(const bov_data& data) const
            {
                return core::operators::process<T>(data.operation, tree.evaluate(data.child),
                                                   *data.v);
            }

            T operator()(const cob_data& data) const
            {
                return core::operators::process<T>(data.operation, data.c,
                                                   tree.evaluate(data.child));
            }

            T operator()(const boc_data& data) const
            {
                return core::operators::process<T>(data.operation, tree.evaluate(data.child),
                                                   data.c);
            }

            T operator()(const uvouv_data& data) const
            {
                return data.f(data.u0(*data.v0), data.u1(*data.v1));
            }

            T operator()(const t0ot1ot2_data& data) const
            {
                return hot_expression_tree::eval_t0ot1ot2(data);
            }

            T operator()(const t0ot1ot2ot3_data& data) const
            {
                return hot_expression_tree::eval_t0ot1ot2ot3(data);
            }

            T operator()(const nulleq_data& data) const
            {
                const bool result = core::numeric::is_nan(tree.evaluate(data.child));
                if (result)
                {
                    return data.equality ? core::numeric::true_v<T> : core::numeric::false_v<T>;
                }

                return data.equality ? core::numeric::false_v<T> : core::numeric::true_v<T>;
            }

            T operator()(const vararg_multi_data& data) const
            {
                for (std::size_t i = 0; i + 1 < data.args.size(); ++i)
                {
                    tree.evaluate(data.args[i]);
                }
                return tree.evaluate(data.args.back());
            }

            T operator()(const vec_celem_data& data) const
            {
                tree.evaluate(data.vec_node);
                return *(data.vector_base + data.index);
            }

            T operator()(const vec_elem_data& data) const
            {
                tree.evaluate(data.vec_node);
                const auto idx = core::numeric::to_uint64(tree.evaluate(data.index_child));
                return *(data.vector_base + idx);
            }

            T operator()(const swap_data& data) const
            {
                std::swap(*data.var0, *data.var1);
                return *data.var1;
            }

            T operator()(const vec_elem_rtc_data& data) const
            {
                const std::uint64_t index =
                    core::numeric::to_uint64(tree.evaluate(data.index_child));
                tree.evaluate(data.vec_node);

                if (index <= data.max_vector_index)
                {
                    return *(data.holder->data() + index);
                }

                typename vector_access_runtime_check<T>::violation_context context;
                context.base_ptr = data.vector_base;
                context.end_ptr = data.vector_base + data.holder->size();
                context.access_ptr = data.vector_base + index;
                context.type_size = sizeof(T);

                T* result_ptr = data.rt_check->handle_runtime_violation(context)
                                    ? context.access_ptr
                                    : data.vector_base;
                return *result_ptr;
            }

            T operator()(const vec_celem_rtc_data& data) const
            {
                tree.evaluate(data.vec_node);

                if (data.index <= data.max_vector_index)
                {
                    return *(data.holder->data() + data.index);
                }

                typename vector_access_runtime_check<T>::violation_context context;
                context.base_ptr = data.vector_base;
                context.end_ptr = data.vector_base + data.holder->size();
                context.access_ptr = data.vector_base + data.index;
                context.type_size = sizeof(T);

                T* result_ptr = data.rt_check->handle_runtime_violation(context)
                                    ? context.access_ptr
                                    : data.vector_base;
                return *result_ptr;
            }

            T operator()(const rbvec_elem_data& data) const
            {
                tree.evaluate(data.vec_node);
                const auto idx = core::numeric::to_uint64(tree.evaluate(data.index_child));
                return *(data.holder->data() + idx);
            }

            T operator()(const rbvec_celem_data& data) const
            {
                tree.evaluate(data.vec_node);
                return *(data.holder->data() + data.index);
            }

            T operator()(const fallback_subtree_data& data) const
            {
                return node_variant_adapter_t::value(data.node);
            }
        };

        return std::visit(visitor{*this}, nodes_[index].data);
    }

    std::optional<node_index_t> append(expression_ptr expr_node)
    {
        const auto hot_variant = node_variant_adapter_t::classify_hot(expr_node);

        auto append_child = [this](expression_ptr child) -> std::optional<node_index_t>
        {
            if (nullptr == child)
            {
                return std::nullopt;
            }
            return append(child);
        };

        auto emplace = [this](node_data_t data) -> std::optional<node_index_t>
        {
            nodes_.push_back(node{std::move(data)});
            return static_cast<node_index_t>(nodes_.size() - 1);
        };

        return std::visit(
            [this, &append_child, &emplace](const auto& view) -> std::optional<node_index_t>
            {
                using view_t = std::decay_t<decltype(view)>;

                if constexpr (std::is_same_v<view_t, std::monostate>)
                {
                    return std::nullopt;
                }
                else if constexpr (std::is_same_v<view_t,
                                                  typename node_variant_adapter_t::literal_view>)
                {
                    return emplace(literal_data{view.literal->value()});
                }
                else if constexpr (std::is_same_v<view_t,
                                                  typename node_variant_adapter_t::variable_view>)
                {
                    return emplace(variable_data{&view.variable->ref()});
                }
                else if constexpr (std::is_same_v<view_t,
                                                  typename node_variant_adapter_t::unary_hot_view>)
                {
                    const auto child = append_child(view.unary->branch(0));
                    if (!child.has_value())
                        return std::nullopt;
                    return emplace(unary_data{view.unary->operation(), *child});
                }
                else if constexpr (std::is_same_v<view_t,
                                                  typename node_variant_adapter_t::binary_hot_view>)
                {
                    const auto lhs = append_child(view.binary->branch(0));
                    const auto rhs = append_child(view.binary->branch(1));
                    if (!lhs.has_value() || !rhs.has_value())
                        return std::nullopt;
                    return emplace(binary_data{view.binary->operation(), *lhs, *rhs});
                }
                else if constexpr (std::is_same_v<
                                       view_t, typename node_variant_adapter_t::trinary_hot_view>)
                {
                    const auto arg0 = append_child(view.trinary->branch(0));
                    const auto arg1 = append_child(view.trinary->branch(1));
                    const auto arg2 = append_child(view.trinary->branch(2));
                    if (!arg0.has_value() || !arg1.has_value() || !arg2.has_value())
                        return std::nullopt;
                    return emplace(trinary_data{view.trinary->operation(), *arg0, *arg1, *arg2});
                }
                else if constexpr (std::is_same_v<view_t,
                                                  typename node_variant_adapter_t::sf3_hot_view>)
                {
                    const auto arg0 = append_child(view.sf3->branch(0));
                    const auto arg1 = append_child(view.sf3->branch(1));
                    const auto arg2 = append_child(view.sf3->branch(2));
                    if (!arg0.has_value() || !arg1.has_value() || !arg2.has_value())
                    {
                        return std::nullopt;
                    }
                    return emplace(sf3_data{view.sf3->functor(), *arg0, *arg1, *arg2});
                }
                else if constexpr (std::is_same_v<view_t,
                                                  typename node_variant_adapter_t::sf4_hot_view>)
                {
                    const auto arg0 = append_child(view.sf4->branch(0));
                    const auto arg1 = append_child(view.sf4->branch(1));
                    const auto arg2 = append_child(view.sf4->branch(2));
                    const auto arg3 = append_child(view.sf4->branch(3));
                    if (!arg0.has_value() || !arg1.has_value() || !arg2.has_value() ||
                        !arg3.has_value())
                    {
                        return std::nullopt;
                    }
                    return emplace(sf4_data{view.sf4->functor(), *arg0, *arg1, *arg2, *arg3});
                }
                else if constexpr (std::is_same_v<
                                       view_t,
                                       typename node_variant_adapter_t::fixed_function_hot_view>)
                {
                    fixed_function_data data{};
                    data.function = view.function;
                    data.arity = static_cast<std::uint8_t>(view.function->arity());
                    for (std::size_t i = 0; i < data.arity; ++i)
                    {
                        const auto child = append_child(view.function->branch(i));
                        if (!child.has_value())
                        {
                            return std::nullopt;
                        }
                        data.args[i] = *child;
                    }
                    return emplace(data);
                }
                else if constexpr (std::is_same_v<
                                       view_t,
                                       typename node_variant_adapter_t::conditional_hot_view>)
                {
                    const auto condition =
                        append_child(node_variant_adapter_t::branch(view.node, 0));
                    const auto consequent =
                        append_child(node_variant_adapter_t::branch(view.node, 1));
                    if (!condition.has_value() || !consequent.has_value())
                    {
                        return std::nullopt;
                    }

                    expression_ptr alternative_node = node_variant_adapter_t::branch(view.node, 2);
                    if (alternative_node)
                    {
                        const auto alternative = append_child(alternative_node);
                        if (!alternative.has_value())
                        {
                            return std::nullopt;
                        }
                        return emplace(
                            conditional_data{*condition, *consequent, *alternative, true});
                    }

                    return emplace(conditional_data{*condition, *consequent, 0, false});
                }
                else if constexpr (std::is_same_v<view_t,
                                                  typename node_variant_adapter_t::scand_hot_view>)
                {
                    const auto lhs = append_child(node_variant_adapter_t::branch(view.node, 0));
                    const auto rhs = append_child(node_variant_adapter_t::branch(view.node, 1));
                    if (!lhs.has_value() || !rhs.has_value())
                    {
                        return std::nullopt;
                    }
                    return emplace(scand_data{*lhs, *rhs});
                }
                else if constexpr (std::is_same_v<view_t,
                                                  typename node_variant_adapter_t::scor_hot_view>)
                {
                    const auto lhs = append_child(node_variant_adapter_t::branch(view.node, 0));
                    const auto rhs = append_child(node_variant_adapter_t::branch(view.node, 1));
                    if (!lhs.has_value() || !rhs.has_value())
                    {
                        return std::nullopt;
                    }
                    return emplace(scor_data{*lhs, *rhs});
                }
                else if constexpr (std::is_same_v<view_t,
                                                  typename node_variant_adapter_t::uv_hot_view>)
                {
                    return emplace(uv_data{view.uv->operation(), &view.uv->v()});
                }
                else if constexpr (std::is_same_v<
                                       view_t,
                                       typename node_variant_adapter_t::scalar_pow_hot_view>)
                {
                    return emplace(scalar_pow_data{view.pow->pow_function(), view.pow->reciprocal(),
                                                   &view.pow->v()});
                }
                else if constexpr (std::is_same_v<
                                       view_t,
                                       typename node_variant_adapter_t::branch_pow_hot_view>)
                {
                    const auto child = append_child(node_variant_adapter_t::branch(view.node));
                    if (!child.has_value())
                        return std::nullopt;
                    return emplace(
                        branch_pow_data{view.pow->pow_function(), view.pow->reciprocal(), *child});
                }
                else if constexpr (std::is_same_v<
                                       view_t,
                                       typename node_variant_adapter_t::unary_branch_hot_view>)
                {
                    const auto child = append_child(node_variant_adapter_t::branch(view.node));
                    if (!child.has_value())
                        return std::nullopt;
                    return emplace(unary_branch_data{view.operation, *child});
                }
                else if constexpr (std::is_same_v<view_t,
                                                  typename node_variant_adapter_t::vov_hot_view>)
                {
                    return emplace(
                        vov_data{view.vov->operation(), &view.vov->v0(), &view.vov->v1()});
                }
                else if constexpr (std::is_same_v<view_t,
                                                  typename node_variant_adapter_t::cov_hot_view>)
                {
                    return emplace(cov_data{view.cov->operation(), view.cov->c(), &view.cov->v()});
                }
                else if constexpr (std::is_same_v<view_t,
                                                  typename node_variant_adapter_t::voc_hot_view>)
                {
                    return emplace(voc_data{view.voc->operation(), &view.voc->v(), view.voc->c()});
                }
                else if constexpr (std::is_same_v<view_t,
                                                  typename node_variant_adapter_t::vob_hot_view>)
                {
                    const auto child = append_child(node_variant_adapter_t::branch(view.node));
                    if (!child.has_value())
                        return std::nullopt;
                    return emplace(vob_data{view.vob->operation(), &view.vob->v(), *child});
                }
                else if constexpr (std::is_same_v<view_t,
                                                  typename node_variant_adapter_t::bov_hot_view>)
                {
                    const auto child = append_child(node_variant_adapter_t::branch(view.node));
                    if (!child.has_value())
                        return std::nullopt;
                    return emplace(bov_data{view.bov->operation(), *child, &view.bov->v()});
                }
                else if constexpr (std::is_same_v<view_t,
                                                  typename node_variant_adapter_t::cob_hot_view>)
                {
                    const auto child = append_child(node_variant_adapter_t::branch(view.node));
                    if (!child.has_value())
                        return std::nullopt;
                    return emplace(cob_data{view.cob->operation(), view.cob->c(), *child});
                }
                else if constexpr (std::is_same_v<view_t,
                                                  typename node_variant_adapter_t::boc_hot_view>)
                {
                    const auto child = append_child(node_variant_adapter_t::branch(view.node));
                    if (!child.has_value())
                        return std::nullopt;
                    return emplace(boc_data{view.boc->operation(), *child, view.boc->c()});
                }
                else if constexpr (std::is_same_v<view_t,
                                                  typename node_variant_adapter_t::uvouv_hot_view>)
                {
                    return emplace(uvouv_data{&view.uvouv->v0(), &view.uvouv->v1(),
                                              view.uvouv->u0(), view.uvouv->u1(), view.uvouv->f()});
                }
                else if constexpr (std::is_same_v<
                                       view_t, typename node_variant_adapter_t::t0ot1ot2_hot_view>)
                {
                    std::array<operand_slot, 3> operands{{
                        make_operand_slot(view.base->operand_is_reference(0),
                                          view.base->operand_reference(0),
                                          view.base->operand_value(0)),
                        make_operand_slot(view.base->operand_is_reference(1),
                                          view.base->operand_reference(1),
                                          view.base->operand_value(1)),
                        make_operand_slot(view.base->operand_is_reference(2),
                                          view.base->operand_reference(2),
                                          view.base->operand_value(2)),
                    }};
                    return emplace(t0ot1ot2_data{operands, view.base->binary_functor(0),
                                                 view.base->binary_functor(1),
                                                 view.base->ternary_functor(),
                                                 static_cast<std::uint8_t>(view.base->mode_index()),
                                                 nullptr != view.base->ternary_functor()});
                }
                else if constexpr (std::is_same_v<
                                       view_t,
                                       typename node_variant_adapter_t::t0ot1ot2ot3_hot_view>)
                {
                    std::array<operand_slot, 4> operands{{
                        make_operand_slot(view.base->operand_is_reference(0),
                                          view.base->operand_reference(0),
                                          view.base->operand_value(0)),
                        make_operand_slot(view.base->operand_is_reference(1),
                                          view.base->operand_reference(1),
                                          view.base->operand_value(1)),
                        make_operand_slot(view.base->operand_is_reference(2),
                                          view.base->operand_reference(2),
                                          view.base->operand_value(2)),
                        make_operand_slot(view.base->operand_is_reference(3),
                                          view.base->operand_reference(3),
                                          view.base->operand_value(3)),
                    }};
                    return emplace(t0ot1ot2ot3_data{
                        operands, view.base->binary_functor(0), view.base->binary_functor(1),
                        view.base->binary_functor(2), view.base->quaternary_functor(),
                        static_cast<std::uint8_t>(view.base->mode_index()),
                        nullptr != view.base->quaternary_functor()});
                }
                else if constexpr (std::is_same_v<view_t,
                                                  typename node_variant_adapter_t::nulleq_hot_view>)
                {
                    const auto child = append_child(node_variant_adapter_t::branch(view.node, 0));
                    if (!child.has_value())
                    {
                        return std::nullopt;
                    }
                    return emplace(nulleq_data{*child, view.nulleq->equality()});
                }
                else if constexpr (std::is_same_v<
                                       view_t,
                                       typename node_variant_adapter_t::vararg_multi_hot_view>)
                {
                    vararg_multi_data data{};
                    data.args.reserve(view.count);
                    for (std::size_t i = 0; i < view.count; ++i)
                    {
                        const auto child = append_child(view.node->branch(i));
                        if (!child.has_value())
                        {
                            return std::nullopt;
                        }
                        data.args.push_back(*child);
                    }
                    return emplace(std::move(data));
                }
                else if constexpr (std::is_same_v<
                                       view_t, typename node_variant_adapter_t::vec_celem_hot_view>)
                {
                    const auto vec_child = append_child(view.celem->vec_branch());
                    if (!vec_child.has_value())
                    {
                        return std::nullopt;
                    }
                    return emplace(
                        vec_celem_data{view.celem->vec_data(), view.celem->elem_idx(), *vec_child});
                }
                else if constexpr (std::is_same_v<
                                       view_t, typename node_variant_adapter_t::vec_elem_hot_view>)
                {
                    const auto vec_child = append_child(view.elem->vec_branch());
                    const auto idx_child = append_child(view.elem->index_branch());
                    if (!vec_child.has_value() || !idx_child.has_value())
                    {
                        return std::nullopt;
                    }
                    return emplace(vec_elem_data{view.elem->vec_data(), *vec_child, *idx_child});
                }
                else if constexpr (std::is_same_v<view_t,
                                                  typename node_variant_adapter_t::swap_hot_view>)
                {
                    return emplace(swap_data{&view.swap_ptr->var0_ptr()->ref(),
                                             &view.swap_ptr->var1_ptr()->ref()});
                }
                else if constexpr (std::is_same_v<
                                       view_t,
                                       typename node_variant_adapter_t::vec_elem_rtc_hot_view>)
                {
                    const auto vec_child = append_child(view.rtc->vec_branch());
                    const auto idx_child = append_child(view.rtc->index_branch());
                    if (!vec_child.has_value() || !idx_child.has_value())
                    {
                        return std::nullopt;
                    }
                    return emplace(vec_elem_rtc_data{view.rtc->holder(), view.rtc->vec_data(),
                                                     view.rtc->max_idx(), view.rtc->rt_check(),
                                                     *vec_child, *idx_child});
                }
                else if constexpr (std::is_same_v<
                                       view_t,
                                       typename node_variant_adapter_t::vec_celem_rtc_hot_view>)
                {
                    const auto vec_child = append_child(view.rtc->vec_branch());
                    if (!vec_child.has_value())
                    {
                        return std::nullopt;
                    }
                    return emplace(vec_celem_rtc_data{view.rtc->holder(), view.rtc->vec_data(),
                                                      view.rtc->elem_idx(), view.rtc->max_idx(),
                                                      view.rtc->rt_check(), *vec_child});
                }
                else if constexpr (std::is_same_v<
                                       view_t,
                                       typename node_variant_adapter_t::rbvec_elem_hot_view>)
                {
                    const auto vec_child = append_child(view.rbvec->vec_branch());
                    const auto idx_child = append_child(view.rbvec->index_branch());
                    if (!vec_child.has_value() || !idx_child.has_value())
                    {
                        return std::nullopt;
                    }
                    return emplace(rbvec_elem_data{view.rbvec->holder(), *vec_child, *idx_child});
                }
                else if constexpr (std::is_same_v<
                                       view_t,
                                       typename node_variant_adapter_t::rbvec_celem_hot_view>)
                {
                    const auto vec_child = append_child(view.rbvec->vec_branch());
                    if (!vec_child.has_value())
                    {
                        return std::nullopt;
                    }
                    return emplace(
                        rbvec_celem_data{view.rbvec->holder(), view.rbvec->elem_idx(), *vec_child});
                }
                else if constexpr (std::is_same_v<view_t,
                                                  typename node_variant_adapter_t::fallback_view>)
                {
                    return emplace(fallback_subtree_data{view.node});
                }
                else
                {
                    return std::nullopt;
                }
            },
            hot_variant);
    }

    std::vector<node> nodes_;
    node_index_t root_{0};
};
}  // namespace math_expr::details

#endif
