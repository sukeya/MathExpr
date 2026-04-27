   class count final : public math_expr::igeneric_function<T>
   {
   public:

      typedef typename math_expr::igeneric_function<T> igfun_t;
      typedef typename igfun_t::parameter_list_t    parameter_list_t;
      typedef typename igfun_t::generic_type        generic_type;
      typedef typename generic_type::scalar_view    scalar_t;
      typedef typename generic_type::vector_view    vector_t;

      using igfun_t::operator();

      count()
      : math_expr::igeneric_function<T>("V|VTT|T*")
        /*
           Overloads:
           0. V   - vector
           1. VTT - vector, r0, r1
           2. T*  - T....T
        */
      {}

      inline T operator() (const std::size_t& ps_index, parameter_list_t parameters) override
      {
         std::size_t cnt = 0;

         if (2 == ps_index)
         {
            for (std::size_t i = 0; i < parameters.size(); ++i)
            {
               if (scalar_t(parameters[i])() != T(0)) ++cnt;
            }
         }
         else
         {
            const vector_t vec(parameters[0]);

            std::size_t r0 = 0;
            std::size_t r1 = vec.size() - 1;

            if (
                 (1 == ps_index) &&
                 !helper::load_vector_range<T>::process(parameters, r0, r1, 1, 2, 0)
               )
            {
               return std::numeric_limits<T>::quiet_NaN();
            }

            for (std::size_t i = r0; i <= r1; ++i)
            {
               if (vec[i] != T(0)) ++cnt;
            }
         }

         return T(cnt);
      }
   };

   template <typename T>
