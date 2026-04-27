namespace math_expr::details
{
      template <typename T>
      class conditional_node final : public expression_node<T>
      {
      public:

         typedef expression_node<T>* expression_ptr;
         typedef std::pair<expression_ptr,bool> branch_t;

         conditional_node(expression_ptr condition,
                          expression_ptr consequent,
                          expression_ptr alternative)
         {
            construct_branch_pair(condition_  , condition  );
            construct_branch_pair(consequent_ , consequent );
            construct_branch_pair(alternative_, alternative);
            assert(valid());
         }

         inline T value() const override
         {
            if (is_true(condition_))
               return consequent_.first->value();
            else
               return alternative_.first->value();
         }

         inline typename expression_node<T>::node_type type() const override
         {
            return expression_node<T>::e_conditional;
         }

         inline bool valid() const override
         {
            return
               condition_  .first && condition_  .first->valid() &&
               consequent_ .first && consequent_ .first->valid() &&
               alternative_.first && alternative_.first->valid() ;
         }

         void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) override
         {
            expression_node<T>::ndb_t::collect(condition_   , node_delete_list);
            expression_node<T>::ndb_t::collect(consequent_  , node_delete_list);
            expression_node<T>::ndb_t::collect(alternative_ , node_delete_list);
         }

         std::size_t node_depth() const override
         {
            return expression_node<T>::ndb_t::compute_node_depth
               (condition_, consequent_, alternative_);
         }

      private:

         branch_t condition_;
         branch_t consequent_;
         branch_t alternative_;
      };

      template <typename T>
      class cons_conditional_node final : public expression_node<T>
      {
      public:

         // Consequent only conditional statement node
         typedef expression_node<T>* expression_ptr;
         typedef std::pair<expression_ptr,bool> branch_t;

         cons_conditional_node(expression_ptr condition,
                               expression_ptr consequent)
         {
            construct_branch_pair(condition_ , condition );
            construct_branch_pair(consequent_, consequent);
            assert(valid());
         }

         inline T value() const override
         {
            if (is_true(condition_))
               return consequent_.first->value();
            else
               return std::numeric_limits<T>::quiet_NaN();
         }

         inline typename expression_node<T>::node_type type() const override
         {
            return expression_node<T>::e_conditional;
         }

         inline bool valid() const override
         {
            return
               condition_ .first && condition_ .first->valid() &&
               consequent_.first && consequent_.first->valid() ;
         }

         void collect_nodes(typename expression_node<T>::noderef_list_t& node_delete_list) override
         {
            expression_node<T>::ndb_t::collect(condition_  , node_delete_list);
            expression_node<T>::ndb_t::collect(consequent_ , node_delete_list);
         }

         std::size_t node_depth() const override
         {
            return expression_node<T>::ndb_t::
               compute_node_depth(condition_, consequent_);
         }

      private:

         branch_t condition_;
         branch_t consequent_;
      };

} // namespace math_expr::details
