# Files

[Back to index](index.md)

[Previous](28-build-options.md) | [Index](index.md) | [Next](30-language-structure.md)

The source distribution of MathExpr is comprised of the following set of files and directories:

## Build system

1. `CMakeLists.txt`
1. `CMakePresets.json`
1. `config.cmake.in`

## License

1. `license.txt`

## Headers

1. `include/math_expr.hpp` — top-level include

The `include/math_expr/` directory contains the individual component headers:

1. `include/math_expr/assert_check.hpp`
1. `include/math_expr/collect_types.hpp`
1. `include/math_expr/compilation_check.hpp`
1. `include/math_expr/compilation_options.hpp`
1. `include/math_expr/expression.hpp`
1. `include/math_expr/expression_helper.hpp`
1. `include/math_expr/function_compositor.hpp`
1. `include/math_expr/function_traits.hpp`
1. `include/math_expr/fwd.hpp`
1. `include/math_expr/ifunction.hpp`
1. `include/math_expr/igeneric_function.hpp`
1. `include/math_expr/ivararg_function.hpp`
1. `include/math_expr/loop_runtime_check.hpp`
1. `include/math_expr/loop_types.hpp`
1. `include/math_expr/parser.hpp`
1. `include/math_expr/parser_dependent_entity_collector.hpp`
1. `include/math_expr/parser_error.hpp`
1. `include/math_expr/parser_settings.hpp`
1. `include/math_expr/parser_symbol_types.hpp`
1. `include/math_expr/parser_unknown_symbol_resolver.hpp`
1. `include/math_expr/polynomial.hpp`
1. `include/math_expr/results_context.hpp`
1. `include/math_expr/stringvar_base.hpp`
1. `include/math_expr/symbol_table.hpp`
1. `include/math_expr/timer.hpp`
1. `include/math_expr/type_defs.hpp`
1. `include/math_expr/type_store.hpp`
1. `include/math_expr/vector_access_runtime_check.hpp`
1. `include/math_expr/vector_view.hpp`

## Tests

1. `test/math_expr_test.cpp`

## Benchmarks

1. `benchmark/math_expr_bench.cpp`

## Examples

The `examples/` directory contains 24 example programs, one per documentation section:

1. `examples/01_introduction.cpp`
1. `examples/02_capabilities.cpp`
1. `examples/03_example_expressions.cpp`
1. `examples/04_copyright_notice.cpp`
1. `examples/05_downloads_and_updates.cpp`
1. `examples/06_installation.cpp`
1. `examples/07_compilation.cpp`
1. `examples/08_compiler_compatibility.cpp`
1. `examples/09_built-in_operations_and_funcs.cpp`
1. `examples/10_fundamental_types.cpp`
1. `examples/11_components.cpp`
1. `examples/12_compilation_options.cpp`
1. `examples/13_expression_structures.cpp`
1. `examples/14_variable_vector_and_string_def.cpp`
1. `examples/15_vector_processing.cpp`
1. `examples/16_user_defined_functions.cpp`
1. `examples/17_expression_dependents.cpp`
1. `examples/18_hierarchies_of_symbol_tables.cpp`
1. `examples/19_unknown_unknowns.cpp`
1. `examples/20_enabling_and_disabling_features.cpp`
1. `examples/21_expression_return_values.cpp`
1. `examples/22_compilation_errors.cpp`
1. `examples/23_runtime_library_packages.cpp`
1. `examples/24_helpers_and_utils.cpp`

[Previous](28-build-options.md) | [Index](index.md) | [Next](30-language-structure.md)
