namespace math_expr
{
   namespace details
   {
      template <typename T>
      struct collector_helper
      {
         typedef math_expr::symbol_table<T> symbol_table_t;
         typedef math_expr::expression<T>   expression_t;
         typedef math_expr::parser<T>       parser_t;
         typedef typename parser_t::dependent_entity_collector::symbol_t symbol_t;
         typedef typename parser_t::unknown_symbol_resolver usr_t;

         struct resolve_as_vector : public usr_t
         {
            typedef math_expr::parser<T> parser_t;

            using usr_t::process;

            resolve_as_vector()
            : usr_t(usr_t::e_usrmode_extended)
            {}

            virtual bool process(const std::string& unknown_symbol,
                                 symbol_table_t& symbol_table,
                                 std::string&) override
            {
               static T v[1];
               symbol_table.add_vector(unknown_symbol,v);
               return true;
            }
         };

         static inline bool collection_pass(const std::string& expression_string,
                                            std::set<std::string>& symbol_set,
                                            const bool collect_variables,
                                            const bool collect_functions,
                                            const bool vector_pass,
                                            symbol_table_t& ext_symbol_table)
         {
            symbol_table_t symbol_table;
            expression_t   expression;
            parser_t       parser;

            resolve_as_vector vect_resolver;

            expression.register_symbol_table(symbol_table    );
            expression.register_symbol_table(ext_symbol_table);

            if (vector_pass)
               parser.enable_unknown_symbol_resolver(&vect_resolver);
            else
               parser.enable_unknown_symbol_resolver();

            if (collect_variables)
               parser.dec().collect_variables() = true;

            if (collect_functions)
               parser.dec().collect_functions() = true;

            bool pass_result = false;

            details::disable_type_checking(parser);

            if (parser.compile(expression_string, expression))
            {
               pass_result = true;

               std::deque<symbol_t> symb_list;
               parser.dec().symbols(symb_list);

               for (std::size_t i = 0; i < symb_list.size(); ++i)
               {
                  symbol_set.insert(symb_list[i].first);
               }
            }

            return pass_result;
         }
      };
   }

   template <typename Allocator,
             template <typename, typename> class Sequence>
   inline bool collect_variables(const std::string& expression,
                                 Sequence<std::string, Allocator>& symbol_list)
   {
      typedef double T;
      typedef details::collector_helper<T> collect_t;

      collect_t::symbol_table_t null_symbol_table;

      std::set<std::string> symbol_set;

      const bool variable_pass = collect_t::collection_pass
                                    (expression, symbol_set, true, false, false, null_symbol_table);
      const bool vector_pass   = collect_t::collection_pass
                                    (expression, symbol_set, true, false,  true, null_symbol_table);

      if (!variable_pass && !vector_pass)
         return false;

      std::set<std::string>::iterator itr = symbol_set.begin();

      while (symbol_set.end() != itr)
      {
         symbol_list.push_back(*itr);
         ++itr;
      }

      return true;
   }

   template <typename T,
             typename Allocator,
             template <typename, typename> class Sequence>
   inline bool collect_variables(const std::string& expression,
                                 math_expr::symbol_table<T>& extrnl_symbol_table,
                                 Sequence<std::string, Allocator>& symbol_list)
   {
      typedef details::collector_helper<T> collect_t;

      std::set<std::string> symbol_set;

      const bool variable_pass = collect_t::collection_pass
                                    (expression, symbol_set, true, false, false, extrnl_symbol_table);
      const bool vector_pass   = collect_t::collection_pass
                                    (expression, symbol_set, true, false,  true, extrnl_symbol_table);

      if (!variable_pass && !vector_pass)
         return false;

      std::set<std::string>::iterator itr = symbol_set.begin();

      while (symbol_set.end() != itr)
      {
         symbol_list.push_back(*itr);
         ++itr;
      }

      return true;
   }

