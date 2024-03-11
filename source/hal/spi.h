//------------------------------------------------------------------------------
// module.h - author - date
//------------------------------------------------------------------------------

#ifndef _MODULE_H_
#define _MODULE_H_

// public includes
#include <stdint.h>
#include <bool.h>
#include <string.h>

//------------------------------------------------------------------------------
// public structures and definitions
//------------------------------------------------------------------------------

// public defines
#define MODULE_DEFINE1    (1.0f)
#define MODULE_DEFINE2    (2.0f)

// public default params
#define MODULE_DEF_PARAM1  (1.0F)
#define MODULE_DEF_PARAM2  (1.5F)
#define MODULE_DEF_PARAM3  (2.3F)

// module enumerations
typedef enum module_enum {
    MODULE_ENUM1,
    MODULE_ENUM2,
    MODULE_ENUM3,
    MODULE_ENUM_COUNT,
} module_enum_t;

// public parameter list
typedef enum module_parameter_enum {
    MODULE_PARAM1,          // parameter 1
    MODULE_PARAM2,          // parameter 2
    MODULE_PARAM3,          // parameter 3
    MODULE_PARAM_COUNT,
} module_param_t;

// public public param struct typedef
typedef struct module_params_struct {
    float param1;
    float param2;
    float param3;
} module_param_table_t;

// public public module out struct typedef
typedef struct module_out_struct{
    float out1;
    float out2;
    float out3;
} module_out_table_t;

//------------------------------------------------------------------------------
// public prototypes
//------------------------------------------------------------------------------

// init
void module_init(void);

// step
void module_step(void);

// get out struct
module_out_t * module_get_out_struct(void);

// get param struct
module_param_table_t * module_get_param_table(void);

// set param
// XXX params can be float or ints but this function passes float only
// should work but will result in extra type conversion and possible
// incorrect data due to float > int conversion. This needs fixing...
void module_set_param(module_param_t param, float value);

// load params
void module_load_param(void);

// comm handler
void module_comm_handler(uint32_t rx_size);

#endif  // _MODULE_H_
