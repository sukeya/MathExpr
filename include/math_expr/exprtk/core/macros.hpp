namespace exprtk
{
   #ifdef exprtk_enable_debugging
     #define exprtk_debug(params) printf params
   #else
     #define exprtk_debug(params) (void)0
   #endif

   #define exprtk_error_location                     \
   std::string(__FILE__) + ":" + details::to_str(__LINE__) \

   #if __cplusplus >= 201103L
      #define exprtk_override override
      #define exprtk_final    final
      #define exprtk_delete   = delete
   #else
      #define exprtk_override
      #define exprtk_final
      #define exprtk_delete
   #endif

   #if __cplusplus >= 201603L
      #define exprtk_fallthrough [[fallthrough]];
   #elif (__cplusplus >= 201103L) && (defined(__GNUC__) && !defined(__clang__))
      #define exprtk_fallthrough [[gnu::fallthrough]];
   #else
      #ifndef _MSC_VER
         #define exprtk_fallthrough __attribute__ ((fallthrough));
      #else
         #define exprtk_fallthrough
      #endif
   #endif
