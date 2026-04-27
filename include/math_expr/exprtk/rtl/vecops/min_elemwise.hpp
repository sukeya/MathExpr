   class min_elemwise exprtk_final : public exprtk::igeneric_function<T>
   {
   public:

      typedef typename exprtk::igeneric_function<T> igfun_t;
      typedef typename igfun_t::parameter_list_t    parameter_list_t;
      typedef typename igfun_t::generic_type        generic_type;
      typedef typename generic_type::scalar_view    scalar_t;
      typedef typename generic_type::vector_view    vector_t;

      using igfun_t::operator();

      min_elemwise()
      : exprtk::igeneric_function<T>("VT|VVT|VTTT|VVTTT")
      /*
         Overloads:
         0. VT    - vector, T
         0. VVT   - vector, vector, T
         0. VTTT  - vector, r0, r1, T
         0. VVTTT - vector, vector, r0, r1, T
      */
      {}

      inline T operator() (const std::size_t& ps_index, parameter_list_t parameters) exprtk_override
      {
         std::size_t out_vec_index = 0;
         std::size_t in_vec_index  = (ps_index & 1) ? 1 : 0;
         std::size_t scalar_index  = parameters.size() - 1;

         vector_t out_vec(parameters[out_vec_index]);
         vector_t in_vec (parameters[in_vec_index ]);

         const T s = scalar_t(parameters[scalar_index ])();

         std::size_t r0 = 0;
         std::size_t r1 = in_vec.size() - 1;

         if ((2 == ps_index) || (3 == ps_index))
         {
            std::size_t rng_idx0 = 0;
            std::size_t rng_idx1 = 0;

            switch (ps_index)
            {
               case 2 : { rng_idx0 = 1; rng_idx1 = 2; }; break;
               case 3 : { rng_idx0 = 2; rng_idx1 = 3; }; break;
            }

            if (!helper::load_vector_range<T>::process(parameters, r0, r1, rng_idx0, rng_idx1, 0))
            {
               return T(0);
            }
         }

         for (std::size_t i = r0; i <= r1; ++i)
         {
            out_vec[i] = exprtk::details::numeric::min(in_vec[i], s);
         }

         return T(1);
      }
   };

   template <typename T>
