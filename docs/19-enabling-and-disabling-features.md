# Enabling & Disabling Features

[Back to index](index.md)

[Previous](18-unknown-unknowns.md) | [Index](index.md) | [Next](20-expression-return-values.md)

The parser can be configured via its settings instance to either allow or disallow certain features that are available within the MathExpr grammar. The features fall into one of the following six categories:

1. Base Functions
1. Control Flow Structures
1. Logical Operators
1. Arithmetic Operators
1. Inequality Operators
1. Assignment Operators

## Base Functions

The list of available base functions is as follows:

abs, acos, acosh, asin, asinh, atan, atanh, atan2, avg, ceil, clamp, cos, cosh, cot, csc, equal, erf, erfc, exp, expm1, floor, frac, hypot, iclamp, like, log, log10, log2, logn, log1p, mand, max, min, mod, mor, mul, ncdf, pow, root, round, roundn, sec, sgn, sin, sinc, sinh, sqrt, sum, swap, tan, tanh, trunc, not_equal, inrange, deg2grad, deg2rad, rad2deg, grad2deg

The above mentioned base functions can be either enabled or disabled 'all' at once, as is demonstrated below:

```text
parser_t parser;
expression_t expression;

parser.settings().disable_all_base_functions();

parser
   .compile("2 * abs(2 - 3)",expression); // compilation failure

parser.settings().enable_all_base_functions();

parser
   .compile("2 * abs(2 - 3)",expression); // compilation success
```

One can also enable or disable specific base functions. The following example demonstrates the disabling of the trigonometric functions 'sin' and 'cos':

```text
parser_t parser;
expression_t expression;

parser.settings()
   .disable_base_function(settings_t::e_bf_sin)
   .disable_base_function(settings_t::e_bf_cos);

parser
   .compile("(sin(x) / cos(x)) == tan(x)",expression); // failure

parser.settings()
   .enable_base_function(settings_t::e_bf_sin)
   .enable_base_function(settings_t::e_bf_cos);

parser
   .compile("(sin(x) / cos(x)) == tan(x)",expression); // success
```

## Control Flow Structures

The list of available control flow structures is as follows:

1. If or If-Else
1. Switch statement
1. For Loop
1. While Loop
1. Repeat Loop

The above mentioned control flow structures can be either enabled or disabled 'all' at once, as is demonstrated below:

```cpp
parser_t parser;
expression_t expression;

const std::string program =
   " var x := 0;                      "
   " for (var i := 0; i < 10; i += 1) "
   " {                                "
   "   x += i;                        "
   " }                                ";

parser.settings().disable_all_control_structures();

parser
   .compile(program,expression); // compilation failure

parser.settings().enable_all_control_structures();

parser
   .compile(program,expression); // compilation success
```

One can also enable or disable specific control flow structures. The following example demonstrates the disabling of the for-loop control flow structure:

```cpp
parser_t parser;
expression_t expression;

const std::string program =
   " var x := 0;                      "
   " for (var i := 0; i < 10; i += 1) "
   " {                                "
   "   x += i;                        "
   " }                                ";

parser.settings()
   .disable_control_structure(settings_t::e_ctrl_for_loop);

parser
   .compile(program,expression); // failure

parser.settings()
   .enable_control_structure(settings_t::e_ctrl_for_loop);

parser
   .compile(program,expression); // success
```

## Logical Operators

The list of available logical operators is as follows:

and, nand, nor, not, or, xnor, xor, &, |

The above mentioned logical operators can be either enabled or disabled 'all' at once, as is demonstrated below:

```text
parser_t parser;
expression_t expression;

parser.settings().disable_all_logic_ops();

parser
   .compile("1 or not(0 and 1)",expression); // compilation failure

parser.settings().enable_all_logic_ops();

parser
   .compile("1 or not(0 and 1)",expression); // compilation success
```

One can also enable or disable specific logical operators. The following example demonstrates the disabling of the 'and' logical operator:

```text
parser_t parser;
expression_t expression;

parser.settings()
   .disable_logic_operation(settings_t::e_logic_and);

parser
   .compile("1 or not(0 and 1)",expression); // failure

parser.settings()
   .enable_logic_operation(settings_t::e_logic_and);

parser
   .compile("1 or not(0 and 1)",expression); // success
```

## Arithmetic Operators

The list of available arithmetic operators is as follows:

```text
   +, -, *, /, %, ^
```

