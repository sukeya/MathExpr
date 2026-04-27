namespace math_expr
{
   class function_traits
   {
   public:

      function_traits()
      : allow_zero_parameters_(false)
      , has_side_effects_(true)
      , min_num_args_(0)
      , max_num_args_(std::numeric_limits<std::size_t>::max())
      {}

      inline bool& allow_zero_parameters()
      {
         return allow_zero_parameters_;
      }

      inline bool& has_side_effects()
      {
         return has_side_effects_;
      }

      std::size_t& min_num_args()
      {
         return min_num_args_;
      }

      std::size_t& max_num_args()
      {
         return max_num_args_;
      }

   private:

      bool allow_zero_parameters_;
      bool has_side_effects_;
      std::size_t min_num_args_;
      std::size_t max_num_args_;
   };

   template <typename FunctionType>
   void enable_zero_parameters(FunctionType& func)
   {
      func.allow_zero_parameters() = true;

      if (0 != func.min_num_args())
      {
         func.min_num_args() = 0;
      }
   }

   template <typename FunctionType>
   void disable_zero_parameters(FunctionType& func)
   {
      func.allow_zero_parameters() = false;
   }

   template <typename FunctionType>
   void enable_has_side_effects(FunctionType& func)
   {
      func.has_side_effects() = true;
   }

   template <typename FunctionType>
   void disable_has_side_effects(FunctionType& func)
   {
      func.has_side_effects() = false;
   }

   template <typename FunctionType>
   void set_min_num_args(FunctionType& func, const std::size_t& num_args)
   {
      func.min_num_args() = num_args;

      if ((0 != func.min_num_args()) && func.allow_zero_parameters())
         func.allow_zero_parameters() = false;
   }

   template <typename FunctionType>
   void set_max_num_args(FunctionType& func, const std::size_t& num_args)
   {
      func.max_num_args() = num_args;
   }

} // namespace math_expr
