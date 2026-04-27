namespace math_expr::rtl::vecops
{
   template <typename T>
   class sumk final : public math_expr::igeneric_function<T>
   {
   public:

      typedef typename math_expr::igeneric_function<T> igfun_t;
      typedef typename igfun_t::parameter_list_t    parameter_list_t;
      typedef typename igfun_t::generic_type        generic_type;
      typedef typename generic_type::scalar_view    scalar_t;
      typedef typename generic_type::vector_view    vector_t;

      using igfun_t::operator();

      sumk()
      : math_expr::igeneric_function<T>("V|VTT|VTTT")
        /*
           Overloads:
           0. V    - vector
           1. VTT  - vector, r0, r1
           2. VTTT - vector, r0, r1, stride
        */
      {}

      inline T operator() (const std::size_t& ps_index, parameter_list_t parameters) override
      {
         const vector_t vec(parameters[0]);

         const std::size_t stride = (2 != ps_index) ? 1 :
                                    static_cast<std::size_t>(scalar_t(parameters[3])());

         std::size_t r0 = 0;
         std::size_t r1 = vec.size() - 1;

         if (
              ((1 == ps_index) || (2 == ps_index)) &&
              !helper::load_vector_range<T>::process(parameters, r0, r1, 1, 2, 0)
            )
         {
            return std::numeric_limits<T>::quiet_NaN();
         }

         T result = T(0);
         T error  = T(0);

         for (std::size_t i = r0; i <= r1; i += stride)
         {
            details::kahan_sum(result, error, vec[i]);
         }

         return result;
      }
   };


} // namespace math_expr::rtl::vecops