   template <typename Allocator,
             template <typename, typename> class Sequence>
   inline bool collect_functions(const std::string& expression,
                                 Sequence<std::string, Allocator>& symbol_list)
   {
      typedef double T;
      typedef details::collector_helper<T> collect_t;

      collect_t::symbol_table_t null_symbol_table;

      std::set<std::string> symbol_set;

      const bool variable_pass = collect_t::collection_pass
                                    (expression, symbol_set, false, true, false, null_symbol_table);
      const bool vector_pass   = collect_t::collection_pass
                                    (expression, symbol_set, false, true,  true, null_symbol_table);

      if (!variable_pass && !vector_pass)
         return false;

      std::set<std::string>::iterator itr = symbol_set.begin();

      while (symbol_set.end() != itr)
      {
         symbol_list.push_back(*itr);
         ++itr;
      }

      return true;
   }

   template <typename T,
             typename Allocator,
             template <typename, typename> class Sequence>
   inline bool collect_functions(const std::string& expression,
                                 math_expr::symbol_table<T>& extrnl_symbol_table,
                                 Sequence<std::string, Allocator>& symbol_list)
   {
      typedef details::collector_helper<T> collect_t;

      std::set<std::string> symbol_set;

      const bool variable_pass = collect_t::collection_pass
                                    (expression, symbol_set, false, true, false, extrnl_symbol_table);
      const bool vector_pass   = collect_t::collection_pass
                                    (expression, symbol_set, false, true,  true, extrnl_symbol_table);

      if (!variable_pass && !vector_pass)
         return false;

      std::set<std::string>::iterator itr = symbol_set.begin();

      while (symbol_set.end() != itr)
      {
         symbol_list.push_back(*itr);
         ++itr;
      }

      return true;
   }

   template <typename T>
   inline T integrate(const expression<T>& e,
                      T& x,
                      const T& r0, const T& r1,
                      const std::size_t number_of_intervals = 1000000)
   {
      if (r0 > r1)
         return T(0);

      const T h = (r1 - r0) / (T(2) * number_of_intervals);
      T total_area = T(0);

      for (std::size_t i = 0; i < number_of_intervals; ++i)
      {
         x = r0 + T(2) * i * h;
         const T y0 = e.value(); x += h;
         const T y1 = e.value(); x += h;
         const T y2 = e.value(); x += h;
         total_area += h * (y0 + T(4) * y1 + y2) / T(3);
      }

      return total_area;
   }

   template <typename T>
   inline T integrate(const expression<T>& e,
                      const std::string& variable_name,
                      const T& r0, const T& r1,
                      const std::size_t number_of_intervals = 1000000)
   {
      const symbol_table<T>& sym_table = e.get_symbol_table();

      if (!sym_table.valid())
      {
         return std::numeric_limits<T>::quiet_NaN();
      }

      details::variable_node<T>* var = sym_table.get_variable(variable_name);

      if (var)
      {
         T& x = var->ref();
         const T x_original = x;
         const T result = integrate(e, x, r0, r1, number_of_intervals);
         x = x_original;

         return result;
      }

      return std::numeric_limits<T>::quiet_NaN();
   }

   template <typename T>
   inline T derivative(const expression<T>& e,
                       T& x,
                       const T& h = T(0.00000001))
   {
      const T x_init = x;
      const T _2h    = T(2) * h;

      x = x_init + _2h;
      const T y0 = e.value();
      x = x_init + h;
      const T y1 = e.value();
      x = x_init - h;
      const T y2 = e.value();
      x = x_init - _2h;
      const T y3 = e.value();
      x = x_init;

      return (-y0 + T(8) * (y1 - y2) + y3) / (T(12) * h);
   }

   template <typename T>
   inline T second_derivative(const expression<T>& e,
                              T& x,
                              const T& h = T(0.00001))
   {
      const T x_init = x;
      const T _2h    = T(2) * h;

      const T y = e.value();
      x = x_init + _2h;
      const T y0 = e.value();
      x = x_init + h;
      const T y1 = e.value();
      x = x_init - h;
      const T y2 = e.value();
      x = x_init - _2h;
      const T y3 = e.value();
      x = x_init;

      return (-y0 + T(16) * (y1 + y2) - T(30) * y - y3) / (T(12) * h * h);
   }

