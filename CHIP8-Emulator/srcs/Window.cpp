#include "Window.h"

#include <filesystem>
#include <iostream>
#include <vector>

#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_sdl3.h>
#include <utils/ImGui_Utils.h>

const std::string Window::INVALID_ROM = "Invalid ROM";

Window::Window(const std::string& title, int width, int height, int textureWidth, int textureHeight)
    : window(nullptr), glContext(nullptr), texture(0)
{
    this->textureWidth = textureWidth;
    this->textureHeight = textureHeight;

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

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
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // Setup ImGui style
    ImGui_Utils::SetPurpleTheme();

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

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);

    // TODO: Maybe move this to a separate method
    // Check if the ROMs folder exists and is a directory
    if (!std::filesystem::exists(ROMSFolder) || !std::filesystem::is_directory(ROMSFolder))
    {
        std::cerr << "ROMs folder does not exist: " << ROMSFolder << std::endl;
        exit(1);
    }

    // Populate the ROMS vector with .ch8 files
    for (const auto& entry : std::filesystem::directory_iterator(ROMSFolder))
    {
        if (entry.is_regular_file() && entry.path().extension() == ".ch8")
        {
            ROMS.push_back(entry.path().string().c_str());
        }
    }

    // If no ROMs were found, print a message
    if (ROMS.empty())
    {
        std::cerr << "No ROMs found in " << ROMSFolder << std::endl;
        exit(1);
    }

    InitAudio();
}

void Window::InitAudio()
{
    audioGen = std::mt19937(std::random_device{}());
    audioFloatDist = std::uniform_real_distribution<float>(0.95f, 1.05f);

    SDL_AudioSpec audioSpec = {};
    audioSpec.freq = SAMPLE_RATE;
    audioSpec.format = SDL_AUDIO_F32;
    audioSpec.channels = 1; // Mono

    if (!(audioStream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &audioSpec, nullptr, nullptr)))
    {
        std::cerr << "Audio error: " << SDL_GetError() << std::endl;
        exit(1);
    }

    SDL_ResumeAudioStreamDevice(audioStream);
}

Window::~Window()
{
    // Cleanup ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    // Cleanup SDL/OpenGL
    SDL_DestroyAudioStream(audioStream);
    SDL_GL_DestroyContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

// TODO: Split interfaces in different methods
void Window::Update(const void* buffer)
{
    glViewport(0, 0, 1920, 1080);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Update texture with new pixel data
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, textureWidth, textureHeight, GL_RGBA, GL_UNSIGNED_BYTE, buffer);

    // Start ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

	SetupDockingSpace();

	DisplayEditor();

    // Render ImGui
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    SDL_GL_SwapWindow(window);
}

void Window::SetupDockingSpace()
{
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImVec2 pos(viewport->Pos.x, viewport->Pos.y);
    ImVec2 size(viewport->Size.x, viewport->Size.y);
    ImGui::SetNextWindowPos(pos);
    ImGui::SetNextWindowSize(size);
    ImGui::Begin("DockSpace", nullptr,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
        ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDocking);
    ImGuiID dockspace_id = ImGui::GetID("DockSpace");
    ImGui::DockSpace(dockspace_id);
    ImGui::End();
}

void Window::DisplayEditor() 
{
    // Display the texture inside ImGui window
    {
        ImGui::Begin("CHIP8-Emulator", nullptr, ImGuiWindowFlags_NoResize);

        ImVec2 available_size = ImGui::GetContentRegionAvail();
        ImGui::Image((ImTextureID)(intptr_t)texture, available_size, ImVec2(0, 0), ImVec2(1, 1));

        ImGui::End();
    }

    // Footer
    {
        ImGui::Begin("Info", nullptr, ImGuiWindowFlags_NoResize);
        ImGui::Text("Press ESC to exit");
        ImGui::End();
    }

    // Debug Menu
    {
        ImGui::Begin("Debug Menu", nullptr, ImGuiWindowFlags_NoResize);

        ImGui_Utils::DrawIntControl("Cycles", config.emulationCycles, 5, 125);

        std::vector<const char*> cROMS;
        cROMS.reserve(ROMS.size());
        std::transform(ROMS.begin(), ROMS.end(), std::back_inserter(cROMS),
            [](const std::string& s) { return s.c_str(); });
        ImGui_Utils::DrawComboBoxControl("Loaded ROM", ROMIndexRequested, cROMS, 125);

        if (ImGui_Utils::DrawButtonControl("Reset ROM", "RESET", 125)) 
        {
			ROMIndexRequested = currentROMIndex;
			// HACK to force the ROM to reload
			currentROMIndex = -1; 
        }

        ImGui::End();
    }
}

bool Window::ProcessInput(uint8_t* keys)
{
    bool running = true;

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        // Send and handle CHIP-8 input events
        if (event.type == SDL_EVENT_QUIT)
        {
            running = false;
        }
        else if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP)
        {
            if (event.key.key == SDLK_ESCAPE)
            {
                running = false;
            }

            auto it = keymap.find(event.key.key);
            if (it != keymap.end())
            {
                keys[it->second] = (event.type == SDL_EVENT_KEY_DOWN) ? 1 : 0;
            }
        }

        // Send events to ImGui
        ImGui_ImplSDL3_ProcessEvent(&event);
    }

    return running;
}

void Window::PlaySound()
{
    std::vector<float> beepSamples(SAMPLE_COUNT);

    // Add random pitch factor to the frequency
    float pitchFactor = audioFloatDist(audioGen);
    float pitchFrequency = FREQUENCY * pitchFactor;

    for (int i = 0; i < SAMPLE_COUNT; ++i)
    {
        float t = static_cast<float>(i) / SAMPLE_RATE;

        // Generate a square wave
        float wave = fmod(pitchFrequency * t, 1.0f) < 0.5f ? 0.25f : -0.25f;
        beepSamples.push_back(wave);
    }

    if (!SDL_GetAudioStreamAvailable(audioStream))
    {
        SDL_PutAudioStreamData(audioStream, beepSamples.data(), beepSamples.size() * sizeof(float));
    }
}

const std::string& Window::GetFirstFoundROM() const
{
    if (ROMS.empty())
    {
        return INVALID_ROM;
    }
    else
    {
        return ROMS.front();
    }
}