# Exprtk Notes

[Back to index](index.md)

[Previous](25-benchmarking.md) | [Index](index.md) | [Next](27-simple-exprtk-example.md)

The following is a list of facts and suggestions one may want to take into account when using MathExpr:

## Precision  and performance  of expression  evaluations are  the

dominant principles of the MathExpr library.

## MathExpr  uses a  rudimentary imperative  programming model  with

syntax based on languages such as Pascal and C. Furthermore MathExpr is an LL(2) type grammar and is processed using a recursive descent parsing algorithm.

1. Supported types are float, double, long double and MPFR/GMP. Generally any user defined numerical type that supports all the

```text
basic floating point arithmetic operations: -, +, *,/,^, % etc;
unary and binary operations: sin,cos,min,max,equal etc and  any
other MathExpr dependent operations can be used to specialise the
various components: expression, parser and symbol_table.
```

## Standard arithmetic operator precedence is applied (BEDMAS). In

general C, Pascal or Rust equivalent unary, binary, logical and equality/inequality operator precedence rules apply. eg: a == b and c > d + 1 ---> (a == b) and (c > (d + 1)) x - y <= z / 2 ---> (x - y) <= (z / 2) a - b / c * d^2^3 ---> a - ((b / c) * d^(2^3))

## Results of expressions that are deemed as being 'valid' are  to

exist within the set of Real numbers. All other results will be of the value: Not-A-Number (NaN). However this may not necessarily be a requirement for user defined numerical types, eg: complex number type.

## Supported   user  defined   types   are   numeric  and   string

variables, numeric vectors and functions.

## All  reserved words,  keywords, variable,  vector, string   and

function names are case-insensitive.

## Variable, vector, string variable and function names must begin

with a letter (A-Z or a-z), then can be comprised of any combination of letters, digits, underscores and non-consecutive dots, ending in either a letter (A-Z or a-z), digit or underscore. (eg: x, y2, var1, power_func99, person.age, item.size.0). The associated regex pattern is: [a-zA-Z][a-zA-Z0-9_]*(\.[a-zA-Z0-9_]+)*

## Expression lengths and sub-expression lists are limited only by

storage capacity.

## The  life-time of  objects registered  with or  created from  a

specific symbol-table must, at least, span the lifetime of the symbol table instance and all compiled expressions which utilise objects, such as variables, strings, vectors, function compositor functions and functions of that symbol-table, otherwise the result will be undefined behaviour.

## Equal and not_equal  are  normalised-epsilon equality routines,

which use epsilons of 0.0000000001 and 0.000001 for double and float types respectively.

## All trigonometric functions  assume radian input  unless stated

otherwise.

## Expressions may contain  white-space characters such  as space,

tabs, new-lines, control-feed et al. ('\n', '\r', '\t', '\b', '\v', '\f')

## Strings may be comprised of any  combination of letters, digits