   template <typename T>
   inline T third_derivative(const expression<T>& e,
                             T& x,
                             const T& h = T(0.0001))
   {
      const T x_init = x;
      const T _2h    = T(2) * h;

      x = x_init + _2h;
      const T y0 = e.value();
      x = x_init + h;
      const T y1 = e.value();
      x = x_init - h;
      const T y2 = e.value();
      x = x_init - _2h;
      const T y3 = e.value();
      x = x_init;

      return (y0 + T(2) * (y2 - y1) - y3) / (T(2) * h * h * h);
   }

   template <typename T>
   inline T derivative(const expression<T>& e,
                       const std::string& variable_name,
                       const T& h = T(0.00000001))
   {
      const symbol_table<T>& sym_table = e.get_symbol_table();

      if (!sym_table.valid())
      {
         return std::numeric_limits<T>::quiet_NaN();
      }

      details::variable_node<T>* var = sym_table.get_variable(variable_name);

      if (var)
      {
         T& x = var->ref();
         const T x_original = x;
         const T result = derivative(e, x, h);
         x = x_original;

         return result;
      }

      return std::numeric_limits<T>::quiet_NaN();
   }

   template <typename T>
   inline T second_derivative(const expression<T>& e,
                              const std::string& variable_name,
                              const T& h = T(0.00001))
   {
      const symbol_table<T>& sym_table = e.get_symbol_table();

      if (!sym_table.valid())
      {
         return std::numeric_limits<T>::quiet_NaN();
      }

      details::variable_node<T>* var = sym_table.get_variable(variable_name);

      if (var)
      {
         T& x = var->ref();
         const T x_original = x;
         const T result = second_derivative(e, x, h);
         x = x_original;

         return result;
      }

      return std::numeric_limits<T>::quiet_NaN();
   }

   template <typename T>
   inline T third_derivative(const expression<T>& e,
                             const std::string& variable_name,
                             const T& h = T(0.0001))
   {
      const symbol_table<T>& sym_table = e.get_symbol_table();

      if (!sym_table.valid())
      {
         return std::numeric_limits<T>::quiet_NaN();
      }

      details::variable_node<T>* var = sym_table.get_variable(variable_name);

      if (var)
      {
         T& x = var->ref();
         const T x_original = x;
         const T result = third_derivative(e, x, h);
         x = x_original;

         return result;
      }

      return std::numeric_limits<T>::quiet_NaN();
   }

   /*
      Note: The following 'compute' routines are simple helpers,
      for quickly setting up the required pieces of code in order
      to evaluate an expression. By virtue of how they operate
      there will be an overhead with regards to their setup and
      teardown and hence should not be used in time critical
      sections of code.
      Furthermore they only assume a small sub set of variables,
      no string variables or user defined functions.
   */
   template <typename T>
   inline bool compute(const std::string& expression_string, T& result)
   {
      // No variables
      symbol_table<T> symbol_table;
      symbol_table.add_constants();

      expression<T> expression;
      expression.register_symbol_table(symbol_table);

      parser<T> parser;

      if (parser.compile(expression_string,expression))
      {
         result = expression.value();

         return true;
      }
      else
         return false;
   }

   template <typename T>
   inline bool compute(const std::string& expression_string,
                       const T& x,
                       T& result)
   {
      // Only 'x'
      static const std::string x_var("x");

      symbol_table<T> symbol_table;
      symbol_table.add_constants();
      symbol_table.add_constant(x_var,x);

      expression<T> expression;
      expression.register_symbol_table(symbol_table);

      parser<T> parser;

      if (parser.compile(expression_string,expression))
      {
         result = expression.value();

         return true;
      }
      else
         return false;
   }

   template <typename T>
   inline bool compute(const std::string& expression_string,
                       const T&x, const T& y,
                       T& result)
   {
      // Only 'x' and 'y'
      static const std::string x_var("x");
      static const std::string y_var("y");

      symbol_table<T> symbol_table;
      symbol_table.add_constants();
      symbol_table.add_constant(x_var,x);
      symbol_table.add_constant(y_var,y);

      expression<T> expression;
      expression.register_symbol_table(symbol_table);

      parser<T> parser;

      if (parser.compile(expression_string,expression))
      {
         result = expression.value();

         return true;
      }
      else
         return false;
   }

