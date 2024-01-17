#include "visualizer.h"
#include <SDL2/SDL_ttf.h>
#include <pthread.h>
#include <math.h>

/**
 * @todo Enhance interpolation
 * @todo Color-code time left to serve customer
*/

void DrawCircle(SDL_Renderer* renderer, int32_t centreX, int32_t centreY, int32_t radius) {
    for (int32_t y = -radius; y <= radius; y++) {
        for (int32_t x = -radius; x <= radius; x++) {
            if (x*x + y*y <= radius*radius) {
                SDL_RenderDrawPoint(renderer, centreX + x, centreY + y);
            }
        }
    }
}

/**
 * @source https://discourse.libsdl.org/t/query-how-do-you-draw-a-circle-in-sdl2-sdl2/33379
*/
void DrawHollowCircle(SDL_Renderer* renderer, int32_t centreX, int32_t centreY, int32_t radius) {
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

/**
 * @todo Replace xcoord_t and ycoord_t with int32_t or SDL_Point
*/
void adjust_coordinates(xcoord_t* x, ycoord_t* y, cvrptw_problem_t problem, unsigned int window_width, unsigned int window_height) {
    static unsigned int x_scale = 0;
    static unsigned int y_scale = 0;
    static xcoord_t min_x = 0;
    static xcoord_t max_x = 0;
    static ycoord_t min_y = 0;
    static ycoord_t max_y = 0;
    static int initialized = 0x0;
    
    if (!initialized) {
        initialized = 0x1;
        min_x = problem.depot.xcoord;
        max_x = problem.depot.xcoord;
        min_y = problem.depot.ycoord;
        max_y = problem.depot.ycoord;

        for (cust_numeric_t i = 0; i < problem.num_customers; i++) {
            if (problem.customer_data[i].xcoord < min_x) {
                min_x = problem.customer_data[i].xcoord;
            }
            if (problem.customer_data[i].xcoord > max_x) {
                max_x = problem.customer_data[i].xcoord;
            }
            if (problem.customer_data[i].ycoord < min_y) {
                min_y = problem.customer_data[i].ycoord;
            }
            if (problem.customer_data[i].ycoord > max_y) {
                max_y = problem.customer_data[i].ycoord;
            }
        }

        // Use VISUALIZER_SCENE_MARGIN to add some padding to the scene.
        min_x -= VISUALIZER_SCENE_MARGIN;
        max_x += VISUALIZER_SCENE_MARGIN;
        min_y -= VISUALIZER_SCENE_MARGIN;
        max_y += VISUALIZER_SCENE_MARGIN;

        unsigned int x_range = max_x - min_x;
        unsigned int y_range = max_y - min_y;
        x_scale = window_width / x_range;
        y_scale = window_height / y_range;
    }

    *x = ((*x) - min_x) * x_scale;
    *y = ((*y) - min_y) * y_scale;
}

void draw_customer_locations(SDL_Renderer* m_window_renderer, cvrptw_problem_t problem, unsigned int window_width, unsigned int window_height, ready_time_t t) {
    
    adjust_coordinates(&problem.depot.xcoord, &problem.depot.ycoord, problem, window_width, window_height);
    for (int i = 0; i < problem.num_customers; i++) {
        adjust_coordinates(&(problem.customer_data[i].xcoord), &(problem.customer_data[i].ycoord), problem, window_width, window_height);
    }

    SDL_SetRenderDrawColor(m_window_renderer, 255, 0, 0, 255);
    DrawCircle(m_window_renderer, problem.depot.xcoord, problem.depot.ycoord, VISUALIZER_DEPOT_RADIUS);
    
    #if TIME_WINDOWS_COLOR_CODING == 0
    SDL_SetRenderDrawColor(m_window_renderer, 100, 100, 100, 255);
    #endif

    SDL_SetRenderDrawColor(m_window_renderer, 100, 100, 100, 255);
    #if TIME_WINDOWS_HIDE_CUSTOMERS == 0
    for (int i = 0; i < problem.num_customers; i++) {
        if (problem.customer_data[i].ready_time > t || t > problem.customer_data[i].due_time) {
            int32_t radius = VISUALIZER_CUSTOMER_RADIUS == 0 ? DEMAND_CODING_FACTOR * problem.customer_data[i].demand : VISUALIZER_CUSTOMER_RADIUS;
            if (radius < MIN_CUSTOMER_RADIUS) {
                radius = MIN_CUSTOMER_RADIUS;
            }
            else if (radius > MAX_CUSTOMER_RADIUS) {
                radius = MAX_CUSTOMER_RADIUS;
            }
            #if CUSTOMER_CIRCLE_FILLED == 1
            DrawCircle(m_window_renderer, problem.customer_data[i].xcoord, problem.customer_data[i].ycoord, radius);
            #else
            DrawHollowCircle(m_window_renderer, problem.customer_data[i].xcoord, problem.customer_data[i].ycoord, radius);
            #endif
        }
    }
    #endif
    #if TIME_WINDOWS_COLOR_CODING == 1
    SDL_SetRenderDrawColor(m_window_renderer, 0, 255, 0, 255);
    #endif
    for (int i = 0; i < problem.num_customers; i++) {
        if (problem.customer_data[i].ready_time <= t && t <= problem.customer_data[i].due_time) {
            int32_t radius = VISUALIZER_CUSTOMER_RADIUS == 0 ? DEMAND_CODING_FACTOR * problem.customer_data[i].demand : VISUALIZER_CUSTOMER_RADIUS;
            if (radius < MIN_CUSTOMER_RADIUS) {
                radius = MIN_CUSTOMER_RADIUS;
            }
            else if (radius > MAX_CUSTOMER_RADIUS) {
                radius = MAX_CUSTOMER_RADIUS;
            }
            #if CUSTOMER_CIRCLE_FILLED == 1
            DrawCircle(m_window_renderer, problem.customer_data[i].xcoord, problem.customer_data[i].ycoord, radius);
            #else
            DrawHollowCircle(m_window_renderer, problem.customer_data[i].xcoord, problem.customer_data[i].ycoord, radius);
            #endif
        }
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

/**
 * @todo Get rid of magic numbers
*/
void render_counter(unsigned int counter) {
    // Clear counter area
    SDL_Rect rect;
    rect.x = 0;
    rect.y = 0;
    rect.w = 90;
    rect.h = 30;
    SDL_SetRenderDrawColor(m_window_renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(m_window_renderer, &rect);
    SDL_RenderCopy(m_window_renderer, NULL, &rect, &rect);

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

void animate_problem(cvrptw_problem_t problem) {
    ready_time_t counter = 0;
    due_time_t max_due_time = problem.depot.due_time;
    while(counter <= max_due_time) {
        draw_customer_locations(m_window_renderer, problem, window_width, window_height, counter);
        render_counter(counter);
        SDL_RenderPresent(m_window_renderer);
        SDL_Delay(FRAME_DURATION_MS);
        counter++;
    }
}

typedef struct {
    cvrptw_data_entry_t from_place;
    cvrptw_data_entry_t to_place;
    cust_numeric_t pending_visit_custno;
    vehicle_capacity_t remaining_capacity;
    ready_time_t departured_time;
    ready_time_t expected_arrival_time;
    ready_time_t expected_service_end_time;
    unsigned char en_route_to_depot;
    unsigned char finished_work; // union with en_route_to_depot ?
} vehicle_info_t;

/**
 * Assumes that all vehicles move at the same speed = 1.
*/
distance_t get_travel_duration(cvrptw_data_entry_t e1, cvrptw_data_entry_t e2) {
    return sqrt((e1.xcoord - e2.xcoord)*(e1.xcoord - e2.xcoord) + (e1.ycoord - e2.ycoord)*(e1.ycoord - e2.ycoord));
}

/**
 * @note This function initializes its static variables only once.
 * @note This function assumes that the solution is feasible.
 * @note This function assumes that each vehicle has at least one customer to visit.
 * @note This function assumes that each vehicle starts from the depot.
 * @note This function assumes that each vehicle has enough capacity to serve all its customers.
 * @note This function assumes that all vehicles start at time 0.
*/
void draw_vehicle_locations(SDL_Renderer* m_windows_renderer, cvrptw_problem_t problem, cvrptw_solution_t sol, unsigned int window_width, unsigned int window_height, ready_time_t t) {
    static vehicle_info_t __vehicle_info[CVRPTW_MAX_VEHICLES];
    static vehicle_info_t* vehicle_info_p = NULL;
    if (vehicle_info_p == NULL) {
        vehicle_info_p = __vehicle_info;
        for (vehicle_numeric_t i = 0; i < sol.num_vehicles; i++) {
            vehicle_info_p[i].from_place = problem.depot;
            vehicle_info_p[i].to_place = problem.customer_data[sol.custno_index[sol.route_head_index[i]]];
            vehicle_info_p[i].pending_visit_custno = sol.route_head_index[i];
            vehicle_info_p[i].remaining_capacity = problem.vehicle_capacity;
            vehicle_info_p[i].departured_time = 0;
            vehicle_info_p[i].expected_arrival_time = get_travel_duration(problem.depot, problem.customer_data[sol.custno_index[sol.route_head_index[i]]]);
            vehicle_info_p[i].expected_service_end_time = problem.customer_data[sol.custno_index[sol.route_head_index[i]]].ready_time + problem.customer_data[sol.custno_index[sol.route_head_index[i]]].service_time;
            vehicle_info_p[i].en_route_to_depot = 0x0;
            vehicle_info_p[i].finished_work = 0x0;
            if (vehicle_info_p[i].expected_arrival_time > problem.customer_data[sol.custno_index[sol.route_head_index[i]]].due_time) {
                perror("Error: Vehicle cannot reach customer before due time!");
                exit(EXIT_FAILURE);
            }
            if (vehicle_info_p[i].remaining_capacity < problem.customer_data[sol.custno_index[sol.route_head_index[i]]].demand) {
                perror("Error: Vehicle does not have enough capacity to serve customer!");
                exit(EXIT_FAILURE);
            }
            if (vehicle_info_p[i].expected_service_end_time > problem.customer_data[sol.custno_index[sol.route_head_index[i]]].due_time) {
                perror("Error: Too little time left to serve the customer in time window!");
                exit(EXIT_FAILURE);
            }
        }
    }
    

    xcoord_t vehicle_x = 0;
    ycoord_t vehicle_y = 0;
    for (vehicle_numeric_t i = 0; i < sol.num_vehicles; i++) {
        if (__vehicle_info[i].finished_work == 0x1) {
            continue;
        }
        
        if (t >= __vehicle_info[i].expected_arrival_time) {
            vehicle_x = __vehicle_info[i].to_place.xcoord;
            vehicle_y = __vehicle_info[i].to_place.ycoord;
        }
        else {
            double travel_ratio = 1 - ((double)vehicle_info_p[i].expected_arrival_time - (double)t) / ((double)vehicle_info_p[i].expected_arrival_time - (double)vehicle_info_p[i].departured_time);
            //Use the travel ratio to interpolate between customer locations.
            double x = (double)(__vehicle_info[i].from_place.xcoord) + travel_ratio * ((double)__vehicle_info[i].to_place.xcoord - (double)__vehicle_info[i].from_place.xcoord);
            double y = (double)(__vehicle_info[i].from_place.ycoord) + travel_ratio * ((double)__vehicle_info[i].to_place.ycoord - (double)__vehicle_info[i].from_place.ycoord);
            vehicle_x = x;
            vehicle_y = y;
            //printf("Travel ratio: %f\n", travel_ratio);
        }
        
        //printf("Vehicle %u: (%u, %u) dep=%u arr=%u from=(%u, %u) to=(%u, %u)\n", i, vehicle_x, vehicle_y, __vehicle_info[i].departured_time, __vehicle_info[i].expected_arrival_time, __vehicle_info[i].from_place.xcoord, __vehicle_info[i].from_place.ycoord, __vehicle_info[i].to_place.xcoord, __vehicle_info[i].to_place.ycoord);
        SDL_SetRenderDrawColor(m_windows_renderer, 0, 0, 255, 255);
        adjust_coordinates(&vehicle_x, &vehicle_y, problem, window_width, window_height);
        DrawCircle(m_windows_renderer, vehicle_x, vehicle_y, VISUALIZER_VEHICLE_RADIUS);

        /*if (t >= __vehicle_info[i].expected_arrival_time) {
            
        }*/

        /*if (t == __vehicle_info[i].expected_arrival_time) {
            // Update vehicle info
            __vehicle_info[i].from_place = __vehicle_info[i].to_place;
            __vehicle_info[i].pending_visit_custno++;
            __vehicle_info[i].to_place = problem.customer_data[sol.custno_index[__vehicle_info[i].pending_visit_custno]];
            __vehicle_info[i].departure_time = __vehicle_info[i].expected_arrival_time;
            __vehicle_info[i].expected_arrival_time = __vehicle_info[i].departure_time + get_travel_duration(__vehicle_info[i].from_place, __vehicle_info[i].to_place);
            if (__vehicle_info[i].expected_arrival_time > __vehicle_info[i].to_place.due_time) {
                perror("Error: Vehicle cannot reach customer before due time!");
                exit(EXIT_FAILURE);
            }
            if (__vehicle_info[i].remaining_capacity < __vehicle_info[i].to_place.demand) {
                perror("Error: Vehicle does not have enough capacity to serve customer!");
                exit(EXIT_FAILURE);
            }
            __vehicle_info[i].remaining_capacity -= __vehicle_info[i].to_place.demand;
            
        }*/
        if (t >= __vehicle_info[i].expected_service_end_time) {
            if (__vehicle_info[i].en_route_to_depot == 0x1) {
                __vehicle_info[i].finished_work = 0x1;
                printf("Vehicle %u finished work at time %u\n", i, t);
            }

            else {
                // Vehicle has finished serving the customer.
                // Move to the next customer.
                __vehicle_info[i].remaining_capacity -= __vehicle_info[i].to_place.demand;
                if (__vehicle_info[i].remaining_capacity > problem.vehicle_capacity) {
                    perror("Error: Vehicle has invalid capacity. Trying to serve more than its capacity allows?");
                    exit(EXIT_FAILURE);
                }
                __vehicle_info[i].from_place = __vehicle_info[i].to_place;

                
                ////////
                if (__vehicle_info[i].pending_visit_custno + 1 == sol.num_customers_in_route[i]) {
                    // Vehicle has finished its route.
                    // Move to the depot.
                    __vehicle_info[i].to_place = problem.depot;
                    __vehicle_info[i].en_route_to_depot = 0x1;
                }
                else {
                    // Vehicle has not finished its route.
                    // Move to the next customer.
                    __vehicle_info[i].pending_visit_custno++;
                    printf("Vehicle %u, new custno=(index: %u, value: %u)\n", i, __vehicle_info[i].pending_visit_custno, sol.custno_index[__vehicle_info[i].pending_visit_custno]);
                    __vehicle_info[i].to_place = problem.customer_data[sol.custno_index[__vehicle_info[i].pending_visit_custno]];
                }
                ///////

                __vehicle_info[i].departured_time = t; //accuracy?
                __vehicle_info[i].expected_arrival_time = __vehicle_info[i].departured_time + get_travel_duration(__vehicle_info[i].from_place, __vehicle_info[i].to_place);
                __vehicle_info[i].expected_service_end_time = __vehicle_info[i].expected_arrival_time + __vehicle_info[i].to_place.service_time;
                if (__vehicle_info[i].expected_arrival_time > __vehicle_info[i].to_place.due_time) {
                    perror("Error: Vehicle cannot reach customer before due time!");
                    char perror_buffer[300];
                    snprintf(perror_buffer, 300, "from_custno: %u to_custno: %u, time shortage: %u", __vehicle_info[i].from_place.cust_no, __vehicle_info[i].to_place.cust_no, __vehicle_info[i].expected_arrival_time - __vehicle_info[i].to_place.due_time);
                    //sprintf("from_custno: %u to_custno: %u", __vehicle_info[i].from_place.cust_no, __vehicle_info[i].to_place.cust_no);
                    perror(perror_buffer);
                    ////exit(EXIT_FAILURE);
                }
                if (__vehicle_info[i].remaining_capacity < __vehicle_info[i].to_place.demand) {
                    perror("Error: Vehicle does not have enough capacity to serve customer!");
                    char perror_buffer[300];
                    snprintf(perror_buffer, 300, "from_custno: %u to_custno: %u, capacity shortage: %u", __vehicle_info[i].from_place.cust_no, __vehicle_info[i].to_place.cust_no, __vehicle_info[i].to_place.demand - __vehicle_info[i].remaining_capacity);
                    perror(perror_buffer);
                    ////exit(EXIT_FAILURE);
                }
                if (__vehicle_info[i].expected_service_end_time > __vehicle_info[i].to_place.due_time) {
                    perror("!!! Error: Too little time left to serve the customer in time window!");
                    char perror_buffer[300];
                    snprintf(perror_buffer, 300, "veh_id: %u from_custno: %u to_custno: %u, time shortage: %u", i, __vehicle_info[i].from_place.cust_no, __vehicle_info[i].to_place.cust_no, __vehicle_info[i].expected_service_end_time - __vehicle_info[i].to_place.due_time);
                    perror(perror_buffer);
                    ////exit(EXIT_FAILURE);
                }
            }
        }
    }
}

/**
 * @todo Add support for solution
*/
void animate_problem_with_solution(cvrptw_problem_t problem, cvrptw_solution_t solution) {
    ready_time_t counter = 0;
    due_time_t max_due_time = problem.depot.due_time;
    while(counter <= max_due_time) {
        draw_customer_locations(m_window_renderer, problem, window_width, window_height, counter);
        draw_vehicle_locations(m_window_renderer, problem, solution, window_width, window_height, counter);
        render_counter(counter);
        SDL_RenderPresent(m_window_renderer);
        SDL_Delay(FRAME_DURATION_MS);
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

void load_animation_for_data(char* problem_path, char* sol_path) {
    if (problem_path == NULL) {
        perror("Error: problem_path is NULL");
        exit(EXIT_FAILURE);
    }
    
    cvrptw_problem_t problem = cvrptw_data_get(problem_path);
    if (sol_path == NULL) {
        animate_problem(problem);
    }
    else {
        cvrptw_solution_t solution = cvrptw_solution_get(sol_path);
        if (solution.num_vehicles == 0) {
            perror("Error: solution.num_vehicles == 0");
            exit(EXIT_FAILURE);
        }
        else if (solution.total_num_customers != problem.num_customers) {
            perror("Error: Invalid solution file for given problem file!");
            exit(EXIT_FAILURE);
        }
        animate_problem_with_solution(problem, solution);   
    }
    
}