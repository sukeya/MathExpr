# Example Expressions

[Back to index](index.md)

[Previous](01-capabilities.md) | [Index](index.md) | [Next](03-copyright-notice.md)

The following is a short listing of infix format based mathematical expressions that can be parsed and evaluated using the MathExpr library.

1. `sqrt(1 - (3 / x^2))`
1. `clamp(-1, sin(2 * pi * x) + cos(y / 2 * pi), +1)`
1. `sin(2.34e-3 * x)`
1. `if(((x[2] + 2) == 3) and ((y + 5) <= 9), 1 + w, 2 / z)`
1. `inrange(-2, m, +2) == if(({-2 <= m} and [m <= +2]), 1, 0)`
1. `({1/1}*[1/2]+(1/3)) - {1/4}^[1/5]+(1/6) - ({1/7}+[1/8]*(1/9))`
1. `a * exp(2.2 / 3.3 * t) + c`
1. `z := x + sin(2.567 * pi / y)`
1. `u := 2.123 * {pi * z} / (w := x + cos(y / pi))`
1. `2x + 3y + 4z + 5w == 2 * x + 3 * y + 4 * z + 5 * w`
1. `3(x + y) / 2.9 + 1.234e+12 == 3 * (x + y) / 2.9 + 1.234e+12`
1. `(x + y)3.3 + 1 / 4.5 == [x + y] * 3.3 + 1 / 4.5`
1. `(x + y[i])z + 1.1 / 2.7 == (x + y[i]) * z + 1.1 / 2.7`
1. `(sin(x / pi) cos(2y) + 1) == (sin(x / pi) * cos(2 * y) + 1)`
1. `75x^17 + 25.1x^5 - 35x^4 - 15.2x^3 + 40x^2 - 15.3x + 1`
1. `(avg(x,y) <= x + y ? x - y : x * y) + 2.345 * pi / x`
1. `while (x <= 100) { x -= 1; }`
1. `x <= 'abc123' and (y in 'AString') or ('1x2y3z' != z)`
1. `((x + 'abc') like '*123*') or ('a123b' ilike y)`
1. `sgn(+1.2^3.4z / -5.6y) <= {-7.8^9 / -10.11x }`

[Previous](01-capabilities.md) | [Index](index.md) | [Next](03-copyright-notice.md)
