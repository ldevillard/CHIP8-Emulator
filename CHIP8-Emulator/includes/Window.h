#pragma once

#include <string>

#include <SDL3/SDL.h>
#include <utils/glad.h>

class Window 
{
public:
	Window(const std::string& title, int width, int height, int textureWidth, int textureHeight);
	~Window();

	void Update(const void* buffer, int pitch);
	bool ProcessInput(SDL_Event* event, uint8_t* keys);

private:
	SDL_Window* window;
	SDL_GLContext glContext;

	GLuint texture;
};