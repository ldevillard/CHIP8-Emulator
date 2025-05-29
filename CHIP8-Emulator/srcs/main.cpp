#include <chrono>

#include "Chip8.h"
#include "Window.h"

int main()
{
	std::unique_ptr <Window> window = std::make_unique<Window>("CHIP8-Emulator", 1280, 720, 64, 32);

	std::unique_ptr<Chip8> chip8 = std::make_unique<Chip8>();
	if (!chip8->LoadROM("roms/Pong.ch8"))
	{
		return -1;
	}

	std::chrono::steady_clock::time_point lastCycleTime = std::chrono::high_resolution_clock::now();
	bool running = true;
	
	while (running) 
	{
		running = window->ProcessInput(chip8->GetKeypad());
		
		std::chrono::steady_clock::time_point currentTime = std::chrono::high_resolution_clock::now();
		float deltaTime = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();

		// Limit the cycle time to approximately 60Hz (16.67ms per cycle)
		if (deltaTime > 16.67f)
		{
			lastCycleTime = currentTime;

			// Execute a single CHIP-8 cycle
			chip8->Cycle();

			// Rendering
			window->Update(chip8->GetVideo());
		}
	}
}