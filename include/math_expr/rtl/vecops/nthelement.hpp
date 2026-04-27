   class nthelement math_expr_final : public math_expr::igeneric_function<T>
   {
   public:

      typedef typename math_expr::igeneric_function<T> igfun_t;
      typedef typename igfun_t::parameter_list_t    parameter_list_t;
      typedef typename igfun_t::generic_type        generic_type;
      typedef typename generic_type::scalar_view    scalar_t;
      typedef typename generic_type::vector_view    vector_t;

      using igfun_t::operator();

      nthelement()
      : math_expr::igeneric_function<T>("VT|VTTT")
        /*
           Overloads:
           0. VT   - vector, nth-element
           1. VTTT - vector, nth-element, r0, r1
        */
      {}

      inline T operator() (const std::size_t& ps_index, parameter_list_t parameters) math_expr_override
      {
         vector_t vec(parameters[0]);

         std::size_t n  = 0;
         std::size_t r0 = 0;
         std::size_t r1 = vec.size() - 1;

         if (!scalar_t(parameters[1]).to_uint(n))
            return T(0);

         if ((1 == ps_index) && !helper::load_vector_range<T>::process(parameters, r0, r1, 2, 3, 0))
         {
            return std::numeric_limits<T>::quiet_NaN();
         }

         std::nth_element(
            vec.begin() + r0,
            vec.begin() + r0 + n ,
            vec.begin() + r1 + 1);

         return T(1);
      }
   };

   template <typename T>
