namespace math_expr::rtl::io::file
{
   template <typename T>
   class getline final : public math_expr::igeneric_function<T>
   {
   public:

      typedef typename math_expr::igeneric_function<T> igfun_t;
      typedef typename igfun_t::parameter_list_t    parameter_list_t;
      typedef typename igfun_t::generic_type        generic_type;
      typedef typename generic_type::string_view    string_t;
      typedef typename generic_type::scalar_view    scalar_t;

      using igfun_t::operator();

      getline()
      : igfun_t("T",igfun_t::e_rtrn_string)
      { details::perform_check<T>(); }

      inline T operator() (std::string& result, parameter_list_t parameters) override
      {
         details::file_descriptor* fd = details::make_handle(scalar_t(parameters[0])());
         return T(fd->getline(result) ? 1 : 0);
      }
   };


} // namespace math_expr::rtl::io::file
