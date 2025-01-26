#include "ray.h"
#include <float.h>
#include "player.h"
#include "map.h"
#include "raymath.h"

MyRay rays[NUM_RAYS];

void castRays() {
    for (int col=0; col < NUM_RAYS; col++) {
        float rayAngle = player.rotationAngle + atan2((col - NUM_RAYS/2), PROJ_PLANE_DIST);
        castRay(rayAngle, col);
    }
}

void castRay(float rayAngle, int stripId) {
    rayAngle = normalizeAngle(rayAngle);

    bool isRayFacingDown = rayAngle > 0 && rayAngle < PI;
    bool isRayFacingRight = rayAngle < (PI * 0.5) || rayAngle > (1.5 * PI);

    float xIntercept, yIntercept;
    float xStep, yStep;

    // Horizontal intersection
    bool foundHorizontalWall = false;
    int horizontalWallHitX = 0;
    int horizontalWallHitY = 0;
    int horizontalWallContent = 0;

    yIntercept = floor(player.playerPos.y / TILE_SIZE) * TILE_SIZE;
    yIntercept += isRayFacingDown ? TILE_SIZE : 0;

    xIntercept = player.playerPos.x + (yIntercept - player.playerPos.y) / tan(rayAngle);

    yStep = TILE_SIZE;
    yStep *= !isRayFacingDown ? -1 : 1;

    xStep = TILE_SIZE / tan(rayAngle);
    xStep *= (!isRayFacingRight && xStep > 0) ? -1 : 1;
    xStep *= (isRayFacingRight && xStep < 0) ? -1 : 1;

    float nextHorizontalTouchX = xIntercept;
    float nextHorizontalTouchY = yIntercept;

    while (isInMapBounds(nextHorizontalTouchX, nextHorizontalTouchY)) {
        float xToCheck = nextHorizontalTouchX;
        float yToCheck = nextHorizontalTouchY + (!isRayFacingDown ? -1 : 0);

        if (hasWallAtPos((Vector2){xToCheck, yToCheck})) {
            horizontalWallHitX = nextHorizontalTouchX;
            horizontalWallHitY = nextHorizontalTouchY;
            horizontalWallContent = getMapCellAt((int)floor(xToCheck/TILE_SIZE), (int)floor(yToCheck/TILE_SIZE));
            foundHorizontalWall = true;
            break;
        } else {
            nextHorizontalTouchX += xStep;
            nextHorizontalTouchY += yStep;
        }
    }

    // Vertical intersection
    bool foundVerticalWall = false;
    int verticalWallHitX = 0;
    int verticalWallHitY = 0;
    int verticalWallContent = 0;

    xIntercept = floor(player.playerPos.x / TILE_SIZE) * TILE_SIZE;
    xIntercept += isRayFacingRight ? TILE_SIZE : 0;

    yIntercept = player.playerPos.y + (xIntercept - player.playerPos.x) * tan(rayAngle);

    xStep = TILE_SIZE;
    xStep *= !isRayFacingRight ? -1 : 1;

    yStep = TILE_SIZE * tan(rayAngle);
    yStep *= (!isRayFacingDown && yStep > 0) ? -1 : 1;
    yStep *= (isRayFacingDown && yStep < 0) ? -1 : 1;

    float nextVerticalTouchX = xIntercept;
    float nextVerticalTouchY = yIntercept;

    while (isInMapBounds(nextVerticalTouchX, nextVerticalTouchY)) {
        float xToCheck = nextVerticalTouchX + (!isRayFacingRight ? -1 : 0);
        float yToCheck = nextVerticalTouchY;

        if (hasWallAtPos((Vector2){xToCheck, yToCheck})) {
            verticalWallHitX = nextVerticalTouchX;
            verticalWallHitY = nextVerticalTouchY;
            verticalWallContent = getMapCellAt((int)floor(xToCheck/TILE_SIZE), (int)floor(yToCheck/TILE_SIZE));
            foundVerticalWall = true;
            break;
        } else {
            nextVerticalTouchX += xStep;
            nextVerticalTouchY += yStep;
        }
    }

    float horizontalHitDistance = foundHorizontalWall ? Vector2Distance(player.playerPos, (Vector2){horizontalWallHitX, horizontalWallHitY}) : FLT_MAX;
    float verticalHitDistance = foundVerticalWall ? Vector2Distance(player.playerPos, (Vector2){verticalWallHitX, verticalWallHitY}) : FLT_MAX;

    if (verticalHitDistance < horizontalHitDistance) {
        rays[stripId].distance = verticalHitDistance;
        rays[stripId].wallHitPos = (Vector2){verticalWallHitX, verticalWallHitY};
        rays[stripId].wasHitVertical = true;
        rays[stripId].wallHitContent = verticalWallContent;
    } else {
        rays[stripId].distance = horizontalHitDistance;
        rays[stripId].wallHitPos = (Vector2){horizontalWallHitX, horizontalWallHitY};
        rays[stripId].wasHitVertical = false;
        rays[stripId].wallHitContent = horizontalWallContent;
    }
    rays[stripId].rayAngle = rayAngle;
}

void drawRays() {
    Vector2 rayDrawStartPos = (Vector2){
        (player.playerPos.x + 2.5) * MINIMAP_SCALE_FACTOR,
        player.playerPos.y * MINIMAP_SCALE_FACTOR
    };
    for (int i=0; i < NUM_RAYS; i++) {
        Vector2 currRayPos = (Vector2){
            rays[i].wallHitPos.x * MINIMAP_SCALE_FACTOR,
            rays[i].wallHitPos.y * MINIMAP_SCALE_FACTOR
        };
        DrawLineV(rayDrawStartPos, currRayPos, RED);
    }
}

float normalizeAngle(float rayAngle) {
    rayAngle = remainder(rayAngle, TWO_PI);

    if (rayAngle < 0) {
        rayAngle = TWO_PI + rayAngle;
    }

    return rayAngle;
}