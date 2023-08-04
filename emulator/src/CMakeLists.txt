find_package(sdl2 REQUIRED)

add_library(emulator OBJECT "emulator.cpp")
set_warning_flags(emulator "Debug")
target_include_directories(emulator PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../include")

list(APPEND app_sources main.cpp platform.cpp)

if (UNIX AND NOT APPLE)
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        add_executable(chip8 ${app_sources})
    endif()
elseif (MSVC)
      add_executable(chip8 WIN32 ${app_sources})
endif()

set_warning_flags(chip8 "Debug")
target_link_libraries(chip8 PRIVATE emulator)
target_link_libraries(chip8 PRIVATE SDL2::SDL2 SDL2::SDL2main)
