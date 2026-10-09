cmake_minimum_required(VERSION 3.15)
project(FinalProjExamples)

# filesystem.h require C++17
set(CMAKE_CXX_STANDARD 17)

file(COPY build/sample_data/ DESTINATION sample_data/)

# Show compiler output and enable warnings
set(CMAKE_VERBOSE_MAKEFILE ON)
add_compile_options(-Wall -Wextra -pedantic)

add_executable(rapidJSONExample docs/rapidJSONExample.cpp)
add_executable(supersearch src/main.cpp src/SearchEngine.cpp src/DocumentParser.cpp)
add_executable(AVL_Tree_Test tests/AVL_Tree_Test.cpp)
add_executable(Parser_Test tests/Parser_Test.cpp src/SearchEngine.cpp src/DocumentParser.cpp)
# This makes sure we also link rapidjson
target_include_directories(supersearch PRIVATE include/ include/rapidjson/)
target_include_directories(rapidJSONExample PRIVATE include/rapidjson/)
target_include_directories(AVL_Tree_Test PRIVATE include/)
target_include_directories(Parser_Test PRIVATE include/ include/rapidjson/)
# For CURL and libzip 
find_package(CURL REQUIRED)
find_package(libzip REQUIRED)
target_link_libraries(supersearch PRIVATE CURL::libcurl libzip::zip)
target_link_libraries(Parser_Test PRIVATE CURL::libcurl libzip::zip)

# For FTXUI Installation
include(FetchContent)
 
FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI
  GIT_TAG v7.0.3 
)
 
FetchContent_MakeAvailable(ftxui)
 
add_executable(main main.cpp)
target_link_libraries(main
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component
)

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