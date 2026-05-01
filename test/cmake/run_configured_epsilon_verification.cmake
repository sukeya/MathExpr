if(NOT DEFINED SOURCE_DIR)
  message(FATAL_ERROR "SOURCE_DIR must be provided.")
endif()

if(NOT DEFINED VERIFY_PROJECT_SOURCE_DIR)
  message(FATAL_ERROR "VERIFY_PROJECT_SOURCE_DIR must be provided.")
endif()

if(NOT DEFINED BINARY_DIR)
  message(FATAL_ERROR "BINARY_DIR must be provided.")
endif()

if(NOT DEFINED GENERATOR)
  message(FATAL_ERROR "GENERATOR must be provided.")
endif()

find_program(CTEST_COMMAND ctest REQUIRED)

file(REMOVE_RECURSE "${BINARY_DIR}")

execute_process(
  COMMAND "${CMAKE_COMMAND}"
    -S "${VERIFY_PROJECT_SOURCE_DIR}"
    -B "${BINARY_DIR}"
    -G "${GENERATOR}"
    "-DMATH_EXPR_SOURCE_DIR=${SOURCE_DIR}"
    -DMATH_EXPR_EPSILON=0.125
  RESULT_VARIABLE configure_result
  OUTPUT_VARIABLE configure_stdout
  ERROR_VARIABLE configure_stderr
)

if(NOT configure_result EQUAL 0)
  message(FATAL_ERROR
    "Failed to configure the configured-epsilon verification build.\n"
    "stdout:\n${configure_stdout}\n"
    "stderr:\n${configure_stderr}")
endif()

execute_process(
  COMMAND "${CMAKE_COMMAND}" --build "${BINARY_DIR}" --target math_expr_configured_epsilon_verify
  RESULT_VARIABLE build_result
  OUTPUT_VARIABLE build_stdout
  ERROR_VARIABLE build_stderr
)

if(NOT build_result EQUAL 0)
  message(FATAL_ERROR
    "Failed to build the configured-epsilon verification target.\n"
    "stdout:\n${build_stdout}\n"
    "stderr:\n${build_stderr}")
endif()

execute_process(
  COMMAND "${CTEST_COMMAND}" --test-dir "${BINARY_DIR}" --output-on-failure
  RESULT_VARIABLE test_result
  OUTPUT_VARIABLE test_stdout
  ERROR_VARIABLE test_stderr
)

if(NOT test_result EQUAL 0)
  message(FATAL_ERROR
    "The configured-epsilon verification test failed.\n"
    "stdout:\n${test_stdout}\n"
    "stderr:\n${test_stderr}")
endif()
