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

#include "emulator.h"

#include <chrono>

using namespace chip8;

Chip8::Chip8() : randomGenerator(std::chrono::system_clock::now().time_since_epoch().count())
{
    auto it = memory.begin();
    std::advance(it, FONTSET_START_ADDRESS);
    std::copy(fontset.begin(), fontset.end(), it);

    randomByte = std::uniform_int_distribution<unsigned int>(0, 255);

    InitInstructionTable();
}

void Chip8::LoadRom(std::string_view filename)
{
    std::ifstream file(filename.data(), std::ios::binary | std::ios::ate);

    if (!file.is_open())
    {
        return;
    }

    auto size = file.tellg();
    file.seekg(0, std::ios::beg);
    file.read(reinterpret_cast<char*>(&memory[START_ADDRESS]), size);
    file.close();
}

void Chip8::OP_00E0()
{
    std::fill(video.begin(), video.end(), 0);
}

void Chip8::OP_00EE()
{
    pc = stack[--sp];
}

void Chip8::OP_1nnn()
{
    pc = opcode & 0x0FFFu;
}

void Chip8::OP_2nnn()
{
    u16 address = opcode & 0x0FFFu;
    stack[sp++] = pc;
    pc = address;
}

#define GET_VX (opcode & 0x0F00u) >> 8u
#define GET_VY (opcode & 0x00F0u) >> 4u

void Chip8::OP_3xkk()
{
    u8 vx = GET_VX;
    u8 byte = opcode & 0x00FFu;

    if (registers[vx] == byte)
    {
        pc += 2;
    }
}

void Chip8::OP_4xkk()
{
    u8 vx = GET_VX;
    u8 byte = opcode & 0x00FFu;

    if (registers[vx] != byte)
    {
        pc += 2;
    }
}

void Chip8::OP_5xy0()
{
    u8 vx = GET_VX;
    u8 vy = GET_VY;

    if (registers[vx] == registers[vy])
    {
        pc += 2;
    }
}

void Chip8::OP_6xkk()
{
    u8 vx = GET_VX;
    u8 byte = opcode & 0x00FFu;

    registers[vx] = byte;
}

void Chip8::OP_7xkk()
{
    u8 vx = (opcode & 0xF00u) >> 8u;
    u8 byte = opcode & 0x00FFu;

    registers[vx] += byte;
}

void Chip8::OP_8xy0()
{
    u8 vx = GET_VX;
    u8 vy = GET_VY;

    registers[vx] = registers[vy];
}

void Chip8::OP_8xy1()
{
    u8 vx = GET_VX;
    u8 vy = GET_VY;

    registers[vx] |= registers[vy];
}

void Chip8::OP_8xy2()
{
    u8 vx = GET_VX;
    u8 vy = GET_VY;

    registers[vx] &= registers[vy];
}

void Chip8::OP_8xy3()
{
    u8 vx = GET_VX;
    u8 vy = GET_VY;

    registers[vx] ^= registers[vy];
}

void Chip8::OP_8xy4()
{
    u8 vx = GET_VX;
    u8 vy = GET_VY;

    u16 sum = registers[vx] + registers[vy];

    if (sum >= 256U)
    {
        registers[0xF] = 1;
    }
    else
    {
        registers[0xF] = 0;
    }

    registers[vx] = sum & 0xFFu;
}

void Chip8::OP_8xy5()
{
    u8 vx = GET_VX;
    u8 vy = GET_VY;

    if (registers[vx] > registers[vy])
    {
        registers[0xF] = 1u;
    }
    else
    {
        registers[0xF] = 0;
    }

    registers[vx] -= registers[vy];
}

void Chip8::OP_8xy6()
{
    u8 vx = GET_VX;

    registers[0xF] = (registers[vx] & 0x01u);
    registers[vx] >>= 1;
}

