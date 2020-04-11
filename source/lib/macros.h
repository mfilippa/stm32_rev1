// -----------------------------------------------------------------------------
// macros.h - MPF 11/2019
// -----------------------------------------------------------------------------

#ifndef _MACROS_H_
#define _MACROS_H_

// includes
#include <stdint.h>

// float/double to q conversion macro
#define F2Q(v,q)    ((int32_t)((float)(v)*((float)((int32_t)1<<(int32_t)(q)))))
#define D2Q(v,q)    ((int32_t)((double)(v)*((double)((int32_t)1<<(int32_t)(q)))))
#define Q2F(v,q)    ((float)(v)/((float)((int32_t)1<<(int32_t)(q))))
#define Q2D(v,q)    ((double)(v)/((double)((int32_t)1<<(int32_t)(q))))

// array size macro
#define ARRAYSIZE(x)  (sizeof(x) / sizeof((x)[0]))

// store in circular buffer
#define CIRC_BUFFER_WRITE(value, buffer, index, buffer_size) \
    buffer[index]=(value);\
    index++;\
    if(index>=buffer_size) index=0;

#define DEBOUNCE(condition, count, limit, state) \
    if (state==0) { \
        if(condition) { \
            if(count<limit) count++; \
            else state=1; \
        } else count = 0; \
    } else { \
        if(!(condition)) { \
            if(count>0) count--; \
            else state=0; \
        } else count = limit; \
    }

#endif // _MACROS_H_
