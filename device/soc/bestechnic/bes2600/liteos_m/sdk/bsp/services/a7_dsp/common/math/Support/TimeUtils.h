/*
	Copyright (C) 2001-2021 Apple Inc. All Rights Reserved. Not to be used or disclosed without permission from Apple.
*/

#ifndef __TimeUtils_h__
#define __TimeUtils_h__

#include "CommonServices.h" // Include early for TARGET_* conditionals.
#include "DebugServices.h" // Include early for DEBUG_* conditionals.

#if (TARGET_HAS_STD_C_LIB)
#include <time.h>
#endif

#if (TARGET_OS_POSIX)
#include <sys/time.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if 0
#pragma mark == Conversions ==
#endif

//===========================================================================================================================
//	Conversions
//===========================================================================================================================

#define kNTPvsUnixSeconds 2208988800U // Seconds between 1900-01-01 00:00:00 UTC (NTP) and  1970-01-01 00:00:00 UTC (Unix).

#define kNTPvsUnixSeconds_FP 2208988800.0 // NTP vs Unix seconds as a floating-point value.

#if 0
#pragma mark == NTP ==
#endif

//===========================================================================================================================
//	NTP
//===========================================================================================================================

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	NTP32fromNTP64
     @abstract	Converts a 64-bit NTP timestamp format to a 32-bit NTP format specified by the RTP spec (middle 32 bits).
     */
#define NTP32fromNTP64(NTP_SECS, NTP_FRAC) \
    ((((NTP_SECS)&0x0000FFFF) << 16) | (((NTP_FRAC)&0xFFFF0000) >> 16))

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	NTPFractionFromMicroseconds
     @abstract	Converts microseconds to NTP fractional seconds (1/2^32 second units).
     @discussion
     
     This relies on the factorization of 2^32 / 10^6 being 2^12 + 2^8 - 1825/2^5, which results in a maximum conversion
     error of 3 * 10^-7 and an average error of half that.
     */
