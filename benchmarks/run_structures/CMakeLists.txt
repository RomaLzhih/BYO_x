# CMakeLists.txt for benchmarks/run_structures
# This directory contains various graph container benchmarks

if(NOT BUILD_BENCHMARKS)
    return()
endif()

# Define common dependencies and include directories
set(COMMON_DEPS run_unweighted gbbs)

# Set base compile definitions for all targets
set(BASE_COMPILE_OPTIONS "")

# Helper function to create benchmark executables
function(add_benchmark_executable target_name source_file)
    add_executable(${target_name} ${source_file})
    target_link_libraries(${target_name} PRIVATE ${COMMON_DEPS} ${ARGN})
    target_include_directories(${target_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    set_target_properties(${target_name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/benchmarks/run_structures
    )
endfunction()

# Helper function to add benchmark with compile definitions
function(add_benchmark_with_defines target_name source_file defines)
    add_benchmark_executable(${target_name} ${source_file})
    if(defines)
        target_compile_definitions(${target_name} PRIVATE ${defines})
    endif()
endfunction()

# =============================================================================
# Core Graph Structures
# =============================================================================

# CSR (Compressed Sparse Row) benchmarks
add_benchmark_executable(run_csr run_csr.cc)

add_benchmark_executable(run_csr_shuffled run_csr_shuffled.cc)
target_compile_definitions(run_csr_shuffled PRIVATE CSR_SHUFFLE)

# GBBS benchmark
add_benchmark_executable(run_gbbs run_gbbs.cc gbbs_bridge)

# =============================================================================
# Standard Library Containers
# =============================================================================

# std::set benchmarks
add_benchmark_executable(run_std_set run_std_set.cc)

add_benchmark_executable(run_std_set_inplace run_std_set.cc)
target_compile_definitions(run_std_set_inplace PRIVATE USE_INPLACE)

# std::unordered_set benchmarks
add_benchmark_executable(run_std_unordered_set run_std_unordered_set.cc)

add_benchmark_executable(run_std_unordered_set_inplace run_std_unordered_set.cc)
target_compile_definitions(run_std_unordered_set_inplace PRIVATE USE_INPLACE)

# =============================================================================
# Abseil Container Benchmarks
# =============================================================================

# Abseil btree_set benchmarks
add_benchmark_executable(run_absl_btree_set run_absl_btree_set.cc absl::btree)

add_benchmark_executable(run_absl_btree_set_inplace run_absl_btree_set.cc absl::btree)
target_compile_definitions(run_absl_btree_set_inplace PRIVATE USE_INPLACE)

# Abseil flat_hash_set benchmarks
add_benchmark_executable(run_absl_flat_hash_set run_absl_flat_hash_set.cc absl::flat_hash_set)

add_benchmark_executable(run_absl_flat_hash_set_inplace run_absl_flat_hash_set.cc absl::flat_hash_set)
target_compile_definitions(run_absl_flat_hash_set_inplace PRIVATE USE_INPLACE)

# =============================================================================
# Vector-based Structures
# =============================================================================

# Vector of vectors
add_benchmark_executable(run_vector_vector run_vector_vector.cc gbbs_bridge)

# =============================================================================
# Advanced Graph Structures
# =============================================================================

# SSTGraph benchmarks
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../../external/SSTGraph)
    add_benchmark_executable(run_sstgraph run_sstgraph.cc gbbs_bridge)
    target_include_directories(run_sstgraph PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/../../external/SSTGraph/include)
    
    add_benchmark_executable(run_vector_tinyset run_vector_tinyset.cc gbbs_bridge)
    target_include_directories(run_vector_tinyset PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/../../external/SSTGraph/include)
endif()

# PCSR (Packed Compressed Sparse Row) benchmarks
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../../external/Packed-Memory-Array)
    add_benchmark_executable(run_pcsr run_pcsr.cc gbbs_bridge)
    target_include_directories(run_pcsr PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/../../external/Packed-Memory-Array/include)
endif()

# Original PCSR
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../../external/pcsr_orig)
    add_benchmark_executable(run_pcsr_orig run_pcsr_orig.cc gbbs_bridge)
    target_include_directories(run_pcsr_orig PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/../../external/pcsr_orig)
endif()

# DHB (Dynamic Hashed Blocks)
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../../external/dhb)
    add_benchmark_executable(run_dhb run_dhb.cc gbbs_bridge)
    target_include_directories(run_dhb PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/../../external/dhb/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../external/semisort)
endif()

