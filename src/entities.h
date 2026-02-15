#ifndef ENTITIES_H
#define ENTITIES_H



#include <stdio.h>
#include <stdbool.h>



typedef struct
{
    float x;
    float y;
}Player;
  

typedef struct{
    float x;
    float y;
    bool alive;
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


#endif