#ifndef ALIEN_H
#define ALIEN_H
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "render.h"

rectangle calculate_square_from_center(float ox, float oy, int w, int h);

#define ALIEN_ROW 5
#define ALIEN_COL 11
#define ALIEN_X 120
#define ALIEN_Y 70
#define ALIEN_SPACE_X 40
#define ALIEN_SPACE_Y 40
#define ALIEN_SIZE 30
// seen as 1 / ALIEN_SHOOT_CHANGE
#define ALIEN_SHOOT_CHANGE 100
#define MAX_ALIEN_BULLETS 6
#define BULLET_SPEED 10
#define BULLET_HEIGHT 20
#define BULLET_WIDTH 3

void create_aliens();
void update_alien_length();
void init_alien_bullets();
void update_alien_position();
int does_alien_shoot();
void alien_shoot();
void update_alien_bullet(SDL_Renderer *renderer);
void render_aliens(SDL_Renderer *renderer);


/*
Species list
depressed alien
happy alien
squid alien
mothership
*/
typedef struct{
    float x;
    float y;
    bool alive;
    int species; 
}Alien;

typedef struct 
{
    float x;
    float y;
    bool alive;
}AlienBullet;


#endif