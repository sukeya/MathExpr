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

set(example_files 
    01_introduction.cpp
    02_capabilities.cpp
    03_example_expressions.cpp
    04_copyright_notice.cpp
    05_downloads_and_updates.cpp
    06_installation.cpp
    07_compilation.cpp
    08_compiler_compatibility.cpp
    09_built-in_operations_and_funcs.cpp
    10_fundamental_types.cpp
    11_components.cpp
    12_compilation_options.cpp
    13_expression_structures.cpp
    14_variable_vector_and_string_def.cpp
    15_vector_processing.cpp
    16_user_defined_functions.cpp
    17_expression_dependents.cpp
    18_hierarchies_of_symbol_tables.cpp
    19_unknown_unknowns.cpp
    20_enabling_and_disabling_features.cpp
    21_expression_return_values.cpp
    22_compilation_errors.cpp
    23_runtime_library_packages.cpp
    24_helpers_and_utils.cpp
)

foreach(example_file ${example_files})
    get_filename_component(example_filename ${example_file} NAME_WE)
    add_executable(${example_filename} ${example_file})
    target_link_libraries(${example_filename} MathExpr)
endforeach()