# Variable, Vector & String Definition

[Back to index](index.md)

[Previous](12-expression-structures.md) | [Index](index.md) | [Next](14-vector-processing.md)

MathExpr supports the definition of expression local variables, vectors and strings. The definitions must be unique as shadowing is not allowed and object lifetimes are based on scope. Definitions use the following general form:

```text
var <name> := <initialiser>;
```

## Variable Definition

Variables are of numeric type denoting a single value. They can be explicitly initialised to a value, otherwise they will be defaulted to zero. The following are examples of variable definitions:

## Initialise x to zero

```text
var x;
```

## Initialise y to three

```text
var y := 3;
```

## Initialise z to the expression

```text
var z := if (max(1, x + y) > 2, w, v);
```

## Initialise const literal n

```text
var n := 12 / 3;
```

## Vector Definition

Vectors are arrays of a common numeric type. The elements in a vector can be explicitly initialised, otherwise they will all be defaulted to zero. The following are examples of vector definitions:

## Initialise all values to zero

```text
var x[3];
```

## Initialise all values to zero

```text
var x[3] := {};
```

## Initialise all values to given value or expression

```text
var x[3]   := [ 42 ];
var y[x[]] := [ 123 + 3y + sin(w / z) ];
```

## Initialise all values iota style

```text
var v[4] := [ 0 : +1];  //  0,  1,  2,  3
var v[5] := [-3 : -2];  // -3, -5, -7, -9, -11
```

## Initialise the first two values, all other elements to zero

```text
var x[3] := { (1 + x[2]) / x[], (sin(y[0] / x[]) + 3) / x[] };
```

## Initialise the first three (all) values

```text
const var size := 3;
var x[size] := { 1, 2, 3 };
```

## Initialise vector from a vector

```text
var x[4] := { 1, 2, 3, 4 };
var y[3] := x;
var w[5] := { 1, 2 }; // 1, 2, 0, 0, 0
```

## Initialise vector from a smaller vector

```text
var x[3] := { 1, 2, 3 };
var y[5] := x;   // 1, 2, 3, ??, ??
```

## Non-initialised vector

```text
var x[3] := null; // ?? ?? ??
```

## Error as there are too many initialisers

```text
var x[3] := { 1, 2, 3, 4 };
```

1. Error as a vector of size zero is not allowed.

```text
var x[0];
```

## String Definition

Strings are sequences comprised of 8-bit characters. They can only be defined with an explicit initialisation value. The following are examples of string variable definitions:

## Initialise to a string

```text
var x := 'abc';
```

## Initialise to an empty string

```text
var x := '';
```

## Initialise to a string expression

```text
var x := 'abc' + '123';
```

## Initialise to a string range

```text
var x := 'abc123'[2:4];
```

## Initialise to another string variable

```text
var x := 'abc';
var y := x;
```

## Initialise to another string variable range

```text
var x := 'abc123';
var y := x[2:4];
```

## Initialise to a string expression

```text
var x := 'abc';
var y := x + '123';
```

## Initialise to a string expression range

```text
var x := 'abc';
var y := (x + '123')[1:3];
```

## Return Value

Variable and vector definitions have a return value. In the case of variable definitions, the value to which the variable is initialised will be returned. Where as for vectors, the value of the first element

1. shall be returned.

```text
8 == ((var x := 7;) + 1)
4 == (var y[3] := {4, 5, 6};)
```

## Variable/Vector Assignment

The value of a variable can be assigned to a vector and a vector or a vector expression can be assigned to a variable.

## Variable To Vector:

Every element of the vector is assigned the value of the variable or expression.

```text
var x    := 3;
var y[3] := { 1, 2, 3 };
y := x + 1;
```

## Vector To Variable:

The variable is assigned the value of the first element of the vector (aka vec[0])

```text
var x    := 3;
var y[3] := { 1, 2, 3 };
x := y + 1;
```

**Note 10.** During the expression compilation phase, tokens are classified based on the following priorities:

1. Reserved keywords or operators (+, -, and, or, etc)
1. Base functions (abs, sin, cos, min, max etc)
1. Symbol table variables
1. Expression local defined variables
1. Symbol table functions
1. Unknown symbol resolver based variables

[Previous](12-expression-structures.md) | [Index](index.md) | [Next](14-vector-processing.md)
