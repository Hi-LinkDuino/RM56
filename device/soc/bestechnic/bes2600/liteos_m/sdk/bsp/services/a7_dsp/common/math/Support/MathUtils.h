/*
	Copyright (C) 2001-2021 Apple Inc. All Rights Reserved. Not to be used or disclosed without permission from Apple.
*/

#ifndef __MathUtils_h__
#define __MathUtils_h__

#include "CommonServices.h"
#include "DebugServices.h"

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	iceil2
	@abstract	Rounds up to an integral power of 2 <= x. 0 and 1 are considered powers of 2.
	@discussion
	
	iceil2    is valid for 0 >= x <= 2^31.
*/
uint32_t iceil2(uint32_t x);

#if 0
#pragma mark == Statistics ==
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@group		Exponentially Weighted Moving Average (EWMA) for floating point.
	@abstract	See <http://en.wikipedia.org/wiki/Moving_average>.
*/
#define kEWMAFlags_StartWithFirstValue 0x00
#define kEWMAFlags_StartWithZero 0x01

typedef struct
{
    double alpha;
    double average;
    unsigned char flags;

} EWMA_FP_Data;

#define EWMA_FP_Init(DATA, ALPHA, FLAGS) \
    do {                                 \
        (DATA)->alpha = (ALPHA);         \
        (DATA)->average = 0;             \
        (DATA)->flags = (FLAGS);         \
                                         \
    } while (0)

#define EWMA_FP_Update(DATA, X)                                           \
    do {                                                                  \
        if ((DATA)->flags) {                                              \
            (DATA)->average += ((DATA)->alpha * ((X) - (DATA)->average)); \
        } else {                                                          \
            (DATA)->average = (X);                                        \
            (DATA)->flags = 1;                                            \
        }                                                                 \
                                                                          \
    } while (0)

#define EWMA_FP_Get(DATA) (DATA)->average

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	MovingAverageF
	@abstract	Floating point exponentially weighted moving average (EWMA).
	@discussion	See <http://en.wikipedia.org/wiki/Moving_average>.
	
	@param		AVG		Current average. This is often initialized to the first value.
	@param		X		New value to add to the average.
	@param		ALPHA	Smoothing factor between 0 and 1. To express in time periods: ALPHA = 2/(N+1), such as N=19: 2/(19/1) = 0.1.
	
	@result		New average.
*/
#define MovingAverageF(AVG, X, ALPHA) (((AVG) * (1 - (ALPHA))) + ((X) * (ALPHA)))

#if 0
#pragma mark == Debugging ==
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	MathUtils_Test
	@abstract	Unit test.
*/
#if (!EXCLUDE_UNIT_TESTS)
OSStatus MathUtils_Test(int inPrint);
#endif

#ifdef __cplusplus
}
#endif

#endif // __MathUtils_h__
