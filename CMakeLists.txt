cmake_minimum_required(VERSION 3.0.0)
project(cvrptw_animation VERSION 0.1.0 LANGUAGES C)




add_executable(cvrptw_animation cvrptw_data_get.c visualizer.c main.c)

INCLUDE(FindPkgConfig)

PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)

INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${SDL2_LIBRARIES})