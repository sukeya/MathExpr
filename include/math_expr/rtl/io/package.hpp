#ifndef MATH_EXPR_RTL_IO_PACKAGE_HPP
#define MATH_EXPR_RTL_IO_PACKAGE_HPP

namespace math_expr::rtl::io
{
   template <typename T>
   struct package
   {
      print  <T> p;
      println<T> pl;

      bool register_package(math_expr::symbol_table<T>& symtab)
      {
         #define math_expr_register_function(FunctionName, FunctionType)             \
         if (!symtab.add_function(FunctionName,FunctionType))                     \
         {                                                                        \
            math_expr_debug((                                                        \
              "math_expr::rtl::io::register_package - Failed to add function: %s\n", \
              FunctionName));                                                     \
            return false;                                                         \
         }                                                                        \

         math_expr_register_function("print"  , p )
         math_expr_register_function("println", pl)
         #undef math_expr_register_function

         return true;
      }
   };
} // math_expr::rtl::io

#endif
