#ifndef SPRITES_H
#define SPRITES_H

#include <stdbool.h>
#include "raylib.h"

typedef struct {
    Vector2 pos;
    float distance;
    float angle;
    int textureId;
    bool visible;
} sprite_t;

void createSpriteProjection();
void drawMinimapSprites();

#endif