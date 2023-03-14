/*
	Copyright (C) 2001-2021 Apple Inc. All Rights Reserved. Not to be used or disclosed without permission from Apple.
*/
/*!
    @header		Tick API
    @discussion APIs for providing a high-resolution, low-latency tick counter and conversions.
*/

#ifndef __TickUtils_h__
#define __TickUtils_h__

#if (defined(TickUtils_PLATFORM_HEADER))
#include TickUtils_PLATFORM_HEADER
#endif

#include "CommonServices.h" // Include early for TARGET_* conditionals.
#include "DebugServices.h" // Include early for DEBUG_* conditionals.

#if (TARGET_HAS_STD_C_LIB)
#include <time.h>
#endif

#if (TARGET_MACH)
#include <mach/mach_time.h>
#endif

#if (TARGET_OS_POSIX)
#include <sys/time.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@group		Tick counter access.
	@abstract	Provides access to the raw tick counter, ticks per second, and convenience functions for common units.
	@discussion
	
	Primitive functions:
	
	uint64_t	UpTicks( void );			// Monotonically increasing number of ticks since the system started.
	uint64_t	UpTicksPerSecond( void );	// Number of ticks per second.
	
	If your platform is not already supported then you can implement UpTicks() and UpTicksPerSecond() in your own file
	and link it in. Alternatively, if you have an existing API and want to avoid the overhead of wrapping your function 
	then you can define TickUtils_PLATFORM_HEADER to point to your custom header file and inside that file you can 
	define UpTicks() and UpTicksPerSecond() to point to your existing functions. This assumes they are API compatible.
	
	Convenience functions if a platform has a more efficient way of provide time in standard units:
	
	uint64_t	UpSeconds( void );			// Monotonically increasing number of seconds since the system started.
	uint64_t	UpMilliseconds( void );		// Monotonically increasing number of milliseconds since the system started.
	uint64_t	UpMicroseconds( void );		// Monotonically increasing number of microseconds since the system started.
	uint64_t	UpNanoseconds( void );		// Monotonically increasing number of nanoseconds since the system started.
	
	If the platform provides versions of the convenience functions then TickUtils_CONVERSION_OVERRIDES is defined to 1.
	If the platform doesn't provide these functions, built-in conversions are provided using the primitive functions.
	
	Implementors of these APIs must be careful to avoid temporary integer overflows. Even with 64-bit values, it's 
	easy to exceed the range of a 64-bit value when conversion to/from very small units or very large counts.
*/
#if (TARGET_OS_FREEBSD || TARGET_OS_LINUX || TARGET_OS_QNX)
uint64_t UpTicks(void);
#define UpTicksPerSecond() UINT64_C_safe(kNanosecondsPerSecond)

#define UpSeconds() (UpTicks() / kNanosecondsPerSecond)
#define UpMilliseconds() (UpTicks() / kNanosecondsPerMillisecond)
#define UpMicroseconds() (UpTicks() / kNanosecondsPerMicrosecond)
#define UpNanoseconds() UpTicks()
#define TickUtils_CONVERSION_OVERRIDES 1
#elif (TARGET_MACH)
#define UpTicks() mach_absolute_time()
uint64_t UpTicksPerSecond(void);
#elif (TARGET_OS_NETBSD)
#define UpTicks() upticks()
#if (TARGET_KERNEL)
#define UpTicksPerSecond() upticksrate
#else
uint64_t UpTicksPerSecond(void);
#endif
#elif (TARGET_OS_THREADX)
#define UpTicks() ((uint64_t)tx_time_get())
#define UpTicksPerSecond() UINT64_C_safe(kMillisecondsPerSecond)

#define UpSeconds() (UpTicks() / kMillisecondsPerSecond)
#define UpMilliseconds() UpTicks()
#define UpMicroseconds() (UpTicks() * kMicrosecondsPerMillisecond)
#define UpNanoseconds() (UpTicks() * kNanosecondsPerMillisecond)
#define TickUtils_CONVERSION_OVERRIDES 1
#elif (TARGET_OS_WINDOWS)
uint64_t UpTicks(void);
uint64_t UpTicksPerSecond(void);
#else
#if (!defined(UpTicks))
uint64_t UpTicks(void);
#endif
#if (!defined(UpTicksPerSecond))
uint64_t UpTicksPerSecond(void);
#endif
#endif

