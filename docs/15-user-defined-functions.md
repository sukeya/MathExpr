# User Defined Functions

[Back to index](index.md)

[Previous](14-vector-processing.md) | [Index](index.md) | [Next](16-expression-dependents.md)

MathExpr provides a means whereby custom functions can be defined and utilised within expressions. The concept requires the user to provide a reference to the function coupled with an associated name that will be invoked within expressions. Functions may take numerous inputs but will always return a single value of the underlying numeric type.

During expression compilation when required the reference to the function shall be obtained from the associated symbol_table and be embedded into the expression.

There are six types of function interface:

```text
  +---+----------------------+--------------+----------------------+
  | # |         Name         | Return Type  | Input Types          |
  +---+----------------------+--------------+----------------------+
  | 1 | ifunction            | Scalar       | Scalar               |
  | 2 | ivararg_function     | Scalar       | Scalar               |
  | 3 | igeneric_function    | Scalar       | Scalar,Vector,String |
  | 4 | igeneric_function II | String       | Scalar,Vector,String |
  | 5 | igeneric_function III| String/Scalar| Scalar,Vector,String |
  | 6 | function_compositor  | Scalar       | Scalar               |
  +---+----------------------+--------------+----------------------+
```

## ifunction

This interface supports zero to 20 input parameters of only the scalar type (numbers). The usage requires a custom function be derived from ifunction and to override one of the 21 function operators. As part of the constructor the custom function will define how many parameters it expects to handle. The following example defines a 3 parameter function called 'foo':

```cpp
template <typename T>
struct foo final : public math_expr::ifunction<T>
{
   foo() : math_expr::ifunction<T>(3)
   {}

   T operator()(const T& v1, const T& v2, const T& v3) override
   {
      return T(1) + (v1 * v2) / T(v3);
   }
};
```

## ivararg_function

This interface supports a variable number of scalar arguments as input into the function. The function operator interface uses a std::vector specialised upon type T to facilitate parameter passing. The following example defines a vararg function called 'boo':

```cpp
template <typename T>
struct boo final : public math_expr::ivararg_function<T>
{
   inline T operator()(const std::vector<T>& arglist) override
   {
      T result = T(0);

      for (std::size_t i = 0; i < arglist.size(); ++i)
      {
         result += arglist[i] / arglist[i > 0 ? (i - 1) : 0];
      }

      return result;
   }
};
```

## igeneric_function

This interface supports a variable number of arguments and types as input into the function. The function operator interface uses a std::vector specialised upon the type_store type to facilitate parameter passing.

Scalar <-- function(i_0, i_1, i_2....., i_N)

The fundamental types that can be passed into the function as parameters and their views are as follows:

1. Scalar - scalar_view
1. Vector - vector_view
1. String - string_view

The above denoted type views provide non-const reference-like access to each parameter, as such modifications made to the input parameters will persist after the function call has completed. The following example defines a generic function called 'too':

```cpp
template <typename T>
struct too final : public math_expr::igeneric_function<T>
{
   typedef typename math_expr::igeneric_function<T>::parameter_list_t
                                                  parameter_list_t;

   too()
   {}

   inline T operator()(parameter_list_t parameters) override
   {
      for (std::size_t i = 0; i < parameters.size(); ++i)
      {
         ...
      }

      return T(0);
   }
};
```

In the example above, the input 'parameters' to the function operator, parameter_list_t, is a type of std::vector of type_store. Each type_store instance has a member called 'type' which holds the enumeration pertaining to the underlying type of the type_store. There are three type enumerations:

## e_scalar - literals, variables, vector elements, expressions

eg: 123.456, x, vec[3x + 1], 2x + 3

## e_vector - vectors, vector expressions

eg: vec1, 2 * vec1 + vec2 / 3

## e_string - strings, string literals and range variants of both

eg: 'AString', s0, 'AString'[x:y], s1[1 + x:] + 'AString'

Each of the parameters can be accessed using its designated view. A typical loop for processing the parameters is as follows:

