// -----------------------------------------------------------------------------
// sm.c - MPF 02/2017
// -----------------------------------------------------------------------------

// includes
#include "sm.h"

// -----------------------------------------------------------------------------
// init
// -----------------------------------------------------------------------------
uint32_t sm_init(sm_table_t * table, sm_state_t * state, uint32_t tsize){
    uint32_t i;
    // check that table states are ordered 0..N
    // and that all states are less than tsize
    for (i=0;i<tsize;i++) {
        if (table[i].state != i) return 1;
        if (table[i].next_state >= tsize) return 1;
        if (table[i].during_state >= tsize) return 1;
        if (table[i].exit_state >= tsize) return 1;
    }

    // initialize sm state and execute first state
    state->tsize = tsize;
    sm_set_state(table,state,0);
    // success
    return 0;
}

// -----------------------------------------------------------------------------
// step
// -----------------------------------------------------------------------------
uint32_t sm_step(sm_table_t * table, sm_state_t * state){
    uint32_t curr_state = state->state;
    // test during
    if (table[curr_state].during_test!=0){
        // if tests positive, go to during state
        if ((*table[curr_state].during_test)()!=0) {
            sm_set_state(table,state, table[curr_state].during_state);
            // new state achieved, exit now
            return state->state;
        }
    }
    // wait for timeout
    if (state->wait!=0) {
        state->wait--;
    }
    // wait time is over, execute transition
    else {
        // if there is a test function, execute it
        if (table[curr_state].exit_test!=0) {
            // if tests positive, go to test state
            if ((*table[curr_state].exit_test)()!=0) {
                sm_set_state(table,state, table[curr_state].exit_state);
            }
            // else tests negative, go to next state
            else {
                sm_set_state(table,state, table[curr_state].next_state);
            }
        }
        // else no test function, go to next state
        else {
            sm_set_state(table,state, table[curr_state].next_state);
        }
    }
    // done
    return state->state;
}

// -----------------------------------------------------------------------------
// set state
// -----------------------------------------------------------------------------
void sm_set_state(sm_table_t * table, sm_state_t * state, uint32_t set_state){
    if (set_state>=state->tsize) return;
    // set wait time of new state
    if (table[set_state].wait!=0) state->wait = table[set_state].wait-1;
    else state->wait=0;
    // execute new state function, if not entering same state
    if (state->state != set_state) {
        if(table[set_state].entry_func!=0) (*table[set_state].entry_func)();
    }
    // set state
    state->state = set_state;
}
