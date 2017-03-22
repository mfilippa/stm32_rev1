// -----------------------------------------------------------------------------
// sch.c - MPF 01/2017
// -----------------------------------------------------------------------------


// includes
#include "sch.h"

// function table struct
typedef struct sch_function_struct {
    uint32_t reload;            // reload of zero means disabled
    uint32_t count;                // countdown to execution
    void (*function) (void);    // function pointer
} sch_function_t;

// module structure
struct sch_struct {
    // timers
    uint32_t timer_count;            // nr of registered timers
    uint32_t timer[SCH_NR_TIMERS];    // timer table
    // functions
    uint32_t function_count;        // nr of registered functions
    sch_function_t function[SCH_NR_FUNCTIONS];    // function table
} sch;

// -----------------------------------------------------------------------------
// init
// -----------------------------------------------------------------------------
uint32_t sch_init(void){
    uint32_t i;
    // initialize timer stuff
    sch.timer_count = 0;
    for (i=0; i<SCH_NR_TIMERS; i++){
        sch.timer[i]=0;
    }
    // initialize function stuff
    sch.function_count = 0;
    for (i=0; i<SCH_NR_FUNCTIONS; i++){
        sch.function[i].reload = 0;        // disabled
        sch.function[i].count = 0;
        sch.function[i].function = 0;
    }
    // success
    return 0;
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
    // step timer
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
        // execute only if enabled (reload!=0) and not NULL
        if ((sch.function[i].reload!=0)&&(sch.function[i].function!=0)){
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
uint32_t sch_function_register(void (*function)(void), uint32_t reload){
    if (sch.function_count == SCH_NR_FUNCTIONS) return 0;
    else {
        sch.function[sch.function_count].function = function;
        sch.function[sch.function_count].reload = reload;
        sch.function_count++;
        return sch.function_count;
    }
}
// -----------------------------------------------------------------------------
// set reload
// -----------------------------------------------------------------------------
void sch_function_set_reload(uint32_t handle, uint32_t reload){
    if (handle>sch.function_count) return;
    else if (handle==0) return;
    else {
        sch.function[handle-1].reload = reload;
    }
}
// -----------------------------------------------------------------------------
// set function - set to NULL to disable it
// -----------------------------------------------------------------------------
void sch_function_set(uint32_t handle, void (*function)(void)){
    if (handle>sch.function_count) return;
    else if (handle==0) return;
    else {
        sch.function[handle-1].function = function;
    }
}

// -----------------------------------------------------------------------------
// register timer - return handle ZERO if ran out of timers
// -----------------------------------------------------------------------------
uint32_t sch_timer_register(void){
    // check if max nr of timer has been reached
    if (sch.timer_count == SCH_NR_TIMERS) return 0;
    // else create timer and return handle
    else {
        sch.timer_count++;
        return sch.timer_count;
    }
}


// -----------------------------------------------------------------------------
// set timer
// -----------------------------------------------------------------------------
void sch_timer_set(uint32_t handle, uint32_t count){
    if (handle>sch.timer_count) return;
    else if (handle==0) return;
    else {
        sch.timer[handle-1]= count;
    }
}

// -----------------------------------------------------------------------------
// check if expired - return 1 if expired
// -----------------------------------------------------------------------------
uint32_t sch_timer_has_expired(uint32_t handle){
    // if timer has been never been registered, it defaults to never expire
    if (handle>sch.timer_count) return 0;
    else if (handle==0) return 0;
    else {
        if (sch.timer[handle-1]==0) return 1;
        else return 0;
    }
}

