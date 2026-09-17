#ifndef MAIN_H
#define MAIN_H

#include <stdbool.h>
#include "aliens.h"


extern Alien **aliens;
extern AlienBullet *alien_bullets;
extern int alien_bullet_count;
extern int alien_start;
extern int alien_end;
extern int alien_frame;
extern Uint32 alien_frame_timer;

typedef struct
{
    float x;
    float y;
}Player;
  
/*
Species list
depressed alien
happy alien
squid alien
mothership
*/


typedef struct
{
    float x;
    float y;
    bool alive;
}PlayerBullet;

typedef struct Controller{
    bool left;
    bool right;
    bool shoot;
}Controller;

typedef struct
{
    float x;
    float y;
}defence_block;

typedef struct
{
    int points;

}score;

#endif