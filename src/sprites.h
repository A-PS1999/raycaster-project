#ifndef SPRITES_H
#define SPRITES_H

typedef struct {
    float xPos;
    float yPos;
    float distance;
    float angle;
    int textureId;
} sprite_t;

void createSpriteProjection();
void drawMinimapSprites();

#endif