   class diff math_expr_final : public math_expr::igeneric_function<T>
   {
   public:

      typedef typename math_expr::igeneric_function<T> igfun_t;
      typedef typename igfun_t::parameter_list_t    parameter_list_t;
      typedef typename igfun_t::generic_type        generic_type;
      typedef typename generic_type::scalar_view    scalar_t;
      typedef typename generic_type::vector_view    vector_t;

      using igfun_t::operator();

      diff()
      : math_expr::igeneric_function<T>("VV|VVT")
        /*
           x_(i - stride) - x_i
           Overloads:
           0. VV  - x(vector), y(vector)
           1. VVT - x(vector), y(vector), stride
        */
      {}

      inline T operator() (const std::size_t& ps_index, parameter_list_t parameters) math_expr_override
      {
         const vector_t x(parameters[0]);
               vector_t y(parameters[1]);

         const std::size_t r0 = 0;
         const std::size_t r1 = std::min(x.size(),y.size()) - 1;

         const std::size_t stride = (1 != ps_index) ? 1 :
                                    std::min(r1,static_cast<std::size_t>(scalar_t(parameters[2])()));

         for (std::size_t i = 0; i < stride; ++i)
         {
            y[i] = std::numeric_limits<T>::quiet_NaN();
         }

         for (std::size_t i = (r0 + stride); i <= r1; ++i)
         {
            y[i] = x[i] - x[i - stride];
         }

         return T(1);
      }
   };

   template <typename T>