   template <typename T>
   inline bool compute(const std::string& expression_string,
                       const T& x, const T& y, const T& z,
                       T& result)
   {
      // Only 'x', 'y' or 'z'
      static const std::string x_var("x");
      static const std::string y_var("y");
      static const std::string z_var("z");

      symbol_table<T> symbol_table;
      symbol_table.add_constants();
      symbol_table.add_constant(x_var,x);
      symbol_table.add_constant(y_var,y);
      symbol_table.add_constant(z_var,z);

      expression<T> expression;
      expression.register_symbol_table(symbol_table);

      parser<T> parser;

      if (parser.compile(expression_string,expression))
      {
         result = expression.value();

         return true;
      }
      else
         return false;
   }

   template <typename T, std::size_t N>
   class polynomial : public ifunction<T>
   {
   private:

      template <typename Type, std::size_t NumberOfCoefficients>
      struct poly_impl { };

      template <typename Type>
      struct poly_impl <Type,12>
      {
         static inline T evaluate(const Type x,
                                  const Type c12, const Type c11, const Type c10, const Type c9, const Type c8,
                                  const Type  c7, const Type  c6, const Type  c5, const Type c4, const Type c3,
                                  const Type  c2, const Type  c1, const Type  c0)
         {
            // p(x) = c_12x^12 + c_11x^11 + c_10x^10 + c_9x^9 + c_8x^8 + c_7x^7 + c_6x^6 + c_5x^5 + c_4x^4 + c_3x^3 + c_2x^2 + c_1x^1 + c_0x^0
            return ((((((((((((c12 * x + c11) * x + c10) * x + c9) * x + c8) * x + c7) * x + c6) * x + c5) * x + c4) * x + c3) * x + c2) * x + c1) * x + c0);
         }
      };

      template <typename Type>
      struct poly_impl <Type,11>
      {
         static inline T evaluate(const Type x,
                                  const Type c11, const Type c10, const Type c9, const Type c8, const Type c7,
                                  const Type c6,  const Type  c5, const Type c4, const Type c3, const Type c2,
                                  const Type c1,  const Type  c0)
         {
            // p(x) = c_11x^11 + c_10x^10 + c_9x^9 + c_8x^8 + c_7x^7 + c_6x^6 + c_5x^5 + c_4x^4 + c_3x^3 + c_2x^2 + c_1x^1 + c_0x^0
            return (((((((((((c11 * x + c10) * x + c9) * x + c8) * x + c7) * x + c6) * x + c5) * x + c4) * x + c3) * x + c2) * x + c1) * x + c0);
         }
      };

      template <typename Type>
      struct poly_impl <Type,10>
      {
         static inline T evaluate(const Type x,
                                  const Type c10, const Type c9, const Type c8, const Type c7, const Type c6,
                                  const Type c5,  const Type c4, const Type c3, const Type c2, const Type c1,
                                  const Type c0)
         {
            // p(x) = c_10x^10 + c_9x^9 + c_8x^8 + c_7x^7 + c_6x^6 + c_5x^5 + c_4x^4 + c_3x^3 + c_2x^2 + c_1x^1 + c_0x^0
            return ((((((((((c10 * x + c9) * x + c8) * x + c7) * x + c6) * x + c5) * x + c4) * x + c3) * x + c2) * x + c1) * x + c0);
         }
      };

      template <typename Type>
      struct poly_impl <Type,9>
      {
         static inline T evaluate(const Type x,
                                  const Type c9, const Type c8, const Type c7, const Type c6, const Type c5,
                                  const Type c4, const Type c3, const Type c2, const Type c1, const Type c0)
         {
            // p(x) = c_9x^9 + c_8x^8 + c_7x^7 + c_6x^6 + c_5x^5 + c_4x^4 + c_3x^3 + c_2x^2 + c_1x^1 + c_0x^0
            return (((((((((c9 * x + c8) * x + c7) * x + c6) * x + c5) * x + c4) * x + c3) * x + c2) * x + c1) * x + c0);
         }
      };

