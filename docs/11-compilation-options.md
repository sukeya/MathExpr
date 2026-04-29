# Section 11: Compilation Options

[Back to index](index.md)

[Previous](10-components.md) | [Index](index.md) | [Next](12-expression-structures.md)

The math_expr::parser when being instantiated takes as input a set of options to be used during the compilation process of expressions. An example instantiation of math_expr::parser where only the joiner, commutative and strength reduction options are enabled is as follows:

```cpp
typedef math_expr::parser<NumericType>::settings_t settings_t;

const std::size_t compile_options =
                     settings_t::e_joiner            +
                     settings_t::e_commutative_check +
                     settings_t::e_strength_reduction;

parser_t parser(compile_options);
```

Currently nine types of compile time options are supported, and enabled by default. The options and their explanations are as follows:

1. `Replacer`
1. `Joiner`
1. Numeric Check
1. Bracket Check
1. Sequence Check
1. Commutative Check
1. Strength Reduction Check
1. Stack And Node Depth Check
1. Expression Size Check

## Replacer (e_replacer)

Enable replacement of specific tokens with other tokens. For example the token "true" of type symbol shall be replaced with the numeric token of value one.

1. (x < y) == true ---> (x < y) == 1
1. false == (x > y) ---> 0 == (x > y)

## Joiner (e_joiner)

Enable joining of multi-character operators that may have been incorrectly disjoint in the string representation of the specified expression. For example the consecutive tokens of ">" "=" will become ">=" representing the "greater than or equal to" operator. If not properly resolved the original form will cause a compilation error. The following is a listing of the scenarios that the joiner can handle:

1. '>' '=' ---> '>=' (gte)
1. '<' '=' ---> '<=' (lte)
1. '=' '=' ---> '==' (equal)
1. '!' '=' ---> '!=' (not-equal)
1. '<' '>' ---> '<>' (not-equal)
1. ':' '=' ---> ':=' (assignment)
1. '+' '=' ---> '+=' (addition assignment)
1. '-' '=' ---> '-=' (subtraction assignment)
1. '*' '=' ---> '*=' (multiplication assignment)
1. '/' '=' ---> '/=' (division assignment)
1. '%' '=' ---> '%=' (modulo assignment)
1. '+' '-' ---> '-' (subtraction)
1. '-' '+' ---> '-' (subtraction)
1. '-' '-' ---> '+' (addition)
1. '<=' '>' ---> '<=>' (swap)

An example of the transformation that takes place is as follows:

1. (x > = y) and (z ! = w) ---> (x >= y) and (z != w)

## Numeric Check (e_numeric_check)

Enable validation of tokens representing numeric types so as to catch any errors prior to the costly process of the main compilation step commencing.

## Bracket Check (e_bracket_check)

Enable the check for validating the ordering of brackets in the specified expression.

## Sequence Check (e_sequence_check)

Enable the check for validating that sequences of either pairs or triplets of tokens make sense. For example the following sequence of tokens when encountered will raise an error:

1. (x + * 3) ---> sequence error

## Commutative Check (e_commutative_check)

Enable the check that will transform sequences of pairs of tokens that imply a multiplication operation. The following are some examples of such transformations:

1. 2x ---> 2 * x
1. 25x^3 ---> 25 * x^3
1. 3(x + 1) ---> 3 * (x + 1)
1. (x + 1)4 ---> (x + 1) * 4
1. 5foo(x,y) ---> 5 * foo(x,y)
1. foo(x,y)6 + 1 ---> foo(x,y) * 6 + 1
1. (4((2x)3)) ---> 4 * ((2 * x) * 3)
1. w / (x - y)z ---> w / (x - y) * z

## Strength Reduction Check (e_strength_reduction)

Enable the use of strength reduction optimisations during the compilation process. In ExprTk strength reduction optimisations predominantly involve transforming sub-expressions into other forms that are algebraically equivalent yet less costly to compute. The following are examples of the various transformations that can occur:

1. (x / y) / z ---> x / (y * z)
1. (x / y) / (z / w) ---> (x * w) / (y * z)
1. (2 * x) - (2 * y) ---> 2 * (x - y)
1. (2 / x) / (3 / y) ---> (2 / 3) / (x * y)
1. (2 * x) * (3 * y) ---> 6 * (x * y)
1. (2 * x) * (2 - 4 / 2) ---> 0
1. (3 - 6 / 2) / (2 * x) ---> 0
1. avg(x,y,z) * (2 - 4 / 2) ---> 0

