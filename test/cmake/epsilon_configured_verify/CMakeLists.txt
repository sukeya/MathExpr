cmake_minimum_required(VERSION 3.31.6)

project(MathExprConfiguredEpsilonVerify LANGUAGES CXX)

if(NOT DEFINED MATH_EXPR_SOURCE_DIR)
  message(FATAL_ERROR "MATH_EXPR_SOURCE_DIR must be provided.")
endif()

add_subdirectory("${MATH_EXPR_SOURCE_DIR}" math_expr)

add_executable(math_expr_configured_epsilon_verify main.cpp)
target_link_libraries(math_expr_configured_epsilon_verify PRIVATE MathExpr)
target_compile_features(math_expr_configured_epsilon_verify PRIVATE cxx_std_20)

enable_testing()
add_test(NAME math_expr_configured_epsilon_verify COMMAND math_expr_configured_epsilon_verify)
