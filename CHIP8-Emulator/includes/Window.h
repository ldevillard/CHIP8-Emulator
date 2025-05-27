#pragma once

#include <memory>
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
	void createProgram();
	GLuint compileShader(GLenum type, const char* src);

private:
	SDL_Window* window;
	SDL_GLContext glContext;

	GLuint texture;
	GLuint shaderProgram;

	// Hardcoded shaders for simplicity
	std::string vertexShaderSrc = 
	R"(
		#version 460 core

		layout(location = 0) in vec2 aPos;
		layout(location = 1) in vec2 aTexCoord;

		out vec2 TexCoord;

		void main()
		{
			gl_Position = vec4(aPos.xy, 0.0, 1.0);
		    TexCoord = aTexCoord;
		}
	)";

	std::string fragmentShaderSrc =
	R"(
		#version 460 core

		in vec2 TexCoord;
		out vec4 FragColor;

		uniform sampler2D texture1;

		void main()
		{
			FragColor = texture(texture1, TexCoord);
		}
	)";

	float vertices[16] = 
	{
		// Positions   // TexCoords
		-1.f,  1.f,    0.f, 1.f,  // top-left
		-1.f, -1.f,    0.f, 0.f,  // bottom-left
		 1.f, -1.f,    1.f, 0.f,  // bottom-right
		 1.f,  1.f,    1.f, 1.f   // top-right
	};

	unsigned int indices[6] = 
	{
		0, 1, 2,
		2, 3, 0
	};

	GLuint VAO;
	GLuint VBO;
	GLuint EBO;
};