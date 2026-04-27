namespace math_expr::rtl::vecops
{
   template <typename T>
   class threshold_above final : public math_expr::igeneric_function<T>
   {
   public:

      typedef typename math_expr::igeneric_function<T> igfun_t;
      typedef typename igfun_t::parameter_list_t    parameter_list_t;
      typedef typename igfun_t::generic_type        generic_type;
      typedef typename generic_type::scalar_view    scalar_t;
      typedef typename generic_type::vector_view    vector_t;

      using igfun_t::operator();

      threshold_above()
      : math_expr::igeneric_function<T>("VTT|VTTTT")
      /*
         Overloads:
         0. VTT   - vector, TV, SV
         1. VTTTT - vector, r0, r1, TV, SV

         Where:
         TV - Threshold value
         SV - Snap-to value
      */
      {}

      inline T operator() (const std::size_t& ps_index, parameter_list_t parameters) override
      {
         vector_t vec(parameters[0]);

         const T threshold_value = (0 == ps_index) ?
                                   scalar_t(parameters[1]) :
                                   scalar_t(parameters[3]) ;

         const T snap_value = scalar_t(parameters.back());

         std::size_t r0 = 0;
         std::size_t r1 = vec.size() - 1;

         if (
              (1 == ps_index) &&
              !helper::load_vector_range<T>::process(parameters, r0, r1, 1, 2, 0)
            )
         {
            return T(0);
         }

         for (std::size_t i = r0; i <= r1; ++i)
         {
            if (vec[i] > threshold_value)
            {
               vec[i] = snap_value;
            }
         }

         return T(1);
      }
   };


} // namespace math_expr::rtl::vecops
