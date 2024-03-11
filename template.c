//------------------------------------------------------------------------------
// module.c - author - date
//------------------------------------------------------------------------------

// private includes
#include "module.h"

// simulation variables
#ifdef SIMULATION
extern double * pin;
extern double * pout;
#endif

// private defines
#define MODULE_PRIV_DEFINE1 (1.0f)
#define MODULE_PRIV_DEFINE2 (2.0f)

// module structure
struct {

    // parameters
    module_param_table_t params;

    // parameter reloads
    module_param_table_t params_reload;

    // internal variables


    // output struct
    module_out_table_t out;

} module;

// private prototypes

//------------------------------------------------------------------------------
// init
//------------------------------------------------------------------------------
void module_init(void) {

}

//------------------------------------------------------------------------------
// step
//------------------------------------------------------------------------------
void module_step(void){

}

//------------------------------------------------------------------------------
// get out struct
//------------------------------------------------------------------------------
module_out_t * module_get_out_struct(void){
    return &module.out;
}

//------------------------------------------------------------------------------
// get param struct
//------------------------------------------------------------------------------
module_param_table_t * module_get_param_table(void){
    return &module.params;
}

//------------------------------------------------------------------------------
// set param
//------------------------------------------------------------------------------
void module_set_param(module_param_t param, float value){
    switch(param){
    case MODULE_PARAM1:
        module.param_reload.param1 = value;
        break;
    case MODULE_PARAM2:
        module.param_reload.param2 = value;
        break;
    default:
        break;
    }
}

//------------------------------------------------------------------------------
// load params
//------------------------------------------------------------------------------
void module_load_param(void){
    // parameters are not to take effect until a load param is issued    
    // parameters are loaded one by one because there can be initialization
    // required after loading a new parameter, e.g. PID initialization
    module.params.param1 = module.params_reload.param1;
    module.params.param2 = module.params_reload.param2;
}

//------------------------------------------------------------------------------
// comm handler
//------------------------------------------------------------------------------
void module_comm_handler(uint32_t rx_size){

    // first byte is cmd, echo it
    uint32_t cmd = comm_read8();
    comm_write8(cmd);

    // commands
    switch(cmd){
    case (0x00):
        break;
    case (0x01):
        break;
    default:
        // command not recognized
        comm_write8(COMM_INVALID_CMD);
    }
}
