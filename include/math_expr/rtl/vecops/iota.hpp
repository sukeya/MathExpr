#ifndef MATH_EXPR_RTL_VECOPS_IOTA_HPP
#define MATH_EXPR_RTL_VECOPS_IOTA_HPP

namespace math_expr::rtl::vecops
{
   template <typename T>
   class iota final : public math_expr::igeneric_function<T>
   {
   public:

      typedef typename math_expr::igeneric_function<T> igfun_t;
      typedef typename igfun_t::parameter_list_t    parameter_list_t;
      typedef typename igfun_t::generic_type        generic_type;
      typedef typename generic_type::scalar_view    scalar_t;
      typedef typename generic_type::vector_view    vector_t;

      using igfun_t::operator();

      iota()
      : math_expr::igeneric_function<T>("VTT|VT|VTTTT|VTTT")
        /*
           Overloads:
           0. VTT  - vector, SV, SS
           1. VT   - vector, SV, SS (+1)
           2. VTTT - vector, r0, r1, SV, SS
           3. VTT  - vector, r0, r1, SV, SS (+1)

           Where:
           1. SV - Start value
           2. SS - Step size
        */
      {}

      inline T operator() (const std::size_t& ps_index, parameter_list_t parameters) override
      {
         vector_t vec(parameters[0]);

         const T start_value = (ps_index <= 1) ?
                               scalar_t(parameters[1]) :
                               scalar_t(parameters[3]) ;

         const T step_size = ((0 == ps_index) || (2 == ps_index)) ?
                             scalar_t(parameters.back())() :
                             T(1) ;

         std::size_t r0 = 0;
         std::size_t r1 = vec.size() - 1;

         if (
              ((ps_index == 2) || (ps_index == 3)) &&
              !helper::load_vector_range<T>::process(parameters, r0, r1, 1, 2, 0)
            )
         {
            return T(0);
         }

         for (std::size_t i = r0; i <= r1; ++i)
         {
            vec[i] = start_value + ((i - r0) * step_size);
         }

         return T(1);
      }
   };


} // namespace math_expr::rtl::vecops

#endif
