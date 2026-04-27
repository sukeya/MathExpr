#ifndef MATH_EXPR_TYPE_DEFS_HPP
#define MATH_EXPR_TYPE_DEFS_HPP

namespace math_expr
{
   template <typename T>
   struct type_defs
   {
      typedef symbol_table<T>         symbol_table_t;
      typedef expression<T>           expression_t;
      typedef parser<T>               parser_t;
      typedef parser_error::type      error_t;
      typedef function_compositor<T>  compositor_t;
      typedef typename compositor_t::function function_t;
   };

} // namespace math_expr

#endif
