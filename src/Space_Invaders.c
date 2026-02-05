#include <stdio.h>
#include <SDL2/SDL.h>

#define GAME_NAME "Space invaders"
#define WIDTH 900
#define HEIGHT 600
#define PIXEL_FORMAT SDL_PIXELFORMAT_RGBA8888

typedef struct SDL_Instances{
    SDL_Window * window;
    SDL_Renderer * renderer;
    SDL_Texture * texture;
}SDL_Instances;

struct SDL_Instances SDL_Instances_t;

void shutdown(){
    SDL_DestroyTexture(SDL_Instances_t.texture);
    SDL_DestroyRenderer(SDL_Instances_t.renderer);
    SDL_DestroyWindow(SDL_Instances_t.window);
}


int main(int argc, char * argv[]){
    SDL_Init(SDL_INIT_AUDIO);
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window * window;
    SDL_Renderer * renderer;
    SDL_Texture * texture;
    SDL_Instances_t.window = window;
    SDL_Instances_t.renderer = renderer;
    SDL_Instances_t.texture = texture;
    


    window = SDL_CreateWindow(GAME_NAME,  SDL_WINDOWPOS_CENTERED,  SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);   
    
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    texture = SDL_CreateTexture(renderer, PIXEL_FORMAT, SDL_TEXTUREACCESS_TARGET, WIDTH, HEIGHT);
    //SDL_RenderSetLogicalSize(renderer, WIDTH,HEIGHT );
    SDL_SetRenderTarget(renderer, texture);

    SDL_Rect background = {0, 0, WIDTH, HEIGHT};
    SDL_RenderDrawRect(renderer, &background);
    SDL_SetRenderDrawColor(renderer,20,150,20,255);
    SDL_RenderFillRect(renderer, &background);


    SDL_SetRenderTarget(renderer, NULL);
    printf("TEST");
    while(1){
        SDL_Event e;
        if (SDL_PollEvent(&e)){
            if(e.type == SDL_QUIT){
                break;
            }
        }
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL , NULL);
        //SDL_RenderSetScale(renderer, zoom_amount, zoom_amount);
        SDL_RenderPresent(renderer);
        SDL_Delay(10);
        


    }

    shutdown();
}  


