   class select final : public math_expr::igeneric_function<T>
   {
   public:

      typedef typename math_expr::igeneric_function<T> igfun_t;
      typedef typename igfun_t::parameter_list_t    parameter_list_t;
      typedef typename igfun_t::generic_type        generic_type;
      typedef typename generic_type::vector_view    vector_t;

      using igfun_t::operator();

      select()
      : math_expr::igeneric_function<T>("VVVV|VVVVTT")
      /*
         Overloads:
         0. VVVV  - out vector, mask, vector 0, vector 1
         1. VVVV  - out vector, mask, vector 0, vector 1, r0, r1
      */
      {}

      inline T operator() (const std::size_t& ps_index, parameter_list_t parameters) override
      {
               vector_t out (parameters[0]);
         const vector_t mask(parameters[1]);
         const vector_t vec0(parameters[2]);
         const vector_t vec1(parameters[3]);

         std::size_t r0 = 0;
         std::size_t r1 = out.size() - 1;

         if (1 == ps_index)
         {
            std::size_t rng_idx0 = 4;
            std::size_t rng_idx1 = 5;

            if (!helper::load_vector_range<T>::process(parameters, r0, r1, rng_idx0, rng_idx1, 0))
            {
               return T(0);
            }

            if (helper::invalid_range(out , r0, r1)) return T(0);
            if (helper::invalid_range(mask, r0, r1)) return T(0);
            if (helper::invalid_range(vec0, r0, r1)) return T(0);
            if (helper::invalid_range(vec1, r0, r1)) return T(0);
         }

         for (std::size_t i = r0; i <= r1; ++i)
         {
            out[i] = (mask[i] != T(0)) ? vec0[i] : vec1[i];
         }

         return T(1);
      }
   };

   template <typename T>
