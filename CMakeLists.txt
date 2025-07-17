cmake_minimum_required(VERSION 3.16)
project(BYO_x VERSION 1.0.0 LANGUAGES CXX)

# cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ /path/to/your/source

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # works

# Compiler requirements check
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "11.0")
    message(FATAL_ERROR "GCC version must be at least 11.0")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "14.0")
    message(FATAL_ERROR "Clang version must be at least 14.0")
endif()

# Build options
option(LONG "Support graphs with more than 2^32 edges" OFF)
option(EDGELONG "Support graphs with more than 2^32 vertices" OFF)
option(USE_OPENMP "Use OpenMP for parallelism" OFF)
option(USE_CILK "Use OpenCilk for parallelism" OFF)
option(USE_SERIAL "Compile serially" OFF)
option(BUILD_BENCHMARKS "Build benchmark executables" ON)
option(BUILD_TESTS "Build unit tests" ON)

# Compiler flags
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")

# Add compile definitions based on options
if(LONG)
    add_compile_definitions(LONG)
endif()

if(EDGELONG)
    add_compile_definitions(EDGELONG)
endif()

# Parallelism configuration
if(USE_SERIAL)
    add_compile_definitions(PARLAY_SEQUENTIAL)
elseif(USE_OPENMP)
    find_package(OpenMP REQUIRED)
    add_compile_definitions(PARLAY_OPENMP)
elseif(USE_CILK)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopencilk")
    add_compile_definitions(PARLAY_OPENCILK)
else()
    # Default: Use homemade scheduler
    add_compile_definitions(PARLAY_HOMEMADE)
endif()

# Find required packages
find_package(Threads REQUIRED)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(external/parlaylib/include)
include_directories(external/PAM/include)
include_directories(external/CPAM/include)
include_directories(external/abseil-cpp)
include_directories(external/reducer)
include_directories(external/semisort)
include_directories(external/StructOfArrays/include)
include_directories(external/ParallelTools)
include_directories(external/SSTGraph/include)
include_directories(external/terrace/include)
include_directories(external/dhb/include)
include_directories(external/aspen/code)
include_directories(external/Packed-Memory-Array/include)
include_directories(external/pcsr_orig)

# External libraries setup
# Abseil
add_subdirectory(external/abseil-cpp EXCLUDE_FROM_ALL)

# GoogleTest (for unit tests)
if(BUILD_TESTS)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/release-1.11.0.tar.gz
        URL_HASH SHA256=b4870bf121ff7795ba20d20bcdd8627b8e088f2d1dab299a031c1034eddc93d5
    )
    FetchContent_MakeAvailable(googletest)
endif()

# pybind11 (for Python bindings)
include(FetchContent)
FetchContent_Declare(
    pybind11
    URL https://github.com/pybind/pybind11/archive/fe755dce12766820a99eefbde32d6ceb0a828ca8.tar.gz
    URL_HASH SHA256=5702350060e965043609a5115576c598ef3262a7367f063aebfeaa7961f2cbfd
)
FetchContent_MakeAvailable(pybind11)

# GBBS Core Libraries
# Bridge library (core parallel primitives)
add_library(gbbs_bridge INTERFACE)
target_include_directories(gbbs_bridge INTERFACE gbbs)
target_link_libraries(gbbs_bridge INTERFACE Threads::Threads)

# Flags library
add_library(gbbs_flags INTERFACE)
target_include_directories(gbbs_flags INTERFACE gbbs)

# Macros library
add_library(gbbs_macros INTERFACE)
target_include_directories(gbbs_macros INTERFACE gbbs)
target_link_libraries(gbbs_macros INTERFACE gbbs_bridge)

# Bit array library
add_library(gbbs_bit_array INTERFACE)
target_include_directories(gbbs_bit_array INTERFACE gbbs)
target_link_libraries(gbbs_bit_array INTERFACE gbbs_bridge)

# Vertex subset library
add_library(gbbs_vertex_subset INTERFACE)
target_include_directories(gbbs_vertex_subset INTERFACE gbbs)
target_link_libraries(gbbs_vertex_subset INTERFACE gbbs_bridge gbbs_flags gbbs_macros gbbs_bit_array)

# Intersect library
add_library(gbbs_intersect INTERFACE)
target_include_directories(gbbs_intersect INTERFACE gbbs)
target_link_libraries(gbbs_intersect INTERFACE gbbs_macros)

# Vertex library
add_library(gbbs_vertex INTERFACE)
target_include_directories(gbbs_vertex INTERFACE gbbs)
target_link_libraries(gbbs_vertex INTERFACE gbbs_bridge gbbs_intersect gbbs_macros)

# Edge map utils
add_library(gbbs_edge_map_utils INTERFACE)
target_include_directories(gbbs_edge_map_utils INTERFACE gbbs)
target_link_libraries(gbbs_edge_map_utils INTERFACE gbbs_macros)