#define UpTicks32() ((uint32_t)(UpTicks() & UINT32_C(0xFFFFFFFF)))

#if (!defined(TickUtils_CONVERSION_OVERRIDES))
#define TickUtils_CONVERSION_OVERRIDES 0
#endif
#if (!TickUtils_CONVERSION_OVERRIDES)
#define UpSeconds() UpTicksToSeconds(UpTicks())
#define UpMilliseconds() UpTicksToMilliseconds(UpTicks())
#define UpMicroseconds() UpTicksToMicroseconds(UpTicks())
#define UpNanoseconds() UpTicksToNanoseconds(UpTicks())
#endif

#define UpNTP() UpTicksToNTP(UpTicks())

//---------------------------------------------------------------------------------------------------------------------------
/*!	@group		Sleep APIs
	@abstract	APIs for blocking the current thread with minimal CPU usage while sleeping.
	@discussion
	
	SleepForUpTicks( uint32_t inTicks );	// Sleep for the specified number of ticks.
	SleepUntilUpTicks( uint32_t inTicks );	// Sleep until the specified tick deadline has been reached.
	
	If the platform provides the sleep functions then TickUtils_HAS_SLEEP_PRIMITIVE is defined to 1.
	
	Implementors should be careful to avoid premature returns on signals or for other interruptions.
	For example, the POSIX nanosleep returns prematurely with EINT on signals and must be repeated for the remaining time.
*/
#if (TARGET_OS_POSIX || TARGET_OS_WINDOWS)
void SleepForUpTicks(uint64_t inTicks);
void SleepUntilUpTicks(uint64_t inTicks);
#define TickUtils_HAS_SLEEP_PRIMITIVE 1
#elif (TARGET_OS_THREADX)
#define SleepForUpTicks(TICKS) tx_thread_sleep((ULONG)(TICKS))
void SleepUntilUpTicks(uint64_t inTicks);
#define TickUtils_HAS_SLEEP_PRIMITIVE 1
#endif

#define kUpTicksNow UINT64_C(0)
#define kUpTicksForever UINT64_C(0xFFFFFFFFFFFFFFFF)

//---------------------------------------------------------------------------------------------------------------------------
/*!	@group		UpTicks Conversions
	@abstract	Conversions between UpTicks and other units.
*/
uint64_t UpTicksToSeconds(uint64_t inTicks);
double UpTicksToSecondsF(uint64_t inTicks);
uint64_t UpTicksToMilliseconds(uint64_t inTicks);
uint64_t UpTicksToMicroseconds(uint64_t inTicks);
uint64_t UpTicksToNanoseconds(uint64_t inTicks);
uint64_t UpTicksToNTP(uint64_t inTicks);

uint64_t SecondsToUpTicks(uint64_t x);
uint64_t SecondsToUpTicksF(double x);
uint64_t MillisecondsToUpTicks(uint64_t x);
uint64_t MicrosecondsToUpTicks(uint64_t x);
uint64_t NanosecondsToUpTicks(uint64_t x);
uint64_t NTPtoUpTicks(uint64_t inNTP);

#if (!TARGET_OS_THREADX)
//---------------------------------------------------------------------------------------------------------------------------
/*! @function    UpTicksToTimeValTimeout
        @abstract    Converts an absolute, UpTicks deadline to a timeval timeout, suitable for passing to APIs like select.
        @discussion    This handles deadlines that have already expired (immediate timeout) and kUpTicksForever (no timeout).
    */
struct timeval* UpTicksToTimeValTimeout(uint64_t inDeadline, struct timeval* inTimeVal);
#endif

#if (!EXCLUDE_UNIT_TESTS)
//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	TickUtilsTest
        @abstract	Unit test.
    */
OSStatus TickUtilsTest(void);
#endif

#ifdef __cplusplus
}
#endif

#endif // __TickUtils_h__
