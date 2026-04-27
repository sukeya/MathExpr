#ifndef MATH_EXPR_EXPRESSION_HELPER_HPP
#define MATH_EXPR_EXPRESSION_HELPER_HPP

namespace math_expr
{
   template <typename T>
   class expression_helper
   {
   public:

      enum node_types
      {
         e_literal,
         e_variable,
         e_string,
         e_unary,
         e_binary,
         e_function,
         e_vararg,
         e_null,
         e_assert,
         e_sf3ext,
         e_sf4ext
      };

      static inline bool is_literal(const expression<T>& expr)
      {
         return expr.control_block_ && details::is_literal_node(expr.control_block_->expr);
      }

      static inline bool is_variable(const expression<T>& expr)
      {
         return expr.control_block_ && details::is_variable_node(expr.control_block_->expr);
      }

      static inline bool is_string(const expression<T>& expr)
      {
         return expr.control_block_ && details::is_generally_string_node(expr.control_block_->expr);
      }

      static inline bool is_unary(const expression<T>& expr)
      {
         return expr.control_block_ && details::is_unary_node(expr.control_block_->expr);
      }

      static inline bool is_binary(const expression<T>& expr)
      {
         return expr.control_block_ && details::is_binary_node(expr.control_block_->expr);
      }

      static inline bool is_function(const expression<T>& expr)
      {
         return expr.control_block_ && details::is_function(expr.control_block_->expr);
      }

      static inline bool is_vararg(const expression<T>& expr)
      {
         return expr.control_block_ && details::is_vararg_node(expr.control_block_->expr);
      }

      static inline bool is_null(const expression<T>& expr)
      {
         return expr.control_block_ && details::is_null_node(expr.control_block_->expr);
      }

      static inline bool is_assert(const expression<T>& expr)
      {
         return expr.control_block_ && details::is_assert_node(expr.control_block_->expr);
      }

      static inline bool is_sf3ext(const expression<T>& expr)
      {
         return expr.control_block_ && details::is_sf3ext_node(expr.control_block_->expr);
      }

      static inline bool is_sf4ext(const expression<T>& expr)
      {
         return expr.control_block_ && details::is_sf4ext_node(expr.control_block_->expr);
      }

      static inline bool is_type(const expression<T>& expr, const node_types node_type)
      {
         if (0 == expr.control_block_)
         {
            return false;
         }

         switch (node_type)
         {
            case e_literal  : return is_literal_node(expr);
            case e_variable : return is_variable    (expr);
            case e_string   : return is_string      (expr);
            case e_unary    : return is_unary       (expr);
            case e_binary   : return is_binary      (expr);
            case e_function : return is_function    (expr);
            case e_null     : return is_null        (expr);
            case e_assert   : return is_assert      (expr);
            case e_sf3ext   : return is_sf3ext      (expr);
            case e_sf4ext   : return is_sf4ext      (expr);
         };

         return false;
      }

      static inline bool match_type_sequence(const expression<T>& expr, const std::vector<node_types>& type_seq)
      {
         if ((0 == expr.control_block_) || !is_vararg(expr))
         {
            return false;
         }

         typedef details::vararg_node<T, math_expr::details::vararg_multi_op<T> > mo_vararg_t;

         mo_vararg_t* vnode = dynamic_cast<mo_vararg_t*>(expr.control_block_->expr);

         if (
              (0 == vnode) ||
              type_seq.empty() ||
              (vnode->size() < type_seq.size())
            )
         {
            return false;
         }

         for (std::size_t i = 0; i < type_seq.size(); ++i)
         {
            assert((*vnode)[i]);

            switch (type_seq[i])
            {
               case e_literal  : { if (details::is_literal_node         ((*vnode)[i])) continue; } break;
               case e_variable : { if (details::is_variable_node        ((*vnode)[i])) continue; } break;
               case e_string   : { if (details::is_generally_string_node((*vnode)[i])) continue; } break;
               case e_unary    : { if (details::is_unary_node           ((*vnode)[i])) continue; } break;
               case e_binary   : { if (details::is_binary_node          ((*vnode)[i])) continue; } break;
               case e_function : { if (details::is_function             ((*vnode)[i])) continue; } break;
               case e_null     : { if (details::is_null_node            ((*vnode)[i])) continue; } break;
               case e_assert   : { if (details::is_assert_node          ((*vnode)[i])) continue; } break;
               case e_sf3ext   : { if (details::is_sf3ext_node          ((*vnode)[i])) continue; } break;
               case e_sf4ext   : { if (details::is_sf4ext_node          ((*vnode)[i])) continue; } break;
               case e_vararg   : break;
            }

            return false;
         }

         return true;
      }
   };

   template <typename T>
   inline bool is_valid(const expression<T>& expr)
   {
      return expr.control_block_ && !expression_helper<T>::is_null(expr);
   }

} // namespace math_expr

#endif
