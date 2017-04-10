// -----------------------------------------------------------------------------
// param.c - MPF 04/2017
// -----------------------------------------------------------------------------

// includes
#include "param.h"

// -----------------------------------------------------------------------------
// param init
// -----------------------------------------------------------------------------
int32_t param_init(param_config_t * config, param_array_t * array,
        uint32_t size, param_set_fcn_t * set_fcn){
    int32_t i;
    // range checks
    if (size==0) return 1;
    // check that array of params is properly initialized, i.e. param is
    // ordered ascending from zero
    for (i=0;i<size;i++){
        if (array[i].param!=i) return 1;
    }
    // store values
    config->array = array;
    config->size = size;
    config->set_fcn = set_fcn;

    // initialize parameter table with default values
    for(i=0;i<size;i++){
        param_set(config, i, array[i].value);
    }
    // success
    return 0;
}

// -----------------------------------------------------------------------------
// param set
// -----------------------------------------------------------------------------
void param_set(param_config_t * config, int32_t param, int32_t value){
    // range check
    if (param>=config->size) return;
    // store param
    config->array[param].value = value;
    // execute set function
    if (config->set_fcn!=0) config->set_fcn(param, value);
}
