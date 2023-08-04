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

#include <gtest/gtest.h>

#include "emulator.h"
#include "types.h"

using namespace chip8;

TEST(LoadROM, LoadTestRomInMemoryAndCheckMemory)
{
    chip8::Chip8 emulator;

    emulator.LoadRom("test.rom");

    auto index = emulator.START_ADDRESS;

    ASSERT_EQ(0x01, emulator.memory[index++]);
    ASSERT_EQ(0x23, emulator.memory[index++]);
    ASSERT_EQ(0x45, emulator.memory[index++]);
    ASSERT_EQ(0x67, emulator.memory[index++]);
    ASSERT_EQ(0x89, emulator.memory[index++]);
    ASSERT_EQ(0xaa, emulator.memory[index++]);
    ASSERT_EQ(0xbb, emulator.memory[index++]);
    ASSERT_EQ(0xcc, emulator.memory[index++]);
    ASSERT_EQ(0xdd, emulator.memory[index++]);
    ASSERT_EQ(0xee, emulator.memory[index++]);
    ASSERT_EQ(0xff, emulator.memory[index++]);
}

TEST(LoadROM, CheckFontsinMemory)
{
    chip8::Chip8 emulator;

    for (size_t i = 0; i < chip8::FONTSET_SIZE; ++i)
    {
        ASSERT_EQ(emulator.memory[emulator.FONTSET_START_ADDRESS + i], chip8::fontset[i]);
    }
}

TEST(InstructionTest, Test_OP_00E0)
{
    chip8::Chip8 emulator;
    // Assuming OP_00E0 clear the display
    emulator.OP_00E0();
    // Check that the video memory is cleared.
    for (const auto& pixel : emulator.video)
    {
        EXPECT_EQ(pixel, 0);
    }
}

TEST(InstructionTest, Test_OP_00EE)
{
    // Assuming OP_00EE returns from a subroutine
    // Assuming the stack pointer is at 1 and will decrease to 0 after the function
    chip8::Chip8 emulator;

    // Set up the stack to simulate a call
    emulator.sp = 1;
    emulator.stack[0] = 0x300;  // Example return address

    // Execute the instruction
    emulator.OP_00EE();

    // Verify the stack pointer decremented
    ASSERT_EQ(emulator.sp, 0);

    // Verify the program counter was set to the return address
    ASSERT_EQ(emulator.pc, 0x300);
}

TEST(InstructionTest, Test_OP_1nnn)
{
    chip8::Chip8 emulator;

    // Assuming OP_1nnn jumps to location nnn
    // Test with different opcode values containing the address to jump to
    std::vector<u16> opcodes = {0x1123, 0x1200, 0x13AF, 0x1FFF};

    for (u16 opcode : opcodes)
    {
        // Set the opcode
        emulator.opcode = opcode;

        // Execute the instruction
        emulator.OP_1nnn();

        // Extract the address from the opcode
        u16 address = opcode & 0x0FFF;

        // Verify that the program counter was set to the correct address
        ASSERT_EQ(emulator.pc, address) << "Failed for opcode: 0x" << std::hex << opcode;
    }
}

TEST(InstructionTest, Test_OP_2nnn)
{
    chip8::Chip8 emulator;

    // Assuming OP_2nnn calls a subroutine at location nnn
    // Test with different opcode values containing the address to jump to
    std::vector<u16> opcodes = {0x2123, 0x2200, 0x23AF, 0x2FFF};

    for (u16 opcode : opcodes)
    {
        // Resetting stack pointer for each iteration
        emulator.sp = 0;

        // Set the opcode
        emulator.opcode = opcode;

        // Storing current program counter to validate if it's pushed to the stack
        u16 prevPc = emulator.pc;

        // Execute the instruction
        emulator.OP_2nnn();

        // Extract the address from the opcode
        u16 address = opcode & 0x0FFF;

        // Verify that the program counter was set to the correct address
        ASSERT_EQ(emulator.pc, address) << "Failed to jump to address for opcode: 0x" << std::hex << opcode;

        // Verify that the stack pointer was incremented
        ASSERT_EQ(emulator.sp, 1) << "Failed to increment stack pointer for opcode: 0x" << std::hex << opcode;

        // Verify that the previous program counter was stored on the stack
        ASSERT_EQ(emulator.stack[emulator.sp - 1], prevPc)
            << "Failed to store previous PC on the stack for opcode: 0x" << std::hex << opcode;
    }
}

