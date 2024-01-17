#include "cvrptw_data_get.h"
#include "config.h"
#include <stdio.h>
#include <stdlib.h> // atoi
#include <string.h> // strtok


/**
 * @brief Loads CVRPTW problem data from a file.
 * @param path The path to the file containing the data.
 * @note Each line in the file should contain the following data, separated by a single space:
 * 1. Customer number
 * 2. X coordinate
 * 3. Y coordinate
 * 4. Demand
 * 5. Ready time
 * 6. Due time
 * 7. Service time
 * @return The CVRPTW problem data.
 * @note The function assumes that the file is formatted correctly.
 * @note The function assumes that the file contains no more than CVRPTW_MAX_CUSTOMERS+1 lines
 * @note The first line of the file should contain the depot data.
 * @note The function assumes that file lines are no longer than MAX_CVRPTW_DATA_FILE_LINE_LENGTH characters.
*/
cvrptw_problem_t cvrptw_data_get(char* path) {
    cvrptw_problem_t problem;
    FILE* file = fopen(path, "r");
    //fscanf(file, "%u", &problem.num_customers);
    problem.num_customers = 0;
    
    char line[MAX_CVRPTW_DATA_FILE_LINE_LENGTH];
    
    if (NULL != fgets(line, MAX_CVRPTW_DATA_FILE_LINE_LENGTH, file)) {
        sscanf(line, "%u", &problem.vehicle_capacity);
    }
    else {
        perror("Error: fgets returned NULL when trying to read vehicle capacity");
        exit(EXIT_FAILURE);
    }

    if (NULL != fgets(line, MAX_CVRPTW_DATA_FILE_LINE_LENGTH, file)) {
        sscanf(line, "%u %u.00 %u.00 %u.00 %u.00 %u.00 %u.00",
            &problem.depot.cust_no,
            &problem.depot.xcoord,
            &problem.depot.ycoord,
            &problem.depot.demand,
            &problem.depot.ready_time,
            &problem.depot.due_time,
            &problem.depot.service_time
        );
    }
    else {
        perror("Error: fgets returned NULL when trying to read depot data");
        exit(EXIT_FAILURE);
    }

    while (NULL != fgets(line, MAX_CVRPTW_DATA_FILE_LINE_LENGTH, file)) {
        sscanf(line, "%u %u.00 %u.00 %u.00 %u.00 %u.00 %u.00",
            &problem.customer_data[problem.num_customers].cust_no,
            &problem.customer_data[problem.num_customers].xcoord,
            &problem.customer_data[problem.num_customers].ycoord,
            &problem.customer_data[problem.num_customers].demand,
            &problem.customer_data[problem.num_customers].ready_time,
            &problem.customer_data[problem.num_customers].due_time,
            &problem.customer_data[problem.num_customers].service_time
        );
        problem.num_customers++;
    }
    
    fclose(file);
    return problem;
}

cvrptw_solution_t cvrptw_solution_get(char* path) {
    cvrptw_solution_t solution;
    FILE* file = fopen(path, "r");
    if (file == NULL) {
        perror("Error: could not open file");
        exit(EXIT_FAILURE);
    }

    solution.total_num_customers = 0;
    solution.num_vehicles = 0;

    char line[MAX_CVRPTW_SOL_FILE_LINE_LENGTH];
    while (NULL != fgets(line, MAX_CVRPTW_SOL_FILE_LINE_LENGTH, file)) {
        // Read dash-separated customer numbers into solution.custno_index using strtok
        // Insert the position of the first customer in the solution.custno_index buffer into solution.route_head_custno_index as well
        char* custno_str = strtok(line, "-");
        if (custno_str != NULL) {
            solution.route_head_index[solution.num_vehicles] = solution.total_num_customers;
        }
        else {
            perror("Error: strtok returned NULL");
            exit(EXIT_FAILURE);
        }
        solution.num_customers_in_route[solution.num_vehicles] = 0;
        while (custno_str != NULL) {
            solution.custno_index[solution.total_num_customers] = atoi(custno_str) - 1;
            solution.total_num_customers++;
            solution.num_customers_in_route[solution.num_vehicles]++;
            custno_str = strtok(NULL, "-");
        }
        solution.num_vehicles++;
    }

    fclose(file);
    return solution;
}

void cvrptw_data_print(cvrptw_problem_t problem) {
    printf("Number of customers: %u\n", problem.num_customers);
    printf("Vehicle capacity: %u\n", problem.vehicle_capacity);
    printf("Depot: %u %u %u %u %u %u %u\n", 
        problem.depot.cust_no,
        problem.depot.xcoord,
        problem.depot.ycoord,
        problem.depot.demand,
        problem.depot.ready_time,
        problem.depot.due_time,
        problem.depot.service_time
    );
    printf("Customer data:\n");

    for (cust_numeric_t i = 0; i < problem.num_customers; i++) {
        printf("%u %u %u %u %u %u %u\n", 
            problem.customer_data[i].cust_no,
            problem.customer_data[i].xcoord,
            problem.customer_data[i].ycoord,
            problem.customer_data[i].demand,
            problem.customer_data[i].ready_time,
            problem.customer_data[i].due_time,
            problem.customer_data[i].service_time
        );
    }
}

void cvrptw_solution_print(cvrptw_solution_t solution) {
    printf("Number of vehicles: %u\n", solution.num_vehicles);
    for (vehicle_numeric_t i = 0; i < solution.num_vehicles - 1; i++) {
        printf("Route of vehicle %u: ", i);
        for (cust_numeric_t j = solution.route_head_index[i]; j < solution.route_head_index[i + 1] - 1; j++) {
            printf("%u-", 1 + solution.custno_index[j]);
        }
        printf("%u", 1 + solution.custno_index[solution.route_head_index[i + 1] - 1]);
        printf("\n");
    }
    printf("Route of vehicle %u: ", solution.num_vehicles - 1);
    for (cust_numeric_t j = solution.route_head_index[solution.num_vehicles - 1]; j < solution.total_num_customers - 1; j++) {
        printf("%u-", 1 + solution.custno_index[j]);
    }
    printf("%u", 1 + solution.custno_index[solution.total_num_customers - 1]);
    printf("\n");
}