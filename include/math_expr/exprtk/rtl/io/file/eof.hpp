   struct eof exprtk_final : public exprtk::ifunction<T>
   {
      using exprtk::ifunction<T>::operator();

      eof()
      : exprtk::ifunction<T>(1)
      { details::perform_check<T>(); }

      inline T operator() (const T& v) exprtk_override
      {
         details::file_descriptor* fd = details::make_handle(v);
         return (fd->eof() ? T(1) : T(0));
      }
   };

   template <typename T>