The above mentioned arithmetic operators can be either enabled or disabled 'all' at once, as is demonstrated below:

```text
parser_t parser;
expression_t expression;

parser.settings().disable_all_arithmetic_ops();

parser
   .compile("1 + 2 / 3",expression); // compilation failure

parser.settings().enable_all_arithmetic_ops();

parser
   .compile("1 + 2 / 3",expression); // compilation success
```

One can also enable or disable specific arithmetic operators. The following example demonstrates the disabling of the addition '+' arithmetic operator:

```text
parser_t parser;
expression_t expression;

parser.settings()
   .disable_arithmetic_operation(settings_t::e_arith_add);

parser
   .compile("1 + 2 / 3",expression); // failure

parser.settings()
   .enable_arithmetic_operation(settings_t::e_arith_add);

parser
   .compile("1 + 2 / 3",expression); // success
```

## Inequality Operators

The list of available inequality operators is as follows:

<, <=, >, >=, ==, =, != <>

The above mentioned inequality operators can be either enabled or disabled 'all' at once, as is demonstrated below:

```text
parser_t parser;
expression_t expression;

parser.settings().disable_all_inequality_ops();

parser
   .compile("1 < 3",expression); // compilation failure

parser.settings().enable_all_inequality_ops();

parser
   .compile("1 < 3",expression); // compilation success
```

One can also enable or disable specific inequality operators. The following example demonstrates the disabling of the less-than '<' inequality operator:

```text
parser_t parser;
expression_t expression;

parser.settings()
   .disable_inequality_operation(settings_t::e_ineq_lt);

parser
   .compile("1 < 3",expression); // failure

parser.settings()
   .enable_inequality_operation(settings_t::e_ineq_lt);

parser
   .compile("1 < 3",expression); // success
```

## Assignment Operators

The list of available assignment operators is as follows:

:=, +=, -=, *=, /=, %=

The above mentioned assignment operators can be either enabled or disabled 'all' at once, as is demonstrated below:

```text
T x = T(0);

parser_t       parser;
expression_t   expression;
symbol_table_t symbol_table;

symbol_table.add_variable("x",x);

expression.register_symbol_table(symbol_table);

parser.settings().disable_all_assignment_ops();

parser
   .compile("x := 3",expression); // compilation failure

parser.settings().enable_all_assignment_ops();

parser
   .compile("x := 3",expression); // compilation success
```

One can also enable or disable specific assignment operators. The following example demonstrates the disabling of the '+=' addition assignment operator:

```text
T x = T(0);

parser_t       parser;
expression_t   expression;
symbol_table_t symbol_table;

symbol_table.add_variable("x",x);

expression.register_symbol_table(symbol_table);

parser.settings()
   .disable_assignment_operation(settings_t::e_assign_addass);

parser
   .compile("x += 3",expression); // failure

parser.settings()
   .enable_assignment_operation(settings_t::e_assign_addass);

parser
   .compile("x += 3",expression); // success
```

**Note 30.** In the event of a base function being disabled, one can redefine the base function using the standard custom function definition process. In the following example the 'sin' function is disabled then redefined as a function taking degree input.

```cpp
template <typename T>
struct sine_deg final : public math_expr::ifunction<T>
{
   sine_deg() : math_expr::ifunction<T>(1) {}

   inline T operator()(const T& v) override
   {
      const T pi = math_expr::details::numeric::constant::pi;
      return std::sin((v * T(pi)) / T(180));
   }
};

 ...

typedef math_expr::symbol_table<T> symbol_table_t;
typedef math_expr::expression<T>   expression_t;
typedef math_expr::parser<T>       parser_t;

typedef typename parser_t::settings_store settings_t;

sine_deg<T> sine;

symbol_table.add_reserved_function("sin",sine);

expression_t expression;

expression.register_symbol_table(symbol_table);

parser_t parser;

parser.settings()
   .disable_base_function(settings_t::e_bf_sin);

parser.compile("1 + sin(30)",expression);
```

In the example above, the custom 'sin' function is registered with the symbol_table using the method 'add_reserved_function'. This is done so as to bypass the checks for reserved words that are carried out on the provided symbol names when calling the standard 'add_function' method. Normally if a user specified symbol name conflicts with any of the MathExpr reserved words, the add_function call will fail.

[Previous](18-unknown-unknowns.md) | [Index](index.md) | [Next](20-expression-return-values.md)
