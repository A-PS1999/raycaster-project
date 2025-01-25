#ifndef RAY_H
#define RAY_H

#include <stdbool.h>
#include "raylib.h"
#include "constants.h"

typedef struct {
    float distance;
    float rayAngle;
    Vector2 wallHitPos;
    bool wasHitVertical;
    int wallHitContent;
} MyRay;

extern MyRay rays[NUM_RAYS];

void drawRays();
void castRays();
void castRay(float rayAngle, int stripId);
float normalizeAngle(float rayAngle);

#endif