# Terrace benchmarks
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../../external/terrace)
    add_benchmark_executable(run_terrace run_terrace.cc gbbs_bridge)
    target_include_directories(run_terrace PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/../../external/terrace/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../external/ParallelTools)
    
    add_benchmark_executable(run_ppcsr run_ppcsr.cc gbbs_bridge)
    target_include_directories(run_ppcsr PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/../../external/terrace/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../external/ParallelTools)
endif()

# =============================================================================
# PMA (Packed Memory Array) Variants
# =============================================================================

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../../external/Packed-Memory-Array)
    # Vector PMA variants
    add_benchmark_executable(run_vector_pma run_vector_pma.cc)
    target_compile_definitions(run_vector_pma PRIVATE PMA_SETTINGS=pma_settings)
    target_include_directories(run_vector_pma PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/../../external/Packed-Memory-Array/include)

    add_benchmark_executable(run_vector_spma run_vector_pma.cc)
    target_compile_definitions(run_vector_spma PRIVATE PMA_SETTINGS=spmae_settings)
    target_include_directories(run_vector_spma PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/../../external/Packed-Memory-Array/include)

    add_benchmark_executable(run_vector_cpma run_vector_pma.cc)
    target_compile_definitions(run_vector_cpma PRIVATE PMA_SETTINGS=cpma_settings)
    target_include_directories(run_vector_cpma PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/../../external/Packed-Memory-Array/include)

    add_benchmark_executable(run_vector_scpma run_vector_pma.cc)
    target_compile_definitions(run_vector_scpma PRIVATE PMA_SETTINGS=scpmae_settings)
    target_include_directories(run_vector_scpma PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/../../external/Packed-Memory-Array/include)

    # Single PMA variants (F-Graph implementations)
    add_benchmark_executable(run_single_pma run_single_pma.cc gbbs_bridge)
    target_compile_definitions(run_single_pma PRIVATE PMA_SETTINGS=pma_settings)
    target_include_directories(run_single_pma PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/../../external/Packed-Memory-Array/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../external/reducer)

    add_benchmark_executable(run_single_spma run_single_pma.cc gbbs_bridge)
    target_compile_definitions(run_single_spma PRIVATE PMA_SETTINGS=spmae_settings)
    target_include_directories(run_single_spma PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/../../external/Packed-Memory-Array/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../external/reducer)

    add_benchmark_executable(run_single_cpma run_single_pma.cc gbbs_bridge)
    target_compile_definitions(run_single_cpma PRIVATE PMA_SETTINGS=cpma_settings)
    target_include_directories(run_single_cpma PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/../../external/Packed-Memory-Array/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../external/reducer)

    add_benchmark_executable(run_single_scpma run_single_pma.cc gbbs_bridge)
    target_compile_definitions(run_single_scpma PRIVATE PMA_SETTINGS=scpmae_settings)
    target_include_directories(run_single_scpma PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/../../external/Packed-Memory-Array/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../external/reducer)
endif()

# =============================================================================
# CPAM (Compressed PAM) Variants
# =============================================================================

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../../external/CPAM)
    # Standard CPAM
    add_benchmark_executable(run_vector_cpam run_vector_cpam.cc gbbs_bridge)
    target_include_directories(run_vector_cpam PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/../../external/CPAM/include)

    # Compressed CPAM
    add_benchmark_executable(run_vector_compressed_cpam run_vector_cpam.cc gbbs_bridge)
    target_compile_definitions(run_vector_compressed_cpam PRIVATE CPAM_COMPRESSED)
    target_include_directories(run_vector_compressed_cpam PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/../../external/CPAM/include)

    # Inplace variants
    add_benchmark_executable(run_vector_cpam_inplace run_vector_cpam.cc gbbs_bridge)
    target_compile_definitions(run_vector_cpam_inplace PRIVATE USE_INPLACE)
    target_include_directories(run_vector_cpam_inplace PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/../../external/CPAM/include)

    add_benchmark_executable(run_vector_compressed_cpam_inplace run_vector_cpam.cc gbbs_bridge)
    target_compile_definitions(run_vector_compressed_cpam_inplace PRIVATE CPAM_COMPRESSED USE_INPLACE)
    target_include_directories(run_vector_compressed_cpam_inplace PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/../../external/CPAM/include)
endif()