TEST(InstructionTest, Test_OP_3xkk)
{
    chip8::Chip8 emulator;

    // Set up some test cases for register values and comparison bytes
    std::vector<std::pair<u8, u8>> testCases = {
        {0xAA, 0xAA},  // Equal values
        {0xAA, 0xAB},  // Unequal values
        {0x00, 0x00},  // Zero values
        {0xFF, 0xFF}   // Max values
    };

    // Starting address for the program counter
    u16 startAddress = emulator.pc;

    for (const auto& [registerValue, kk] : testCases)
    {
        // Set the value in register Vx (let's choose x = 5 for this test)
        emulator.registers[5] = registerValue;

        // Set the opcode for the instruction (using x = 5)
        emulator.opcode = 0x3000 | (5 << 8) | kk;

        // Execute the instruction
        emulator.OP_3xkk();

        // If the values were equal, the program counter should have incremented by 2
        if (registerValue == kk)
        {
            ASSERT_EQ(emulator.pc, startAddress + 2)
                << "Failed for register value: 0x" << std::hex << static_cast<int>(registerValue) << ", kk: 0x"
                << static_cast<int>(kk);
        }
        // If the values were not equal, the program counter should not have incremented
        else
        {
            ASSERT_EQ(emulator.pc, startAddress)
                << "Failed for register value: 0x" << std::hex << static_cast<int>(registerValue) << ", kk: 0x"
                << static_cast<int>(kk);
        }

        // Reset the program counter for the next iteration
        emulator.pc = startAddress;
    }
}

TEST(InstructionTest, Test_OP_4xkk)
{
    chip8::Chip8 emulator;

    // Set up some test cases for register values and comparison bytes
    std::vector<std::pair<u8, u8>> testCases = {
        {0xAA, 0xAA},  // Equal values
        {0xAA, 0xAB},  // Unequal values
        {0x00, 0x01},  // Zero values
        {0xFF, 0xFE}   // Max values
    };

    // Starting address for the program counter
    u16 startAddress = emulator.pc;

    for (const auto& [registerValue, kk] : testCases)
    {
        // Set the value in register Vx (let's choose x = 5 for this test)
        emulator.registers[5] = registerValue;

        // Set the opcode for the instruction (using x = 5)
        emulator.opcode = 0x4000 | (5 << 8) | kk;

        // Execute the instruction
        emulator.OP_4xkk();

        // If the values were not equal, the program counter should have incremented by 2
        if (registerValue != kk)
        {
            ASSERT_EQ(emulator.pc, startAddress + 2)
                << "Failed for register value: 0x" << std::hex << static_cast<int>(registerValue) << ", kk: 0x"
                << static_cast<int>(kk);
        }
        // If the values were equal, the program counter should not have incremented
        else
        {
            ASSERT_EQ(emulator.pc, startAddress)
                << "Failed for register value: 0x" << std::hex << static_cast<int>(registerValue) << ", kk: 0x"
                << static_cast<int>(kk);
        }

        // Reset the program counter for the next iteration
        emulator.pc = startAddress;
    }
}

TEST(InstructionTest, Test_OP_5xy0)
{
    chip8::Chip8 emulator;

    // Set up some test cases for register values
    std::vector<std::pair<u8, u8>> testCases = {
        {0xAA, 0xAA},  // Equal values
        {0xAA, 0xAB},  // Unequal values
        {0x00, 0x00},  // Zero values
        {0xFF, 0xFF}   // Max values
    };

    // Starting address for the program counter
    u16 startAddress = emulator.pc;

    for (const auto& [vx, vy] : testCases)
    {
        // Set the values in registers Vx and Vy (let's choose x = 5 and y = 3 for this test)
        emulator.registers[5] = vx;
        emulator.registers[3] = vy;

        // Set the opcode for the instruction (using x = 5 and y = 3)
        emulator.opcode = 0x5000 | (5 << 8) | (3 << 4);

        // Execute the instruction
        emulator.OP_5xy0();

        // If the values were equal, the program counter should have incremented by 2
        if (vx == vy)
        {
            ASSERT_EQ(emulator.pc, startAddress + 2)
                << "Failed for Vx: 0x" << std::hex << static_cast<int>(vx) << ", Vy: 0x" << static_cast<int>(vy);
        }
        // If the values were not equal, the program counter should not have incremented
        else
        {
            ASSERT_EQ(emulator.pc, startAddress)
                << "Failed for Vx: 0x" << std::hex << static_cast<int>(vx) << ", Vy: 0x" << static_cast<int>(vy);
        }

        // Reset the program counter for the next iteration
        emulator.pc = startAddress;
    }
}

TEST(InstructionTest, Test_OP_6xkk)
{
    chip8::Chip8 emulator;

    // Set up some test cases for register values and bytes to be loaded
    std::vector<std::pair<u8, u8>> testCases = {
        {5, 0xAA},  // Regular value
        {3, 0x00},  // Zero value
        {7, 0xFF}   // Max value
    };

    for (const auto& [registerIndex, kk] : testCases)
    {
        // Set the opcode for the instruction (using the test register index and kk value)
        emulator.opcode = 0x6000 | (registerIndex << 8) | kk;

        // Execute the instruction
        emulator.OP_6xkk();

        // Verify that the value in register Vx has been set to kk
        ASSERT_EQ(emulator.registers[registerIndex], kk)
            << "Failed for register index: " << registerIndex << ", kk: 0x" << std::hex << static_cast<int>(kk);
    }
}

