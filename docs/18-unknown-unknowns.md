# Section 18: Unknown Unknowns

[Back to index](index.md)

[Previous](17-hierarchies-of-symbol-tables.md) | [Index](index.md) | [Next](19-enabling-and-disabling-features.md)

In this section we will discuss the process of handling expressions with a mix of known and unknown variables. Initially a discussion into the types of expressions that exist will be provided, then a series of possible solutions will be presented for each scenario.

When parsing an expression, there may be situations where one is not fully aware of what if any variables will be used prior to the expression being compiled.

This can become problematic, as in the default scenario it is assumed the symbol_table that is registered with the expression instance will already possess the externally available variables, functions and constants needed during the compilation of the expression.

In the event there are symbols in the expression that can't be mapped to either a reserved word, or located in the associated symbol_table(s), an "Undefined symbol" error will be raised and the compilation process will fail.

The numerous scenarios that can occur when compiling an expression with ExprTk generally fall into one of the following three categories:

1. No external variables
1. Predetermined set of external variables
1. Unknown set of variables

## No external variables

These are expressions that contain no external variables but may contain local variables. As local variables cannot be accessed externally from the expression, it is assumed that such expressions will not have a need for a symbol_table and furthermore expressions which don't make use of functions that have side-effects will be evaluated completely at compile time resulting in a constant return value. The following are examples of such expressions:

1. 1 + 2
1. var x := 3; 2 * x - 3
1. var x := 3; var y := abs(x - 8); x - y / 7

## Predetermined set of external variables

These are expressions that are comprised of externally available variables and functions and will only compile successfully if the symbols that correspond to the variables and functions are already defined in their associated symbol_table(s). This is by far the most common scenario when using ExprTk.

As an example, one may have three external variables: x, y and z which have been registered with the associated symbol_table, and will then need to compile and evaluate expressions comprised of any subset of these three variables. The following are a few examples of such expressions:

1. 1 + x
1. x / y
1. 2 * x * y / z

In this scenario one can use the 'dependent_entity_collector' component as described in [Section 16] to further determine which of the registered variables were actually used in the given expression. As an example once the set of utilised variables are known, any further 'attention' can be restricted to only those variables when evaluating the expression. This can be quite useful when dealing with expressions that can draw from a set of hundreds or even thousands of variables.

## Unknown set of variables

These are expressions that are comprised of symbols other than the standard ExprTk reserved words or what has been registered with their associated symbol_table, and will normally fail compilation due to the associated symbol_table not having a reference to them. As such this scenario can be seen as a combination of scenario B, where one may have a symbol_table with registered variables, but would also like to handle the situation of variables that aren't present in said symbol_table.

When dealing with expressions of category (c), one must perform all of the following:

1. Determine the variables used in the expression
1. Populate a symbol_table(s) with the entities from (1)
1. Compile the expression
1. Provide a means by which the entities from (1) can be modified

Depending on the nature of processing, steps (1) and (2) can be done either independently of each other or combined into one. The following example will initially look at solving the problem of unknown variables with the latter method using the 'unknown_symbol_resolver' component.

```cpp
typedef math_expr::symbol_table<T> symbol_table_t;
typedef math_expr::expression<T>   expression_t;
typedef math_expr::parser<T>       parser_t;

T x = T(123.456);
T y = T(789.123);

symbol_table_t unknown_var_symbol_table;

symbol_table_t symbol_table;
symbol_table.add_variable("x",x);
symbol_table.add_variable("y",y);

expression_t expression;
expression.register_symbol_table(unknown_var_symbol_table);
expression.register_symbol_table(symbol_table);

parser_t parser;
parser.enable_unknown_symbol_resolver();

const std::string expression_str = "x + abs(y / 3k) * z + 2";

parser.compile(expression_str,expression);
```

In the example above, the symbols 'k' and 'z' will be treated as unknown symbols. The parser in the example is set to handle unknown symbols using the built-in default unknown_symbol_resolver (USR). The default USR will automatically resolve any unknown symbols as a variable (scalar type). The new variables will be added to the primary symbol_table, which in this case is the 'unknown_var_symbol_table' instance. Once the compilation has completed successfully, the variables that were resolved during compilation can be accessed from the primary symbol_table using the 'get_variable_list' and 'variable_ref' methods and then if needed can be modified accordingly after which the expression itself can be evaluated.

```cpp
std::vector<std::string> variable_list;

unknown_var_symbol_table.get_variable_list(variable_list);

for (const auto& var_name : variable_list)
{
   T& v = unknown_var_symbol_table.variable_ref(var_name);

   v = ...;
}

...

expression.value();
```

**Note 25.** As previously mentioned the default USR will automatically assume any unknown symbol to be a valid scalar variable, and will then proceed to add said symbol as a variable to the primary symbol_table of the associated expression during the compilation process. However a problem that may arise, is that expressions that are parsed with the USR enabled, but contain 'typos' or otherwise syntactic errors may inadvertently compile successfully due to the simplistic nature of the default USR. The following are some example expressions:

