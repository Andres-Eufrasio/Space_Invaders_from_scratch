#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <time.h>
#include "main.h"
#include "render.h"
#include "shield.h"



enum {
    FPS = 60,
    FRAME_TIME = 1000 / FPS,
    CENTER_X = WIDTH / 2,
    CENTER_Y = HEIGHT / 2,
    PLAYER_MOVE_SPEED = 200
};


/*A Space invaders clone built from scratch
By Andres Eufrasio Tinajero
created 06/02/06

TODO:
ADD LOSING LIFE
ADD MULTIPLE LIVES
ADD TOP SPACESHIP FOR EXTRA POINTS
SWITCH TO DELTA TIME
*/


Player player = {CENTER_X, CENTER_Y + (CENTER_Y*0.8), 3, 0};
PlayerBullet player_bullet;

rectangle calculate_square_from_center(float ox, float oy, int w, int h){
    rectangle rect;
    
    int HALF = 2.0f;
    rect.x = (int)ox-w / HALF;
    rect.y = (int)oy-h / HALF;
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
    SDL_Rect bullet_box= {player_bullet.x, player_bullet.y, BULLET_WIDTH, BULLET_HEIGHT};
    SDL_SetRenderDrawColor(renderer,255,255,255,255);
    SDL_RenderFillRect(renderer, &bullet_box);
    SDL_RenderDrawRect(renderer, &bullet_box);
    return 1;
};


static bool between(int v, int lo, int hi) {
    return v > lo && v < hi;
}

static void kill_alien_bullet(int i) {
    alien_bullets[i].alive = false;
    alien_bullet_count--;
}

// player bullet vs aliens
static void collide_player_bullet_with_aliens(void) {
    //gives slightly extra reach on hit box
    int const ALIEN_COLLISION_LEFT = -3;
    int const ALIEN_COLLISION_TOP = -5;
    if (!player_bullet.alive) return;

    for (int y = 0; y < ALIEN_ROW; y++) {
        for (int x = alien_start; x <= alien_end; x++) {
            if (!aliens[y][x].alive) continue;

            int dx = player_bullet.x - aliens[y][x].x;
            int dy = player_bullet.y - aliens[y][x].y;

            if (between(dx, ALIEN_COLLISION_LEFT, ALIEN_SIZE) &&
                between(dy, ALIEN_COLLISION_TOP,  ALIEN_SIZE)) {
                player_bullet.alive = false;
                aliens[y][x].alive = false;
                update_alien_length();
                return; // bullet is used up
            }
        }
    }
}

// alien bullets vs player and vs player bullet
static void collide_alien_bullets(void) {
    for (int i = 0; i < MAX_ALIEN_BULLETS; i++) {
        if (!alien_bullets[i].alive) continue;

        // vs player
        if (between(player.x - alien_bullets[i].x, -10, 10) &&
            alien_bullets[i].y - player.y > 20) {
            kill_alien_bullet(i);
            printf("death");
            continue;
        }

        // vs player bullet
        if (player_bullet.alive &&
            between(player_bullet.x - alien_bullets[i].x, -10, 10) &&
            alien_bullets[i].y - player_bullet.y > 20) {
            player_bullet.alive = false;
            kill_alien_bullet(i);
        }
    }
}

// both kinds of bullets vs shield blocks
static void collide_shields(void) {
    for (int s = 0; s < NUMBER_OF_SHIELDS; s++) {
        for (int y = 0; y < SHIELD_HEIGHT; y++) {
            for (int x = 0; x < SHIELD_WIDTH; x++) {
                Shield *b = &shields[s][x][y];   
                if (!b->alive) continue;

                // alien bullets
                for (int i = 0; i < MAX_ALIEN_BULLETS; i++) {
                    if (!alien_bullets[i].alive) continue;

                    if (between(b->x - alien_bullets[i].x, -2, 2) &&
                        alien_bullets[i].y - b->y > 2) {
                        b->alive = false;
                        kill_alien_bullet(i);
                        shield_explosion(s, x, y);
                        break;
                    }
                }
                if (!b->alive) continue;

                // player bullet
                if (player_bullet.alive &&
                    between(b->x - player_bullet.x, -2, 2) &&
                    b->y - player_bullet.y >= 1) {
                    b->alive = false;
                    player_bullet.alive = false;
                    shield_explosion(s, x, y);
                }
            }
        }
    }
}

