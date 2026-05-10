cmake_minimum_required(VERSION 3.28)

project(cppart VERSION 1.0 LANGUAGES CXX)

option(CPPART_BUILD_TESTS "Build tests" OFF)
option(CPPART_BUILD_BENCHMARKS "Build benchmarks" OFF)
option(CPPART_BUILD_EXAMPLES "Build examples" OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(ConfigureCompiler)
include(CompilerWarnings)

add_subdirectory(src)

if (CPPART_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif ()

if (CPPART_BUILD_BENCHMARKS)
    add_subdirectory(benchmarks)
endif ()

if (CPPART_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif ()
