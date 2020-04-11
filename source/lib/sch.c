// -----------------------------------------------------------------------------
// sch.c - MPF 11/2019
// -----------------------------------------------------------------------------

// includes
#include "error.h"
#include "sch.h"

// function table struct
typedef struct sch_table_struct {
    uint32_t reload;                // reload of zero means disabled
    uint32_t count;                 // countdown to execution
    sch_function_t * function;      // function pointer
} sch_table_t;

// module structure
struct sch_struct {
    // timers
    uint32_t timer_count;               // nr of registered timers
    uint32_t timer[SCH_NR_TIMERS];      // timer table
    // functions
    uint32_t function_count;            // nr of registered functions
    sch_table_t function[SCH_NR_FUNCTIONS];    // function table
} sch;

// -----------------------------------------------------------------------------
// init
// -----------------------------------------------------------------------------
void sch_init(void){
    uint32_t i;
    // initialize timer stuff
    sch.timer_count = 0;
    for (i=0; i<SCH_NR_TIMERS; i++){
        sch.timer[i]=0;
    }
    // initialize function stuff
    sch.function_count = 0;
    for (i=0; i<SCH_NR_FUNCTIONS; i++){
        sch.function[i].reload = 0;
        sch.function[i].count = 0;
        sch.function[i].function = NULL;
    }
}

// -----------------------------------------------------------------------------
// tick
// -----------------------------------------------------------------------------
void sch_tick(void){
    int32_t i;
    // search for non-zero function count and decrement its count
    for (i=0; i<sch.function_count;i++){
        if (sch.function[i].count != 0) {
            sch.function[i].count--;
        }
    }
    // search for non-zero timers, decrement its count
    for (i=0; i<sch.timer_count; i++){
        if (sch.timer[i] != 0) sch.timer[i]--;
    }
}

// -----------------------------------------------------------------------------
// step
// -----------------------------------------------------------------------------
void sch_step(void){
    int32_t i;
    // check each function count, if reached zero, execute task and reload
    for (i=0; i<sch.function_count;i++){
        // execute only if not NULL
        if (sch.function[i].function!=NULL){
            if (sch.function[i].count == 0) {
                sch.function[i].function();
                sch.function[i].count = sch.function[i].reload;
            }
        }
    }
}

// -----------------------------------------------------------------------------
// register function
// -----------------------------------------------------------------------------
sch_handle_t sch_function_register(sch_function_t * function, uint32_t reload){
    if (sch.function_count == SCH_NR_FUNCTIONS) error_raise(ERROR_SCH_REG);
    sch.function[sch.function_count].function = function;
    sch.function[sch.function_count].reload = reload;
    sch.function_count++;
    // handle maps to index + 1
    return sch.function_count;
}

// -----------------------------------------------------------------------------
// set reload
// -----------------------------------------------------------------------------
void sch_function_set_reload(sch_handle_t handle, uint32_t reload){
    if (handle>sch.function_count) error_raise(ERROR_SCH_ARG);
    else if (handle==0) error_raise(ERROR_SCH_ARG);
    else sch.function[handle-1].reload = reload;
}

// -----------------------------------------------------------------------------
// set function - set to NULL to disable it
// -----------------------------------------------------------------------------
void sch_function_set(sch_handle_t handle, void (*function)(void)){
    if (handle>sch.function_count) error_raise(ERROR_SCH_ARG);
    else sch.function[handle-1].function = function;
}

// -----------------------------------------------------------------------------
// register timer
// -----------------------------------------------------------------------------
sch_handle_t sch_timer_register(void){
    // check if max nr of timer has been reached
    if (sch.timer_count == SCH_NR_TIMERS) error_raise(ERROR_SCH_REG);
    // else create timer and return handle
    sch.timer_count++;
    return sch.timer_count;
}


// -----------------------------------------------------------------------------
// set timer
// -----------------------------------------------------------------------------
void sch_timer_set(sch_handle_t handle, uint32_t count){
    if (handle>sch.timer_count) error_raise(ERROR_SCH_ARG);
    else if (handle==0) error_raise(ERROR_SCH_ARG);
    sch.timer[handle-1]= count;
}

// -----------------------------------------------------------------------------
// check if expired - return 1 if expired
// -----------------------------------------------------------------------------
bool sch_timer_has_expired(sch_handle_t handle){
    if (handle>sch.timer_count) error_raise(ERROR_SCH_ARG);
    else if (handle==0) error_raise(ERROR_SCH_ARG);
    if (sch.timer[handle-1]==0) return true;
    else return false;
}

