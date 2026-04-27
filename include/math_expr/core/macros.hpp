#ifndef MATH_EXPR_CORE_MACROS_HPP
#define MATH_EXPR_CORE_MACROS_HPP

   #ifdef math_expr_enable_debugging
     #define math_expr_debug(params) printf params
   #else
     #define math_expr_debug(params) (void)0
   #endif

   #define math_expr_error_location \
   std::string(__FILE__) + ":" + details::to_str(__LINE__)

#endif