      template <typename Type>
      struct poly_impl <Type,8>
      {
         static inline T evaluate(const Type x,
                                  const Type c8, const Type c7, const Type c6, const Type c5, const Type c4,
                                  const Type c3, const Type c2, const Type c1, const Type c0)
         {
            // p(x) = c_8x^8 + c_7x^7 + c_6x^6 + c_5x^5 + c_4x^4 + c_3x^3 + c_2x^2 + c_1x^1 + c_0x^0
            return ((((((((c8 * x + c7) * x + c6) * x + c5) * x + c4) * x + c3) * x + c2) * x + c1) * x + c0);
         }
      };

      template <typename Type>
      struct poly_impl <Type,7>
      {
         static inline T evaluate(const Type x,
                                  const Type c7, const Type c6, const Type c5, const Type c4, const Type c3,
                                  const Type c2, const Type c1, const Type c0)
         {
            // p(x) = c_7x^7 + c_6x^6 + c_5x^5 + c_4x^4 + c_3x^3 + c_2x^2 + c_1x^1 + c_0x^0
            return (((((((c7 * x + c6) * x + c5) * x + c4) * x + c3) * x + c2) * x + c1) * x + c0);
         }
      };

      template <typename Type>
      struct poly_impl <Type,6>
      {
         static inline T evaluate(const Type x,
                                  const Type c6, const Type c5, const Type c4, const Type c3, const Type c2,
                                  const Type c1, const Type c0)
         {
            // p(x) = c_6x^6 + c_5x^5 + c_4x^4 + c_3x^3 + c_2x^2 + c_1x^1 + c_0x^0
            return ((((((c6 * x + c5) * x + c4) * x + c3) * x + c2) * x + c1) * x + c0);
         }
      };

      template <typename Type>
      struct poly_impl <Type,5>
      {
         static inline T evaluate(const Type x,
                                  const Type c5, const Type c4, const Type c3, const Type c2,
                                  const Type c1, const Type c0)
         {
            // p(x) = c_5x^5 + c_4x^4 + c_3x^3 + c_2x^2 + c_1x^1 + c_0x^0
            return (((((c5 * x + c4) * x + c3) * x + c2) * x + c1) * x + c0);
         }
      };

      template <typename Type>
      struct poly_impl <Type,4>
      {
         static inline T evaluate(const Type x, const Type c4, const Type c3, const Type c2, const Type c1, const Type c0)
         {
            // p(x) = c_4x^4 + c_3x^3 + c_2x^2 + c_1x^1 + c_0x^0
            return ((((c4 * x + c3) * x + c2) * x + c1) * x + c0);
         }
      };

      template <typename Type>
      struct poly_impl <Type,3>
      {
         static inline T evaluate(const Type x, const Type c3, const Type c2, const Type c1, const Type c0)
         {
            // p(x) = c_3x^3 + c_2x^2 + c_1x^1 + c_0x^0
            return (((c3 * x + c2) * x + c1) * x + c0);
         }
      };

      template <typename Type>
      struct poly_impl <Type,2>
      {
         static inline T evaluate(const Type x, const Type c2, const Type c1, const Type c0)
         {
            // p(x) = c_2x^2 + c_1x^1 + c_0x^0
            return ((c2 * x + c1) * x + c0);
         }
      };

      template <typename Type>
      struct poly_impl <Type,1>
      {
         static inline T evaluate(const Type x, const Type c1, const Type c0)
         {
            // p(x) = c_1x^1 + c_0x^0
            return (c1 * x + c0);
         }
      };

   public:

      using ifunction<T>::operator();

      polynomial()
      : ifunction<T>((N+2 <= 20) ? (N + 2) : std::numeric_limits<std::size_t>::max())
      {
         disable_has_side_effects(*this);
      }

      virtual ~polynomial() override
      {}

      #define poly_rtrn(NN) \
      return (NN != N) ? std::numeric_limits<T>::quiet_NaN() :

      inline virtual T operator() (const T& x, const T& c1, const T& c0) override
      {
         poly_rtrn(1) (poly_impl<T,1>::evaluate(x, c1, c0));
      }

