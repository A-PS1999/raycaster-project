#include "sprites.h"
#include "constants.h"
#include "raylib.h"
#include "player.h"
#include "textures.h"

static sprite_t sprites[NUM_SPRITES] = {
    { .xPos = 850, .yPos = 630, .textureId = 9, .angle = 0, .distance = 0 }
};

void createSpriteProjection() {
    
}

void drawMinimapSprites() {
    for (int i=0; i < NUM_SPRITES; i++) {
        DrawRectangle(sprites[i].xPos * MINIMAP_SCALE_FACTOR, 
        sprites[i].yPos * MINIMAP_SCALE_FACTOR,
        MINIMAP_SPRITE_SIZE * MINIMAP_SCALE_FACTOR,
        MINIMAP_SPRITE_SIZE * MINIMAP_SCALE_FACTOR,
        YELLOW);
    }
}