inline T operator()(parameter_list_t parameters)

```cpp
{
   typedef typename math_expr::igeneric_function<T>::generic_type
                                                  generic_type;

   typedef typename generic_type::scalar_view scalar_t;
   typedef typename generic_type::vector_view vector_t;
   typedef typename generic_type::string_view string_t;

   for (std::size_t i = 0; i < parameters.size(); ++i)
   {
      generic_type& gt = parameters[i];

      if (generic_type::store_type::e_scalar == gt.type)
      {
         scalar_t x(gt);
         ...
      }
      else if (generic_type::store_type::e_vector == gt.type)
      {
         vector_t vector(gt);
         ...
      }
      else if (generic_type::store_type::e_string == gt.type)
      {
         string_t string(gt);
         ...
      }
   }

   return T(0);
}
```

Most often than not a custom generic function will require a specific sequence of parameters, rather than some arbitrary sequence of types. In those situations, MathExpr can perform compile-time type checking to validate that function invocations are carried out using the correct sequence of parameters. Furthermore performing the checks at compile -time rather than at run-time (aka every time the function is invoked) will result in expression evaluation performance gains.

Compile-time type checking of input parameters can be requested by passing a string to the constructor of the igeneric_function that represents the required sequence of parameter types. When no parameter sequence is provided, it is implied the function can accept a variable number of parameters comprised of any of the fundamental types.

Each fundamental type has an associated character. The following is a listing of said characters and their meanings:

1. T - Scalar
1. V - Vector
1. S - String
1. Z - Zero or no parameters
1. ? - Any type (Scalar, Vector or String)
1. * - Wildcard operator
1. | - Parameter sequence delimiter

No other characters other than the seven denoted above may be included in the parameter sequence definition. If any such invalid characters do exist, registration of the associated generic function to a symbol table ('add_function' method) will fail. If the parameter sequence is modified resulting in it becoming invalid after having been added to the symbol table but before the compilation step, a compilation error will be incurred.

The following example demonstrates a simple generic function implementation with a user specified parameter sequence:

```cpp
template <typename T>
struct moo final : public math_expr::igeneric_function<T>
{
   typedef typename math_expr::igeneric_function<T>::parameter_list_t
                                                  parameter_list_t;

   moo()
   : math_expr::igeneric_function<T>("SVTT")
   {}

   inline T operator()(parameter_list_t parameters) override
   {
      ...
   }
};
```

In the example above the generic function 'moo' expects exactly four parameters in the following sequence:

1. `String`
1. `Vector`
1. `Scalar`
1. `Scalar`

**Note 16.** The 'Z' or no parameter option may not be used in conjunction with any other type option in a parameter sequence. When incorporated in the parameter sequence list, the 'No Parameter' option indicates that the function may be invoked without any parameters being passed. For more information refer to the section: 'Zero Parameter Functions'

## igeneric_function II

This interface is identical to the igeneric_function, in that in can consume an arbitrary number of parameters of varying type, but the difference being that the function returns a string and as such is treated as a string when invoked within expressions. As a result the function call can alias a string and interact with other strings in situations such as concatenation and equality operations.

String <-- function(i_0, i_1, i_2....., i_N)

The following example defines a generic function named 'toupper' with the string return type function operator being explicitly overridden:

```cpp
template <typename T>
struct toupper final : public math_expr::igeneric_function<T>
{
   typedef math_expr::igeneric_function<T> igenfunct_t;
   typedef typename igenfunct_t::generic_type generic_t;
   typedef typename igenfunct_t::parameter_list_t parameter_list_t;
   typedef typename generic_t::string_view string_t;

   toupper()
   : math_expr::igeneric_function<T>("S",igenfunct_t::return_type::e_rtrn_string)
   {}

   inline T operator()(std::string& result,
                       parameter_list_t parameters) override
   {
      result.clear();

      string_t string(parameters[0]);

      for (std::size_t i = 0; i < string.size(); ++i)
      {
         result += std::toupper(string[i]);
      }

      return T(0);
   }
};
```

