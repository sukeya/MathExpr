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

    struct uv_data
    {
        core::operators::operator_type operation;
        const T* ref;
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

    using node_data_t = std::variant<literal_data, variable_data, unary_data, binary_data,
                                     trinary_data, uv_data, unary_branch_data, vov_data, cov_data,
                                     voc_data, vob_data, bov_data, cob_data, boc_data, uvouv_data>;

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

            T operator()(const uv_data& data) const
            {
                return core::operators::process<T>(data.operation, *data.ref);
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
                                                  typename node_variant_adapter_t::uv_hot_view>)
                {
                    return emplace(uv_data{view.uv->operation(), &view.uv->v()});
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
