# Runtime Library Packages

[Back to index](index.md)

[Previous](21-compilation-errors.md) | [Index](index.md) | [Next](23-helpers-and-utils.md)

MathExpr includes a range of extensions, that provide functionalities beyond simple numerical calculations. Currently the available packages are:

```text
  +---+--------------------+-----------------------------------+
  | # |    Package Name    |          Namespace/Type           |
  +---+--------------------+-----------------------------------+
  | 1 | Basic I/O          | math_expr::rtl::io::package<T>       |
  | 2 | File I/O           | math_expr::rtl::io::file::package<T> |
  | 3 | Vector Operations  | math_expr::rtl::vecops::package<T>   |
  +---+--------------------+-----------------------------------+
```

In order to make the features of a specific package available within an expression, an instance of the package must be added to the expression's associated symbol table. In the following example, the file I/O package is made available for the given expression:

```cpp
typedef math_expr::symbol_table<T> symbol_table_t;
typedef math_expr::expression<T>   expression_t;
typedef math_expr::parser<T>       parser_t;

math_expr::rtl::io::file::package<T> fileio_package;

const std::string expression_string =
   " var file_name := 'file.txt';       "
   " var stream    := null;             "
   "                                    "
   " stream := open(file_name,'w');     "
   "                                    "
   " write(stream,'Hello world....\n'); "
   "                                    "
   " close(stream);                     "
   "                                    ";

symbol_table_t symbol_table;
symbol_table.add_package(fileio_package);

expression_t expression;
expression.register_symbol_table(symbol_table);

parser_t parser;
parser.compile(expression_string,expression);

expression.value();
```

1. Basic I/O functions:

1. `print`
1. `println`

1. File I/O functions:

1. open (b) close
1. write (d) read
1. getline (f) eof

1. Vector Operations functions:

1. all_true (01) all_false
1. any_true (03) any_false
1. assign (05) count
1. copy (07) reverse
1. rotate-left (09) rotate-right
1. shift-left (11) shift-right
1. sort (13) nth_element
1. iota (15) sumk
1. axpy (17) axpby
1. axpyz (19) axpbyz
1. axpbsy (21) axpbsyz
1. axpbz (23) dot
1. dotk (25) diff
1. threshold_above (27) threshold_below
1. select (29) min/max_elementwise

[Previous](21-compilation-errors.md) | [Index](index.md) | [Next](23-helpers-and-utils.md)
