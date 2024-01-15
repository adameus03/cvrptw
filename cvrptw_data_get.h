#include "cvrptw_typedefs.h"

/**
 * @brief Loads CVRPTW problem data from a file.
 * @param path The path to the file containing the data.
 * @note Each row in the file should contain the following data, separated by a single space:
 * 1. Customer number
 * 2. X coordinate
 * 3. Y coordinate
 * 4. Demand
 * 5. Ready time
 * 6. Due time
 * 7. Service time
 * @return The CVRPTW problem data.
 * @note The function assumes that the file is formatted correctly.
 * @note The function assumes that the file contains no more than CVRPTW_MAX_CUSTOMERS rows.
*/
cvrptw_problem_t cvrptw_data_get(char* path);

void cvrptw_data_print(cvrptw_problem_t problem);