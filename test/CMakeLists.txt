
find_package(GTest CONFIG REQUIRED)
include(GoogleTest)

add_executable(chip8_test test.cpp)
target_link_libraries(chip8_test PRIVATE GTest::gmock_main)
target_link_libraries(chip8_test PRIVATE emulator)
set_warning_flags(chip8_test "Debug")

gtest_discover_tests(chip8_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
set_property(TARGET chip8_test PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
