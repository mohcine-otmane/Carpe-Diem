@echo off
gcc countdown.c -o countdown -I"C:\SDL2\include" -L"C:\SDL2\lib" -lSDL2main -lSDL2 -lSDL2_ttf -lm 