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
    /// 00EE - RET
    /// Return from a subroutine.
    ///
    /// The interpreter sets the program counter to the address at the top of the stack,
    /// then subtracts 1 from the stack pointer.
    /// </summary>
    void OP_00EE();

    /// <summary>
    /// 1nnn - JP addr
    /// Jump to location nnn.
    ///
    /// The interpreter sets the program counter to nnn.
    /// </summary>
    void OP_1nnn();

    /// <summary>
    /// 2nnn - CALL addr Call subroutine at nnn.
    ///
    /// The interpreter increments the stack pointer,
    /// then puts the current PC on the top of the stack.The PC is then set to nnn.
    /// </summary>
    void OP_2nnn();

    /// <summary>
    /// 3xkk - SE Vx, byte
    /// Skip next instruction if Vx = kk.
    ///
    /// The interpreter compares register Vx to kk, and if they are equal,
    /// increments the program counter by 2.
    /// </summary>
    void OP_3xkk();

    /// <summary>
    /// 4xkk - SNE Vx, byte
    /// Skip next instruction if Vx != kk.
    ///
    /// The interpreter compares register Vx to kk, and if they are not equal, increments the program counter by 2.
    /// </summary>
    void OP_4xkk();

    /// <summary>
    /// 5xy0 - SE Vx, Vy
    /// Skip next instruction if Vx = Vy.
    ///
    /// The interpreter compares register Vx to register Vy, and if they are equal, increments the program counter by 2.
    /// </summary>
    void OP_5xy0();

    /// <summary>
    /// 6xkk - LD Vx, byte
    /// Set Vx = kk.
    ///
    /// The interpreter puts the value kk into register Vx.
    /// </summary>
    void OP_6xkk();

    /// <summary>
    /// 7xkk - ADD Vx, byte
    /// Set Vx = Vx + kk.
    ///
    /// Adds the value kk to the value of register Vx, then stores the result in Vx.
    /// </summary>
    void OP_7xkk();

    /// <summary>
    /// 8xy0 - LD Vx, Vy
    /// Set Vx = Vy.
    ///
    /// Stores the value of register Vy in register Vx.
    /// </summary>
    void OP_8xy0();

    /// <summary>
    /// 8xy1 - OR Vx, Vy
    /// Set Vx = Vx OR Vy.
    ///
    /// Performs a bitwise OR on the values of Vx and Vy, then stores the result in Vx. A bitwise OR compares the
    /// corresponding bits from two values, and if either bit is 1, then the same bit in the result is also 1.
    /// Otherwise, it is 0.
    /// </summary>
    void OP_8xy1();

    /// <summary>
    /// 8xy2 - AND Vx, Vy
    /// Set Vx = Vx AND Vy.
    ///
    /// Performs a bitwise AND on the values of Vx and Vy, then stores the result in Vx. A bitwise AND compares the
    /// corresponding bits from two values, and if both bits are 1, then the same bit in the result is also 1.
    /// Otherwise, it is 0.
    /// </summary>
    void OP_8xy2();

    /// <summary>
    /// 8xy3 - XOR Vx, Vy
    /// Set Vx = Vx XOR Vy.
    ///
    /// Performs a bitwise exclusive OR on the values of Vx and Vy, then stores the result in Vx. An exclusive OR
    /// compares the corresponding bits from two values, and if the bits are not both the same, then the corresponding
    /// bit in the result is set to 1. Otherwise, it is 0.
    /// </summary>
    void OP_8xy3();

    /// <summary>
    /// 8xy4 - ADD Vx, Vy
    /// Set Vx = Vx + Vy, set VF = carry.
    ///
    /// The values of Vx and Vy are added together. If the result is greater than 8 bits (i.e., > 255,) VF is set to 1,
    /// otherwise 0. Only the lowest 8 bits of the result are kept, and stored in Vx.
    /// </summary>
    void OP_8xy4();

    /// <summary>
    /// 8xy5 - SUB Vx, Vy
    /// Set Vx = Vx - Vy, set VF = NOT borrow.
    ///
    /// If Vx > Vy, then VF is set to 1, otherwise 0. Then Vy is subtracted from Vx, and the results stored in Vx.
    /// </summary>
    void OP_8xy5();

    /// <summary>
    /// 8xy6 - SHR Vx {, Vy}
    /// Set Vx = Vx SHR 1.
    ///
    /// If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0. Then Vx is divided by 2. 
    /// </summary>
    void OP_8xy6();

    /// <summary>
    /// 8xy7 - SUBN Vx, Vy
    /// Set Vx = Vy - Vx, set VF = NOT borrow.
    ///
    /// If Vy > Vx, then VF is set to 1, otherwise 0. Then Vx is subtracted from Vy, and the results stored in Vx.
    /// </summary>
    void OP_8xy7();

    /// <summary>
    /// 8xyE - SHL Vx {, Vy}
    /// Set Vx = Vx SHL 1.
    ///
    /// If the most-significant bit of Vx is 1, then VF is set to 1, otherwise to 0. Then Vx is multiplied by 2.
    /// </summary>
    void OP_8xyE();

    /// <summary>
    /// 9xy0 - SNE Vx, Vy
    /// Skip next instruction if Vx != Vy.
    ///
    /// The values of Vx and Vy are compared, and if they are not equal, the program counter is increased by 2.
    /// </summary>
    void OP_9xy0();

    /// <summary>
    /// Annn - LD I, addr
    /// Set I = nnn.
    ///
    /// The value of register I is set to nnn.
    /// </summary>
    void OP_Annn();

    /// <summary>
    /// Bnnn - JP V0, addr
    /// Jump to location nnn + V0.
    ///
    /// The program counter is set to nnn plus the value of V0.
    /// </summary>
    void OP_Bnnn();

    /// <summary>
    /// Cxkk - RND Vx, byte
    /// Set Vx = random byte AND kk.
    ///
    /// The interpreter generates a random number from 0 to 255, which is then ANDed with the value kk. The results are
    /// stored in Vx. See instruction 8xy2 for more information on AND.
    /// </summary>
    void OP_Cxkk();

    /// <summary>
    /// Dxyn - DRW Vx, Vy, nibble
    /// Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
    ///
    /// The interpreter reads n bytes from memory, starting at the address stored in I. These bytes are then displayed
    /// as sprites on screen at coordinates (Vx, Vy). Sprites are XORed onto the existing screen. If this causes any
    /// pixels to be erased, VF is set to 1, otherwise it is set to 0. If the sprite is positioned so part of it is
    /// outside the coordinates of the display, it wraps around to the opposite side of the screen. See instruction 8xy3
    /// for more information on XOR, and section 2.4, Display, for more information on the Chip-8 screen and sprites.
    /// </summary>
    void OP_Dxyn();

    /// <summary>
    /// Ex9E - SKP Vx
    /// Skip next instruction if key with the value of Vx is pressed.
    ///
    /// Checks the keyboard, and if the key corresponding to the value of Vx is currently in the down position, PC is
    /// increased by 2.
    /// </summary>
    void OP_Ex9E();

    /// <summary>
    /// ExA1 - SKNP Vx
    /// Skip next instruction if key with the value of Vx is not pressed.
    ///
    /// Checks the keyboard, and if the key corresponding to the value of Vx is currently in the up position, PC is
    /// increased by 2.
    /// </summary>
    void OP_ExA1();

    /// <summary>
    /// Fx07 - LD Vx, DT
    /// Set Vx = delay timer value.
    ///
    /// The value of DT is placed into Vx.
    /// </summary>
    void OP_Fx07();

    /// <summary>
    /// Fx0A - LD Vx, K
    /// Wait for a key press, store the value of the key in Vx.
    ///
    /// All execution stops until a key is pressed, then the value of that key is stored in Vx.
    /// </summary>
    void OP_Fx0A();

    /// <summary>
    /// Fx15 - LD DT, Vx
    /// Set delay timer = Vx.
    ///
    /// DT is set equal to the value of Vx.
    /// </summary>
    void OP_Fx15();

    /// <summary>
    /// Fx18 - LD ST, Vx
    /// Set sound timer = Vx.
    ///
    /// ST is set equal to the value of Vx.
    /// </summary>
    void OP_Fx18();

    /// <summary>
    /// Fx1E - ADD I, Vx
    /// Set I = I + Vx.
    ///
    /// The values of I and Vx are added, and the results are stored in I.
    /// </summary>
    void OP_Fx1E();

    /// <summary>
    /// Fx29 - LD F, Vx
    /// Set I = location of sprite for digit Vx.
    ///
    /// The value of I is set to the location for the hexadecimal sprite corresponding to the value of Vx. See
    /// section 2.4, Display, for more information on the Chip-8 hexadecimal font.
    /// </summary>
    void OP_Fx29();

    /// <summary>
    /// Fx33 - LD B, Vx
    /// Store BCD representation of Vx in memory locations I, I+1, and I+2.
    ///
    /// The interpreter takes the decimal value of Vx, and places the hundreds digit in memory at location in I, the
    /// tens digit at location I+1, and the ones digit at location I+2.
    /// </summary>
    void OP_Fx33();

    /// <summary>
    /// Fx55 - LD [I], Vx
    /// Store registers V0 through Vx in memory starting at location I.
    ///
    /// The interpreter copies the values of registers V0 through Vx into memory, starting at the address in I.
    /// </summary>
    void OP_Fx55();

    /// <summary>
    /// Fx65 - LD Vx, [I]
    /// Read registers V0 through Vx from memory starting at location I.
    ///
    /// The interpreter reads values from memory starting at location I into registers V0 through Vx.
    /// </summary>
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

    using Chip8Func = void (Chip8::*)();
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