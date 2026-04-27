   class reverse exprtk_final : public exprtk::igeneric_function<T>
   {
   public:

      typedef typename exprtk::igeneric_function<T> igfun_t;
      typedef typename igfun_t::parameter_list_t    parameter_list_t;
      typedef typename igfun_t::generic_type        generic_type;
      typedef typename generic_type::scalar_view    scalar_t;
      typedef typename generic_type::vector_view    vector_t;

      using igfun_t::operator();

      reverse()
      : exprtk::igeneric_function<T>("V|VTT")
        /*
           Overloads:
           0. V   - vector
           1. VTT - vector, r0, r1
        */
      {}

      inline T operator() (const std::size_t& ps_index, parameter_list_t parameters) exprtk_override
      {
         vector_t vec(parameters[0]);

         std::size_t r0 = 0;
         std::size_t r1 = vec.size() - 1;

         if (
              (1 == ps_index) &&
              !helper::load_vector_range<T>::process(parameters, r0, r1, 1, 2, 0)
            )
            return T(0);

         std::reverse(vec.begin() + r0, vec.begin() + r1 + 1);

         return T(1);
      }
   };

   template <typename T>
