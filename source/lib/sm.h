// -----------------------------------------------------------------------------
// sm.h - MPF 02/2017
// -----------------------------------------------------------------------------

#ifndef _SM_H_
#define _SM_H_

// function to execute at each state
typedef void (sm_func_t)(void);

// function to test transition to new state
typedef uint32_t (sm_test_func_t)(void);

// state machine table
typedef struct sm_table_struct {
	// state - MUST BE ORDERED NUMBERED LIST STARTING FROM ZERO
	// suggestion: use an enum to list states
	uint32_t state;
	// pointer to function to execute at state, null to execute nothing
	sm_func_t * func;
	// wait time
	uint32_t wait;
	// pointer fo test function, returns 1 to move to next state, 0 to remain
	sm_test_func_t * test_func;
	// test state - state to jump to if test func returns 1
	uint32_t test_state;
	// else next state
	uint32_t next_state;
} sm_table_t;

// current state
typedef struct sm_state_struct {
	// table size
	uint32_t tsize;
	// current state
	uint32_t state;
	// current wait count
	uint32_t wait;
} sm_state_t;

// initialize sm, returns 0 if successful
uint32_t sm_init(sm_table_t * table, sm_state_t * state, uint32_t tsize);

// step machine, returns current state
uint32_t sm_step(sm_table_t * table, sm_state_t * state);

// set state
void sm_set_state(sm_table_t * table, sm_state_t * state, uint32_t set_state);


#endif // _SM_H_