void Chip8::OP_8xy7()
{
    u8 vx = GET_VX;
    u8 vy = GET_VY;

    if (registers[vy] > registers[vx])
    {
        registers[0xF] = 1u;
    }
    else
    {
        registers[0xF] = 0;
    }

    registers[vx] = registers[vy] - registers[vx];
}

void Chip8::OP_8xyE()
{
    u8 vx = GET_VX;

    registers[0xF] = (registers[vx] & 0x80u) >> 7u;
    registers[vx] <<= 1;
}

void Chip8::OP_9xy0()
{
    u8 vx = GET_VX;
    u8 vy = GET_VY;

    if (registers[vx] != registers[vy])
    {
        pc += 2;
    }
}

void Chip8::OP_Annn()
{
    index = opcode & 0x0FFFu;
}

void Chip8::OP_Bnnn()
{
    u16 address = opcode & 0x0FFFu;
    pc = registers[0] + address;
}

void Chip8::OP_Cxkk()
{
    u8 vx = GET_VX;
    u8 byte = opcode & 0x00FFu;

    registers[vx] = randomByte(randomGenerator) & byte;
}

void Chip8::OP_Dxyn()
{
    u8 vx = GET_VX;
    u8 vy = GET_VY;
    u8 height = opcode & 0x000Fu;

    u8 xPos = registers[vx] % VIDEO_WIDTH;
    u8 yPos = registers[vy] % VIDEO_HEIGHT;

    registers[0xF] = 0;

    for (size_t row = 0; row < height; row++)
    {
        u8 spriteByte = memory[index + row];

        for (size_t col = 0; col < 8; col++)
        {
            u32 index = (yPos + row) * VIDEO_WIDTH + (xPos + col);
            index %= (VIDEO_WIDTH * VIDEO_HEIGHT);
            u8 spritePixel = spriteByte & (0x80 >> col);
            auto* screenPixel = &video[index];

            if (spritePixel)
            {
                if (*screenPixel == UINT32_MAX)
                {
                    registers[0xF] = 1;
                }

                *screenPixel ^= UINT32_MAX;
            }
        }
    }
}

void chip8::Chip8::OP_Ex9E()
{
    u8 vx = GET_VX;
    u8 key = registers[vx];

    if (keypad[key])
    {
        pc += 2;
    }
}

void chip8::Chip8::OP_ExA1()
{
    u8 vx = GET_VX;
    u8 key = registers[vx];

    if (!keypad[key])
    {
        pc += 2;
    }
}

void chip8::Chip8::OP_Fx07()
{
    u8 vx = GET_VX;
    registers[vx] = delayTimer;
}

void chip8::Chip8::OP_Fx0A()
{
    u8 vx = GET_VX;

    for (u8 i = 0; i < keypad.size(); ++i)
    {
        if (keypad[i])
        {
            registers[vx] = i;
            return;
        }
    }

    pc -= 2;
}

void chip8::Chip8::OP_Fx15()
{
    u8 vx = GET_VX;
    delayTimer = registers[vx];
}

void chip8::Chip8::OP_Fx18()
{
    u8 vx = GET_VX;
    soundTimer = registers[vx];
}

void chip8::Chip8::OP_Fx1E()
{
    u8 vx = GET_VX;
    index += registers[vx];
}

void chip8::Chip8::OP_Fx29()
{
    u8 vx = GET_VX;
    u8 digit = registers[vx];

    index = FONTSET_START_ADDRESS + (5 * digit);
}

void chip8::Chip8::OP_Fx33()
{
    u8 vx = GET_VX;
    u8 value = registers[vx];

    memory[index + 2] = value % 10;
    value /= 10;

    memory[index + 1] = value % 10;
    value /= 10;

    memory[index] = value % 10;
}

void chip8::Chip8::OP_Fx55()
{
    u8 vx = GET_VX;

    for (u8 i = 0; i <= vx; ++i)
    {
        memory[index + i] = registers[i];
    }
}

