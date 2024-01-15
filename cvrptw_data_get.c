#include "cvrptw_data_get.h"
#include "config.h"
#include <stdio.h>
#include <string.h>


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
 * @note The function assumes that the file contains no more than CVRPTW_MAX_CUSTOMERS lines.
*/
cvrptw_problem_t cvrptw_data_get(char* path) {
    cvrptw_problem_t problem;
    FILE* file = fopen(path, "r");
    //fscanf(file, "%u", &problem.num_customers);
    problem.num_customers = 0;
    
    char line[MAX_CVRPTW_DATA_FILE_LINE_LENGTH];
    while (NULL != fgets(line, MAX_CVRPTW_DATA_FILE_LINE_LENGTH, file)) {
        sscanf(line, "%u %u.00 %u.00 %u.00 %u.00 %u.00 %u.00", 
            &problem.data[problem.num_customers].cust_no,
            &problem.data[problem.num_customers].xcoord,
            &problem.data[problem.num_customers].ycoord,
            &problem.data[problem.num_customers].demand,
            &problem.data[problem.num_customers].ready_time,
            &problem.data[problem.num_customers].due_time,
            &problem.data[problem.num_customers].service_time
        );
        problem.num_customers++;
    }
    
    fclose(file);
    return problem;
}

void cvrptw_data_print(cvrptw_problem_t problem) {
    printf("Number of customers: %u\n", problem.num_customers);
    for (int i = 0; i < problem.num_customers; i++) {
        printf("%u %u %u %u %u %u %u\n", 
            problem.data[i].cust_no,
            problem.data[i].xcoord,
            problem.data[i].ycoord,
            problem.data[i].demand,
            problem.data[i].ready_time,
            problem.data[i].due_time,
            problem.data[i].service_time
        );
    }
}
