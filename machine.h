#include "cvrptw_typedefs.h"
#include "config.h"

#define MAX_STATES 500

typedef unsigned int state_t;
typedef unsigned int transitions_num_limit_t;
typedef unsigned char opcode_t;

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
    cust_numeric_t cposreg;
    vehicle_numeric_t vposreg;
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
    opcode_t opcode;
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

typedef void (*SIMPLE_OP)(mp); //Operation that does not return a branch trigger
typedef unsigned char (*BRANCH_OP)(mp); //Operation that returns a branch trigger

typedef union {
    SIMPLE_OP simple_op;
    BRANCH_OP branch_op;
} OP;




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

const opcode_t BRANCH_OPS_START_OPCODE = 0x02;
const opcode_t SIMPLE_OPS_START_OPCODE = 0x05; 

//BRANCH macro returns an OP with a branch operation
#define BRANCH(op) (OP){.branch_op = op}
//SIMPLE macro returns an OP with a simple operation
#define SIMPLE(op) (OP){.simple_op = op} 

OP instructions[20];

void init_instructions() {
    instructions[0] = SIMPLE(fin); 
    instructions[1] = SIMPLE(nop); 
    instructions[2] = BRANCH(cmovl); 
    instructions[3] = BRANCH(cmovr); 
    instructions[4] = BRANCH(nc); 
    instructions[5] = SIMPLE(creg1w); 
    instructions[6] = SIMPLE(creg2w); 
    instructions[7] = SIMPLE(creg1r); 
    instructions[8] = SIMPLE(creg2r); 
    instructions[9] = SIMPLE(cdist); 
    instructions[10] = SIMPLE(nad); 
    instructions[11] = SIMPLE(nsum); 
    instructions[12] = SIMPLE(ndue); 
    instructions[13] = SIMPLE(nready); 
    instructions[14] = SIMPLE(nservt); 
    instructions[15] = SIMPLE(ndem); 
    instructions[16] = SIMPLE(nreg1w);
    instructions[17] = SIMPLE(nreg2w); 
    instructions[18] = SIMPLE(nreg1r); 
    instructions[19] = SIMPLE(nreg2r);
}

machine_exit_status run_machine(cvrptw_problem_t* problem, cvrptw_machine_sol_t* sol, fsm_matrix_t* fsm_matrix, transitions_num_limit_t op_limit);


