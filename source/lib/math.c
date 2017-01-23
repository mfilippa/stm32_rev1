// -----------------------------------------------------------------------------
// math.c - MPF - 10/2013
// -----------------------------------------------------------------------------

// includes
#include "math_tables.h"
#include "math.h"

// math tables

// atan2 ratio in Q14 to table offset of size 2047
// = 2048 / 2^14
#define MATH_ATAN2_RATIO2OFFSET           16383	// minus 1, simulate in matlab
#define MATH_ATAN2_RATIO2OFFSET_Q           17

// sin table angle (rad) to table offset
// = 2048 / (2*pi*2^11)
#define MATH_SIN_RAD2OFFSET           20860
#define MATH_SIN_RAD2OFFSET_Q           17


// angle defines in q11
#define ANGLE_30DEG		MATH_DEG_TO_RAD(30,11)
#define ANGLE_60DEG		MATH_DEG_TO_RAD(60,11)
#define ANGLE_90DEG		MATH_DEG_TO_RAD(90,11)

// -----------------------------------------------------------------------------
// sin - angle in rads q11, returns amplitude in Q14 - NO RANGE CHECK (use macro)
// -----------------------------------------------------------------------------
int32_t math_sin(int32_t angle_q11){
	return sine_table[(angle_q11*MATH_SIN_RAD2OFFSET)>>MATH_SIN_RAD2OFFSET_Q];
}

// -----------------------------------------------------------------------------
// cos - angle in rads q11, returns amplitude in Q14 - NO RANGE CHECK (use macro)
// -----------------------------------------------------------------------------
int32_t math_cos(int32_t angle_q11){
	angle_q11 = angle_q11+MATH_DEG_TO_RAD(90,11);
	if (angle_q11>=MATH_DEG_TO_RAD(360, 11)) angle_q11 -= MATH_DEG_TO_RAD(360, 11);
	return math_sin(angle_q11);
}

// -----------------------------------------------------------------------------
// table look up
// -----------------------------------------------------------------------------
int32_t math_table_lookup(int32_t value, const math_tlookup_t * table, uint32_t tsize){
	// NOTE: TABLE MUST BE ORDERED ASCENDING WITH RESPECT TO X
	uint32_t i;
	int32_t result = 0;
	// table too small
	if (tsize<=1) return 0;
	// outside boundaries: extrapolate
	if (value < table[0].x) {
		result = table[0].y +
				((table[0].slope*(value-table[0].x))>>table[0].slope_q);
	}
	else if (value > table[tsize-1].x){
		result = table[tsize-1].y +
				((table[tsize-1].slope*(value-table[tsize-1].x))>>table[tsize-1].slope_q);
	}
	// within boundaries
	else {
		for (i=1;i<tsize;i++){
			if (value < table[i].x) break;
		}
		result = table[i-1].y +
						((table[i-1].slope*(value-table[i-1].x))>>table[i-1].slope_q);
	}
	return result;
}

// -----------------------------------------------------------------------------
// biquad filter init
// -----------------------------------------------------------------------------
void math_biquad_init(math_biquad_t * filter, int32_t b0, int32_t b1, int32_t b2,
		int32_t a1, int32_t a2, int32_t q){
	filter->a1 = a1;
	filter->a2 = a2;
	filter->b0 = b0;
	filter->b1 = b1;
	filter->b2 = b2;
	filter->q = q;
	filter->fraction = 0;
	filter->out = 0;
	filter->x_z1 = filter->x_z2 = filter->y_z1 = filter->y_z2 = 0;
}

// -----------------------------------------------------------------------------
// biquad filter
// -----------------------------------------------------------------------------
int32_t math_biquad_filter(math_biquad_t * filter, int32_t input){
    // Implements through Direct Form 1:
    //  H(z) = __(b0 + b1 z^-1 + b2 z^-2)__
    //           ( 1 + a1 z^-1 + a2 z^-2)
	int32_t temp;

    // y(n) = b0 x(n) + b1 x(-1) + b2 x(-2) -a1 y(-1) - a2 y(-2)
    temp  = filter->b0 * input;
    temp += filter->b1 * filter->x_z1;
    temp += filter->b2 * filter->x_z2;
    temp -= filter->a1 * filter->y_z1;
    temp -= filter->a2 * filter->y_z2;
	temp += filter->fraction;	// fraction saving
	filter->out = temp >> filter->q;

    // shift memories
    filter->x_z2 = filter->x_z1;
    filter->x_z1 = input;
    filter->y_z2 = filter->y_z1;
    filter->y_z1 = filter->out;
	filter->fraction = temp - (filter->out<<filter->q);

    // output
    return filter->out;
}

//------------------------------------------------------------------------------
// square root - from Wilco Dijkstra (ARM)
//------------------------------------------------------------------------------
int32_t math_sqrt(int32_t n) {
	#define iter1(N) try = root + (1 << (N)); \
	    if (n >= try << (N)) { n -= try << (N); root |= 2 << (N); }
	uint32_t root = 0, try;
	if (n<0) return 0;
    iter1 (15);    iter1 (14);    iter1 (13);    iter1 (12);
    iter1 (11);    iter1 (10);    iter1 ( 9);    iter1 ( 8);
    iter1 ( 7);    iter1 ( 6);    iter1 ( 5);    iter1 ( 4);
    iter1 ( 3);    iter1 ( 2);    iter1 ( 1);    iter1 ( 0);
    return root >> 1;
}

