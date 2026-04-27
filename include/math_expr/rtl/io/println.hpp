#ifndef MATH_EXPR_RTL_IO_PRINTLN_HPP
#define MATH_EXPR_RTL_IO_PRINTLN_HPP

namespace math_expr::rtl::io
{
   template <typename T>
   struct println final : public math_expr::igeneric_function<T>
   {
      typedef typename igeneric_function<T>::parameter_list_t parameter_list_t;

      using math_expr::igeneric_function<T>::operator();

      explicit println(const std::string& scalar_format = "%10.5f")
      : scalar_format_(scalar_format)
      {
         math_expr::enable_zero_parameters(*this);
      }

      inline T operator() (parameter_list_t parameters) override
      {
         details::print_impl<T>::process(scalar_format_,parameters);
         printf("\n");
         return T(0);
      }

      std::string scalar_format_;
   };


} // namespace math_expr::rtl::io

#endif
