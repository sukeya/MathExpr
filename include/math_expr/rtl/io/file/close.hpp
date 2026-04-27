   struct close math_expr_final : public math_expr::ifunction<T>
   {
      using math_expr::ifunction<T>::operator();

      close()
      : math_expr::ifunction<T>(1)
      { details::perform_check<T>(); }

      inline T operator() (const T& v) math_expr_override
      {
         details::file_descriptor* fd = details::make_handle(v);

         if (!fd->close())
            return T(0);

         delete fd;

         return T(1);
      }
   };

   template <typename T>
