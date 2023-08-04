cmake_minimum_required(VERSION 3.15)

project(chip8 CXX)

include(cmake/warnings.cmake)
include(cmake/optimizations.cmake)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(emulator)

enable_testing()
add_subdirectory(test)