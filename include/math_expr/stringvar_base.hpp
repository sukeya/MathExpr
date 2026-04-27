   #ifndef math_expr_disable_string_capabilities
   template <typename T>
   class stringvar_base
   {
   public:

      typedef typename details::stringvar_node<T> stringvar_node_t;

      stringvar_base(const std::string& name, stringvar_node_t* svn)
      : name_(name)
      , string_varnode_(svn)
      {}

      bool valid() const
      {
         return !name_.empty() && (0 != string_varnode_);
      }

      std::string name() const
      {
         assert(string_varnode_);
         return name_;
      }

      void rebase(std::string& s)
      {
         assert(string_varnode_);
         string_varnode_->rebase(s);
      }

   private:

      std::string name_;
      stringvar_node_t* string_varnode_;
   };
   #endif

   template <typename T> class parser;
   template <typename T> class expression_helper;