TEST(InstructionTest, Test_OP_7xkk)
{
    chip8::Chip8 emulator;

    // Set up some test cases for register values and bytes to be added
    std::vector<std::tuple<u8, u8, u8>> testCases = {
        {5, 0xAA, 0x10},  // Regular value
        {3, 0x00, 0x00},  // Zero value
        {7, 0xFF, 0x01}   // Testing wrap-around (0xFF + 0x01 should wrap to 0x00)
    };

    for (const auto& [registerIndex, registerValue, kk] : testCases)
    {
        // Set the initial value in register Vx
        emulator.registers[registerIndex] = registerValue;

        // Set the opcode for the instruction (using the test register index and kk value)
        emulator.opcode = 0x7000 | (registerIndex << 8) | kk;

        // Execute the instruction
        emulator.OP_7xkk();

        // Calculate the expected result (considering wrap-around)
        u8 expectedValue = (registerValue + kk) & 0xFF;

        // Verify that the value in register Vx has been correctly updated
        ASSERT_EQ(emulator.registers[registerIndex], expectedValue)
            << "Failed for register index: " << registerIndex << ", register value: 0x" << std::hex
            << static_cast<int>(registerValue) << ", kk: 0x" << static_cast<int>(kk);
    }
}

TEST(InstructionTest, Test_OP_8xy0)
{
    chip8::Chip8 emulator;

    // Set up some test cases for register values
    std::vector<std::tuple<u8, u8, u8>> testCases = {
        {5, 3, 0xAA},  // Regular values
        {3, 2, 0x00},  // Zero values
        {7, 6, 0xFF}   // Max values
    };

    for (const auto& [regX, regY, vy] : testCases)
    {
        // Set the value in register Vy
        emulator.registers[regY] = vy;

        // Set the opcode for the instruction (using the test register indices)
        emulator.opcode = 0x8000 | (regX << 8) | (regY << 4);

        // Execute the instruction
        emulator.OP_8xy0();

        // Verify that the value in register Vx is now equal to the value in Vy
        ASSERT_EQ(emulator.registers[regX], vy) << "Failed for Vx index: " << regX << ", Vy index: " << regY
                                                << ", Vy value: 0x" << std::hex << static_cast<int>(vy);
    }
}

TEST(InstructionTest, Test_OP_8xy1)
{
    chip8::Chip8 emulator;

    // Set up some test cases for register values
    std::vector<std::tuple<u8, u8, u8, u8>> testCases = {
        {5, 3, 0xAA, 0x55},  // Regular values
        {3, 2, 0x00, 0xFF},  // Zero value for Vx
        {7, 6, 0xFF, 0x00}   // Zero value for Vy
    };

    for (const auto& [regX, regY, vx, vy] : testCases)
    {
        // Set the value in register Vx and Vy
        emulator.registers[regX] = vx;
        emulator.registers[regY] = vy;

        // Set the opcode for the instruction (using the test register indices)
        emulator.opcode = 0x8001 | (regX << 8) | (regY << 4);

        // Execute the instruction
        emulator.OP_8xy1();

        // Calculate the expected result as the bitwise OR between vx and vy
        u8 expectedValue = vx | vy;

        // Verify that the value in register Vx is now equal to the expected result
        ASSERT_EQ(emulator.registers[regX], expectedValue)
            << "Failed for Vx index: " << regX << ", Vy index: " << regY << ", Vx value: 0x" << std::hex
            << static_cast<int>(vx) << ", Vy value: 0x" << static_cast<int>(vy);
    }
}

TEST(InstructionTest, Test_OP_8xy2)
{
    chip8::Chip8 emulator;

    // Set up some test cases for register values
    std::vector<std::tuple<u8, u8, u8, u8>> testCases = {
        {5, 3, 0xAA, 0x55},  // Regular values
        {3, 2, 0x00, 0xFF},  // Zero value for Vx
        {7, 6, 0xFF, 0x00}   // Zero value for Vy
    };

    for (const auto& [regX, regY, vx, vy] : testCases)
    {
        // Set the value in register Vx and Vy
        emulator.registers[regX] = vx;
        emulator.registers[regY] = vy;

        // Set the opcode for the instruction (using the test register indices)
        emulator.opcode = 0x8002 | (regX << 8) | (regY << 4);

        // Execute the instruction
        emulator.OP_8xy2();

        // Calculate the expected result as the bitwise AND between vx and vy
        u8 expectedValue = vx & vy;

        // Verify that the value in register Vx is now equal to the expected result
        ASSERT_EQ(emulator.registers[regX], expectedValue)
            << "Failed for Vx index: " << regX << ", Vy index: " << regY << ", Vx value: 0x" << std::hex
            << static_cast<int>(vx) << ", Vy value: 0x" << static_cast<int>(vy);
    }
}

