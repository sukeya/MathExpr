# Section 17: Hierarchies Of Symbol Tables

[Back to index](index.md)

[Previous](16-expression-dependents.md) | [Index](index.md) | [Next](18-unknown-unknowns.md)

Most situations will only require a single symbol_table instance to be associated with a given expression instance.

However as an expression can have more than one symbol table instance associated with itself, when building more complex systems that utilise many expressions where each can in turn utilise one or more variables from a large set of potential variables, functions or constants, it becomes evident that grouping variables into layers of symbol_tables will simplify and streamline the overall process.

A recommended hierarchy of symbol tables is the following:

1. Global constant value symbol table
1. Global non side-effect functions symbol table
1. Global variable symbol table
1. Expression specific variable symbol table

## Global constant value symbol table

This symbol table will contain constant variables denoting immutable values. These variables can be made available to all expressions, and in turn expressions will assume the values themselves will never be modified for the duration of the process run-time. Examples of such variables are:

1. pi or e
1. `speed_of_light`
1. `avogadro_number`
1. `num_cpus`

## Global non side-effect functions symbol table

This symbol table will contain only user defined functions that will not incur any side-effects that are observable to any of the expressions that invoke them. These functions shall be thread-safe or threading invariant and will not maintain any form of state between invocations. Examples of such functions are:

1. calc_volume_of_sphere(r)
1. distance(x0,y0,x1,y1)

## Global variable symbol table

This symbol table will contain variables that will be accessible to all associated expressions and will not be specific or exclusive to any one expression. This variant differs from (a) in that the values of the variables can change (or be updated) between evaluations of expressions - but through properly scheduled evaluations are guaranteed to never change during the evaluation of any dependent expressions. Furthermore it is assumed that these variables will be used in a read-only context and that no expressions will attempt to modify these variables via assignments or other means.

1. `price_of_stock_xyz`
1. outside_temperature or inside_temperature
1. `fuel_in_tank`
1. `num_customers_in_store`
1. `num_items_on_shelf`

## Expression specific variable symbol table

This symbol_table is the most common form, and is used to store variables that are specific and exclusive to a particular expression. That is to say references to variables in this symbol_table will not be part of another expression. Though it may be possible to have expressions that contain the variables with the same name, in that case those variables will be distinctly different. Which would mean if a particular expression were to be compiled twice, each expression would have its own unique symbol_table which in turn would have its own instances of those variables. Examples of such variables could be:

1. x or y
1. `customer_name`

The following is a diagram depicting a possible variant of the denoted symbol table hierarchies. In the diagram there are two unique expressions, each of which have a reference to the Global constant, functions and variables symbol tables and an exclusive reference to a local symbol table.

```text
  +-------------------------+    +-------------------------+
  |     Global Constants    |    |     Global Functions    |
  |       Symbol Table      |    |       Symbol Table      |
  +----o--o-----------------+    +--------------------o----+
       |  |                                           |
       |  |                                           +-------+
       |  +------------------->----------------------------+  |
       |         +----------------------------+            |  |
       |         |      Global Variables      |            |  |
       |  +------o        Symbol Table        o-----+      |  V
       |  |      +----------------------------+     |      |  |
       |  |                                         |      |  |
       |  | +----------------+   +----------------+ |      |  |
       |  | | Symbol Table 0 |   | Symbol Table 1 | |      V  |
       |  | +--o-------------+   +--o-------------+ |      |  |
       |  |    |                    |               |      |  |
       |  |    |                    |               |      |  |
    +--V--V----V---------+        +-V---------------V--+   |  |
    |    Expression 0    |        |    Expression 1    |<--+--+
    |  '2 * sin(x) - y'  |        |  'k + abs(x - y)'  |
    +--------------------+        +--------------------+
```

Bringing all of the above together, in the following example the hierarchy of symbol tables are instantiated and initialised. An expression that makes use of various elements of each symbol table is then compiled and later on evaluated:

```cpp
typedef math_expr::symbol_table<double> symbol_table_t;
typedef math_expr::expression<double>   expression_t;

// Setup global constants symbol table
symbol_table_t glbl_const_symbol_table;
glbl_const_symbtab.add_constants(); // pi, epsilon and inf
glbl_const_symbtab.add_constant("speed_of_light",299e6);
glbl_const_symbtab.add_constant("avogadro_number",6e23);

// Setup global function symbol table
symbol_table_t glbl_funcs_symbol_table;
glbl_func_symbtab.add_function('distance',distance);
glbl_func_symbtab.add_function('calc_spherevol',calc_sphrvol);

......

// Setup global variable symbol table
symbol_table_t glbl_variable_symbol_table;
glbl_variable_symbtab.add_variable('temp_outside',thermo.outside);
glbl_variable_symbtab.add_variable('temp_inside' ,thermo.inside );
glbl_variable_symbtab.add_variable('num_cstmrs',store.num_cstmrs);

......

double x,y,z;

// Setup expression specific symbol table
symbol_table_t symbol_table;
symbol_table.add_variable('x',x);
symbol_table.add_variable('y',y);
symbol_table.add_variable('z',z);

expression_t expression;

// Register the various symbol tables
expression
   .register_symbol_table(symbol_table);

expression
   .register_symbol_table(glbl_funcs_symbol_table);

expression
   .register_symbol_table(glbl_const_symbol_table);

expression
   .register_symbol_table(glbl_variable_symbol_table);

const std::string expression_str =
   "abs(temp_inside - temp_outside) + 2 * speed_of_light / x";

parser_t parser;
parser.compile(expression_str,expression);

......

while (keep_evaluating)
{
  ....

  T result = expression.value();

  ....
}
```

[Previous](16-expression-dependents.md) | [Index](index.md) | [Next](18-unknown-unknowns.md)
