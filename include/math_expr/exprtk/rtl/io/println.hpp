   struct println exprtk_final : public exprtk::igeneric_function<T>
   {
      typedef typename igeneric_function<T>::parameter_list_t parameter_list_t;

      using exprtk::igeneric_function<T>::operator();

      explicit println(const std::string& scalar_format = "%10.5f")
      : scalar_format_(scalar_format)
      {
         exprtk::enable_zero_parameters(*this);
      }

      inline T operator() (parameter_list_t parameters) exprtk_override
      {
         details::print_impl<T>::process(scalar_format_,parameters);
         printf("\n");
         return T(0);
      }

      std::string scalar_format_;
   };

   template <typename T>
