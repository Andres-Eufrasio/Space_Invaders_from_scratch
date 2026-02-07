#include <stdio.h>
#include <SDL2/SDL.h>
#include "entities.c"
#define GAME_NAME "Space invaders"
#define WIDTH 900
#define HEIGHT 600
#define PIXEL_FORMAT SDL_PIXELFORMAT_RGBA8888
#define CENTERE_X WIDTH/2
#define CENTERE_Y HEIGHT/2
#define PLAYER_MOVE_SPEED 5
#define FPS 60
#define FRAME_TIME 1000 / FPS

/*A Space invaders clone built from scratch
By Andres Eufrasio Tinajero
created 06/02/06


*/
Player player = {CENTERE_X, CENTERE_Y + (CENTERE_Y*0.9)};

typedef struct SDL_Instances{
    SDL_Window * window;
    SDL_Renderer * renderer;
    SDL_Texture * texture;
}SDL_Instances;

typedef struct Point
{
    float x,y;
}Point;

typedef struct Controller{
    bool left;
    bool right;
    bool up;
}Controller;

struct SDL_Instances sdlin_t;


void shutdown(){
    SDL_DestroyTexture(sdlin_t.texture);
    SDL_DestroyRenderer(sdlin_t.renderer);
    SDL_DestroyWindow(sdlin_t.window);
}

void render_background(SDL_Renderer * renderer){
    SDL_Rect * background = {0, 0, WIDTH, HEIGHT};
    SDL_RenderDrawRect(renderer, background);
    SDL_SetRenderDrawColor(renderer,0,0,20,255);
    SDL_RenderFillRect(renderer, background);
}

void draw_player(SDL_Renderer * renderer){
    float size = 0.1;
    SDL_SetRenderDrawColor(renderer,255,255,255,255);

    SDL_Rect player_body = {(int)player.x-20, player.y-10, 40, 20};
    SDL_Rect player_gun = {(int)player.x-5, player.y-18,10,10};

    SDL_RenderFillRect(renderer, &player_body);
    SDL_RenderFillRect(renderer, &player_gun);
    SDL_RenderDrawRect(renderer, &player_body);
    SDL_RenderDrawRect(renderer, &player_gun);

}

void shoot(){};

int main(int argc, char * argv[]){
    SDL_Init(SDL_INIT_AUDIO);
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window * window = sdlin_t.window;
    SDL_Renderer * renderer = sdlin_t.renderer;
    SDL_Texture * texture =sdlin_t.texture; 

    window = SDL_CreateWindow(GAME_NAME,  SDL_WINDOWPOS_CENTERED,  SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);   
    
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    texture = SDL_CreateTexture(renderer, PIXEL_FORMAT, SDL_TEXTUREACCESS_TARGET, WIDTH, HEIGHT);
    SDL_RenderSetLogicalSize(renderer, WIDTH,HEIGHT );
    SDL_SetRenderTarget(renderer, texture);


    render_background(renderer);
    draw_player(renderer);


    SDL_SetRenderTarget(renderer, NULL);


    //controler
    Controller plyrctrl = {false, false, false};
    
    int shoot_time = 0;
    int move_left_speed=PLAYER_MOVE_SPEED;
    int move_right_speed=PLAYER_MOVE_SPEED;

    
    while(1){
        SDL_Event e;
        if (SDL_PollEvent(&e)){
            if(e.type == SDL_QUIT){
                break;
            }
            if(e.type == SDL_KEYDOWN){
                switch (e.key.keysym.sym) {
                case SDLK_LEFT:
                    plyrctrl.left = true;
                    break;
                case SDLK_RIGHT:
                    plyrctrl.right = true;
                    break;
                case SDLK_UP:
                    plyrctrl.up = true;
                    break;    
                default:
                    // ignore
                }

            }
            if(e.type == SDL_KEYUP){
                switch (e.key.keysym.sym) {
                case SDLK_LEFT:
                    plyrctrl.left = false;
                    
                    break;
                case SDLK_RIGHT:
                    plyrctrl.right = false;
                    break;
                case SDLK_UP:
                    plyrctrl.up = false;
                    break;    
                default:
                    // ignore
                }

            }
        }

        // calc if player on edge
        if (player.x<50){
            move_left_speed =0;
            
        }
        else{move_left_speed = PLAYER_MOVE_SPEED;}
        if (player.x>WIDTH-50){
            move_right_speed =0;
        }
        else{move_right_speed = PLAYER_MOVE_SPEED;}


        // start frame
        Uint32 frameStart = SDL_GetTicks();
        if (plyrctrl.up){

        }
        if (plyrctrl.left){
            player.x-=move_left_speed;
        }
        if (plyrctrl.right){
            player.x+=move_right_speed;
        }
        
        SDL_SetRenderTarget(renderer, texture);
        SDL_RenderClear(renderer);
        render_background(renderer);
        draw_player(renderer);
        SDL_SetRenderTarget(renderer, NULL);
        
        SDL_RenderCopy(renderer, texture, NULL , NULL);
        SDL_RenderPresent(renderer);

        Uint32 frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < FRAME_TIME){
            SDL_Delay(FRAME_TIME - frameTime);
        }

    shutdown();
    }
}  


