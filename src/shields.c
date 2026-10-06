#include "shield.h"

#define SHIELD_HEIGHT 16
#define SHIELD_WIDTH  22
#define NUMBER_OF_SHIELDS  4
#define SPACE_BETWEEN_SHIELDS 100
#define PIXEL_SIZE 3
#define SHIELD_X_START 115
#define SHIELD_Y_START 470
Shield shields[NUMBER_OF_SHIELDS][SHIELD_WIDTH][SHIELD_HEIGHT];

void kill_shield_top_init(){

    
    for (int shield = 0; shield < NUMBER_OF_SHIELDS; shield++) {
        int shield_units_to_delete = 3;
        for (int y = 0; y < 3; y++) {
                
            for (int x = 0; x < 3; x++) {

                shields[shield][x][y].alive = false;
                shields[shield][(SHIELD_WIDTH-1) -x][y].alive = false;
            }
        shield_units_to_delete -= 1;
        }
    
    }
}

void kill_shield_bottom_init(){
     for (int shield = 0; shield < NUMBER_OF_SHIELDS; shield++) {
        int shield_units_to_delete = 3;
        for (int y = SHIELD_HEIGHT; y > 3; y++) {
                
            for (int x = 0; x < 3; x++) {

                shields[shield][x][y].alive = false;
                shields[shield][(SHIELD_WIDTH-1) -x][y].alive = false;
            }
        shield_units_to_delete -= 1;
        }
    
    }    
}



void init_shields()
{
    int shield_pos_x = SHIELD_X_START;
    int shield_pos_y = SHIELD_Y_START;

    for (int shield = 0; shield < NUMBER_OF_SHIELDS; shield++) {

        for (int x = 0; x < SHIELD_WIDTH; x++) {

            for (int y = 0; y < SHIELD_HEIGHT; y++) {

                shields[shield][x][y].alive = true;
                shields[shield][x][y].x = shield_pos_x;
                shields[shield][x][y].y = shield_pos_y;

                shield_pos_y += PIXEL_SIZE;
            }

            shield_pos_x += PIXEL_SIZE;
        }

        shield_pos_x += SPACE_BETWEEN_SHIELDS;
        shield_pos_y = SHIELD_Y_START;
    }
    kill_shield_top_init();
}






/*
loop through shields and create and render line for each contigous line going down
*/
void render_shields(SDL_Renderer *renderer)
{
    for (int shield = 0; shield < NUMBER_OF_SHIELDS; shield++) {

        for (int y = 0; y < SHIELD_HEIGHT; y++) {

            int start_col = -1;

            for (int x = 0; x < SHIELD_WIDTH; x++) {

                if (shields[shield][x][y].alive) {

                    // Start of a new run
                    if (start_col == -1) {
                        start_col = x;
                    }

                } else {

                    // End of a run
                    if (start_col != -1) {

                        int end_col = x - 1;

                        SDL_Rect rect = {
                            shields[shield][start_col][y].x,
                            shields[shield][start_col][y].y,
                            (end_col - start_col + 9) * 3,
                            3
                        };

                        SDL_RenderFillRect(renderer, &rect);

                        start_col = -1;
                    }
                }
            }

            // Render run reaching the end of the row
            if (start_col != -1) {

                int end_col = SHIELD_WIDTH - 1;

                SDL_Rect rect = {
                    shields[shield][start_col][y].x,
                    shields[shield][start_col][y].y,
                    (end_col - start_col + 1) * 3, 3
                };

                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }
}
