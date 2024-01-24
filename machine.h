#include "cvrptw_typedefs.h"
#include "config.h"

#define MAX_STATES 500

typedef unsigned int state_t;
typedef unsigned int transitions_num_limit_t;

/**
 * The file specifies FSM instructions
*/

typedef unsigned char CMOV_RES;
typedef unsigned char NC_RES;
typedef unsigned char FIN_FLAG;

typedef double nreg_val_t;

/**
 * Triggers
*/
#define NOW 0x00 //unconditionally move to next state
#define MOVACK 0x01
#define MOVNACK 0x02
#define NGREATER 0x03
#define NLESS 0x04

typedef struct {
    cust_numeric_t custno_index[CVRPTW_MAX_CUSTOMERS];
    cust_numeric_t num_customers_in_route;
} cvrptw_machine_route_t;

typedef struct {
    cvrptw_machine_route_t routes[CVRPTW_MAX_VEHICLES];
    vehicle_numeric_t num_vehicles;
} cvrptw_machine_sol_t;

/**
 * "Client" registers
*/
typedef struct {
    cvrptw_data_entry_t* creg1;
    cvrptw_data_entry_t* creg2;
    cvrptw_data_entry_t* ctemp;
} cregs_t;

/**
 * "Number" registers
*/
typedef struct {
    nreg_val_t nreg1;
    nreg_val_t nreg2;
    nreg_val_t ntemp;
} nregs_t;

/**
 * Tape "position" registers; used for reading/writing cleint and vehicle tapes
*/
typedef struct {
    cust_numeric_t cposreg = 0;
    vehicle_numeric_t vposreg = 0;
} posregs_t;


typedef enum {
    MACHINE_FIN_REQUESTED = 0x00,
    MACHINE_KILLED = 0x01
} machine_exit_status;

typedef struct {
    unsigned char signal;
} branch_trigger_t;

typedef struct {
    branch_trigger_t trigger;
    state_t next_state;
} state_transition_t;

typedef struct {
    state_transition_t transitions[MAX_STATES][MAX_STATES];
    state_t num_states;
} fsm_matrix_t;

typedef struct
{
    cvrptw_problem_t *problem;
    cvrptw_machine_sol_t* sol;
    cregs_t cregs;
    nregs_t nregs;
    posregs_t posregs;
    FIN_FLAG fin_flag; // 0x00 = false, 0x01 = true (finished)
    
    fsm_matrix_t* fsm_matrix;
    state_t state;
} machine_internals_t;

typedef machine_internals_t* mp;
typedef void (*OP)(mp);
typedef unsigned char (*BRANCH_OP)(mp);




/**
 * Instructions
*/

CMOV_RES cmovl(mp m);
CMOV_RES cmovr(mp m);

void creg1w(mp m);
void creg2w(mp m);

void creg1r(mp m);
void creg2r(mp m);

/**
 * Writes calculated creg1<-->creg2 distance to NTEMP register
*/
void cdist(mp m);


NC_RES nc(mp m);
void nad(mp m);
void nsum(mp m);
void ndue(mp m);
void nready(mp m);
void nservt(mp m);
void ndem(mp m);

void nreg1w(mp m);
void nreg2w(mp m);
void nreg1r(mp m);
void nreg2r(mp m);

void nop(mp m);

/**
 * Halts the machine
*/
void fin(mp m);

OP instructions[17] = {
    fin, nop, creg1w, creg2w, creg1r, creg2r, cdist, nad, nsum, ndue, nready, nservt, ndem, nreg1w, nreg2w, nreg1r, nreg2r
};

BRANCH_OP branch_instructions[3] = {
    cmovl, cmovr, nc
};

machine_exit_status run_machine(cvrptw_problem_t* problem, cvrptw_machine_sol_t* sol, fsm_matrix_t* fsm_matrix);


