/// MIT License
///
/// Copyright(c) 2023 Simon Lauser
///
/// Permission is hereby granted,
/// free of charge, to any person obtaining a copy of this software and associated documentation files(the "Software"),
/// to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge,
/// publish, distribute, sublicense, and / or sell copies of the Software,
/// and to permit persons to whom the Software is furnished to do so,
/// subject to the following conditions :
///
/// The above copyright notice and this permission notice shall be included in all copies
/// or
/// substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS",
/// WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND
/// NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
/// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#pragma once

#include <array>

namespace chip8
{
using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;

using register_set = std::array<u8, 16>;
using memory_t = std::array<u8, 4096>;
using stack_t = std::array<u16, 16>;
using keypad_t = std::array<u8, 16>;

constexpr u32 VIDEO_WIDTH = 64;
constexpr u32 VIDEO_HEIGHT = 32;

using video_mem_t = std::array<u32, VIDEO_WIDTH * VIDEO_HEIGHT>;
}  // namespace chip8