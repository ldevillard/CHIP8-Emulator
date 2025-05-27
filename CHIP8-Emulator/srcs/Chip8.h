#pragma once

#include <cstdint>
#include <string>
#include <random>

class Chip8 
{

public:
	Chip8();

	bool LoadROM(const std::string& filename);

private:
	static constexpr unsigned int START_ADDRESS = 0x200;

	static constexpr unsigned int FONTSET_SIZE = 80;
	static constexpr unsigned int FONTSET_START_ADDRESS = 0x50;

	static constexpr unsigned int KEY_COUNT = 16;
	static constexpr unsigned int MEMORY_SIZE = 4096;
	static constexpr unsigned int REGISTER_COUNT = 16;
	static constexpr unsigned int STACK_LEVELS = 16;
	static constexpr unsigned int VIDEO_HEIGHT = 32;
	static constexpr unsigned int VIDEO_WIDTH = 64;


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
	uint8_t registers[REGISTER_COUNT]{};
	uint8_t memory[MEMORY_SIZE]{};
	uint16_t index{};
	uint16_t pc{};
	uint16_t stack[STACK_LEVELS]{};
	uint8_t sp{};
	uint8_t delayTimer{};
	uint8_t soundTimer{};
	uint8_t keypad[KEY_COUNT]{};
	uint32_t video[VIDEO_HEIGHT * VIDEO_WIDTH]{};
	uint16_t opcode;

	// Random number generator
	std::default_random_engine randGen;
	// uniform_int_distribution does not support uint8_t, so we use int (we'll need static_cast<uint8_t> when using it)
	std::uniform_int_distribution<int> randByte;
};