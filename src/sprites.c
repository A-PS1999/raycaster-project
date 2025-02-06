#include "sprites.h"
#include "constants.h"
#include "raymath.h"
#include "player.h"
#include "textures.h"
#include "graphics.h"
#include "ray.h"
#include <stdlib.h>

static sprite_t sprites[NUM_SPRITES] = {
    { .pos.x = 850, .pos.y = 630, .textureId = 9, .angle = 0, .distance = 0, .visible = false },
    { .pos.x = 500, .pos.y = 600, .textureId = 11, .angle = 0, .distance = 0, .visible = false },
    { .pos.x = 500, .pos.y = 600, .textureId = 10, .angle = 0, .distance = 0, .visible = false },
    { .pos.x = 420, .pos.y = 220, .textureId = 12, .angle = 0, .distance = 0, .visible = false },
    { .pos.x = 90, .pos.y = 100, .textureId = 13, .angle = 0, .distance = 0, .visible = false },
};

int distCompare(const void* spriteA, const void* spriteB) {
    return ((sprite_t*)spriteA)->distance - ((sprite_t*)spriteB)->distance;
}

void createSpriteProjection() {
    sprite_t visibleSprites[NUM_SPRITES];
    int numVisible = 0;

    for (int i=0; i < NUM_SPRITES; i++) {
        float angleBetweenSpriteAndPlayer = player.rotationAngle - atan2(sprites[i].pos.y - player.playerPos.y, sprites[i].pos.x - player.playerPos.x);

        if (angleBetweenSpriteAndPlayer > PI) {
            angleBetweenSpriteAndPlayer -= TWO_PI;
        }
        angleBetweenSpriteAndPlayer = fabs(angleBetweenSpriteAndPlayer);

        float visibilityMargin = 0.2;
        if (angleBetweenSpriteAndPlayer < (FOV_ANGLE / 2) + visibilityMargin) {
            sprites[i].visible = true;
            sprites[i].distance = Vector2Distance(sprites[i].pos, player.playerPos);
            visibleSprites[numVisible] = sprites[i];
            numVisible++;
        } else {
            sprites[i].visible = false;
        }
    }

    qsort(visibleSprites, numVisible, sizeof(sprite_t), distCompare);

    for (int j=0; j < numVisible; j++) {
        sprite_t currSprite = visibleSprites[j];

        float perpenDist = currSprite.distance * cos(currSprite.angle);

        float spriteProjHeight = (TILE_SIZE / perpenDist) * PROJ_PLANE_DIST;
        float spriteProjWidth = spriteProjHeight;

        float spriteTopY = (WINDOW_HEIGHT / 2) - (spriteProjHeight / 2);
        spriteTopY = (spriteTopY < 0)? 0 : spriteTopY;

        float spriteBottomY = spriteTopY + spriteProjHeight;
        spriteBottomY = (spriteBottomY > WINDOW_HEIGHT)? WINDOW_HEIGHT : spriteBottomY;

        float spriteAngle = atan2(currSprite.pos.y - player.playerPos.y, currSprite.pos.x - player.playerPos.x) - player.rotationAngle;
        float spriteScreenPosX = tan(spriteAngle) * PROJ_PLANE_DIST;

        float spriteLeftX = (WINDOW_WIDTH / 2) + spriteScreenPosX - (spriteProjWidth / 2);
        float spriteRightX = spriteLeftX + spriteProjWidth;

        int textureWidth = upng_get_width(textures[currSprite.textureId]);
        int textureHeight = upng_get_height(textures[currSprite.textureId]);

        for (int x = spriteLeftX; x < spriteRightX; x++) {
            float texelWidth = (textureWidth / spriteProjWidth);
            int textureOffsetX = (x - spriteLeftX) * texelWidth;

            for (int y = spriteTopY; y < spriteBottomY; y++) {
                // TODO: Add code for selecting color from texture
                if (x > 0 && x < WINDOW_WIDTH && y > 0 && y < WINDOW_HEIGHT) {
                    int distanceFromTop = y + (spriteProjHeight / 2) - (WINDOW_HEIGHT / 2);
                    int textureOffsetY = distanceFromTop * (textureHeight/ spriteProjHeight);

                    Color* spriteTexBuffer = (Color*)upng_get_buffer(textures[currSprite.textureId]);
                    Color texelColor = spriteTexBuffer[(textureWidth * textureOffsetY) + textureOffsetX];
                    bool isMagenta = (texelColor.r == 0xFF && texelColor.g == 0x00 && texelColor.b == 0xFF && texelColor.a == 0xFF);

                    if (!isMagenta && currSprite.distance < rays[x].distance) {
                        drawPixelAt(x, y, texelColor);
                    }
                }
            }
        }
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