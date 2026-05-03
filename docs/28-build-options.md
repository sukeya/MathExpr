# Build Options

[Back to index](index.md)

[Previous](27-simple-exprtk-example.md) | [Index](index.md) | [Next](29-files.md)

When building MathExpr there are a number of defines that will enable or disable certain features and capabilities. The defines can either be part of a compiler command line switch or scoped around the include to the MathExpr header, or enabled via the same-named CMake cache variable. The defines are as follows:

1. `MATH_EXPR_ENABLE_DEBUGGING`
1. `MATH_EXPR_DISABLE_CARDINAL_POW_OPTIMISATION`
1. `MATH_EXPR_DISABLE_COMMENTS`
1. `MATH_EXPR_DISABLE_BREAK_CONTINUE`
1. `MATH_EXPR_DISABLE_SC_ANDOR`
1. `MATH_EXPR_DISABLE_RETURN_STATEMENT`
1. `MATH_EXPR_DISABLE_ENHANCED_FEATURES`
1. `MATH_EXPR_DISABLE_STRING_CAPABILITIES`
1. `MATH_EXPR_DISABLE_SUPERSCALAR_UNROLL`
1. `MATH_EXPR_DISABLE_RTL_IO`
1. `MATH_EXPR_DISABLE_RTL_IO_FILE`
1. `MATH_EXPR_DISABLE_RTL_VECOPS`
1. `MATH_EXPR_DISABLE_CASEINSENSITIVITY`
1. `MATH_EXPR_ENABLE_RANGE_RUNTIME_CHECKS`

In addition to the preprocessor-style switches above, MathExpr also accepts the CMake cache
variable `MATH_EXPR_EPSILON`. Unlike the defines listed above, this is not a preprocessor switch.
It configures the floating-point epsilon used by `equal()`, `not_equal()`, and the built-in
`epsilon` constant.

## MATH_EXPR_ENABLE_DEBUGGING

This define will enable printing of debug information to stdout during the compilation process.

## MATH_EXPR_DISABLE_CARDINAL_POW_OPTIMISATION

This define will disable the optimisation invoked when constant integers are used as powers in exponentiation expressions (eg: x^7).

## MATH_EXPR_DISABLE_COMMENTS

This define will disable the ability for expressions to have comments. Expressions that have comments when parsed with a build that has this option, will result in a compilation failure.

## MATH_EXPR_DISABLE_BREAK_CONTINUE

This define will disable the loop-wise 'break' and 'continue' capabilities. Any expression that contains those keywords will result in a compilation failure.

## MATH_EXPR_DISABLE_SC_ANDOR

This define will disable the short-circuit '&' (and) and '|' (or) operators

## MATH_EXPR_DISABLE_RETURN_STATEMENT

This define will disable use of return statements within expressions.

## MATH_EXPR_DISABLE_ENHANCED_FEATURES

This define will disable all enhanced features such as strength reduction and special function optimisations and expression specific type instantiations. This feature will reduce compilation times and binary sizes but will also result in massive performance degradation of expression evaluations.

## MATH_EXPR_DISABLE_STRING_CAPABILITIES

This define will disable all string processing capabilities. Any expression that contains a string or string related syntax will result in a compilation failure.

## MATH_EXPR_DISABLE_SUPERSCALAR_UNROLL

This define will set the loop unroll batch size to 4 operations per loop instead of the default 8 operations. This define is used in operations that involve vectors and aggregations over vectors. When targeting non-superscalar architectures, it may be recommended to build using this particular option if efficiency of evaluations is of concern.

## MATH_EXPR_DISABLE_RTL_IO

This define will disable all of basic IO RTL package features. When present, any attempt to register the basic IO RTL package with a given symbol table will fail causing a compilation error.

## MATH_EXPR_DISABLE_RTL_IO_FILE

This define will disable the file I/O RTL package features. When present, any attempts to register the file I/O package with a given symbol table will fail causing a compilation error.

## MATH_EXPR_DISABLE_RTL_VECOPS

This define will disable the extended vector operations RTL package features. When present, any attempts to register the vector operations package with a given symbol table will fail causing a compilation error.

## MATH_EXPR_DISABLE_CASEINSENSITIVITY

This define will disable case-insensitivity when matching variables and functions. Furthermore all reserved and keywords will only be acknowledged when in all lower-case.

## MATH_EXPR_ENABLE_RANGE_RUNTIME_CHECKS

This define will enable run-time checks pertaining to vector indexing operations used in any of the vector-to-vector and vector-to-scalar operations.

## MATH_EXPR_EPSILON

This CMake cache variable overrides the floating-point epsilon used by MathExpr for real-valued
comparisons and for the built-in `epsilon` symbol table constant.

When `MATH_EXPR_EPSILON` is not set, MathExpr uses `std::numeric_limits<T>::epsilon()` for each
supported real type.

When it is set, the configured scalar is cast to `float`, `double`, and `long double` as needed.
The value must be a positive finite decimal literal such as `1e-9` or `0.125`.

[Previous](27-simple-exprtk-example.md) | [Index](index.md) | [Next](29-files.md)
