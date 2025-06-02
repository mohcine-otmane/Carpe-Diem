@echo off
gcc complex_plotter.c -o complex_plotter -I"C:\SDL2\include" -L"C:\SDL2\lib" -lSDL2main -lSDL2 -lSDL2_image -lm 