#ifndef SHIELD_H
#define SHIELD_H

#include <stdbool.h>
#include <SDL2/SDL.h>
#include "render.h"

typedef struct{
    float x;
    float y;
    bool alive;
}Shield;

void init_shields();
void render_shields(SDL_Renderer * renderer);
    
#endif
