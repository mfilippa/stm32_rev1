// -----------------------------------------------------------------------------
// matlab wrapper h file - MPF 01/2017
// -----------------------------------------------------------------------------

#ifndef _MATLAB_H_
#define _MATLAB_H_

// includes
#include <stdint.h>

// global matlab ports
extern const double * pin;
extern double * pout;

// public prototypes
void wrapper_init(const double * pin, double * pout);
void wrapper_step(void);

#endif // _MATLAB_H_

