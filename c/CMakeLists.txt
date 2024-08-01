cmake_minimum_required(VERSION 3.21)

# CMake cache variables
option(BUILD_TESTS "Build the test cases in the `test/` folder." ON)
if (BUILD_TESTS)
    enable_testing()
endif ()

# Project settings
project(iml-paintability)

set(CMAKE_CXX_STANDARD 20)
set(DEPS_PATH "${CMAKE_SOURCE_DIR}/deps")
list(APPEND CMAKE_PREFIX_PATH "${DEPS_PATH}")

# Compiler flags
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
if (MSVC)
    add_compile_options(/utf-8 /W4 /WX)
    add_compile_options("$<$<CONFIG:Release>:/Ox>")
    add_compile_options("$<$<CONFIG:Debug>:/Zi>")
    add_compile_options("/arch:AVX2")
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
else ()
    add_compile_options(-Wall -Wextra -Werror)
    add_compile_options("$<$<CONFIG:Release>:-O3>")
    add_compile_options("$<$<CONFIG:Debug>:-g>")
    add_compile_options("-march=native")
endif ()

# For static analysis and linting
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE HEADERS "${CMAKE_SOURCE_DIR}/include/*.h")
file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")

# Main target
add_executable(main main.cpp ${SOURCES})
set_target_properties(main PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
        DEBUG_POSTFIX -d
)
target_include_directories(main PRIVATE "${CMAKE_SOURCE_DIR}/include")

# Verify target
add_executable(verify verify.cpp ${SOURCES})
set_target_properties(verify PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
        DEBUG_POSTFIX -d
)
target_include_directories(verify PRIVATE "${CMAKE_SOURCE_DIR}/include")

# Testcases
if (BUILD_TESTS)
    add_subdirectory("test")
endif ()