In the example above the generic function 'toupper' expects only one input parameter of type string, as noted by the parameter sequence string passed during the constructor. Furthermore a second parameter is passed to the constructor indicating that it should be treated as a string returning function - by default it is assumed to be a scalar returning function.

When executed, the function will return as a result a copy of the input string converted to uppercase form. An example expression using the toupper function registered as the symbol 'toupper' is as follows:

"'ABCDEF' == toupper('aBc') + toupper('DeF')"

**Note 17.** When adding a string type returning generic function to a symbol table the 'add_function' is invoked. The example below demonstrates how this can be done:

```text
toupper<T> tu;

math_expr::symbol_table<T> symbol_table;

symbol_table.add_function("toupper",tu);
```

**Note 18.** Two further refinements to the type checking facility are the possibilities of a variable number of common types which can be accomplished by using a wildcard '*' and a special 'any type' which is done using the '?' character. It should be noted that the wildcard operator is associated with the previous type in the sequence and implies one or more of that type.

```cpp
template <typename T>
struct zoo final : public math_expr::igeneric_function<T>
{
   typedef typename math_expr::igeneric_function<T>::parameter_list_t
                                                  parameter_list_t;

   zoo()
   : math_expr::igeneric_function<T>("SVT*V?")
   {}

   inline T operator()(parameter_list_t parameters) override
   {
      ...
   }
};
```

In the example above the generic function 'zoo' expects at least five parameters in the following sequence:

1. `String`
1. `Vector`
1. One or more Scalars
1. `Vector`
1. Any type (one type of either a scalar, vector or string)

A final piece of type checking functionality is available for the scenarios where a single function name is intended to be used for multiple distinct parameter sequences, another name for this feature is function overloading. The parameter sequences are passed to the constructor as a single string delimited by the pipe '|' character. Two specific overrides of the function operator are provided one for standard generic functions and one for string returning functions. The overrides are as follows:

```cpp
// Scalar <-- function(psi,i_0,i_1,....,i_N)
inline T operator()(const std::size_t& ps_index,
                    parameter_list_t parameters)
{
   ...
}

// String <-- function(psi,i_0,i_1,....,i_N)
inline T operator()(const std::size_t& ps_index,
                    std::string& result,
                    parameter_list_t parameters)
{
   ...
}
```

When the function operator is invoked the 'ps_index' parameter will have as its value the index of the parameter sequence that matches the specific invocation. This way complex and time consuming type checking conditions need not be executed in the function itself but rather a simple and efficient dispatch to a specific implementation for that particular parameter sequence can be performed.

```cpp
template <typename T>
struct roo final : public math_expr::igeneric_function<T>
{
   typedef typename math_expr::igeneric_function<T>::parameter_list_t
                                                  parameter_list_t;

   roo()
   : math_expr::igeneric_function<T>("SVTT|SS|TTV|S?V*S")
   {}

   inline T operator()(const std::size_t& ps_index,
                       parameter_list_t parameters) override
   {
      ...
   }
};
```

In the example above there are four distinct parameter sequences that can be processed by the generic function 'roo'. Any other parameter sequences will cause a compilation error. The four valid sequences are as follows:

Sequence-0 Sequence-1 Sequence-2 Sequence-3 'SVTT' 'SS' 'TTV' 'S?V*S'

1. String (1) String (1) Scalar (1) String
1. Vector (2) String (2) Scalar (2) Any Type
1. Scalar (3) Vector (3) One or more Vectors
1. Scalar (4) String

## igeneric_function III

In this section we will discuss an extension of the igeneric_function interface that will allow for the overloading of a user defined custom function, where by it can return either a scalar or string value type depending on the input parameter sequence with which the function is invoked.

