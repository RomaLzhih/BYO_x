# Utilities
if(BUILD_BENCHMARKS)
    # Graph compressor utility
    add_executable(compressor compressor.cc)
    target_link_libraries(compressor gbbs)

    # SNAP converter utility  
    add_executable(snap_converter snap_converter.cc)
    target_link_libraries(snap_converter gbbs)
endif()