#include "Window.h"

#include <iostream>
#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_sdl3.h>
#include <vector>

Window::Window(const std::string& title, int width, int height, int textureWidth, int textureHeight)
	: window(nullptr), glContext(nullptr), texture(0)
{
    SDL_Init(SDL_INIT_VIDEO);

    // Setup OpenGL context attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);

    // Create window with OpenGL support
    window = SDL_CreateWindow(title.c_str(), width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    // Create OpenGL context
    glContext = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, glContext);
    SDL_GL_SetSwapInterval(1); // vsync

    // Load OpenGL functions with glad
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) 
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        exit(1);
    }

    // Setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup ImGui style
    ImGui::StyleColorsDark();

    // Init SDL + OpenGL3 backends
    ImGui_ImplSDL3_InitForOpenGL(window, glContext);
    ImGui_ImplOpenGL3_Init("#version 460");

	// Setup render texture 
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Create a default checkerboard texture (this will be remove in the future)
    std::vector<GLubyte> pixels(textureWidth * textureHeight * 4, 255);
    {
        for (int j = 0; j < textureHeight; ++j)
        {
            for (int i = 0; i < textureWidth; ++i)
            {
                bool grid = (i + j) % 2 == 0;

                int index = (j * textureWidth + i) * 4;

                if (grid) 
                {
                    pixels[index + 0] = 255; // R
                    pixels[index + 1] = 255; // G
                    pixels[index + 2] = 255; // B
                    pixels[index + 3] = 255; // A
                }
                else 
                {
                    pixels[index + 0] = 0;   // R
                    pixels[index + 1] = 0;   // G
                    pixels[index + 2] = 0;   // B
                    pixels[index + 3] = 255; // A
                }
            }
        }
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
    glBindTexture(GL_TEXTURE_2D, 0);
}

Window::~Window()
{
    // Cleanup ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

	// Cleanup SDL/OpenGL
    SDL_GL_DestroyContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Window::Update(const void* buffer, int pitch)
{
    glViewport(0, 0, 1920, 1080);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Start ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

	// Display the texture inside ImGui window
    {
        ImGui::Begin("Render");
        
        ImVec2 available_size = ImGui::GetContentRegionAvail();
        ImGui::Image((ImTextureID)(intptr_t)texture, available_size, ImVec2(0, 0), ImVec2(1, 1));

        ImGui::End();
    }

    // Render ImGui
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    SDL_GL_SwapWindow(window);
}

bool Window::ProcessInput(SDL_Event* event, uint8_t* keys)
{
    ImGui_ImplSDL3_ProcessEvent(event);

	// TODO: Handle all CHIP-8 input events here
	return true;
}