```cpp
template <typename T>
struct foo final : public math_expr::igeneric_function<T>
{
   typedef typename math_expr::igeneric_function<T>::parameter_list_t
                                                  parameter_list_t;

   foo()
   : math_expr::igeneric_function<T>
     (
       "T:T|S:TS",
       igfun_t::return_type::e_rtrn_overload
     )
   {}

   // Scalar value returning invocations
   inline T operator()(const std::size_t& ps_index,
                       parameter_list_t parameters) override
   {
      ...
   }

   // String value returning invocations
   inline T operator()(const std::size_t& ps_index,
                       std::string& result,
                       parameter_list_t& parameters) override
   {
      ...
   }
};
```

In the example above the custom user defined function "foo" can be invoked by using either one of two input parameter sequences, which are defined as follows:

Sequence-0 Sequence-1 'T' -> T 'TS' -> S

1. Scalar (1) Scalar
1. `String`

The parameter sequence definitions are identical to the previously defined igeneric_function, with the exception of the inclusion of the return type - which can only be either a scalar T or a string S.

## function_compositor

The function compositor is a factory that allows one to define and construct a function using MathExpr syntax. The functions are limited to returning a single scalar value and consuming up to six parameters as input.

All composited functions are registered with a symbol table, allowing them to call other functions and use variables that have been registered with the symbol table instance. Furthermore the functions can be recursive in nature due to the inherent function prototype forwarding that occurs during construction. The following example defines, by using two different methods, composited functions and implicitly registering the functions with the denoted symbol table.

```cpp
typedef math_expr::symbol_table<T>         symbol_table_t;
typedef math_expr::function_compositor<T>  compositor_t;
typedef typename compositor_t::function function_t;

T avogadro = T(6.022e23);

symbol_table_t symbol_table;

symbol_table.add_constant("avogadro", avogadro);

compositor_t compositor(symbol_table);

// Define function koo0(v1, v2) { ... }
compositor.add(
   function_t("koo0"),
   .vars("v1", "v2")
   .expression
   (
      " 1 + cos(v1 * v2) / avogadro; "
   ));

// Define function koo1(x, y, z) { ... }
compositor.add(
   function_t()
   .name("koo1")
   .var("x").var("y").var("z")
   .expression
   (
      "1 + koo0(x * y, 3) / z;"
   ));
```

A function compositor can also be instantiated without a symbol_table. When this is the case an internal symbol_table is used for holding the references to the composited functions.

```text
compositor_t compositor;

// Define function koo2(v1, v2) { ... }
compositor.add(
   function_t("koo2"),
   .vars("v1", "v2", "v3")
   .expression
```

## / v3; " ));

When wanting to reference functions from the compositor above in an expression, the compositor's symbol_table will need to be registered with the expression prior to compilation, as is demonstrated in the following code:

```text
expression_t expression;
.
.
expression.register_symbol_table(compositor.symbol_table());
```

In the situation where more than one symbol table's contents will be required by the functions being composited, then those symbol tables can be registered as auxiliary symbol tables with the compositor:

```text
symbol_table_t global_symbol_table;
symbol_table_t local_symbol_table;
.
.
.
compositor_t compositor;

compositor.add_auxiliary_symtab(global_symbol_table);
compositor.add_auxiliary_symtab(local_symbol_table );
```

**Note 19.** In the event, that two or more symbol tables contain similarly named variables, vectors, strings or functions, the order of registration with the compositor shall determine the symbol table from which the target symbol will be referenced.

## Using Functions In Expressions

For the above denoted custom and composited functions to be used in an expression, an instance of each function needs to be registered with a symbol_table that has been associated with the expression instance. The following demonstrates how all the pieces are put together:

