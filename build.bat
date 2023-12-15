@echo off

mkdir build
cd build
cl -Zi ..\src\main.cpp ..\src\window.cpp ..\src\layouts.cpp ..\src\screen.cpp user32.lib kernel32.lib dwmapi.lib
cd ..