**Note 08.** When using strength reduction in conjunction with expressions whose inputs or sub-expressions may result in values nearing either of the bounds of the underlying numeric type (eg: double), there may be the possibility of a decrease in the precision of results.

In the following example the given expression which represents an attempt at computing the average between x and y will be transformed as follows:

## + (y * 0.5) ---> 0.5 * (x + y)

There may be situations where the above transformation will cause numerical overflows and that the original form of the expression is desired over the strength reduced form. In these situations it is best to turn off strength reduction optimisations or to use a type with a larger numerical bound.

## Stack And Node Depth Check

ExprTk incorporates a recursive descent parser. When parsing expressions comprising inner sub-expressions, the recursive nature of the parsing process causes the stack to grow. If the expression causes the stack to grow beyond the stack size limit, this would lead to a stackoverflow and its associated stack corruption and security vulnerability issues.

Similarly to parsing, evaluating an expression may cause the stack to grow. Such things like user defined functions, composite functions and the general nature of the AST being evaluated can cause the stack to grow, and may result in potential stackoverflow issues as denoted above.

ExprTk provides a set of checks that prevent both of the above denoted problems at compile time. These checks rely on two specific limits being set on the parser settings instance, these limits are:

1. max_stack_depth (default: 400 )
1. max_node_depth (default: 10000)

The following demonstrates how these two parser parameters can be set:

```text
parser_t parser;

parser.settings().set_max_stack_depth(100);
parser.settings().set_max_node_depth(200);
```

In the above code, during parsing if the stack depth reaches or exceeds 100 levels, the parsing process will immediately halt and return with a failure. Similarly, during synthesizing the AST nodes, if the compilation process detects an AST tree depth exceeding 200 levels the parsing process will halt and return a parsing failure.

## Expression Size Check

ExprTk allows for expression local variables, vectors and strings to be defined. As such the amount of data in terms of bytes consumed by the expression locals can also be limited, so as to prevent expressions once compiled from consuming large amounts of data.

The maximum number of bytes an expression's locally defined variables may use can be set via the parser settings prior to compilation as follows:

```cpp
using expression_t = math_expr::expression<double>;
using parser_t     = math_expr::parser<double>;

expression_t expression;
parser_t     parser;

parser.settings().set_max_total_local_symbol_size_bytes(16);

const std::string expression1 = "var x := 1; var y := 1;";
const std::string expression2 = "var x := 1; var v[2] := [1];";

expression_t expression;

parser.compile(expression1, expression); // compilation success
parser.compile(expression2, expression); // compilation error
```

In the above example, the expression's max total local symbol size has been set to 16 bytes. The numeric type being used is double which is 8 bytes per instance. The first expression (expression1) compiles successfully because the total local symbol size is 16 bytes, 8 bytes for each of the variables x and y.

However the second expression (expression2) fails to compile. This is because the total number of bytes needed for the expression is 24 bytes, 8 bytes for the variable x and 16 bytes for the vector v, as such exceeding the previously set limit of 16 bytes. The error diagnostic generated by the parser on compilation failure will look like the following:

ERR161 - Adding vector 'v' of size 2 bytes will exceed max total local symbol size of: 16 bytes, current total size: 8 bytes

## Vector Size Check

When defining an expression local vector, ExprTk uses a default maximum vector size of two billion elements. One may want to limit the maximum vector size to be either smaller or larger than the specified default value. The maximum size value can be changed via the parser settings.

```cpp
parser_t parser;

parser.settings().set_max_local_vector_size(1000000);

std::string expression1 = "var v[1e6] := [123]";
std::string expression2 = "var v[1e9] := [123]";

expression_t expression;

parser.compile(expression1, expression); // compilation success
parser.compile(expression2, expression); // compilation error
```

In the above code, the maximum local vector size is set to one million elements. During compilation of an expression if there is a vector definition where the vector's size exceeds the maximum allowed vector size a compilation error shall be emitted. The error diagnostic generated by the parser on compilation failure will look like the following:

ERR160 - Invalid vector size. Must be an integer in the range [0,1000000], size: 1000000000

[Previous](10-components.md) | [Index](index.md) | [Next](12-expression-structures.md)
