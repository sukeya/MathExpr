# Benchmarking

[Back to index](index.md)

[Previous](24-runtime-checks.md) | [Index](index.md) | [Next](26-exprtk-notes.md)

As part of the MathExpr package there is an expression benchmark utility named 'math_expr_benchmark'. The utility attempts to determine expression evaluation speed (or rate of evaluations - evals per second), by evaluating each expression numerous times and mutating the underlying variables of the expression between each evaluation. The utility assumes any valid MathExpr expression (containing conditionals, loops etc), however it will only make use of a predefined set of scalar variables, namely: a, b, c, x, y, z and w. That being said expressions themselves can contain any number of local variables, vectors or strings. There are two modes of operation:

1. `Default`
1. User Specified Expressions

## Default

The default mode is enabled simply by executing the math_expr_benchmark binary with no command line parameters. In this mode a predefined set of expressions will be evaluated in three phases:

1. MathExpr evaluation
1. Native evaluation
1. MathExpr parse

In the first two phases (a and b) a list of predefined (hard-coded) expressions will be evaluated using both MathExpr and native mode implementations. This is done so as to compare evaluation times between MathExpr and native implementations. The set of expressions used are as follows:

1. (y + x)
1. 2 * (y + x)
1. (2 * y + 2 * x)
1. ((1.23 * x^2) / y) - 123.123
1. (y + x / y) * (x - y / x)
1. x / ((x + y) + (x - y)) / y
1. 1 - ((x * y) + (y / x)) - 3
1. (5.5 + x) + (2 * x - 2 / 3 * y) * (x / 3 + y / 4) + (y + 7.7)
1. 1.1x^1 + 2.2y^2 - 3.3x^3 + 4.4y^15 - 5.5x^23 + 6.6y^55
1. sin(2 * x) + cos(pi / y)
1. 1 - sin(2 * x) + cos(pi / y)
1. sqrt(111.111 - sin(2 * x) + cos(pi / y) / 333.333)
1. (x^2 / sin(2 * pi / y)) - x / 2
1. x + (cos(y - sin(2 / x * pi)) - sin(x - cos(2 * y / pi))) - y
1. clamp(-1.0, sin(2 * pi * x) + cos(y / 2 * pi), +1.0)
1. max(3.33, min(sqrt(1 - sin(2 * x) + cos(pi / y) / 3), 1.11))
1. if((y + (x * 2.2)) <= (x + y + 1.1), x - y, x*y) + 2 * pi / x

The third and final phase (c), is used to determine average compilation rates (compiles per second) for expressions of varying complexity. Each expression is compiled 100K times and the average for each expression is output.

## User Specified Expressions

In this mode two parameters are passed to the utility via the command line:

1. A name of a text file containing one expression per line
1. An integer representing the number of evaluations per expression

An example execution of the benchmark utility in this mode is as follows:

```text
./math_expr_benchmark my_expressions.txt 1000000
```

The above invocation will load the expressions from the file 'my_expressions.txt' and will then proceed to evaluate each expression one million times, varying the above mentioned variables (x, y, z etc.) between each evaluation, and at the end of each expression round a print out of running times, result of a single evaluation and total sum of results is provided as demonstrated below:

Expression 1 of 7 4.770 ns 47700 ns ( 9370368.0) '((((x+y)+z)))' Expression 2 of 7 4.750 ns 47500 ns ( 1123455.9) '((((x+y)-z)))' Expression 3 of 7 4.766 ns 47659 ns (21635410.7) '((((x+y)*z)))' Expression 4 of 7 5.662 ns 56619 ns ( 1272454.9) '((((x+y)/z)))' Expression 5 of 7 4.950 ns 49500 ns ( 4123455.9) '((((x-y)+z)))' Expression 6 of 7 7.581 ns 75810 ns (-4123455.9) '((((x-y)-z)))' Expression 7 of 7 4.801 ns 48010 ns ( 0.0) '((((x-y)*z)))'

The benchmark utility can be very useful when investigating evaluation efficiency issues with MathExpr or simply during the prototyping of expressions. As an example, lets take the following expression:

1 / sqrt(2x) * e^(3y)

Lets say we would like to determine which sub-part of the expression takes the most time to evaluate and perhaps attempt to rework the expression based on the results. In order to do this we will create a text file called 'test.txt' and then proceed to make some educated guesses about how to break the expression up into its more 'interesting' sub-parts which we will then add as one expression per line to the file. An example breakdown may be as follows:

1 / sqrt(2x) * e^(3y) 1 / sqrt(2x) e^(3y)

The benchmark with the given file, where each expression will be evaluated 100K times can be executed as follows:

```text
./math_expr_benchmark test.txt 100000
Expr 1 of 3 90.340 ns 9034000 ns (296417859.3) '1/sqrt(2x)*e^(3y)'
Expr 2 of 3 11.100 ns 1109999 ns (    44267.3) '1/sqrt(2x)'
Expr 3 of 3 77.830 ns 7783000 ns (615985286.6) 'e^(3y)'
[*] Number Of Evals:         300000
[*] Total Time:              0.018sec
[*] Total Single Eval Time:  0.000ms
```

From the results above we conclude that the third expression (e^(3y)) consumes the largest amount of time. The variable 'e', as used in both the benchmark and in the expression, is an approximation of the transcendental mathematical constant e (2.71828182845904...) hence the sub-expression should perhaps be modified to use the generally more efficient built-in 'exp' function.

```text
./math_expr_benchmark test.txt 1000000
Expr 1 of 5 86.563 ns 8656300ns (296417859.6) '1/sqrt(2x)*e^(3y)'
Expr 2 of 5 40.506 ns 4050600ns (296417859.6) '1/sqrt(2x)*exp(3y)'
Expr 3 of 5 14.248 ns 1424799ns (    44267.2) '1/sqrt(2x)'
Expr 4 of 5 88.840 ns 8884000ns (615985286.9) 'e^(3y)'
Expr 5 of 5 29.267 ns 2926699ns (615985286.9) 'exp(3y)'
[*] Number Of Evals:        5000000
[*] Total Time:             0.260sec
[*] Total Single Eval Time: 0.000ms
```

The above output demonstrates the results from making the previously mentioned modification to the expression. As can be seen the new form of the expression using the 'exp' function reduces the evaluation time by over 50%, in other words increases the evaluation rate by two fold.

[Previous](24-runtime-checks.md) | [Index](index.md) | [Next](26-exprtk-notes.md)
