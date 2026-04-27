   class axpbsyz math_expr_final : public math_expr::igeneric_function<T>
   {
   public:

      typedef typename math_expr::igeneric_function<T> igfun_t;
      typedef typename igfun_t::parameter_list_t    parameter_list_t;
      typedef typename igfun_t::generic_type        generic_type;
      typedef typename generic_type::scalar_view    scalar_t;
      typedef typename generic_type::vector_view    vector_t;

      using igfun_t::operator();

      axpbsyz()
      : math_expr::igeneric_function<T>("TVTTVV|TVTTVVTT")
        /*
           z <- ax + by
           Overloads:
           0. TVTVV   - a, x(vector), b, shift, y(vector), z(vector)
           1. TVTVVTT - a, x(vector), b, shift, y(vector), z(vector), r0, r1
        */
      {}

      inline T operator() (const std::size_t& ps_index, parameter_list_t parameters) math_expr_override
      {
         const vector_t x(parameters[1]);
         const vector_t y(parameters[4]);
               vector_t z(parameters[5]);

         std::size_t r0 = 0;
         std::size_t r1 = std::min(x.size(),y.size()) - 1;

         if ((1 == ps_index) && !helper::load_vector_range<T>::process(parameters, r0, r1, 6, 7, 1))
            return std::numeric_limits<T>::quiet_NaN();
         else if (helper::invalid_range(y, r0, r1))
            return std::numeric_limits<T>::quiet_NaN();
         else if (helper::invalid_range(z, r0, r1))
            return std::numeric_limits<T>::quiet_NaN();

         const T a = scalar_t(parameters[0])();
         const T b = scalar_t(parameters[2])();

         const std::size_t s = static_cast<std::size_t>(scalar_t(parameters[3])());

         for (std::size_t i = r0; i <= r1; ++i)
         {
            z[i] = (a * x[i]) + (b * y[i + s]);
         }

         return T(1);
      }
   };

   template <typename T>
