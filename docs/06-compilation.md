# Compilation

[Back to index](index.md)

[Previous](05-installation.md) | [Index](index.md) | [Next](07-compiler-compatibility.md)

The MathExpr package contains the MathExpr headers, a set of examples, a benchmark suite, and a unit test suite. The build system is CMake (minimum version 3.24). CMake presets are provided for common configurations.

## Using presets

### Tests

```bash
cmake --preset test
cmake --build --preset test
ctest --preset test
```

### Benchmarks

```bash
cmake --preset benchmark
cmake --build --preset benchmark
```

### Examples

```bash
cmake --preset examples
cmake --build --preset examples
```

## Without presets

```bash
# Configure (enable desired component)
cmake -B build \
    -DMATH_EXPR_BUILD_TESTS=ON \
    -DMATH_EXPR_BUILD_BENCHMARK=OFF \
    -DMATH_EXPR_BUILD_EXAMPLES=OFF

# Build
cmake --build build

# Run tests
ctest --test-dir build
```

The available build options are:

| CMake option | Default | Description |
|---|---|---|
| `MATH_EXPR_BUILD_TESTS` | OFF | Build the unit test suite |
| `MATH_EXPR_BUILD_BENCHMARK` | OFF | Build the benchmark suite |
| `MATH_EXPR_BUILD_EXAMPLES` | OFF | Build the example programs |
| `MATH_EXPR_BUILD_HEADER_SELF_CHECKS` | OFF | Build header self-check targets |

[Previous](05-installation.md) | [Index](index.md) | [Next](07-compiler-compatibility.md)
