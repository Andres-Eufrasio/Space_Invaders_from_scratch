#include "aliens.h"

/*
todo
change does alien shoot to bool
add all alien species
*/

Alien ** aliens;
AlienBullet * alien_bullets;
int alien_bullet_count = 0;
int alien_start = 0;
int alien_end = ALIEN_COL-1;
int alien_frame = 0;
Uint32 alien_frame_timer = 0;

void create_aliens(){
    int alien_x = ALIEN_X;
    int alien_y = ALIEN_Y;
    
    // change to unsigned char
    aliens = (Alien **)malloc(ALIEN_ROW * sizeof(Alien *));
    if (!aliens){
        printf("Alien allocation error on creation");
        abort();
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

Alien generate_alien_shooter(){
    
    int alien_x = (rand() % alien_end) + alien_start;
    int buff = alien_x;
    while (alien_x <= alien_end){
        
        for (int y=ALIEN_ROW-1; y>=0; y--){
            if (aliens[y][alien_x].alive){
                return aliens[y][alien_x]; 
            }
        }
        alien_x ++;
    }
    alien_x = buff;
    while (alien_x >= alien_start){
        alien_x --;
        for (int y=ALIEN_ROW-1; y>=0; y--){
            if (aliens[y][alien_x].alive){
                return aliens[y][alien_x]; 
            }
        }
    
    }
}



int does_alien_shoot(){
    int random = rand() % ALIEN_SHOOT_CHANGE;
    if (1 == random){
        return 1;
    }
    return 0;
}

void init_alien_bullets(){
    
    alien_bullets = (AlienBullet *)malloc(sizeof(AlienBullet) * MAX_ALIEN_BULLETS);
    if (!alien_bullets){
        printf("ERROR FAILED ALIEN BULLET INITALIZATION");
        abort();
    }
    for (int i=0; i<MAX_ALIEN_BULLETS; i++){
        alien_bullets[i] = (AlienBullet){.x = 0,.y= 0,.alive=false};
    }

}

void alien_shoot(){
    if (alien_bullet_count < MAX_ALIEN_BULLETS){
        for(int i=0; i<MAX_ALIEN_BULLETS; i++){
            if(alien_bullets[i].alive == false){
                alien_bullets[i].alive = true;
                Alien buff = generate_alien_shooter();
                alien_bullets[i].x=buff.x+ALIEN_SIZE/2;
                alien_bullets[i].y=buff.y+ALIEN_SIZE;
                alien_bullets[i].alive=true;
                alien_bullet_count++;
                break;
            }
        }
    }
}

void update_alien_bullet(SDL_Renderer * renderer){
    for (int i = 0; i<MAX_ALIEN_BULLETS;i++){
        if (!alien_bullets[i].alive){
            continue;
        }
        alien_bullets[i].y += BULLET_SPEED/2;
        if (alien_bullets[i].y > HEIGHT - 10){
            alien_bullets[i].alive = false;
            alien_bullet_count--;
            }
        SDL_Rect bullet_box= {alien_bullets[i].x, alien_bullets[i].y, BULLET_WIDTH ,BULLET_HEIGHT };
        SDL_SetRenderDrawColor(renderer,255,255,255,255);
        SDL_RenderFillRect(renderer, &bullet_box);
        SDL_RenderDrawRect(renderer, &bullet_box);
    }
}

void update_alien_bullet_animation(){
    
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
    //call recursively until the right length is created
    if (!left_alive || !right_alive){
        update_alien_length();
    }
    
};

int alien_direction = -1;
float alien_speed = 0.2;
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
    if(new_line_flag){
        for (int y=0; y<ALIEN_ROW; y++){ 
            for (int x=alien_start; x<=alien_end; x++){
                aliens[y][x].y += 20;
            }
            
        }
    }
        new_line_flag = 0;
    }
    

}


void render_triangle_alien(SDL_Renderer *renderer, float ox, float oy, int frame)
{
    SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
    ox = (int)ox;
    oy = (int)oy;

    //middle of the alien
    int mx = ox + ALIEN_SIZE / 2;

    rectangle l1 = calculate_square_from_center(mx, oy, 8 , 10 );
    rectangle l2 = calculate_square_from_center(mx, oy,  14, 10 );
    rectangle l3 = calculate_square_from_center(mx, oy, 20, 10 );
    rectangle l4 = calculate_square_from_center(mx, oy,26, 8);

    SDL_Rect body[] = {
        { l1.x, oy+1, l1.w, l1.h },
        { l2.x, oy+4, l2.w, l2.h },
        { l3.x, oy+6, l3.w, l3.h },
        { l4.x, oy+9, l4.w, l4.h }
    };

    for (int i = 0; i < 4; i++)
        SDL_RenderFillRect(renderer, &body[i]);

    SDL_Rect legs_frame0[] = {
        { mx - 12 + 6,  oy + 17, 3, 3 },
        { mx - 12 + 15, oy + 17, 3, 3 },

        { mx - 12 + 3,  oy + 20, 3, 3 },
        { mx - 12 + 9,  oy + 20, 3, 3 },
        { mx - 12 + 12, oy + 20, 3, 3 },
        { mx - 12 + 18, oy + 20, 3, 3 },

        { mx - 12,      oy + 23, 3, 3 },
        { mx - 12 + 6,  oy + 23, 3, 3 },
        { mx - 12 + 15, oy + 23, 3, 3 },
        { mx - 12 + 21, oy + 23, 3, 3 }
    };


    SDL_Rect legs_frame1[] = {
        { mx - 12 + 3,  oy + 17, 3, 3 },
        { mx - 12 + 9,  oy + 17, 3, 3 },
        { mx - 12 + 12, oy + 17, 3, 3 },
        { mx - 12 + 18,  oy + 17, 3, 3 },

        { mx - 12 + 0,  oy + 20, 3, 3 },
        { mx - 12 + 21,  oy + 20, 3, 3 },


        { mx - 12 + 3,      oy + 23, 3, 3 },
        { mx - 12 + 18,  oy + 23, 3, 3 },

    };

    if (frame == 0){
        for (int i = 0; i < 10; i++)
            SDL_RenderFillRect(renderer, &legs_frame0[i]);
    }
    else if (frame == 1){
        for (int i = 0; i < 8; i++)
            SDL_RenderFillRect(renderer, &legs_frame1[i]);
    }



    // color of eyes
    SDL_SetRenderDrawColor(renderer,0,0,20,255);
    SDL_Rect eyes[] = {
        { mx - 12 + 6,  oy + 12, 3, 3 },
        { mx - 12 + 15,  oy + 12, 3, 3 },
    };

    for (int i = 0; i < 2; i++)
        SDL_RenderFillRect(renderer, &eyes[i]);
 
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
}

void render_aliens(SDL_Renderer * renderer){
    
    SDL_SetRenderDrawColor(renderer,255,255,255,255);
    
    for (int y=0; y<ALIEN_ROW; y++){
        for (int x=0; x<=alien_end; x++){
            if (aliens[y][x].alive){
                
                SDL_Rect alien_box= {aliens[y][x].x, aliens[y][x].y, ALIEN_SIZE, ALIEN_SIZE };
                //SDL_RenderFillRect(renderer, &alien_box);
                //SDL_RenderDrawRect(renderer, &alien_box);

                render_triangle_alien(renderer,aliens[y][x].x,aliens[y][x].y, alien_frame);
                }
        }
    }
};