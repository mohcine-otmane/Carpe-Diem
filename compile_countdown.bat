@echo off
g++ countdown.c -o countdown.exe -I"C:\SDL2\include" -L"C:\SDL2\lib" -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -mwindows 