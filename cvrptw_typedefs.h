#include "config.h"

typedef unsigned int cust_numeric_t;
typedef unsigned int xcoord_t;
typedef unsigned int ycoord_t;
typedef unsigned int demand_t;
typedef unsigned int ready_time_t;
typedef unsigned int due_time_t;
typedef unsigned int service_time_t;

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
    cvrptw_data_entry_t data[CVRPTW_MAX_CUSTOMERS];
} cvrptw_problem_t;