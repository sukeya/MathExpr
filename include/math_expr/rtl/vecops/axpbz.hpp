#ifndef MATH_EXPR_RTL_VECOPS_AXPBZ_HPP
#define MATH_EXPR_RTL_VECOPS_AXPBZ_HPP

namespace math_expr::rtl::vecops
{
   template <typename T>
   class axpbz final : public math_expr::igeneric_function<T>
   {
   public:

      typedef typename math_expr::igeneric_function<T> igfun_t;
      typedef typename igfun_t::parameter_list_t    parameter_list_t;
      typedef typename igfun_t::generic_type        generic_type;
      typedef typename generic_type::scalar_view    scalar_t;
      typedef typename generic_type::vector_view    vector_t;

      using igfun_t::operator();

      axpbz()
      : math_expr::igeneric_function<T>("TVTV|TVTVTT")
        /*
           z <- ax + b
           Overloads:
           0. TVTV   - a, x(vector), b, z(vector)
           1. TVTVTT - a, x(vector), b, z(vector), r0, r1
        */
      {}

      inline T operator() (const std::size_t& ps_index, parameter_list_t parameters) override
      {
         const vector_t x(parameters[1]);
               vector_t z(parameters[3]);

         std::size_t r0 = 0;
         std::size_t r1 = x.size() - 1;

         if ((1 == ps_index) && !helper::load_vector_range<T>::process(parameters, r0, r1, 4, 5, 1))
            return std::numeric_limits<T>::quiet_NaN();
         else if (helper::invalid_range(z, r0, r1))
            return std::numeric_limits<T>::quiet_NaN();

         const T a = scalar_t(parameters[0])();
         const T b = scalar_t(parameters[2])();

         for (std::size_t i = r0; i <= r1; ++i)
         {
            z[i] = (a * x[i]) + b;
         }

         return T(1);
      }
   };


} // namespace math_expr::rtl::vecops

#endif
