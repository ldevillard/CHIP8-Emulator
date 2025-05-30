#include "Chip8.h"

#include <chrono>
#include <fstream>
#include <iostream>
#include <vector>

Chip8::Chip8()
	: opcode(0), index(0), pc(START_ADDRESS), sp(0), delayTimer(0), soundTimer(0), 
        randGen(std::chrono::system_clock::now().time_since_epoch().count())
{
    // Load fonts into memory
    for (unsigned int i = 0; i < FONTSET_SIZE; ++i)
    {
        memory[FONTSET_START_ADDRESS + i] = fontset[i];
    }

    // Initialize RNG
    randByte = std::uniform_int_distribution<int>(0U, 255U);

	// Initialize opcode table
	table[0x0] = &Chip8::Table0;
	table[0x1] = &Chip8::OP_1nnn;
	table[0x2] = &Chip8::OP_2nnn;
	table[0x3] = &Chip8::OP_3xnn;
	table[0x4] = &Chip8::OP_4xnn;
	table[0x5] = &Chip8::OP_5xy0;
	table[0x6] = &Chip8::OP_6xnn;
	table[0x7] = &Chip8::OP_7xnn;
	// 0x8xy0 to 0x8xyE
	table[0x8] = &Chip8::Table8; 
	table[0x9] = &Chip8::OP_9xy0;
	table[0xA] = &Chip8::OP_Annn;
	table[0xB] = &Chip8::OP_Bnnn;
	table[0xC] = &Chip8::OP_Cxnn;
	table[0xD] = &Chip8::OP_Dxyn;
	// 0Ex9E and 0ExA1
	table[0xE] = &Chip8::TableE; 
	// 0Fx07 to 0Fx65
	table[0xF] = &Chip8::TableF; 

	// Initialize Table0, Table8, TableE with OP_NULL
	for (int i = 0; i <= 0xE; ++i) 
	{
		table0[i] = &Chip8::OP_NULL;
		table8[i] = &Chip8::OP_NULL;
		tableE[i] = &Chip8::OP_NULL;
	}

	// Initialize Table0
	table0[0x0] = &Chip8::OP_00E0;
	table0[0xE] = &Chip8::OP_00EE;

	// Initialize Table8
	table8[0x0] = &Chip8::OP_8xy0;
	table8[0x1] = &Chip8::OP_8xy1;
	table8[0x2] = &Chip8::OP_8xy2;
	table8[0x3] = &Chip8::OP_8xy3;
	table8[0x4] = &Chip8::OP_8xy4;
	table8[0x5] = &Chip8::OP_8xy5;
	table8[0x6] = &Chip8::OP_8xy6;
	table8[0x7] = &Chip8::OP_8xy7;
	table8[0xE] = &Chip8::OP_8xyE;

	// Initialize TableE
	tableE[0x1] = &Chip8::OP_ExA1;
	tableE[0xE] = &Chip8::OP_Ex9E;

	// Initialize TableF
	for (int i = 0; i <= 0x65; ++i)
	{
		tableF[i] = &Chip8::OP_NULL;
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

bool Chip8::LoadROM(const std::string& filename)
{
	ResetHardware();

    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (file)
    {
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        std::vector<char> buffer(size);
        if (file.read(buffer.data(), size))
        {
			// Load the ROM into memory starting at address 0x200
            for (std::streamsize i = 0; i < size; ++i)
            {
                // Prevent overflow
				if (START_ADDRESS + i >= MEMORY_SIZE) 
                {
					std::cerr << "ROM size exceeds memory limit." << std::endl;
                    return false;
				}
                memory[START_ADDRESS + i] = static_cast<uint8_t>(buffer[i]);
            }
        }
        else
        {
			std::cerr << "Failed to read ROM data from file: " << filename << std::endl;
            return false;
        }
    }
    else 
    {
        // Failed to open the file
		std::cerr << "Failed to load ROM: " << filename << std::endl;
        return false; 
    }
    
	return true;
}

void Chip8::Cycle()
{
	// Fetch the next opcode
	opcode = (memory[pc] << 8) | memory[pc + 1];

	// Increment the program counter
	pc += 2;

	// Decode and execute the opcode
	((*this).*(table[(opcode & 0xF000) >> 12]))();

	// Update timers
	if (delayTimer > 0)
	{
		--delayTimer;
	}
	
	// TODO: Check if the sound is working
	// Update sound timer
	if (soundTimer)
	{
		--soundTimer;
	}
}

void Chip8::ResetHardware()
{
	// Reset registers
	memset(registers, 0, sizeof(registers));

	// Reset memory and reload fonts
	memset(memory, 0, sizeof(memory));
	for (unsigned int i = 0; i < FONTSET_SIZE; ++i)
	{
		memory[FONTSET_START_ADDRESS + i] = fontset[i];
	}

	// Reset index register and program counter
	index = 0;
	pc = START_ADDRESS;
	// Reset stack pointer and stack
	sp = 0;
	memset(stack, 0, sizeof(stack));
	// Reset timers
	delayTimer = 0;
	soundTimer = 0;
	// Clear video memory
	memset(video, 0, sizeof(video));

	// Clear keypad state
	memset(keypad, 0, sizeof(keypad));

	// Reinitialize RNG
	randByte = std::uniform_int_distribution<int>(0U, 255U);
}

#pragma region Opcode Tables
void Chip8::Table0()
{
	((*this).*(table0[opcode & 0x000F]))();
}

void Chip8::Table8()
{
	((*this).*(table8[opcode & 0x000F]))();
}

void Chip8::TableE()
{
	((*this).*(tableE[opcode & 0x000F]))();
}

void Chip8::TableF()
{
	((*this).*(tableF[opcode & 0x00FF]))();
}
#pragma endregion

#pragma region Operation Codes
void Chip8::OP_NULL()
{
	// This is a no-operation code, typically used for debugging or as a placeholder.
	// It does nothing and simply returns control to the main loop.
}

void Chip8::OP_00E0()
{
	memset(video, 0, sizeof(video));
}

void Chip8::OP_00EE()
{
    --sp;
	pc = stack[sp];
}

void Chip8::OP_1nnn()
{
	pc = opcode & 0x0FFF;
}

void Chip8::OP_2nnn()
{
	stack[sp] = pc;
	++sp;
	pc = opcode & 0x0FFF;
}

void Chip8::OP_3xnn()
{
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	uint8_t nn = opcode & 0x00FF;

	if (registers[Vx] == nn)
	{
        // Skip next instruction
		pc += 2; 
	}
}

void Chip8::OP_4xnn()
{
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	uint8_t nn = opcode & 0x00FF;

	if (registers[Vx] != nn)
	{
		// Skip next instruction
		pc += 2;
	}
}

void Chip8::OP_5xy0()
{
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	uint8_t Vy = (opcode & 0x00F0) >> 4;

	if (registers[Vx] == registers[Vy])
	{
		// Skip next instruction
		pc += 2;
	}
}

void Chip8::OP_6xnn()
{
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	uint8_t nn = opcode & 0x00FF;

	registers[Vx] = nn;
}

void Chip8::OP_7xnn()
{
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	uint8_t nn = opcode & 0x00FF;

	registers[Vx] += nn;
}

void Chip8::OP_8xy0()
{
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	uint8_t Vy = (opcode & 0x00F0) >> 4;

	registers[Vx] = registers[Vy];
}

void Chip8::OP_8xy1()
{
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	uint8_t Vy = (opcode & 0x00F0) >> 4;

	registers[Vx] |= registers[Vy];
}

void Chip8::OP_8xy2()
{
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	uint8_t Vy = (opcode & 0x00F0) >> 4;

	registers[Vx] &= registers[Vy];
}

void Chip8::OP_8xy3()
{
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	uint8_t Vy = (opcode & 0x00F0) >> 4;

	registers[Vx] ^= registers[Vy];
}

void Chip8::OP_8xy4()
{
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	uint8_t Vy = (opcode & 0x00F0) >> 4;

	uint16_t sum = registers[Vx] + registers[Vy];
	
	// Store the result in Vx
	registers[Vx] = sum & 0xFF;
	
	// Set carry flag
	registers[0xF] = (sum > 0xFF) ? 1 : 0;
}

void Chip8::OP_8xy5()
{
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	uint8_t Vy = (opcode & 0x00F0) >> 4;

	// Set carry flag
	registers[0xF] = (registers[Vx] > registers[Vy]) ? 1 : 0;
	
	// Subtract Vy from Vx
	registers[Vx] -= registers[Vy];
}

void Chip8::OP_8xy6()
{
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	
	// Set carry flag to the least significant bit of Vx
	registers[0xF] = registers[Vx] & 0x01;

	// Shift Vx right by 1
	registers[Vx] >>= 1;
}

void Chip8::OP_8xy7()
{
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	uint8_t Vy = (opcode & 0x00F0) >> 4;

	// Set carry flag
	registers[0xF] = (registers[Vy] > registers[Vx]) ? 1 : 0;

	// Subtract Vx from Vy
	registers[Vx] = registers[Vy] - registers[Vx];
}

void Chip8::OP_8xyE()
{
	uint8_t Vx = (opcode & 0x0F00) >> 8;

	// Set carry flag to the most significant bit of Vx
	registers[0xF] = (registers[Vx] & 0x80) >> 7;

	// Shift Vx left by 1
	registers[Vx] <<= 1;
}

void Chip8::OP_9xy0()
{
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	uint8_t Vy = (opcode & 0x00F0) >> 4;

	if (registers[Vx] != registers[Vy])
	{
		// Skip next instruction
		pc += 2;
	}
}

void Chip8::OP_Annn()
{
	index = opcode & 0x0FFF;
}

void Chip8::OP_Bnnn()
{
	pc = (opcode & 0x0FFF) + registers[0];
}

void Chip8::OP_Cxnn()
{
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	uint8_t nn = opcode & 0x00FF;

	// Generate a random byte and mask it with nn
	registers[Vx] = static_cast<uint8_t>(randByte(randGen)) & nn;
}

void Chip8::OP_Dxyn()
{
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	uint8_t Vy = (opcode & 0x00F0) >> 4;
	uint8_t n = opcode & 0x000F;

	// Draw a sprite at the position (Vx, Vy) with height n
	uint8_t x = registers[Vx] % VIDEO_WIDTH;
	uint8_t y = registers[Vy] % VIDEO_HEIGHT;

	registers[0xF] = 0; // Clear collision flag

	for (uint8_t row = 0; row < n; ++row)
	{
		uint8_t spriteByte = memory[index + row];

		for (uint8_t col = 0; col < 8; ++col)
		{
			uint8_t spritePixel = spriteByte & (0x80u >> col);
			uint32_t* screenPixel = &video[(y + row) * VIDEO_WIDTH + (x + col)];

			// Sprite pixel is on
			if (spritePixel)
			{
				// Screen pixel also on - collision
				if (*screenPixel == 0xFFFFFFFF)
				{
					registers[0xF] = 1;
				}

				// Effectively XOR with the sprite pixel
				*screenPixel ^= 0xFFFFFFFF;
			}
		}
	}
}

void Chip8::OP_Ex9E()
{
	uint8_t Vx = (opcode & 0x0F00) >> 8;

	// Check if the key corresponding to Vx is pressed
	if (keypad[registers[Vx]] != 0)
	{
		// Skip next instruction
		pc += 2;
	}
}
void Chip8::OP_ExA1()
{
	uint8_t Vx = (opcode & 0x0F00) >> 8;

	// Check if the key corresponding to Vx is not pressed
	if (keypad[registers[Vx]] == 0)
	{
		// Skip next instruction
		pc += 2;
	}
}

void Chip8::OP_Fx07()
{
	uint8_t Vx = (opcode & 0x0F00) >> 8;

	// Set Vx to the value of the delay timer
	registers[Vx] = delayTimer;
}

void Chip8::OP_Fx0A()
{
	uint8_t Vx = (opcode & 0x0F00) >> 8;

	// Wait for a key press and store the value in Vx
	bool keyPressed = false;
	for (int i = 0; i < KEY_COUNT; ++i)
	{
		if (keypad[i] != 0)
		{
			registers[Vx] = i;
			keyPressed = true;
			break;
		}
	}

	if (!keyPressed)
	{
		// Repeat this instruction until a key is pressed
		pc -= 2; 
	}
}

void Chip8::OP_Fx15()
{
	uint8_t Vx = (opcode & 0x0F00) >> 8;

	// Set the delay timer to the value of Vx
	delayTimer = registers[Vx];
}

void Chip8::OP_Fx18()
{
	uint8_t Vx = (opcode & 0x0F00) >> 8;

	// Set the sound timer to the value of Vx
	soundTimer = registers[Vx];
}

void Chip8::OP_Fx1E()
{
	uint8_t Vx = (opcode & 0x0F00) >> 8;

	// Add Vx to I
	index += registers[Vx];
}

void Chip8::OP_Fx29()
{
	uint8_t Vx = (opcode & 0x0F00) >> 8;

	// Set I to the address of the sprite for the character in Vx
	// A font sprite is 5 bytes tall
	index = FONTSET_START_ADDRESS + (registers[Vx] * 5);
}

void Chip8::OP_Fx33()
{
	uint8_t Vx = (opcode & 0x0F00) >> 8;

	// Store the binary-coded decimal representation of Vx in memory starting at I
	memory[index] = registers[Vx] / 100; // Hundreds
	memory[index + 1] = (registers[Vx] / 10) % 10; // Tens
	memory[index + 2] = registers[Vx] % 10; // Ones
}

void Chip8::OP_Fx55()
{
	uint8_t Vx = (opcode & 0x0F00) >> 8;

	// Store the values of V0 to Vx in memory starting at I
	for (uint8_t i = 0; i <= Vx; ++i)
	{
		memory[index + i] = registers[i];
	}
}

void Chip8::OP_Fx65()
{
	uint8_t Vx = (opcode & 0x0F00) >> 8;

	// Load the values from memory starting at I into V0 to Vx
	for (uint8_t i = 0; i <= Vx; ++i)
	{
		registers[i] = memory[index + i];
	}
}
#pragma endregion
