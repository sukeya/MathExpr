# Copyright 2013 Google Inc. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# Copyright 2024-2026 Yuya Asano <my_favorite_theory@yahoo.co.jp>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

cmake_minimum_required(VERSION 4.3.1)

project(MathExpr VERSION 0.1.0 LANGUAGES CXX)

option(MATHEXPR_BUILD_HEADER_SELF_CHECKS "Build MathExpr header self-check targets" OFF)
option(MATHEXPR_BUILD_TESTS "Build MathExpr tests" OFF)
option(MATH_EXPR_ENABLE_DEBUGGING "Enable MathExpr debug logging" OFF)
option(MATH_EXPR_DISABLE_CARDINAL_POW_OPTIMISATION
  "Disable MathExpr cardinal pow optimisation" OFF)
option(MATH_EXPR_DISABLE_COMMENTS "Disable MathExpr comments" OFF)
option(MATH_EXPR_DISABLE_BREAK_CONTINUE "Disable MathExpr break/continue support" OFF)
option(MATH_EXPR_DISABLE_SC_ANDOR "Disable MathExpr short-circuit and/or support" OFF)
option(MATH_EXPR_DISABLE_RETURN_STATEMENT "Disable MathExpr return statements" OFF)
option(MATH_EXPR_DISABLE_ENHANCED_FEATURES "Disable MathExpr enhanced features" OFF)
option(MATH_EXPR_DISABLE_STRING_CAPABILITIES "Disable MathExpr string capabilities" OFF)
option(MATH_EXPR_DISABLE_SUPERSCALAR_UNROLL
  "Disable MathExpr superscalar loop unrolling" OFF)
option(MATH_EXPR_DISABLE_RTL_IO "Disable MathExpr RTL IO package" OFF)
option(MATH_EXPR_DISABLE_RTL_IO_FILE "Disable MathExpr RTL file IO package" OFF)
option(MATH_EXPR_DISABLE_RTL_VECOPS "Disable MathExpr RTL vector operations package" OFF)
option(MATH_EXPR_DISABLE_CASEINSENSITIVITY
  "Disable MathExpr case-insensitive symbol matching" OFF)
option(MATH_EXPR_ENABLE_RANGE_RUNTIME_CHECKS
  "Enable MathExpr range runtime checks" OFF)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_OPTIMIZE_DEPENDENCIES ON)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

add_library(MathExpr INTERFACE)