```cpp
typedef math_expr::symbol_table<double>        symbol_table_t;
typedef math_expr::expression<double>          expression_t;
typedef math_expr::parser<double>              parser_t;
typedef math_expr::function_compositor<double> compositor_t;
typedef typename compositor_t::function     function_t;

foo<double> f;
boo<double> b;
too<double> t;
toupper<double> tu;

symbol_table_t symbol_table;
compositor_t   compositor(symbol_table);

symbol_table.add_function("foo",f);
symbol_table.add_function("boo",b);
symbol_table.add_function("too",t);

symbol_table
   .add_function("toupper", tu, symbol_table_t::e_ft_strfunc);

compositor.add(
   function_t("koo")
   .var("v1")
   .var("v2")
   .expression
   (
      "1 + cos(v1 * v2) / 3;"
   ));

expression_t expression;
expression.register_symbol_table(symbol_table);

const std::string expression_str =
   " if (foo(1,2,3) + boo(1) > boo(1/2, 2/3, 3/4, 4/5)) "
   "    koo(3,4);                                       "
   " else                                               "
   "    too(2 * v1 + v2 / 3, 'abcdef'[2:4], 3.3);       "
   "                                                    ";

parser_t parser;
parser.compile(expression_str,expression);

expression.value();
```

## Function Side-Effects

All function calls are assumed to have side-effects by default. This assumption implicitly disables constant folding optimisations when all parameters being passed to the function are deduced as being constants at compile time.

If it is certain that the function being registered does not have any side-effects and can be correctly constant folded where appropriate, then during the construction of the function the side-effect trait of the function can be disabled.

```cpp
template <typename T>
struct foo final : public math_expr::ifunction<T>
{
   foo() : math_expr::ifunction<T>(3)
   {
      math_expr::disable_has_side_effects(*this);
   }

   T operator()(const T& v1, const T& v2, const T& v3) override
   { ... }
};
```

## Zero Parameter Functions

When either an ifunction, ivararg_function or igeneric_function derived type is defined with zero number of parameters, there are two calling conventions within expressions that are allowed. For a function named 'foo' with zero input parameters the calling styles are as follows:

1. x + sin(foo()- 2) / y
1. x + sin(foo - 2) / y

By default the zero parameter trait is disabled. In order to enable it, a process similar to that of enabling of the side-effect trait is carried out:

```cpp
template <typename T>
struct foo final : public math_expr::ivararg_function<T>
{
   foo()
   {
      math_expr::enable_zero_parameters(*this);
   }

   inline T operator()(const std::vector<T>& arglist) override
   { ... }
};
```

**Note 20.** For the igeneric_function type, there also needs to be a 'Z' parameter sequence defined in order for the zero parameter trait to properly take effect otherwise a compilation error will occur.

## Free Functions

The MathExpr symbol table supports the registration of free functions and lambdas (anonymous functors) for use in expressions. The basic requirements are similar to those found in ifunction derived user defined functions. This includes support for free functions using anywhere from zero up to fifteen input parameters of scalar type, with a return type that is also scalar. Furthermore such functions will by default be assumed to have side-effects and hence will not participate in constant folding optimisations.

In the following example, a one input parameter free function named 'compute1', a two input parameter template free function named 'compute2' and a three input parameter lambda named 'compute3' will be registered with the given symbol_table instance:

double compute1(double v0)

```cpp
{
   return 2.0 * std::abs(v0);
}

template <typename T>
T compute2(T v0, T v1)
{
   return 2.0 * v0 + v1 / 3.0;
}
.
.
.

typedef math_expr::symbol_table<double> symbol_table_t;

symbol_table_t symbol_table;

symbol_table.add_function("compute1", compute1);
symbol_table.add_function("compute2", compute2<double>);

symbol_table.add_function(
   "compute3",
   [](double v0, double v1, double v2) -> double
   { return v0 / v1 + v2; });
```

**Note 21.** Similar to variables registered with symbol_table instances, for any of the following function providers:

1. ifunction
1. ivararg_function
1. igeneric_function
1. function_compositor
1. Free function
1. Lambda

Their instance lifetimes must exceed the symbol_tables and expressions they are registered with. In the event that is not the case, the expected result shall be undefined behaviour.

[Previous](14-vector-processing.md) | [Index](index.md) | [Next](16-expression-dependents.md)
