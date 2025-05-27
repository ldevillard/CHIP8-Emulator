#include "Window.h"

int main()
{
	Window window("CHIP8-Emulator", 1920, 1080, 1920, 1080);

	bool running = true;
	SDL_Event event;
	while (running) 
	{
		while (SDL_PollEvent(&event)) 
		{
			// Send and handle CHIP-8 input events
			running = window.ProcessInput(&event, nullptr);
		
			if (event.type == SDL_EVENT_QUIT) 
			{
				running = false;
			}
		}
		
		// Rendering
		window.Update(nullptr, 0);
	}
}