#pragma once

#include <cstdint>
#include <string>
#include <random>

class Chip8 
{

public:
	Chip8();

	bool LoadROM(const std::string& filename);
	void Cycle();

	uint8_t* GetKeypad() { return keypad; }

private:
#pragma region Opcode Tables
	void Table0();
	void Table8();
	void TableE();
	void TableF();
#pragma endregion

#pragma region Operation Codes
	// Do nothing, used for unimplemented or reserved opcodes
	void OP_NULL();

	// Clear the display
	void OP_00E0();
	// Return from a subroutine
	void OP_00EE();
	// Jump to a specific address
	void OP_1nnn();
	// Call a subroutine at a specific address
	void OP_2nnn();
	// Skip the next instruction if Vx equals nn
	void OP_3xnn();
	// Skip the next instruction if Vx does not equal nn
	void OP_4xnn();
	// Skip the next instruction if Vx equals Vy
	void OP_5xy0();
	// Set Vx to nn
	void OP_6xnn();
	// Add nn to Vx
	void OP_7xnn();
	// Set Vx to the value of Vy
	void OP_8xy0();
	// Set Vx to Vx OR Vy
	void OP_8xy1();
	// Set Vx to Vx AND Vy
	void OP_8xy2();
	// Set Vx to Vx XOR Vy
	void OP_8xy3();
	// Add Vy to Vx, set carry if overflow
	void OP_8xy4();
	// Subtract Vy from Vx, set carry if no overflow
	void OP_8xy5();
	// Set Vx to Vx minus Vy, set carry if no overflow
	void OP_8xy6();
	// Set Vx to Vx divided by 2, set carry if no overflow
	void OP_8xy7();
	// Set Vx to Vy minus Vx, set carry if no overflow
	void OP_8xyE();
	// Set Vx to the value of nn
	void OP_9xy0();
	// Set I to the address nnn
	void OP_Annn();
	// Jump to the address nnn plus V0
	void OP_Bnnn();
	// Set Vx to a random number AND nn
	void OP_Cxnn();
	// Draw a sprite at coordinates (Vx, Vy) with n bytes of sprite data
	void OP_Dxyn();
	// Skip the next instruction if the key corresponding to Vx is pressed
	void OP_Ex9E();
	// Skip the next instruction if the key corresponding to Vx is not pressed
	void OP_ExA1();
	// Set Vx to the value of the delay timer
	void OP_Fx07();
	// Wait for a key press and store the value in Vx
	void OP_Fx0A();
	// Set the delay timer to the value of Vx
	void OP_Fx15();
	// Set the sound timer to the value of Vx
	void OP_Fx18();
	// Add Vx to I
	void OP_Fx1E();
	// Set I to the address of the sprite for the character in Vx
	void OP_Fx29();
	// Store the binary-coded decimal representation of Vx in memory starting at I
	void OP_Fx33();
	// Store the values of V0 to Vx in memory starting at I
	void OP_Fx55();
	// Load the values from memory starting at I into V0 to Vx
	void OP_Fx65();
#pragma endregion

private:
	
#pragma region Static Variables
	static constexpr unsigned int START_ADDRESS = 0x200;

	static constexpr unsigned int FONTSET_SIZE = 80;
	static constexpr unsigned int FONTSET_START_ADDRESS = 0x50;

	static constexpr unsigned int KEY_COUNT = 16;
	static constexpr unsigned int MEMORY_SIZE = 4096;
	static constexpr unsigned int REGISTER_COUNT = 16;
	static constexpr unsigned int STACK_LEVELS = 16;
	static constexpr unsigned int VIDEO_HEIGHT = 32;
	static constexpr unsigned int VIDEO_WIDTH = 64;
#pragma endregion

	uint8_t fontset[FONTSET_SIZE] =
	{
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};


	// CHIP-8 hardware specifications
	uint8_t registers[REGISTER_COUNT] = {};
	uint8_t memory[MEMORY_SIZE] = {};
	uint16_t index;
	uint16_t pc;
	uint16_t stack[STACK_LEVELS] {};
	uint8_t sp;
	uint8_t delayTimer;
	uint8_t soundTimer;
	uint8_t keypad[KEY_COUNT] = {};
	uint32_t video[VIDEO_HEIGHT * VIDEO_WIDTH] = {};
	uint16_t opcode;

	// Random number generator
	std::default_random_engine randGen;
	// uniform_int_distribution does not support uint8_t, so we use int (we'll need static_cast<uint8_t> when using it)
	std::uniform_int_distribution<int> randByte;

	// Opcode tables
	using Chip8Func = void (Chip8::*)();

	Chip8Func table[0xF + 1];
	Chip8Func table0[0xE + 1];
	Chip8Func table8[0xE + 1];
	Chip8Func tableE[0xE + 1];
	Chip8Func tableF[0x65 + 1];
};