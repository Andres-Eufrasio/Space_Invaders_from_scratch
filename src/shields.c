#include "shield.h"



Shield shields[NUMBER_OF_SHIELDS][SHIELD_WIDTH][SHIELD_HEIGHT];

void kill_shield_top_init()
{
    int shield_units_to_delete = 3;
    const int rows_deleted = 3;
    for (int shield = 0; shield < NUMBER_OF_SHIELDS; shield++) {
        for (int y = 0; y < rows_deleted;    y++) {
            for (int x = 0; x < shield_units_to_delete; x++) {
                shields[shield][x][y].alive = false;
                shields[shield][SHIELD_WIDTH - 1 - x][y].alive = false;
            }
            shield_units_to_delete -= 1;
        }
        shield_units_to_delete = 3; 
    }
}

void kill_shield_bottom_init(){

    const int x_middle = SHIELD_WIDTH/2;
    int left_pointer = x_middle;
    int right_pointer = left_pointer + 1;

    // empty contigous space
    int x_first_pos = 5;
    int x_end_pos = 17;
    for (int shield = 0; shield < NUMBER_OF_SHIELDS; shield++) {
        for (int y = SHIELD_HEIGHT - 1; y > SHIELD_HEIGHT - 5; y--){
            for (int x = x_first_pos; x < x_end_pos; x++)
                shields[shield][x][y].alive = false;
        }
    }
    // empty shortning space
    int x_offset = 1;
    for (int shield = 0; shield < NUMBER_OF_SHIELDS; shield++){
        for (int y = SHIELD_HEIGHT - 5; y > SHIELD_HEIGHT - 9; y--){
            for (int x = x_first_pos+x_offset; x < x_end_pos-x_offset; x++){
                shields[shield][x][y].alive = false;
            }
            x_offset += 1;
        }
        x_offset = 1;
    }
}



void init_shields()
{
    for (int shield = 0; shield < NUMBER_OF_SHIELDS; shield++) {

        int shield_pos_x =
            SHIELD_X_START + shield * (SHIELD_WIDTH * PIXEL_SIZE + SPACE_BETWEEN_SHIELDS);

        for (int x = 0; x < SHIELD_WIDTH; x++) {
            for (int y = 0; y < SHIELD_HEIGHT; y++) {

                shields[shield][x][y].alive = true;

                shields[shield][x][y].x =
                    shield_pos_x + x * PIXEL_SIZE;

                shields[shield][x][y].y =
                    SHIELD_Y_START + y * PIXEL_SIZE;
            }
        }
    }
    kill_shield_top_init();
    kill_shield_bottom_init();
}

void shield_explosion(int shield ,int x, int y){
    for (int i = 0; i<10; i++){
        // random number between -10 and 10
        int random_x = x + (rand()%5) - 4;
        int random_y = y + (rand()%5) - 4;
        if (random_x <= SHIELD_WIDTH && random_x >= 0 &&
            random_y <= SHIELD_HEIGHT&& random_y >= 0){

                    shields[shield][random_x][random_y].alive = false;
            }


    }
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
                            (end_col - start_col + 1) * 3,
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
