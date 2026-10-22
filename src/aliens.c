#include "aliens.h"
#include "hashmap.h"
#include <SDL2/SDL_stdinc.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

Alien ** aliens;
AlienBullet * alien_bullets;
int alien_bullet_count = 0;
int alien_start = 0;
int alien_end = ALIEN_COL-1;
int alien_frame = 0;
Uint32 alien_frame_timer = 0;


// set utility 
typedef struct {
    Alien **data;
    size_t size;
    size_t capacity;
} Set;

Set *set_init(void) {
    Set *set = malloc(sizeof *set);
    if (!set) return NULL;

    set->data = NULL;
    set->size = 0;
    set->capacity = 0;

    return set;
}

bool set_contains(const Set *set, const Alien *alien) {
    if (!set || !alien) return false;

    for (size_t i = 0; i < set->size; i++) {
        if (set->data[i] == alien)
            return true;
    }

    return false;
}

bool set_add(Set *set, Alien *alien) {
    if (!set || !alien) return false;

    if (set_contains(set, alien))
        return true;

    if (set->size == set->capacity) {
        size_t new_capacity = set->capacity
            ? set->capacity * 2
            : 4;

        Alien **new_data = realloc(
            set->data,
            new_capacity * sizeof *new_data
        );

        if (!new_data) return false;

        set->data = new_data;
        set->capacity = new_capacity;
    }

    set->data[set->size++] = alien;
    return true;
}

void set_free(Set *set) {
    if (set) {
        free(set->data);
        free(set);
    }
}

