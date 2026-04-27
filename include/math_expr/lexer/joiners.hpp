#ifndef MATH_EXPR_LEXER_JOINERS_HPP
#define MATH_EXPR_LEXER_JOINERS_HPP

namespace math_expr::lexer
{
      class token_joiner : public helper_interface
      {
      public:

         explicit token_joiner(const std::size_t& stride)
         : stride_(stride)
         {}

         inline std::size_t process(generator& g) override
         {
            if (g.token_list_.empty())
               return 0;

            switch (stride_)
            {
               case 2  : return process_stride_2(g);
               case 3  : return process_stride_3(g);
               default : return 0;
            }
         }

         virtual bool join(const token&, const token&, token&)               { return false; }
         virtual bool join(const token&, const token&, const token&, token&) { return false; }

      private:

         inline std::size_t process_stride_2(generator& g)
         {
            if (g.token_list_.size() < 2)
               return 0;

            std::size_t changes = 0;

            generator::token_list_t token_list;
            token_list.reserve(10000);

            for (int i = 0; i < static_cast<int>(g.token_list_.size() - 1); ++i)
            {
               token t;

               for ( ; ; )
               {
                  if (!join(g[i], g[i + 1], t))
                  {
                     token_list.push_back(g[i]);
                     break;
                  }

                  token_list.push_back(t);

                  ++changes;

                  i += 2;

                  if (static_cast<std::size_t>(i) >= (g.token_list_.size() - 1))
                     break;
               }
            }

            token_list.push_back(g.token_list_.back());

            assert(token_list.size() <= g.token_list_.size());

            std::swap(token_list, g.token_list_);

            return changes;
         }

         inline std::size_t process_stride_3(generator& g)
         {
            if (g.token_list_.size() < 3)
               return 0;

            std::size_t changes = 0;

            generator::token_list_t token_list;
            token_list.reserve(10000);

            for (int i = 0; i < static_cast<int>(g.token_list_.size() - 2); ++i)
            {
               token t;

               for ( ; ; )
               {
                  if (!join(g[i], g[i + 1], g[i + 2], t))
                  {
                     token_list.push_back(g[i]);
                     break;
                  }

                  token_list.push_back(t);

                  ++changes;

                  i += 3;

                  if (static_cast<std::size_t>(i) >= (g.token_list_.size() - 2))
                     break;
               }
            }

            token_list.push_back(*(g.token_list_.begin() + g.token_list_.size() - 2));
            token_list.push_back(*(g.token_list_.begin() + g.token_list_.size() - 1));

            assert(token_list.size() <= g.token_list_.size());

            std::swap(token_list, g.token_list_);

            return changes;
         }

         const std::size_t stride_;
      };

} // namespace math_expr::lexer

#endif
