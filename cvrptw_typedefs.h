#include "config.h"

typedef unsigned int cust_numeric_t;
typedef unsigned int vehicle_numeric_t;
typedef unsigned int vehicle_capacity_t;
typedef unsigned int xcoord_t;
typedef unsigned int ycoord_t;
typedef unsigned int demand_t;
typedef unsigned int ready_time_t;
typedef unsigned int due_time_t;
typedef unsigned int service_time_t;
typedef unsigned int distance_t;
typedef double distance_ratio_t;

typedef struct {
    cust_numeric_t cust_no;
    xcoord_t xcoord;
    ycoord_t ycoord;
    demand_t demand;
    ready_time_t ready_time;
    due_time_t due_time;
    service_time_t service_time;
} cvrptw_data_entry_t;

typedef struct {
    cust_numeric_t num_customers;
    cvrptw_data_entry_t depot;
    cvrptw_data_entry_t customer_data[CVRPTW_MAX_CUSTOMERS];
    vehicle_capacity_t vehicle_capacity;
} cvrptw_problem_t;

typedef struct {
    cust_numeric_t custno_index[CVRPTW_MAX_CUSTOMERS];
    cust_numeric_t route_head_index[CVRPTW_MAX_VEHICLES];
    vehicle_numeric_t num_vehicles;
    cust_numeric_t total_num_customers;
    cust_numeric_t num_customers_in_route[CVRPTW_MAX_VEHICLES];
} cvrptw_solution_t;

/*typedef struct {
    cust_numeric_t custno_index[CVRPTW_MAX_CUSTOMERS];
    cust_numeric_t route_head_index[CVRPTW_MAX_VEHICLES];
    vehicle_numeric_t num_vehicles;
    cust_numeric_t num_customers_in_route[CVRPTW_MAX_VEHICLES];
} cvrptw_trunc_solution_t;*/