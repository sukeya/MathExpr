#ifndef MATH_EXPR_ASSERT_CHECK_HPP
#define MATH_EXPR_ASSERT_CHECK_HPP

namespace math_expr
{
   struct assert_check
   {
      struct assert_context
      {
         std::string condition;
         std::string message;
         std::string id;
         std::size_t offet;
      };

      virtual ~assert_check()
      {}

      virtual void handle_assert(const assert_context& /*context*/)
      {}
   };

   typedef assert_check* assert_check_ptr;

} // namespace math_expr

#endif
