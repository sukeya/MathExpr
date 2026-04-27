namespace math_expr::rtl::io::file
{
   template <typename T>
   struct close final : public math_expr::ifunction<T>
   {
      using math_expr::ifunction<T>::operator();

      close()
      : math_expr::ifunction<T>(1)
      { details::perform_check<T>(); }

      inline T operator() (const T& v) override
      {
         details::file_descriptor* fd = details::make_handle(v);

         if (!fd->close())
            return T(0);

         delete fd;

         return T(1);
      }
   };


} // namespace math_expr::rtl::io::file
