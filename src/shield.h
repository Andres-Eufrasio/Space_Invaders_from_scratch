#ifndef SHIELD_H
#define SHIELD_H

#include <stdbool.h>
#include "main.c"
#include "render.h"

typedef struct{
    float x;
    float y;
    bool alive;
}Shield;

void init_shields();
void render(SDL_Renderer * renderer);
    
#endif