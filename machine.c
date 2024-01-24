#include "machine.h"

CMOV_RES cmovl(mp m) {
    
}
CMOV_RES cmovr(mp m) {

}

void creg1w(mp m) {

}
void creg2w(mp m) {

}

void creg1r(mp m) {

}
void creg2r(mp m) {

}

/**
 * Writes calculated creg1<-->creg2 distance to NTEMP register
*/
void cdist(mp m) {

}


NC_RES nc(mp m) {

}
void nad(mp m) {

}
void nsum(mp m) {

}
void ndue(mp m) {

}
void nready(mp m) {

}
void nservt(mp m) {

}
void ndem(mp m) {

}

void nreg1w(mp m) {

}
void nreg2w(mp m) {

}
void nreg1r(mp m) {

}
void nreg2r(mp m) {
    
}

void nop(mp m) {

}

/**
 * Halts the machine
*/
void fin(mp m);

BRANCH_OP branch_instructions[3] = {
    cmovl, cmovr, nc
};

void machine_init(mp m, cvrptw_problem_t* problem, cvrptw_machine_sol_t* sol, fsm_matrix_t* fsm_matrix) {
    m->problem = problem;
    m->sol = sol;
    m->fsm_matrix = fsm_matrix;

    m->cregs.creg1 = &problem->customer_data[0];
    m->cregs.creg2 = &problem->customer_data[0];
    m->cregs.ctemp = &problem->customer_data[0];

    m->nregs.nreg1 = 0;
    m->nregs.nreg2 = 0;
    m->nregs.ntemp = 0;

    m->posregs.cposreg = 0;
    m->posregs.vposreg = 0;

    m->fin_flag = 0x00;
}
machine_exit_status run_machine(cvrptw_problem_t* problem, cvrptw_machine_sol_t* sol, fsm_matrix_t* fsm_matrix, transitions_num_limit_t op_limit) {
    machine_internals_t m;
    machine_init(&m, problem, sol, fsm_matrix);
    
    transitions_num_limit_t ops_remaining = 0;

    unsigned char last_signal = NOW;
    while(m.fin_flag == 0x00 && ops_remaining > 0) {
        /*// Fetch
        fsm_state_t state = m->fsm_matrix[m->posregs.cposreg][m->posregs.vposreg];
        // Decode
        fsm_state_t opcode = state & 0x0F;
        fsm_state_t op1 = (state & 0xF0) >> 4;
        fsm_state_t op2 = (state & 0xF00) >> 8;
        // Execute
        branch_instructions[opcode](&m);
        // Update
        m->posregs.cposreg = op1;
        m->posregs.vposreg = op2;
        // Check exit condition
        ops_remaining--;*/

        state_transition_t* choosen_transition = (void*)0;
        state_transition_t* possible_transitions = m.fsm_matrix[m.state];
        // Check if any transition has a NOW signal
        for (int i = 0; i < m.fsm_matrix->num_states; i++) {
            if (possible_transitions[i].trigger.signal == NOW) {
                choosen_transition = &possible_transitions[i];
                break;
            }
        }

    }
}