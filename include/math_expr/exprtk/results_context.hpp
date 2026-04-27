   #ifndef exprtk_disable_return_statement
   namespace details
   {
      template <typename T> class return_node;
      template <typename T> class return_envelope_node;
   }
   #endif

   template <typename T>
   class results_context
   {
   public:

      typedef type_store<T> type_store_t;
      typedef typename type_store_t::scalar_view scalar_t;
      typedef typename type_store_t::vector_view vector_t;
      typedef typename type_store_t::string_view string_t;

      results_context()
      : results_available_(false)
      {}

      inline std::size_t count() const
      {
         if (results_available_)
            return parameter_list_.size();
         else
            return 0;
      }

      inline type_store_t& operator[](const std::size_t& index)
      {
         return parameter_list_[index];
      }

      inline const type_store_t& operator[](const std::size_t& index) const
      {
         return parameter_list_[index];
      }

      inline bool get_scalar(const std::size_t& index, T& out) const
      {
         if (
              (index < parameter_list_.size()) &&
              (parameter_list_[index].type == type_store_t::e_scalar)
            )
         {
            const scalar_t scalar(parameter_list_[index]);
            out = scalar();
            return true;
         }

         return false;
      }

      template <typename OutputIterator>
      inline bool get_vector(const std::size_t& index, OutputIterator out_itr) const
      {
         if (
              (index < parameter_list_.size()) &&
              (parameter_list_[index].type == type_store_t::e_vector)
            )
         {
            const vector_t vector(parameter_list_[index]);
            for (std::size_t i = 0; i < vector.size(); ++i)
            {
               *(out_itr++) = vector[i];
            }

            return true;
         }

         return false;
      }

      inline bool get_vector(const std::size_t& index, std::vector<T>& out) const
      {
         return get_vector(index,std::back_inserter(out));
      }

      inline bool get_string(const std::size_t& index, std::string& out) const
      {
         if (
              (index < parameter_list_.size()) &&
              (parameter_list_[index].type == type_store_t::e_string)
            )
         {
            const string_t str(parameter_list_[index]);
            out.assign(str.begin(),str.size());
            return true;
         }

         return false;
      }

   private:

      inline void clear()
      {
         results_available_ = false;
      }

      typedef std::vector<type_store_t> ts_list_t;
      typedef typename type_store_t::parameter_list parameter_list_t;

      inline void assign(const parameter_list_t& pl)
      {
         parameter_list_    = pl.parameter_list_;
         results_available_ = true;
      }

      bool results_available_;
      ts_list_t parameter_list_;

      #ifndef exprtk_disable_return_statement
      friend class details::return_node<T>;
      friend class details::return_envelope_node<T>;
      #endif
   };

