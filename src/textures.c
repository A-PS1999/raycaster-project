#include "textures.h"

static char* textureFileNames[NUM_TEXTURES] = {
    "./resources/redbrick.png",
    "./resources/purplestone.png",
    "./resources/mossystone.png",
    "./resources/graystone.png",
    "./resources/colorstone.png",
    "./resources/bluestone.png",
    "./resources/wood.png",
    "./resources/eagle.png",
    "./resources/pikuma.png"
};

texture_t wallTextures[NUM_TEXTURES];

void loadWallTextures() {
    for (int i=0; i < NUM_TEXTURES; i++) {
        upng_t* upng;

        upng = upng_new_from_file(textureFileNames[i]);
        if (upng != NULL) {
            upng_decode(upng);
            if (upng_get_error(upng) == UPNG_EOK) {
                wallTextures[i].width = upng_get_width(upng);
                wallTextures[i].height = upng_get_height(upng);
                wallTextures[i].textureBuffer = (Color*)upng_get_buffer(upng);
                wallTextures[i].upngTexture = upng;
            }
        }
    }
}

void freeWallTextures() {
    for (int i=0; i < NUM_TEXTURES; i++) {
        upng_free(wallTextures[i].upngTexture);
    }
}