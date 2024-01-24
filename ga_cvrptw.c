#include "ga_cvrptw.h"
#include <stdlib.h>

int compare_cvrptw_data_entries(cvrptw_data_entry_t* a, cvrptw_data_entry_t* b) {
    if (a->ready_time < b->ready_time) {
        return -1;
    }
    else if (a->ready_time > b->ready_time) {
        return 1;
    }
    else {
        return 0;
    } 
};

/**
 * @brief Sorts the customer data by ready time using qsort
 * @param in_customer_data The customer data to be sorted.
 * @param out_customer_data The sorted customer data.
*/
void sortby_tw_start(cvrptw_data_entry_t* in_customer_data, cvrptw_data_entry_t* out_customer_data, cust_numeric_t num_customers) {
    // Copy the data to the output array
    for (cust_numeric_t i = 0; i < num_customers; i++) {
        out_customer_data[i] = in_customer_data[i];
    }
    // Sort the output array
    qsort(out_customer_data, num_customers, sizeof(cvrptw_data_entry_t), (int (*)(const void*, const void*))compare_cvrptw_data_entries);
}



