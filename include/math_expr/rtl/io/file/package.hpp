namespace math_expr::rtl::io::file
{
   template <typename T>
   struct package
   {
      open   <T> o;
      close  <T> c;
      write  <T> w;
      read   <T> r;
      getline<T> g;
      eof    <T> e;

      bool register_package(math_expr::symbol_table<T>& symtab)
      {
         #define math_expr_register_function(FunctionName, FunctionType)                   \
         if (!symtab.add_function(FunctionName,FunctionType))                           \
         {                                                                              \
            math_expr_debug((                                                              \
              "math_expr::rtl::io::file::register_package - Failed to add function: %s\n", \
              FunctionName));                                                           \
            return false;                                                               \
         }                                                                              \

         math_expr_register_function("open"    , o)
         math_expr_register_function("close"   , c)
         math_expr_register_function("write"   , w)
         math_expr_register_function("read"    , r)
         math_expr_register_function("getline" , g)
         math_expr_register_function("eof"     , e)
         #undef math_expr_register_function

         return true;
      }
   };
} // math_expr::rtl::io::file
