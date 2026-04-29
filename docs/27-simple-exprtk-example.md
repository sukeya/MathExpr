# Section 27: Simple Exprtk Example

[Back to index](index.md)

[Previous](26-exprtk-notes.md) | [Index](index.md) | [Next](28-build-options.md)

The following is a simple yet complete example demonstrating typical usage of the ExprTk Library. The example instantiates a symbol table object, adding to it three variables named x, y and z, and a custom user defined function, that accepts only two parameters, named myfunc. The example then proceeds to instantiate an expression object and register to it the symbol table instance.

A parser is then instantiated, and the string representation of the expression and the expression object are passed to the parser's compile method for compilation. If an error occurred during compilation, the compile method will return false, leading to a series of error diagnostics being printed to stdout. Otherwise the newly compiled expression is evaluated by invoking the expression object's value method, and subsequently printing the result of the computation to stdout.

--- snip --- #include <cstdio> #include <string>

#include "math_expr.hpp"

template <typename T> struct myfunc final : public math_expr::ifunction<T> {

```cpp
myfunc() : math_expr::ifunction<T>(2) {}

T operator()(const T& v1, const T& v2) override
{
   return T(1) + (v1 * v2) / T(3);
}
```

};

int main() {

```cpp
typedef math_expr::symbol_table<double> symbol_table_t;
typedef math_expr::expression<double>   expression_t;
typedef math_expr::parser<double>       parser_t;
typedef math_expr::parser_error::type   error_t;

const std::string expression_string =
   "z := 2 myfunc([4 + sin(x / pi)^3],y ^ 2)";

double x = 1.1;
double y = 2.2;
double z = 3.3;

myfunc<double> mf;

symbol_table_t symbol_table;
symbol_table.add_constants();
symbol_table.add_variable("x",x);
symbol_table.add_variable("y",y);
symbol_table.add_variable("z",z);
symbol_table.add_function("myfunc",mf);

expression_t expression;
expression.register_symbol_table(symbol_table);

parser_t parser;

if (!parser.compile(expression_string,expression))
{
   // A compilation error has occurred. Attempt to
   // print all errors to stdout.

   printf("Error: %s\tExpression: %s\n",
          parser.error().c_str(),
          expression_string.c_str());

   for (std::size_t i = 0; i < parser.error_count(); ++i)
   {
      // Include the specific nature of each error
      // and its position in the expression string.

      error_t error = parser.get_error(i);

      printf("Error: %02d Position: %02d "
             "Type: [%s] "
             "Message: %s "
             "Expression: %s\n",
             static_cast<int>(i),
             static_cast<int>(error.token.position),
             math_expr::parser_error::to_str(error.mode).c_str(),
             error.diagnostic.c_str(),
             expression_string.c_str());
   }

   return 1;
}

// Evaluate the expression and obtain its result.

double result = expression.value();

printf("Result: %10.5f\n",result);

return 0;
```

} --- snip ---

[Previous](26-exprtk-notes.md) | [Index](index.md) | [Next](28-build-options.md)