#define NTPFractionFromMicroseconds(X) \
    (((((uint32_t)(X)) << 12) + (((uint32_t)(X)) << 8)) - ((((uint32_t)(X)) * 1825) >> 5))

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	NTP64fromRelativeTimeVal
     @abstract	Converts a timeval structure containing a relative time (not 1970's based) to NTP time.
     */
#define NTP64fromRelativeTimeVal(TV_PTR, NTP_SECS_PTR, NTP_FRAC_PTR) \
    do {                                                             \
        uint32_t tmp;                                                \
                                                                     \
        *(NTP_SECS_PTR) = (TV_PTR)->tv_sec;                          \
        tmp = (uint32_t)((TV_PTR)->tv_usec);                         \
        *(NTP_FRAC_PTR) = NTPFractionFromMicroseconds(tmp);          \
                                                                     \
    } while (0)

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	NTP64fromTimeVal
     @abstract	Converts a timeval structure containing absolute time (Unix-style 1970's based) to NTP time.
     */
#define NTP64fromTimeVal(TV_PTR, NTP_SECS_PTR, NTP_FRAC_PTR)    \
    do {                                                        \
        uint32_t tmp;                                           \
                                                                \
        *(NTP_SECS_PTR) = (TV_PTR)->tv_sec + kNTPvsUnixSeconds; \
        tmp = (uint32_t)((TV_PTR)->tv_usec);                    \
        *(NTP_FRAC_PTR) = NTPFractionFromMicroseconds(tmp);     \
                                                                \
    } while (0)

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	TimeValToNTP64
     @abstract	Converts a timeval structure containing absolute time (Unix-style 1970's based) to 64-bit NTP time.
     */
#define TimeValToNTP64(TV_PTR) \
    (((((uint64_t)((TV_PTR)->tv_sec)) + kNTPvsUnixSeconds) << 32) | NTPFractionFromMicroseconds((TV_PTR)->tv_usec))

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	NTPSecondsFP
     @abstract	Converts a 64-bit NTP timestamp to a single, floating point seconds value.
     */
#define kNTPFraction (1.0 / 4294967296.0)
#define NTPSecondsFP(NTP_SECS, NTP_FRAC) (((double)(NTP_SECS)) + (((double)(NTP_FRAC)) * kNTPFraction))

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	NTPSecondsFP
     @abstract	Converts a 64-bit NTP timestamp to a single, floating point seconds value relative to Unix-style 1970 time.
     @discussion	Warning: Result will be negative if the time is before 1970-01-01 00:00:00.
     */
#define NTPSeconds1970FP(NTP_SECS, NTP_FRAC) (NTPSecondsFP(NTP_SECS, NTP_FRAC) - kNTPvsUnixSeconds_FP)

#if 0
#pragma mark == Misc ==
#endif

#define kNTPFraction (1.0 / 4294967296.0)

// Converts a 24-hour value to AM/PM strings.
#define Hour24ToAMPM(HOUR) (((HOUR) < 12) ? "AM" : "PM")

// Converts a 24-hour value (0-23) to a 12-hour value (0-12).
STATIC_INLINE int Hour24ToHour12(int inHour)
{
    if (inHour == 0)
        return (12);
    else if (inHour <= 12)
        return (inHour);
    return (inHour - 12);
}

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	clock_gettime
	@abstract	POSIX clock_gettime. See <http://www.opengroup.org/onlinepubs/007908799/xsh/clock_gettime.html>.
*/
#if (TARGET_OS_WINDOWS)

typedef int clockid_t;

#define CLOCK_REALTIME 1

int clock_gettime(clockid_t inClockID, struct timespec* outTS);
#endif

#if (TARGET_OS_VXWORKS || TARGET_PLATFORM_WICED || TARGET_OS_WINDOWS)
//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	GETTIMEOFDAY
	@abstract	Gets the current time. WARNING: this timeline may change via NTP so don't rely on it being consistent.
	
	@param		outTV		Receives current time.
	@param		inUnused	Unused timezone parameter. Pass NULL.
	
	@returns	Error code or 0 if successful.
*/
int GETTIMEOFDAY(struct timeval* outTV, void* inUnused);
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	YMD_HMStoSeconds
	@abstract	Converts a date/time as year, month, day, hour, minute, and second to a cumulative number of seconds.
	
	@param		inYear		Year starting from 0 (e.g. the year 2007 would be just be 2007).
	@param		inMonth		Month where 1=January and 12=December.
	@param		inDay		Day between 1-31 where 1 is the first day of the month.
	@param		inHour		Hour between 0-23 where 0 is 12:00 AM (beginning of the day).
	@param		inMinute	Minute between 0-59.
	@param		inSecond	Second between 0-62. 62 is allowed for double leap seconds (no longer allowed by POSIX).
	@param		inEpochDays	The number of cumulative days from 0 to act as the epoch (e.g. 719162 for 1970-01-01).
	
	@result		int64_t		Number of seconds between the specified date/time and the epoch.
*/

#define kDaysTo1900_01_01 693596 //! Number of days since 1900-01-01 (NTP epoch).
#define kDaysTo1904_01_01 695056 //! Number of days since 1904-01-01 (Mac epoch).
#define kDaysTo1970_01_01 719163 //! Number of days since 1970-01-01 (Unix epoch).
#define kDaysTo2001_01_01 730486 //! Number of days since 2001-01-01 (CoreFoundation epoch).

#define kDaysToNTPEpoch kDaysSince1900_01_01
#define kDaysToMacEpoch kDaysTo1904_01_01
#define kDaysToUnixEpoch kDaysTo1970_01_01
#define kDaysToCoreFoundationEpoch kDaysTo2001_01_01

int64_t YMD_HMStoSeconds(int inYear, int inMonth, int inDay, int inHour, int inMinute, int inSecond, int inEpochDays);
void SecondsToYMD_HMS(
    int64_t inSeconds,
    int* outYear,
    int* outMonth,
    int* outDay,
    int* outHour,
    int* outMinute,
    int* outSecond);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	TimeUtilsTest
	@abstract	Unit test.
*/
OSStatus TimeUtilsTest(void);

#ifdef __cplusplus
}
#endif

#endif // __TimeUtils_h__
