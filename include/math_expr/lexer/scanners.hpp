#ifndef MATH_EXPR_LEXER_SCANNERS_HPP
#define MATH_EXPR_LEXER_SCANNERS_HPP

namespace math_expr::lexer
{
      class helper_interface
      {
      public:

         virtual void init()                     {              }
         virtual void reset()                    {              }
         virtual bool result()                   { return true; }
         virtual std::size_t process(generator&) { return 0;    }
         virtual ~helper_interface()             {              }
      };

      class token_scanner : public helper_interface
      {
      public:

         virtual ~token_scanner() override
         {}

         explicit token_scanner(const std::size_t& stride)
         : stride_(stride)
         {
            if (stride > 4)
            {
               throw std::invalid_argument("token_scanner() - Invalid stride value");
            }
         }

         inline std::size_t process(generator& g) override
         {
            if (g.token_list_.size() >= stride_)
            {
               for (std::size_t i = 0; i < (g.token_list_.size() - stride_ + 1); ++i)
               {
                  token t;

                  switch (stride_)
                  {
                     case 1 :
                              {
                                 const token& t0 = g.token_list_[i];

                                 if (!operator()(t0))
                                 {
                                    return 0;
                                 }
                              }
                              break;

                     case 2 :
                              {
                                 const token& t0 = g.token_list_[i    ];
                                 const token& t1 = g.token_list_[i + 1];

                                 if (!operator()(t0, t1))
                                 {
                                    return 0;
                                 }
                              }
                              break;

                     case 3 :
                              {
                                 const token& t0 = g.token_list_[i    ];
                                 const token& t1 = g.token_list_[i + 1];
                                 const token& t2 = g.token_list_[i + 2];

                                 if (!operator()(t0, t1, t2))
                                 {
                                    return 0;
                                 }
                              }
                              break;

                     case 4 :
                              {
                                 const token& t0 = g.token_list_[i    ];
                                 const token& t1 = g.token_list_[i + 1];
                                 const token& t2 = g.token_list_[i + 2];
                                 const token& t3 = g.token_list_[i + 3];

                                 if (!operator()(t0, t1, t2, t3))
                                 {
                                    return 0;
                                 }
                              }
                              break;

                     default: continue;
                  }
               }
            }

            return 0;
         }

         virtual bool operator() (const token&)
         {
            return false;
         }

         virtual bool operator() (const token&, const token&)
         {
            return false;
         }

         virtual bool operator() (const token&, const token&, const token&)
         {
            return false;
         }

         virtual bool operator() (const token&, const token&, const token&, const token&)
         {
            return false;
         }

      private:

         const std::size_t stride_;
      }; // class token_scanner

      class token_modifier : public helper_interface
      {
      public:

         inline std::size_t process(generator& g) override
         {
            std::size_t changes = 0;

            for (std::size_t i = 0; i < g.token_list_.size(); ++i)
            {
               if (modify(g.token_list_[i])) changes++;
            }

            return changes;
         }

         virtual bool modify(token& t) = 0;
      };

} // namespace math_expr::lexer

#endif
