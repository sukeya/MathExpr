namespace math_expr::rtl::vecops
{
   template <typename T>
   class ror final : public math_expr::igeneric_function<T>
   {
   public:

      typedef typename math_expr::igeneric_function<T> igfun_t;
      typedef typename igfun_t::parameter_list_t    parameter_list_t;
      typedef typename igfun_t::generic_type        generic_type;
      typedef typename generic_type::scalar_view    scalar_t;
      typedef typename generic_type::vector_view    vector_t;

      using igfun_t::operator();

      ror()
      : math_expr::igeneric_function<T>("VT|VTTT")
        /*
           Overloads:
           0. VT   - vector, N
           1. VTTT - vector, N, r0, r1
        */
      {}

      inline T operator() (const std::size_t& ps_index, parameter_list_t parameters) override
      {
         vector_t vec(parameters[0]);

         std::size_t n  = 0;
         std::size_t r0 = 0;
         std::size_t r1 = vec.size() - 1;

         if (!scalar_t(parameters[1]).to_uint(n))
            return T(0);

         if (
              (1 == ps_index) &&
              !helper::load_vector_range<T>::process(parameters, r0, r1, 2, 3, 0)
            )
            return T(0);

         const std::size_t dist  = r1 - r0 + 1;
         const std::size_t shift = (dist - (n % dist)) % dist;

         std::rotate(
            vec.begin() + r0,
            vec.begin() + r0 + shift,
            vec.begin() + r1 + 1);

         return T(1);
      }
   };


} // namespace math_expr::rtl::vecops
