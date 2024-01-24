#include "visualizer.h"
#include <stdio.h>

#define VISUALIZATION_MODE 1

int main()
{
    #if VISUALIZATION_MODE == 1
    display_cvrptw_visualization_window();
    //load_animation_for_data("../data/r109", "../sol/r109");
    load_animation_for_data("../data/r211", "../sol/r211");
    //load_animation_for_data("../data/c101-25", "../sol/c101-25");
    #endif
    return 0;
}
