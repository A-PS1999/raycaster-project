#ifndef TEXTURES_H
#define TEXTURES_H

#include <stdint.h>
#include "constants.h"
#include "upng.h"
#include "raylib.h"

typedef struct {
    int width;
    int height;
    Color* textureBuffer;
    upng_t* upngTexture;
} texture_t;

extern texture_t wallTextures[NUM_TEXTURES];

void loadWallTextures();
void freeWallTextures();

#endif