# Edge map blocked
add_library(gbbs_edge_map_blocked INTERFACE)
target_include_directories(gbbs_edge_map_blocked INTERFACE gbbs)
target_link_libraries(gbbs_edge_map_blocked INTERFACE gbbs_bridge gbbs_edge_map_utils gbbs_flags gbbs_vertex_subset)

# Compressed vertex
add_library(gbbs_compressed_vertex INTERFACE)
target_include_directories(gbbs_compressed_vertex INTERFACE gbbs)
target_link_libraries(gbbs_compressed_vertex INTERFACE gbbs_bridge gbbs_macros)

# Edge array
add_library(gbbs_edge_array INTERFACE)
target_include_directories(gbbs_edge_array INTERFACE gbbs)
target_link_libraries(gbbs_edge_array INTERFACE gbbs_bridge gbbs_macros gbbs_vertex gbbs_vertex_subset)

# Edge map data
add_library(gbbs_edge_map_data INTERFACE)
target_include_directories(gbbs_edge_map_data INTERFACE gbbs)
target_link_libraries(gbbs_edge_map_data INTERFACE 
    gbbs_bridge gbbs_bit_array gbbs_compressed_vertex gbbs_edge_map_blocked 
    gbbs_edge_map_utils gbbs_flags gbbs_vertex gbbs_vertex_subset)

# Edge map reduce
add_library(gbbs_edge_map_reduce INTERFACE)
target_include_directories(gbbs_edge_map_reduce INTERFACE gbbs)
target_link_libraries(gbbs_edge_map_reduce INTERFACE gbbs_bridge gbbs_flags gbbs_vertex_subset)

# Graph mutation
add_library(gbbs_graph_mutation INTERFACE)
target_include_directories(gbbs_graph_mutation INTERFACE gbbs)
target_link_libraries(gbbs_graph_mutation INTERFACE 
    gbbs_bridge gbbs_compressed_vertex gbbs_edge_array gbbs_vertex gbbs_vertex_subset)

# Graph library
add_library(gbbs_graph INTERFACE)
target_include_directories(gbbs_graph INTERFACE gbbs)
target_link_libraries(gbbs_graph INTERFACE 
    gbbs_bridge gbbs_compressed_vertex gbbs_edge_array gbbs_edge_map_data 
    gbbs_edge_map_reduce gbbs_flags gbbs_graph_mutation gbbs_macros 
    gbbs_vertex gbbs_vertex_subset)

# Interface library
add_library(gbbs_interface INTERFACE)
target_include_directories(gbbs_interface INTERFACE gbbs)
target_link_libraries(gbbs_interface INTERFACE 
    gbbs_edge_array gbbs_edge_map_data gbbs_edge_map_reduce gbbs_flags 
    gbbs_graph gbbs_graph_mutation gbbs_macros gbbs_vertex_subset)

# Contract library
add_library(gbbs_contract INTERFACE)
target_include_directories(gbbs_contract INTERFACE gbbs)
target_link_libraries(gbbs_contract INTERFACE gbbs_bridge gbbs_graph gbbs_vertex)

# IO libraries
add_library(gbbs_io gbbs/io.cc)
target_include_directories(gbbs_io PUBLIC gbbs)
target_link_libraries(gbbs_io PUBLIC gbbs_macros)

add_library(gbbs_graph_io gbbs/graph_io.cc)
target_include_directories(gbbs_graph_io PUBLIC gbbs)
target_link_libraries(gbbs_graph_io PUBLIC gbbs_graph gbbs_io gbbs_macros gbbs_vertex)

# Benchmark library
add_library(gbbs_benchmark INTERFACE)
target_include_directories(gbbs_benchmark INTERFACE gbbs)
target_link_libraries(gbbs_benchmark INTERFACE gbbs_graph_io)

# Bucket library
add_library(gbbs_bucket INTERFACE)
target_include_directories(gbbs_bucket INTERFACE gbbs)
target_link_libraries(gbbs_bucket INTERFACE gbbs_bridge gbbs_vertex_subset)

# Helper libraries
add_subdirectory(gbbs/helpers)
add_subdirectory(gbbs/encodings)

# Main GBBS library
add_library(gbbs INTERFACE)
target_include_directories(gbbs INTERFACE gbbs)
target_link_libraries(gbbs INTERFACE 
    gbbs_benchmark gbbs_bridge gbbs_flags gbbs_graph gbbs_interface 
    gbbs_macros gbbs_vertex_subset gbbs_helpers_parse_command_line)

# Julienne library
add_library(julienne INTERFACE)
target_include_directories(julienne INTERFACE gbbs)
target_link_libraries(julienne INTERFACE gbbs_bucket gbbs gbbs_helpers_dyn_arr)

# Utils
add_subdirectory(utils)

# Benchmarks
if(BUILD_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()
