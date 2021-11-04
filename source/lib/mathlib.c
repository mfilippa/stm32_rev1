// -----------------------------------------------------------------------------
// mathlib.c - MPF - 11/2020
// -----------------------------------------------------------------------------

// includes
#include "math_tables.h"
#include "mathlib.h"

// MANY OF THESE FUNCTIONS DO NOT PERFORM RANGE CHECK, TRADE OFF FOR SPEED
// RANGE CHECK BEFORE PASSING ARGUMENTS TO FUNCTION

// -----------------------------------------------------------------------------
// sin - angle mapped to q16, returns amplitude in Q14
// -----------------------------------------------------------------------------
int32_t math_sin(uint32_t angle_q16){
#ifdef MATH_SIN_COS_INTERPOLATION
    // with interpolation
    int32_t index, fract_q14, s1, s2;
    angle_q16 = MATH_ANGLE_RANGE_CHECK(angle_q16);
    index = angle_q16>>(16-MATH_SINE_TABLE_SIZE_Q);
    fract_q14 = (angle_q16 - (index<<(16-MATH_SINE_TABLE_SIZE_Q)))<<
        (14-(16-MATH_SINE_TABLE_SIZE_Q));
    s1 = sine_table[index];
    s2 = sine_table[index+1];
    return ((((1<<14)-fract_q14)*s1)>>14)+((fract_q14*s2)>>14);
#else
    // without interpolation
    return sine_table[MATH_ANGLE_RANGE_CHECK(angle_q16)>>
        (16-MATH_SINE_TABLE_SIZE_Q)];
#endif
}

// -----------------------------------------------------------------------------
// cos - angle mapped to q16, returns amplitude in Q14 
// -----------------------------------------------------------------------------
int32_t math_cos(uint32_t angle_q16){
    angle_q16 += MATH_DEG_TO_Q16(90);
    return sine_table[MATH_ANGLE_RANGE_CHECK(angle_q16)
        >>(16-MATH_SINE_TABLE_SIZE_Q)];
}