# =============================================================================
# Aspen Tree Variants
# =============================================================================

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../../external/aspen)
    # Standard Aspen
    add_benchmark_executable(run_vector_aspen run_vector_aspen.cc gbbs_macros)
    target_include_directories(run_vector_aspen PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/../../external/aspen/code)

    # Compressed Aspen
    add_benchmark_executable(run_vector_compressed_aspen run_vector_aspen.cc gbbs_macros)
    target_compile_definitions(run_vector_compressed_aspen PRIVATE ASPEN_USE_COMPRESSION)
    target_include_directories(run_vector_compressed_aspen PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/../../external/aspen/code)

    # Inplace variants
    add_benchmark_executable(run_vector_aspen_inplace run_vector_aspen.cc gbbs_macros)
    target_compile_definitions(run_vector_aspen_inplace PRIVATE USE_INPLACE)
    target_include_directories(run_vector_aspen_inplace PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/../../external/aspen/code)

    add_benchmark_executable(run_vector_compressed_aspen_inplace run_vector_aspen.cc gbbs_macros)
    target_compile_definitions(run_vector_compressed_aspen_inplace PRIVATE ASPEN_USE_COMPRESSION USE_INPLACE)
    target_include_directories(run_vector_compressed_aspen_inplace PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/../../external/aspen/code)
endif()

# =============================================================================
# Create target groups for convenience
# =============================================================================

# Group all standard library benchmarks
add_custom_target(std_benchmarks DEPENDS 
    run_std_set run_std_set_inplace 
    run_std_unordered_set run_std_unordered_set_inplace)

# Group all Abseil benchmarks
add_custom_target(absl_benchmarks DEPENDS 
    run_absl_btree_set run_absl_btree_set_inplace
    run_absl_flat_hash_set run_absl_flat_hash_set_inplace)

# Group all CSR benchmarks
add_custom_target(csr_benchmarks DEPENDS 
    run_csr run_csr_shuffled run_gbbs)

# Group all vector-based benchmarks
set(VECTOR_TARGETS run_vector_vector)
if(TARGET run_vector_tinyset)
    list(APPEND VECTOR_TARGETS run_vector_tinyset)
endif()
if(TARGET run_vector_pma)
    list(APPEND VECTOR_TARGETS run_vector_pma run_vector_spma run_vector_cpma run_vector_scpma)
endif()
if(TARGET run_vector_cpam)
    list(APPEND VECTOR_TARGETS run_vector_cpam run_vector_compressed_cpam 
         run_vector_cpam_inplace run_vector_compressed_cpam_inplace)
endif()
if(TARGET run_vector_aspen)
    list(APPEND VECTOR_TARGETS run_vector_aspen run_vector_compressed_aspen 
         run_vector_aspen_inplace run_vector_compressed_aspen_inplace)
endif()

add_custom_target(vector_benchmarks DEPENDS ${VECTOR_TARGETS})

# Group all advanced graph structure benchmarks
set(ADVANCED_TARGETS)
if(TARGET run_sstgraph)
    list(APPEND ADVANCED_TARGETS run_sstgraph)
endif()
if(TARGET run_pcsr)
    list(APPEND ADVANCED_TARGETS run_pcsr)
endif()
if(TARGET run_pcsr_orig)
    list(APPEND ADVANCED_TARGETS run_pcsr_orig)
endif()
if(TARGET run_dhb)
    list(APPEND ADVANCED_TARGETS run_dhb)
endif()
if(TARGET run_terrace)
    list(APPEND ADVANCED_TARGETS run_terrace run_ppcsr)
endif()
if(TARGET run_single_pma)
    list(APPEND ADVANCED_TARGETS run_single_pma run_single_spma run_single_cpma run_single_scpma)
endif()

if(ADVANCED_TARGETS)
    add_custom_target(advanced_benchmarks DEPENDS ${ADVANCED_TARGETS})
endif()

# Main target to build all run_structures benchmarks
get_property(ALL_TARGETS DIRECTORY PROPERTY BUILDSYSTEM_TARGETS)
list(FILTER ALL_TARGETS INCLUDE REGEX "^run_")
add_custom_target(all_run_structures DEPENDS ${ALL_TARGETS})

# Print available benchmark targets
message(STATUS "BYO_x run_structures benchmarks configured:")
message(STATUS "  Standard Library: std_benchmarks")
message(STATUS "  Abseil Containers: absl_benchmarks") 
message(STATUS "  CSR Variants: csr_benchmarks")
message(STATUS "  Vector-based: vector_benchmarks")
if(TARGET advanced_benchmarks)
    message(STATUS "  Advanced Structures: advanced_benchmarks")
endif()
message(STATUS "  All structures: all_run_structures")