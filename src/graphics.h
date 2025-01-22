#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "raylib.h"

void initColourBuffer();
void freeMemory();
void drawToColourBuffer();
void drawPixelAt(int x, int y, Color colorToDraw);

#endif