cmake_minimum_required(VERSION 4.0.1)
project(CHIP8-Emulator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

# SDL3
set(SDL_TEST OFF CACHE BOOL "" FORCE)
set(SDL_INSTALL OFF CACHE BOOL "" FORCE)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)

FetchContent_Declare(
  SDL3
  GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
  GIT_TAG release-3.2.14
)
FetchContent_MakeAvailable(SDL3)

# ImGui
FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG v1.91.8
)
FetchContent_MakeAvailable(imgui)

# ImGui sources
file(GLOB IMGUI_SOURCES
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlgpu3.cpp
)

source_group(TREE ${imgui_SOURCE_DIR} PREFIX "ImGui" FILES ${IMGUI_SOURCES})

# Sources
file(GLOB_RECURSE SOURCES "CHIP8-Emulator/srcs/*.cpp" "CHIP8-Emulator/srcs/*.c" "CHIP8-Emulator/includes/*.h")

# Headers
file(GLOB_RECURSE INCLUDES)

# Executable
add_executable(CHIP8-Emulator ${SOURCES} ${IMGUI_SOURCES})

# Include headers
target_include_directories(CHIP8-Emulator PRIVATE
    ${PROJECT_SOURCE_DIR}/CHIP8-Emulator/includes
    ${SDL3_SOURCE_DIR}/include
    ${imgui_SOURCE_DIR}
)

# Link libraries
target_link_libraries(CHIP8-Emulator PRIVATE SDL3::SDL3-static)