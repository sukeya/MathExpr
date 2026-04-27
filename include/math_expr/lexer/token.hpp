#ifndef MATH_EXPR_LEXER_TOKEN_HPP
#define MATH_EXPR_LEXER_TOKEN_HPP

namespace math_expr::lexer
{
      struct token
      {
         enum token_type
         {
            e_none        =   0, e_error       =   1, e_err_symbol  =   2,
            e_err_number  =   3, e_err_string  =   4, e_err_sfunc   =   5,
            e_eof         =   6, e_number      =   7, e_symbol      =   8,
            e_string      =   9, e_assign      =  10, e_addass      =  11,
            e_subass      =  12, e_mulass      =  13, e_divass      =  14,
            e_modass      =  15, e_shr         =  16, e_shl         =  17,
            e_lte         =  18, e_ne          =  19, e_gte         =  20,
            e_swap        =  21, e_lt          = '<', e_gt          = '>',
            e_eq          = '=', e_rbracket    = ')', e_lbracket    = '(',
            e_rsqrbracket = ']', e_lsqrbracket = '[', e_rcrlbracket = '}',
            e_lcrlbracket = '{', e_comma       = ',', e_add         = '+',
            e_sub         = '-', e_div         = '/', e_mul         = '*',
            e_mod         = '%', e_pow         = '^', e_colon       = ':',
            e_ternary     = '?'
         };

         token()
         : type(e_none)
         , value("")
         , position(std::numeric_limits<std::size_t>::max())
         {}

         void clear()
         {
            type     = e_none;
            value    = "";
            position = std::numeric_limits<std::size_t>::max();
         }

         template <typename Iterator>
         inline token& set_operator(const token_type tt,
                                    const Iterator begin, const Iterator end,
                                    const Iterator base_begin = Iterator(0))
         {
            type = tt;
            value.assign(begin,end);
            if (base_begin)
               position = static_cast<std::size_t>(std::distance(base_begin,begin));
            return (*this);
         }

         template <typename Iterator>
         inline token& set_symbol(const Iterator begin, const Iterator end, const Iterator base_begin = Iterator(0))
         {
            type = e_symbol;
            value.assign(begin,end);
            if (base_begin)
               position = static_cast<std::size_t>(std::distance(base_begin,begin));
            return (*this);
         }

         template <typename Iterator>
         inline token& set_numeric(const Iterator begin, const Iterator end, const Iterator base_begin = Iterator(0))
         {
            type = e_number;
            value.assign(begin,end);
            if (base_begin)
               position = static_cast<std::size_t>(std::distance(base_begin,begin));
            return (*this);
         }

         template <typename Iterator>
         inline token& set_string(const Iterator begin, const Iterator end, const Iterator base_begin = Iterator(0))
         {
            type = e_string;
            value.assign(begin,end);
            if (base_begin)
               position = static_cast<std::size_t>(std::distance(base_begin,begin));
            return (*this);
         }

         inline token& set_string(const std::string& s, const std::size_t p)
         {
            type     = e_string;
            value    = s;
            position = p;
            return (*this);
         }

         template <typename Iterator>
         inline token& set_error(const token_type et,
                                 const Iterator begin, const Iterator end,
                                 const Iterator base_begin = Iterator(0))
         {
            if (
                 (e_error      == et) ||
                 (e_err_symbol == et) ||
                 (e_err_number == et) ||
                 (e_err_string == et) ||
                 (e_err_sfunc  == et)
               )
            {
               type = et;
            }
            else
               type = e_error;

            value.assign(begin,end);

            if (base_begin)
               position = static_cast<std::size_t>(std::distance(base_begin,begin));

            return (*this);
         }

         static inline std::string to_str(const token_type t)
         {
            switch (t)
            {
               case e_none        : return "NONE";
               case e_error       : return "ERROR";
               case e_err_symbol  : return "ERROR_SYMBOL";
               case e_err_number  : return "ERROR_NUMBER";
               case e_err_string  : return "ERROR_STRING";
               case e_eof         : return "EOF";
               case e_number      : return "NUMBER";
               case e_symbol      : return "SYMBOL";
               case e_string      : return "STRING";
               case e_assign      : return ":=";
               case e_addass      : return "+=";
               case e_subass      : return "-=";
               case e_mulass      : return "*=";
               case e_divass      : return "/=";
               case e_modass      : return "%=";
               case e_shr         : return ">>";
               case e_shl         : return "<<";
               case e_lte         : return "<=";
               case e_ne          : return "!=";
               case e_gte         : return ">=";
               case e_lt          : return "<";
               case e_gt          : return ">";
               case e_eq          : return "=";
               case e_rbracket    : return ")";
               case e_lbracket    : return "(";
               case e_rsqrbracket : return "]";
               case e_lsqrbracket : return "[";
               case e_rcrlbracket : return "}";
               case e_lcrlbracket : return "{";
               case e_comma       : return ",";
               case e_add         : return "+";
               case e_sub         : return "-";
               case e_div         : return "/";
               case e_mul         : return "*";
               case e_mod         : return "%";
               case e_pow         : return "^";
               case e_colon       : return ":";
               case e_ternary     : return "?";
               case e_swap        : return "<=>";
               default            : return "UNKNOWN";
            }
         }

         static inline std::string seperator_to_str(const token_type t)
         {
            switch (t)
            {
               case e_comma : return ",";
               case e_colon : return ":";
               case e_eof   : return ";";
               default      : return "UNKNOWN";
            }

            #if !defined(_MSC_VER) && !defined(__NVCOMPILER)
            return "UNKNOWN";
            #endif
         }

         inline bool is_error() const
         {
            return (e_error      == type) ||
                   (e_err_symbol == type) ||
                   (e_err_number == type) ||
                   (e_err_string == type) ||
                   (e_err_sfunc  == type) ;
         }

         token_type type;
         std::string value;
         std::size_t position;
      };

} // namespace math_expr::lexer

#endif
