del /F /Q ALL_Build.vcxproj
del /F /Q ALL_Build.vcxproj.filters
del /F /Q CHIP8-Emulator.sln
del /F /Q CHIP8-Emulator.vcxproj
del /F /Q CHIP8-Emulator.vcxproj.filters
del /F /Q CHIP8-Emulator.vcxproj.user
del /F /Q cmake_install.cmake
del /F /Q CMakeCache.txt
del /F /Q ZERO_CHECK.vcxproj
del /F /Q ZERO_CHECK.vcxproj.filters
del /F /Q imgui.ini

rmdir /S /Q _deps
rmdir /S /Q CHIP8-Emulator.dir
rmdir /S /Q CMakeFiles
rmdir /S /Q Debug
rmdir /S /Q Release
rmdir /S /Q x64