// -----------------------------------------------------------------------------
// table look up
// -----------------------------------------------------------------------------
int32_t math_table_lookup(int32_t value, const math_tlookup_t * table, 
    uint32_t tsize){
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
void math_biquad_init(math_biquad_t * filter, int32_t b0, int32_t b1, 
    int32_t b2, int32_t a1, int32_t a2, int32_t q){
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
    temp += filter->fraction;    // fraction saving
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
    // int32_t ratio, angle_q11;
    // if ((x==0)&&(y==0)) return 0; // undefined, return 0 degrees
    // if (y>=0){
    //     if (x>=0){    // I quadrant
    //         // compute ratio = (y-x)/(x+y)
    //         ratio = (((y-x))<<14)/((x+y));
    //         ratio = ((1<<14)/2) + (ratio/2);
    //         ratio = (ratio * MATH_ATAN2_RATIO2OFFSET)>>MATH_ATAN2_RATIO2OFFSET_Q;
    //         // return angle
    //         angle_q11 = atan_table[ratio];
    //     } else {    // II quadrant
    //         // compute ratio = (y+x)/(-x+y)
    //         ratio = (((y+x))<<14)/((-x+y));
    //         ratio = ((1<<14)/2) + (ratio/2);
    //         ratio = (ratio * MATH_ATAN2_RATIO2OFFSET)>>MATH_ATAN2_RATIO2OFFSET_Q;
    //         // return angle
    //         angle_q11 = MATH_DEG_TO_Q16(180,11) - atan_table[ratio];
    //     }
    // } else {    // y < 0
    //     if (x>=0) {    // IV quadrant
    //         // compute ratio = (-y-x)/(x-y)
    //         ratio = (((-y-x))<<14)/((x-y));
    //         ratio = ((1<<14)/2) + (ratio/2);
    //         ratio = (ratio * MATH_ATAN2_RATIO2OFFSET)>>MATH_ATAN2_RATIO2OFFSET_Q;
    //         // return angle
    //         angle_q11 = - atan_table[ratio];
    //     } else {    // III quadrant
    //         // compute ratio = (-y+x)/(-x-y)
    //         ratio = (((-y+x))<<14)/((-x-y));
    //         ratio = ((1<<14)/2) + (ratio/2);
    //         ratio = (ratio * MATH_ATAN2_RATIO2OFFSET)>>MATH_ATAN2_RATIO2OFFSET_Q;
    //         // return angle
    //         angle_q11 = - MATH_DEG_TO_Q16(180,11) + atan_table[ratio];
    //     }
    // }
    // return angle_q11;
    return 0;
}

//------------------------------------------------------------------------------
// abc to qd0, angle mapped to q16
//------------------------------------------------------------------------------
void math_abc_to_qd0(math_abcqd0_t * abcqd0, uint32_t angle_q16){
    int32_t angle_m;
    int32_t angle_p;
    angle_m = angle_q16 - MATH_DEG_TO_Q16(120);
    angle_p = angle_q16 + MATH_DEG_TO_Q16(120);
    // q = 2/3*(a*cos(ang) + b*cos(ang-120) + c*cos(ang+120))
    abcqd0->q  = (abcqd0->a*math_cos(angle_q16))  >>14;
    abcqd0->q += (abcqd0->b*math_cos(angle_m))>>14;
    abcqd0->q += (abcqd0->c*math_cos(angle_p))>>14;
    abcqd0->q  = (abcqd0->q*21845)>>15;
    // d = 2/3*(a*sin(ang) + b*sin(ang-120) + c*sin(ang+120))
    abcqd0->d  = (abcqd0->a*math_sin(angle_q16))  >>14;
    abcqd0->d += (abcqd0->b*math_sin(angle_m))>>14;
    abcqd0->d += (abcqd0->c*math_sin(angle_p))>>14;
    abcqd0->d  = (abcqd0->d*21845)>>15;
    // zero = 1/2*(a+b+c)
    abcqd0->zero = (abcqd0->a + abcqd0->b + abcqd0->c)>>1;
}

//------------------------------------------------------------------------------
// qd0 to abc, angle mapped to q16
//------------------------------------------------------------------------------
void math_qd0_to_abc(math_abcqd0_t * abcqd0, uint32_t angle_q16){
    int32_t angle_m;
    int32_t angle_p;
    angle_m = angle_q16 - MATH_DEG_TO_Q16(120);
    angle_p = angle_q16 + MATH_DEG_TO_Q16(120);
    // a = q*cos(ang) + d*sin(ang) + zero
    abcqd0->a  = (abcqd0->q*math_cos(angle_q16))>>14;
    abcqd0->a += (abcqd0->d*math_sin(angle_q16))>>14;
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

//------------------------------------------------------------------------------
// math exp
//------------------------------------------------------------------------------
#define MATH_EXP_TSIZE  10
math_tlookup_t math_exp_table[MATH_EXP_TSIZE] = {
        {0,8192,8612,14},
        {1638,9053,9522,14},
        {3276,10005,10526,14},
        {4915,11058,11632,14},
        {6553,12221,12845,14},
        {8192,13506,14203,14},
        {9830,14926,15703,14},
        {11468,16496,17343,14},
        {13107,18231,19184,14},
        {14745,20149,21182,14},
};
int32_t math_fast_table_lookup(int32_t value, const math_tlookup_t * table,
        uint32_t tsize){
uint32_t index = (tsize * value)>>14;
if (index>=tsize) return 0;
return table[index].y + ((table[index].slope*(value-table[index].x))>>
                table[index].slope_q);
}
int32_t math_exp(int32_t value){
    return math_fast_table_lookup(value, math_exp_table, MATH_EXP_TSIZE);
}

//------------------------------------------------------------------------------
// step angle
//------------------------------------------------------------------------------
void math_step_angle(uint32_t * angle_q16, uint32_t * angle_fraction, 
    int32_t frequency, uint32_t frequency_q, int32_t deltaT, uint32_t deltaT_q){
    
    int32_t temp, step_q14;

    // calculates angle += 2 pi freq dt

    // first calculate multiplication freq x dt, store in q14 to scale angle
    temp = frequency*deltaT+*angle_fraction;
    step_q14 = temp >>(frequency_q+deltaT_q-14);

    // save fraction
    *angle_fraction = temp - (step_q14<<(frequency_q+deltaT_q-14));

    // finally add to angle
    temp = (((1<<16)*step_q14)>>14);
    *angle_q16 = MATH_ANGLE_RANGE_CHECK(*angle_q16+temp);
}