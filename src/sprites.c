#include "sprites.h"
#include "constants.h"
#include "raymath.h"
#include "player.h"
#include "textures.h"
#include "graphics.h"

static sprite_t sprites[NUM_SPRITES] = {
    { .pos.x = 850, .pos.y = 630, .textureId = 9, .angle = 0, .distance = 0, .visible = false },
    { .pos.x = 500, .pos.y = 600, .textureId = 11, .angle = 0, .distance = 0, .visible = false },
    { .pos.x = 420, .pos.y = 220, .textureId = 12, .angle = 0, .distance = 0, .visible = false }
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

    for (int j=0; j < numVisible; j++) {
        sprite_t currSprite = visibleSprites[j];

        float spriteProjHeight = (TILE_SIZE / currSprite.distance) * PROJ_PLANE_DIST;
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

                    if (!isMagenta) {
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