1. 1 + abz(x + 1)
1. sine(y / 2) - coz(3x)

The two expressions above contain misspelt symbols (abz, sine, coz) which if implied multiplications and default USR are enabled during compilation will result in them being assumed to be valid 'variables', which obviously is not the intended outcome by the user. A possible solution to this problem is for one to implement their own specific USR that will perform a user defined business logic in determining if an encountered unknown symbol should be treated as a variable or if it should raise a compilation error. The following example demonstrates a simple user defined USR:

```cpp
typedef math_expr::symbol_table<T> symbol_table_t;
typedef math_expr::expression<T>   expression_t;
typedef math_expr::parser<T>       parser_t;

template <typename T>
struct my_usr final : public parser_t::unknown_symbol_resolver
{
   typedef typename parser_t::unknown_symbol_resolver usr_t;

   bool process(const std::string& unknown_symbol,
                typename usr_t::usr_symbol_type& st,
                T& default_value,
                std::string& error_message) override
   {
      if (0 != unknown_symbol.find("var_"))
      {
         error_message = "Invalid symbol: " + unknown_symbol;
         return false;
      }

      st = usr_t::e_usr_variable_type;
      default_value = T(123.123);

      return true;
   }
};

...

T x = T(123.456);
T y = T(789.123);

symbol_table_t unknown_var_symbol_table;

symbol_table_t symbol_table;
symbol_table.add_variable("x",x);
symbol_table.add_variable("y",y);

expression_t expression;
expression.register_symbol_table(unknown_var_symbol_table);
expression.register_symbol_table(symbol_table);

my_usr<T> musr;

parser_t parser;
parser.enable_unknown_symbol_resolver(&musr);

std::string expression_str = "var_x + abs(var_y - 3) * var_z";

parser.compile(expression_str,expression);
```

In the example above, a user specified USR is defined, and is registered with the parser enabling the USR functionality. Subsequently during the compilation process when an unknown symbol is encountered, the USR's process method will be invoked. The USR in the example will only 'accept' unknown symbols that have a prefix of 'var_' as being valid variables, all other unknown symbols will result in a compilation error being raised.

In the example above the callback of the USR that is invoked during the unknown symbol resolution process only allows for scalar variables to be defined and resolved - as that is the simplest and most common form.

There is a further extended version of the callback that can be overridden that will allow for more control and choice over the type of symbol being resolved. The following is an example definition of said extended callback:

```cpp
template <typename T>
struct my_usr final : public parser_t::unknown_symbol_resolver
{
  typedef typename parser_t::unknown_symbol_resolver usr_t;

  my_usr()
  : usr_t(usr_t::e_usrmode_extended)
  {}

  bool process(const std::string& unknown_symbol,
               symbol_table_t&    symbol_table,
               std::string&       error_message) override
  {
     bool result = false;

     if (0 == unknown_symbol.find("var_"))
     {
        // Default value of zero
        result = symbol_table.create_variable(unknown_symbol,0);

        if (!result)
        {
           error_message = "Failed to create variable...";
        }
     }
     else if (0 == unknown_symbol.find("str_"))
     {
        // Default value of empty string
        result = symbol_table.create_stringvar(unknown_symbol,"");

        if (!result)
        {
           error_message = "Failed to create string variable...";
        }
     }
     else
        error_message = "Indeterminable symbol type.";

     return result;
  }
};
```

In the example above, the USR callback when invoked will pass the primary symbol table associated with the expression being parsed. The symbol resolution business logic can then determine under what conditions a symbol will be resolved including its type (scalar, string, vector etc) and default value. When the callback successfully returns the symbol parsing and resolution process will again be executed by the parser. The idea here is that given the primary symbol table will now have the previously detected unknown symbol registered, it will be correctly resolved and the general parsing processing can then resume as per normal.

**Note 26.** In order to have the USR's extended mode callback be invoked it is necessary to pass the e_usrmode_extended enum value during the constructor of the user defined USR.

**Note 27.** The primary symbol table for an expression is the first symbol table to be registered with that instance of the expression.

**Note 28.** For a successful symbol resolution using the normal USR all of the following are required:

1. Only if successful shall the process method return TRUE
1. The default_value parameter will have been set
1. The error_message parameter will be empty
1. usr_symbol_type input parameter field will be set to either:
1. `e_usr_variable_type`
1. `e_usr_constant_type`

**Note 29.** For a successful symbol resolution using the extended USR all of the following are required:

1. Only if successful shall the process method return TRUE
1. symbol_table parameter will have had the newly resolved variable or string added to it
1. error_message parameter will be empty

[Previous](17-hierarchies-of-symbol-tables.md) | [Index](index.md) | [Next](19-enabling-and-disabling-features.md)
