# Copyright 2026 Yuya Asano <my_favorite_theory@yahoo.co.jp>
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

find_package(Catch2 REQUIRED)

add_executable(math_expr_test math_expr_test.cpp)
target_link_libraries(math_expr_test MathExpr Catch2::Catch2WithMain)
add_test(NAME math_expr_test COMMAND math_expr_test)

add_test(
  NAME math_expr_configured_epsilon_subbuild
  COMMAND ${CMAKE_COMMAND}
    -DSOURCE_DIR=${PROJECT_SOURCE_DIR}
    -DVERIFY_PROJECT_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}/cmake/epsilon_configured_verify
    -DBINARY_DIR=${CMAKE_CURRENT_BINARY_DIR}/epsilon_configured_verify
    -DGENERATOR=${CMAKE_GENERATOR}
    -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/run_configured_epsilon_verification.cmake
)

add_test(
  NAME math_expr_default_epsilon_subbuild
  COMMAND ${CMAKE_COMMAND}
    -DSOURCE_DIR=${PROJECT_SOURCE_DIR}
    -DVERIFY_PROJECT_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}/cmake/epsilon_default_verify
    -DBINARY_DIR=${CMAKE_CURRENT_BINARY_DIR}/epsilon_default_verify
    -DGENERATOR=${CMAKE_GENERATOR}
    -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/run_default_epsilon_verification.cmake
)

function(math_expr_add_invalid_epsilon_configure_test test_name epsilon_value)
  add_test(
    NAME "${test_name}"
    COMMAND ${CMAKE_COMMAND}
      -DSOURCE_DIR=${PROJECT_SOURCE_DIR}
      -DBINARY_DIR=${CMAKE_CURRENT_BINARY_DIR}/${test_name}
      -DGENERATOR=${CMAKE_GENERATOR}
      -DEPSILON_VALUE=${epsilon_value}
      -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/run_invalid_epsilon_configure_test.cmake
  )
endfunction()

math_expr_add_invalid_epsilon_configure_test(math_expr_invalid_epsilon_non_numeric abc)
math_expr_add_invalid_epsilon_configure_test(math_expr_invalid_epsilon_zero 0)
math_expr_add_invalid_epsilon_configure_test(math_expr_invalid_epsilon_negative -1e-6)
math_expr_add_invalid_epsilon_configure_test(math_expr_invalid_epsilon_nan nan)
math_expr_add_invalid_epsilon_configure_test(math_expr_invalid_epsilon_inf inf)
