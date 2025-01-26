#include "walls.h"
#include "constants.h"
#include "graphics.h"
#include "ray.h"
#include "player.h"
#include "textures.h"

void changeColourIntensity(Color* colour, float factor) {
    colour->r *= factor;
    colour->g *= factor;
    colour->b *= factor;
}

void create3DProjection() {
    for (int i=0; i < NUM_RAYS; i++) {
        float perpenDistance = rays[i].distance * cos(rays[i].rayAngle - player.rotationAngle);
        float projectedWallHeight = (TILE_SIZE / perpenDistance) * PROJ_PLANE_DIST;

        float wallTopPixel = (WINDOW_HEIGHT/2) - (projectedWallHeight/2);
        wallTopPixel = wallTopPixel < 0 ? 0 : wallTopPixel;

        float wallBottomPixel = (WINDOW_HEIGHT/2) + (projectedWallHeight/2);
        wallBottomPixel = wallBottomPixel > WINDOW_HEIGHT ? WINDOW_HEIGHT : wallBottomPixel;

        for (int ceilPixel=0; ceilPixel < wallTopPixel; ceilPixel++) {
            drawPixelAt(i, ceilPixel, (Color){ .a = 0xFF, .r = 0x8A, .g = 0x89, .b = 0x88 });
        }

        int textureOffsetX;

        if (rays[i].wasHitVertical) {
            textureOffsetX = (int)rays[i].wallHitPos.y % TILE_SIZE;
        } else {
            textureOffsetX = (int)rays[i].wallHitPos.x % TILE_SIZE;
        }

        int textureIdx = rays[i].wallHitContent-1;

        for (int wallPixel=wallTopPixel; wallPixel < wallBottomPixel; wallPixel++) {
            int distanceFromTop = wallPixel + (projectedWallHeight / 2) - (WINDOW_HEIGHT / 2);
            int textureOffsetY = distanceFromTop * ((float)TEXTURE_HEIGHT / projectedWallHeight);

            Color texelColor = wallTextures[textureIdx].textureBuffer[(TEXTURE_WIDTH * textureOffsetY) + textureOffsetX];

            if (rays[i].wasHitVertical) {
                changeColourIntensity(&texelColor, 0.7);
            }

            drawPixelAt(i, wallPixel, texelColor);
        }

        for (int floorPixel=wallBottomPixel; floorPixel < WINDOW_HEIGHT; floorPixel++) {
            drawPixelAt(i, floorPixel, (Color){ .a = 0xFF, .r = 0xB3, .g = 0xB2, .b = 0xB1 });
        }
    }
}