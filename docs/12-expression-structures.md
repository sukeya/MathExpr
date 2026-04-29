# Section 12: Expression Structures

[Back to index](index.md)

[Previous](11-compilation-options.md) | [Index](index.md) | [Next](13-variable-vector-and-string-definition.md)

Exprtk supports mathematical expressions in numerous forms based on a simple imperative programming model. This section will cover the following topics related to general structure and programming of expressions using ExprTk:

1. Multi-Statement Expressions
1. Statements And Side-Effects
1. Conditional Statements
1. Special Functions

## Multi-Statement Expressions

Expressions in ExprTk can be comprised of one or more statements, which may sometimes be called sub-expressions. The following are two examples of expressions stored in std::string variables, the first a single statement and the second a multi-statement expression:

```cpp
std::string single_statement = " z := x + y ";

std::string multi_statement  = " var temp := x; "
                               " x := y + z;    "
                               " y := temp;     ";
```

In a multi-statement expression, the final statement will determine the overall result of the expression. In the following multi-statement expression, the result of the expression when evaluated will be '2.3', which will also be the value stored in the 'y' variable.

```text
z := x + y;
y := 2.3;
```

As demonstrated in the expression above, statements within an expression are separated using the semi-colon ';' operator. In the event two statements are not separated by a semi-colon, and the implied multiplication feature is active (enabled by default), the compiler will assume a multiplication operation between the two statements.

In the following example we have a multi-statement expression composed of two variable definitions and initialisations for variables x and y and two seemingly separate mathematical operations.

```text
var x:= 2;
var y:= 3;
x + 1
y * 2
```

However the result of the expression will not be 6 as may have been assumed based on the calculation of 'y * 2', but rather the result will be 8. This is because the compiler will have conjoined the two mathematical statements into one via a multiplication operation. The expression when compiled will actually evaluate as the following:

```text
var x:= 2;
var y:= 3;
x + 1 * y * 2;   // 2 + 1 * 3 * 2 == 8
```

In ExprTk any valid statement will itself return a value. This value can further be used in conjunction with other statements. This includes language structures such as if-statements, loops (for, while) and the switch statement. Typically the last statement executed in the given construct (conditional, loop etc), will be the value that is returned.

In the following example, the return value of the expression will be 11, which is the sum of the variable 'x' and the final value computed within the loop body upon its last iteration:

```text
var x := 1;
x + for (var i := x; i < 10; i += 1)
    {
       i / 2;
       i + 1;
    }
```

## Statements And Side-Effects

Statements themselves may have side effects, which in-turn affects the proceeding statements in multi-statement expressions.

A statement is said to have a side-effect if it causes the state of the expression to change in some way - this includes but is not limited to the modification of the state of external variables used within the expression. Currently the following actions being present in a statement will cause it to have a side-effect:

1. Assignment operation (explicit or potentially)
1. Invoking a user-defined function that has side-effects

The following are examples of expressions where the side-effect status of the statements (sub-expressions) within the expressions have been noted:

```text
  +-+----------------------+------------------------------+
  |#|      Expression      |      Side Effect Status      |
  +-+----------------------+------------------------------+
  |0| x + y                | False                        |
  +-+----------------------+------------------------------+
  |1| z := x + y           | True - Due to assignment     |
  +-+----------------------+------------------------------+
  |2| abs(x - y)           | False                        |
  +-+----------------------+------------------------------+
  |3| abs(x - y);          | False                        |
  | | z := (x += y);       | True - Due to assignments    |
  +-+----------------------+------------------------------+
  |4| var t := abs(x - y); | True - Due to initialisation |
  | | t + x;               | False                        |
  | | z := (x += y);       | True - Due to assignments    |
  +-+----------------------+------------------------------+
  |5| foo(x - y)           | True - user defined function |
  +-+----------------------+------------------------------+
```

**Note 09.** In example 5 from the above set, it is assumed the user defined function foo has been registered as having a side-effect. By default all user defined functions are assumed to have side-effects, unless they are configured in their constructors to not have side- effects using the 'disable_has_side_effects' free function. For more information review Section 15 - User Defined Functions sub-section 7 Function Side-Effects.

At this point we can see that there will be expressions composed of certain kinds of statements that when executed will not affect the nature of the expression's result. These statements are typically called 'dead code'. These statements though not influencing the final result will still be executed and as such they will consume processing time that could otherwise be saved. Consequently ExprTk attempts to detect and remove such statements from expressions.

The 'Dead Code Elimination' (DCE) optimisation process, which is enabled by default, will remove any statements that are determined to not have a side-effect in a multi-statement expression, excluding the final or last statement.

By default the final statement in an expression will always be present regardless of its side-effect status, as it is the statement whose value shall be used as the result of the expression.

In order to further explain the actions taken during the DCE process, lets review the following expression:

```text
var x := 2;      // Statement 1
var y := x + 2;  // Statement 2
x + y;           // Statement 3
y := x + 3y;     // Statement 4
x - y;           // Statement 5
```

