# Compilation Errors

[Back to index](index.md)

[Previous](20-expression-return-values.md) | [Index](index.md) | [Next](22-runtime-library-packages.md)

When attempting to compile a malformed or otherwise erroneous MathExpr expression, the compilation process will result in an error, as is indicated by the 'compile' method returning a false value. A diagnostic indicating the first error encountered and its cause can be obtained by invoking the 'error' method, as is demonstrated in the following example:

```text
if (!parser.compile(expression_string,expression))
{
   printf("Error: %s\n", parser.error().c_str());
   return false;
}
```

Any error(s) resulting from a failed compilation will be stored in the parser instance until the next time a compilation is performed. Before then errors can be enumerated in the order they occurred by invoking the 'get_error' method which itself will return a 'parser_error' type. A parser_error object will contain an error diagnostic, an error mode (or class), and the character position of the error in the expression string. The following example demonstrates the enumeration of error(s) in the event of a failed compilation.

```cpp
typedef math_expr::parser<T>          parser_t;
typedef math_expr::parser_error::type error_t;

if (!parser.compile(expression_string,expression))
{
   for (std::size_t i = 0; i < parser.error_count(); ++i)
   {
      typedef math_expr::parser_error::type error_t;

      error_t error = parser.get_error(i);

      printf("Error[%02d] Position: %02d Type: [%14s] Msg: %s\n",
             i,
             error.token.position,
             math_expr::parser_error::to_str(error.mode).c_str(),
             error.diagnostic.c_str());
   }

   return false;
}
```

Assuming the following expression '2 + (3 / log(1 + x))' which uses a variable named 'x' that has not been registered with the appropriate symbol_table instance and is not a locally defined variable, once compiled the above denoted post compilation error handling code shall produce the following output:

Error[00] Pos:17 Type:[Syntax] Msg: ERR184 - Undefined symbol: 'x'

For expressions comprised of multiple lines, the error position provided in the parser_error object can be converted into a pair of line and column numbers by invoking the 'update_error' function as is demonstrated by the following example:

```cpp
if (!parser.compile(program_str,expression))
{
   for (std::size_t i = 0; i < parser.error_count(); ++i)
   {
      typedef math_expr::parser_error::type error_t;

      error_t error = parser.get_error(i);

      math_expr::parser_error::update_error(error,program_str);

      printf("Error[%0lu] at line: %lu column: %lu\n",
             i,
             error.line_no,
             error.column_no);
   }

   return false;
}
```

**Note 32.** There are five distinct error modes in MathExpr which denote the class of an error. These classes are as follows:

1. `Syntax`
1. `Token`
1. `Numeric`
1. Symbol Table
1. `Lexer`

## Syntax Errors

These are errors related to invalid syntax found within the denoted expression. Examples are invalid sequences of operators and variables, incorrect number of parameters to functions, invalid conditional or loop structures and invalid use of keywords.

eg: 'for := sin(x,y,z) + 2 * equal > until[2 - x,3]'

## Token Errors

Errors in this class relate to token level errors detected by one or more of the following checkers:

1. Bracket Checker
1. Numeric Checker
1. Sequence Checker

## Numeric Errors

This class of error is related to conversion of numeric values from their string form to the underlying numerical type (float, double etc).

## Symbol Table Errors

This is the class of errors related to failures when interacting with the registered symbol_table instance. Errors such as not being able to find, within the symbol_table, symbols representing variables or functions, to being unable to create new variables in the symbol_table via the 'unknown symbol resolver' mechanism.

**Note 33.** The function compositor also supports error message handling similar to how it is done via the parser. The following demonstrates how after a failed function composition the associated errors can be enumerated.

```cpp
typedef math_expr::function_compositor<T>  compositor_t;
typedef typename compositor_t::function function_t;

compositor_t compositor;

const bool compositor_result =
compositor.add(
   function_t("foobar")
   .vars("x","y")
   .expression
   ( " x + y / z " ));

if (!compositor_result)
{
   printf("Error: %s\n", compositor.error().c_str());

   for (std::size_t i = 1; i < compositor.error_count(); ++i)
   {
      typedef math_expr::parser_error::type error_t;

      error_t error = compositor.get_error(i);

      printf("Err No.: %02d  Pos: %02d  Type: [%14s] Msg: %s\n",
             static_cast<unsigned int>(i),
             static_cast<unsigned int>(error.token.position),
             math_expr::parser_error::to_str(error.mode).c_str(),
             error.diagnostic.c_str());
   }
}
```

[Previous](20-expression-return-values.md) | [Index](index.md) | [Next](22-runtime-library-packages.md)