TEST(InstructionTest, Test_OP_8xy3)
{
    chip8::Chip8 emulator;

    // Set up some test cases for register values
    std::vector<std::tuple<u8, u8, u8, u8>> testCases = {
        {5, 3, 0xAA, 0x55},  // Regular values
        {3, 2, 0x00, 0xFF},  // Zero value for Vx
        {7, 6, 0xFF, 0x00},  // Zero value for Vy
        {1, 1, 0xFF, 0xFF}   // Same values for Vx and Vy, XOR should result in 0
    };

    for (const auto& [regX, regY, vx, vy] : testCases)
    {
        // Set the value in register Vx and Vy
        emulator.registers[regX] = vx;
        emulator.registers[regY] = vy;

        // Set the opcode for the instruction (using the test register indices)
        emulator.opcode = 0x8003 | (regX << 8) | (regY << 4);

        // Execute the instruction
        emulator.OP_8xy3();

        // Calculate the expected result as the bitwise XOR between vx and vy
        u8 expectedValue = vx ^ vy;

        // Verify that the value in register Vx is now equal to the expected result
        ASSERT_EQ(emulator.registers[regX], expectedValue)
            << "Failed for Vx index: " << regX << ", Vy index: " << regY << ", Vx value: 0x" << std::hex
            << static_cast<int>(vx) << ", Vy value: 0x" << static_cast<int>(vy);
    }
}

TEST(InstructionTest, Test_OP_8xy4)
{
    chip8::Chip8 emulator;

    // Set up some test cases for register values
    // Including cases for no carry and carry
    std::vector<std::tuple<u8, u8, u8, u8, u8, u8>> testCases = {
        {5, 3, 0x20, 0x10, 0x30, 0x0},  // No carry
        {3, 2, 0xFF, 0x01, 0x00, 0x1}   // Carry
    };

    for (const auto& [regX, regY, vx, vy, expectedValue, expectedCarry] : testCases)
    {
        // Reset the emulator state
        emulator = chip8::Chip8();

        // Set the value in register Vx and Vy
        emulator.registers[regX] = vx;
        emulator.registers[regY] = vy;

        // Set the opcode for the instruction (using the test register indices)
        emulator.opcode = 0x8004 | (regX << 8) | (regY << 4);

        // Execute the instruction
        emulator.OP_8xy4();

        // Verify that the value in register Vx is now equal to the expected result
        ASSERT_EQ(emulator.registers[regX], expectedValue)
            << "Failed for Vx index: " << regX << ", Vy index: " << regY << ", Vx value: 0x" << std::hex
            << static_cast<int>(vx) << ", Vy value: 0x" << static_cast<int>(vy);

        // Verify that the carry flag (register V[0xF]) is set correctly
        ASSERT_EQ(emulator.registers[0xF], expectedCarry)
            << "Carry flag incorrect for Vx index: " << regX << ", Vy index: " << regY << ", Vx value: 0x" << std::hex
            << static_cast<int>(vx) << ", Vy value: 0x" << static_cast<int>(vy);
    }
}

TEST(InstructionTest, Test_OP_8xy5)
{
    chip8::Chip8 emulator;

    // Set up some test cases for register values
    // Including cases for no borrow and borrow
    std::vector<std::tuple<u8, u8, u8, u8, u8, u8>> testCases = {
        {5, 3, 0x20, 0x10, 0x10, 0x1},  // No borrow
        {3, 2, 0x01, 0xFF, 0x02, 0x0}   // Borrow
    };

    for (const auto& [regX, regY, vx, vy, expectedValue, expectedBorrow] : testCases)
    {
        // Reset the emulator state
        emulator = chip8::Chip8();

        // Set the value in register Vx and Vy
        emulator.registers[regX] = vx;
        emulator.registers[regY] = vy;

        // Set the opcode for the instruction (using the test register indices)
        emulator.opcode = 0x8005 | (regX << 8) | (regY << 4);

        // Execute the instruction
        emulator.OP_8xy5();

        // Verify that the value in register Vx is now equal to the expected result
        ASSERT_EQ(emulator.registers[regX], expectedValue)
            << "Failed for Vx index: " << regX << ", Vy index: " << regY << ", Vx value: 0x" << std::hex
            << static_cast<int>(vx) << ", Vy value: 0x" << static_cast<int>(vy);

        // Verify that the borrow flag (register V[0xF]) is set correctly
        ASSERT_EQ(emulator.registers[0xF], expectedBorrow)
            << "Borrow flag incorrect for Vx index: " << regX << ", Vy index: " << regY << ", Vx value: 0x" << std::hex
            << static_cast<int>(vx) << ", Vy value: 0x" << static_cast<int>(vy);
    }
}

