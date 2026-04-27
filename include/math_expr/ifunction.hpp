namespace math_expr
{
   template <typename T>
   class ifunction : public function_traits
   {
   public:

      explicit ifunction(const std::size_t& pc)
      : param_count(pc)
      {}

      virtual ~ifunction()
      {}

      #define empty_method_body(N)                   \
      {                                              \
         math_expr_debug(("ifunction::operator() - Operator(" #N ") has not been overridden\n")); \
         return std::numeric_limits<T>::quiet_NaN(); \
      }                                              \

      inline virtual T operator() ()
      empty_method_body(0)

      inline virtual T operator() (const T&)
      empty_method_body(1)

      inline virtual T operator() (const T&,const T&)
      empty_method_body(2)

      inline virtual T operator() (const T&, const T&, const T&)
      empty_method_body(3)

      inline virtual T operator() (const T&, const T&, const T&, const T&)
      empty_method_body(4)

      inline virtual T operator() (const T&, const T&, const T&, const T&, const T&)
      empty_method_body(5)

      inline virtual T operator() (const T&, const T&, const T&, const T&, const T&, const T&)
      empty_method_body(6)

      inline virtual T operator() (const T&, const T&, const T&, const T&, const T&, const T&, const T&)
      empty_method_body(7)

      inline virtual T operator() (const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&)
      empty_method_body(8)

      inline virtual T operator() (const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&)
      empty_method_body(9)

      inline virtual T operator() (const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&)
      empty_method_body(10)

      inline virtual T operator() (const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&,
                                   const T&)
      empty_method_body(11)

      inline virtual T operator() (const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&,
                                   const T&, const T&)
      empty_method_body(12)

      inline virtual T operator() (const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&,
                                   const T&, const T&, const T&)
      empty_method_body(13)

      inline virtual T operator() (const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&,
                                   const T&, const T&, const T&, const T&)
      empty_method_body(14)

      inline virtual T operator() (const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&,
                                   const T&, const T&, const T&, const T&, const T&)
      empty_method_body(15)

      inline virtual T operator() (const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&,
                                   const T&, const T&, const T&, const T&, const T&, const T&)
      empty_method_body(16)

      inline virtual T operator() (const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&,
                                   const T&, const T&, const T&, const T&, const T&, const T&, const T&)
      empty_method_body(17)

      inline virtual T operator() (const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&,
                                   const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&)
      empty_method_body(18)

      inline virtual T operator() (const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&,
                                   const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&)
      empty_method_body(19)

      inline virtual T operator() (const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&,
                                   const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&, const T&)
      empty_method_body(20)

      #undef empty_method_body

      std::size_t param_count;
   };

} // namespace math_expr
