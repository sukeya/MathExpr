# Installation

[Back to index](index.md)

[Previous](04-downloads-and-updates.md) | [Index](index.md) | [Next](06-compilation.md)

MathExpr is a header-only C++ library distributed as a CMake package, and requires a C++20-capable compiler.
There are three ways to use it in a project.

## CMake FetchContent (Recommended)

Add MathExpr as a dependency fetched at configure time:

```cmake
include(FetchContent)
FetchContent_Declare(
    MathExpr
    GIT_REPOSITORY https://github.com/sukeya/MathExpr.git
    GIT_TAG        main
)
FetchContent_MakeAvailable(MathExpr)

target_link_libraries(my_target PRIVATE MathExpr)
```

## Direct header inclusion

Copy the `include/` directory into your project and add it to the include path. Then include the top-level header:

```cpp
#include "math_expr.hpp"
```

## System-wide install (not Recommended)

Configure and install MathExpr to the system prefix:

```bash
cmake -B build
cmake --install build
```

Then consume it from another CMake project with:

```cmake
find_package(MathExpr REQUIRED)
target_link_libraries(my_target PRIVATE MathExpr::MathExpr)
```

[Previous](04-downloads-and-updates.md) | [Index](index.md) | [Next](06-compilation.md)
