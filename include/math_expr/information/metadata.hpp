namespace math_expr
{
   namespace information
   {
      using ::math_expr::details::char_cptr;

      static char_cptr library = "Mathematical Expression Toolkit";
      static char_cptr version = "2.71828182845904523536028747135266249775724"
                                 "7093699959574966967627724076630353547594571"
                                 "3821785251664274274663919320030599218174135"
                                 "9662904357290033429526059563073813232862794";
      static char_cptr date    = "20250101";
      static char_cptr min_cpp = "199711L";

      static inline std::string data()
      {
         static const std::string info_str = std::string(library) +
                                             std::string(" v") + std::string(version) +
                                             std::string(" (") + date + std::string(")") +
                                             std::string(" (") + min_cpp + std::string(")");
         return info_str;
      }

   } // namespace information

