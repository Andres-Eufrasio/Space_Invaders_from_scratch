#include "aliens.h"

#include <SDL2/SDL_stdinc.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>


Alien **aliens = NULL;
AlienBullet *alien_bullets = NULL;

int alien_bullet_count = 0;
int alien_start = 0;
int alien_end = ALIEN_COL - 1;
int alien_frame = 0;
Uint32 alien_frame_timer = 0;



typedef struct {
    Alien **data;
    size_t size;
    size_t capacity;
} Set;


static Set *set_init(void)
{
    Set *set = malloc(sizeof *set);

    if (set == NULL)
        return NULL;

    set->data = NULL;
    set->size = 0;
    set->capacity = 0;

    return set;
}


static bool set_contains(const Set *set, const Alien *alien)
{
    if (set == NULL || alien == NULL)
        return false;

    for (size_t i = 0; i < set->size; i++) {
        if (set->data[i] == alien)
            return true;
    }

    return false;
}


static bool set_add(Set *set, Alien *alien)
{
    if (set == NULL || alien == NULL)
        return false;

    if (set_contains(set, alien))
        return true;

    if (set->size == set->capacity) {
        size_t new_capacity = set->capacity == 0
            ? 4
            : set->capacity * 2;

        /* Prevent capacity arithmetic overflow. */
        if (new_capacity < set->capacity ||
            new_capacity > (size_t)-1 / sizeof *set->data) {
            return false;
        }

        Alien **new_data = realloc(
            set->data,
            new_capacity * sizeof *new_data
        );

        if (new_data == NULL)
            return false;

        set->data = new_data;
        set->capacity = new_capacity;
    }

    set->data[set->size++] = alien;
    return true;
}


static void set_free(Set *set)
{
    if (set != NULL) {
        free(set->data);
        free(set);
    }
}




void create_aliens(void)
{
    int alien_x = ALIEN_X;
    int alien_y = ALIEN_Y;


    if (aliens != NULL) {
        for (int row = 0; row < ALIEN_ROW; row++)
            free(aliens[row]);

        free(aliens);
        aliens = NULL;
    }

    aliens = calloc(ALIEN_ROW, sizeof *aliens);

    if (aliens == NULL) {
        fprintf(stderr, "Alien allocation error on creation\n");
        abort();
    }

    for (int row = 0; row < ALIEN_ROW; row++) {
        aliens[row] = malloc(ALIEN_COL * sizeof *aliens[row]);

        if (aliens[row] == NULL) {
            for (int i = 0; i < row; i++)
                free(aliens[i]);

            free(aliens);
            aliens = NULL;

            fprintf(stderr, "Alien row allocation error\n");
            abort();
        }

        for (int col = 0; col < ALIEN_COL; col++) {
            aliens[row][col] = (Alien){
                alien_x,
                alien_y,
                true
            };
            if (row == 0) aliens[row][col].species = top;
            else if (row > 2) aliens[row][col].species = bottom;
            else aliens[row][col].species = middle;
            


            alien_x += ALIEN_SPACE_X;
        }

        alien_y += ALIEN_SPACE_Y;
        alien_x = ALIEN_X;
    }

    alien_start = 0;
    alien_end = ALIEN_COL - 1;
}




typedef struct {
    int row;
    int col;
} Position;



static Alien *find_closest_neighbour(int start_row, int start_col)
{
    if (aliens == NULL)
        return NULL;

    if (start_row < 0 || start_row >= ALIEN_ROW ||
        start_col < 0 || start_col >= ALIEN_COL) {
        return NULL;
    }

    const size_t max_aliens = (size_t)ALIEN_ROW * ALIEN_COL;

    if (ALIEN_ROW <= 0 || ALIEN_COL <= 0)
        return NULL;

    Position *queue = malloc(max_aliens * sizeof *queue);

    if (queue == NULL)
        return NULL;

    Set *visited = set_init();

    if (visited == NULL) {
        free(queue);
        return NULL;
    }

    size_t head = 0;
    size_t tail = 0;

    Alien *start = &aliens[start_row][start_col];

    if (!set_add(visited, start)) {
        set_free(visited);
        free(queue);
        return NULL;
    }

    queue[tail++] = (Position){start_row, start_col};

    const int dr[] = {1, -1, 0, 0};
    const int dc[] = {0, 0, 1, -1};

    while (head < tail) {
        Position pos = queue[head++];

        Alien *current = &aliens[pos.row][pos.col];

        /* BFS guarantees the first living alien is nearest. */
        if (current->alive) {
            set_free(visited);
            free(queue);
            return current;
        }

        for (int i = 0; i < 4; i++) {
            int next_row = pos.row + dr[i];
            int next_col = pos.col + dc[i];

            if (next_row < 0 || next_row >= ALIEN_ROW ||
                next_col < 0 || next_col >= ALIEN_COL) {
                continue;
            }

            Alien *neighbour = &aliens[next_row][next_col];

            if (set_contains(visited, neighbour))
                continue;

            if (!set_add(visited, neighbour)) {
                set_free(visited);
                free(queue);
                return NULL;
            }

            queue[tail++] = (Position){next_row, next_col};
        }
    }

    set_free(visited);
    free(queue);

    return NULL;
}

Alien *generate_alien_shooter(void)
{
    if (aliens == NULL)
        return NULL;

    if (ALIEN_ROW <= 0 || ALIEN_COL <= 0)
        return NULL;
    // bound check
    int start = alien_start;
    int end = alien_end;

    if (start < 0)
        start = 0;

    if (end >= ALIEN_COL)
        end = ALIEN_COL - 1;

    if (start > end)
        return NULL;

    int col = start + rand() % (end - start + 1);
    int row = rand() % ALIEN_ROW;

    Alien *selected = &aliens[row][col];

    if (selected->alive)
        return selected;

    return find_closest_neighbour(row, col);
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


