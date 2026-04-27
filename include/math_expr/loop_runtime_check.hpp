namespace math_expr
{
   struct loop_runtime_check
   {
      enum loop_types
      {
         e_invalid           = 0,
         e_for_loop          = 1,
         e_while_loop        = 2,
         e_repeat_until_loop = 4,
         e_all_loops         = 7
      };

      enum violation_type
      {
          e_unknown         = 0,
          e_iteration_count = 1,
          e_timeout         = 2
      };

      loop_types loop_set;

      loop_runtime_check()
      : loop_set(e_invalid)
      , max_loop_iterations(0)
      {}

      details::_uint64_t max_loop_iterations;

      struct violation_context
      {
         loop_types loop;
         violation_type violation;
         details::_uint64_t iteration_count;
      };

      virtual bool check()
      {
         return true;
      }

      virtual void handle_runtime_violation(const violation_context&)
      {
         throw std::runtime_error("ExprTk Loop runtime violation.");
      }

      virtual ~loop_runtime_check()
      {}
   };

   typedef loop_runtime_check* loop_runtime_check_ptr;

} // namespace math_expr