void chip8::Chip8::OP_Fx65()
{
    u8 vx = GET_VX;

    for (u8 i = 0; i <= vx; ++i)
    {
        registers[i] = memory[index + i];
    }
}

void chip8::Chip8::Table0()
{
    ((*this).*(table0[opcode & 0x000Fu]))();
}

void chip8::Chip8::Table8()
{
    ((*this).*(table8[opcode & 0x000Fu]))();
}

void chip8::Chip8::TableE()
{
    ((*this).*(tableE[opcode & 0x000Fu]))();
}

void chip8::Chip8::TableF()
{
    ((*this).*(tableF[opcode & 0x00FFu]))();
}

void chip8::Chip8::OP_NOP() {}

void chip8::Chip8::InitInstructionTable()
{
    table[0x0] = &Chip8::Table0;
    table[0x1] = &Chip8::OP_1nnn;
    table[0x2] = &Chip8::OP_2nnn;
    table[0x3] = &Chip8::OP_3xkk;
    table[0x4] = &Chip8::OP_4xkk;
    table[0x5] = &Chip8::OP_5xy0;
    table[0x6] = &Chip8::OP_6xkk;
    table[0x7] = &Chip8::OP_7xkk;
    table[0x8] = &Chip8::Table8;
    table[0x9] = &Chip8::OP_9xy0;
    table[0xA] = &Chip8::OP_Annn;
    table[0xB] = &Chip8::OP_Bnnn;
    table[0xC] = &Chip8::OP_Cxkk;
    table[0xD] = &Chip8::OP_Dxyn;
    table[0xE] = &Chip8::TableE;
    table[0xF] = &Chip8::TableF;

    for (size_t i = 0; i <= 0xE; i++)
    {
        table0[i] = &Chip8::OP_NOP;
        table8[i] = &Chip8::OP_NOP;
        tableE[i] = &Chip8::OP_NOP;
    }

    table0[0x0] = &Chip8::OP_00E0;
    table0[0xE] = &Chip8::OP_00EE;

    table8[0x0] = &Chip8::OP_8xy0;
    table8[0x1] = &Chip8::OP_8xy1;
    table8[0x2] = &Chip8::OP_8xy2;
    table8[0x3] = &Chip8::OP_8xy3;
    table8[0x4] = &Chip8::OP_8xy4;
    table8[0x5] = &Chip8::OP_8xy5;
    table8[0x6] = &Chip8::OP_8xy6;
    table8[0x7] = &Chip8::OP_8xy7;
    table8[0xE] = &Chip8::OP_8xyE;

    tableE[0x1] = &Chip8::OP_ExA1;
    tableE[0xE] = &Chip8::OP_Ex9E;

    for (size_t i = 0; i <= 0x65; i++)
    {
        tableF[i] = &Chip8::OP_NOP;
    }

    tableF[0x07] = &Chip8::OP_Fx07;
    tableF[0x0A] = &Chip8::OP_Fx0A;
    tableF[0x15] = &Chip8::OP_Fx15;
    tableF[0x18] = &Chip8::OP_Fx18;
    tableF[0x1E] = &Chip8::OP_Fx1E;
    tableF[0x29] = &Chip8::OP_Fx29;
    tableF[0x33] = &Chip8::OP_Fx33;
    tableF[0x55] = &Chip8::OP_Fx55;
    tableF[0x65] = &Chip8::OP_Fx65;
}

void chip8::Chip8::Cycle()
{
    // Fetch
    opcode = (memory[pc] << 8u) | memory[pc + 1];

    // Increment the PC before we execute anything
    pc += 2;

    // Decode and Execute
    ((*this).*(table[(opcode & 0xF000u) >> 12u]))();

    // Decrement the delay timer if it's been set
    if (delayTimer > 0)
    {
        --delayTimer;
    }

    // Decrement the sound timer if it's been set
    if (soundTimer > 0)
    {
        --soundTimer;
    }
}
