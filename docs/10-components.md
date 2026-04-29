# Section 10: Components

[Back to index](index.md)

[Previous](09-fundamental-types.md) | [Index](index.md) | [Next](11-compilation-options.md)

There are three primary components, that are specialised upon a given numeric type, which make up the core of ExprTk. The components are as follows:

1. Symbol Table math_expr::symbol_table<NumericType>
1. Expression math_expr::expression<NumericType>
1. Parser math_expr::parser<NumericType>

## Symbol Table

A structure that is used to store references to variables, constants and functions that are to be used within expressions. Furthermore in the context of composited recursive functions the symbol table can also be thought of as a simple representation of a stack specific for the expression(s) that reference it. The following is a list of the types a symbol table can handle:

1. Numeric variables
1. Numeric constants
1. Numeric vector elements
1. String variables
1. String constants
1. `Functions`
1. Vararg functions

During the compilation process if an expression is found to require any of the elements noted above, the expression's associated symbol_table will be queried for the element and if present a reference to the element will be embedded within the expression's AST. This allows for the original element to be modified independently of the expression instance and to also allow the expression to be evaluated using the current value of the element.

The example below demonstrates the relationship between variables, symbol_table and expression. Note the variables are modified as they normally would in a program, and when the expression is evaluated the current values assigned to the variables shall be used.

```cpp
typedef math_expr::symbol_table<double> symbol_table_t;
typedef math_expr::expression<double>   expression_t;
typedef math_expr::parser<double>       parser_t;

double x = 0;
double y = 0;

symbol_table_t symbol_table;
expression_t   expression;
parser_t       parser;

std::string expression_string = "x * y + 3";

symbol_table.add_variable("x",x);
symbol_table.add_variable("y",y);

expression.register_symbol_table(symbol_table);

parser.compile(expression_string,expression);

x = 1.0;
y = 2.0;
expression.value(); // 1 * 2 + 3

x = 3.7;
expression.value(); // 3.7 * 2 + 3

y = -9.0;
expression.value(); // 3.7 * -9 + 3

// 'x * -9 + 3' for x in range of [0,100) in steps of 0.0001
for (x = 0.0; x < 100.0; x += 0.0001)
{
   expression.value(); // x * -9 + 3
}
```

**Note 02.** Any variable reference provided to a given symbol_table instance, must have a lifetime at least as long as the lifetime of the symbol_table instance. In the event the variable reference is invalidated before the symbol_table or any dependent expression instances have been destructed, then any associated expression evaluations or variable referencing via the symbol_table instance will result in undefined behaviour.

The following bit of code instantiates a symbol_table and expression instance, then proceeds to demonstrate various ways in which references to variables can be added to the symbol_table, and how those references are subsequently invalidated resulting in various forms of undefined behaviour.

```cpp
typedef math_expr::symbol_table<double> symbol_table_t;
typedef math_expr::expression<double>   expression_t;

symbol_table_t symbol_table;
expression_t   expression;

std::deque<double > y {1.1, 2.2, 3.3};
std::vector<double> z {4.4, 5.5, 6.6};
double* w = new double(123.456);

{
   double x = 123.4567;
   symbol_table.add_variable("x", x);
}             // Reference to variable x has been invalidated


symbol_table.add_variable("y", y.back());

y.pop_back(); // Reference to variable y has been invalidated


symbol_table.add_variable("z", z.front());

z.erase(z.begin());
              // Reference to variable z has been invalidated

symbol_table.add_variable("w", *w);

delete w;     // Reference to variable w has been invalidated

const std::string expression_string = "x + y / z * w";

              // Compilation of expression will succeed
parser.compile(expression_string,expression);

expression.value();
              // Evaluation will result in undefined behaviour
              // due to 'x' and 'w' having been destroyed.

symbol_table.get_variable("x")->ref() = 135.791;
              // Assignment will result in undefined behaviour
```

A compiled expression that references variables from a symbol_table is said to be dependent on that symbol_table instance and the variables it holds being valid.

```cpp
typedef math_expr::symbol_table<double> symbol_table_t;
typedef math_expr::expression<double>   expression_t;

symbol_table_t symbol_table;
expression_t   expression;

double x = 123.456;

symbol_table.add_variable("x", x);

const std::string expression_string = "(x + 1) / 2";

              // Compilation of the expression will succeed
parser.compile(expression_string,expression);

              // Clear all variables from symbol_table
symbol_table.clear();

expression.value();
              // Evaluation will result in undefined behaviour
              // because the reference to 'x' having been destroyed
              // during the clearing of the symbol_table
```