target_compile_features(MathExpr INTERFACE cxx_std_20)
target_compile_definitions(MathExpr INTERFACE
  $<$<BOOL:${MATH_EXPR_ENABLE_DEBUGGING}>:MATH_EXPR_ENABLE_DEBUGGING>
  $<$<BOOL:${MATH_EXPR_DISABLE_CARDINAL_POW_OPTIMISATION}>:
    MATH_EXPR_DISABLE_CARDINAL_POW_OPTIMISATION>
  $<$<BOOL:${MATH_EXPR_DISABLE_COMMENTS}>:MATH_EXPR_DISABLE_COMMENTS>
  $<$<BOOL:${MATH_EXPR_DISABLE_BREAK_CONTINUE}>:MATH_EXPR_DISABLE_BREAK_CONTINUE>
  $<$<BOOL:${MATH_EXPR_DISABLE_SC_ANDOR}>:MATH_EXPR_DISABLE_SC_ANDOR>
  $<$<BOOL:${MATH_EXPR_DISABLE_RETURN_STATEMENT}>:MATH_EXPR_DISABLE_RETURN_STATEMENT>
  $<$<BOOL:${MATH_EXPR_DISABLE_ENHANCED_FEATURES}>:MATH_EXPR_DISABLE_ENHANCED_FEATURES>
  $<$<BOOL:${MATH_EXPR_DISABLE_STRING_CAPABILITIES}>:MATH_EXPR_DISABLE_STRING_CAPABILITIES>
  $<$<BOOL:${MATH_EXPR_DISABLE_SUPERSCALAR_UNROLL}>:MATH_EXPR_DISABLE_SUPERSCALAR_UNROLL>
  $<$<BOOL:${MATH_EXPR_DISABLE_RTL_IO}>:MATH_EXPR_DISABLE_RTL_IO>
  $<$<BOOL:${MATH_EXPR_DISABLE_RTL_IO_FILE}>:MATH_EXPR_DISABLE_RTL_IO_FILE>
  $<$<BOOL:${MATH_EXPR_DISABLE_RTL_VECOPS}>:MATH_EXPR_DISABLE_RTL_VECOPS>
  $<$<BOOL:${MATH_EXPR_DISABLE_CASEINSENSITIVITY}>:MATH_EXPR_DISABLE_CASEINSENSITIVITY>
  $<$<BOOL:${MATH_EXPR_ENABLE_RANGE_RUNTIME_CHECKS}>:MATH_EXPR_ENABLE_RANGE_RUNTIME_CHECKS>
)
target_include_directories(MathExpr INTERFACE
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/MathExpr>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

install(TARGETS MathExpr
  EXPORT MathExprTargets
  DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(
  DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/include/
  DESTINATION
    ${CMAKE_INSTALL_INCLUDEDIR}/MathExpr
)

install(EXPORT MathExprTargets
  FILE MathExprTargets.cmake
  NAMESPACE MathExpr::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/MathExpr
)

configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/MathExprConfig.cmake"
  INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/MathExpr"
  NO_SET_AND_CHECK_MACRO
  NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/MathExprConfigVersion.cmake"
  VERSION "${PROJECT_VERSION}"
  COMPATIBILITY AnyNewerVersion
)

install(
  FILES
    ${CMAKE_CURRENT_BINARY_DIR}/MathExprConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/MathExprConfigVersion.cmake
  DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/cmake/MathExpr
)

export(EXPORT MathExprTargets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/MathExprTargets.cmake"
  NAMESPACE MathExpr::
)

if(MATHEXPR_BUILD_HEADER_SELF_CHECKS)
  file(GLOB_RECURSE math_expr_installed_headers
    RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
  )

  set(math_expr_header_self_check_dir "${CMAKE_CURRENT_BINARY_DIR}/header_self_checks")
  file(MAKE_DIRECTORY "${math_expr_header_self_check_dir}")

  add_custom_target(math_expr_header_self_checks)

  foreach(header_rel_path IN LISTS math_expr_installed_headers)
    string(MAKE_C_IDENTIFIER "${header_rel_path}" header_target_suffix)
    set(header_self_check_target "math_expr_header_self_check_${header_target_suffix}")
    set(header_self_check_source "${math_expr_header_self_check_dir}/${header_target_suffix}.cpp")

    file(WRITE "${header_self_check_source}"
      "#include \"${header_rel_path}\"\n"
      "int main() { return 0; }\n"
    )

    add_executable("${header_self_check_target}" "${header_self_check_source}")
    target_link_libraries("${header_self_check_target}" PRIVATE MathExpr)
    add_dependencies(math_expr_header_self_checks "${header_self_check_target}")
  endforeach()
endif()

if(MATHEXPR_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

if(MATHEXPR_BUILD_TESTS)
  enable_testing()
  include(FetchContent)
  FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        b670de4fe12ac7c5e858b7de3a14fb4bd18c760e # v3.14.0
    OVERRIDE_FIND_PACKAGE
  )

  add_subdirectory(test)
endif()

if(MATHEXPR_BUILD_BENCHMARK)
  include(FetchContent)
  set(BENCHMARK_ENABLE_TESTING OFF)
  FetchContent_Declare(
    benchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG        192ef10025eb2c4cdd392bc502f0c852196baa48 # v1.9.5
    OVERRIDE_FIND_PACKAGE
  )
  FetchContent_MakeAvailable(benchmark)

  add_subdirectory(benchmark)
endif()
