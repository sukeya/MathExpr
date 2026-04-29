# Section 14: Vector Processing

[Back to index](index.md)

[Previous](13-variable-vector-and-string-definition.md) | [Index](index.md) | [Next](15-user-defined-functions.md)

ExprTk provides support for various forms of vector oriented arithmetic, inequalities and processing. The various supported pairs are as follows:

1. vector and vector (eg: v0 + v1)
1. vector and scalar (eg: v + 33)
1. scalar and vector (eg: 22 * v)

The following is a list of operations that can be used in conjunction with vectors:

1. Arithmetic: +, -, *, /, %
1. Exponentiation: vector ^ scalar
1. Assignment: :=, +=, -=, *=, /=, %=, <=>
1. Inequalities: <, <=, >, >=, ==, =, equal
1. Boolean logic: and, nand, nor, or, xnor, xor
1. Unary operations: abs, acos, acosh, asin, asinh, atan, atanh, ceil, cos, cosh, cot, csc, deg2grad, deg2rad, erf, erfc, exp, expm1, floor, frac, grad2deg, log, log10, log1p, log2, rad2deg, round, sec, sgn, sin, sinc, sinh, sqrt, swap, tan, tanh, trunc, thresholding
1. Aggregate and Reduce operations: avg, max, min, mul, dot, dotk, sum, sumk, count, all_true, all_false, any_true, any_false
1. Transformation operations: copy, diff, reverse, rotate-left/right, shift-left/right, sort, nth_element
1. BLAS-L1: axpy, axpby, axpyz, axpbyz, axpbz

**Note 11.** When one of the above described operations is being performed between two vectors, the operation will only span the size of the smallest vector. The elements of the larger vector outside of the range will not be included. The operation itself will be processed element-wise over values of the smaller of the two ranges.

The following simple example demonstrates the vector processing capabilities by computing the dot-product of the vectors v0 and v1 and then assigning it to the variable v0dotv1:

```text
var v0[3] := { 1, 2, 3 };
var v1[3] := { 4, 5, 6 };
var v0dotv1 := sum(v0 * v1);
```

The following is a for-loop based implementation that is equivalent to the previously mentioned dot-product computation expression:

```text
var v0[3] := { 1, 2, 3 };
var v1[3] := { 4, 5, 6 };
var v0dotv1;

for (var i := 0; i < min(v0[],v1[]); i += 1)
{
   v0dotv1 += (v0[i] * v1[i]);
}
```

**Note 12.** When the aggregate or reduction operations denoted above are used in conjunction with a vector or vector expression, the return value is not a vector but rather a single value.

```text
var x[3] := { 1, 2, 3 };

sum(x)      ==  6
sum(1 + 2x) == 15
avg(3x + 1) ==  7
min(1 / x)  == (1 / 3)
max(x / 2)  == (3 / 2)
sum(x > 0 and x < 5) == x[]
```

When utilising external user defined vectors via the symbol table as opposed to expression local defined vectors, the typical 'add_vector' method from the symbol table will register the entirety of the vector that is passed. The following example attempts to evaluate the sum of elements of the external user defined vector within a typical yet trivial expression:

```cpp
const std::string reduce_program = " sum(2 * v + 1) ";

std::vector<T> v0 { T(1.1), T(2.2), ..... , T(99.99) };

symbol_table_t symbol_table;
symbol_table.add_vector("v",v);

expression_t expression;
expression.register_symbol_table(symbol_table);

parser_t parser;
parser.compile(reduce_program,expression);

T sum = expression.value();
```

For the most part, this is a very common use-case. However there may be situations where one may want to evaluate the same vector oriented expression many times over, but using different vectors or sub ranges of the same vector of the same size to that of the original upon every evaluation.

The usual solution is to either recompile the expression for the new vector instance, or to copy the contents from the new vector to the symbol table registered vector and then perform the evaluation. When the vectors are large or the re-evaluation attempts are numerous, these solutions can become rather time consuming and generally inefficient.

```cpp
std::vector<T> v1 { T(2.2), T(2.2), ..... , T(2.2) };
std::vector<T> v2 { T(3.3), T(3.3), ..... , T(3.3) };
std::vector<T> v3 { T(4.4), T(4.4), ..... , T(4.4) };

std::vector<std::vector<T>> vv { v1, v2, v3 };
...
T sum = T(0);

for (auto& new_vec : vv)
{
   v = new_vec; // update vector
   sum += expression.value();
}
```

A solution to the above 'efficiency' problem, is to use the math_expr::vector_view object. The vector_view is instantiated with a size and backing based upon a vector. Upon evaluations if the backing needs to be 'updated' to either another vector or sub-range, the vector_view instance can be efficiently rebased, and the expression evaluated as normal.

```text
math_expr::vector_view<T> view = math_expr::make_vector_view(v,v.size());

symbol_table_t symbol_table;
symbol_table.add_vector("v",view);

...

T sum = T(0);

for (auto& new_vec : vv)
{
   view.rebase(new_vec.data()); // update vector
   sum += expression.value();
}
```

Another useful feature of math_expr::vector_view is that all such vectors can have their sizes modified (or "resized"). The resizing of the associated vectors can happen either between or during evaluations.

```cpp
std::vector<T> v = { 1, 2, 3, 4, 5, 6, 7, 8 };
math_expr::vector_view<T> view = math_expr::make_vector_view(v,v.size());

symbol_table_t symbol_table;
symbol_table.add_vector("v",view);

const std::string expression_string = "v[]";

expression_t expression;
expression.register_symbol_table(symbol_table);

parser_t parser;
parser.compile(expression_string, expression);

for (std::size_t i = 1; i <= v.size(); ++i)
{
   vv.set_size(i);
   expression.value();
}
```

In the example above, a vector_view is instantiated with a std::vector instance with eight elements and registered to the given symbol_table. An expression is then compiled, which in this case simply returns the size of the vector at that point in time. The expression is evaluated eight times (size of vector times), where upon each iteration the size of the vector is changed with values ranging from one to eight.

**Note 13.** When modifying the size of a vector, the new size must be at least one or larger and must not exceed the original size of the vector_view when it was instantiated.

**Note 14.** The lifetime of any parser, symbol_table or expression instance must not exceed that of any vector_view instance that has been registered with it. Furthermore the lifetime of a vector_view must not exceed that of the underlying vector instance it is associated with.

**Note 15.** In a multi-threaded context the rebase function should not be called during associated expression evaluation, as this will lead to undefined behaviour (eg: torn reads and writes).

[Previous](13-variable-vector-and-string-definition.md) | [Index](index.md) | [Next](15-user-defined-functions.md)
