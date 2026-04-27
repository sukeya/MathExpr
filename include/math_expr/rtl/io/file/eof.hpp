   struct eof final : public math_expr::ifunction<T>
   {
      using math_expr::ifunction<T>::operator();

      eof()
      : math_expr::ifunction<T>(1)
      { details::perform_check<T>(); }

      inline T operator() (const T& v) override
      {
         details::file_descriptor* fd = details::make_handle(v);
         return (fd->eof() ? T(1) : T(0));
      }
   };

   template <typename T>