      inline virtual T operator() (const T& x, const T& c2, const T& c1, const T& c0) override
      {
         poly_rtrn(2) (poly_impl<T,2>::evaluate(x, c2, c1, c0));
      }

      inline virtual T operator() (const T& x, const T& c3, const T& c2, const T& c1, const T& c0) override
      {
         poly_rtrn(3) (poly_impl<T,3>::evaluate(x, c3, c2, c1, c0));
      }

      inline virtual T operator() (const T& x, const T& c4, const T& c3, const T& c2, const T& c1,
                                   const T& c0) override
      {
         poly_rtrn(4) (poly_impl<T,4>::evaluate(x, c4, c3, c2, c1, c0));
      }

      inline virtual T operator() (const T& x, const T& c5, const T& c4, const T& c3, const T& c2,
                                   const T& c1, const T& c0) override
      {
         poly_rtrn(5) (poly_impl<T,5>::evaluate(x, c5, c4, c3, c2, c1, c0));
      }

      inline virtual T operator() (const T& x, const T& c6, const T& c5, const T& c4, const T& c3,
                                   const T& c2, const T& c1, const T& c0) override
      {
         poly_rtrn(6) (poly_impl<T,6>::evaluate(x, c6, c5, c4, c3, c2, c1, c0));
      }

      inline virtual T operator() (const T& x, const T& c7, const T& c6, const T& c5, const T& c4,
                                   const T& c3, const T& c2, const T& c1, const T& c0) override
      {
         poly_rtrn(7) (poly_impl<T,7>::evaluate(x, c7, c6, c5, c4, c3, c2, c1, c0));
      }

      inline virtual T operator() (const T& x, const T& c8, const T& c7, const T& c6, const T& c5,
                                   const T& c4, const T& c3, const T& c2, const T& c1, const T& c0) override
      {
         poly_rtrn(8) (poly_impl<T,8>::evaluate(x, c8, c7, c6, c5, c4, c3, c2, c1, c0));
      }

      inline virtual T operator() (const T& x, const T& c9, const T& c8, const T& c7, const T& c6,
                                   const T& c5, const T& c4, const T& c3, const T& c2, const T& c1,
                                   const T& c0) override
      {
         poly_rtrn(9) (poly_impl<T,9>::evaluate(x, c9, c8, c7, c6, c5, c4, c3, c2, c1, c0));
      }

      inline virtual T operator() (const T& x, const T& c10, const T& c9, const T& c8, const T& c7,
                                   const T& c6, const T& c5, const T& c4, const T& c3, const T& c2,
                                   const T& c1, const T& c0) override
      {
         poly_rtrn(10) (poly_impl<T,10>::evaluate(x, c10, c9, c8, c7, c6, c5, c4, c3, c2, c1, c0));
      }

      inline virtual T operator() (const T& x, const T& c11, const T& c10, const T& c9, const T& c8,
                                   const T& c7, const T& c6, const T& c5, const T& c4, const T& c3,
                                   const T& c2, const T& c1, const T& c0) override
      {
         poly_rtrn(11) (poly_impl<T,11>::evaluate(x, c11, c10, c9, c8, c7, c6, c5, c4, c3, c2, c1, c0));
      }

      inline virtual T operator() (const T& x, const T& c12, const T& c11, const T& c10, const T& c9,
                                   const T& c8, const T& c7, const T& c6, const T& c5, const T& c4,
                                   const T& c3, const T& c2, const T& c1, const T& c0) override
      {
         poly_rtrn(12) (poly_impl<T,12>::evaluate(x, c12, c11, c10, c9, c8, c7, c6, c5, c4, c3, c2, c1, c0));
      }

      #undef poly_rtrn

      inline virtual T operator() () override
      {
         return std::numeric_limits<T>::quiet_NaN();
      }

      inline virtual T operator() (const T&) override
      {
         return std::numeric_limits<T>::quiet_NaN();
      }

      inline virtual T operator() (const T&, const T&) override
      {
         return std::numeric_limits<T>::quiet_NaN();
      }
   };


} // namespace math_expr
