@echo off

mkdir build
cd build
cl -Zi ..\src\main.cpp user32.lib kernel32.lib  dwmapi.lib
