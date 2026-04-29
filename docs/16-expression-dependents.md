# Section 16: Expression Dependents

[Back to index](index.md)

[Previous](15-user-defined-functions.md) | [Index](index.md) | [Next](17-hierarchies-of-symbol-tables.md)

Any expression that is not a literal (aka constant) will have dependencies. The types of 'dependencies' an expression can have are as follows:

1. `Variables`
1. `Vectors`
1. `Strings`
1. `Functions`
1. `Assignments`

In the following example the denoted expression has its various dependencies listed:

z := abs(x + sin(2 * pi / y))

1. Variables: x, y, z and pi
1. Functions: abs, sin
1. Assignments: z

ExprTk allows for the derivation of expression dependencies via the 'dependent_entity_collector' (DEC). When activated either through 'compile_options' at the construction of the parser or through calls to enabler methods just prior to compilation, the DEC will proceed to collect any of the relevant types that are encountered during the parsing phase. Once the compilation process has successfully completed, the caller can then obtain a list of symbols and their associated types from the DEC.

The kinds of questions one can ask regarding the dependent entities within an expression are as follows:

* What user defined variables, vectors or strings are used? * What functions or custom user functions are used? * Which variables, vectors or strings have values assigned to them?

The following example demonstrates usage of the DEC in determining the dependents of the given expression:

```cpp
typedef typename parser_t::
   dependent_entity_collector::symbol_t symbol_t;

const std::string expression_string =
   "z := abs(x + sin(2 * pi / y))";

T x,y,z;

parser_t parser;
symbol_table_t symbol_table;

symbol_table.add_variable("x",x);
symbol_table.add_variable("y",y);
symbol_table.add_variable("z",z);

expression_t expression;
expression.register_symbol_table(symbol_table);

// Collect only variable and function symbols
parser.dec().collect_variables() = true;
parser.dec().collect_functions() = true;

if (!parser.compile(expression_string,expression))
{
   // error....
}

std::deque<symbol_t> symbol_list;

parser.dec().symbols(symbol_list);

for (std::size_t i = 0; i < symbol_list.size(); ++i)
{
   const symbol_t& symbol = symbol_list[i];

   switch (symbol.second)
   {
      case parser_t::e_st_variable : ... break;
      case parser_t::e_st_vector   : ... break;
      case parser_t::e_st_string   : ... break;
      case parser_t::e_st_function : ... break;
   }
}
```

**Note 22.** The 'symbol_t' type is a std::pair comprising of the symbol name (std::string) and the associated type of the symbol as denoted by the cases in the switch statement.

Having particular symbols (variable or function) present in an expression is one form of dependency. Another and just as interesting and important type of dependency is that of assignments. Assignments are the set of dependent symbols that 'may' have their values modified within an expression. The following are example expressions and their associated assignments:

Assignments Expression

1. x x := y + z
1. x, y x += y += z
1. x, y, z x := y += sin(z := w + 2)
1. w, z if (x > y, z := x + 2, w := 'A String')
1. None x + y + z

**Note 23.** In Expression 4, both variables 'w' and 'z' are denoted as being assignments even though only one of them can ever be modified at the time of evaluation. Furthermore determining of which of the two variables the modification will occur upon can only be known with certainty at evaluation time (aka runtime) and not beforehand, hence both are listed as being candidates for assignment.

The following builds upon the previous example demonstrating the usage of the DEC in determining the 'assignments' of the given expression:

```cpp
// Collect assignments
parser.dec().collect_assignments() = true;

if (!parser.compile(expression_string,expression))
{
   // error....
}

std::deque<symbol_t> symbol_list;

parser.dec().assignment_symbols(symbol_list);

for (std::size_t i = 0; i < symbol_list.size(); ++i)
{
   symbol_t& symbol = symbol_list[i];

   switch (symbol.second)
   {
      case parser_t::e_st_variable : ... break;
      case parser_t::e_st_vector   : ... break;
      case parser_t::e_st_string   : ... break;
   }
}
```

**Note 24.** The assignments will only consist of variable types and as such will not contain symbols denoting functions.

[Previous](15-user-defined-functions.md) | [Index](index.md) | [Next](17-hierarchies-of-symbol-tables.md)
