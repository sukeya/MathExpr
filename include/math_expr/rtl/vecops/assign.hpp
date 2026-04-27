   class assign math_expr_final : public math_expr::igeneric_function<T>
   {
   public:

      typedef typename math_expr::igeneric_function<T> igfun_t;
      typedef typename igfun_t::parameter_list_t    parameter_list_t;
      typedef typename igfun_t::generic_type        generic_type;
      typedef typename generic_type::scalar_view    scalar_t;
      typedef typename generic_type::vector_view    vector_t;

      using igfun_t::operator();

      assign()
      : math_expr::igeneric_function<T>("VT|VTTT|VTTTT")
        /*
           Overloads:
           0. VT    - vector, V
           1. VTTT  - vector, V, r0, r1
           2. VTTTT - vector, V, r0, r1, SS
        */
      {}

      inline T operator() (const std::size_t& ps_index, parameter_list_t parameters) math_expr_override
      {
         vector_t vec(parameters[0]);

         const T assign_value = scalar_t(parameters[1]);

         const std::size_t step_size = (2 != ps_index) ? 1 :
                                       static_cast<std::size_t>(scalar_t(parameters.back())());

         std::size_t r0 = 0;
         std::size_t r1 = vec.size() - 1;

         if (
              ((ps_index == 1) || (ps_index == 2)) &&
              !helper::load_vector_range<T>::process(parameters, r0, r1, 2, 3, 0)
            )
         {
            return T(0);
         }

         for (std::size_t i = r0; i <= r1; i += step_size)
         {
            vec[i] = assign_value;
         }

         return T(1);
      }
   };

   template <typename T>
