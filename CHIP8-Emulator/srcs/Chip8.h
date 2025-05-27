#pragma once

#include <cstdint>
#include <string>

class Chip8 
{

public:
	Chip8();

	bool LoadROM(const std::string& filename);

public:
	static constexpr unsigned int START_ADDRESS = 0x200;

	static constexpr unsigned int KEY_COUNT = 16;
	static constexpr unsigned int MEMORY_SIZE = 4096;
	static constexpr unsigned int REGISTER_COUNT = 16;
	static constexpr unsigned int STACK_LEVELS = 16;
	static constexpr unsigned int VIDEO_HEIGHT = 32;
	static constexpr unsigned int VIDEO_WIDTH = 64;

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
};