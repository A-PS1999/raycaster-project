#include <stdlib.h>
#include "graphics.h"
#include "constants.h"

static Color* colourBuffer = NULL;
static Image imgFromBuffer;
static Texture2D colourBufferTexture;

void initColourBuffer() {
    colourBuffer = malloc(WINDOW_WIDTH * WINDOW_HEIGHT * sizeof(Color));
    imgFromBuffer = (Image){ 
        .data = colourBuffer, 
        .width = WINDOW_WIDTH, 
        .height = WINDOW_HEIGHT,
        .mipmaps = 1,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8
    };
    colourBufferTexture = LoadTextureFromImage(imgFromBuffer);
}

void freeMemory() {
    free(colourBuffer);
    UnloadTexture(colourBufferTexture);
}

void drawToColourBuffer() {
    UpdateTexture(colourBufferTexture, colourBuffer);
    DrawTexture(colourBufferTexture, 0, 0, WHITE);
}

void drawPixelAt(int x, int y, Color colorToDraw) {
    colourBuffer[(WINDOW_WIDTH * y) + x] = colorToDraw;
}