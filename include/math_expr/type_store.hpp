namespace math_expr
{
   template <typename T>
   struct type_store
   {
      enum store_type
      {
         e_unknown,
         e_scalar ,
         e_vector ,
         e_string
      };

      type_store()
      : data(0)
      , size(0)
      , type(e_unknown)
      , ivec(0)
      {}

      union
      {
         void* data;
         T*    vec_data;
      };

      typedef details::vector_interface<T>* ivec_t;

      std::size_t size;
      store_type  type;
      ivec_t      ivec;

      class parameter_list
      {
      public:

         explicit parameter_list(std::vector<type_store>& pl)
         : parameter_list_(pl)
         {}

         inline bool empty() const
         {
            return parameter_list_.empty();
         }

         inline std::size_t size() const
         {
            return parameter_list_.size();
         }

         inline type_store& operator[](const std::size_t& index)
         {
            return parameter_list_[index];
         }

         inline const type_store& operator[](const std::size_t& index) const
         {
            return parameter_list_[index];
         }

         inline type_store& front()
         {
            return parameter_list_[0];
         }

         inline const type_store& front() const
         {
            return parameter_list_[0];
         }

         inline type_store& back()
         {
            return parameter_list_.back();
         }

         inline const type_store& back() const
         {
            return parameter_list_.back();
         }

         inline typename std::vector<type_store>::const_iterator begin() const
         {
            return parameter_list_.begin();
         }

         inline typename std::vector<type_store>::const_iterator end() const
         {
            return parameter_list_.end();
         }

         inline typename std::vector<type_store>::iterator begin()
         {
            return parameter_list_.begin();
         }

         inline typename std::vector<type_store>::iterator end()
         {
            return parameter_list_.end();
         }

      private:

         std::vector<type_store>& parameter_list_;

         friend class results_context<T>;
      };

      template <typename ViewType>
      struct type_view
      {
         typedef type_store<T> type_store_t;
         typedef ViewType      value_t;

         explicit type_view(type_store_t& ts)
         : ts_(ts)
         , data_(reinterpret_cast<value_t*>(ts_.data))
         {}

         explicit type_view(const type_store_t& ts)
         : ts_(const_cast<type_store_t&>(ts))
         , data_(reinterpret_cast<value_t*>(ts_.data))
         {}

         inline std::size_t size() const
         {
            return ts_.size;
         }

         inline value_t& operator[](const std::size_t& i)
         {
            return data_[i];
         }

         inline const value_t& operator[](const std::size_t& i) const
         {
            return data_[i];
         }

         inline const value_t* begin() const { return data_; }
         inline       value_t* begin()       { return data_; }

         inline const value_t* end() const
         {
            return static_cast<value_t*>(data_ + ts_.size);
         }

         inline value_t* end()
         {
            return static_cast<value_t*>(data_ + ts_.size);
         }

         type_store_t& ts_;
         value_t* data_;
      };

      typedef type_view<T>    vector_view;
      typedef type_view<char> string_view;

      struct scalar_view
      {
         typedef type_store<T> type_store_t;
         typedef T value_t;

         explicit scalar_view(type_store_t& ts)
         : v_(*reinterpret_cast<value_t*>(ts.data))
         {}

         explicit scalar_view(const type_store_t& ts)
         : v_(*reinterpret_cast<value_t*>(const_cast<type_store_t&>(ts).data))
         {}

         inline value_t& operator() ()
         {
            return v_;
         }

         inline const value_t& operator() () const
         {
            return v_;
         }

         inline operator value_t() const
         {
            return v_;
         }

         inline operator value_t()
         {
            return v_;
         }

         template <typename IntType>
         inline bool to_int(IntType& i) const
         {
            if (!math_expr::details::numeric::is_integer(v_))
               return false;

            i = static_cast<IntType>(v_);

            return true;
         }

         template <typename UIntType>
         inline bool to_uint(UIntType& u) const
         {
            if (v_ < T(0))
               return false;
            else if (!math_expr::details::numeric::is_integer(v_))
               return false;

            u = static_cast<UIntType>(v_);

            return true;
         }

         T& v_;
      };
   };

   template <typename StringView>
   inline std::string to_str(const StringView& view)
   {
      return std::string(view.begin(),view.size());
   }

} // namespace math_expr
