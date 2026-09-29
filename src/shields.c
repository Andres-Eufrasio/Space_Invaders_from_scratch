#include "shield.h"

#define SHIELD_HEIGHT 16
#define SHIELD_WIDTH  22

Shield shields[3][SHIELD_WIDTH][SHIELD_HEIGHT];
// todo remove bits off the top side and bottom middle
void init_shields(void)
{
    int shield_pos_x = 50;
    int shield_pos_y = 400;

    for (int shield = 0; shield < 3; shield++) {

        for (int x = 0; x < SHIELD_WIDTH; x++) {

            for (int y = 0; y < SHIELD_HEIGHT; y++) {

                shields[shield][x][y].alive = true;
                shields[shield][x][y].x = shield_pos_x;
                shields[shield][x][y].y = shield_pos_y;

                shield_pos_y += 2;
            }

            shield_pos_y = 1;
            shield_pos_x += 2;
        }

        shield_pos_x += 300;
    }
}
/*
loop through shields and create and render line for each contigous line going down
*/
void draw_shields(SDL_Renderer *renderer)
{
    for (int shield = 0; shield < 3; shield++) {

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
                            (end_col - start_col + 1) * 2,
                            2
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
                    (end_col - start_col + 1) * 2, 2
                };

                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }
}