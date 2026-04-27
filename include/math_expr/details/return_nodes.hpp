      class return_exception {};

      template <typename T>
      class null_igenfunc
      {
      public:

         virtual ~null_igenfunc()
         {}

         typedef type_store<T> generic_type;
         typedef typename generic_type::parameter_list parameter_list_t;

         inline virtual T operator() (parameter_list_t)
         {
            return std::numeric_limits<T>::quiet_NaN();
         }
      };

      #ifndef math_expr_disable_return_statement
      template <typename T>
      class return_node final : public generic_function_node<T,null_igenfunc<T> >
      {
      public:

         typedef results_context<T>   results_context_t;
         typedef null_igenfunc<T>     igeneric_function_t;
         typedef igeneric_function_t* igeneric_function_ptr;
         typedef generic_function_node<T,igeneric_function_t> gen_function_t;

         return_node(const std::vector<typename gen_function_t::expression_ptr>& arg_list,
                     results_context_t& rc)
         : gen_function_t  (arg_list)
         , results_context_(&rc)
         {
            assert(valid());
         }

         inline T value() const override
         {
            if (gen_function_t::populate_value_list())
            {
               prepare_typestore_list();

               typedef typename type_store<T>::parameter_list parameter_list_t;

               results_context_->
                  assign(parameter_list_t(gen_function_t::typestore_list_));

               throw return_exception();
            }

            return std::numeric_limits<T>::quiet_NaN();
         }

         inline typename expression_node<T>::node_type type() const override
         {
            return expression_node<T>::e_return;
         }

         inline bool valid() const override
         {
            return results_context_;
         }

      private:

         void prepare_typestore_list() const
         {
            for (std::size_t i = 0; i < gen_function_t::typestore_list_.size(); ++i)
            {
               typename gen_function_t::type_store_t& ts = gen_function_t::typestore_list_[i];

               if (ts.ivec)
               {
                  ts.size = ts.ivec->size();
               }
            }
         }

         results_context_t* results_context_;
      };

      template <typename T>
      class return_envelope_node final : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef results_context<T>  results_context_t;
         typedef std::pair<expression_ptr,bool> branch_t;

         return_envelope_node(expression_ptr body, results_context_t& rc)
         : results_context_(&rc  )
         , return_invoked_ (false)
         {
            construct_branch_pair(body_, body);
            assert(valid());
         }

         inline T value() const override
         {
            try
            {
               return_invoked_ = false;
               results_context_->clear();

               return body_.first->value();
            }
            catch(const return_exception&)
            {
               return_invoked_ = true;

               return std::numeric_limits<T>::quiet_NaN();
            }
         }

         inline typename expression_node<T>::node_type type() const override
         {
            return expression_node<T>::e_retenv;
         }

         inline bool valid() const override
         {
            return results_context_ && body_.first;
         }

         inline bool* retinvk_ptr()
         {
            return &return_invoked_;
         }

         void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) override
         {
            expression_node<T>::ndb_t::collect(body_, node_delete_list);
         }

         std::size_t node_depth() const override
         {
            return expression_node<T>::ndb_t::compute_node_depth(body_);
         }

      private:

         results_context_t* results_context_;
         mutable bool        return_invoked_;
         branch_t                      body_;
      };
      #endif

      #define math_expr_define_unary_op(OpName)                    \
      template <typename T>                                     \
      struct OpName##_op                                        \
      {                                                         \
         typedef typename functor_t<T>::Type Type;              \
         typedef typename expression_node<T>::node_type node_t; \
                                                                \
         static inline T process(Type v)                        \
         {                                                      \
            return numeric:: OpName (v);                        \
         }                                                      \
                                                                \
         static inline node_t type()                            \
         {                                                      \
            return expression_node<T>::e_##OpName;              \
         }                                                      \
                                                                \
         static inline details::operator_type operation()       \
         {                                                      \
            return details::e_##OpName;                         \
         }                                                      \
      };                                                        \

      math_expr_define_unary_op(abs  )
      math_expr_define_unary_op(acos )
      math_expr_define_unary_op(acosh)
      math_expr_define_unary_op(asin )
      math_expr_define_unary_op(asinh)
      math_expr_define_unary_op(atan )
      math_expr_define_unary_op(atanh)
      math_expr_define_unary_op(ceil )
      math_expr_define_unary_op(cos  )
      math_expr_define_unary_op(cosh )
      math_expr_define_unary_op(cot  )
      math_expr_define_unary_op(csc  )
      math_expr_define_unary_op(d2g  )
      math_expr_define_unary_op(d2r  )
      math_expr_define_unary_op(erf  )
      math_expr_define_unary_op(erfc )
      math_expr_define_unary_op(exp  )
      math_expr_define_unary_op(expm1)
      math_expr_define_unary_op(floor)
      math_expr_define_unary_op(frac )
      math_expr_define_unary_op(g2d  )
      math_expr_define_unary_op(log  )
      math_expr_define_unary_op(log10)
      math_expr_define_unary_op(log2 )
      math_expr_define_unary_op(log1p)
      math_expr_define_unary_op(ncdf )
      math_expr_define_unary_op(neg  )
      math_expr_define_unary_op(notl )
      math_expr_define_unary_op(pos  )
      math_expr_define_unary_op(r2d  )
      math_expr_define_unary_op(round)
      math_expr_define_unary_op(sec  )
      math_expr_define_unary_op(sgn  )
      math_expr_define_unary_op(sin  )
      math_expr_define_unary_op(sinc )
      math_expr_define_unary_op(sinh )
      math_expr_define_unary_op(sqrt )
      math_expr_define_unary_op(tan  )
      math_expr_define_unary_op(tanh )
      math_expr_define_unary_op(trunc)
      #undef math_expr_define_unary_op

      template <typename T>