void create_aliens(void){
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


typedef struct {
    int x;
    int y;
} Position;

Alien* find_closest_neighbour(int x, int y, Set* set, bool* found) {
    *found = false;

    // Queue for BFS
    Position queue[ALIEN_ROW * ALIEN_COL];
    int head = 0;
    int tail = 0;

    // Validate starting position
    if (x < 0 || x >= ALIEN_ROW ||
        y < 0 || y >= ALIEN_COL) {
        return NULL;
    }

    Alien* start = &aliens[x][y];

    if (set_contains(set, start)) {
        return NULL;
    }

    // Mark as visited when enqueued
    set_add(set, start);
    queue[tail++] = (Position){x, y};

    int dx[] = {1, -1, 0, 0};
    int dy[] = {0, 0, 1, -1};

    while (head < tail) {
        Position pos = queue[head++];
        Alien* current_alien = &aliens[pos.x][pos.y];

        // First living alien encountered is the closest
        if (current_alien->alive) {
            *found = true;
            return current_alien;
        }

        // Enqueue all unvisited neighbours
        for (int i = 0; i < 4; i++) {
            int nx = pos.x + dx[i];
            int ny = pos.y + dy[i];

            if (nx < 0 || nx >= ALIEN_ROW ||
                ny < 0 || ny >= ALIEN_COL) {
                continue;
            }

            Alien* neighbour = &aliens[nx][ny];

            if (set_contains(set, neighbour)) {
                continue;
            }

            set_add(set, neighbour);
            queue[tail++] = (Position){nx, ny};
        }
    }

    // No living alien found
    return NULL;
}

// returns dead aline at 0,0 if no alien alive
Alien* generate_alien_shooter(void){
    int alien_x = (rand() % (alien_end - alien_start + 1)) + alien_start;
    int alien_y = rand() % ALIEN_ROW;

    if (aliens[alien_x][alien_y].alive)
        return &aliens[alien_x][alien_y];

    Set* set_checked_aliens = set_init();
    _Bool* found = false;
    Alien* chosen = find_closest_neighbour(alien_x, alien_y, set_checked_aliens, found);
    set_free(set_checked_aliens);
    return chosen; 
}



// Alien generate_alien_shooter(void)
// {
//     int alien_x = (rand() % (alien_end - alien_start + 1)) + alien_start;
//     int alien_y = rand() % ALIEN_ROW;
//
//     int start_x = alien_x;
//     int start_y = alien_y;
//
//     do {
//         if (aliens[alien_y][alien_x].alive) {
//             return aliens[alien_y][alien_x];
//         }
//
//         alien_x++;
//
//         if (alien_x > alien_end) {
//             alien_x = alien_start;
//
//             alien_y++;
//
//             if (alien_y >= ALIEN_ROW) {
//                 alien_y = 0;
//             }
//         }
//
//     } while (alien_x != start_x || alien_y != start_y);
//
//     // No alive alien was found
//     return (Alien){
//         .x = 0,
//         .y = 0,
//         .alive = false
//     };
// }


bool does_alien_shoot(void){
    int random = rand() % ALIEN_SHOOT_CHANGE;

    if (random == 1) {
        return true;
    }
    return false;
}


void init_alien_bullets(void){
    alien_bullets = malloc(
        sizeof *alien_bullets * MAX_ALIEN_BULLETS
    );

    if (!alien_bullets) {
        printf("ERROR: FAILED ALIEN BULLET INITIALIZATION\n");
        abort();
    }

    for (int i = 0; i < MAX_ALIEN_BULLETS; i++) {
        alien_bullets[i] = (AlienBullet){
            .x = 0,
            .y = 0,
            .alive = false
        };
    }
}


void alien_shoot(void){
    // Don't create more bullets than the maximum
    if (alien_bullet_count >= MAX_ALIEN_BULLETS) {
        return;
    }

    // Find an alien to shoot
    Alien buff = *generate_alien_shooter();

    // No alive alien was found
    if (!buff.alive) {
        return;
    }

    // Find an unused bullet slot
    for (int i = 0; i < MAX_ALIEN_BULLETS; i++) {

        if (!alien_bullets[i].alive) {

            alien_bullets[i].x =
                buff.x + ALIEN_SIZE / 2.0;

            alien_bullets[i].y =
                buff.y + ALIEN_SIZE;

            alien_bullets[i].alive = true;

            alien_bullet_count++;

            return;
        }
    }
}



void update_alien_bullet(SDL_Renderer * renderer){
    for (int i = 0; i<MAX_ALIEN_BULLETS;i++){
        if (!alien_bullets[i].alive){
            continue;
        }
        alien_bullets[i].y += BULLET_SPEED/2.0;
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

void update_alien_bullet_animation(void){
}


void update_alien_length(void){
    bool left_alive = false;
    bool right_alive = false;
    for (int y = 0; y < ALIEN_ROW; y++) {
        if (aliens[y][alien_start].alive) {
            left_alive = true;
        }
    }
    for (int y = 0; y < ALIEN_ROW; y++) {
        if (aliens[y][alien_end].alive) {
            right_alive = true;
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

int alien_direction = 1;
float alien_speed = 21; 
int new_line_flag = 0;
int new_line_distance = 6;
bool update_alien_position(bool new_line){
    if (aliens[0][alien_start].x <= 30 ||aliens[0][alien_end].x >= WIDTH - 60){
        alien_direction *= -1;
        new_line_flag = 1;
        
    }
    
    for (int y=0; y<ALIEN_ROW; y++){ 
        for (int x=alien_start; x<=alien_end; x++){
            aliens[y][x].x += alien_direction*alien_speed;
        }

    if(new_line_flag){
        for (int y=0; y<ALIEN_ROW; y++){ 
            for (int x=alien_start; x<=alien_end; x++){
                aliens[y][x].y += new_line_distance;
            }
        }
    }
    }
    if (new_line_flag){
        new_line_flag = 0;
        return true;
    }
    return false;

}


void render_alien_top(SDL_Renderer *renderer, float ox, float oy, int frame)
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
};


void render_alien_bottom(SDL_Renderer *renderer, float ox, float oy, int frame){
    SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
    ox = (int)ox;
    oy = (int)oy;

    //middle of the alien
    int mx = ox + ALIEN_SIZE / 2;

    rectangle l1 = calculate_square_from_center(mx, oy, 12 , 3 );
    rectangle l2 = calculate_square_from_center(mx, oy,  30, 3 );
    rectangle l3 = calculate_square_from_center(mx, oy, 36, 9 );

    SDL_Rect body[] = {
        { l1.x, oy-0, l1.w, l1.h },
        { l2.x, oy+3, l2.w, l2.h },
        { l3.x, oy+6, l3.w, l3.h },
        
    };

    for (int i = 0; i < 3; i++)
        SDL_RenderFillRect(renderer, &body[i]);
    // pixels are placed left to right
	SDL_Rect legs_frame0[] = {
		{ mx - 12 + 3,  oy + 15, 6, 3 },
		{ mx - 12 + 15, oy + 15, 6, 3 },

		{ mx - 12 + 0,  oy + 18, 6, 3 },
		{ mx - 12 + 9,  oy + 18, 6, 3 },
		{ mx - 12 + 18, oy + 18, 6, 3 },

		{ mx - 12 - 6,  oy + 21, 6, 3 },
		{ mx - 12 + 24, oy + 21, 6, 3 },
	};


	SDL_Rect legs_frame1[] = {
		{ mx - 12 + 3,  oy + 15, 6, 3 },
		{ mx - 12 + 15, oy + 15, 6, 3 },

		{ mx - 12 - 3,  oy + 18, 9, 3 },
		{ mx - 12 + 9,  oy + 18, 6, 3 },
		{ mx - 12 + 18, oy + 18, 9, 3 },

		{ mx - 12 + 3,  oy + 21, 6, 3 },
		{ mx - 12 + 15, oy + 21, 6, 3 },
	};


    

     if (frame == 0){
         for (int i = 0; i < 8; i++)
             SDL_RenderFillRect(renderer, &legs_frame0[i]);
     }
     else if (frame == 1){
         for (int i = 0; i < 8; i++)
             SDL_RenderFillRect(renderer, &legs_frame1[i]);
     }

    

    // color of eyes
    SDL_SetRenderDrawColor(renderer,0,0,20,255);
    SDL_Rect eyes[] = {
        { mx - 12 + 3,  oy + 9, 6, 3 },
        { mx - 12 + 15,  oy + 9, 6, 3 },
    };

    for (int i = 0; i < 2; i++)
        SDL_RenderFillRect(renderer, &eyes[i]);
 
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
};

void render_alien_middle(SDL_Renderer *renderer, float ox, float oy, int frame){
    SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
    ox = (int)ox;
    oy = (int)oy;

    //middle of the alien
    int mx = ox + ALIEN_SIZE / 2;

    rectangle l1 = calculate_square_from_center(mx, oy, 21 , 10 );
    rectangle l2 = calculate_square_from_center(mx, oy,  28, 9 );

    SDL_Rect body[] = {
        { l1.x, oy+6, l1.w, l1.h },
        { l2.x - 1 , oy+9, l2.w, l2.h },
        
    };


    SDL_Rect antena[] = {
        { mx - 12 + 0,  oy + 0, 3, 3 },
        { mx - 12 + 18, oy + 0, 3, 3 },

        { mx - 12 + 3,  oy + 3, 3, 3 },
        { mx - 12 + 15, oy + 3, 3, 3 },
    };

    SDL_Rect arms_frame0[] = {
        { mx - 12 - 6,  oy + 3, 3, 9 },
        { mx - 12 + 24, oy + 3, 3, 9 },
    };
    SDL_Rect arms_frame1[] = {
        { mx - 12 - 6,  oy + 12, 3, 9 },
        { mx - 12 + 24, oy + 12, 3, 9 },
    };
    for (int i = 0; i < 2; i++)
        SDL_RenderFillRect(renderer, &body[i]);
 
    for (int i = 0; i < 4; i++)
        SDL_RenderFillRect(renderer, &antena[i]);     
  
 
    // pixels are placed left to right
	// 
    SDL_Rect legs_frame0[] = {
        { mx - 12 + 0,  oy + 18, 3, 3 },
        { mx - 12 + 18, oy + 18, 3, 3 },

        { mx - 12 - 3,  oy + 21, 3, 3 },
        { mx - 12 + 21,  oy + 21, 3, 3 },

    };


    SDL_Rect legs_frame1[] = {
        { mx - 12 + 0,  oy + 18, 3, 3 },
        { mx - 12 + 18, oy + 18, 3, 3 },

        { mx - 12 + 3,  oy + 21, 6, 3 },
        { mx - 12 + 12,  oy + 21, 6, 3 },

    };

     if (frame == 0){
         for (int i = 0; i < 4; i++)
             SDL_RenderFillRect(renderer, &legs_frame0[i]);
         for (int i = 0; i < 2; i++)
        	SDL_RenderFillRect(renderer, &arms_frame0[i]);
     }
     else if (frame == 1){
         for (int i = 0; i < 4; i++)
             SDL_RenderFillRect(renderer, &legs_frame1[i]);
         for (int i = 0; i < 2; i++)
        	SDL_RenderFillRect(renderer, &arms_frame1[i]);
	}

    

    
    SDL_SetRenderDrawColor(renderer,0,0,20,255);
    SDL_Rect eyes[] = {
        { mx - 12 + 3,  oy + 9, 3, 3 },
        { mx - 12 + 15,  oy + 9, 3, 3 },
    };

    for (int i = 0; i < 2; i++)
        SDL_RenderFillRect(renderer, &eyes[i]);
 
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
};


void render_aliens(SDL_Renderer * renderer){
    
    SDL_SetRenderDrawColor(renderer,255,255,255,255);
    
    for (int y=0; y<ALIEN_ROW; y++){
        for (int x=0; x<=alien_end; x++){
            if (aliens[y][x].alive){
				if (y == 0) {
					render_alien_top(renderer, aliens[y][x].x, aliens[y][x].y, alien_frame);
				}
				else if (y == 1 || y == 2) {
					render_alien_middle(renderer, aliens[y][x].x, aliens[y][x].y, alien_frame);
				}
				else {
					render_alien_bottom(renderer, aliens[y][x].x, aliens[y][x].y, alien_frame);
				}
        
                //SDL_Rect alien_box= {aliens[y][x].x, aliens[y][x].y, ALIEN_SIZE, ALIEN_SIZE };
                //SDL_RenderFillRect(renderer, &alien_box);
                //SDL_RenderDrawRect(renderer, &alien_box);

                
            }
        }
    }
};


