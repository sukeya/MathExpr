   class copy final : public math_expr::igeneric_function<T>
   {
   public:

      typedef typename math_expr::igeneric_function<T> igfun_t;
      typedef typename igfun_t::parameter_list_t    parameter_list_t;
      typedef typename igfun_t::generic_type        generic_type;
      typedef typename generic_type::scalar_view    scalar_t;
      typedef typename generic_type::vector_view    vector_t;

      using igfun_t::operator();

      copy()
      : math_expr::igeneric_function<T>("VV|VTTVTT")
        /*
           Overloads:
           0. VV     - x(vector), y(vector)
           1. VTTVTT - x(vector), xr0, xr1, y(vector), yr0, yr1,
        */
      {}

      inline T operator() (const std::size_t& ps_index, parameter_list_t parameters) override
      {
         const vector_t x(parameters[0]);
               vector_t y(parameters[(0 == ps_index) ? 1 : 3]);

         std::size_t xr0 = 0;
         std::size_t xr1 = x.size() - 1;

         std::size_t yr0 = 0;
         std::size_t yr1 = y.size() - 1;

         if (1 == ps_index)
         {
            if (
                 !helper::load_vector_range<T>::process(parameters, xr0, xr1, 1, 2, 0) ||
                 !helper::load_vector_range<T>::process(parameters, yr0, yr1, 4, 5, 3)
               )
               return T(0);
         }

         const std::size_t n = std::min(xr1 - xr0 + 1, yr1 - yr0 + 1);

         std::copy(
            x.begin() + xr0,
            x.begin() + xr0 + n,
            y.begin() + yr0);

         return T(n);
      }
   };

   template <typename T>
