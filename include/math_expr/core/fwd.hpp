#ifndef MATH_EXPR_CORE_FWD_HPP
#define MATH_EXPR_CORE_FWD_HPP

// SPDX-License-Identifier: MIT

namespace math_expr
{
   template <typename T> class parser;
   template <typename T> class expression;
   template <typename T> class symbol_table;
   template <typename T> class function_compositor;
   template <typename T> class results_context;
   template <typename T> class vector_view;
   template <typename T> class ifunction;
   template <typename T> class ivararg_function;
   template <typename T> class igeneric_function;

   namespace details
   {
      template <typename T> class expression_node;
      template <typename T> class variable_node;
      template <typename T> class stringvar_node;
      template <typename T> class vector_holder;
      template <typename T> class vector_interface;
      template <typename T> class null_node;
      template <typename T> class literal_node;
      template <typename T> class unary_node;
      template <typename T> class binary_node;
      template <typename T> class trinary_node;
      template <typename T> class quaternary_node;
      template <typename T> class conditional_node;
      template <typename T> class cons_conditional_node;
      template <typename T> class while_loop_node;
      template <typename T> class while_loop_rtc_node;
      template <typename T> class repeat_until_loop_node;
      template <typename T> class repeat_until_loop_rtc_node;
      template <typename T> class for_loop_node;
      template <typename T> class for_loop_rtc_node;
      template <typename T> class vector_elem_node;
      template <typename T> class vector_celem_node;
      template <typename T> class return_node;
      template <typename T> class return_envelope_node;
   }
}

#endif
