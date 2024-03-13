//------------------------------------------------------------------------------
// can.h - Mariano F - 03/2024
//------------------------------------------------------------------------------

#ifndef _CAN_H_
#define _CAN_H_

// public includes
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

//------------------------------------------------------------------------------
// public structures and definitions
//------------------------------------------------------------------------------

// public defines
#define CAN_DEFINE1    (1.0f)
#define CAN_DEFINE2    (2.0f)

// public default params
#define CAN_DEF_PARAM1  (1.0F)
#define CAN_DEF_PARAM2  (1.5F)
#define CAN_DEF_PARAM3  (2.3F)

// module enumerations
typedef enum can_enum {
    CAN_ENUM1,
    CAN_ENUM2,
    CAN_ENUM3,
    CAN_ENUM_COUNT,
} can_enum_t;

// public parameter list
typedef enum can_parameter_enum {
    CAN_PARAM1,          // parameter 1
    CAN_PARAM2,          // parameter 2
    CAN_PARAM3,          // parameter 3
    CAN_PARAM_COUNT,
} can_param_t;

// public public param struct typedef
typedef struct can_params_struct {
    float param1;
    float param2;
    float param3;
} can_param_table_t;

// public public module out struct typedef
typedef struct can_out_struct{
    float out1;
    float out2;
    float out3;
} can_out_table_t;

//------------------------------------------------------------------------------
// public prototypes
//------------------------------------------------------------------------------

// init
void can_init(void);

// step
void can_step(void);

// get out struct
can_out_t * can_get_out_struct(void);

// get param struct
can_param_table_t * can_get_param_table(void);

// set param
// XXX params can be float or ints but this function passes float only
// should work but will result in extra type conversion and possible
// incorrect data due to float > int conversion. This needs fixing...
void can_set_param(can_param_t param, float value);

// load params
void can_load_param(void);

// comm handler
void can_comm_handler(uint32_t rx_size);

#endif  // _CAN_H_
