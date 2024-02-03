@echo off

rmdir build /Q /S
mkdir build
cd build
cl -Zi ..\src\*.cpp user32.lib kernel32.lib dwmapi.lib shell32.lib gdi32.lib /Fe:i3win.exe
cd ..
