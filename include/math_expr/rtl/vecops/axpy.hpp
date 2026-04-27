namespace math_expr::rtl::vecops
{
   template <typename T>
   class axpy final : public math_expr::igeneric_function<T>
   {
   public:

      typedef typename math_expr::igeneric_function<T> igfun_t;
      typedef typename igfun_t::parameter_list_t    parameter_list_t;
      typedef typename igfun_t::generic_type        generic_type;
      typedef typename generic_type::scalar_view    scalar_t;
      typedef typename generic_type::vector_view    vector_t;

      using igfun_t::operator();

      axpy()
      : math_expr::igeneric_function<T>("TVV|TVVTT")
        /*
           y <- ax + y
           Overloads:
           0. TVV   - a, x(vector), y(vector)
           1. TVVTT - a, x(vector), y(vector), r0, r1
        */
      {}

      inline T operator() (const std::size_t& ps_index, parameter_list_t parameters) override
      {
         const vector_t x(parameters[1]);
               vector_t y(parameters[2]);

         std::size_t r0 = 0;
         std::size_t r1 = std::min(x.size(),y.size()) - 1;

         if ((1 == ps_index) && !helper::load_vector_range<T>::process(parameters, r0, r1, 3, 4, 1))
            return std::numeric_limits<T>::quiet_NaN();
         else if (helper::invalid_range(y, r0, r1))
            return std::numeric_limits<T>::quiet_NaN();

         const T a = scalar_t(parameters[0])();

         for (std::size_t i = r0; i <= r1; ++i)
         {
            y[i] = (a * x[i]) + y[i];
         }

         return T(1);
      }
   };


} // namespace math_expr::rtl::vecops
