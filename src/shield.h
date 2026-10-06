#ifndef SHIELD_H
#define SHIELD_H

#include <stdbool.h>
#include <SDL2/SDL.h>
#include "render.h"

#define SHIELD_HEIGHT 16
#define SHIELD_WIDTH  22
#define NUMBER_OF_SHIELDS  4
#define SPACE_BETWEEN_SHIELDS 100
#define PIXEL_SIZE 3
#define SHIELD_X_START 115
#define SHIELD_Y_START 470

typedef struct{
    float x;
    float y;
    bool alive;
}Shield;

void init_shields();
void kill_shield_bottom_init();
void kill_shield_top_init();
void render_shields(SDL_Renderer * renderer);
extern Shield shields[NUMBER_OF_SHIELDS][SHIELD_WIDTH][SHIELD_HEIGHT];
    
#endif
