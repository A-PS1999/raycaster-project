#ifndef MAP_H
#define MAP_H

#include <stdbool.h>
#include "raylib.h"

void drawMap();
int getMapCellAt(int posX, int posY);
bool isInMapBounds(float posX, float posY);
bool wouldCollide(float newX, float newY);
bool hasWallAtPos(Vector2 mapPos);

#endif