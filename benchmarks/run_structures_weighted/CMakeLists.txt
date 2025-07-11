# CMakeLists.txt for benchmarks/run_structures_weighted
# This directory contains various weighted graph container benchmarks

if(NOT BUILD_BENCHMARKS)
    return()
endif()

# Define common dependencies and include directories
set(COMMON_DEPS run_weighted gbbs)

# Helper function to create weighted benchmark executables
function(add_weighted_benchmark_executable target_name source_file)
    add_executable(${target_name} ${source_file})
    target_link_libraries(${target_name} PRIVATE ${COMMON_DEPS} ${ARGN})
    target_include_directories(${target_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    set_target_properties(${target_name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/benchmarks/run_structures_weighted
    )
endfunction()

# Helper function to add weighted benchmark with compile definitions
function(add_weighted_benchmark_with_defines target_name source_file defines)
    add_weighted_benchmark_executable(${target_name} ${source_file})
    if(defines)
        target_compile_definitions(${target_name} PRIVATE ${defines})
    endif()
endfunction()

# =============================================================================
# Core Weighted Graph Structures
# =============================================================================

# CSR (Compressed Sparse Row) for weighted graphs
add_weighted_benchmark_executable(run_csr_weighted run_csr.cc)

# =============================================================================
# Standard Library Map Containers
# =============================================================================

# std::map benchmark
add_weighted_benchmark_with_defines(run_std_map_weighted run_simple_map.cc "STD_MAP")

# std::unordered_map benchmark
add_weighted_benchmark_with_defines(run_std_unordered_map_weighted run_simple_map.cc "STD_UNORDERED_MAP")

# =============================================================================
# Abseil Map Container Benchmarks
# =============================================================================

# Abseil btree_map benchmark
add_weighted_benchmark_with_defines(run_absl_btree_map_weighted run_simple_map.cc "ABSL_BTREE")
target_link_libraries(run_absl_btree_map_weighted PRIVATE absl::btree)

# Abseil flat_hash_map benchmark
add_weighted_benchmark_with_defines(run_absl_flat_map_weighted run_simple_map.cc "ABSL_FLAT")
target_link_libraries(run_absl_flat_map_weighted PRIVATE absl::flat_hash_map)

# =============================================================================
# Create target groups for convenience
# =============================================================================

# Group all standard library map benchmarks
add_custom_target(std_map_benchmarks DEPENDS 
    run_std_map_weighted run_std_unordered_map_weighted)

# Group all Abseil map benchmarks
add_custom_target(absl_map_benchmarks DEPENDS 
    run_absl_btree_map_weighted run_absl_flat_map_weighted)

# Group CSR weighted benchmarks
add_custom_target(weighted_csr_benchmarks DEPENDS 
    run_csr_weighted)

# Main target to build all run_structures_weighted benchmarks
get_property(ALL_WEIGHTED_TARGETS DIRECTORY PROPERTY BUILDSYSTEM_TARGETS)
list(FILTER ALL_WEIGHTED_TARGETS INCLUDE REGEX "^run_")
add_custom_target(all_run_structures_weighted DEPENDS ${ALL_WEIGHTED_TARGETS})

# Print available weighted benchmark targets
message(STATUS "BYO_x run_structures_weighted benchmarks configured:")
message(STATUS "  Standard Library Maps: std_map_benchmarks")
message(STATUS "  Abseil Map Containers: absl_map_benchmarks") 
message(STATUS "  Weighted CSR: weighted_csr_benchmarks")
message(STATUS "  All weighted structures: all_run_structures_weighted")