In the above example, an expression is compiled that references variable "x". As part of the compilation process the node holding the variable "x" is obtained from the symbol_table and embedded in the AST of the expression - in short the expression is now referencing the node that holds the variable "x". The following diagram depicts the dependencies between the variable x, the symbol table and the expression:

```text
      +--[Symbol Table]--+
      |                  |
      | +- ------+       |
      | | x-node |       |
      | +-----A--+       |    +--[Expression]--+
      +---|---|----------+    |  +---------+   |
```

v | | | A.S.T | |

```text
          |   +--------<--------[.]        |   |
    +-----+                   |  +---------+   |
    |                         +----------------+
  +-v-[variable]---+
  | x: 123.456     |
  +----------------+
```

When the clear method is called on the symbol table the X-Node is destroyed, so now the expression is referencing a node that has been destroyed. From this point onwards any attempts to reference the expression instance will result in undefined behaviour. Simply put the above example violates the requirement that the lifetime of any objects referenced by expressions should exceed the lifetime of the expression instance.

```cpp
typedef math_expr::symbol_table<double> symbol_table_t;
typedef math_expr::expression<double>   expression_t;

symbol_table_t symbol_table;
expression_t   expression;

double x = 123.456;

symbol_table.add_variable("x", x);

const std::string expression_string = "(x + 1) / 2";

              // Compilation of the expression will succeed
parser.compile(expression_string,expression);

expression.value();

              // Release the expression and its dependents
expression.release();

              // Clear all variables from symbol_table
symbol_table.clear();

expression.value();
              // Will return null_node value of NaN
```

In the above example the expression is released before the associated symbol_table is cleared of its variables, which resolves the undefined behaviour issue noted in the previous example.

**Note 03.** It is possible to register multiple symbol_tables with a single expression object. In the event an expression has multiple symbol tables, and where there exists conflicts between symbols, the compilation stage will resolve the conflicts based on the order of registration of the symbol_tables to the expression. For a more expansive discussion please review section [17 - Hierarchies Of Symbol Tables]

```cpp
typedef math_expr::symbol_table<double> symbol_table_t;
typedef math_expr::expression<double>   expression_t;
typedef math_expr::parser<double>       parser_t;

symbol_table_t symbol_table0;
symbol_table_t symbol_table1;

expression_t   expression;
parser_t       parser;

double x0 = 123.0;
double x1 = 678.0;

std::string expression_string = "x + 1";

symbol_table0.add_variable("x",x0);
symbol_table1.add_variable("x",x1);

expression.register_symbol_table(symbol_table0);
expression.register_symbol_table(symbol_table1);

parser.compile(expression_string,expression);

expression.value(); // 123 + 1
```

The symbol table supports adding references to external instances of types that can be accessed within expressions via the following methods:

1. bool add_variable (const std::string& name, scalar_t& )
1. bool add_constant (const std::string& name, const scalar_t& )
1. bool add_stringvar (const std::string& name, std::string& )
1. bool add_vector (const std::string& name, vector_type& )
1. bool add_function (const std::string& name, function_t& )
1. bool create_stringvar(const std::string& name,const std::string&)
1. bool create_variable (const std::string& name, const T& )

**Note 04.** The 'vector' type must be comprised from a contiguous array of scalars with a size that is larger than zero. The vector type itself can be any one of the following:

1. std::vector<scalar_t>
1. scalar_t(&v)[N]
1. scalar_t* and array size
1. math_expr::vector_view<scalar_t>

When registering a variable, vector, string or function with an instance of a symbol_table, the call to 'add_...' may fail and return a false result due to one or more of the following reasons:

1. Variable name contains invalid characters or is ill-formed
1. Variable name conflicts with a reserved word (eg: 'while')
1. Variable name conflicts with a previously registered variable
1. A vector of size (length) zero is being registered
1. A free function exceeding fifteen parameters is being registered
1. The symbol_table instance is in an invalid state

**Note 05.** The symbol_table has a method called clear, which when invoked will clear all variables, vectors, strings and functions registered with the symbol_table instance. If this method is to be called, then one must make sure that all compiled expression instances that reference variables belonging to that symbol_table instance are released (aka call release method on expression) before calling the clear method on the symbol_table instance, otherwise undefined behaviours will occur.

A further property of symbol tables is that they can be classified at instantiation as either being mutable (by default) or immutable. This property determines if variables, vectors or strings registered with the symbol table can undergo modifications within expressions that reference them. The following demonstrates construction of an immutable symbol table instance:

```cpp
symbol_table_t immutable_symbol_table
   (symbol_table_t::symtab_mutability_type::e_immutable);
```

When a symbol table, that has been constructed as being immutable, is registered with an expression, any statements in the expression string that modify the variables that are managed by the immutable symbol table will result in a compilation error. The operations that trigger the mutability constraint are the following assignment operators:

