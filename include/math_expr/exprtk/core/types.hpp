   namespace details
   {
      typedef char                   char_t;
      typedef char_t*                char_ptr;
      typedef char_t const*          char_cptr;
      typedef unsigned char          uchar_t;
      typedef uchar_t*               uchar_ptr;
      typedef uchar_t const*         uchar_cptr;
      typedef unsigned long long int _uint64_t;
      typedef long long int          _int64_t;

      inline bool is_whitespace(const char_t c)
      {
         return (' '  == c) || ('\n' == c) ||
                ('\r' == c) || ('\t' == c) ||
                ('\b' == c) || ('\v' == c) ||
                ('\f' == c) ;
      }

      inline bool is_operator_char(const char_t c)
      {
         return ('+' == c) || ('-' == c) ||
                ('*' == c) || ('/' == c) ||
                ('^' == c) || ('<' == c) ||
                ('>' == c) || ('=' == c) ||
                (',' == c) || ('!' == c) ||
                ('(' == c) || (')' == c) ||
                ('[' == c) || (']' == c) ||
                ('{' == c) || ('}' == c) ||
                ('%' == c) || (':' == c) ||
                ('?' == c) || ('&' == c) ||
                ('|' == c) || (';' == c) ;
      }

      inline bool is_letter(const char_t c)
      {
         return (('a' <= c) && (c <= 'z')) ||
                (('A' <= c) && (c <= 'Z')) ;
      }

      inline bool is_digit(const char_t c)
      {
         return ('0' <= c) && (c <= '9');
      }

      inline bool is_letter_or_digit(const char_t c)
      {
         return is_letter(c) || is_digit(c);
      }

      inline bool is_left_bracket(const char_t c)
      {
         return ('(' == c) || ('[' == c) || ('{' == c);
      }

      inline bool is_right_bracket(const char_t c)
      {
         return (')' == c) || (']' == c) || ('}' == c);
      }

      inline bool is_bracket(const char_t c)
      {
         return is_left_bracket(c) || is_right_bracket(c);
      }

      inline bool is_sign(const char_t c)
      {
         return ('+' == c) || ('-' == c);
      }

      inline bool is_invalid(const char_t c)
      {
         return !is_whitespace   (c) &&
                !is_operator_char(c) &&
                !is_letter       (c) &&
                !is_digit        (c) &&
                ('.'  != c)          &&
                ('_'  != c)          &&
                ('$'  != c)          &&
                ('~'  != c)          &&
                ('\'' != c);
      }

      inline bool is_valid_string_char(const char_t c)
      {
         return std::isprint(static_cast<uchar_t>(c)) ||
                is_whitespace(c);
      }