TEST(InstructionTest, Test_OP_8xy6)
{
    chip8::Chip8 emulator;

    // Set up test cases for register values, including cases for LSB = 0 and LSB = 1
    std::vector<std::tuple<u8, u8, u8>> testCases = {
        {5, 0x2E, 0x0},  // LSB = 1, value is even
        {3, 0x11, 0x1}   // LSB = 0, value is odd
    };

    for (const auto& [regX, vx, expected_lsb] : testCases)
    {
        // Reset the emulator state
        emulator = chip8::Chip8();

        // Set the value in register Vx
        emulator.registers[regX] = vx;

        // Set the opcode for the instruction (using the test register indices)
        emulator.opcode = 0x8006 | (regX << 8);

        // Execute the instruction
        emulator.OP_8xy6();

        // Verify that the value in register Vx is now equal to the original value shifted right by one
        ASSERT_EQ(emulator.registers[regX], vx >> 1)
            << "Failed for Vx index: " << regX << ", Vx value: 0x" << std::hex << static_cast<int>(vx);

        // Verify that the LSB flag (register V[0xF]) is set correctly
        ASSERT_EQ(emulator.registers[0xF], expected_lsb)
            << "LSB flag incorrect for Vx index: " << regX << ", Vx value: 0x" << std::hex << static_cast<int>(vx);
    }
}

TEST(InstructionTest, Test_OP_8xy7)
{
    chip8::Chip8 emulator;

    // Set up test cases for register values, including cases where Vx is greater, less than, and equal to Vy
    std::vector<std::tuple<u8, u8, u8, u8, u8, u8>> testCases = {
        {5, 3, 0x2E, 0x2F, 0x1, 0x01},  // Vy > Vx
        {3, 2, 0x11, 0x10, 0x0, 0xFF},  // Vx < Vy
        {4, 4, 0x1F, 0x1F, 0x0, 0x00}   // Vy = Vx
    };

    for (const auto& [regX, regY, vx, vy, expectedVf, expectedResult] : testCases)
    {
        // Reset the emulator state
        emulator = chip8::Chip8();

        // Set the value in register Vx and Vy
        emulator.registers[regX] = vx;
        emulator.registers[regY] = vy;

        // Set the opcode for the instruction (using the test register indices)
        emulator.opcode = 0x8007 | (regX << 8) | (regY << 4);

        // Execute the instruction
        emulator.OP_8xy7();

        // Verify that the value in register Vx is now equal to Vy - Vx
        ASSERT_EQ(emulator.registers[regX], expectedResult)
            << "Failed for Vx index: " << regX << ", Vy index: " << regY << ", Vx value: 0x" << std::hex
            << static_cast<int>(vx) << ", Vy value: 0x" << static_cast<int>(vy);

        // Verify that the VF register is set correctly
        ASSERT_EQ(emulator.registers[0xF], expectedVf)
            << "VF flag incorrect for Vx index: " << regX << ", Vy index: " << regY << ", Vx value: 0x" << std::hex
            << static_cast<int>(vx) << ", Vy value: 0x" << static_cast<int>(vy);
    }
}

TEST(InstructionTest, Test_OP_8xyE)
{
    chip8::Chip8 emulator;

    // Set up test cases for register values, including cases where the MSB is 1 and 0
    std::vector<std::tuple<u8, u8, u8>> testCases = {
        {5, 0xA2, 0x1},  // MSB = 1
        {3, 0x11, 0x0}   // MSB = 0
    };

    for (const auto& [regX, vx, expectedMsb] : testCases)
    {
        // Reset the emulator state
        emulator = chip8::Chip8();

        // Set the value in register Vx
        emulator.registers[regX] = vx;

        // Set the opcode for the instruction (using the test register index)
        emulator.opcode = 0x800E | (regX << 8);

        // Execute the instruction
        emulator.OP_8xyE();

        // Verify that the value in register Vx is now equal to the original value shifted left by one
        ASSERT_EQ(emulator.registers[regX], (vx << 1) & 0xFF)
            << "Failed for Vx index: " << regX << ", Vx value: 0x" << std::hex << static_cast<int>(vx);

        // Verify that the MSB flag (register V[0xF]) is set correctly
        ASSERT_EQ(emulator.registers[0xF], expectedMsb)
            << "MSB flag incorrect for Vx index: " << regX << ", Vx value: 0x" << std::hex << static_cast<int>(vx);
    }
}