1. Assignment: :=
1. Assign operation: +=, -=, *=, /= , %=

```cpp
const std::string expression_str = "x += x + 123.456";

symbol_table_t immutable_symbol_table
   (symbol_table_t::symtab_mutability_type::e_immutable);

T x = 0.0;

immutable_symbol_table.add_variable("x" , x);

expression_t expression;
expression.register_symbol_table(immutable_symbol_table);

parser_t parser;

parser.compile(expression_str, expression);
   // Compile error because of assignment to variable x
```

In the above example, variable x is registered to an immutable symbol table, making it an immutable variable within the context of any expressions that reference it. The expression string being compiled uses the addition assignment operator which will modify the value of variable x. The compilation process detects this semantic violation and proceeds to halt compilation and return the appropriate error.

One of the main reasons for this functionality is that, one may want the immutability properties that come with constness of a variable such as scalars, vectors and strings, but not necessarily the accompanying compile time const-folding optimisations, that would result in the value of the variables being retrieved only once at compile time, causing external updates to the variables to not be part of the expression evaluation.

```cpp
symbol_table_t immutable_symbol_table
   (symbol_table_t::symtab_mutability_type::e_immutable);

T x = 0.0;

const std::string expression_str = "x + (y + y)";

immutable_symbol_table.add_variable("x" , x    );
immutable_symbol_table.add_constant("y" , 123.0);

expression_t expression;
expression.register_symbol_table(immutable_symbol_table);

parser_t parser;
parser.compile(expression_str, expression);

for (; x < 10.0; ++x)
{
   const auto expected_value = x + (123.0 + 123.0);
   const auto result_value   = expression.value();
   assert(expression.value() !=  expected_value);
}
```

In the above example, there are two variables X and Y. Where Y is a constant and X is a normal variable. Both are registered with a symbol table that is immutable. The expression when compiled will result in the "(y + y)" part being const-folded at compile time to the literal value of 246. Whereas the current value of X, being updated via the for-loop, externally to the expression and the symbol table shall be observable to the expression upon each evaluation.

## Expression

A structure that holds an Abstract Syntax Tree or AST for a specified expression and is used to evaluate said expression. Evaluation of the expression is accomplished by performing a post-order traversal of the AST. If a compiled Expression uses variables or user defined functions, it will have an associated Symbol Table, which will contain references to said variables, functions or strings. An example AST structure for the denoted expression is as follows:

```text
Expression:  z := (x + y^-2.345) * sin(pi / min(w - 7.3,v))

                  [Root]
                    |
               [Assignment]
        ________/        \_____
       /                       \
 Variable(z)            [Multiplication]
                ____________/      \___________
               /                               \
              /                      [Unary-Function(sin)]
         [Addition]                            |
      ____/      \____                    [Division]
     /                \                 ___/      \___
 Variable(x)   [Exponentiation]        /              \
              ______/   \______  Constant(pi) [Binary-Function(min)]
             /                 \                ____/    \___
        Variable(y)        [Negation]          /             \
                               |              /          Variable(v)
                        Constant(2.345)      /
                                            /
                                     [Subtraction]
                                   ____/      \____
                                  /                \
                             Variable(w)      Constant(7.3)
```

The above denoted AST shall be evaluated in the following order:

1. Load Variable (z) (10) Load Constant (7.3)
1. Load Variable (x) (11) Subtraction (09 & 10)
1. Load Variable (y) (12) Load Variable (v)
1. Load Constant (2.345) (13) Min (11 & 12)
1. Negation (04) (14) Division (08 & 13)
1. Exponentiation (03 & 05) (15) Sin (14)
1. Addition (02 & 06) (16) Multiplication (07 & 15)
1. Load Constant (pi) (17) Assignment (01 & 16)
1. Load Variable (w)

Generally an expression in ExprTk can be thought of as a free function similar to those found in imperative languages. This form of pseudo function will have a name, it may have a set of one or more inputs and will return at least one value as its result. Furthermore the function when invoked, may cause a side-effect that changes the state of the host program.

As an example the following is a pseudo-code definition of a free function that performs a computation taking four inputs, modifying one of them and returning a value based on some arbitrary calculation:

```cpp
ResultType foo(InputType x, InputType y, InputType z, InputType w)
{
   w = 2 * x^y + z;        // Side-Effect
   return abs(x - y) / z;  // Return Result
}
```

Given the above definition the following is a functionally equivalent version using ExprTk:

