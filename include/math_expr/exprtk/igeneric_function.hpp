   template <typename T>
   class igeneric_function : public function_traits
   {
   public:

      enum return_type
      {
         e_rtrn_scalar   = 0,
         e_rtrn_string   = 1,
         e_rtrn_overload = 2
      };

      typedef T type;
      typedef type_store<T> generic_type;
      typedef typename generic_type::parameter_list parameter_list_t;

      explicit igeneric_function(const std::string& param_seq = "", const return_type rtr_type = e_rtrn_scalar)
      : parameter_sequence(param_seq)
      , rtrn_type(rtr_type)
      {}

      virtual ~igeneric_function()
      {}

      #define igeneric_function_empty_body(N)        \
      {                                              \
         exprtk_debug(("igeneric_function::operator() - Operator(" #N ") has not been overridden\n")); \
         return std::numeric_limits<T>::quiet_NaN(); \
      }                                              \

      // f(i_0,i_1,....,i_N) --> Scalar
      inline virtual T operator() (parameter_list_t)
      igeneric_function_empty_body(1)

      // f(i_0,i_1,....,i_N) --> String
      inline virtual T operator() (std::string&, parameter_list_t)
      igeneric_function_empty_body(2)

      // f(psi,i_0,i_1,....,i_N) --> Scalar
      inline virtual T operator() (const std::size_t&, parameter_list_t)
      igeneric_function_empty_body(3)

      // f(psi,i_0,i_1,....,i_N) --> String
      inline virtual T operator() (const std::size_t&, std::string&, parameter_list_t)
      igeneric_function_empty_body(4)

      #undef igeneric_function_empty_body

      std::string parameter_sequence;
      return_type rtrn_type;

      static inline std::string generate_prefix_args(const std::string& prefix_args, std::size_t start = 0, std::size_t end = 10)
      {
         std::string result;

         for (std::size_t i = start; i <= end; ++i)
         {
            result += prefix_args + std::string(i,'?');
            result += (i != end) ? "|" : "";
         }

         return result;
      }

      static inline std::string generate_suffix_args(const std::string& suffix_args, std::size_t start = 0, std::size_t end = 10)
      {
         std::string result;

         for (std::size_t i = start; i <= end; ++i)
         {
            result += std::string(i,'?') + suffix_args;
            result += (i != end) ? "|" : "";
         }

         return result;
      }
   };