TEST(InstructionTest, Test_OP_9xy0)
{
    chip8::Chip8 emulator;

    // Set up test cases for register values, including cases where Vx is equal to Vy and Vx is not equal to Vy
    std::vector<std::tuple<u8, u8, u8, u8, u16>> testCases = {
        {4, 3, 0xAA, 0xAA, 0},  // Vx == Vy
        {2, 5, 0x10, 0x20, 2}   // Vx != Vy
    };

    for (const auto& [regX, regY, vx, vy, expectedPcIncrement] : testCases)
    {
        // Reset the emulator state
        emulator = chip8::Chip8();
        emulator.pc = chip8::Chip8::START_ADDRESS;  // Set PC to the start address

        // Set the value in registers Vx and Vy
        emulator.registers[regX] = vx;
        emulator.registers[regY] = vy;

        // Set the opcode for the instruction (using the test register indices)
        emulator.opcode = 0x9000 | (regX << 8) | (regY << 4);

        // Execute the instruction
        emulator.OP_9xy0();

        // Verify that the program counter has increased by the expected amount
        ASSERT_EQ(emulator.pc, chip8::Chip8::START_ADDRESS + expectedPcIncrement)
            << "Failed for Vx index: " << regX << ", Vy index: " << regY << ", Vx value: 0x" << std::hex
            << static_cast<int>(vx) << ", Vy value: 0x" << static_cast<int>(vy);
    }
}

TEST(InstructionTest, Test_OP_Annn)
{
    chip8::Chip8 emulator;

    // Set up test cases for various values of nnn
    std::vector<u16> testCases = {0x123, 0xFFF, 0x0};

    for (const auto& nnnValue : testCases)
    {
        // Reset the emulator state
        emulator = chip8::Chip8();

        // Set the opcode for the instruction (using the test nnn value)
        emulator.opcode = 0xA000 | nnnValue;

        // Execute the instruction
        emulator.OP_Annn();

        // Verify that the index register is now equal to the nnn value
        ASSERT_EQ(emulator.index, nnnValue) << "Failed for nnn value: 0x" << std::hex << nnnValue;
    }
}

TEST(InstructionTest, Test_OP_Bnnn)
{
    chip8::Chip8 emulator;

    // Set up test cases for various values of nnn and V0
    std::vector<std::tuple<u16, u8, u16>> testCases = {{0x123, 0x10, 0x133}, {0xFFF, 0x01, 0x1000}, {0x0, 0x0, 0x0}};

    for (const auto& [nnnValue, v0Value, expectedPcValue] : testCases)
    {
        // Reset the emulator state
        emulator = chip8::Chip8();

        // Set the value in register V0
        emulator.registers[0] = v0Value;

        // Set the opcode for the instruction (using the test nnn value)
        emulator.opcode = 0xB000 | nnnValue;

        // Execute the instruction
        emulator.OP_Bnnn();

        // Verify that the program counter is now equal to the nnn value plus the value in register V0
        ASSERT_EQ(emulator.pc, expectedPcValue)
            << "Failed for nnn value: 0x" << std::hex << nnnValue << ", V0 value: 0x" << static_cast<int>(v0Value);
    }
}

TEST(InstructionTest, Test_OP_Cxkk)
{
    chip8::Chip8 emulator;

    // Set up test cases for various values of x and kk
    std::vector<std::tuple<u8, u8>> testCases = {{4, 0xAA}, {2, 0xFF}, {5, 0x00}};

    for (const auto& [regX, kk] : testCases)
    {
        // Reset the emulator state
        emulator = chip8::Chip8();

        // Set the opcode for the instruction (using the test x and kk values)
        emulator.opcode = 0xC000 | (regX << 8) | kk;

        // Execute the instruction
        emulator.OP_Cxkk();

        // Verify that the result in Vx is within the possible range (i.e., 0 to kk)
        ASSERT_GE(emulator.registers[regX], 0)
            << "Failed for Vx index: " << regX << ", kk value: 0x" << std::hex << static_cast<int>(kk);
        ASSERT_LE(emulator.registers[regX], kk)
            << "Failed for Vx index: " << regX << ", kk value: 0x" << std::hex << static_cast<int>(kk);
    }
}

TEST(InstructionTest, Test_OP_Dxyn)
{
    // Assuming OP_Dxyn displays a sprite at (Vx, Vy) coordinates
    chip8::Chip8 emulator;

    // Setup coordinates
    u8 x = 5;
    u8 y = 3;
    emulator.registers[0] = x;  // V0
    emulator.registers[1] = y;  // V1

    // Setup sprite (2 rows, 8 bits per row)
    emulator.memory[emulator.index] = 0b10101010;
    emulator.memory[emulator.index + 1] = 0b01010101;

    // Opcode for drawing a sprite at (V0, V1) of 2 bytes height
    emulator.opcode = 0xD012;

    // Execute instruction
    emulator.OP_Dxyn();

    // Validate each pixel
    for (size_t row = 0; row < 2; row++)
    {
        u8 spriteByte = emulator.memory[emulator.index + row];
        for (size_t col = 0; col < 8; col++)
        {
            u8 expectedValue = (spriteByte >> (7 - col)) & 1u;
            u32 screenPixelValue = emulator.video[(y + row) * VIDEO_WIDTH + (x + col)];
            u32 expectedPixelValue = expectedValue ? UINT32_MAX : 0;  // assuming 1 is represented as UINT32_MAX

            ASSERT_EQ(screenPixelValue, expectedPixelValue);
        }
    }
}

