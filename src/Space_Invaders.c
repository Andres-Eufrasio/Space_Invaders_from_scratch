#include <stdio.h>
#include <SDL2/SDL.h>
//#include <SDL_image.h>
#include "entities.c"
#define GAME_NAME "Space invaders"
//screen size
#define WIDTH 650
#define HEIGHT 650

#define PIXEL_FORMAT SDL_PIXELFORMAT_RGBA8888
#define FPS 60
#define FRAME_TIME 1000 / FPS

#define CENTERE_X WIDTH/2
#define CENTERE_Y HEIGHT/2
#define PLAYER_MOVE_SPEED 5

#define ALIEN_ROW 5
#define ALIEN_COL 11
#define ALIEN_X 120
#define ALIEN_Y 70
#define ALIEN_SPACE_X 40
#define ALIEN_SPACE_Y 40
#define ALIEN_SIZE 20

/*A Space invaders clone built from scratch
By Andres Eufrasio Tinajero
created 06/02/06

TODO:
ADD LOSING LIFE
ADD MULTIPLE LIVES
ADD TOP SPACESHIP FOR EXTRA POINTS
ADD SHIELDS
*/





Player player = {CENTERE_X, CENTERE_Y + (CENTERE_Y*0.75)};
PlayerBullet player_bullet;
Alien ** aliens;
int alien_start = 0;
int alien_end = ALIEN_COL-1;




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
    SDL_SetRenderDrawColor(renderer,0,255,0,255);

    SDL_Rect player_body = {(int)player.x-20, player.y-10, 40, 20};
    SDL_Rect player_gun = {(int)player.x-5, player.y-18,10,10};

    SDL_RenderFillRect(renderer, &player_body);
    SDL_RenderFillRect(renderer, &player_gun);
    SDL_RenderDrawRect(renderer, &player_body);
    SDL_RenderDrawRect(renderer, &player_gun);

}

void create_aliens(){
    int alien_x = ALIEN_X;
    int alien_y = ALIEN_Y;
    
    // change to unsigned char
    aliens = (Alien **)malloc(ALIEN_ROW * sizeof(Alien *));
    if (!aliens){
        printf("Alien allocation error on creation");

    }
    for (int y=0; y<ALIEN_ROW; y++){
        aliens[y] = (Alien *)malloc(ALIEN_COL * sizeof(Alien));
        for (int x=0; x<ALIEN_COL; x++){
            aliens[y][x] = (Alien){alien_x, alien_y, true};
            alien_x += ALIEN_SPACE_X;
        }
        alien_y += ALIEN_SPACE_Y;
        alien_x = ALIEN_X;
    }
    
};



void render_aliens(SDL_Renderer * renderer){
    
    SDL_SetRenderDrawColor(renderer,255,255,255,255);
    
    for (int y=0; y<ALIEN_ROW; y++){
        for (int x=0; x<=alien_end; x++){
            if (aliens[y][x].alive){
                
                SDL_Rect alien_box= {aliens[y][x].x, aliens[y][x].y, ALIEN_SIZE, ALIEN_SIZE };
                SDL_RenderFillRect(renderer, &alien_box);
                SDL_RenderDrawRect(renderer, &alien_box);
                }
        }
    }
};

int player_shoot(){
    if (player_bullet.alive){
        return 0;
    }
    
    player_bullet.alive = true;
    player_bullet.x = player.x - 3;
    player_bullet.y = player.y; 
    //player.x player.y
    return 1;
};

int update_bullet(SDL_Renderer * renderer){
    if (!player_bullet.alive){
        return 0;
    }

    player_bullet.y -= 10;
    if (player_bullet.y < 0){
        player_bullet.alive = false;
    }
    //draw
    SDL_Rect bullet_box= {player_bullet.x, player_bullet.y, 10 ,20 };
    SDL_SetRenderDrawColor(renderer,255,255,255,255);
    SDL_RenderFillRect(renderer, &bullet_box);
    SDL_RenderDrawRect(renderer, &bullet_box);
    //printf("%d",player.y);
    return 1;
};




int alien_direction = -1;
float alien_speed = 0.2;
int collision(){
    {
        for (int y=0; y<ALIEN_ROW; y++){ 
            for (int x=alien_start; x<=alien_end; x++){
                //Collision
                if (aliens[y][x].alive && player_bullet.x - aliens[y][x].x > -10 && player_bullet.x - aliens[y][x].x < 20 && player_bullet.y - aliens[y][x].y -5 < 0){
                    player_bullet.alive = false;
                    aliens[y][x].alive = false;
                    player_bullet.x = player.x - 3;
                    player_bullet.y = player.y; 
                    return 0;
                } 
            }
        }
    }

};

int new_line_flag = 0;
void update_alien_position(){
    if (aliens[0][alien_start].x <= 0 ||aliens[0][alien_end].x >= WIDTH - 20){
        alien_direction *= -1;
        alien_speed += 0.2;
        new_line_flag = 1;
        
    }
    
    for (int y=0; y<ALIEN_ROW; y++){ 
        for (int x=alien_start; x<=alien_end; x++){
            aliens[y][x].x += alien_direction*alien_speed;
        }
    if(new_line_flag)
        for (int y=0; y<ALIEN_ROW; y++){ 
            for (int x=alien_start; x<=alien_end; x++){
                aliens[y][x].y += 20;
            }
            
        }
        new_line_flag = 0;
    }
    

}

void update_alien_length(){
    bool left_alive = false;
    bool right_alive = false;
    for (int y = 0; y < ALIEN_ROW; y++) {
        if (aliens[y][alien_start].alive) {
            left_alive = true;
            break;
        }
    }
    for (int y = 0; y < ALIEN_ROW; y++) {
        if (aliens[y][alien_end].alive) {
            right_alive = true;
            break;
        }
    }

    if (!left_alive) {
        alien_start++;
        
    }
    if (!right_alive) {
        alien_end--;
        
    }
};

void update(){
    update_alien_position();
    collision();
    update_alien_length();

};

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
    create_aliens();


    


    //controler
    Controller plyrctrl = {false, false, false};
    
    int shoot_time = 0;
    int move_left_speed=PLAYER_MOVE_SPEED;
    int move_right_speed=PLAYER_MOVE_SPEED;
    player_bullet.alive=false;
    player_bullet.x=10;
    player_bullet.y=10;
    
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
        if (player.x<=0){
            move_left_speed =0;
            
        }
        else{move_left_speed = PLAYER_MOVE_SPEED;}
        if (player.x>WIDTH-25){
            move_right_speed =0;
        }
        else{move_right_speed = PLAYER_MOVE_SPEED;}
        

        

        // start frame
        Uint32 frameStart = SDL_GetTicks();
        
        

        if (plyrctrl.up){
            player_shoot();
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
        update_bullet(renderer);
        update();
        
        draw_player(renderer);
        render_aliens(renderer);


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


