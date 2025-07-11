# Benchmarks
if(BUILD_BENCHMARKS)
    # Add benchmark executables
    add_subdirectory(run_structures)
    add_subdirectory(run_structures_weighted)
    
    # Core benchmark runner
    add_library(run_unweighted run_unweighted.cc)
    target_include_directories(run_unweighted PUBLIC .)
    target_link_libraries(run_unweighted PUBLIC gbbs)
    
    
endif()
