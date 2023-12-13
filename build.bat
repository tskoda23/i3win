@echo off

mkdir build
cd build
cl ..\main.cpp user32.lib
