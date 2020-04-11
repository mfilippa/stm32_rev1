// -----------------------------------------------------------------------------
// error.c - MPF 11/2019
// -----------------------------------------------------------------------------

// includes
#include <string.h>
#include "error.h"

// module struct
struct error_struct {
    error_t error;
    void (*err_handler)(error_t err);
} error;


// -----------------------------------------------------------------------------
// init
// -----------------------------------------------------------------------------
void error_init(void (*err_handler)(error_t err)){
    error.err_handler = err_handler;
}

// -----------------------------------------------------------------------------
// prototypes
// -----------------------------------------------------------------------------
void error_raise(error_t err){
    if (error.err_handler!=NULL) (*error.err_handler)(err);
}
