   struct close exprtk_final : public exprtk::ifunction<T>
   {
      using exprtk::ifunction<T>::operator();

      close()
      : exprtk::ifunction<T>(1)
      { details::perform_check<T>(); }

      inline T operator() (const T& v) exprtk_override
      {
         details::file_descriptor* fd = details::make_handle(v);

         if (!fd->close())
            return T(0);

         delete fd;

         return T(1);
      }
   };

   template <typename T>
