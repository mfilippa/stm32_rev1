// -----------------------------------------------------------------------------
// mathlib.h - MPF - 11/2020
// -----------------------------------------------------------------------------

#ifndef _MATHLIB_H_
#define _MATHLIB_H_

// includes
#include <stdint.h>

// MANY OF THESE FUNCTIONS DO NOT PERFORM RANGE CHECK, TRADE OFF FOR SPEED
// RANGE CHECK BEFORE PASSING ARGUMENTS TO FUNCTION

// angle representation:
// [0..360) = [0..2pi) -> 0..0xFFFF (q16)

// comment for fast sin/cos (table look up without interpolation)
#define MATH_SIN_COS_INTERPOLATION

// degrees to q16
#define MATH_DEG_TO_Q16(deg)     \
    ((uint32_t)(((int32_t)((double)deg/360.0*((int32_t)1<<16)))&0xFFFF))

// radians to q16
#define MATH_RAD_TO_Q16(deg)     \
    ((uint32_t)(((int32_t)((double)deg/2.0/3.141592653589793* \
        ((int32_t)1<<16)))&0xFFFF))

// macro range check angle
#define MATH_ANGLE_RANGE_CHECK(angle)   ((angle)&0xFFFF)

// macro slew rate limiter
#define MATH_SLR(var, target, step) \
        if ((var) > (target)+(step)) var -= (step); \
        else if ((var) < (target)-(step)) var += (step); \
        else var = target;

// macro abs
#define MATH_ABS(a) (((a) < 0) ? -(a) : (a))

// macro low pass filter (IIR filter) out/in = 1 / (1 + tao s)
// implements: out(n) = out(n-1) - out(n-1) T/tao + in(n-1) T/tao
// T is sampling time, tao is filter time constant, T/tao in q14
// in is input value, out is output of filter, mem is 32b storage
// to match Navg sample average at n tao: T/tao = n/Navg
// store initial value of mem in Q14!
#define MATH_FAST_LPF(out,in,mem,Ttao) \
    (mem)= (mem)-((mem>>14)*(Ttao))+((in)*(Ttao)); \
    (out) = (mem)>>14;

// biquad filter struct
typedef struct math_biquad_struct {
    int32_t b0;            // b0 coefficient in Q format
    int32_t b1;            // b1 coefficient in Q format
    int32_t b2;            // b2 coefficient in Q format
    int32_t a1;            // a1 coefficient in Q format
    int32_t a2;            // a2 coefficient in Q format
    int32_t q;             // Q for filter coefficients
    int32_t x_z1;          // memory location x(-1)
    int32_t x_z2;          // memory location x(-2)
    int32_t y_z1;          // memory location y(-1)
    int32_t y_z2;          // memory location y(-2)
    int32_t out;           // output of filter
    int32_t fraction;      // for fraction saving
} math_biquad_t;

// table lookup struct
// NOTE: TABLE MUST BE ORDERED ASCENDING WITH RESPECT TO X
typedef struct math_tlookup_struct {
    int32_t x;            // x point
    int32_t y;            // y point
    int32_t slope;        // slope (y/x) in q format
    int32_t slope_q;      // q coefficient of slope
} math_tlookup_t;

// abcqd0 structure
typedef struct math_abcqd0_struct{
    int32_t a;
    int32_t b;
    int32_t c;
    int32_t q;
    int32_t d;
    int32_t zero;
} math_abcqd0_t;

// sin: sin of angle mapped to q16, returns value in q14
int32_t math_sin(uint32_t angle_q16);

// cos: cos of angle mapped to q16, returns value in q14
int32_t math_cos(uint32_t angle_q16);

// table_lookup: look up table with interpolation/extrapolation, returns value
// NOTE: TABLE MUST BE ORDERED ASCENDING WITH RESPECT TO X
int32_t math_table_lookup(int32_t value, const math_tlookup_t * table, 
    uint32_t tsize);

// biquad_filter: implements a biquad filter
int32_t math_biquad_filter(math_biquad_t * filter, int32_t input);

// biquad_init: function to initialize filter
void math_biquad_init(math_biquad_t * filter, int32_t b0, int32_t b1, 
    int32_t b2, int32_t a1, int32_t a2, int32_t q);

// sqrt: square root of int
int32_t math_sqrt(int32_t n);

// abc_to_qd0: abc to qd0 transform, angle mapped to q16
void math_abc_to_qd0(math_abcqd0_t * abcqd0, uint32_t angle_q16);

// qd0_to_abc: qd0 to abc transform, angle mapped to q16
void math_qd0_to_abc(math_abcqd0_t * abcqd0, uint32_t angle_q16);

// step angle, angle mapped to q16
void math_step_angle(uint32_t * angle_q16, uint32_t * angle_fraction, 
    int32_t frequency, uint32_t frequency_q, int32_t deltaT, uint32_t deltaT_q);

#endif // _MATHLIB_H_
