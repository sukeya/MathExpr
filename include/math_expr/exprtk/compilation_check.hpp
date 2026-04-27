   struct compilation_check
   {
      struct compilation_context
      {
         std::string error_message;
      };

      virtual bool continue_compilation(compilation_context& /*context*/) = 0;

      virtual ~compilation_check()
      {}
   };

   typedef compilation_check* compilation_check_ptr;
