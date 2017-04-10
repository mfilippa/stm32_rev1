// -----------------------------------------------------------------------------
// param.h - MPF 04/2017
// -----------------------------------------------------------------------------

#ifndef _PARAM_H_
#define _PARAM_H_

/* Usage:
 *
 * Enumerate parameters:
 * enum param_enum {
 *     PARAM1,
 *     PARAM2,
 *     PARAM3,
 *     PARAM_COUNT,
 * };
 *
 * Declare param array and fill with default values:
 * param_array_t param_array[PARAM_COUNT] = {
 *     {PARAM1, 10},
 *     {PARAM2, 20},
 *     {PARAM3, 30},
 * }
 *
 * Create config variable:
 * param_congig_t param_config;
 *
 * Param callback: use to perform additional actions when setting a param
 * void param_set_fcn(int32_t param, int32_t value) {
 *     switch(param) {
 *     case PARAM1:
 *         // do more stuff
 *         break;
 *     }
 * }
 *
 * Initialize parameters to default val (use return value to check for errors):
 * param_init(&param_config, param_array, PARAM_COUNT, &param_set_fcn);
 *
 * To set a parameter:
 * param_set(&param_config, PARAM1, 11);
 *
 */

// includes
#include <stdint.h>

// param array typedef
// param must be sorted ascending from zero, use enum to create params
typedef struct param_array_struct {
    int32_t param;      // parameter
    int32_t value;      // default value (init) / stored value (set)
} param_array_t;

// set param typedef
typedef void (param_set_fcn_t)(int32_t param, int32_t value);

// config typedef
typedef struct param_config_struct {
    param_array_t * array;
    uint32_t size;
    param_set_fcn_t * set_fcn;
} param_config_t;

// initialize param
// array: array of parameters to initialize
// param set: callback if further application action is needed, null to
// ignore.
int32_t param_init(param_config_t * config, param_array_t * array,
        uint32_t size, param_set_fcn_t * set_fcn);

// param set
void param_set(param_config_t * config, int32_t param, int32_t value);

#endif
