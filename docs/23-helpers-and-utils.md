# Helpers & Utils

[Back to index](index.md)

[Previous](22-runtime-library-packages.md) | [Index](index.md) | [Next](24-runtime-checks.md)

The MathExpr library provides a series of usage simplifications via helper routines that combine various processes into a single 'function call' making certain actions easier to carry out though not necessarily in the most efficient way possible. A list of the routines are as follows:

1. `collect_variables`
1. `collect_functions`
1. `compute`
1. `integrate`
1. `derivative`
1. `second_derivative`
1. `third_derivative`

## collect_variables

This function will collect all the variable symbols in a given string representation of an expression and return them in an STL compatible sequence data structure (eg: std::vector, dequeue etc) specialised upon a std::string type. If an error occurs during the parsing of the expression then the return value of the function will be false, otherwise it will be true. An example use of the given routine is as follows:

```cpp
const std::string expression = "x + abs(y / z)";

std::vector<std::string> variable_list;

if (math_expr::collect_variables(expression, variable_list))
{
   for (const auto& var : variable_list)
   {
      ...
   }
}
else
  printf("An error occurred.");
```

## collect_functions

This function will collect all the function symbols in a given string representation of an expression and return them in an STL compatible sequence data structure (eg: std::vector, dequeue etc) specialised upon a std::string type. If an error occurs during the parsing of the expression then the return value of the function will be false, otherwise it will be true. An example use of the given routine is as follows:

```cpp
const std::string expression = "x + abs(y / cos(1 + z))";

std::deque<std::string> function_list;

if (math_expr::collect_functions(expression, function_list))
{
   for (const auto& func : function_list)
   {
      ...
   }
}
else
  printf("An error occurred.");
```

**Note 34.** When either the 'collect_variables' or 'collect_functions' free functions return true - that does not necessarily indicate the expression itself is valid. It is still possible that when compiled the expression may have certain 'type' related errors - though it is highly likely that no semantic errors will occur if either return true.

**Note 35.** The default interface provided for both the collect_variables and collect_functions free_functions, assumes that expressions will only be utilising the MathExpr reserved functions (eg: abs, cos, min etc). When user defined functions are to be used in an expression, a symbol_table instance containing said functions can be passed to either routine, and will be incorporated during the compilation and Dependent Entity Collection processes. In the following example, a user defined free function named 'foo' is registered with a symbol_table. Finally the symbol_table instance and associated expression string are passed to the math_expr::collect_functions routine.

```cpp
template <typename T>
T foo(T v)
{
   return std::abs(v + T(2)) / T(3);
}

......

math_expr::symbol_table<T> sym_tab;

symbol_table.add_function("foo",foo);

const std::string expression = "x + foo(y / cos(1 + z))";

std::deque<std::string> function_list;

if (math_expr::collect_functions(expression, sym_tab, function_list))
{
   for (const auto& func : function_list)
   {
      ...
   }
}
else
  printf("An error occurred.");
```

## compute

This free function will compute the value of an expression from its string form. If an invalid expression is passed, the result of the function will be false indicating an error, otherwise the return value will be true indicating success. The compute function has three overloads, the definitions of which are:

1. No variables
1. One variable called x
1. Two variables called x and y
1. Three variables called x, y and z

Example uses of each of the three overloads for the compute routine are as follows:

```cpp
T result = T(0);

// No variables overload
const std::string no_vars = "abs(1 - (3 / pi)) * 5";

if (!math_expr::compute(no_vars,result))
   printf("Failed to compute: %s",no_vars.c_str());
else
   printf("Result: %15.5f\n",result);

// One variable 'x' overload
T x = T(123.456);

const std::string one_var = "abs(x - (3 / pi)) * 5";

if (!math_expr::compute(one_var, x, result))
   printf("Failed to compute: %s",one_var.c_str());
else
   printf("Result: %15.5f\n",result);

// Two variables 'x' and 'y' overload
T y = T(789.012);

const std::string two_var = "abs(x - (y / pi)) * 5";

if (!math_expr::compute(two_var, x, y, result))
   printf("Failed to compute: %s",two_var.c_str());
else
   printf("Result: %15.5f\n",result);

// Three variables 'x', 'y' and 'z' overload
T z = T(345.678);

const std::string three_var = "abs(x - (y / pi)) * z";

if (!math_expr::compute(three_var, x, y, z, result))
   printf("Failed to compute: %s",three_var.c_str());
else
   printf("Result: %15.5f\n",result);
```

