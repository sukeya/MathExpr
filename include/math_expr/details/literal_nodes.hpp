namespace math_expr::details
{
      template <typename T>
      class null_node final : public expression_node<T>
      {
      public:

         inline T value() const override
         {
            return std::numeric_limits<T>::quiet_NaN();
         }

         inline typename expression_node<T>::node_type type() const override
         {
            return expression_node<T>::e_null;
         }
      };

      template <typename T, std::size_t N>
      inline void construct_branch_pair(std::pair<expression_node<T>*,bool> (&branch)[N],
                                        expression_node<T>* b,
                                        const std::size_t& index)
      {
         if (b && (index < N))
         {
            branch[index] = std::make_pair(b,branch_deletable(b));
         }
      }

      template <typename T>
      inline void construct_branch_pair(std::pair<expression_node<T>*,bool>& branch, expression_node<T>* b)
      {
         if (b)
         {
            branch = std::make_pair(b,branch_deletable(b));
         }
      }

      template <std::size_t N, typename T>
      inline void init_branches(std::pair<expression_node<T>*,bool> (&branch)[N],
                                expression_node<T>* b0,
                                expression_node<T>* b1 = reinterpret_cast<expression_node<T>*>(0),
                                expression_node<T>* b2 = reinterpret_cast<expression_node<T>*>(0),
                                expression_node<T>* b3 = reinterpret_cast<expression_node<T>*>(0),
                                expression_node<T>* b4 = reinterpret_cast<expression_node<T>*>(0),
                                expression_node<T>* b5 = reinterpret_cast<expression_node<T>*>(0),
                                expression_node<T>* b6 = reinterpret_cast<expression_node<T>*>(0),
                                expression_node<T>* b7 = reinterpret_cast<expression_node<T>*>(0),
                                expression_node<T>* b8 = reinterpret_cast<expression_node<T>*>(0),
                                expression_node<T>* b9 = reinterpret_cast<expression_node<T>*>(0))
      {
         construct_branch_pair(branch, b0, 0);
         construct_branch_pair(branch, b1, 1);
         construct_branch_pair(branch, b2, 2);
         construct_branch_pair(branch, b3, 3);
         construct_branch_pair(branch, b4, 4);
         construct_branch_pair(branch, b5, 5);
         construct_branch_pair(branch, b6, 6);
         construct_branch_pair(branch, b7, 7);
         construct_branch_pair(branch, b8, 8);
         construct_branch_pair(branch, b9, 9);
      }

      template <typename T>
      class null_eq_node final : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef std::pair<expression_ptr,bool> branch_t;

         explicit null_eq_node(expression_ptr branch, const bool equality = true)
         : equality_(equality)
         {
            construct_branch_pair(branch_, branch);
            assert(valid());
         }

         inline T value() const override
         {
            const T v = branch_.first->value();
            const bool result = details::numeric::is_nan(v);

            if (result)
               return equality_ ? T(1) : T(0);
            else
               return equality_ ? T(0) : T(1);
         }

         inline typename expression_node<T>::node_type type() const override
         {
            return expression_node<T>::e_nulleq;
         }

         inline expression_node<T>* branch(const std::size_t&) const override
         {
            return branch_.first;
         }

         inline bool valid() const override
         {
            return branch_.first;
         }

         void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) override
         {
            expression_node<T>::ndb_t::collect(branch_, node_delete_list);
         }

         std::size_t node_depth() const override
         {
            return expression_node<T>::ndb_t::compute_node_depth(branch_);
         }

      private:

         bool equality_;
         branch_t branch_;
      };

      template <typename T>
      class literal_node final : public expression_node<T>
      {
      public:

         explicit literal_node(const T& v)
         : value_(v)
         {}

         inline T value() const override
         {
            return value_;
         }

         inline typename expression_node<T>::node_type type() const override
         {
            return expression_node<T>::e_constant;
         }

         inline expression_node<T>* branch(const std::size_t&) const override
         {
            return reinterpret_cast<expression_node<T>*>(0);
         }

      private:

         literal_node(const literal_node<T>&) = delete;
         literal_node<T>& operator=(const literal_node<T>&) = delete;

         const T value_;
      };

      template <typename T>
      struct range_pack;

      template <typename T>
      struct range_data_type;

      template <typename T>
      class range_interface
      {
      public:

         typedef range_pack<T> range_t;

         virtual ~range_interface()
         {}

         virtual range_t& range_ref() = 0;

         virtual const range_t& range_ref() const = 0;
      };

      #ifndef math_expr_disable_string_capabilities
      template <typename T>
      class string_base_node
      {
      public:

         typedef range_data_type<T> range_data_type_t;

         virtual ~string_base_node()
         {}

         virtual std::string str () const = 0;

         virtual char_cptr   base() const = 0;

         virtual std::size_t size() const = 0;
      };

      template <typename T>
      class string_literal_node final
                                : public expression_node <T>
                                , public string_base_node<T>
                                , public range_interface <T>
      {
      public:

         typedef range_pack<T> range_t;

         explicit string_literal_node(const std::string& v)
         : value_(v)
         {
            rp_.n0_c = std::make_pair<bool,std::size_t>(true, 0);
            rp_.n1_c = std::make_pair<bool,std::size_t>(true, v.size());
            rp_.cache.first  = rp_.n0_c.second;
            rp_.cache.second = rp_.n1_c.second;
         }

         inline T value() const override
         {
            return std::numeric_limits<T>::quiet_NaN();
         }

         inline typename expression_node<T>::node_type type() const override
         {
            return expression_node<T>::e_stringconst;
         }

         inline expression_node<T>* branch(const std::size_t&) const override
         {
            return reinterpret_cast<expression_node<T>*>(0);
         }

         std::string str() const override
         {
            return value_;
         }

         char_cptr base() const override
         {
            return value_.data();
         }

         std::size_t size() const override
         {
            return value_.size();
         }

         range_t& range_ref() override
         {
            return rp_;
         }

         const range_t& range_ref() const override
         {
            return rp_;
         }

      private:

         string_literal_node(const string_literal_node<T>&) = delete;
         string_literal_node<T>& operator=(const string_literal_node<T>&) = delete;

         const std::string value_;
         range_t rp_;
      };
      #endif


} // namespace math_expr::details
