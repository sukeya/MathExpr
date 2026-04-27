   class function_compositor
   {
   public:

      typedef exprtk::expression<T>             expression_t;
      typedef exprtk::symbol_table<T>           symbol_table_t;
      typedef exprtk::parser<T>                 parser_t;
      typedef typename parser_t::settings_store settings_t;

      struct function
      {
         function()
         {}

         explicit function(const std::string& n)
         : name_(n)
         {}

         function(const std::string& name,
                  const std::string& expression)
         : name_(name)
         , expression_(expression)
         {}

         function(const std::string& name,
                  const std::string& expression,
                  const std::string& v0)
         : name_(name)
         , expression_(expression)
         {
            v_.push_back(v0);
         }

         function(const std::string& name,
                  const std::string& expression,
                  const std::string& v0, const std::string& v1)
         : name_(name)
         , expression_(expression)
         {
            v_.push_back(v0); v_.push_back(v1);
         }

         function(const std::string& name,
                  const std::string& expression,
                  const std::string& v0, const std::string& v1,
                  const std::string& v2)
         : name_(name)
         , expression_(expression)
         {
            v_.push_back(v0); v_.push_back(v1);
            v_.push_back(v2);
         }

         function(const std::string& name,
                  const std::string& expression,
                  const std::string& v0, const std::string& v1,
                  const std::string& v2, const std::string& v3)
         : name_(name)
         , expression_(expression)
         {
            v_.push_back(v0); v_.push_back(v1);
            v_.push_back(v2); v_.push_back(v3);
         }

         function(const std::string& name,
                  const std::string& expression,
                  const std::string& v0, const std::string& v1,
                  const std::string& v2, const std::string& v3,
                  const std::string& v4)
         : name_(name)
         , expression_(expression)
         {
            v_.push_back(v0); v_.push_back(v1);
            v_.push_back(v2); v_.push_back(v3);
            v_.push_back(v4);
         }

         inline function& name(const std::string& n)
         {
            name_ = n;
            return (*this);
         }

         inline function& expression(const std::string& e)
         {
            expression_ = e;
            return (*this);
         }

         inline function& var(const std::string& v)
         {
            v_.push_back(v);
            return (*this);
         }

         inline function& vars(const std::string& v0,
                               const std::string& v1)
         {
            v_.push_back(v0);
            v_.push_back(v1);
            return (*this);
         }

         inline function& vars(const std::string& v0,
                               const std::string& v1,
                               const std::string& v2)
         {
            v_.push_back(v0);
            v_.push_back(v1);
            v_.push_back(v2);
            return (*this);
         }

         inline function& vars(const std::string& v0,
                               const std::string& v1,
                               const std::string& v2,
                               const std::string& v3)
         {
            v_.push_back(v0);
            v_.push_back(v1);
            v_.push_back(v2);
            v_.push_back(v3);
            return (*this);
         }

         inline function& vars(const std::string& v0,
                               const std::string& v1,
                               const std::string& v2,
                               const std::string& v3,
                               const std::string& v4)
         {
            v_.push_back(v0);
            v_.push_back(v1);
            v_.push_back(v2);
            v_.push_back(v3);
            v_.push_back(v4);
            return (*this);
         }

         std::string name_;
         std::string expression_;
         std::deque<std::string> v_;
      };

   private:

      struct base_func : public exprtk::ifunction<T>
      {
         typedef const T&                  type;
         typedef exprtk::ifunction<T>      function_t;
         typedef std::vector<T*>           varref_t;
         typedef std::vector<T>            var_t;
         typedef std::vector<std::string>  str_t;
         typedef std::pair<T*,std::size_t> lvarref_t;
         typedef std::vector<lvarref_t>    lvr_vec_t;
         typedef std::vector<std::string*> lstr_vec_t;

         using exprtk::ifunction<T>::operator();

         explicit base_func(const std::size_t& pc = 0)
         : exprtk::ifunction<T>(pc)
         , local_var_stack_size(0)
         , stack_depth(0)
         {
            v.resize(pc);
         }

         virtual ~base_func()
         {}

         #define exprtk_assign(Index) \
         (*v[Index]) = v##Index;      \

         inline void update(const T& v0)
         {
            exprtk_assign(0)
         }

         inline void update(const T& v0, const T& v1)
         {
            exprtk_assign(0) exprtk_assign(1)
         }

         inline void update(const T& v0, const T& v1, const T& v2)
         {
            exprtk_assign(0) exprtk_assign(1)
            exprtk_assign(2)
         }

         inline void update(const T& v0, const T& v1, const T& v2, const T& v3)
         {
            exprtk_assign(0) exprtk_assign(1)
            exprtk_assign(2) exprtk_assign(3)
         }

         inline void update(const T& v0, const T& v1, const T& v2, const T& v3, const T& v4)
         {
            exprtk_assign(0) exprtk_assign(1)
            exprtk_assign(2) exprtk_assign(3)
            exprtk_assign(4)
         }

         inline void update(const T& v0, const T& v1, const T& v2, const T& v3, const T& v4, const T& v5)
         {
            exprtk_assign(0) exprtk_assign(1)
            exprtk_assign(2) exprtk_assign(3)
            exprtk_assign(4) exprtk_assign(5)
         }

         #ifdef exprtk_assign
         #undef exprtk_assign
         #endif

         inline function_t& setup(expression_t& expr)
         {
            expression = expr;

            typedef typename expression_t::control_block  ctrlblk_t;
            typedef typename ctrlblk_t::local_data_list_t ldl_t;
            typedef typename ctrlblk_t::data_type         data_t;
            typedef typename ldl_t::value_type            ldl_value_type;

            const ldl_t ldl = expr.local_data_list();

            std::vector<std::pair<std::size_t,data_t> > index_list;

            for (std::size_t i = 0; i < ldl.size(); ++i)
            {
               exprtk_debug(("base_func::setup() - element[%02d] type: %s size: %d\n",
                             static_cast<int>(i),
                             expression_t::control_block::to_str(ldl[i].type).c_str(),
                             static_cast<int>(ldl[i].size)));

               switch (ldl[i].type)
               {
                  case ctrlblk_t::e_unknown   : continue;
                  case ctrlblk_t::e_expr      : continue;
                  case ctrlblk_t::e_vecholder : continue;
                  default                     : break;
               }

               if (ldl[i].size)
               {
                  index_list.push_back(std::make_pair(i,ldl[i].type));
               }
            }

            std::size_t input_param_count = 0;

            for (std::size_t i = 0; i < index_list.size(); ++i)
            {
               const std::size_t index         = index_list[i].first;
               const ldl_value_type& local_var = ldl[index];

               assert(local_var.pointer);

               if (i < (index_list.size() - v.size()))
               {
                  if (local_var.type == ctrlblk_t::e_string)
                  {
                     local_str_vars.push_back(
                        reinterpret_cast<std::string*>(local_var.pointer));
                  }
                  else if (
                            (local_var.type == ctrlblk_t::e_data   ) ||
                            (local_var.type == ctrlblk_t::e_vecdata)
                          )
                  {
                     local_vars.push_back(std::make_pair(
                        reinterpret_cast<T*>(local_var.pointer),
                        local_var.size));

                     local_var_stack_size += local_var.size;
                  }
               }
               else
               {
                  v[input_param_count++] = reinterpret_cast<T*>(local_var.pointer);
               }
            }

            clear_stack();

            return (*this);
         }

         inline void pre()
         {
            if (stack_depth++)
            {
               if (!v.empty())
               {
                  var_t var_stack(v.size(),T(0));
                  copy(v,var_stack);
                  input_params_stack.push_back(var_stack);
               }

               if (!local_vars.empty())
               {
                  var_t local_vec_frame(local_var_stack_size,T(0));
                  copy(local_vars,local_vec_frame);
                  local_var_stack.push_back(local_vec_frame);
               }

               if (!local_str_vars.empty())
               {
                  str_t local_str_frame(local_str_vars.size());
                  copy(local_str_vars,local_str_frame);
                  local_str_stack.push_back(local_str_frame);
               }
            }
         }

         inline void post()
         {
            if (--stack_depth)
            {
               if (!v.empty())
               {
                  copy(input_params_stack.back(), v);
                  input_params_stack.pop_back();
               }

               if (!local_vars.empty())
               {
                  copy(local_var_stack.back(), local_vars);
                  local_var_stack.pop_back();
               }

               if (!local_str_vars.empty())
               {
                  copy(local_str_stack.back(), local_str_vars);
                  local_str_stack.pop_back();
               }
            }
         }

         void copy(const varref_t& src_v, var_t& dest_v)
         {
            for (std::size_t i = 0; i < src_v.size(); ++i)
            {
               dest_v[i] = (*src_v[i]);
            }
         }

         void copy(const lstr_vec_t& src_v, str_t& dest_v)
         {
            for (std::size_t i = 0; i < src_v.size(); ++i)
            {
               dest_v[i] = (*src_v[i]);
            }
         }

         void copy(const var_t& src_v, varref_t& dest_v)
         {
            for (std::size_t i = 0; i < src_v.size(); ++i)
            {
               (*dest_v[i]) = src_v[i];
            }
         }

         void copy(const lvr_vec_t& src_v, var_t& dest_v)
         {
            typename var_t::iterator itr = dest_v.begin();
            typedef  typename std::iterator_traits<typename var_t::iterator>::difference_type diff_t;

            for (std::size_t i = 0; i < src_v.size(); ++i)
            {
               lvarref_t vr = src_v[i];

               if (1 == vr.second)
                  *itr++ = (*vr.first);
               else
               {
                  std::copy(vr.first, vr.first + vr.second, itr);
                  itr += static_cast<diff_t>(vr.second);
               }
            }
         }

         void copy(const var_t& src_v, lvr_vec_t& dest_v)
         {
            typename var_t::const_iterator itr = src_v.begin();
            typedef  typename std::iterator_traits<typename var_t::iterator>::difference_type diff_t;

            for (std::size_t i = 0; i < dest_v.size(); ++i)
            {
               lvarref_t& vr = dest_v[i];

               assert(vr.first != 0);
               assert(vr.second > 0);

               if (1 == vr.second)
                  (*vr.first) = *itr++;
               else
               {
                  std::copy(itr, itr + static_cast<diff_t>(vr.second), vr.first);
                  itr += static_cast<diff_t>(vr.second);
               }
            }
         }

         void copy(const str_t& src_str, lstr_vec_t& dest_str)
         {
            assert(src_str.size() == dest_str.size());

            for (std::size_t i = 0; i < dest_str.size(); ++i)
            {
               *dest_str[i] = src_str[i];
            }
         }

         inline void clear_stack()
         {
            for (std::size_t i = 0; i < v.size(); ++i)
            {
               (*v[i]) = 0;
            }
         }

         inline virtual T value(expression_t& e)
         {
            return e.value();
         }

         expression_t      expression;
         varref_t          v;
         lvr_vec_t         local_vars;
         lstr_vec_t        local_str_vars;
         std::size_t       local_var_stack_size;
         std::size_t       stack_depth;
         std::deque<var_t> input_params_stack;
         std::deque<var_t> local_var_stack;
         std::deque<str_t> local_str_stack;
      };

      typedef std::map<std::string,base_func*> funcparam_t;

      typedef const T& type;

      template <typename BaseFuncType>
      struct scoped_bft
      {
         explicit scoped_bft(BaseFuncType& bft)
         : bft_(bft)
         {
            bft_.pre ();
         }

        ~scoped_bft()
         {
            bft_.post();
         }

         BaseFuncType& bft_;

      private:

         scoped_bft(const scoped_bft&) exprtk_delete;
         scoped_bft& operator=(const scoped_bft&) exprtk_delete;
      };

      struct func_0param : public base_func
      {
         using exprtk::ifunction<T>::operator();

         func_0param() : base_func(0) {}

         inline T operator() () exprtk_override
         {
            scoped_bft<func_0param> sb(*this);
            return this->value(base_func::expression);
         }
      };

      struct func_1param : public base_func
      {
         using exprtk::ifunction<T>::operator();

         func_1param() : base_func(1) {}

         inline T operator() (type v0) exprtk_override
         {
            scoped_bft<func_1param> sb(*this);
            base_func::update(v0);
            return this->value(base_func::expression);
         }
      };

      struct func_2param : public base_func
      {
         using exprtk::ifunction<T>::operator();

         func_2param() : base_func(2) {}

         inline T operator() (type v0, type v1) exprtk_override
         {
            scoped_bft<func_2param> sb(*this);
            base_func::update(v0, v1);
            return this->value(base_func::expression);
         }
      };

      struct func_3param : public base_func
      {
         using exprtk::ifunction<T>::operator();

         func_3param() : base_func(3) {}

         inline T operator() (type v0, type v1, type v2) exprtk_override
         {
            scoped_bft<func_3param> sb(*this);
            base_func::update(v0, v1, v2);
            return this->value(base_func::expression);
         }
      };

      struct func_4param : public base_func
      {
         using exprtk::ifunction<T>::operator();

         func_4param() : base_func(4) {}

         inline T operator() (type v0, type v1, type v2, type v3) exprtk_override
         {
            scoped_bft<func_4param> sb(*this);
            base_func::update(v0, v1, v2, v3);
            return this->value(base_func::expression);
         }
      };

      struct func_5param : public base_func
      {
         using exprtk::ifunction<T>::operator();

         func_5param() : base_func(5) {}

         inline T operator() (type v0, type v1, type v2, type v3, type v4) exprtk_override
         {
            scoped_bft<func_5param> sb(*this);
            base_func::update(v0, v1, v2, v3, v4);
            return this->value(base_func::expression);
         }
      };

      struct func_6param : public base_func
      {
         using exprtk::ifunction<T>::operator();

         func_6param() : base_func(6) {}

         inline T operator() (type v0, type v1, type v2, type v3, type v4, type v5) exprtk_override
         {
            scoped_bft<func_6param> sb(*this);
            base_func::update(v0, v1, v2, v3, v4, v5);
            return this->value(base_func::expression);
         }
      };

      static T return_value(expression_t& e)
      {
         typedef exprtk::results_context<T> results_context_t;
         typedef typename results_context_t::type_store_t type_t;
         typedef typename type_t::scalar_view scalar_t;

         const T result = e.value();

         if (e.return_invoked())
         {
            // Due to the post compilation checks, it can be safely
            // assumed that there will be at least one parameter
            // and that the first parameter will always be scalar.
            return scalar_t(e.results()[0])();
         }

         return result;
      }

      #define def_fp_retval(N)                                            \
      struct func_##N##param_retval exprtk_final : public func_##N##param \
      {                                                                   \
         inline T value(expression_t& e) exprtk_override                  \
         {                                                                \
            return return_value(e);                                       \
         }                                                                \
      };                                                                  \

      def_fp_retval(0)
      def_fp_retval(1)
      def_fp_retval(2)
      def_fp_retval(3)
      def_fp_retval(4)
      def_fp_retval(5)
      def_fp_retval(6)

      #undef def_fp_retval

      template <typename Allocator,
                template <typename, typename> class Sequence>
      inline bool add(const std::string& name,
                      const std::string& expression,
                      const Sequence<std::string,Allocator>& var_list,
                      const bool override = false)
      {
         const typename std::map<std::string,expression_t>::iterator itr = expr_map_.find(name);

         if (expr_map_.end() != itr)
         {
            if (!override)
            {
               exprtk_debug(("Compositor error(add): function '%s' already defined\n",
                             name.c_str()));

               return false;
            }

            remove(name, var_list.size());
         }

         if (compile_expression(name, expression, var_list))
         {
            const std::size_t n = var_list.size();

            fp_map_[n][name]->setup(expr_map_[name]);

            return true;
         }
         else
         {
            exprtk_debug(("Compositor error(add): Failed to compile function '%s'\n",
                          name.c_str()));

            return false;
         }
      }

   public:

      function_compositor()
      : parser_(settings_t::default_compile_all_opts +
                settings_t::e_disable_zero_return)
      , fp_map_(7)
      , load_variables_(false)
      , load_vectors_(false)
      {}

      explicit function_compositor(const symbol_table_t& st)
      : symbol_table_(st)
      , parser_(settings_t::default_compile_all_opts +
                settings_t::e_disable_zero_return)
      , fp_map_(7)
      , load_variables_(false)
      , load_vectors_(false)
      {}

     ~function_compositor()
      {
         clear();
      }

      inline symbol_table_t& symbol_table()
      {
         return symbol_table_;
      }

      inline const symbol_table_t& symbol_table() const
      {
         return symbol_table_;
      }

      inline void add_auxiliary_symtab(symbol_table_t& symtab)
      {
         auxiliary_symtab_list_.push_back(&symtab);
      }

      void load_variables(const bool load = true)
      {
         load_variables_ = load;
      }

      void load_vectors(const bool load = true)
      {
         load_vectors_ = load;
      }

      inline void register_loop_runtime_check(loop_runtime_check& lrtchk)
      {
         parser_.register_loop_runtime_check(lrtchk);
      }

      inline void register_vector_access_runtime_check(vector_access_runtime_check& vartchk)
      {
         parser_.register_vector_access_runtime_check(vartchk);
      }

      inline void register_compilation_timeout_check(compilation_check& compchk)
      {
         parser_.register_compilation_timeout_check(compchk);
      }

      inline void clear_loop_runtime_check()
      {
         parser_.clear_loop_runtime_check();
      }

      inline void clear_vector_access_runtime_check()
      {
         parser_.clear_vector_access_runtime_check();
      }

      inline void clear_compilation_timeout_check()
      {
         parser_.clear_compilation_timeout_check();
      }

      void clear()
      {
         symbol_table_.clear();
         expr_map_    .clear();

         for (std::size_t i = 0; i < fp_map_.size(); ++i)
         {
            typename funcparam_t::iterator itr = fp_map_[i].begin();
            typename funcparam_t::iterator end = fp_map_[i].end  ();

            while (itr != end)
            {
               delete itr->second;
               ++itr;
            }

            fp_map_[i].clear();
         }

         clear_loop_runtime_check         ();
         clear_vector_access_runtime_check();
         clear_compilation_timeout_check  ();
      }

      inline bool add(const function& f, const bool override = false)
      {
         return add(f.name_, f.expression_, f.v_,override);
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

      inline parser_error::type get_error(const std::size_t& index) const
      {
         if (index < error_list_.size())
         {
            return error_list_[index];
         }

         throw std::invalid_argument("compositor::get_error() - Invalid error index specified");
      }

   private:

      template <typename Allocator,
                template <typename, typename> class Sequence>
      bool compile_expression(const std::string& name,
                              const std::string& expression,
                              const Sequence<std::string,Allocator>& input_var_list,
                              bool  return_present = false)
      {
         expression_t compiled_expression;
         symbol_table_t local_symbol_table;

         local_symbol_table.load_from(symbol_table_);
         local_symbol_table.add_constants();

         if (load_variables_)
         {
            local_symbol_table.load_variables_from(symbol_table_);
         }

         if (load_vectors_)
         {
            local_symbol_table.load_vectors_from(symbol_table_);
         }

         error_list_.clear();

         if (!valid(name,input_var_list.size()))
         {
            parser_error::type error =
               parser_error::make_error(
                  parser_error::e_parser,
                  lexer::token(),
                  "ERR283 - Function '" + name + "' is an invalid overload",
                  exprtk_error_location);

            error_list_.push_back(error);
            return false;
         }

         if (!forward(name,
                      input_var_list.size(),
                      local_symbol_table,
                      return_present))
            return false;

         compiled_expression.register_symbol_table(local_symbol_table);

         for (std::size_t i = 0; i < auxiliary_symtab_list_.size(); ++i)
         {
            compiled_expression.register_symbol_table((*auxiliary_symtab_list_[i]));
         }

         std::string mod_expression;

         for (std::size_t i = 0; i < input_var_list.size(); ++i)
         {
            mod_expression += " var " + input_var_list[i] + "{};\n";
         }

         if (
              ('{' == details::front(expression)) &&
              ('}' == details::back (expression))
            )
            mod_expression += "~" + expression + ";";
         else
            mod_expression += "~{" + expression + "};";

         if (!parser_.compile(mod_expression,compiled_expression))
         {
            exprtk_debug(("Compositor Error: %s\n", parser_.error().c_str()));
            exprtk_debug(("Compositor modified expression: \n%s\n", mod_expression.c_str()));

            remove(name,input_var_list.size());

            for (std::size_t err_index = 0; err_index < parser_.error_count(); ++err_index)
            {
               error_list_.push_back(parser_.get_error(err_index));
            }

            return false;
         }

         if (!return_present && parser_.dec().return_present())
         {
            remove(name,input_var_list.size());
            return compile_expression(name, expression, input_var_list, true);
         }

         // Make sure every return point has a scalar as its first parameter
         if (parser_.dec().return_present())
         {
            typedef std::vector<std::string> str_list_t;

            str_list_t ret_param_list = parser_.dec().return_param_type_list();

            for (std::size_t i = 0; i < ret_param_list.size(); ++i)
            {
               const std::string& params = ret_param_list[i];

               if (params.empty() || ('T' != params[0]))
               {
                  exprtk_debug(("Compositor Error: Return statement in function '%s' is invalid\n",
                                name.c_str()));

                  remove(name,input_var_list.size());

                  return false;
               }
            }
         }

         expr_map_[name] = compiled_expression;

         exprtk::ifunction<T>& ifunc = (*(fp_map_[input_var_list.size()])[name]);

         if (symbol_table_.add_function(name,ifunc))
            return true;
         else
         {
            exprtk_debug(("Compositor Error: Failed to add function '%s' to symbol table\n",
                          name.c_str()));
            return false;
         }
      }

      inline bool symbol_used(const std::string& symbol) const
      {
         return (
                  symbol_table_.is_variable       (symbol) ||
                  symbol_table_.is_stringvar      (symbol) ||
                  symbol_table_.is_function       (symbol) ||
                  symbol_table_.is_vector         (symbol) ||
                  symbol_table_.is_vararg_function(symbol)
                );
      }

      inline bool valid(const std::string& name,
                        const std::size_t& arg_count) const
      {
         if (arg_count > 6)
            return false;
         else if (symbol_used(name))
            return false;
         else if (fp_map_[arg_count].end() != fp_map_[arg_count].find(name))
            return false;
         else
            return true;
      }

      inline bool forward(const std::string& name,
                          const std::size_t& arg_count,
                          symbol_table_t& sym_table,
                          const bool ret_present = false)
      {
         switch (arg_count)
         {
            #define case_stmt(N)                                     \
            case N : (fp_map_[arg_count])[name] =                    \
                     (!ret_present) ? static_cast<base_func*>        \
                                      (new func_##N##param) :        \
                                      static_cast<base_func*>        \
                                      (new func_##N##param_retval) ; \
                     break;                                          \

            case_stmt(0) case_stmt(1) case_stmt(2)
            case_stmt(3) case_stmt(4) case_stmt(5)
            case_stmt(6)
            #undef case_stmt
         }

         exprtk::ifunction<T>& ifunc = (*(fp_map_[arg_count])[name]);

         return sym_table.add_function(name,ifunc);
      }

      inline void remove(const std::string& name, const std::size_t& arg_count)
      {
         if (arg_count > 6)
            return;

         const typename std::map<std::string,expression_t>::iterator em_itr = expr_map_.find(name);

         if (expr_map_.end() != em_itr)
         {
            expr_map_.erase(em_itr);
         }

         const typename funcparam_t::iterator fp_itr = fp_map_[arg_count].find(name);

         if (fp_map_[arg_count].end() != fp_itr)
         {
            delete fp_itr->second;
            fp_map_[arg_count].erase(fp_itr);
         }

         symbol_table_.remove_function(name);
      }

   private:

      symbol_table_t symbol_table_;
      parser_t parser_;
      std::map<std::string,expression_t> expr_map_;
      std::vector<funcparam_t> fp_map_;
      std::vector<symbol_table_t*> auxiliary_symtab_list_;
      std::deque<parser_error::type> error_list_;
      bool load_variables_;
      bool load_vectors_;
   }; // class function_compositor

} // namespace exprtk
