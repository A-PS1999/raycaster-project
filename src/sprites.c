#include "sprites.h"
#include "constants.h"
#include "raymath.h"
#include "player.h"
#include "textures.h"

static sprite_t sprites[NUM_SPRITES] = {
    { .pos.x = 850, .pos.y = 630, .textureId = 9, .angle = 0, .distance = 0, .visible = false },
};

void createSpriteProjection() {
    sprite_t visibleSprites[NUM_SPRITES];
    int numVisible = 0;

    for (int i=0; i < NUM_SPRITES; i++) {
        float angleBetweenSpriteAndPlayer = player.rotationAngle - atan2(sprites[i].pos.y - player.playerPos.y, sprites[i].pos.x - player.playerPos.x);

        if (angleBetweenSpriteAndPlayer > PI) {
            angleBetweenSpriteAndPlayer -= TWO_PI;
        }
        angleBetweenSpriteAndPlayer = fabs(angleBetweenSpriteAndPlayer);

        if (angleBetweenSpriteAndPlayer < (FOV_ANGLE / 2)) {
            sprites[i].visible = true;
            sprites[i].distance = Vector2Distance(sprites[i].pos, player.playerPos);
            visibleSprites[numVisible] = sprites[i];
            numVisible++;
        } else {
            sprites[i].visible = false;
        }
    }

    for (int j=0; j < numVisible; j++) {
        sprite_t currSprite = visibleSprites[j];

        float spriteProjHeight = (TILE_SIZE / currSprite.distance) * PROJ_PLANE_DIST;
        float spriteProjWidth = spriteProjHeight;

        float spriteTopY = (WINDOW_HEIGHT / 2) - (spriteProjHeight / 2);
        spriteTopY = (spriteTopY < 0)? 0 : spriteTopY;

        float spriteBottomY = spriteTopY + spriteProjHeight;
        spriteBottomY = (spriteBottomY > WINDOW_HEIGHT)? WINDOW_HEIGHT : spriteBottomY;
    }
}

void drawMinimapSprites() {
    for (int i=0; i < NUM_SPRITES; i++) {
        DrawRectangle(sprites[i].pos.x * MINIMAP_SCALE_FACTOR, 
        sprites[i].pos.y * MINIMAP_SCALE_FACTOR,
        MINIMAP_SPRITE_SIZE * MINIMAP_SCALE_FACTOR,
        MINIMAP_SPRITE_SIZE * MINIMAP_SCALE_FACTOR,
        sprites[i].visible? YELLOW : DARKGRAY);
    }
}