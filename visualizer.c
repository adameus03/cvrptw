#include "visualizer.h"
#include <SDL2/SDL_ttf.h>
#include <pthread.h>

/**
 * @source https://discourse.libsdl.org/t/query-how-do-you-draw-a-circle-in-sdl2-sdl2/33379
*/
void DrawCircle(SDL_Renderer* renderer, int32_t centreX, int32_t centreY, int32_t radius) {
    const int32_t diameter = (radius * 2);

    int32_t x = (radius - 1);
    int32_t y = 0;
    int32_t tx = 1;
    int32_t ty = 1;
    int32_t error = (tx - diameter);

    while (x >= y) {
        // Each of the following renders an octant of the circle
        SDL_RenderDrawPoint(renderer, centreX + x, centreY - y);
        SDL_RenderDrawPoint(renderer, centreX + x, centreY + y);
        SDL_RenderDrawPoint(renderer, centreX - x, centreY - y);
        SDL_RenderDrawPoint(renderer, centreX - x, centreY + y);
        SDL_RenderDrawPoint(renderer, centreX + y, centreY - x);
        SDL_RenderDrawPoint(renderer, centreX + y, centreY + x);
        SDL_RenderDrawPoint(renderer, centreX - y, centreY - x);
        SDL_RenderDrawPoint(renderer, centreX - y, centreY + x);

        if (error <= 0) {
      	    ++y;
      	    error += ty;
      	    ty += 2;
        }

        if (error > 0) {
      	    --x;
      	    tx += 2;
      	    error += (tx - diameter);
        }
    }
}