```text
special characters including (~!@#$%^&*()[]|=+ ,./?<>;:"`~_) or
hexadecimal escaped sequences (eg: \0x30) and must be enclosed
with single-quotes.
eg: 'Frankly my dear, \0x49 do n0t give a damn!'
```

## User defined  normal functions  can have  up to  20 parameters,

where as user defined generic-functions and vararg-functions can have an unlimited number of parameters.

1. The inbuilt polynomial functions can be at most of degree 12.

1. Where appropriate constant folding optimisations may be applied. (eg: The expression '2 + (3 - (x / y))' becomes '5 - (x / y)')

## If the strength  reduction compilation option has  been enabled,

then where applicable strength reduction optimisations may be applied.

## String  processing capabilities  are available  by default.  To

turn them off, the following needs to be defined at compile time: MATH_EXPR_DISABLE_STRING_CAPABILITIES

## Composited functions can call themselves or any other functions

that have been defined prior to their own definition.

## Recursive calls made from within composited functions will have

a stack size bound by the stack of the executing architecture.

## User  defined functions  by default  are assumed  to have  side

effects. As such an "all constant parameter" invocation of such functions wont result in constant folding. If the function has no side-effects then that can be noted during the constructor of the ifunction allowing it to be constant folded where appropriate.

## The entity relationship between symbol_table and an  expression

is many-to-many. However the intended 'typical' use-case where possible, is to have a single symbol table manage the variable and function requirements of multiple expressions.

## The common use-case  for an expression  is to have  it compiled

only ONCE and then subsequently have it evaluated multiple times. An extremely inefficient and suboptimal approach would be to recompile an expression from its string form every time it requires evaluating.

## It is  strongly recommended  that  the  return value  of method

invocations from the parser and symbol_table types be taken into account. Specifically the 'compile' method of the parser and the 'add_xxx' set of methods of the symbol_table as they denote either the success or failure state of the invoked call. Continued processing from a failed state without having first rectified the underlying issue will in turn result in further failures and undefined behaviours.

## The following are examples of compliant floating point value

representations:

1. 12345 (06) -123.456
1. +123.456e+12 (07) 123.456E-12
1. +012.045e+07 (08) .1234
1. 1234. (09) -56789.
1. 123.456f (10) -321.654E+3L

1. Expressions may contain any of the following comment styles:

1. // .... \n
1. # .... \n
1. /* .... */

## The  'null'  value  type  is  a  special  non-zero  type   that

incorporates specific semantics when undergoing operations with the standard numeric type. The following is a list of type and boolean results associated with the use of 'null':

1. null +,-,*,/,% x --> x
1. x +,-,*,/,% null --> x
1. null +,-,*,/,% null --> null
1. null == null --> true
1. null == x --> true
1. x == null --> true
1. x != null --> false
1. null != null --> false
1. null != x --> false

## The following is a list  of reserved words and symbols  used by

MathExpr. Attempting to add a variable or custom function to a symbol table using any of the reserved words will result in a failure.

abs, acos, acosh, and, asin, asinh, assert, atan, atan2, atanh, avg, break, case, ceil, clamp, continue, cosh, cos, cot, csc, default, deg2grad, deg2rad, else, equal, erfc, erf, exp, expm1, false, floor, for, frac, grad2deg, hypot, iclamp, if, ilike, in, inrange, in, like, log, log10, log1p, log2, logn, mand, max, min, mod, mor, mul, nand, ncdf, nor, not, not_equal, not, null, or, pow, rad2deg, repeat, return, root, roundn, round, sec, sgn, shl, shr, sinc, sinh, sin, sqrt, sum, swap, switch, tanh, tan, true, trunc, until, var,

```text
while, xnor, xor
```

1. Every valid MathExpr statement is a "value returning" expression. Unlike some languages that limit the types of expressions that can be performed in certain situations, in MathExpr any valid expression can be used in any "value consuming" context. eg:

```text
var y := 3;
for (var x := switch
              {
                case 1  :  7;
                case 2  : -1 + ~{var x{};};
                default :  y > 2 ? 3 : 4;
              };
     x != while (y > 0) { y -= 1; };
     x -= {
            if (min(x,y) < 2 * max(x,y))
              x + 2;
            else
              x + y - 3;
          }
    )
{
```

## / (x - y);

```text
};
```

## It is recommended when prototyping expressions that the  MathExpr

REPL be utilised, as it supports all the features available in the library, including complete error analysis, benchmarking and dependency dumps etc which allows for rapid coding/prototyping and debug cycles without the hassle of having to recompile test programs with expressions that have been hard-coded. It is also a good source of truth for how the library's various features can be applied.

## For performance considerations,  one should assume  the actions

of expression, symbol table and parser instance instantiation and destruction, and the expression compilation process itself to be of high latency. Hence none of them should be part of any performance critical code paths, and should instead occur entirely either before or after such code paths.

## Deep  copying  an  expression  instance  for  the  purposes  of

persisting to disk or otherwise transmitting elsewhere with the intent to 'resurrect' the expression instance later on is not possible due to the reasons described in the final note of Section 10. The recommendation is to instead simply persist the string form of the expression and compile the expression at run-time on the target.

## The  correctness  and  robustness  of  the  MathExpr  library  is

maintained by having a comprehensive suite of unit tests and functional tests all of which are run using sanitizers (ASAN, UBSAN, LSAN, MSAN, TSAN). Additionally, continuous fuzz-testing provided by Google OSS Fuzz, and static analysis via Synopsis Coverity.

## The library name MathExpr is a modern fork of ExprTk,

which stands for "Mathematical Expression Toolkit".

## For general support, inquiries or bug/issue reporting:

[https://github.com/sukeya/MathExpr/issues](https://github.com/sukeya/MathExpr/issues)

## Before jumping in and using MathExpr, do take the time to  peruse

the documentation and all of the examples, both in the main and the extras distributions. Having an informed general view of what can and can't be done, and how something should be done with MathExpr, will likely result in a far more productive and enjoyable programming experience.

[Previous](25-benchmarking.md) | [Index](index.md) | [Next](27-simple-exprtk-example.md)