TEST(InstructionTest, Test_OP_Ex9E)
{
    // Assuming that OP_Ex9E skips the next instruction if the key corresponding
    // to the value in register Vx is pressed
    chip8::Chip8 emulator;

    // Test case 1: Key is pressed
    {
        emulator.registers[5] = 0xA;  // Set V5 register to represent key 'A'
        emulator.keypad[0xA] = 1;     // Simulate that key 'A' is pressed
        emulator.opcode = 0xE5A1;     // Set opcode for OP_ExA1 (0xA1 == 0x9E) for Vx = 5
        u16 initialPc = emulator.pc;
        emulator.OP_Ex9E();
        ASSERT_EQ(emulator.pc, initialPc + 2);  // PC should increase by 4 (skip next instruction)
    }

    // Test case 2: Key is not pressed
    {
        emulator.registers[6] = 0xB;  // Set V6 register to represent key 'B'
        emulator.keypad[0xB] = 0;     // Simulate that key 'B' is not pressed
        emulator.opcode = 0xE6A1;     // Set opcode for OP_ExA1 (0xA1 == 0x9E) for Vx = 6
        u16 initialPc = emulator.pc;
        emulator.OP_Ex9E();
        ASSERT_EQ(emulator.pc, initialPc);  // PC should increase by 2 (do not skip next instruction)
    }
}

TEST(InstructionTest, Test_OP_ExA1)
{
    // Assuming that OP_Ex9E skips the next instruction if the key corresponding
    // to the value in register Vx is pressed
    chip8::Chip8 emulator;

    // Test case 1: Key is pressed
    {
        emulator.registers[5] = 0xA;  // Set V5 register to represent key 'A'
        emulator.keypad[0xA] = 1;     // Simulate that key 'A' is pressed
        emulator.opcode = 0xE5A1;     // Set opcode for OP_ExA1 (0xA1 == 0x9E) for Vx = 5
        u16 initialPc = emulator.pc;
        emulator.OP_ExA1();
        ASSERT_EQ(emulator.pc, initialPc);  // PC should increase by 4 (skip next instruction)
    }

    // Test case 2: Key is not pressed
    {
        emulator.registers[6] = 0xB;  // Set V6 register to represent key 'B'
        emulator.keypad[0xB] = 0;     // Simulate that key 'B' is not pressed
        emulator.opcode = 0xE6A1;     // Set opcode for OP_ExA1 (0xA1 == 0x9E) for Vx = 6
        u16 initialPc = emulator.pc;
        emulator.OP_ExA1();
        ASSERT_EQ(emulator.pc, initialPc + 2);  // PC should increase by 2 (do not skip next instruction)
    }
}

TEST(InstructionTest, Test_OP_Fx07)
{
    // Assuming OP_Fx07 sets the value of Vx to the value of the delay timer
    chip8::Chip8 emulator;

    // Test case: Set the delay timer to a known value and verify that Vx receives that value
    emulator.delayTimer = 123;  // Set the delay timer to a known value
    emulator.opcode = 0xF507;   // Set opcode for OP_Fx07 (0x07) for Vx = 5
    emulator.OP_Fx07();
    ASSERT_EQ(emulator.registers[5], 123);  // Expect V5 register to be set to the value of the delay timer
}

TEST(InstructionTest, Test_OP_Fx0A)
{
    // Assuming OP_Fx0A waits for a key press and then stores the value of the key in register Vx
    // Assuming that the pc is decremented if no key is pressed
    chip8::Chip8 emulator;

    // Test case 1: Simulate that no key is pressed
    emulator.opcode = 0xF60A;  // Set opcode for OP_Fx0A (0x0A) for Vx = 6
    u16 initialPc = emulator.pc;
    emulator.OP_Fx0A();

    // Verify that the pc is decremented by 2 when no key is pressed
    ASSERT_EQ(emulator.pc, initialPc - 2);

    // Test case 2: Simulate that key 3 is pressed
    emulator.pc = initialPc;  // Reset pc to the initial value
    emulator.keypad[3] = 1;   // Simulate that the key with value 3 is pressed
    emulator.OP_Fx0A();

    // Verify that the value stored in V6 is equal to the pressed key
    ASSERT_EQ(emulator.registers[6], 3);

    // Verify that the pc is not decremented when a key is pressed
    ASSERT_EQ(emulator.pc, initialPc);
}

TEST(InstructionTest, Test_OP_Fx15)
{
    // Assuming OP_Fx15 sets the delay timer to the value of register Vx
    chip8::Chip8 emulator;

    // Test case: Set register V3 to 42
    emulator.registers[3] = 42;
    emulator.opcode = 0xF315;  // Set opcode for OP_Fx15 (0x15) for Vx = 3

    // Execute the OP_Fx15 opcode
    emulator.OP_Fx15();

    // Verify that the delay timer has been set to the value in V3 (42)
    ASSERT_EQ(emulator.delayTimer, 42);
}