void draw_customer_locations(SDL_Renderer* m_window_renderer, cvrptw_problem_t problem, unsigned int window_width, unsigned int window_height, ready_time_t t) {
    SDL_RenderClear(m_window_renderer);

    SDL_Rect rect;
    rect.x = 0;
    rect.y = 0;
    rect.w = window_width;
    rect.h = window_height;

    SDL_SetRenderDrawColor(m_window_renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(m_window_renderer, &rect);

    xcoord_t min_x;
    xcoord_t max_x;
    ycoord_t min_y;
    ycoord_t max_y;
    for (cust_numeric_t i = 0; i < problem.num_customers; i++) {
        if (i == 0) {
            min_x = problem.data[i].xcoord;
            max_x = problem.data[i].xcoord;
            min_y = problem.data[i].ycoord;
            max_y = problem.data[i].ycoord;
        } else {
            if (problem.data[i].xcoord < min_x) {
                min_x = problem.data[i].xcoord;
            }
            if (problem.data[i].xcoord > max_x) {
                max_x = problem.data[i].xcoord;
            }
            if (problem.data[i].ycoord < min_y) {
                min_y = problem.data[i].ycoord;
            }
            if (problem.data[i].ycoord > max_y) {
                max_y = problem.data[i].ycoord;
            }
        }
    }

    // Use VISUALIZER_SCENE_MARGIN to add some padding to the scene.
    min_x -= VISUALIZER_SCENE_MARGIN;
    max_x += VISUALIZER_SCENE_MARGIN;
    min_y -= VISUALIZER_SCENE_MARGIN;
    max_y += VISUALIZER_SCENE_MARGIN;

    unsigned int x_range = max_x - min_x;
    unsigned int y_range = max_y - min_y;
    unsigned int x_scale = window_width / x_range;
    unsigned int y_scale = window_height / y_range;
    for (int i = 0; i < problem.num_customers; i++) {
        problem.data[i].xcoord = (problem.data[i].xcoord - min_x) * x_scale;
        problem.data[i].ycoord = (problem.data[i].ycoord - min_y) * y_scale;
    }

    SDL_SetRenderDrawColor(m_window_renderer, 255, 0, 0, 255);
    DrawCircle(m_window_renderer, problem.data[0].xcoord, problem.data[0].ycoord, VISUALIZER_DEPOT_RADIUS);
    
    SDL_SetRenderDrawColor(m_window_renderer, 0, 255, 0, 255);
    for (int i = 1; i < problem.num_customers; i++) {
        if (problem.data[i].ready_time <= t && t <= problem.data[i].due_time) {
            SDL_SetRenderDrawColor(m_window_renderer, 0, 255, 0, 255);
        } else {
            SDL_SetRenderDrawColor(m_window_renderer, 100, 100, 100, 255);
        }
        DrawCircle(m_window_renderer, problem.data[i].xcoord, problem.data[i].ycoord, VISUALIZER_CUSTOMER_RADIUS == 0 ? problem.data[i].demand : VISUALIZER_CUSTOMER_RADIUS);
    }

}

SDL_Window   *m_window          = NULL;
SDL_Renderer *m_window_renderer = NULL;
SDL_Event     m_window_event;
unsigned int window_width = VISUALIZER_WINDOW_WIDTH;
unsigned int window_height = VISUALIZER_WINDOW_HEIGHT;

void* eventListenerThreadHandler(void* arg_p) {
    while(1)
    {
        SDL_WaitEvent(&m_window_event);
        //while(SDL_PollEvent(&m_window_event) > 0)
        switch(m_window_event.type) {
            case SDL_QUIT:
                SDL_DestroyRenderer(m_window_renderer);
                SDL_DestroyWindow(m_window);
                exit(0);
        }
        //update(1.0/60.0, &x, &y);
        //draw(m_window_renderer, x, y);
    }
}

void render_counter(unsigned int counter) {
    TTF_Font* Sans = TTF_OpenFont("../assets/fonts/OpenSans-SemiboldItalic.ttf", 24);
    if (Sans == NULL) {
        printf("TTF_OpenFont: %s\n", TTF_GetError());
        return;
    }

    SDL_Color White = {255, 255, 255};
    char text[20];
    sprintf(text, "Time: %u", counter);
    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(Sans, text, White);
    SDL_Texture* Message = SDL_CreateTextureFromSurface(m_window_renderer, surfaceMessage);
    SDL_Rect Message_rect; //create a rect
    Message_rect.x = 0;  //controls the rect's x coordinate 
    Message_rect.y = 0; // controls the rect's y coordinte
    Message_rect.w = 90; // controls the width of the rect
    Message_rect.h = 30; // controls the height of the rect
    //Message_rect.w = surfaceMessage->w;
    //Message_rect.h = surfaceMessage->h;
    SDL_RenderCopy(m_window_renderer, Message, NULL, &Message_rect);
    //SDL_RenderPresent(m_window_renderer);
    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(Message);
}

void animate(cvrptw_problem_t problem) {
    ready_time_t counter = 0;
    due_time_t max_due_time = problem.data[0].due_time;
    while(counter <= max_due_time) {
        draw_customer_locations(m_window_renderer, problem, window_width, window_height, counter);
        render_counter(counter);
        SDL_RenderPresent(m_window_renderer);
        SDL_Delay(100);
        counter++;
    }
}

int init_ttf() {
    if (TTF_Init() < 0) {
        printf("TTF_Init: %s\n", TTF_GetError());
        return 1;
    }
    return 0;
}

int display_cvrptw_visualization_window() {

    m_window = SDL_CreateWindow("CVRPTW Visualizer",
                                0/*SDL_WINDOWPOS_CENTERED*/,
                                0/*SDL_WINDOWPOS_CENTERED*/,
                                VISUALIZER_WINDOW_WIDTH, VISUALIZER_WINDOW_HEIGHT,
                                0);

    if(m_window == NULL)
    {
        printf("Failed to create window\n");
        printf("SDL2 Error: %s\n", SDL_GetError());
        return 1;
    }

    //unsigned int window_width = VISUALIZER_WINDOW_WIDTH;
    //unsigned int window_height = VISUALIZER_WINDOW_HEIGHT;
    if (VISUALIZER_WINDOW_WIDTH == 0 || VISUALIZER_WINDOW_HEIGHT == 0) {
        SDL_DisplayMode dm;
        if (SDL_GetDesktopDisplayMode(0, &dm))
        {
            printf("Error getting desktop display mode\n");
            return -1;
        }
        window_width = VISUALIZER_WINDOW_WIDTH == 0 ? dm.w : VISUALIZER_WINDOW_WIDTH;
        window_height = VISUALIZER_WINDOW_HEIGHT == 0 ? dm.h : VISUALIZER_WINDOW_HEIGHT;;
    }
    
    SDL_SetWindowSize(m_window, window_width, window_height);

    
    m_window_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if(m_window_renderer == NULL)
    {
        printf("Failed to create renderer\n");
        printf("SDL2 Error: %s\n", SDL_GetError());
        return 1;
    }

    init_ttf();
    
    pthread_t eventListenerTID;
    pthread_create(&eventListenerTID, NULL, eventListenerThreadHandler, NULL);

    return 0;
}

void load_animation_for_data(char* path) {
    cvrptw_problem_t problem = cvrptw_data_get(path);
    animate(problem);
}