The above expression has five statements. Three of them (1, 2 and 4) actively have side-effects. The first two are variable declaration and initialisations, where as the third is due to an assignment operation. There are two statements (3 and 5), that do not explicitly have side-effects, however the latter, statement 5, is the final statement in the expression and hence will be assumed to have a side-effect.

During compilation when the DCE optimisation is applied to the above expression, statement 3 will be removed from the expression, as it has no bearing on the final result of expression, the rest of the statements will all remain. The optimised form of the expression is as follows:

```text
var x := 2;      // Statement 1
var y := x + 2;  // Statement 2
y := x + 3y;     // Statement 3
x - y;           // Statement 4
```

## Conditional Statements (If-Then-Else)

ExprTk supports two forms of conditional branching or otherwise known as if-statements. The first form, is a simple function based conditional statement, that takes exactly three input expressions: condition, consequent and alternative. The following is an example expression that utilises the function based if-statement.

x := if (y < z, y + 1, 2 * z)

In the example above, if the condition 'y < z' is true, then the consequent 'y + 1' will be evaluated, its value shall be returned and subsequently assigned to the variable 'x'. Otherwise the alternative '2 * z' will be evaluated and its value will be returned. This is essentially the simplest form of an if-then-else statement. A simple variation of the expression where the value of the if-statement is used within another statement is as follows:

x := 3 * if (y < z, y + 1, 2 * z) / 2

The second form of if-statement resembles the standard syntax found in most imperative languages. There are two variations of the statement:

1. `If-Statement`
1. If-Then-Else Statement

## If-Statement

This version of the conditional statement returns the value of the consequent expression when the condition expression is true, else it will return a quiet NaN value as its result.

Example 1:

```text
x := if (y < z) y + 3;

Example 2:
x := if (y < z)
     {
        y + 3
     };
```

The two example expressions above are equivalent. If the condition 'y < z' is true, the 'x' variable shall be assigned the value of the consequent 'y + 3', otherwise it will be assigned the value of quiet NaN. As previously discussed, if-statements are value returning constructs, and if not properly terminated using a semi-colon, will end-up combining with the next statement via a multiplication operation. The following example will NOT result in the expected value of 'w + x' being returned:

```text
x := if (y < z) y + 3  // missing semi-colon ';'
w + x
```

When the above supposed multi-statement expression is compiled, the expression will have a multiplication inserted between the two 'intended' statements resulting in the unanticipated expression:

x := (if (y < z) y + 3) * w + x

The solution to the above situation is to simply terminate the conditional statement with a semi-colon as follows:

```text
x := if (y < z) y + 3;
w + x
```

## If-Then-Else Statement

The second variation of the if-statement is to allow for the use of Else and Else-If cascading statements. Examples of such statements are as follows:

Example 1: Example 2: Example 3: if (x < y) if (x < y) if (x > y + 1)

```text
z := x + 3;         {                     y := abs(x - z);
```

else y := z + x; else

```text
y := x - z;            z := x + 3;     {
                    }                     y := z + x;
                    else                  z := x + 3;
                       y := x - z;     };
```

Example 4: Example 5: Example 6: if (2 * x < max(y,3)) if (x < y) if (x < y or (x + z) > y) { z := x + 3; {

```text
y := z + x;         else if (2y != z)     z := x + 3;
z := x + 3;         {                     y := x - z;
```

} z := x + 3; } else if (2y - z) y := x - z; else if (abs(2y - z) >= 3)

```text
y := x - z;         }                     y := x - z;
                    else               else
                       x * x;          {
                                          z := abs(x * x);
                                          x * y * z;
                                       };
```

In the case where there is no final else statement and the flow through the conditional arrives at this final point, the same rules apply to this form of if-statement as to the previous. That is a quiet NaN shall be returned as the result of the if-statement. Furthermore the same requirements of terminating the statement with a semi-colon apply.

## Special Functions

The purpose of special functions in ExprTk is to provide compiler generated equivalents of common mathematical expressions which can be invoked by using the 'special function' syntax (eg: $f12(x,y,z) or $f82(x,y,z,w)).

Special functions dramatically decrease the total evaluation time of expressions which would otherwise have been written using the common form by reducing the total number of nodes in the evaluation tree of an expression and by also leveraging the compiler's ability to correctly optimise such expressions for a given architecture.

3-Parameter 4-Parameter

```text
 +-------------+-------------+    +--------------+------------------+
 |  Prototype  |  Operation  |    |  Prototype   |    Operation     |
 +-------------+-------------+    +--------------+------------------+
```

