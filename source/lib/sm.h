// -----------------------------------------------------------------------------
// sm.h - MPF 02/2017
// -----------------------------------------------------------------------------

#ifndef _SM_H_
#define _SM_H_

/* Usage:
 *
 * Enumerate states:
 * enum states_enum {
 *     STATE1,
 *     STATE2,
 *     STATE3,
 *     STATE_COUNT,
 * };
 *
 * Prototype entry functions:
 * sm_func_t fcn1;
 * sm_func_t fcn2;
 * sm_func_t fcn3;
 *
 * Prototype test functions:
 * sm_test_func_t test1_during;
 * sm_test_func_t test2_exit;
 * sm_test_func_t test3_during;
 * sm_test_func_t test3_exit;
 *
 * Create state machine table:
 * { state , entry_func , wait , next_state ,
 *   during_test , during_state , exit_test , exit_state },
 * sm_table_t sm_table[STATE_COUNT] {
 *     {STATE1, &fcn1, 100, STATE2,
 *     &test1_during, STATE3, 0, 0},
 *     {STATE2, &fcn2, 200, STATE3,
 *     0, 0, &test2_exit, STATE1},
 *     {STATE3, &fcn3, 300, STATE1,
 *     &test3_during, STATE2, &test3_exit, STATE3},
 * }
 *
 * Create state machine variable:
 * sm_state_t sm;
 *
 * Initialize state machine:
 * sm_init(sm_table, &sm, STATE_COUNT);
 *
 * Force a jump to a state:
 * sm_set_state(sm_table, &sm, STATE2);
 */

// function to execute at each state
typedef void (sm_func_t)(void);

// function to test transition to new state
typedef uint32_t (sm_test_func_t)(void);

// state machine table
// { state , entry_func , wait , next_state ,
//   during_test , during_state , exit_test , exit_state },
typedef struct sm_table_struct {
    // state - MUST BE ORDERED LIST STARTING FROM ZERO, use enum to list states
    uint32_t state;
    // pointer to function to execute at entry state, 0 to skip
    sm_func_t * entry_func;
    // wait time
    uint32_t wait;
    // next state to transition after wait and all tests below return 0
    uint32_t next_state;
    // pointer to function to test at every step, 0 to skip
    // returning 1 moves during state
    sm_test_func_t * during_test;
    uint32_t during_state;
    // pointer to test function every step, 0 to skip test
    // returning 1 moves to test during state
    sm_test_func_t * exit_test;
    uint32_t exit_state;
} sm_table_t;

// regex to generate graphviz code:
//find:
//\s*\{\s*([^,\s]+)\s*,(?(?=\s*0\s*,)\s*0\s*,|\s*([^,\s]+)\s*,)\s*([^,\s]+)\s*,\s*([^,\s]+)\s*,(?(?=\s*0\s*,)\s*0\s*,|\s*([^,\s]+)\s*,)\s*([^,\s]+)\s*,(?(?=\s*0\s*,)\s*0\s*,|\s*([^,\s]+)\s*,)\s*([^,\s\}]+).*\r?\n
//replace:
//"\1" [label="\1 \\n (?2\2\\n:) wait:\3"]\r\n "\1" -> "\4"\r\n(?5 "\1" -> "\6" [color=red label="\5"] \r\n:)(?7 "\1" -> "\8" [color=green label="\7"] \r\n:)

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
