// -----------------------------------------------------------------------------
// math.h - MPF - 01/2017
// -----------------------------------------------------------------------------

#ifndef _MATH_H_
#define _MATH_H_

// includes
#include <stdint.h>

// degrees to radians in Q format
#define MATH_DEG_TO_RAD(deg, q) 	\
	((int32_t)((double)deg/180.0*3.141592653589793*((int32_t)1<<q)))

// macro range check angle
#define MATH_ANGLE_RANGE_CHECK(angle)\
	while(angle>=MATH_DEG_TO_RAD(360, 11)) angle-= MATH_DEG_TO_RAD(360, 11); \
	while(angle<0) angle += MATH_DEG_TO_RAD(360, 11);

// macro slew rate limiter
#define MATH_SLR(var, target, step) \
		if ((var) > (target)+(step)) var -= (step); \
		else if ((var) < (target)-(step)) var += (step); \
		else var = target;

// macro abs
#define QMATH_ABS(a) (((a) < 0) ? -(a) : (a))

// biquad filter struct
typedef struct math_biquad_struct {
	int32_t b0;			// b0 coefficient in Q format
	int32_t b1;			// b1 coefficient in Q format
	int32_t b2;			// b2 coefficient in Q format
	int32_t a1;			// a1 coefficient in Q format
	int32_t a2;			// a2 coefficient in Q format
	int32_t q;			// Q for filter coefficients
	int32_t x_z1;		// memory location x(-1)
	int32_t x_z2;		// memory location x(-2)
	int32_t y_z1;		// memory location y(-1)
	int32_t y_z2;		// memory location y(-2)
	int32_t out;		// output of filter
	int32_t fraction;	// for fraction saving
} math_biquad_t;

// table lookup struct
typedef struct math_tlookup_struct {
	int32_t x;			// x point
	int32_t y;			// y point
	int32_t slope;		// slope (y/x) in q format
	int32_t slope_q;	// q coefficient of slope
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

// sin: sin of angle in q11, returns value in q14
int32_t math_sin(int32_t angle_q11);

// cos: cos of angle in q11, returns value in q14
int32_t math_cos(int32_t angle_q11);

// table_lookup: look up table with interpolation/extrapolation, returns value
// NOTE: TABLE MUST BE ORDERED ASCENDING WITH RESPECT TO X
int32_t math_table_lookup(int32_t value, const math_tlookup_t * table, uint32_t tsize);

// biquad_filter: implements a biquad filter
int32_t math_biquad_filter(math_biquad_t * filter, int32_t input);

// biquad_init: function to initialize filter
void math_biquad_init(math_biquad_t * filter, int32_t b0, int32_t b1, int32_t b2, int32_t a1, int32_t a2, int32_t q);

// sqrt: square root of int
int32_t math_sqrt(int32_t n);

// abc_to_qd0: abc to qd0 transform, angle in q11
void math_abc_to_qd0(math_abcqd0_t * abcqd0, int32_t angle_q11);

// qd0_to_abc: qd0 to abc transform, angle in q11
void math_qd0_to_abc(math_abcqd0_t * abcqd0, int32_t angle_q11);

#endif // _MATH_H_