$f00(x,y,z) | (x + y) / z $f48(x,y,z,w) | x + ((y + z) / w) $f01(x,y,z) | (x + y) * z $f49(x,y,z,w) | x + ((y + z) * w) $f02(x,y,z) | (x + y) - z $f50(x,y,z,w) | x + ((y - z) / w) $f03(x,y,z) | (x + y) + z $f51(x,y,z,w) | x + ((y - z) * w) $f04(x,y,z) | (x - y) + z $f52(x,y,z,w) | x + ((y * z) / w) $f05(x,y,z) | (x - y) / z $f53(x,y,z,w) | x + ((y * z) * w) $f06(x,y,z) | (x - y) * z $f54(x,y,z,w) | x + ((y / z) + w) $f07(x,y,z) | (x * y) + z $f55(x,y,z,w) | x + ((y / z) / w) $f08(x,y,z) | (x * y) - z $f56(x,y,z,w) | x + ((y / z) * w) $f09(x,y,z) | (x * y) / z $f57(x,y,z,w) | x - ((y + z) / w) $f10(x,y,z) | (x * y) * z $f58(x,y,z,w) | x - ((y + z) * w) $f11(x,y,z) | (x / y) + z $f59(x,y,z,w) | x - ((y - z) / w) $f12(x,y,z) | (x / y) - z $f60(x,y,z,w) | x - ((y - z) * w) $f13(x,y,z) | (x / y) / z $f61(x,y,z,w) | x - ((y * z) / w) $f14(x,y,z) | (x / y) * z $f62(x,y,z,w) | x - ((y * z) * w) $f15(x,y,z) | x / (y + z) $f63(x,y,z,w) | x - ((y / z) / w) $f16(x,y,z) | x / (y - z) $f64(x,y,z,w) | x - ((y / z) * w) $f17(x,y,z) | x / (y * z) $f65(x,y,z,w) | ((x + y) * z) - w $f18(x,y,z) | x / (y / z) $f66(x,y,z,w) | ((x - y) * z) - w $f19(x,y,z) | x * (y + z) $f67(x,y,z,w) | ((x * y) * z) - w $f20(x,y,z) | x * (y - z) $f68(x,y,z,w) | ((x / y) * z) - w $f21(x,y,z) | x * (y * z) $f69(x,y,z,w) | ((x + y) / z) - w $f22(x,y,z) | x * (y / z) $f70(x,y,z,w) | ((x - y) / z) - w $f23(x,y,z) | x - (y + z) $f71(x,y,z,w) | ((x * y) / z) - w $f24(x,y,z) | x - (y - z) $f72(x,y,z,w) | ((x / y) / z) - w $f25(x,y,z) | x - (y / z) $f73(x,y,z,w) | (x * y) + (z * w) $f26(x,y,z) | x - (y * z) $f74(x,y,z,w) | (x * y) - (z * w) $f27(x,y,z) | x + (y * z) $f75(x,y,z,w) | (x * y) + (z / w) $f28(x,y,z) | x + (y / z) $f76(x,y,z,w) | (x * y) - (z / w) $f29(x,y,z) | x + (y + z) $f77(x,y,z,w) | (x / y) + (z / w) $f30(x,y,z) | x + (y - z) $f78(x,y,z,w) | (x / y) - (z / w) $f31(x,y,z) | x * y^2 + z $f79(x,y,z,w) | (x / y) - (z * w) $f32(x,y,z) | x * y^3 + z $f80(x,y,z,w) | x / (y + (z * w)) $f33(x,y,z) | x * y^4 + z $f81(x,y,z,w) | x / (y - (z * w)) $f34(x,y,z) | x * y^5 + z $f82(x,y,z,w) | x * (y + (z * w)) $f35(x,y,z) | x * y^6 + z $f83(x,y,z,w) | x * (y - (z * w)) $f36(x,y,z) | x * y^7 + z $f84(x,y,z,w) | x*y^2 + z*w^2 $f37(x,y,z) | x * y^8 + z $f85(x,y,z,w) | x*y^3 + z*w^3 $f38(x,y,z) | x * y^9 + z $f86(x,y,z,w) | x*y^4 + z*w^4 $f39(x,y,z) | x * log(y)+z $f87(x,y,z,w) | x*y^5 + z*w^5 $f40(x,y,z) | x * log(y)-z $f88(x,y,z,w) | x*y^6 + z*w^6 $f41(x,y,z) | x * log10(y)+z $f89(x,y,z,w) | x*y^7 + z*w^7 $f42(x,y,z) | x * log10(y)-z $f90(x,y,z,w) | x*y^8 + z*w^8 $f43(x,y,z) | x * sin(y)+z $f91(x,y,z,w) | x*y^9 + z*w^9 $f44(x,y,z) | x * sin(y)-z $f92(x,y,z,w) | (x and y) ? z : w $f45(x,y,z) | x * cos(y)+z $f93(x,y,z,w) | (x or y) ? z : w $f46(x,y,z) | x * cos(y)-z $f94(x,y,z,w) | (x < y) ? z : w $f47(x,y,z) | x ? y : z $f95(x,y,z,w) | (x <= y) ? z : w $f96(x,y,z,w) | (x > y) ? z : w $f97(x,y,z,w) | (x >= y) ? z : w $f98(x,y,z,w) | (x == y) ? z : w $f99(x,y,z,w) | x*sin(y)+z*cos(w)

[Previous](11-compilation-options.md) | [Index](index.md) | [Next](13-variable-vector-and-string-definition.md)
