   template <typename T>
   class ivararg_function : public function_traits
   {
   public:

      virtual ~ivararg_function()
      {}

      inline virtual T operator() (const std::vector<T>&)
      {
         math_expr_debug(("ivararg_function::operator() - Operator has not been overridden\n"));
         return std::numeric_limits<T>::quiet_NaN();
      }
   };

