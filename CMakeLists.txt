cmake_minimum_required(VERSION 3.5)
include(CheckIncludeFiles)
include(CheckLibraryExists)

# Set the project name and version number.
project(interprogram VERSION 0.1.0 LANGUAGES C)

# Set up the type of build and the compiler flags to use.
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()
set(CMAKE_C_FLAGS "-Wall -Wextra ${CMAKE_C_FLAGS}")
set(CMAKE_C_FLAGS_DEBUG "-g")
set(CMAKE_C_FLAGS_RELEASE "-O2")

# Require the c99 standard to compile C code.
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

# Add the subdirectories.
include_directories(src/common)
add_subdirectory(src)
add_subdirectory(test)

# Enable testing support.
enable_testing()

# Custom 'test' rule to run the unit tests in a more verbose way.
add_custom_target(test-verbose COMMAND ${CMAKE_CTEST_COMMAND}
    --force-new-ctest-process
    --verbose
    --output-on-failure
)
