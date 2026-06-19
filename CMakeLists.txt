cmake_minimum_required(VERSION 3.15)
project(FinalProjExamples)

# filesystem.h require C++17
set(CMAKE_CXX_STANDARD 17)

file(COPY sample_data/ DESTINATION sample_data/)

# Show compiler output and enable warnings
set(CMAKE_VERBOSE_MAKEFILE ON)
add_compile_options(-Wall -Wextra -pedantic)

add_executable(rapidJSONExample rapidJSONExample.cpp)
add_executable(supersearch main.cpp)
add_executable(AVL_Tree_Test AVL_Tree_Test.cpp)

# This makes sure we also link rapidjson
target_include_directories(rapidJSONExample PRIVATE rapidjson/)

set(CMAKE_VERBOSE_MAKEFILE OFF)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeModules)

if(CMAKE_COMPILER_IS_GNUCXX)
    # Add your code here for GNUCXX compiler
    include(CodeCoverage.cmake)
    if(LCOV_PATH)
        append_coverage_compiler_flags_to_target(AVL_Tree_Test)
        setup_target_for_coverage_lcov(
            NAME coverage
            EXECUTABLE AVL_Tree_Test
        )
    endif()
    endif()