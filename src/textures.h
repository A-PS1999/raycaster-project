#ifndef TEXTURES_H
#define TEXTURES_H

#include <stdint.h>
#include "constants.h"
#include "upng.h"
#include "raylib.h"

extern upng_t* textures[NUM_TEXTURES];

void loadTextures();
void freeTextures();

#endif