void collision(void) {
    collide_player_bullet_with_aliens();
    collide_alien_bullets();
    collide_shields();
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
    init_shields();
    Controller plyrctrl = {false, false, false};
    bool plyrQUIT = false;
    int shoot_time = 0;
    int move_left_speed=PLAYER_MOVE_SPEED;
    int move_right_speed=PLAYER_MOVE_SPEED;
    player_bullet.alive=false;
    player_bullet.x=10;
    player_bullet.y=10;
    Uint32 lastFrameTime = 0;
    bool new_line = false;
    int alien_speed =750;
    
    while(1){
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_QUIT:
                    plyrQUIT = true;
                    break;

                case SDL_KEYDOWN:
                    if (e.key.repeat) {
                        break;
                    }

                    switch (e.key.keysym.sym) {
                        case SDLK_LEFT:
                            plyrctrl.left = true;
                            break;

                        case SDLK_RIGHT:
                            plyrctrl.right = true;
                            break;

                        case SDLK_SPACE:
                        case SDLK_UP:
                            plyrctrl.shoot = true;
                            break;

                        case SDLK_ESCAPE:
                            plyrQUIT = true;
                            break;
                    }
                    break;

                case SDL_KEYUP:
                    switch (e.key.keysym.sym) {
                        case SDLK_LEFT:
                            plyrctrl.left = false;
                            break;

                        case SDLK_RIGHT:
                            plyrctrl.right = false;
                            break;

                        case SDLK_SPACE:
                        case SDLK_UP:
                            plyrctrl.shoot = false;
                            break;
                    }
                    break;
            }
        }

        if (plyrQUIT){
            break;
        }

        // calc for player/screen boundry
        if (player.x<=25){
            move_left_speed =0;
        }
        else{move_left_speed = PLAYER_MOVE_SPEED;}
        if (player.x>WIDTH-30){
            move_right_speed =0;
        }
        else{move_right_speed = PLAYER_MOVE_SPEED;}
        

        // start frame
        Uint32 currentTime = SDL_GetTicks();

        float dt = (currentTime - lastFrameTime) / 1000.0f;
        lastFrameTime = currentTime;
        
        // update alien frame
        if (currentTime - alien_frame_timer >= alien_speed){
            alien_frame = !alien_frame;
            alien_frame_timer = currentTime;
            
            if (!new_line){
                new_line = update_alien_position(new_line);
            }
            else{ 
                new_line = update_alien_position(new_line);
                alien_speed += 50;
                new_line = false;
                }
			if(alien_end < alien_start){
				break;
			}
        }
        

        // update based on button presses
        if (plyrctrl.shoot){
            player_shoot();
        }
        if (plyrctrl.left){
            player.x-=move_left_speed*dt;
        }
        if (plyrctrl.right){
            player.x+=move_right_speed*dt; 
        }
        collision();
        SDL_SetRenderTarget(renderer, texture);
        //SDL_RenderClear(renderer);
        

        
        render_background(renderer);

        
        if(does_alien_shoot()){
            alien_shoot();
        }
        update_alien_bullet(renderer);
        update_player_bullet(renderer);
        render_aliens(renderer);
        render_shields(renderer);
        
        draw_player(renderer);

        SDL_SetRenderTarget(renderer, NULL);
        
        SDL_RenderCopy(renderer, texture, NULL , NULL);
        SDL_RenderPresent(renderer);

        Uint32 frameTime = SDL_GetTicks() - currentTime;
        if (frameTime < FRAME_TIME){
            SDL_Delay(FRAME_TIME - frameTime);
        }


    }
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    return 1;
}  


