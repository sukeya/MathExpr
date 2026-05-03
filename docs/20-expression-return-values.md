# Expression Return Values

[Back to index](index.md)

[Previous](19-enabling-and-disabling-features.md) | [Index](index.md) | [Next](21-compilation-errors.md)

MathExpr expressions can return immediately from any point by utilising the return call. Furthermore the return call can be used to transfer out multiple return values from within the expression.

If an expression evaluation exits using a return point, the result of the call to the 'value' method will be NaN, and it is expected that the return values will be available from the results_context.

In the following example there are three return points in the expression. If neither of the return points are hit, then the expression will return normally.

```cpp
const std::string expression_string =
   " if (x < y)                                   "
   "    return [x + 1,'return-call 1'];           "
   " else if (x > y)                              "
   "    return [y / 2, y + 1, 'return-call 2'];   "
   " else if (equal(x,y))                         "
   "    x + y;                                    "
   " return [x, y, x + y, x - y, 'return-call 3'] ";

typedef math_expr::symbol_table<double> symbol_table_t;
typedef math_expr::expression<double>   expression_t;
typedef math_expr::parser<double>       parser_t;

symbol_table_t symbol_table;
expression_t   expression;
parser_t       parser;

double x = 0;
double y = 0;

symbol_table.add_variable("x",x);
symbol_table.add_variable("y",y);

expression.register_symbol_table(symbol_table);

parser.compile(expression_string,expression);

T result = expression.value();

if (expression.return_invoked())
{
   typedef math_expr::results_context<T> results_context_t;
   typedef typename results_context_t::type_store_t type_t;
   typedef typename type_t::scalar_view scalar_t;
   typedef typename type_t::vector_view vector_t;
   typedef typename type_t::string_view string_t;

   const results_context_t& results = expression.results();

   for (std::size_t i = 0; i < results.count(); ++i)
   {
      type_t t = results[i];

      switch (t.type)
      {
         case type_t::store_type::e_scalar : ...
                                 break;

         case type_t::store_type::e_vector : ...
                                 break;

         case type_t::store_type::e_string : ...
                                 break;

         default               : continue;
      }
}
```

In the above example, there are three possible "return" points and one regular result. Only one of the four paths can ever be realised. Hence it is necessary to capture the result of the expression value method call. In the event, the call to return_invoked is not true then the non-return code path was executed and the result of the evaluation will be the result of the expression's value method.

**Note 31.** Processing of the return results is similar to that of the generic function call parameters.

The results_context provides getter methods for each of the possible return types (scalar, vector and string) and can be used as follows:

```cpp
typedef math_expr::symbol_table<T> symbol_table_t;
typedef math_expr::expression<T>   expression_t;
typedef math_expr::parser<T>       parser_t;

const std::string expression_str =
   " if (x > y)                                  "
   "    return [1];                              "
   " else                                        "
   "    return [ x, x + y, 2 * v, s + 'world' ]; ";

symbol_table_t symbol_table;
expression_t   expression;
parser_t       parser;

symbol_table.add_variable ("x", x);
symbol_table.add_variable ("y", y);
symbol_table.add_variable ("z", z);
symbol_table.add_vector   ("v", v);
symbol_table.add_stringvar("s", s);

parser.compile(expression_str, expression);

expression.value();

typedef math_expr::results_context<T> results_context_t;
const results_context_t& results = expression.results();

if (results.count() == 4)
{
   T result_x0;
   T result_x1;
   std::string result_s;
   std::vector<T> result_v;

   results.get_scalar(0, result_x0);
   results.get_scalar(1, result_x1);
   results.get_string(3, result_s );
   results.get_vector(2, result_v );
}
```

It is however recommended that if there is to be only a single flow of execution through the expression, that the simpler approach of registering external variables of appropriate type be used.

This method simply requires the variables that are to hold the various results that are to be computed within the expression to be registered with an associated symbol_table instance. Then within the expression itself to have the result variables be assigned the appropriate values.

```cpp
typedef math_expr::symbol_table<double> symbol_table_t;
typedef math_expr::expression<double>   expression_t;
typedef math_expr::parser<double>       parser_t;

const std::string expression_string =
   " var x := 123.456;     "
   " var s := 'ijk';       "
   " result0 := x + 78.90; "
   " result1 := s + '123'  ";

double      result0;
std::string result1;

symbol_table_t symbol_table;
symbol_table.add_variable ("result0",result0);
symbol_table.add_stringvar("result1",result1);

expression_t expression;
expression.register_symbol_table(symbol_table);

parser_t parser;
parser.compile(expression_string,expression);

expression.value();

printf("Result0: %15.5f\n", result0        );
printf("Result1: %s\n"    , result1.c_str());
```

In the example above, the expression will compute two results. As such two result variables are defined to hold the values named result0 and result1 respectively. The first is of scalar type (double), the second is of string type. Once the expression has been evaluated, the two variables will have been updated with the new result values, and can then be further utilised from within the calling host program.

There will be times when an expression may have multiple exit paths, where not all the paths will be return-statement based. The following example builds upon the previous examples, but this time at least one path is not return based.

```cpp
typedef math_expr::symbol_table<double> symbol_table_t;
typedef math_expr::expression<double>   expression_t;
typedef math_expr::parser<double>       parser_t;

double x = 100.0;
double y = 200.0;

symbol_table_t symbol_table;
expression_t   expression;
parser_t       parser;

symbol_table.add_variable ("x", x);
symbol_table.add_variable ("y", y);

expression.register_symbol_table(symbol_table);

const std::string expression_string =
   " for (var i := 0; i < 10; i += 1)       "
   " {                                      "
   "    if (i > x)                          "
   "    {                                   "
   "       return [x + y, 'return-call 1']; "
   "    }                                   "
   "    else if (i > y)                     "
   "    {                                   "
   "       return [x - y, 'return-call 2']; "
   "    }                                   "
   " };                                     "
   "                                        "
   " x / y                                  ";

parser.compile(expression_str, expression);

const auto result = expression.value();

if (expression.return_invoked())
{
   const auto results = expression.results();

   for (std::size_t i = 0; i <  results.count(); ++i)
   {
      const auto& rtrn_result = results[i];
      .
      .
      .
   }
}
else
{
   printf("result: %f\n",result);
}
```

After having called the value method on the expression, calling the return_invoked method will determine if the expression completed due to a return statement being invoked or if it finished normally.

[Previous](19-enabling-and-disabling-features.md) | [Index](index.md) | [Next](21-compilation-errors.md)
