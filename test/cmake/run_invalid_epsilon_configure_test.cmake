if(NOT DEFINED SOURCE_DIR)
  message(FATAL_ERROR "SOURCE_DIR must be provided.")
endif()

if(NOT DEFINED BINARY_DIR)
  message(FATAL_ERROR "BINARY_DIR must be provided.")
endif()

if(NOT DEFINED GENERATOR)
  message(FATAL_ERROR "GENERATOR must be provided.")
endif()

if(NOT DEFINED EPSILON_VALUE)
  message(FATAL_ERROR "EPSILON_VALUE must be provided.")
endif()

file(REMOVE_RECURSE "${BINARY_DIR}")

execute_process(
  COMMAND "${CMAKE_COMMAND}"
    -S "${SOURCE_DIR}"
    -B "${BINARY_DIR}"
    -G "${GENERATOR}"
    "-DMATH_EXPR_EPSILON=${EPSILON_VALUE}"
  RESULT_VARIABLE configure_result
  OUTPUT_VARIABLE configure_stdout
  ERROR_VARIABLE configure_stderr
)

set(configure_output "${configure_stdout}\n${configure_stderr}")

if(configure_result EQUAL 0)
  message(FATAL_ERROR
    "Expected configure to fail for MATH_EXPR_EPSILON='${EPSILON_VALUE}', but it succeeded.")
endif()

if(NOT configure_output MATCHES "MATH_EXPR_EPSILON")
  message(FATAL_ERROR
    "Configure failed for MATH_EXPR_EPSILON='${EPSILON_VALUE}', but the error output did not "
    "mention MATH_EXPR_EPSILON.\nOutput:\n${configure_output}")
endif()
