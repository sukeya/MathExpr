   class read math_expr_final : public math_expr::igeneric_function<T>
   {
   public:

      typedef typename math_expr::igeneric_function<T> igfun_t;
      typedef typename igfun_t::parameter_list_t    parameter_list_t;
      typedef typename igfun_t::generic_type        generic_type;
      typedef typename generic_type::string_view    string_t;
      typedef typename generic_type::scalar_view    scalar_t;
      typedef typename generic_type::vector_view    vector_t;

      using igfun_t::operator();

      read()
      : igfun_t("TS|TST|TV|TVT")
      { details::perform_check<T>(); }

      inline T operator() (const std::size_t& ps_index, parameter_list_t parameters) math_expr_override
      {
         details::file_descriptor* fd = details::make_handle(scalar_t(parameters[0])());

         switch (ps_index)
         {
            case 0  : {
                         string_t buffer(parameters[1]);
                         const std::size_t amount = buffer.size();
                         return T(fd->read(buffer,amount) ? 1 : 0);
                      }

            case 1  : {
                         string_t buffer(parameters[1]);
                         const std::size_t amount =
                                  std::min(buffer.size(),
                                           static_cast<std::size_t>(scalar_t(parameters[2])()));
                         return T(fd->read(buffer,amount) ? 1 : 0);
                      }

            case 2  : {
                         vector_t vec(parameters[1]);
                         const std::size_t amount = vec.size();
                         return T(fd->read(vec,amount) ? 1 : 0);
                      }

            case 3  : {
                         vector_t vec(parameters[1]);
                         const std::size_t amount =
                                  std::min(vec.size(),
                                           static_cast<std::size_t>(scalar_t(parameters[2])()));
                         return T(fd->read(vec,amount) ? 1 : 0);
                      }
         }

         return T(0);
      }
   };

   template <typename T>
