#include "machine.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

CMOV_RES cmovl(mp m) {
    if (m->posregs.cposreg == 0) {
        return MOVNACK;
    }
    else {
        m->posregs.cposreg--;
        return MOVACK;
    }
}
CMOV_RES cmovr(mp m) {
    if (m->posregs.cposreg == m->problem->num_customers - 1) {
        return MOVNACK;
    }
    else {
        m->posregs.cposreg++;
        return MOVACK;
    }
}

/**
 * Write client pointer indicated by cposreg to creg1
*/
void creg1w(mp m) {
    m->cregs.creg1 = &m->problem->customer_data[m->posregs.cposreg];
    
}

/**
 * Write client pointer indicated by cposreg to creg2
*/
void creg2w(mp m) {
    m->cregs.creg2 = &m->problem->customer_data[m->posregs.cposreg];
}

/**
 * Read creg1 into ctemp
*/
void creg1r(mp m) {
    m->cregs.ctemp = m->cregs.creg1;
}

/**
 * Read creg2 into ctemp
*/
void creg2r(mp m) {
    m->cregs.ctemp = m->cregs.creg2;
}

nreg_val_t calculate_distance(cvrptw_data_entry_t* c1, cvrptw_data_entry_t* c2) {
    nreg_val_t dx = c1->xcoord - c2->xcoord;
    nreg_val_t dy = c1->ycoord - c2->ycoord;
    return (nreg_val_t) sqrt(dx*dx + dy*dy);
}

/**
 * Writes calculated creg1<-->creg2 distance to NTEMP register
*/
void cdist(mp m) {
    m->nregs.ntemp = calculate_distance(m->cregs.creg1, m->cregs.creg2);
}

/**
 * If nreg1 < nreg2 send signal NLESS, else send signal NGREATER
 * @return computed signal
*/
NC_RES nc(mp m) {
    if (m->nregs.nreg1 < m->nregs.nreg2) {
        return NLESS;
    }
    else {
        return NGREATER;
    }
}

/**
 * Compute |nreg1 - nreg2| and write it to ntemp
*/
void nad(mp m) {
    m->nregs.ntemp = abs(m->nregs.nreg1 - m->nregs.nreg2);
}

/**
 * Compute nreg1 + nreg2 and write it to ntemp
*/
void nsum(mp m) {
    m->nregs.ntemp = m->nregs.nreg1 + m->nregs.nreg2;
}

/**
 * Get the due time of the customer pointed by ctemp and write it to ntemp
*/
void ndue(mp m) {
    m->nregs.ntemp = m->cregs.ctemp->due_time;
}

/**
 * Get the ready time of the customer pointed by ctemp and write it to ntemp
*/
void nready(mp m) {
    m->nregs.ntemp = m->cregs.ctemp->ready_time;
}

/**
 * Get the service time of the customer pointed by ctemp and write it to ntemp
*/
void nservt(mp m) {
    m->nregs.ntemp = m->cregs.ctemp->service_time;
}

/**
 * Get the demand of the customer pointed by ctemp and write it to ntemp
*/
void ndem(mp m) {
    m->nregs.ntemp = m->cregs.ctemp->demand;
}

/**
 * Write ntemp to nreg1
*/
void nreg1w(mp m) {
    m->nregs.nreg1 = m->nregs.ntemp;
}

/**
 * Write ntemp to nreg2
*/
void nreg2w(mp m) {
    m->nregs.nreg2 = m->nregs.ntemp;
}

/**
 * Read nreg1 into ntemp
*/
void nreg1r(mp m) {
    m->nregs.ntemp = m->nregs.nreg1;
}

/**
 * Read nreg2 into ntemp
*/
void nreg2r(mp m) {
    m->nregs.ntemp = m->nregs.nreg2;
}

/**
 * Do nothing
*/
void nop(mp m) {

}

/**
 * Halts the machine
*/
void fin(mp m) {
    m->fin_flag = 0x01;
}

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
    init_instructions();
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
        state_transition_t* possible_transitions = m.fsm_matrix->transitions[m.state];
        // Check if any transition has a last_signal signal
        for (int i = 0; i < m.fsm_matrix->num_states; i++) {
            if (possible_transitions[i].trigger.signal == last_signal) {
                choosen_transition = &possible_transitions[i];
                break;
            }
        }
        if (choosen_transition == (void*)0) {   
            fprintf(stderr, "Error: no transition with signal %u found in state %u\n", last_signal, m.state);
            exit(EXIT_FAILURE);
        }
        else {
            // Execute
            //branch_instructions[choosen_transition->trigger.signal](&m);
            if (choosen_transition->opcode >= BRANCH_OPS_START_OPCODE && choosen_transition->opcode < SIMPLE_OPS_START_OPCODE) {
                BRANCH_OP branch_op = instructions[choosen_transition->opcode].branch_op;
                last_signal = branch_op(&m);
            }
            else {
                SIMPLE_OP simple_op = instructions[choosen_transition->opcode].simple_op;
                simple_op(&m);
                last_signal = NOW;
            }
            // Update
            m.state = choosen_transition->next_state;
            // Check exit condition
            ops_remaining--;
        }
        
        if (m.fin_flag == 0x00) {
            return MACHINE_KILLED;
        }
        else {
            return MACHINE_FIN_REQUESTED;
        }

    }
}