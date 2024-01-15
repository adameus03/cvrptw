#include "visualizer.h"
#include <stdio.h>

int main()
{
    display_cvrptw_visualization_window();
    //load_data("../data/r101");
    //animate();
    
    load_animation_for_data("../data/r101");
    return 0;
}
