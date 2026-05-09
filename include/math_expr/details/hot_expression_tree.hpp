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

    struct rbvec_elem_rtc_data
    {
        using vector_holder_t = vector_holder<T>;

        vector_holder_t* holder;
        vector_access_runtime_check<T>* rt_check;
        node_index_t vec_node;
        node_index_t index_child;
    };

    struct rbvec_celem_rtc_data
    {
        using vector_holder_t = vector_holder<T>;

        vector_holder_t* holder;
        T* vector_base;
        std::size_t index;
        vector_access_runtime_check<T>* rt_check;
        node_index_t vec_node;
    };

    struct vecsize_data
    {
        using vector_holder_t = vector_holder<T>;

        vector_holder_t* holder;
    };

    struct assign_data
    {
        T* var_ref;
        node_index_t rhs;
    };

    struct assign_vec_elem_data
    {
        T* vector_base;
        node_index_t vec_node;
        node_index_t index_child;
        node_index_t rhs;
    };

    struct assign_rbvec_elem_data
    {
        using vector_holder_t = vector_holder<T>;

        vector_holder_t* holder;
        node_index_t vec_node;
        node_index_t index_child;
        node_index_t rhs;
    };

    struct assign_rbvec_celem_data
    {
        using vector_holder_t = vector_holder<T>;

        vector_holder_t* holder;
        std::size_t index;
        node_index_t vec_node;
        node_index_t rhs;
    };

    struct assign_op_data
    {
        T* var_ref;
        core::operators::operator_type read_op;
        node_index_t rhs;
    };

    struct assign_vec_elem_op_data
    {
        T* vector_base;
        core::operators::operator_type read_op;
        node_index_t vec_node;
        node_index_t index_child;
        node_index_t rhs;
    };

    struct assign_vec_elem_rtc_data
    {
        using vector_holder_t = vector_holder<T>;

        vector_holder_t* holder;
        T* vector_base;
        std::size_t max_vector_index;
        vector_access_runtime_check<T>* rt_check;
        node_index_t vec_node;
        node_index_t index_child;
        node_index_t rhs;
    };

    struct assign_rbvec_elem_rtc_data
    {
        using vector_holder_t = vector_holder<T>;

        vector_holder_t* holder;
        vector_access_runtime_check<T>* rt_check;
        node_index_t vec_node;
        node_index_t index_child;
        node_index_t rhs;
    };

    struct assign_rbvec_elem_op_data
    {
        using vector_holder_t = vector_holder<T>;

        vector_holder_t* holder;
        core::operators::operator_type read_op;
        node_index_t vec_node;
        node_index_t index_child;
        node_index_t rhs;
    };

    struct assign_rbvec_celem_op_data
    {
        using vector_holder_t = vector_holder<T>;

        vector_holder_t* holder;
        std::size_t index;
        core::operators::operator_type read_op;
        node_index_t vec_node;
        node_index_t rhs;
    };

    struct assign_vec_elem_op_rtc_data
    {
        using vector_holder_t = vector_holder<T>;

        vector_holder_t* holder;
        T* vector_base;
        std::size_t max_vector_index;
        vector_access_runtime_check<T>* rt_check;
        core::operators::operator_type read_op;
        node_index_t vec_node;
        node_index_t index_child;
        node_index_t rhs;
    };

    struct assign_vec_celem_op_rtc_data
    {
        using vector_holder_t = vector_holder<T>;

        vector_holder_t* holder;
        T* vector_base;
        std::size_t index;
        std::size_t max_vector_index;
        vector_access_runtime_check<T>* rt_check;
        core::operators::operator_type read_op;
        node_index_t vec_node;
        node_index_t rhs;
    };

    struct assign_rbvec_elem_op_rtc_data
    {
        using vector_holder_t = vector_holder<T>;

        vector_holder_t* holder;
        vector_access_runtime_check<T>* rt_check;
        core::operators::operator_type read_op;
        node_index_t vec_node;
        node_index_t index_child;
        node_index_t rhs;
    };

    struct assign_rbvec_celem_op_rtc_data
    {
        using vector_holder_t = vector_holder<T>;

        vector_holder_t* holder;
        T* vector_base;
        std::size_t index;
        vector_access_runtime_check<T>* rt_check;
        core::operators::operator_type read_op;
        node_index_t vec_node;
        node_index_t rhs;
    };

    struct assign_vec_scalar_data
    {
        using vector_holder_t = vector_holder<T>;

        T* vector_base;
        vector_holder_t* holder;
        node_index_t rhs;
    };

    struct assign_vec_scalar_op_data
    {
        using vector_iface_t = vector_interface<T>;

        vector_iface_t* iface;
        core::operators::operator_type read_op;
        node_index_t rhs;
    };

    struct while_data
    {
        node_index_t condition;
        node_index_t body;
    };

    struct while_rtc_data
    {
        loop_runtime_checker* checker;
        node_index_t condition;
        node_index_t body;
    };

    struct repeat_until_data
    {
        node_index_t condition;
        node_index_t body;
    };

    struct repeat_until_rtc_data
    {
        loop_runtime_checker* checker;
        node_index_t condition;
        node_index_t body;
    };

    struct for_data
    {
        node_index_t condition;
        node_index_t body;
        node_index_t initialiser;
        node_index_t incrementor;
        bool has_initialiser;
        bool has_incrementor;
    };

    struct for_rtc_data
    {
        loop_runtime_checker* checker;
        node_index_t condition;
        node_index_t body;
        node_index_t initialiser;
        node_index_t incrementor;
        bool has_initialiser;
        bool has_incrementor;
    };

    struct switch_data
    {
        std::vector<std::pair<node_index_t, node_index_t>> cases;
        node_index_t default_case;
    };

    struct multi_switch_data
    {
        std::vector<std::pair<node_index_t, node_index_t>> cases;
    };

    struct assign_vecvec_data
    {
        using vector_holder_t = vector_holder<T>;

        T* vec0_base;
        T* vec1_base;
        vector_holder_t* holder0;
        vector_holder_t* holder1;
        node_index_t branch1_child;
    };

    struct assign_vecvec_op_data
    {
        using vector_holder_t = vector_holder<T>;

        core::operators::operator_type read_op;
        T* vec0_base;
        T* vec1_base;
        vector_holder_t* holder0;
        vector_holder_t* holder1;
        node_index_t branch0_child;
        node_index_t branch1_child;
    };

    struct vecinit_zero_data
    {
        T* vec_base;
        std::size_t vec_size;
    };

    struct vecinit_constfill_data
    {
        T* vec_base;
        std::size_t vec_size;
        T fill_value;
    };

    struct vecinit_dynfill_data
    {
        T* vec_base;
        std::size_t vec_size;
        node_index_t init_child;
    };

    struct vecinit_iota_cc_data
    {
        T* vec_base;
        std::size_t vec_size;
        T base_val;
        T increment_val;
    };

    struct vecinit_iota_cnc_data
    {
        T* vec_base;
        std::size_t vec_size;
        T base_val;
        node_index_t increment_child;
    };

    struct vecinit_iota_ncc_data
    {
        T* vec_base;
        std::size_t vec_size;
        T increment_val;
        node_index_t base_child;
    };

    struct vecinit_iota_ncnc_data
    {
        T* vec_base;
        std::size_t vec_size;
        node_index_t base_child;
        node_index_t increment_child;
    };

    struct vecinit_general_data
    {
        T* vec_base;
        std::size_t vec_size;
        std::vector<node_index_t> init_children;
        bool single_value;
    };

    struct vararg_evaluable_data
    {
        vararg_evaluable_node<T>* fn;
        mutable std::vector<T> temp_values;
        std::vector<node_index_t> children;
    };

    struct vec_binop_vecvec_data
    {
        core::operators::operator_type operation;
        const T* vec0_src;
        const T* vec1_src;
        T* vec_out;
        std::size_t vec_size;
        node_index_t branch0_child;
        node_index_t branch1_child;
    };

    struct vec_binop_vecval_data
    {
        core::operators::operator_type operation;
        const T* vec0_src;
        T* vec_out;
        std::size_t vec_size;
        node_index_t branch0_child;
        node_index_t branch1_child;
    };

    struct vec_binop_valvec_data
    {
        core::operators::operator_type operation;
        const T* vec1_src;
        T* vec_out;
        std::size_t vec_size;
        node_index_t branch0_child;
        node_index_t branch1_child;
    };

    struct unary_vec_data
    {
        core::operators::operator_type operation;
        const T* vec0_src;
        T* vec_out;
        std::size_t vec_size;
        node_index_t branch0_child;
    };

    struct vecfunc_data
    {
        using proc_fn_t = T (*)(vector_interface<T>*);
        proc_fn_t proc_fn;
        vector_interface<T>* ivec;
        node_index_t branch_child;
    };

    using node_data_t = std::variant<
        literal_data, variable_data, unary_data, binary_data, trinary_data, sf3_data, sf4_data,
        fixed_function_data, uv_data, conditional_data, scand_data, scor_data, scalar_pow_data,
        branch_pow_data, unary_branch_data, vov_data, cov_data, voc_data, vob_data, bov_data,
        cob_data, boc_data, uvouv_data, t0ot1ot2_data, t0ot1ot2ot3_data, nulleq_data,
        vararg_multi_data, vec_celem_data, vec_elem_data, swap_data, vec_elem_rtc_data,
        vec_celem_rtc_data, rbvec_elem_data, rbvec_celem_data, rbvec_elem_rtc_data,
        rbvec_celem_rtc_data, vecsize_data, assign_data, assign_vec_elem_data,
        assign_rbvec_elem_data, assign_rbvec_celem_data, assign_op_data, assign_vec_elem_op_data,
        assign_vec_elem_rtc_data, assign_rbvec_elem_rtc_data, assign_rbvec_elem_op_data,
        assign_rbvec_celem_op_data, assign_vec_elem_op_rtc_data, assign_vec_celem_op_rtc_data,
        assign_rbvec_elem_op_rtc_data, assign_rbvec_celem_op_rtc_data, assign_vec_scalar_data,
        assign_vec_scalar_op_data, while_data, while_rtc_data, repeat_until_data,
        repeat_until_rtc_data, for_data, for_rtc_data, switch_data, multi_switch_data,
        assign_vecvec_data, assign_vecvec_op_data, vecinit_zero_data, vecinit_constfill_data,
        vecinit_dynfill_data, vecinit_iota_cc_data, vecinit_iota_cnc_data, vecinit_iota_ncc_data,
        vecinit_iota_ncnc_data, vecinit_general_data, vararg_evaluable_data, vec_binop_vecvec_data,
        vec_binop_vecval_data, vec_binop_valvec_data, unary_vec_data, vecfunc_data,
        fallback_subtree_data>;

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

            T operator()(const rbvec_elem_rtc_data& data) const
            {
                tree.evaluate(data.vec_node);
                const std::uint64_t index =
                    core::numeric::to_uint64(tree.evaluate(data.index_child));

                if (index <= data.holder->size() - 1)
                {
                    return *(data.holder->data() + index);
                }

                typename vector_access_runtime_check<T>::violation_context context;
                context.base_ptr = data.holder->data();
                context.end_ptr = data.holder->data() + data.holder->size();
                context.access_ptr = data.holder->data() + index;
                context.type_size = sizeof(T);

                T* result_ptr = data.rt_check->handle_runtime_violation(context)
                                    ? context.access_ptr
                                    : data.holder->data();
                return *result_ptr;
            }

            T operator()(const rbvec_celem_rtc_data& data) const
            {
                tree.evaluate(data.vec_node);

                if (data.index <= data.holder->size() - 1)
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

            T operator()(const vecsize_data& data) const
            {
                return static_cast<T>(data.holder->size());
            }

            T operator()(const assign_data& data) const
            {
                return (*data.var_ref = tree.evaluate(data.rhs));
            }

            T operator()(const assign_vec_elem_data& data) const
            {
                tree.evaluate(data.vec_node);
                const auto idx = core::numeric::to_uint64(tree.evaluate(data.index_child));
                T& ref = *(data.vector_base + idx);
                return (ref = tree.evaluate(data.rhs));
            }

            T operator()(const assign_rbvec_elem_data& data) const
            {
                tree.evaluate(data.vec_node);
                const auto idx = core::numeric::to_uint64(tree.evaluate(data.index_child));
                T& ref = *(data.holder->data() + idx);
                return (ref = tree.evaluate(data.rhs));
            }

            T operator()(const assign_rbvec_celem_data& data) const
            {
                tree.evaluate(data.vec_node);
                T& ref = *(data.holder->data() + data.index);
                return (ref = tree.evaluate(data.rhs));
            }

            T operator()(const assign_op_data& data) const
            {
                T& v = *data.var_ref;
                v = core::operators::process<T>(data.read_op, v, tree.evaluate(data.rhs));
                return v;
            }

            T operator()(const assign_vec_elem_op_data& data) const
            {
                tree.evaluate(data.vec_node);
                const auto idx = core::numeric::to_uint64(tree.evaluate(data.index_child));
                T& ref = *(data.vector_base + idx);
                ref = core::operators::process<T>(data.read_op, ref, tree.evaluate(data.rhs));
                return ref;
            }

            T operator()(const assign_vec_elem_rtc_data& data) const
            {
                const auto idx = core::numeric::to_uint64(tree.evaluate(data.index_child));
                tree.evaluate(data.vec_node);
                T* ptr;
                if (idx <= data.max_vector_index)
                {
                    ptr = data.holder->data() + idx;
                }
                else
                {
                    typename vector_access_runtime_check<T>::violation_context context;
                    context.base_ptr = data.vector_base;
                    context.end_ptr = data.vector_base + data.holder->size();
                    context.access_ptr = data.vector_base + idx;
                    context.type_size = sizeof(T);
                    ptr = data.rt_check->handle_runtime_violation(context) ? context.access_ptr
                                                                           : data.vector_base;
                }
                T& ref = *ptr;
                return (ref = tree.evaluate(data.rhs));
            }

            T operator()(const assign_rbvec_elem_rtc_data& data) const
            {
                tree.evaluate(data.vec_node);
                const auto idx = core::numeric::to_uint64(tree.evaluate(data.index_child));
                T* ptr;
                if (idx <= data.holder->size() - 1)
                {
                    ptr = data.holder->data() + idx;
                }
                else
                {
                    typename vector_access_runtime_check<T>::violation_context context;
                    context.base_ptr = data.holder->data();
                    context.end_ptr = data.holder->data() + data.holder->size();
                    context.access_ptr = data.holder->data() + idx;
                    context.type_size = sizeof(T);
                    ptr = data.rt_check->handle_runtime_violation(context) ? context.access_ptr
                                                                           : data.holder->data();
                }
                T& ref = *ptr;
                return (ref = tree.evaluate(data.rhs));
            }

            T operator()(const assign_rbvec_elem_op_data& data) const
            {
                tree.evaluate(data.vec_node);
                const auto idx = core::numeric::to_uint64(tree.evaluate(data.index_child));
                T& ref = *(data.holder->data() + idx);
                ref = core::operators::process<T>(data.read_op, ref, tree.evaluate(data.rhs));
                return ref;
            }

            T operator()(const assign_rbvec_celem_op_data& data) const
            {
                tree.evaluate(data.vec_node);
                T& ref = *(data.holder->data() + data.index);
                ref = core::operators::process<T>(data.read_op, ref, tree.evaluate(data.rhs));
                return ref;
            }

            T operator()(const assign_vec_elem_op_rtc_data& data) const
            {
                const auto idx = core::numeric::to_uint64(tree.evaluate(data.index_child));
                tree.evaluate(data.vec_node);
                T* ptr;
                if (idx <= data.max_vector_index)
                {
                    ptr = data.holder->data() + idx;
                }
                else
                {
                    typename vector_access_runtime_check<T>::violation_context context;
                    context.base_ptr = data.vector_base;
                    context.end_ptr = data.vector_base + data.holder->size();
                    context.access_ptr = data.vector_base + idx;
                    context.type_size = sizeof(T);
                    ptr = data.rt_check->handle_runtime_violation(context) ? context.access_ptr
                                                                           : data.vector_base;
                }
                T& ref = *ptr;
                ref = core::operators::process<T>(data.read_op, ref, tree.evaluate(data.rhs));
                return ref;
            }

            T operator()(const assign_vec_celem_op_rtc_data& data) const
            {
                tree.evaluate(data.vec_node);
                T* ptr;
                if (data.index <= data.max_vector_index)
                {
                    ptr = data.holder->data() + data.index;
                }
                else
                {
                    typename vector_access_runtime_check<T>::violation_context context;
                    context.base_ptr = data.vector_base;
                    context.end_ptr = data.vector_base + data.holder->size();
                    context.access_ptr = data.vector_base + data.index;
                    context.type_size = sizeof(T);
                    ptr = data.rt_check->handle_runtime_violation(context) ? context.access_ptr
                                                                           : data.vector_base;
                }
                T& ref = *ptr;
                ref = core::operators::process<T>(data.read_op, ref, tree.evaluate(data.rhs));
                return ref;
            }

            T operator()(const assign_rbvec_elem_op_rtc_data& data) const
            {
                tree.evaluate(data.vec_node);
                const auto idx = core::numeric::to_uint64(tree.evaluate(data.index_child));
                T* ptr;
                if (idx <= data.holder->size() - 1)
                {
                    ptr = data.holder->data() + idx;
                }
                else
                {
                    typename vector_access_runtime_check<T>::violation_context context;
                    context.base_ptr = data.holder->data();
                    context.end_ptr = data.holder->data() + data.holder->size();
                    context.access_ptr = data.holder->data() + idx;
                    context.type_size = sizeof(T);
                    ptr = data.rt_check->handle_runtime_violation(context) ? context.access_ptr
                                                                           : data.holder->data();
                }
                T& ref = *ptr;
                ref = core::operators::process<T>(data.read_op, ref, tree.evaluate(data.rhs));
                return ref;
            }

            T operator()(const assign_rbvec_celem_op_rtc_data& data) const
            {
                tree.evaluate(data.vec_node);
                T* ptr;
                if (data.index <= data.holder->size() - 1)
                {
                    ptr = data.holder->data() + data.index;
                }
                else
                {
                    typename vector_access_runtime_check<T>::violation_context context;
                    context.base_ptr = data.vector_base;
                    context.end_ptr = data.vector_base + data.holder->size();
                    context.access_ptr = data.vector_base + data.index;
                    context.type_size = sizeof(T);
                    ptr = data.rt_check->handle_runtime_violation(context) ? context.access_ptr
                                                                           : data.vector_base;
                }
                T& ref = *ptr;
                ref = core::operators::process<T>(data.read_op, ref, tree.evaluate(data.rhs));
                return ref;
            }

            T operator()(const assign_vec_scalar_data& data) const
            {
                const T v = tree.evaluate(data.rhs);
                T* vec = data.vector_base;
                const std::size_t sz = data.holder->size();
                core::operators::loop_unroll lud(sz);
                const T* upper_bound = vec + lud.upper_bound;
                while (vec < upper_bound)
                {
                    lud.foreach_batch([v, &vec](unsigned int i) { vec[i] = v; });
                    vec += lud.loop_batch_size;
                }
                lud.foreach_remainder([&vec, v]() { *vec++ = v; });
                return data.vector_base[0];
            }

            T operator()(const assign_vec_scalar_op_data& data) const
            {
                const T v = tree.evaluate(data.rhs);
                T* const vec_base = data.iface->vds().data();
                const std::size_t sz = data.iface->size();
                T* vec = vec_base;
                core::operators::loop_unroll lud(sz);
                const T* upper_bound = vec + lud.upper_bound;
                while (vec < upper_bound)
                {
                    lud.foreach_batch(
                        [&](unsigned int i)
                        { vec[i] = core::operators::process<T>(data.read_op, vec[i], v); });
                    vec += lud.loop_batch_size;
                }
                lud.foreach_remainder(
                    [&]()
                    {
                        *vec = core::operators::process<T>(data.read_op, *vec, v);
                        ++vec;
                    });
                return vec_base[0];
            }

            T operator()(const while_data& data) const
            {
                T result{};
                while (is_true(tree.evaluate(data.condition))) result = tree.evaluate(data.body);
                return result;
            }

            T operator()(const while_rtc_data& data) const
            {
                T result{};
                data.checker->reset();
                while (is_true(tree.evaluate(data.condition)) && data.checker->check())
                    result = tree.evaluate(data.body);
                return result;
            }

            T operator()(const repeat_until_data& data) const
            {
                T result{};
                do
                {
                    result = tree.evaluate(data.body);
                } while (is_false(tree.evaluate(data.condition)));
                return result;
            }

            T operator()(const repeat_until_rtc_data& data) const
            {
                T result{};
                data.checker->reset(1);
                do
                {
                    result = tree.evaluate(data.body);
                } while (is_false(tree.evaluate(data.condition)) && data.checker->check());
                return result;
            }

            T operator()(const for_data& data) const
            {
                T result{};
                if (data.has_initialiser)
                    tree.evaluate(data.initialiser);
                if (data.has_incrementor)
                {
                    while (is_true(tree.evaluate(data.condition)))
                    {
                        result = tree.evaluate(data.body);
                        tree.evaluate(data.incrementor);
                    }
                }
                else
                {
                    while (is_true(tree.evaluate(data.condition)))
                        result = tree.evaluate(data.body);
                }
                return result;
            }

            T operator()(const for_rtc_data& data) const
            {
                T result{};
                data.checker->reset();
                if (data.has_initialiser)
                    tree.evaluate(data.initialiser);
                if (data.has_incrementor)
                {
                    while (is_true(tree.evaluate(data.condition)) && data.checker->check())
                    {
                        result = tree.evaluate(data.body);
                        tree.evaluate(data.incrementor);
                    }
                }
                else
                {
                    while (is_true(tree.evaluate(data.condition)) && data.checker->check())
                        result = tree.evaluate(data.body);
                }
                return result;
            }

            T operator()(const switch_data& data) const
            {
                for (const auto& [cond, cons] : data.cases)
                {
                    if (is_true(tree.evaluate(cond)))
                        return tree.evaluate(cons);
                }
                return tree.evaluate(data.default_case);
            }

            T operator()(const multi_switch_data& data) const
            {
                T result{};
                for (const auto& [cond, cons] : data.cases)
                {
                    if (is_true(tree.evaluate(cond)))
                        result = tree.evaluate(cons);
                }
                return result;
            }

            T operator()(const assign_vecvec_data& data) const
            {
                tree.evaluate(data.branch1_child);
                const std::size_t sz = std::min(data.holder0->size(), data.holder1->size());
                T* vec0 = data.vec0_base;
                const T* vec1 = data.vec1_base;
                core::operators::loop_unroll lud(sz);
                const T* upper_bound = vec0 + lud.upper_bound;
                while (vec0 < upper_bound)
                {
                    lud.foreach_batch([&vec0, &vec1](unsigned int i) { vec0[i] = vec1[i]; });
                    vec0 += lud.loop_batch_size;
                    vec1 += lud.loop_batch_size;
                }
                lud.foreach_remainder([&vec0, &vec1]() { *vec0++ = *vec1++; });
                return data.vec0_base[0];
            }

            T operator()(const assign_vecvec_op_data& data) const
            {
                tree.evaluate(data.branch0_child);
                tree.evaluate(data.branch1_child);
                const std::size_t sz = std::min(data.holder0->size(), data.holder1->size());
                T* vec0 = data.vec0_base;
                const T* vec1 = data.vec1_base;
                core::operators::loop_unroll lud(sz);
                const T* upper_bound = vec0 + lud.upper_bound;
                while (vec0 < upper_bound)
                {
                    lud.foreach_batch(
                        [&](unsigned int i)
                        { vec0[i] = core::operators::process<T>(data.read_op, vec0[i], vec1[i]); });
                    vec0 += lud.loop_batch_size;
                    vec1 += lud.loop_batch_size;
                }
                lud.foreach_remainder(
                    [&]()
                    {
                        *vec0 = core::operators::process<T>(data.read_op, *vec0, *vec1);
                        ++vec0;
                        ++vec1;
                    });
                return data.vec0_base[0];
            }

            T operator()(const vecinit_zero_data& data) const
            {
                core::numeric::set_zero_value(data.vec_base, data.vec_size);
                return *data.vec_base;
            }

            T operator()(const vecinit_constfill_data& data) const
            {
                for (std::size_t i = 0; i < data.vec_size; ++i)
                    *(data.vec_base + i) = data.fill_value;
                return *data.vec_base;
            }

            T operator()(const vecinit_dynfill_data& data) const
            {
                const T v = tree.evaluate(data.init_child);
                for (std::size_t i = 0; i < data.vec_size; ++i) *(data.vec_base + i) = v;
                return *data.vec_base;
            }

            T operator()(const vecinit_iota_cc_data& data) const
            {
                T v = data.base_val;
                for (std::size_t i = 0; i < data.vec_size; ++i, v += data.increment_val)
                    *(data.vec_base + i) = v;
                return *data.vec_base;
            }

            T operator()(const vecinit_iota_cnc_data& data) const
            {
                T v = data.base_val;
                for (std::size_t i = 0; i < data.vec_size;
                     ++i, v += tree.evaluate(data.increment_child))
                    *(data.vec_base + i) = v;
                return *data.vec_base;
            }

            T operator()(const vecinit_iota_ncc_data& data) const
            {
                T v = tree.evaluate(data.base_child);
                for (std::size_t i = 0; i < data.vec_size; ++i, v += data.increment_val)
                    *(data.vec_base + i) = v;
                return *data.vec_base;
            }

            T operator()(const vecinit_iota_ncnc_data& data) const
            {
                T v = tree.evaluate(data.base_child);
                for (std::size_t i = 0; i < data.vec_size;
                     ++i, v += tree.evaluate(data.increment_child))
                    *(data.vec_base + i) = v;
                return *data.vec_base;
            }

            T operator()(const vecinit_general_data& data) const
            {
                if (data.single_value)
                {
                    const T v =
                        data.init_children.empty() ? T(0) : tree.evaluate(data.init_children[0]);
                    for (std::size_t i = 0; i < data.vec_size; ++i) *(data.vec_base + i) = v;
                }
                else
                {
                    const std::size_t n = data.init_children.size();
                    for (std::size_t i = 0; i < n; ++i)
                        *(data.vec_base + i) = tree.evaluate(data.init_children[i]);
                    if (n < data.vec_size)
                        core::numeric::set_zero_value(data.vec_base + n, data.vec_size - n);
                }
                return *data.vec_base;
            }

            T operator()(const vararg_evaluable_data& data) const
            {
                for (std::size_t i = 0; i < data.children.size(); ++i)
                    data.temp_values[i] = tree.evaluate(data.children[i]);
                return data.fn->process_values(data.temp_values);
            }

            T operator()(const vec_binop_vecvec_data& data) const
            {
                tree.evaluate(data.branch0_child);
                tree.evaluate(data.branch1_child);
                const T* vec0 = data.vec0_src;
                const T* vec1 = data.vec1_src;
                T* vec_out = data.vec_out;
                core::operators::loop_unroll lud(data.vec_size);
                const T* upper_bound = vec0 + lud.upper_bound;
                while (vec0 < upper_bound)
                {
                    lud.foreach_batch(
                        [&](unsigned int i)
                        {
                            vec_out[i] =
                                core::operators::process<T>(data.operation, vec0[i], vec1[i]);
                        });
                    vec0 += lud.loop_batch_size;
                    vec1 += lud.loop_batch_size;
                    vec_out += lud.loop_batch_size;
                }
                lud.foreach_remainder(
                    [&]()
                    {
                        *vec_out = core::operators::process<T>(data.operation, *vec0, *vec1);
                        ++vec0;
                        ++vec1;
                        ++vec_out;
                    });
                return data.vec_out[0];
            }

            T operator()(const vec_binop_vecval_data& data) const
            {
                tree.evaluate(data.branch0_child);
                const T scalar = tree.evaluate(data.branch1_child);
                const T* vec0 = data.vec0_src;
                T* vec_out = data.vec_out;
                core::operators::loop_unroll lud(data.vec_size);
                const T* upper_bound = vec0 + lud.upper_bound;
                while (vec0 < upper_bound)
                {
                    lud.foreach_batch(
                        [&](unsigned int i)
                        {
                            vec_out[i] =
                                core::operators::process<T>(data.operation, vec0[i], scalar);
                        });
                    vec0 += lud.loop_batch_size;
                    vec_out += lud.loop_batch_size;
                }
                lud.foreach_remainder(
                    [&]()
                    {
                        *vec_out = core::operators::process<T>(data.operation, *vec0, scalar);
                        ++vec0;
                        ++vec_out;
                    });
                return data.vec_out[0];
            }

            T operator()(const vec_binop_valvec_data& data) const
            {
                const T scalar = tree.evaluate(data.branch0_child);
                tree.evaluate(data.branch1_child);
                const T* vec1 = data.vec1_src;
                T* vec_out = data.vec_out;
                core::operators::loop_unroll lud(data.vec_size);
                const T* upper_bound = vec1 + lud.upper_bound;
                while (vec1 < upper_bound)
                {
                    lud.foreach_batch(
                        [&](unsigned int i)
                        {
                            vec_out[i] =
                                core::operators::process<T>(data.operation, scalar, vec1[i]);
                        });
                    vec1 += lud.loop_batch_size;
                    vec_out += lud.loop_batch_size;
                }
                lud.foreach_remainder(
                    [&]()
                    {
                        *vec_out = core::operators::process<T>(data.operation, scalar, *vec1);
                        ++vec1;
                        ++vec_out;
                    });
                return data.vec_out[0];
            }

            T operator()(const unary_vec_data& data) const
            {
                tree.evaluate(data.branch0_child);
                const T* vec0 = data.vec0_src;
                T* vec_out = data.vec_out;
                core::operators::loop_unroll lud(data.vec_size);
                const T* upper_bound = vec0 + lud.upper_bound;
                while (vec0 < upper_bound)
                {
                    lud.foreach_batch(
                        [&](unsigned int i)
                        { vec_out[i] = core::operators::process<T>(data.operation, vec0[i]); });
                    vec0 += lud.loop_batch_size;
                    vec_out += lud.loop_batch_size;
                }
                lud.foreach_remainder(
                    [&]()
                    {
                        *vec_out = core::operators::process<T>(data.operation, *vec0);
                        ++vec0;
                        ++vec_out;
                    });
                return data.vec_out[0];
            }

            T operator()(const vecfunc_data& data) const
            {
                tree.evaluate(data.branch_child);
                return data.proc_fn(data.ivec);
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
                else if constexpr (std::is_same_v<
                                       view_t,
                                       typename node_variant_adapter_t::rbvec_elem_rtc_hot_view>)
                {
                    const auto vec_child = append_child(view.rtc->vec_branch());
                    const auto idx_child = append_child(view.rtc->index_branch());
                    if (!vec_child.has_value() || !idx_child.has_value())
                    {
                        return std::nullopt;
                    }
                    return emplace(rbvec_elem_rtc_data{view.rtc->holder(), view.rtc->rt_check(),
                                                       *vec_child, *idx_child});
                }
                else if constexpr (std::is_same_v<
                                       view_t,
                                       typename node_variant_adapter_t::rbvec_celem_rtc_hot_view>)
                {
                    const auto vec_child = append_child(view.rtc->vec_branch());
                    if (!vec_child.has_value())
                    {
                        return std::nullopt;
                    }
                    return emplace(rbvec_celem_rtc_data{view.rtc->holder(), view.rtc->vec_data(),
                                                        view.rtc->elem_idx(), view.rtc->rt_check(),
                                                        *vec_child});
                }
                else if constexpr (std::is_same_v<
                                       view_t, typename node_variant_adapter_t::vecsize_hot_view>)
                {
                    return emplace(vecsize_data{view.vecsize->vec_holder()});
                }
                else if constexpr (std::is_same_v<view_t,
                                                  typename node_variant_adapter_t::assign_hot_view>)
                {
                    const auto rhs = append_child(node_variant_adapter_t::branch(view.node, 1));
                    if (!rhs.has_value())
                        return std::nullopt;
                    return emplace(assign_data{&view.assign->var_node()->ref(), *rhs});
                }
                else if constexpr (std::is_same_v<
                                       view_t,
                                       typename node_variant_adapter_t::assign_vec_elem_hot_view>)
                {
                    using elem_node_t = typename node_variant_adapter_t::vector_elem_node_t;
                    const elem_node_t* elem = view.assign->elem_node_ptr();
                    const auto vec_child = append_child(elem->vec_branch());
                    const auto idx_child = append_child(elem->index_branch());
                    const auto rhs = append_child(node_variant_adapter_t::branch(view.node, 1));
                    if (!vec_child.has_value() || !idx_child.has_value() || !rhs.has_value())
                        return std::nullopt;
                    return emplace(
                        assign_vec_elem_data{elem->vec_data(), *vec_child, *idx_child, *rhs});
                }
                else if constexpr (std::is_same_v<
                                       view_t,
                                       typename node_variant_adapter_t::assign_rbvec_elem_hot_view>)
                {
                    using elem_node_t = typename node_variant_adapter_t::rebasevector_elem_node_t;
                    const elem_node_t* elem = view.assign->rbvec_elem_node_ptr();
                    const auto vec_child = append_child(elem->vec_branch());
                    const auto idx_child = append_child(elem->index_branch());
                    const auto rhs = append_child(node_variant_adapter_t::branch(view.node, 1));
                    if (!vec_child.has_value() || !idx_child.has_value() || !rhs.has_value())
                        return std::nullopt;
                    return emplace(
                        assign_rbvec_elem_data{elem->holder(), *vec_child, *idx_child, *rhs});
                }
                else if constexpr (std::is_same_v<view_t, typename node_variant_adapter_t::
                                                              assign_rbvec_celem_hot_view>)
                {
                    using elem_node_t = typename node_variant_adapter_t::rebasevector_celem_node_t;
                    const elem_node_t* elem = view.assign->rbvec_celem_node_ptr();
                    const auto vec_child = append_child(elem->vec_branch());
                    const auto rhs = append_child(node_variant_adapter_t::branch(view.node, 1));
                    if (!vec_child.has_value() || !rhs.has_value())
                        return std::nullopt;
                    return emplace(assign_rbvec_celem_data{elem->holder(), elem->elem_idx(),
                                                           *vec_child, *rhs});
                }
                else if constexpr (std::is_same_v<
                                       view_t, typename node_variant_adapter_t::assign_op_hot_view>)
                {
                    const auto rhs = append_child(node_variant_adapter_t::branch(view.node, 1));
                    if (!rhs.has_value())
                        return std::nullopt;
                    return emplace(assign_op_data{&view.var->ref(), view.read_op, *rhs});
                }
                else if constexpr (std::is_same_v<view_t, typename node_variant_adapter_t::
                                                              assign_vec_elem_op_hot_view>)
                {
                    const auto vec_child = append_child(view.elem->vec_branch());
                    const auto idx_child = append_child(view.elem->index_branch());
                    const auto rhs = append_child(node_variant_adapter_t::branch(view.node, 1));
                    if (!vec_child.has_value() || !idx_child.has_value() || !rhs.has_value())
                        return std::nullopt;
                    return emplace(assign_vec_elem_op_data{view.elem->vec_data(), view.read_op,
                                                           *vec_child, *idx_child, *rhs});
                }
                else if constexpr (std::is_same_v<view_t, typename node_variant_adapter_t::
                                                              assign_vec_elem_rtc_hot_view>)
                {
                    using elem_node_t = typename node_variant_adapter_t::vector_elem_rtc_node_t;
                    const elem_node_t* elem = view.assign->elem_rtc_node_ptr();
                    const auto vec_child = append_child(elem->vec_branch());
                    const auto idx_child = append_child(elem->index_branch());
                    const auto rhs = append_child(node_variant_adapter_t::branch(view.node, 1));
                    if (!vec_child.has_value() || !idx_child.has_value() || !rhs.has_value())
                        return std::nullopt;
                    return emplace(assign_vec_elem_rtc_data{elem->holder(), elem->vec_data(),
                                                            elem->max_idx(), elem->rt_check(),
                                                            *vec_child, *idx_child, *rhs});
                }
                else if constexpr (std::is_same_v<view_t, typename node_variant_adapter_t::
                                                              assign_rbvec_elem_rtc_hot_view>)
                {
                    using elem_node_t =
                        typename node_variant_adapter_t::rebasevector_elem_rtc_node_t;
                    const elem_node_t* elem = view.assign->rbvec_elem_rtc_node_ptr();
                    const auto vec_child = append_child(elem->vec_branch());
                    const auto idx_child = append_child(elem->index_branch());
                    const auto rhs = append_child(node_variant_adapter_t::branch(view.node, 1));
                    if (!vec_child.has_value() || !idx_child.has_value() || !rhs.has_value())
                        return std::nullopt;
                    return emplace(assign_rbvec_elem_rtc_data{elem->holder(), elem->rt_check(),
                                                              *vec_child, *idx_child, *rhs});
                }
                else if constexpr (std::is_same_v<view_t, typename node_variant_adapter_t::
                                                              assign_rbvec_elem_op_hot_view>)
                {
                    const auto vec_child = append_child(view.elem->vec_branch());
                    const auto idx_child = append_child(view.elem->index_branch());
                    const auto rhs = append_child(node_variant_adapter_t::branch(view.node, 1));
                    if (!vec_child.has_value() || !idx_child.has_value() || !rhs.has_value())
                        return std::nullopt;
                    return emplace(assign_rbvec_elem_op_data{view.elem->holder(), view.read_op,
                                                             *vec_child, *idx_child, *rhs});
                }
                else if constexpr (std::is_same_v<view_t, typename node_variant_adapter_t::
                                                              assign_rbvec_celem_op_hot_view>)
                {
                    const auto vec_child = append_child(view.elem->vec_branch());
                    const auto rhs = append_child(node_variant_adapter_t::branch(view.node, 1));
                    if (!vec_child.has_value() || !rhs.has_value())
                        return std::nullopt;
                    return emplace(assign_rbvec_celem_op_data{view.elem->holder(),
                                                              view.elem->elem_idx(), view.read_op,
                                                              *vec_child, *rhs});
                }
                else if constexpr (std::is_same_v<view_t, typename node_variant_adapter_t::
                                                              assign_vec_elem_op_rtc_hot_view>)
                {
                    const auto vec_child = append_child(view.elem->vec_branch());
                    const auto idx_child = append_child(view.elem->index_branch());
                    const auto rhs = append_child(node_variant_adapter_t::branch(view.node, 1));
                    if (!vec_child.has_value() || !idx_child.has_value() || !rhs.has_value())
                        return std::nullopt;
                    return emplace(assign_vec_elem_op_rtc_data{
                        view.elem->holder(), view.elem->vec_data(), view.elem->max_idx(),
                        view.elem->rt_check(), view.read_op, *vec_child, *idx_child, *rhs});
                }
                else if constexpr (std::is_same_v<view_t, typename node_variant_adapter_t::
                                                              assign_vec_celem_op_rtc_hot_view>)
                {
                    const auto vec_child = append_child(view.elem->vec_branch());
                    const auto rhs = append_child(node_variant_adapter_t::branch(view.node, 1));
                    if (!vec_child.has_value() || !rhs.has_value())
                        return std::nullopt;
                    return emplace(assign_vec_celem_op_rtc_data{
                        view.elem->holder(), view.elem->vec_data(), view.elem->elem_idx(),
                        view.elem->max_idx(), view.elem->rt_check(), view.read_op, *vec_child,
                        *rhs});
                }
                else if constexpr (std::is_same_v<view_t, typename node_variant_adapter_t::
                                                              assign_rbvec_elem_op_rtc_hot_view>)
                {
                    const auto vec_child = append_child(view.elem->vec_branch());
                    const auto idx_child = append_child(view.elem->index_branch());
                    const auto rhs = append_child(node_variant_adapter_t::branch(view.node, 1));
                    if (!vec_child.has_value() || !idx_child.has_value() || !rhs.has_value())
                        return std::nullopt;
                    return emplace(
                        assign_rbvec_elem_op_rtc_data{view.elem->holder(), view.elem->rt_check(),
                                                      view.read_op, *vec_child, *idx_child, *rhs});
                }
                else if constexpr (std::is_same_v<view_t, typename node_variant_adapter_t::
                                                              assign_rbvec_celem_op_rtc_hot_view>)
                {
                    const auto vec_child = append_child(view.elem->vec_branch());
                    const auto rhs = append_child(node_variant_adapter_t::branch(view.node, 1));
                    if (!vec_child.has_value() || !rhs.has_value())
                        return std::nullopt;
                    return emplace(assign_rbvec_celem_op_rtc_data{
                        view.elem->holder(), view.elem->vec_data(), view.elem->elem_idx(),
                        view.elem->rt_check(), view.read_op, *vec_child, *rhs});
                }
                else if constexpr (std::is_same_v<
                                       view_t,
                                       typename node_variant_adapter_t::assign_vec_scalar_hot_view>)
                {
                    const auto rhs = append_child(node_variant_adapter_t::branch(view.node, 1));
                    if (!rhs.has_value())
                        return std::nullopt;
                    return emplace(assign_vec_scalar_data{view.assign->vds().data(),
                                                          view.assign->vec_holder_ptr(), *rhs});
                }
                else if constexpr (std::is_same_v<view_t, typename node_variant_adapter_t::
                                                              assign_vec_scalar_op_hot_view>)
                {
                    auto* vi = view.node->as_vector_iface();
                    const auto rhs = append_child(node_variant_adapter_t::branch(view.node, 1));
                    if (!rhs.has_value())
                        return std::nullopt;
                    return emplace(assign_vec_scalar_op_data{vi, view.read_op, *rhs});
                }
                else if constexpr (std::is_same_v<view_t,
                                                  typename node_variant_adapter_t::while_hot_view>)
                {
                    const auto condition = append_child(view.loop->condition_branch());
                    const auto body = append_child(view.loop->body_branch());
                    if (!condition.has_value() || !body.has_value())
                        return std::nullopt;
                    return emplace(while_data{*condition, *body});
                }
                else if constexpr (std::is_same_v<
                                       view_t, typename node_variant_adapter_t::while_rtc_hot_view>)
                {
                    const auto condition = append_child(view.loop->condition_branch());
                    const auto body = append_child(view.loop->body_branch());
                    if (!condition.has_value() || !body.has_value())
                        return std::nullopt;
                    return emplace(while_rtc_data{view.loop->rt_checker(), *condition, *body});
                }
                else if constexpr (std::is_same_v<
                                       view_t,
                                       typename node_variant_adapter_t::repeat_until_hot_view>)
                {
                    const auto condition = append_child(view.loop->condition_branch());
                    const auto body = append_child(view.loop->body_branch());
                    if (!condition.has_value() || !body.has_value())
                        return std::nullopt;
                    return emplace(repeat_until_data{*condition, *body});
                }
                else if constexpr (std::is_same_v<
                                       view_t,
                                       typename node_variant_adapter_t::repeat_until_rtc_hot_view>)
                {
                    const auto condition = append_child(view.loop->condition_branch());
                    const auto body = append_child(view.loop->body_branch());
                    if (!condition.has_value() || !body.has_value())
                        return std::nullopt;
                    return emplace(
                        repeat_until_rtc_data{view.loop->rt_checker(), *condition, *body});
                }
                else if constexpr (std::is_same_v<view_t,
                                                  typename node_variant_adapter_t::for_hot_view>)
                {
                    const auto condition = append_child(view.loop->condition_branch());
                    const auto body = append_child(view.loop->body_branch());
                    if (!condition.has_value() || !body.has_value())
                        return std::nullopt;
                    for_data data{};
                    data.condition = *condition;
                    data.body = *body;
                    data.has_initialiser = (view.loop->initialiser_branch() != nullptr);
                    data.has_incrementor = (view.loop->incrementor_branch() != nullptr);
                    if (data.has_initialiser)
                    {
                        const auto init = append_child(view.loop->initialiser_branch());
                        if (!init.has_value())
                            return std::nullopt;
                        data.initialiser = *init;
                    }
                    if (data.has_incrementor)
                    {
                        const auto incr = append_child(view.loop->incrementor_branch());
                        if (!incr.has_value())
                            return std::nullopt;
                        data.incrementor = *incr;
                    }
                    return emplace(data);
                }
                else if constexpr (std::is_same_v<
                                       view_t, typename node_variant_adapter_t::for_rtc_hot_view>)
                {
                    const auto condition = append_child(view.loop->condition_branch());
                    const auto body = append_child(view.loop->body_branch());
                    if (!condition.has_value() || !body.has_value())
                        return std::nullopt;
                    for_rtc_data data{};
                    data.checker = view.loop->rt_checker();
                    data.condition = *condition;
                    data.body = *body;
                    data.has_initialiser = (view.loop->initialiser_branch() != nullptr);
                    data.has_incrementor = (view.loop->incrementor_branch() != nullptr);
                    if (data.has_initialiser)
                    {
                        const auto init = append_child(view.loop->initialiser_branch());
                        if (!init.has_value())
                            return std::nullopt;
                        data.initialiser = *init;
                    }
                    if (data.has_incrementor)
                    {
                        const auto incr = append_child(view.loop->incrementor_branch());
                        if (!incr.has_value())
                            return std::nullopt;
                        data.incrementor = *incr;
                    }
                    return emplace(data);
                }
                else if constexpr (std::is_same_v<view_t,
                                                  typename node_variant_adapter_t::switch_hot_view>)
                {
                    const auto& args = view.sw->arg_list();
                    const std::size_t n_pairs = (args.size() - 1) / 2;
                    switch_data data{};
                    data.cases.reserve(n_pairs);
                    for (std::size_t i = 0; i < n_pairs * 2; i += 2)
                    {
                        const auto cond = append_child(args[i].first);
                        const auto cons = append_child(args[i + 1].first);
                        if (!cond.has_value() || !cons.has_value())
                            return std::nullopt;
                        data.cases.push_back({*cond, *cons});
                    }
                    const auto def = append_child(args.back().first);
                    if (!def.has_value())
                        return std::nullopt;
                    data.default_case = *def;
                    return emplace(std::move(data));
                }
                else if constexpr (std::is_same_v<
                                       view_t,
                                       typename node_variant_adapter_t::multi_switch_hot_view>)
                {
                    const auto& args = view.sw->arg_list();
                    multi_switch_data data{};
                    data.cases.reserve(args.size() / 2);
                    for (std::size_t i = 0; i + 1 < args.size(); i += 2)
                    {
                        const auto cond = append_child(args[i].first);
                        const auto cons = append_child(args[i + 1].first);
                        if (!cond.has_value() || !cons.has_value())
                            return std::nullopt;
                        data.cases.push_back({*cond, *cons});
                    }
                    return emplace(std::move(data));
                }
                else if constexpr (std::is_same_v<
                                       view_t,
                                       typename node_variant_adapter_t::assign_vecvec_hot_view>)
                {
                    const auto branch1 = append_child(node_variant_adapter_t::branch(view.node, 1));
                    if (!branch1.has_value())
                        return std::nullopt;
                    auto* vec0 = view.assign->vec0_ptr();
                    auto* vec1 = view.assign->vec1_ptr();
                    return emplace(assign_vecvec_data{vec0->vds().data(), vec1->vds().data(),
                                                      &vec0->vec_holder(), &vec1->vec_holder(),
                                                      *branch1});
                }
                else if constexpr (std::is_same_v<
                                       view_t,
                                       typename node_variant_adapter_t::assign_vecvec_op_hot_view>)
                {
                    auto* vi0 = view.node->as_vector_iface();
                    auto* vec0_node = vi0->vec();
                    auto* vi1 = node_variant_adapter_t::branch(view.node, 1)->as_vector_iface();
                    auto* vec1_node = vi1->vec();
                    const auto branch0 = append_child(node_variant_adapter_t::branch(view.node, 0));
                    const auto branch1 = append_child(node_variant_adapter_t::branch(view.node, 1));
                    if (!branch0.has_value() || !branch1.has_value())
                        return std::nullopt;
                    return emplace(assign_vecvec_op_data{
                        view.read_op, vec0_node->vds().data(), vec1_node->vds().data(),
                        &vec0_node->vec_holder(), &vec1_node->vec_holder(), *branch0, *branch1});
                }
                else if constexpr (std::is_same_v<
                                       view_t,
                                       typename node_variant_adapter_t::vecinit_zero_hot_view>)
                {
                    return emplace(vecinit_zero_data{view.vec_base, view.vec_size});
                }
                else if constexpr (std::is_same_v<
                                       view_t,
                                       typename node_variant_adapter_t::vecinit_constfill_hot_view>)
                {
                    return emplace(
                        vecinit_constfill_data{view.vec_base, view.vec_size, view.fill_value});
                }
                else if constexpr (std::is_same_v<
                                       view_t,
                                       typename node_variant_adapter_t::vecinit_dynfill_hot_view>)
                {
                    const auto child = append_child(view.init_child);
                    if (!child.has_value())
                        return std::nullopt;
                    return emplace(vecinit_dynfill_data{view.vec_base, view.vec_size, *child});
                }
                else if constexpr (std::is_same_v<
                                       view_t,
                                       typename node_variant_adapter_t::vecinit_iota_cc_hot_view>)
                {
                    return emplace(vecinit_iota_cc_data{view.vec_base, view.vec_size, view.base_val,
                                                        view.increment_val});
                }
                else if constexpr (std::is_same_v<
                                       view_t,
                                       typename node_variant_adapter_t::vecinit_iota_cnc_hot_view>)
                {
                    const auto incr = append_child(view.increment_child);
                    if (!incr.has_value())
                        return std::nullopt;
                    return emplace(
                        vecinit_iota_cnc_data{view.vec_base, view.vec_size, view.base_val, *incr});
                }
                else if constexpr (std::is_same_v<
                                       view_t,
                                       typename node_variant_adapter_t::vecinit_iota_ncc_hot_view>)
                {
                    const auto base = append_child(view.base_child);
                    if (!base.has_value())
                        return std::nullopt;
                    const T baked_incr = view.increment_child->value();
                    return emplace(
                        vecinit_iota_ncc_data{view.vec_base, view.vec_size, baked_incr, *base});
                }
                else if constexpr (std::is_same_v<
                                       view_t,
                                       typename node_variant_adapter_t::vecinit_iota_ncnc_hot_view>)
                {
                    const auto base = append_child(view.base_child);
                    const auto incr = append_child(view.increment_child);
                    if (!base.has_value() || !incr.has_value())
                        return std::nullopt;
                    return emplace(
                        vecinit_iota_ncnc_data{view.vec_base, view.vec_size, *base, *incr});
                }
                else if constexpr (std::is_same_v<
                                       view_t,
                                       typename node_variant_adapter_t::vecinit_general_hot_view>)
                {
                    auto* n = view.vinit;
                    const bool single_value = n->is_single_value();
                    vecinit_general_data data{n->vec_base(), n->vec_size(), {}, single_value};
                    const auto& list = n->initialiser_list();
                    if (single_value)
                    {
                        if (!list.empty())
                        {
                            const bool is_zero =
                                details::is_constant_node(list[0]) && T(0) == list[0]->value();
                            if (!is_zero)
                            {
                                const auto child = append_child(list[0]);
                                if (!child.has_value())
                                    return std::nullopt;
                                data.init_children.push_back(*child);
                            }
                        }
                    }
                    else
                    {
                        data.init_children.reserve(list.size());
                        for (auto* child : list)
                        {
                            const auto idx = append_child(child);
                            if (!idx.has_value())
                                return std::nullopt;
                            data.init_children.push_back(*idx);
                        }
                    }
                    return emplace(std::move(data));
                }
                else if constexpr (std::is_same_v<
                                       view_t,
                                       typename node_variant_adapter_t::vararg_evaluable_hot_view>)
                {
                    const std::size_t n = view.fn->arg_count();
                    vararg_evaluable_data data{view.fn, std::vector<T>(n, T{}), {}};
                    data.children.reserve(n);
                    for (std::size_t i = 0; i < n; ++i)
                    {
                        const auto child = append_child(view.fn->arg_at(i));
                        if (!child.has_value())
                            return std::nullopt;
                        data.children.push_back(*child);
                    }
                    return emplace(std::move(data));
                }
                else if constexpr (std::is_same_v<
                                       view_t,
                                       typename node_variant_adapter_t::vec_binop_vecvec_hot_view>)
                {
                    auto* vi = view.node->as_vector_iface();
                    auto* vi0 = node_variant_adapter_t::branch(view.node, 0)->as_vector_iface();
                    auto* vi1 = node_variant_adapter_t::branch(view.node, 1)->as_vector_iface();
                    const auto branch0 = append_child(node_variant_adapter_t::branch(view.node, 0));
                    const auto branch1 = append_child(node_variant_adapter_t::branch(view.node, 1));
                    if (!branch0.has_value() || !branch1.has_value())
                        return std::nullopt;
                    const std::size_t sz = std::min(vi0->size(), vi1->size());
                    return emplace(vec_binop_vecvec_data{view.operation, vi0->vds().data(),
                                                         vi1->vds().data(), vi->vds().data(), sz,
                                                         *branch0, *branch1});
                }
                else if constexpr (std::is_same_v<
                                       view_t,
                                       typename node_variant_adapter_t::vec_binop_vecval_hot_view>)
                {
                    auto* vi = view.node->as_vector_iface();
                    auto* vi0 = node_variant_adapter_t::branch(view.node, 0)->as_vector_iface();
                    const auto branch0 = append_child(node_variant_adapter_t::branch(view.node, 0));
                    const auto branch1 = append_child(node_variant_adapter_t::branch(view.node, 1));
                    if (!branch0.has_value() || !branch1.has_value())
                        return std::nullopt;
                    return emplace(vec_binop_vecval_data{view.operation, vi0->vds().data(),
                                                         vi->vds().data(), vi0->size(), *branch0,
                                                         *branch1});
                }
                else if constexpr (std::is_same_v<
                                       view_t,
                                       typename node_variant_adapter_t::vec_binop_valvec_hot_view>)
                {
                    auto* vi = view.node->as_vector_iface();
                    auto* vi1 = node_variant_adapter_t::branch(view.node, 1)->as_vector_iface();
                    const auto branch0 = append_child(node_variant_adapter_t::branch(view.node, 0));
                    const auto branch1 = append_child(node_variant_adapter_t::branch(view.node, 1));
                    if (!branch0.has_value() || !branch1.has_value())
                        return std::nullopt;
                    return emplace(vec_binop_valvec_data{view.operation, vi1->vds().data(),
                                                         vi->vds().data(), vi1->size(), *branch0,
                                                         *branch1});
                }
                else if constexpr (std::is_same_v<
                                       view_t, typename node_variant_adapter_t::unary_vec_hot_view>)
                {
                    auto* vi = view.node->as_vector_iface();
                    auto* vi0 = node_variant_adapter_t::branch(view.node, 0)->as_vector_iface();
                    const auto branch0 = append_child(node_variant_adapter_t::branch(view.node, 0));
                    if (!branch0.has_value())
                        return std::nullopt;
                    return emplace(unary_vec_data{view.operation, vi0->vds().data(),
                                                  vi->vds().data(), vi0->size(), *branch0});
                }
                else if constexpr (std::is_same_v<
                                       view_t, typename node_variant_adapter_t::vecfunc_hot_view>)
                {
                    const auto branch_child = append_child(view.fn->vec_branch());
                    if (!branch_child.has_value())
                        return std::nullopt;
                    return emplace(
                        vecfunc_data{view.fn->proc_fn(), view.fn->ivec(), *branch_child});
                }
                else if constexpr (std::is_same_v<view_t,
                                                  typename node_variant_adapter_t::fallback_view>)
                {
                    if (view.node->type() == expression_node<T>::node_type::e_null)
                    {
                        return emplace(literal_data{std::numeric_limits<T>::quiet_NaN()});
                    }
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