TEST(InstructionTest, Test_OP_Fx18)
{
    // Assuming OP_Fx15 sets the delay timer to the value of register Vx
    chip8::Chip8 emulator;

    // Test case: Set register V3 to 42
    emulator.registers[3] = 42;
    emulator.opcode = 0xF318;  // Set opcode for OP_Fx15 (0x15) for Vx = 3

    // Execute the OP_Fx15 opcode
    emulator.OP_Fx18();

    // Verify that the delay timer has been set to the value in V3 (42)
    ASSERT_EQ(emulator.soundTimer, 42);
}

TEST(InstructionTest, Test_OP_Fx1E)
{
    // Assuming OP_Fx1E adds the value of register Vx to the index
    chip8::Chip8 emulator;

    // Test case 1: V5 = 10, index = 100
    emulator.registers[5] = 10;
    emulator.index = 100;
    emulator.opcode = 0xF51E;  // Set opcode for OP_Fx1E (0x1E) for Vx = 5

    // Execute the OP_Fx1E opcode
    emulator.OP_Fx1E();

    // Verify that the index has been set to 110 (100 + 10)
    ASSERT_EQ(emulator.index, 110);

    // Test case 2: V7 = 250, index = 10
    emulator.registers[7] = 250;
    emulator.index = 10;
    emulator.opcode = 0xF71E;  // Set opcode for OP_Fx1E (0x1E) for Vx = 7

    // Execute the OP_Fx1E opcode
    emulator.OP_Fx1E();

    // Verify that the index has been set to 260 (10 + 250)
    ASSERT_EQ(emulator.index, 260);
}

TEST(InstructionTest, Test_OP_Fx29)
{
    chip8::Chip8 emulator;

    // Test with V4 = 3
    emulator.registers[4] = 3;
    emulator.opcode = 0xF429;  // Opcode for OP_Fx29 with Vx = 4
    emulator.OP_Fx29();
    ASSERT_EQ(emulator.index, chip8::Chip8::FONTSET_START_ADDRESS + (5 * 3));

    // Test with V6 = 6
    emulator.registers[6] = 6;
    emulator.opcode = 0xF629;  // Opcode for OP_Fx29 with Vx = 6
    emulator.OP_Fx29();
    ASSERT_EQ(emulator.index, chip8::Chip8::FONTSET_START_ADDRESS + (5 * 6));
}

TEST(InstructionTest, Test_OP_Fx33)
{
    chip8::Chip8 emulator;

    // Assuming a register value to be converted to BCD
    u8 vx_index = 5;
    u8 value = 123;  // For testing purpose, let's use a known value
    emulator.registers[vx_index] = value;

    // Set the opcode for the method under test
    emulator.opcode = 0xF533;  // Using register V5 for this test

    // Set the index to a known location
    emulator.index = 0x300;

    // Execute the instruction
    emulator.OP_Fx33();

    // Check the result
    ASSERT_EQ(emulator.memory[emulator.index], 1);      // Hundreds place
    ASSERT_EQ(emulator.memory[emulator.index + 1], 2);  // Tens place
    ASSERT_EQ(emulator.memory[emulator.index + 2], 3);  // Ones place
}

TEST(InstructionTest, Test_OP_Fx55)
{
    chip8::Chip8 emulator;

    // Assuming registers V0 to VX are to be stored in memory
    u8 vx_index = 5;
    for (u8 i = 0; i <= vx_index; i++)
    {
        emulator.registers[i] = i + 10;  // Assigning some known values
    }

    // Set the opcode for the method under test
    emulator.opcode = 0xF555;  // Using register V5 for this test

    // Set the index to a known location
    emulator.index = 0x300;

    // Execute the instruction
    emulator.OP_Fx55();

    // Check the result
    for (u8 i = 0; i <= vx_index; i++)
    {
        ASSERT_EQ(emulator.memory[emulator.index + i], i + 10);
    }

    // Optionally, you can also check if the index register remains unchanged
    ASSERT_EQ(emulator.index, 0x300);
}

TEST(InstructionTest, Test_OP_Fx65)
{
    chip8::Chip8 emulator;

    // Assuming registers V0 to VX are to be filled from memory
    u8 vx_index = 5;

    // Set the opcode for the method under test
    emulator.opcode = 0xF565;  // Using register V5 for this test

    // Set the index to a known location
    emulator.index = 0x300;

    // Fill memory with known values at the location pointed to by the index register
    for (u8 i = 0; i <= vx_index; i++)
    {
        emulator.memory[emulator.index + i] = i + 20;
    }

    // Execute the instruction
    emulator.OP_Fx65();

    // Check the result
    for (u8 i = 0; i <= vx_index; i++)
    {
        ASSERT_EQ(emulator.registers[i], i + 20);
    }
}
