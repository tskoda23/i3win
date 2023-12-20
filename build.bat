@echo off

rmdir build /Q /S
mkdir build
cd build
cl -Zi ..\src\main.cpp ..\src\logger.cpp ..\src\config.cpp ..\src\window.cpp ..\src\layouts.cpp ..\src\screen.cpp user32.lib kernel32.lib dwmapi.lib
cd ..
