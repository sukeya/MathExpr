# Section 09: Fundamental Types

[Back to index](index.md)

[Previous](08-built-in-operations-and-functions.md) | [Index](index.md) | [Next](10-components.md)

ExprTk supports three fundamental types which can be used freely in expressions. The types are as follows:

1. `Scalar`
1. `Vector`
1. `String`

## Scalar Type

The scalar type is a singular numeric value. The underlying type is that used to specialise the ExprTk components (float, double, long double, MPFR et al).

## Vector Type

The vector type is a fixed size sequence of contiguous scalar values. A vector can be indexed resulting in a scalar value. Operations between a vector and scalar will result in a vector with a size equal to that of the original vector, whereas operations between vectors will result in a vector of size equal to that of the smaller of the two. In both mentioned cases, the operations will occur element-wise.

## String Type

The string type is a variable length sequence of 8-bit chars. Strings can be assigned and concatenated to one another, they can also be manipulated via sub-ranges using the range definition syntax. Strings however can not interact with scalar or vector types.

[Previous](08-built-in-operations-and-functions.md) | [Index](index.md) | [Next](10-components.md)
