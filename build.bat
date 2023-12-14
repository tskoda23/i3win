@echo off

mkdir build
cd build
cl -Zi ..\main.cpp user32.lib kernel32.lib  dwmapi.lib