## integrate

This free function will attempt to perform a numerical integration of a single variable compiled expression over a specified range and step size. The numerical integration is based on the three point form of Simpson's rule. The integrate function has two overloads, where the variable of integration can either be passed as a reference or as a name in string form. Example usage of the function is as follows:

```cpp
typedef math_expr::symbol_table<T> symbol_table_t;
typedef math_expr::expression<T>   expression_t;
typedef math_expr::parser<T>       parser_t;

const std::string expression_string = "sqrt(1 - (x^2))";

T x = T(0);

symbol_table_t symbol_table;
symbol_table.add_variable("x",x);

expression_t expression;
expression.register_symbol_table(symbol_table);

parser_t parser;
parser.compile(expression_string,expression);

....

// Integrate in domain [-1,1] using a reference to x variable
T area1 = math_expr::integrate(expression, x, T(-1), T(1));

// Integrate in domain [-1,1] using name of x variable
T area2 = math_expr::integrate(expression, "x", T(-1), T(1));
```

## derivative

This free function will attempt to perform a numerical differentiation of a single variable compiled expression at a given point for a given epsilon, using a variant of Newton's difference quotient called the five-point stencil method. The derivative function has two overloads, where the variable of differentiation can either be passed as a reference or as a name in string form. Example usage of the derivative function is as follows:

```cpp
typedef math_expr::symbol_table<T> symbol_table_t;
typedef math_expr::expression<T>   expression_t;
typedef math_expr::parser<T>       parser_t;

const std::string expression_string = "sqrt(1 - (x^2))";

T x = T(0);

symbol_table_t symbol_table;
symbol_table.add_variable("x",x);

expression_t expression;
expression.register_symbol_table(symbol_table);

parser_t parser;
parser.compile(expression_string,expression);

....

// Differentiate expression at value of x = 12.3 using a reference
// to the x variable
x = T(12.3);
T derivative1 = math_expr::derivative(expression, x);

// Differentiate expression where value x = 45.6 using name
// of the x variable
x = T(45.6);
T derivative2 = math_expr::derivative(expression, "x");
```

## second_derivative

This free function will attempt to perform a numerical second derivative of a single variable compiled expression at a given point for a given epsilon, using a variant of Newton's difference quotient method. The second_derivative function has two overloads, where the variable of differentiation can either be passed as a reference or as a name in string form. Example usage of the second_derivative function is as follows:

```cpp
typedef math_expr::symbol_table<T> symbol_table_t;
typedef math_expr::expression<T>   expression_t;
typedef math_expr::parser<T>       parser_t;

const std::string expression_string = "sqrt(1 - (x^2))";

T x = T(0);

symbol_table_t symbol_table;
symbol_table.add_variable("x",x);

expression_t expression;
expression.register_symbol_table(symbol_table);

parser_t parser;
parser.compile(expression_string,expression);

....

// Second derivative of expression where value of x = 12.3 using a
// reference to x variable
x = T(12.3);
T derivative1 = math_expr::second_derivative(expression,x);

// Second derivative of expression where value of x = 45.6 using
// name of x variable
x = T(45.6);
T derivative2 = math_expr::second_derivative(expression, "x");
```

## third_derivative

This free function will attempt to perform a numerical third derivative of a single variable compiled expression at a given point for a given epsilon, using a variant of Newton's difference quotient method. The third_derivative function has two overloads, where the variable of differentiation can either be passed as a reference or as a name in string form. Example usage of the third_derivative function is as follows:

```cpp
typedef math_expr::symbol_table<T> symbol_table_t;
typedef math_expr::expression<T>   expression_t;
typedef math_expr::parser<T>       parser_t;

const std::string expression_string = "sqrt(1 - (x^2))";

T x = T(0);

symbol_table_t symbol_table;
symbol_table.add_variable("x",x);

expression_t expression;
expression.register_symbol_table(symbol_table);

parser_t parser;
parser.compile(expression_string,expression);

....

// Third derivative of expression where value of x = 12.3 using a
// reference to the x variable
x = T(12.3);
T derivative1 = math_expr::third_derivative(expression, x);

// Third derivative of expression where value of x = 45.6 using
// name of the x variable
x = T(45.6);
T derivative2 = math_expr::third_derivative(expression, "x");
```

[Previous](22-runtime-library-packages.md) | [Index](index.md) | [Next](24-runtime-checks.md)
