#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdbool.h>

typedef struct player
{
    float x;
    float y;
};
  

typedef struct alien{
    float x;
    float y;

};


typedef struct bullet
{
    int direction;
    float x;
    float y;
};


typedef struct defence_block
{
    float x;
    float y;
};

typedef struct score
{
    int points;

};


