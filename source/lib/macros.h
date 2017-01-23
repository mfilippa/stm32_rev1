// -----------------------------------------------------------------------------
// macros.h - MPF 01/2017
// -----------------------------------------------------------------------------

#ifndef _MACROS_H_
#define _MACROS_H_

// float to q conversion macro
#define F2Q(v,q)	((int32_t)((float)v*((int32_t)1<<q)))
#define Q2F(v,q)  	((float)(v)/((float)((int32_t)1<<(int32_t)q)))

// array size macro
#define ARRAYSIZE(x)  (sizeof(x) / sizeof((x)[0]))

#endif // _MACROS_H_
