namespace math_expr
{
   #ifdef math_expr_enable_debugging
     #define math_expr_debug(params) printf params
   #else
     #define math_expr_debug(params) (void)0
   #endif

   #define math_expr_error_location                     \
   std::string(__FILE__) + ":" + details::to_str(__LINE__) \

   #if __cplusplus >= 201103L
      #define math_expr_override override
      #define math_expr_final    final
      #define math_expr_delete   = delete
   #else
      #define math_expr_override
      #define math_expr_final
      #define math_expr_delete
   #endif

   #if __cplusplus >= 201603L
      #define math_expr_fallthrough [[fallthrough]];
   #elif (__cplusplus >= 201103L) && (defined(__GNUC__) && !defined(__clang__))
      #define math_expr_fallthrough [[gnu::fallthrough]];
   #else
      #ifndef _MSC_VER
         #define math_expr_fallthrough __attribute__ ((fallthrough));
      #else
         #define math_expr_fallthrough
      #endif
   #endif
