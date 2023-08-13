[![Build and Test](https://github.com/visviva/chip8/actions/workflows/build_test.yml/badge.svg)](https://github.com/visviva/chip8/actions/workflows/build_test.yml)
# CHIP8 Emulator

This emulator was developed on Windows 11.

Install [vcpkg](https://vcpkg.io/en/index.html) and run cmake as described [here](https://learn.microsoft.com/en-us/vcpkg/examples/manifest-mode-cmake) or build it using [Visual Studio](https://devblogs.microsoft.com/cppblog/vcpkg-is-now-included-with-visual-studio/).

To build it with CMake:
```sh
$ cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=\PATH\TO\vcpkg\scripts\buildsystems\vcpkg.cmake
$ cmake --build build
```


https://github.com/visviva/chip8/assets/72554879/4bb5a194-c4a5-4be9-a795-d563b45ba200



Get awesome games from [here](https://github.com/dmatlack/chip8/tree/master/roms).
