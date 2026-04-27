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

option(MATHEXPR_BUILD_TESTS "Build MathExpr tests" OFF)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_OPTIMIZE_DEPENDENCIES ON)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

add_library(MathExpr INTERFACE)

target_compile_features(MathExpr INTERFACE cxx_std_20)
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
