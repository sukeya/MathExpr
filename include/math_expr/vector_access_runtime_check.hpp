namespace math_expr
{
   struct vector_access_runtime_check
   {
      struct violation_context
      {
         void* base_ptr;
         void* end_ptr;
         void* access_ptr;
         std::size_t type_size;
      };

      virtual ~vector_access_runtime_check()
      {}

      virtual bool handle_runtime_violation(violation_context& /*context*/)
      {
         throw std::runtime_error("ExprTk runtime vector access violation.");
         #if !defined(_MSC_VER) && !defined(__NVCOMPILER)
         return false;
         #endif
      }
   };

   typedef vector_access_runtime_check* vector_access_runtime_check_ptr;

} // namespace math_expr
