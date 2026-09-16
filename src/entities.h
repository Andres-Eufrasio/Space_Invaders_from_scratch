#ifndef ENTITIES_H
#define ENTITIES_H

#include <stdio.h>
#include <stdbool.h>



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
}PlayerBullet;

typedef struct 
{
    float x;
    float y;
    bool alive;
}AlienBullet;


typedef struct
{
    float x;
    float y;
}defence_block;

typedef struct
{
    int points;

}score;

typedef struct
{
    int x;
    int y;
    int w;
    int h;

}rectangle;


#endif