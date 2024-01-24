#include "cvrptw_typedefs.h"

#define CVRPTW_MAX_POPULATION_SIZE 100

typedef unsigned int cvrptw_chromosome_rating_t;

typedef struct {
    cust_numeric_t custno_index[CVRPTW_MAX_CUSTOMERS];
    cust_numeric_t num_customers_in_route;
} cvrptw_route_t;

typedef struct {
    cvrptw_route_t routes[CVRPTW_MAX_VEHICLES];
    vehicle_numeric_t num_vehicles;
} cvrptw_chromosome_t;

typedef struct {
    cvrptw_chromosome_t chromosomes[CVRPTW_MAX_POPULATION_SIZE];
    cust_numeric_t pop_size;
} cvrptw_population_t;


