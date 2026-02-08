#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdbool.h>



typedef struct Player
{
    float x;
    float y;
}Player;
  

typedef struct Alien{
    float x;
    float y;
    bool alive;
}Alien;



typedef struct PlayerBullet
{
    float x;
    float y;
    bool alive;
}PlayerBullet;


typedef struct defence_block
{
    float x;
    float y;
}defence_block;

typedef struct score
{
    int points;

}score;


