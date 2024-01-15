#include "cvrptw_data_get.h"
#include <stdint.h>
#include <SDL2/SDL.h>

void DrawCircle(SDL_Renderer* renderer, int32_t centreX, int32_t centreY, int32_t radius);
void draw_customer_locations(SDL_Renderer* m_window_renderer, cvrptw_problem_t problem, unsigned int window_width, unsigned int window_height, ready_time_t t);
void animate(cvrptw_problem_t problem);
int display_cvrptw_visualization_window();
void load_animation_for_data(char* path);