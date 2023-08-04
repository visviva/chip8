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

#include <fstream>
#include <random>

#include "font.h"
#include "types.h"

namespace chip8
{
struct Chip8
{
    Chip8();

    /// <summary>
    /// Load a ROM into the memory
    /// </summary>
    /// <param name="filename"> Path to the ROM</param>
    void LoadRom(std::string_view filename);

    /// <summary>
    /// Fetch instruction, decode, execute
    /// </summary>
    void Cycle();

    /// <summary>
    ///  Clear the display
    /// </summary>
    void OP_00E0();

    /// <summary>
    /// Return from a subroutine
    /// </summary>
    void OP_00EE();

    /// <summary>
    /// Jump to location nnn
    /// The interpreter sets the program counter to nnn
    /// </summary>
    void OP_1nnn();

    /// <summary>
    /// Call subroutine at nnn
    /// </summary>
    void OP_2nnn();

    /// <summary>
    /// Skip next instruction if registers[x] == kk
    /// </summary>
    void OP_3xkk();

    /// <summary>
    /// Skip next instruction if registers[x] != kk
    /// </summary>
    void OP_4xkk();

    /// <summary>
    /// Skip next instruction if registers[x] == registers[y]
    /// </summary>
    void OP_5xy0();

    /// <summary>
    /// Load kk to registers[x]
    /// </summary>
    void OP_6xkk();

    /// <summary>
    /// Add kk to registers[x]
    /// </summary>
    void OP_7xkk();

    /// <summary>
    /// Copy registers[y] to registers[x]
    /// </summary>
    void OP_8xy0();

    /// <summary>
    /// Set registers[x] to registers[x] OR registers[y]
    /// </summary>
    void OP_8xy1();

    /// <summary>
    /// Set registers[x] to registers[x] AND registers[y]
    /// </summary>
    void OP_8xy2();

    /// <summary>
    /// Set registers[x] to registers[x] XOR registers[y]
    /// </summary>
    void OP_8xy3();

    /// <summary>
    /// Add register[y] to register[x] and set register[0xF] when sum wraps
    /// </summary>
    void OP_8xy4();

    /// <summary>
    /// Subtract register[y] from register[x] and set register[0xF] when borrow
    /// </summary>
    void OP_8xy5();

    /// <summary>
    /// Set registers[x] >>= registers[x] and save the carry in registers[0xF]
    /// </summary>
    void OP_8xy6();

    /// <summary>
    /// Subtract register[x] from register[y] and save in registers[x] and set register[0xF] when borrow
    /// </summary>
    void OP_8xy7();

    /// <summary>
    /// Set registers[x] <<= registers[x] and save the carry in registers[0xF]
    /// </summary>
    void OP_8xyE();

    /// <summary>
    /// Skip next instruction if registers[x] != registers[y]
    /// </summary>
    void OP_9xy0();

    /// <summary>
    /// Set index to nnn
    /// </summary>
    void OP_Annn();

    /// <summary>
    /// Jump to nnn + registers[0]
    /// </summary>
    void OP_Bnnn();

    /// <summary>
    /// Set registers[x] to a random byte AND kk
    /// </summary>
    void OP_Cxkk();

    /// <summary>
    /// Display n-byte sprite starting at memory location I at (reg[x],reg[y])
    /// If there is a collision, reg[0xF] is set
    /// </summary>
    void OP_Dxyn();

    void OP_Ex9E();

    void OP_ExA1();

    void OP_Fx07();

    void OP_Fx0A();

    void OP_Fx15();

    void OP_Fx18();

    void OP_Fx1E();

    void OP_Fx29();

    void OP_Fx33();

    void OP_Fx55();

    void OP_Fx65();

    register_set registers{};

    constexpr static u32 START_ADDRESS = 0x200;
    constexpr static u32 FONTSET_START_ADDRESS = 0x50;
    memory_t memory{};

    u16 index{};
    u16 pc{START_ADDRESS};
    stack_t stack{};
    u8 sp{};
    u8 delayTimer{};
    u8 soundTimer{};
    keypad_t keypad{};
    video_mem_t video{};
    u16 opcode{};

    std::default_random_engine randomGenerator;
    std::uniform_int_distribution<unsigned int> randomByte;

    using Chip8Func = void(Chip8::*)();
    Chip8Func table[0xF + 1];
    Chip8Func table0[0xE + 1];
    Chip8Func table8[0xE + 1];
    Chip8Func tableE[0xE + 1];
    Chip8Func tableF[0x65 + 1];

    void InitInstructionTable();
    void Table0();
    void Table8();
    void TableE();
    void TableF();
    void OP_NOP();
};
}  // namespace chip8