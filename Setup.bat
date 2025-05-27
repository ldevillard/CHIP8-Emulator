@echo off

cmake -B ./ -S . -G "Visual Studio 17 2022"

del /F /Q ALL_Build.vcxproj
del /F /Q ALL_Build.vcxproj.filters
del /F /Q cmake_install.cmake
del /F /Q CMakeCache.txt
del /F /Q ZERO_CHECK.vcxproj
del /F /Q ZERO_CHECK.vcxproj.filters

dotnet sln CHIP8-Emulator.sln remove ALL_BUILD.vcxproj
dotnet sln CHIP8-Emulator.sln remove ZERO_CHECK.vcxproj