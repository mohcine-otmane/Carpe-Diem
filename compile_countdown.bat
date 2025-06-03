@echo off
windres icon.rc -O coff -o icon.res
g++ countdown.c icon.res -o countdown.exe -I"C:\SDL2\include" -L"C:\SDL2\lib" -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -mwindows 