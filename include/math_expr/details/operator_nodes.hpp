namespace math_expr::details
{
      template <typename T>
      class unary_node : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef std::pair<expression_ptr,bool> branch_t;

         unary_node(const operator_type& opr, expression_ptr branch)
         : operation_(opr)
         {
            construct_branch_pair(branch_,branch);
            assert(valid());
         }

         inline T value() const override
         {
            return numeric::process<T>
                     (operation_,branch_.first->value());
         }

         inline typename expression_node<T>::node_type type() const override
         {
            return expression_node<T>::e_unary;
         }

         inline operator_type operation()
         {
            return operation_;
         }

         inline expression_node<T>* branch(const std::size_t&) const override
         {
            return branch_.first;
         }

         inline bool valid() const override
         {
            return branch_.first && branch_.first->valid();
         }

         inline void release()
         {
            branch_.second = false;
         }

         void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) override
         {
            expression_node<T>::ndb_t::collect(branch_, node_delete_list);
         }

         std::size_t node_depth() const final
         {
            return expression_node<T>::ndb_t::compute_node_depth(branch_);
         }

      private:

         operator_type operation_;
         branch_t branch_;
      };

      template <typename T>
      class binary_node : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef std::pair<expression_ptr,bool> branch_t;

         binary_node(const operator_type& opr,
                     expression_ptr branch0,
                     expression_ptr branch1)
         : operation_(opr)
         {
            init_branches<2>(branch_, branch0, branch1);
            assert(valid());
         }

         inline T value() const override
         {
            return numeric::process<T>
                   (
                      operation_,
                      branch_[0].first->value(),
                      branch_[1].first->value()
                   );
         }

         inline typename expression_node<T>::node_type type() const override
         {
            return expression_node<T>::e_binary;
         }

         inline operator_type operation()
         {
            return operation_;
         }

         inline expression_node<T>* branch(const std::size_t& index = 0) const override
         {
            assert(index < 2);
            return branch_[index].first;
         }

         inline bool valid() const override
         {
            return
               branch_[0].first && branch_[0].first->valid() &&
               branch_[1].first && branch_[1].first->valid() ;
         }

         void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) override
         {
            expression_node<T>::ndb_t::collect(branch_, node_delete_list);
         }

         std::size_t node_depth() const final
         {
            return expression_node<T>::ndb_t::template compute_node_depth<2>(branch_);
         }

      private:

         operator_type operation_;
         branch_t branch_[2];
      };

      template <typename T, typename Operation>
      class binary_ext_node final : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef std::pair<expression_ptr,bool> branch_t;

         binary_ext_node(expression_ptr branch0, expression_ptr branch1)
         {
            init_branches<2>(branch_, branch0, branch1);
            assert(valid());
         }

         inline T value() const override
         {
            const T arg0 = branch_[0].first->value();
            const T arg1 = branch_[1].first->value();
            return Operation::process(arg0,arg1);
         }

         inline typename expression_node<T>::node_type type() const override
         {
            return expression_node<T>::e_binary_ext;
         }

         inline operator_type operation()
         {
            return Operation::operation();
         }

         inline expression_node<T>* branch(const std::size_t& index = 0) const override
         {
            assert(index < 2);
            return branch_[index].first;
         }

         inline bool valid() const override
         {
            return
               branch_[0].first && branch_[0].first->valid() &&
               branch_[1].first && branch_[1].first->valid() ;
         }

         void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) override
         {
            expression_node<T>::ndb_t::collect(branch_, node_delete_list);
         }

         std::size_t node_depth() const override
         {
            return expression_node<T>::ndb_t::template compute_node_depth<2>(branch_);
         }

      protected:

         branch_t branch_[2];
      };

      template <typename T>
      class trinary_node : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef std::pair<expression_ptr,bool> branch_t;

         trinary_node(const operator_type& opr,
                      expression_ptr branch0,
                      expression_ptr branch1,
                      expression_ptr branch2)
         : operation_(opr)
         {
            init_branches<3>(branch_, branch0, branch1, branch2);
            assert(valid());
         }

         inline T value() const override
         {
            const T arg0 = branch_[0].first->value();
            const T arg1 = branch_[1].first->value();
            const T arg2 = branch_[2].first->value();

            switch (operation_)
            {
               case e_inrange : return (arg1 < arg0) ? T(0) : ((arg1 > arg2) ? T(0) : T(1));

               case e_clamp   : return (arg1 < arg0) ? arg0 : (arg1 > arg2 ? arg2 : arg1);

               case e_iclamp  : if ((arg1 <= arg0) || (arg1 >= arg2))
                                   return arg1;
                                else
                                   return ((T(2) * arg1  <= (arg2 + arg0)) ? arg0 : arg2);

               default        : math_expr_debug(("trinary_node::value() - Error: Invalid operation\n"));
                                return std::numeric_limits<T>::quiet_NaN();
            }
         }

         inline typename expression_node<T>::node_type type() const override
         {
            return expression_node<T>::e_trinary;
         }

         inline bool valid() const override
         {
            return
               branch_[0].first && branch_[0].first->valid() &&
               branch_[1].first && branch_[1].first->valid() &&
               branch_[2].first && branch_[2].first->valid() ;
         }

         void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) override
         {
            expression_node<T>::ndb_t::collect(branch_, node_delete_list);
         }

         std::size_t node_depth() const override final
         {
            return expression_node<T>::ndb_t::template compute_node_depth<3>(branch_);
         }

      protected:

         operator_type operation_;
         branch_t branch_[3];
      };

      template <typename T>
      class quaternary_node : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef std::pair<expression_ptr,bool> branch_t;

         quaternary_node(const operator_type& opr,
                         expression_ptr branch0,
                         expression_ptr branch1,
                         expression_ptr branch2,
                         expression_ptr branch3)
         : operation_(opr)
         {
            init_branches<4>(branch_, branch0, branch1, branch2, branch3);
         }

         inline T value() const override
         {
            return std::numeric_limits<T>::quiet_NaN();
         }

         inline typename expression_node<T>::node_type type() const override
         {
            return expression_node<T>::e_quaternary;
         }

         void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) override
         {
            expression_node<T>::ndb_t::collect(branch_, node_delete_list);
         }

         std::size_t node_depth() const override final
         {
            return expression_node<T>::ndb_t::template compute_node_depth<4>(branch_);
         }

         inline bool valid() const override
         {
            return
               branch_[0].first && branch_[0].first->valid() &&
               branch_[1].first && branch_[1].first->valid() &&
               branch_[2].first && branch_[2].first->valid() &&
               branch_[3].first && branch_[3].first->valid() ;
         }

      protected:

         operator_type operation_;
         branch_t branch_[4];
      };


} // namespace math_expr::details
