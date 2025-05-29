#pragma once

#include <string>
#include <unordered_map>

#include <SDL3/SDL.h>
#include <utils/glad.h>

class Window 
{
public:
	Window(const std::string& title, int width, int height, int textureWidth, int textureHeight);
	~Window();

	void Update(const void* buffer, int pitch);
	bool ProcessInput(uint8_t* keys);

private:
	SDL_Window* window;
	SDL_GLContext glContext;

	GLuint texture;

    const std::unordered_map<SDL_Keycode, uint8_t> keymap = 
    {
        {SDLK_X, 0x0},
        {SDLK_1, 0x1},
        {SDLK_2, 0x2},
        {SDLK_3, 0x3},
        {SDLK_Q, 0x4},
        {SDLK_W, 0x5},
        {SDLK_E, 0x6},
        {SDLK_A, 0x7},
        {SDLK_S, 0x8},
        {SDLK_D, 0x9},
        {SDLK_Z, 0xA},
        {SDLK_C, 0xB},
        {SDLK_4, 0xC},
        {SDLK_R, 0xD},
        {SDLK_F, 0xE},
        {SDLK_V, 0xF}
    };
};