//------------------------------------------------------------------------------
// atan2 - based on Jim Shima self normalizing atan, angle in Q11
//------------------------------------------------------------------------------
int32_t math_atan2(int32_t y, int32_t x){
	int32_t ratio, angle_q11;
	if ((x==0)&&(y==0)) return 0; // undefined, return 0 degrees
	if (y>=0){
		if (x>=0){	// I quadrant
			// compute ratio = (y-x)/(x+y)
			ratio = (((y-x))<<14)/((x+y));
			ratio = ((1<<14)/2) + (ratio/2);
			ratio = (ratio * MATH_ATAN2_RATIO2OFFSET)>>MATH_ATAN2_RATIO2OFFSET_Q;
			// return angle
			angle_q11 = atan_table[ratio];
		} else {	// II quadrant
			// compute ratio = (y+x)/(-x+y)
			ratio = (((y+x))<<14)/((-x+y));
			ratio = ((1<<14)/2) + (ratio/2);
			ratio = (ratio * MATH_ATAN2_RATIO2OFFSET)>>MATH_ATAN2_RATIO2OFFSET_Q;
			// return angle
			angle_q11 = MATH_DEG_TO_RAD(180,11) - atan_table[ratio];
		}
	} else {	// y < 0
		if (x>=0) {	// IV quadrant
			// compute ratio = (-y-x)/(x-y)
			ratio = (((-y-x))<<14)/((x-y));
			ratio = ((1<<14)/2) + (ratio/2);
			ratio = (ratio * MATH_ATAN2_RATIO2OFFSET)>>MATH_ATAN2_RATIO2OFFSET_Q;
			// return angle
			angle_q11 = - atan_table[ratio];
		} else {	// III quadrant
			// compute ratio = (-y+x)/(-x-y)
			ratio = (((-y+x))<<14)/((-x-y));
			ratio = ((1<<14)/2) + (ratio/2);
			ratio = (ratio * MATH_ATAN2_RATIO2OFFSET)>>MATH_ATAN2_RATIO2OFFSET_Q;
			// return angle
			angle_q11 = - MATH_DEG_TO_RAD(180,11) + atan_table[ratio];
		}
	}
	return angle_q11;
}

//------------------------------------------------------------------------------
// abc to qd0, angle in Q11
//------------------------------------------------------------------------------
void math_abc_to_qd0(math_abcqd0_t * abcqd0, int32_t angle_q11){
	int32_t angle_m;
	int32_t angle_p;
	angle_m = angle_q11 - MATH_DEG_TO_RAD(120, 11);
	if (angle_m<0) angle_m += MATH_DEG_TO_RAD(360, 11);
	angle_p = angle_q11 + MATH_DEG_TO_RAD(120, 11);
	if (angle_p>=MATH_DEG_TO_RAD(360, 11)) angle_p -= MATH_DEG_TO_RAD(360, 11);
	// q = 2/3*(a*cos(ang) + b*cos(ang-120) + c*cos(ang+120))
	abcqd0->q  = (abcqd0->a*math_cos(angle_q11))  >>14;
	abcqd0->q += (abcqd0->b*math_cos(angle_m))>>14;
	abcqd0->q += (abcqd0->c*math_cos(angle_p))>>14;
	abcqd0->q  = (abcqd0->q*21845)>>15;
	// d = 2/3*(a*sin(ang) + b*sin(ang-120) + c*sin(ang+120))
	abcqd0->d  = (abcqd0->a*math_sin(angle_q11))  >>14;
	abcqd0->d += (abcqd0->b*math_sin(angle_m))>>14;
	abcqd0->d += (abcqd0->c*math_sin(angle_p))>>14;
	abcqd0->d  = (abcqd0->d*21845)>>15;
	// zero = 1/2*(a+b+c)
	abcqd0->zero = (abcqd0->a + abcqd0->b + abcqd0->c)>>1;
}

//------------------------------------------------------------------------------
// qd0 to abc, angle in Q11 - NO RANGE CHECK (use macro)
//------------------------------------------------------------------------------
void math_qd0_to_abc(math_abcqd0_t * abcqd0, int32_t angle_q11){
	int32_t angle_m;
	int32_t angle_p;
	angle_m = angle_q11 - MATH_DEG_TO_RAD(120, 11);
	if (angle_m<0) angle_m += MATH_DEG_TO_RAD(360, 11);
	angle_p = angle_q11 + MATH_DEG_TO_RAD(120, 11);
	if (angle_p>=MATH_DEG_TO_RAD(360, 11)) angle_p -= MATH_DEG_TO_RAD(360, 11);
	// a = q*cos(ang) + d*sin(ang) + zero
	abcqd0->a  = (abcqd0->q*math_cos(angle_q11))>>14;
	abcqd0->a += (abcqd0->d*math_sin(angle_q11))>>14;
	abcqd0->a += abcqd0->zero;
	// b = q*cos(ang-120) + d*sin(ang-120) + zero
	abcqd0->b  = (abcqd0->q*math_cos(angle_m))>>14;
	abcqd0->b += (abcqd0->d*math_sin(angle_m))>>14;
	abcqd0->b += abcqd0->zero;
	// c = q*cos(ang+120) + d*sin(ang+120) + zero
	abcqd0->c  = (abcqd0->q*math_cos(angle_p))>>14;
	abcqd0->c += (abcqd0->d*math_sin(angle_p))>>14;
	abcqd0->c += abcqd0->zero;
}

