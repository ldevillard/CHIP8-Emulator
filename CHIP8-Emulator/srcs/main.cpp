#include "Chip8.h"
#include "Window.h"

int main()
{
	Window window("CHIP8-Emulator", 1280, 720, 64, 32);

	Chip8 chip8;
	if (!chip8.LoadROM("roms/test_opcode.ch8"))
	{
		return -1;
	}

	bool running = true;
	while (running) 
	{
		running = window.ProcessInput(chip8.GetKeypad());
		
		// Rendering
		window.Update(nullptr, 0);
	}
}