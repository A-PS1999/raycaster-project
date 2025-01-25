#include "map.h"
#include "constants.h"

static const int map[MAP_NUM_ROWS][MAP_NUM_COLS] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 ,1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 2, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 5},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 5},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 5},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 5, 5, 5, 5, 5}
};

void drawMap() {
    for (int i=0; i < MAP_NUM_ROWS; i++) {
        for (int j=0; j < MAP_NUM_COLS; j++) {
            int tileX = j * TILE_SIZE;
            int tileY = i * TILE_SIZE;
            Color tileColour = map[i][j] != 0 ? RAYWHITE : BLACK;
            DrawRectangle(
                tileX * MINIMAP_SCALE_FACTOR, 
                tileY * MINIMAP_SCALE_FACTOR, 
                TILE_SIZE * MINIMAP_SCALE_FACTOR, 
                TILE_SIZE * MINIMAP_SCALE_FACTOR, 
                tileColour
            );
        }
    }
}

int getMapCellAt(int posX, int posY) {
    return map[posY][posX];
}

bool wouldCollide(float newX, float newY) {
    if (newX < 0 || newX > WINDOW_WIDTH || newY < 0 || newY > WINDOW_HEIGHT) {
        return true;
    }

    int mapXIdx = floor(newX / TILE_SIZE);
    int mapYIdx = floor(newY / TILE_SIZE);
    return getMapCellAt(mapXIdx, mapYIdx);
}

bool isInMapBounds(float posX, float posY) {
    return (posX >= 0 && posX <= MAP_NUM_COLS * TILE_SIZE) && (posY >= 0 && posY <= MAP_NUM_ROWS * TILE_SIZE);
}

bool hasWallAtPos(Vector2 mapPos) {
    if (mapPos.x < 0 || mapPos.x >= MAP_NUM_COLS * TILE_SIZE || mapPos.y < 0 || mapPos.y >= MAP_NUM_ROWS * TILE_SIZE) {
        return true;
    }
    int mapGridIndexX = floor(mapPos.x / TILE_SIZE);
    int mapGridIndexY = floor(mapPos.y / TILE_SIZE);
    return map[mapGridIndexY][mapGridIndexX] != 0;
}