```cpp
const std::string foo_str =
   " w := 2 * x^y + z; "
   " abs(x - y) / z;   ";

T x, y, z, w;

symbol_table_t symbol_table;
symbol_table.add_variable("x",x);
symbol_table.add_variable("y",y);
symbol_table.add_variable("z",z);
symbol_table.add_variable("w",w);

expression_t foo;
foo.register_symbol_table(symbol_table);

parser_t parser;
if (!parser.compile(foo_str,foo))
{
   // Error in expression...
   return;
}

T result = foo.value();
```

## Parser

A component which takes as input a string representation of an expression and attempts to compile said input with the result being an instance of Expression. If an error is encountered during the compilation process, the parser will stop compiling and return an error status code, with a more detailed description of the error(s) and its location within the input provided by the 'get_error' interface.

**Note 06.** The math_expr::expression and math_expr::symbol_table components are reference counted entities. Copy constructing or assigning to or from either component will result in a shallow copy and a reference count increment, rather than a complete replication. Furthermore the expression and symbol_table components being Default-Constructible, Copy-Constructible and Copy-Assignable make them compatible with various C++ standard library containers and adaptors such as std::vector, std::map, std::stack etc.

The following is an example of two unique expressions, after having been instantiated and compiled, one expression is assigned to the other. The diagrams depict their initial and post assignment states, including which control block each expression references and their associated reference counts.

```text
  math_expr::expression e0; // constructed expression, eg: x + 1
  math_expr::expression e1; // constructed expression, eg: 2z + y

+-----[ e0 cntrl block]----+     +-----[ e1 cntrl block]-----+
| 1. Expression Node 'x+1' |     | 1. Expression Node '2z+y' |
| 2. Ref Count: 1          |<-+  | 2. Ref Count: 1           |<-+
+--------------------------+  |  +---------------------------+  |
                              |                                 |
  +--[ e0 expression]--+      |    +--[ e1 expression]--+       |
  | 1. Reference to    ]------+    | 1. Reference to    ]-------+
  | e0 Control Block   |           | e1 Control Block   |
  +--------------------+           +--------------------+


 e0 = e1; // e0 and e1 are now 2z+y

             +-----[ e1 cntrl block]-----+
             | 1. Expression Node '2z+y' |
+----------->| 2. Ref Count: 2           |<----------+
|            +---------------------------+           |
|                                                    |
|   +--[ e0 expression]--+  +--[ e1 expression]--+   |
+---[ 1. Reference to    |  | 1. Reference to    ]---+
    | e1 Control Block   |  | e1 Control Block   |
    +--------------------+  +--------------------+
```

The reason for the above complexity and restrictions of deep copies for the expression and symbol_table components is because expressions may include user defined variables or functions. These are embedded as references into the expression's AST. When copying an expression, said references need to also be copied. If the references are blindly copied, it will then result in two or more identical expressions utilising the exact same references for variables. This obviously is not the default assumed scenario and will give rise to non-obvious behaviours when using the expressions in various contexts such as multi-threading et al.

The prescribed method for cloning an expression is to compile it from its string form. Doing so will allow the 'user' to properly consider the exact source of user defined variables and functions.

**Note 07.** The math_expr::parser is a non-copyable and non-thread safe component, and should only be shared via either a reference, a shared pointer or a std::ref mechanism, and considerations relating to synchronisation taken into account where appropriate. The parser represents an object factory, specifically a factory of expressions, and generally should not be instantiated solely on a per expression compilation basis.

The following diagram and example depicts the flow of data and operations for compiling multiple expressions via the parser and inserting the newly minted math_expr::expression instances into a std::vector.

```text
                      +----[math_expr::parser]---+
                      |   Expression Factory  |
                      | parser_t::compile(...)|
                    +--> ~.~.~.~.~.~.~.~.~.~ ->--+
                    | +-----------------------+  |
 Expressions in     |                            |  Expressions as
 string form        ^                            V  math_expr::expression
                    |                            |  instances
 [s0:'x+1']--->--+  |                            |  +-[e0: x+1]
                  |  |                            |  |
 [s1:'2z+y']-->--+--+                            +->+-[e1: 2z+y]
                  |                                  |
 [s2:'sin(k+w)']-+                                  +-[e2: sin(k+w)]
```

```cpp
const std::string expression_str[3] =
   {
      "x + 1",
      "2x + y",
      "sin(k + w)"
   };

std::vector<expression_t> expression_list;

parser_t       parser;
expression_t   expression;
symbol_table_t symbol_table;

expression.register_symbol_table(symbol_table);

for (std::size_t i = 0; i < 3; ++i)
{
   if (parser.compile(expression_str[i],expression))
   {
      expression_list.push_back(expression);
   }
   else
     std::cout << "Error in " << expression_str[i] << "\n";
}

for (auto& e : expression_list)
{
   e.value();
}
```

[Previous](09-fundamental-types.md) | [Index](index.md) | [Next](11-compilation-options.md)
