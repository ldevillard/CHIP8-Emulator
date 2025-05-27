#include "Chip8.h"

#include <fstream>
#include <iostream>
#include <vector>

Chip8::Chip8()
	: opcode(0), index(0), pc(START_ADDRESS), sp(0), delayTimer(0), soundTimer(0)
{

}

bool Chip8::LoadROM(const std::string& filename)
{
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
