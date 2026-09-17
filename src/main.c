#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <time.h>
#include "main.h"
#include "render.h"


#define FPS 60
#define FRAME_TIME 1000 / FPS

#define CENTERE_X WIDTH/2
#define CENTERE_Y HEIGHT/2
#define PLAYER_MOVE_SPEED 5



/*A Space invaders clone built from scratch
By Andres Eufrasio Tinajero
created 06/02/06

TODO:
ADD LOSING LIFE
ADD MULTIPLE LIVES
ADD TOP SPACESHIP FOR EXTRA POINTS
ADD SHIELDS
SWITCH TO DELTA TIME
*/





Player player = {CENTERE_X, CENTERE_Y + (CENTERE_Y*0.8)};
PlayerBullet player_bullet;



rectangle calculate_square_from_center(float ox, float oy, int w, int h){
    rectangle rect;
    rect.x = (int)ox-w / 2.0f;
    rect.y = (int)oy-h / 2.0f;
    rect.w = w;
    rect.h = h;
    return rect;
}



void render_background(SDL_Renderer * renderer){
    SDL_Rect background = {0, 0, WIDTH, HEIGHT};
    SDL_RenderDrawRect(renderer, &background);
    SDL_SetRenderDrawColor(renderer,0,0,20,255);
    SDL_RenderFillRect(renderer, &background);
}

void draw_player(SDL_Renderer * renderer){
    SDL_SetRenderDrawColor(renderer,0,255,0,255);

    rectangle pbb = calculate_square_from_center(player.x, player.y, 48, 15);
    rectangle pb  = calculate_square_from_center(player.x, player.y, 40, 15);
    rectangle pgb = calculate_square_from_center(player.x, player.y, 10, 10);
    rectangle pgh = calculate_square_from_center(player.x, player.y, 3, 5);
    // Render player Body
    SDL_Rect player_body_bottom = {pbb.x, pbb.y, pbb.w, pbb.h};
    SDL_Rect player_body = {pb.x, pb.y-3, pb.w, pb.h};

    SDL_RenderFillRect(renderer, &player_body_bottom);
    SDL_RenderDrawRect(renderer, &player_body_bottom);

    SDL_RenderFillRect(renderer, &player_body);
    SDL_RenderDrawRect(renderer, &player_body);

    // Render player gun

    SDL_Rect player_gun_base = {pgb.x, pgb.y-13, pgb.w, pgb.h};
    SDL_RenderFillRect(renderer, &player_gun_base);
    SDL_RenderDrawRect(renderer, &player_gun_base);   

    SDL_Rect player_gun_head = {pgh.x, pgh.y-18, pgh.w, pgh.h};
    SDL_RenderFillRect(renderer, &player_gun_head);
    SDL_RenderDrawRect(renderer, &player_gun_head);

    

}



int player_shoot(){
    if (player_bullet.alive){
        return 0;
    }
    
    player_bullet.alive = true;
    player_bullet.x = player.x - 2;
    player_bullet.y = player.y -2; 
    return 1;
};

int update_player_bullet(SDL_Renderer * renderer){
    if (!player_bullet.alive){
        return 0;
    }

    player_bullet.y -= BULLET_SPEED;
    if (player_bullet.y < 0){
        player_bullet.alive = false;
    }
    //draw
    SDL_Rect bullet_box= {player_bullet.x, player_bullet.y, BULLET_WIDTH ,BULLET_HEIGHT };
    SDL_SetRenderDrawColor(renderer,255,255,255,255);
    SDL_RenderFillRect(renderer, &bullet_box);
    SDL_RenderDrawRect(renderer, &bullet_box);
    return 1;
};




//gives slightly extra reach on hit box
#define ALIEN_COLLISION_LEFT -2
#define ALIEN_COLLISION_TOP -5



void collision(){

    // overlap player bullet + aliens
    for (int y = 0; y < ALIEN_ROW; y++){ 
        for (int x = alien_start; x <= alien_end; x++){
            if (!player_bullet.alive || !aliens[y][x].alive) {
                continue;
            }
            else{
                
                int dy = player_bullet.y - aliens[y][x].y;
                int dx = player_bullet.x - aliens[y][x].x;
                
                
                if (dx > ALIEN_COLLISION_LEFT && dx < ALIEN_SIZE && dy > ALIEN_COLLISION_TOP && dy < ALIEN_SIZE) {
                    player_bullet.alive = false;
                    aliens[y][x].alive = false;
                    player_bullet.x = player.x - 3;
                    player_bullet.y = player.y;
                    update_alien_length();
            }
        

            }
        }
    }       
    // overlap player bullet + alien bullet
    for(int i =0; i<MAX_ALIEN_BULLETS; i++){
        if (!player_bullet.alive){break;}
        if (alien_bullets[i].alive){
            int dy = alien_bullets[i].y - player_bullet.y;
            int dx = player_bullet.x - alien_bullets[i].x;
            //test
            if (dx > -9 && dx < 10 && dy > 20 ){
                player_bullet.alive = false;
                alien_bullets[i].alive = false;
                
            }

        }
    }
    
    // overlap alien bullet + player
    
}


    
    

int main(int argc, char * argv[]){
    srand(time(0));
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) != 0){
        printf("SDL initalization failed.");
        return 0;
    }
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window * window;
    SDL_Renderer * renderer;
    SDL_Texture * texture; 

    window = SDL_CreateWindow(GAME_NAME,  SDL_WINDOWPOS_CENTERED,  SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);   
    
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    texture = SDL_CreateTexture(renderer, PIXEL_FORMAT, SDL_TEXTUREACCESS_TARGET, WIDTH, HEIGHT);

    SDL_RenderSetLogicalSize(renderer, WIDTH,HEIGHT );
    SDL_SetRenderTarget(renderer, texture);


    //initialize game variables
    create_aliens();
    init_alien_bullets();
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
                    plyrctrl.shoot = true;
                    break;    
                case SDLK_SPACE:
                    plyrctrl.shoot = true;
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
                    plyrctrl.shoot = false;
                    break;    
                case SDLK_SPACE:
                    plyrctrl.shoot = false;
                    break;    
                default:
                    // ignore
                }

            }
        }

        // calc for player/screen boundry
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
        
        // update alien frame
        if (frameStart - alien_frame_timer >= 1000){
            alien_frame = !alien_frame;
            alien_frame_timer = frameStart;
        }

        // update based on button presses
        if (plyrctrl.shoot){
            player_shoot();
        }
        if (plyrctrl.left){
            player.x-=move_left_speed;
        }
        if (plyrctrl.right){
            player.x+=move_right_speed; 
        }
        
        SDL_SetRenderTarget(renderer, texture);
        //SDL_RenderClear(renderer);
        

        
        render_background(renderer);
        
        update_alien_position();
        collision();
        if(alien_end < alien_start){
            break;
        }
        
        
        if(does_alien_shoot()){
            alien_shoot();
        }
        update_alien_bullet(renderer);
        update_player_bullet(renderer);
        render_aliens(renderer);
        
        draw_player(renderer);
        


        SDL_SetRenderTarget(renderer, NULL);
        
        SDL_RenderCopy(renderer, texture, NULL , NULL);
        SDL_RenderPresent(renderer);

        Uint32 frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < FRAME_TIME){
            SDL_Delay(FRAME_TIME - frameTime);
        }


    }
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    return 1;
}  


