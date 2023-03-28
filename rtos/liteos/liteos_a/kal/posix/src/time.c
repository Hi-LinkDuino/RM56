/*
 * Copyright (c) 2013-2019 Huawei Technologies Co., Ltd. All rights reserved.
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#define _GNU_SOURCE
#include <time.h>
#include <sys/time.h>
#include <stdint.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include "los_debug.h"
#include "los_task.h"
#include "los_swtmr.h"
#include "los_timer.h"
#include "los_context.h"

#ifndef STATIC
#define STATIC static
#endif

#define OS_SYS_NS_PER_US 1000
#define OS_SYS_NS_PER_SECOND 1000000000
#define OS_SYS_US_PER_SECOND 1000000
#define OS_SYS_MS_PER_SECOND 1000

#define TM_YEAR_BASE         1900
#define EPOCH_YEAR           1970
#define SECS_PER_MIN         60
#define MINS_PER_HOUR        60
#define SECS_PER_HOUR        3600  /* 60 * 60 */
#define SECS_PER_DAY         86400 /* 60 * 60 * 24 */
#define SECS_PER_NORMAL_YEAR 31536000 /* 60 * 60 * 24 * 365 */
#define DAYS_PER_WEEK        7
#define DAYS_PER_NORMAL_YEAR 365
#define DAYS_PER_LEAP_YEAR   366
#define BEGIN_WEEKDAY        4
#define TIME_ZONE_MAX        720 /* 12 * 60 */
#define TIME_ZONE_MIN        (-840) /* -14 * 60 */

/* accumulative time delta from discontinuous modify */
STATIC struct timespec g_accDeltaFromSet;

STATIC const UINT16 g_daysInMonth[2][13] = {
    /* Normal years.  */
    { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 },
    /* Leap years.  */
    { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 }
};

STATIC const UINT8 g_montbl[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

/*
 * Nonzero if YEAR is a leap year (every 4 years,
 * except every 100th isn't, and every 400th is).
 */
#ifndef IS_LEAP_YEAR
#define IS_LEAP_YEAR(year) \
    (((year) % 4 == 0) && (((year) % 100 != 0) || ((year) % 400 == 0)))
#endif
/* The lowest two bytes indicate minutes of the time zone */
#ifndef OFFSET_TO_MINUTE
#define OFFSET_TO_MINUTE(time) (((time) < 0) ? (-(time)) : (time))
#endif
/* The highest 31 bytes, 1 indicates eastern time zone，0 indicates western time zone */
#ifndef TIME_ZONE_SIGN
#define TIME_ZONE_SIGN(time) ((time) >> 31)
#endif

#define DIV(a, b) (((a) / (b)) - ((a) % (b) < 0))
#define LEAPS_THRU_END_OF(y) (DIV (y, 4) - DIV (y, 100) + DIV (y, 400))

static UINT64 g_rtcTimeBase = 0;
static UINT64 g_systickBase = 0;

/*
 * Time zone information, stored in minutes,
 * negative values indicate the east of UTC,
 * positive values indicate the west of UTC.
 */
static INT32 g_rtcTimeZone = -480;
static struct tm g_tm = {0};

/* internal functions */
STATIC INLINE BOOL ValidTimeSpec(const struct timespec *tp)
{
    /* Fail a NULL pointer */
    if (tp == NULL) {
        return FALSE;
    }

    /* Fail illegal nanosecond values */
    if ((tp->tv_nsec < 0) || (tp->tv_nsec >= OS_SYS_NS_PER_SECOND) || (tp->tv_sec < 0)) {
        return FALSE;
    }

    return TRUE;
}

STATIC INLINE UINT32 OsTimeSpec2Tick(const struct timespec *tp)
{
    UINT64 tick, ns;

    ns = (UINT64)tp->tv_sec * OS_SYS_NS_PER_SECOND + tp->tv_nsec;
    /* Round up for ticks */
    tick = (ns * LOSCFG_BASE_CORE_TICK_PER_SECOND + (OS_SYS_NS_PER_SECOND - 1)) / OS_SYS_NS_PER_SECOND;
    if (tick > LOS_WAIT_FOREVER) {
        tick = LOS_WAIT_FOREVER;
    }
    return (UINT32)tick;
}

STATIC INLINE VOID OsTick2TimeSpec(struct timespec *tp, UINT32 tick)
{
    UINT64 ns = ((UINT64)tick * OS_SYS_NS_PER_SECOND) / LOSCFG_BASE_CORE_TICK_PER_SECOND;
    tp->tv_sec = (time_t)(ns / OS_SYS_NS_PER_SECOND);
    tp->tv_nsec = (long)(ns % OS_SYS_NS_PER_SECOND);
}

int nanosleep(const struct timespec *rqtp, struct timespec *rmtp)
{
    UINT64 nseconds;
    UINT64 tick;
    UINT32 ret;
    const UINT32 nsPerTick = OS_SYS_NS_PER_SECOND / LOSCFG_BASE_CORE_TICK_PER_SECOND;

    if (!ValidTimeSpec(rqtp)) {
        errno = EINVAL;
        return -1;
    }

    nseconds = (UINT64)rqtp->tv_sec * OS_SYS_NS_PER_SECOND + rqtp->tv_nsec;

    tick = (nseconds + nsPerTick - 1) / nsPerTick; // Round up for ticks

    if (tick >= UINT32_MAX) {
        errno = EINVAL;
        return -1;
    }

    /* PS: skip the first tick because it is NOT a full tick. */
    ret = LOS_TaskDelay(tick ? (UINT32)(tick + 1) : 0);
    if (ret == LOS_OK || ret == LOS_ERRNO_TSK_YIELD_NOT_ENOUGH_TASK) {
        if (rmtp) {
            rmtp->tv_sec = rmtp->tv_nsec = 0;
        }
        return 0;
    }

    /* sleep in interrupt context or in task sched lock state */
    errno = EPERM;
    return -1;
}

int timer_create(clockid_t clockID, struct sigevent *restrict evp, timer_t *restrict timerID)
{
    UINT32 ret;
    UINT32 swtmrID;

    if (!timerID || (clockID != CLOCK_REALTIME)) {
        errno = EINVAL;
        return -1;
    }

    if (!evp || evp->sigev_notify != SIGEV_THREAD || evp->sigev_notify_attributes) {
        errno = ENOTSUP;
        return -1;
    }

    ret = LOS_SwtmrCreate(1, LOS_SWTMR_MODE_ONCE, (SWTMR_PROC_FUNC)evp->sigev_notify_function,
                          &swtmrID, (UINT32)(UINTPTR)evp->sigev_value.sival_ptr
#if (LOSCFG_BASE_CORE_SWTMR_ALIGN == 1)
                          , OS_SWTMR_ROUSES_IGNORE, OS_SWTMR_ALIGN_INSENSITIVE
#endif
    );
    if (ret != LOS_OK) {
        errno = (ret == LOS_ERRNO_SWTMR_MAXSIZE) ? EAGAIN : EINVAL;
        return -1;
    }

    *timerID = (timer_t)(UINTPTR)swtmrID;
    return 0;
}

int timer_delete(timer_t timerID)
{
    UINT32 swtmrID = (UINT32)(UINTPTR)timerID;
    if (LOS_SwtmrDelete(swtmrID) != LOS_OK) {
        errno = EINVAL;
        return -1;
    }

    return 0;
}

int timer_settime(timer_t timerID, int flags,
                  const struct itimerspec *restrict value,
                  struct itimerspec *restrict oldValue)
{
    UINTPTR intSave;
    UINT32 swtmrID = (UINT32)(UINTPTR)timerID;
    SWTMR_CTRL_S *swtmr = NULL;
    UINT32 interval, expiry, ret;

    if (flags != 0) {
        /* flags not supported currently */
        errno = ENOSYS;
        return -1;
    }

    if (value == NULL) {
        errno = EINVAL;
        return -1;
    }

    if (!ValidTimeSpec(&value->it_value) || !ValidTimeSpec(&value->it_interval)) {
        errno = EINVAL;
        return -1;
    }

    expiry = OsTimeSpec2Tick(&value->it_value);
    interval = OsTimeSpec2Tick(&value->it_interval);

    /* if specified interval, it must be same with expiry due to the limitation of liteos-m */
    if (interval && interval != expiry) {
        errno = ENOTSUP;
        return -1;
    }

    if (oldValue) {
        (VOID)timer_gettime(timerID, oldValue);
    }

    ret = LOS_SwtmrStop(swtmrID);
    if ((ret != LOS_OK) && (ret != LOS_ERRNO_SWTMR_NOT_STARTED)) {
        errno = EINVAL;
        return -1;
    }

    intSave = LOS_IntLock();
    swtmr = OS_SWT_FROM_SID(swtmrID);
    swtmr->ucMode = (interval ? LOS_SWTMR_MODE_PERIOD : LOS_SWTMR_MODE_NO_SELFDELETE);
    swtmr->uwInterval = interval;
    LOS_IntRestore(intSave);

    if ((value->it_value.tv_sec == 0) && (value->it_value.tv_nsec == 0)) {
        /*
         * 1) when expiry is 0, means timer should be stopped.
         * 2) If timer is ticking, stopping timer is already done before.
         * 3) If timer is created but not ticking, return 0 as well.
         */
        return 0;
    }

    if (LOS_SwtmrStart(swtmr->usTimerID) != LOS_OK) {
        errno = EINVAL;
        return -1;
    }

    return 0;
}

int timer_gettime(timer_t timerID, struct itimerspec *value)
{
    UINT32 tick = 0;
    SWTMR_CTRL_S *swtmr = NULL;
    UINT32 swtmrID = (UINT32)(UINTPTR)timerID;
    UINT32 ret;

    if (value == NULL) {
        errno = EINVAL;
        return -1;
    }

    swtmr = OS_SWT_FROM_SID(swtmrID);

    /* get expire time */
    ret = LOS_SwtmrTimeGet(swtmr->usTimerID, &tick);
    if ((ret != LOS_OK) && (ret != LOS_ERRNO_SWTMR_NOT_STARTED)) {
        errno = EINVAL;
        return -1;
    }

    OsTick2TimeSpec(&value->it_value, tick);
    OsTick2TimeSpec(&value->it_interval, (swtmr->ucMode == LOS_SWTMR_MODE_ONCE) ? 0 : swtmr->uwInterval);
    return 0;
}

int timer_getoverrun(timer_t timerID)
{
    (void)timerID;

    errno = ENOSYS;
    return -1;
}

STATIC VOID OsGetHwTime(struct timespec *hwTime)
{
    UINT64 nowNsec;
    UINT32 countHigh = 0;
    UINT32 countLow = 0;
    HalGetCpuCycle(&countHigh, &countLow);
    nowNsec = (((UINT64)countHigh * OS_SYS_NS_PER_SECOND / OS_SYS_CLOCK) << 32) +
              ((((UINT64)countHigh * OS_SYS_NS_PER_SECOND % OS_SYS_CLOCK) << 32) / OS_SYS_CLOCK) +
              ((UINT64)countLow * OS_SYS_NS_PER_SECOND / OS_SYS_CLOCK);
    hwTime->tv_sec = nowNsec / OS_SYS_NS_PER_SECOND;
    hwTime->tv_nsec = nowNsec % OS_SYS_NS_PER_SECOND;
}

STATIC VOID OsGetRealTime(struct timespec *realTime)
{
    UINTPTR intSave;
    struct timespec hwTime = {0};
    OsGetHwTime(&hwTime);
    intSave = LOS_IntLock();
    realTime->tv_nsec = hwTime.tv_nsec + g_accDeltaFromSet.tv_nsec;
    realTime->tv_sec = hwTime.tv_sec + g_accDeltaFromSet.tv_sec + (realTime->tv_nsec >= OS_SYS_NS_PER_SECOND);
    realTime->tv_nsec %= OS_SYS_NS_PER_SECOND;
    LOS_IntRestore(intSave);
}

STATIC VOID OsSetRealTime(const struct timespec *realTime)
{
    UINTPTR intSave;
    struct timespec hwTime = {0};
    OsGetHwTime(&hwTime);
    intSave = LOS_IntLock();
    g_accDeltaFromSet.tv_nsec = realTime->tv_nsec - hwTime.tv_nsec;
    g_accDeltaFromSet.tv_sec = realTime->tv_sec - hwTime.tv_sec - (g_accDeltaFromSet.tv_nsec < 0);
    g_accDeltaFromSet.tv_nsec = (g_accDeltaFromSet.tv_nsec + OS_SYS_NS_PER_SECOND) % OS_SYS_NS_PER_SECOND;
    LOS_IntRestore(intSave);
}

int clock_settime(clockid_t clockID, const struct timespec *tp)
{
    if (!ValidTimeSpec(tp)) {
        errno = EINVAL;
        return -1;
    }

    switch (clockID) {
        case CLOCK_REALTIME:
            /* we only support the realtime clock currently */
            OsSetRealTime(tp);
            return 0;
        case CLOCK_MONOTONIC_COARSE:
        case CLOCK_REALTIME_COARSE:
        case CLOCK_MONOTONIC_RAW:
        case CLOCK_PROCESS_CPUTIME_ID:
        case CLOCK_BOOTTIME:
        case CLOCK_REALTIME_ALARM:
        case CLOCK_BOOTTIME_ALARM:
        case CLOCK_SGI_CYCLE:
        case CLOCK_TAI:
        case CLOCK_THREAD_CPUTIME_ID:
            errno = ENOTSUP;
            return -1;
        case CLOCK_MONOTONIC:
        default:
            errno = EINVAL;
            return -1;
    }
}

int clock_gettime(clockid_t clockID, struct timespec *tp)
{
    if (tp == NULL) {
        errno = EINVAL;
        return -1;
    }

    switch (clockID) {
        case CLOCK_MONOTONIC_RAW:
        case CLOCK_MONOTONIC:
        case CLOCK_MONOTONIC_COARSE:
            OsGetHwTime(tp);
            return 0;
        case CLOCK_REALTIME:
        case CLOCK_REALTIME_COARSE:
            OsGetRealTime(tp);
            return 0;
        case CLOCK_THREAD_CPUTIME_ID:
        case CLOCK_PROCESS_CPUTIME_ID:
        case CLOCK_BOOTTIME:
        case CLOCK_REALTIME_ALARM:
        case CLOCK_BOOTTIME_ALARM:
        case CLOCK_SGI_CYCLE:
        case CLOCK_TAI:
            errno = ENOTSUP;
            return -1;
        default:
            errno = EINVAL;
            return -1;
    }
}

int clock_getres(clockid_t clockID, struct timespec *tp)
{
    if (tp == NULL) {
        errno = EINVAL;
        return -1;
    }

    switch (clockID) {
        case CLOCK_MONOTONIC_RAW:
        case CLOCK_MONOTONIC:
        case CLOCK_REALTIME:
        case CLOCK_MONOTONIC_COARSE:
        case CLOCK_REALTIME_COARSE:
            tp->tv_nsec = OS_SYS_NS_PER_SECOND / OS_SYS_CLOCK;
            tp->tv_sec = 0;
            return 0;
        case CLOCK_THREAD_CPUTIME_ID:
        case CLOCK_PROCESS_CPUTIME_ID:
        case CLOCK_BOOTTIME:
        case CLOCK_REALTIME_ALARM:
        case CLOCK_BOOTTIME_ALARM:
        case CLOCK_SGI_CYCLE:
        case CLOCK_TAI:
            errno = ENOTSUP;
            return -1;
        default:
            errno = EINVAL;
            return -1;
    }
}

int clock_nanosleep(clockid_t clk, int flags, const struct timespec *req, struct timespec *rem)
{
    switch (clk) {
        case CLOCK_REALTIME:
            if (flags == 0) {
                /* we only support the realtime clock currently */
                return nanosleep(req, rem);
            }
            /* fallthrough */
        case CLOCK_MONOTONIC_COARSE:
        case CLOCK_REALTIME_COARSE:
        case CLOCK_MONOTONIC_RAW:
        case CLOCK_MONOTONIC:
        case CLOCK_PROCESS_CPUTIME_ID:
        case CLOCK_BOOTTIME:
        case CLOCK_REALTIME_ALARM:
        case CLOCK_BOOTTIME_ALARM:
        case CLOCK_SGI_CYCLE:
        case CLOCK_TAI:
            if (flags == 0 || flags == TIMER_ABSTIME) {
                return ENOTSUP;
            }
            /* fallthrough */
        case CLOCK_THREAD_CPUTIME_ID:
        default:
            return EINVAL;
    }
}

clock_t clock(void)
{
    return HalGetExpandTick();
}

time_t time(time_t *timer)
{
    UINT64 usec = 0;
    time_t sec;
    INT32 rtcRet;

    rtcRet = HalGetRtcTime(&usec);
    if (rtcRet != 0) {
        UINT64 currentTime;
        UINT64 tickDelta;
        UINT64 currentTick = HalGetExpandTick();
        if ((g_systickBase != 0) && (currentTick > g_systickBase)) {
            tickDelta = currentTick - g_systickBase;
        }
        currentTime = g_rtcTimeBase + tickDelta;
        sec = currentTime / OS_SYS_MS_PER_SECOND;
    } else {
        sec = usec / OS_SYS_US_PER_SECOND;
    }

    if (timer != NULL) {
        *timer = sec;
    }
    return sec;
}

/*
 * Compute the `struct tm' representation of T,
 * offset OFFSET seconds east of UTC,
 * and store year, yday, mon, mday, wday, hour, min, sec into *TP.
 * Return nonzero if successful.
 */
static INT32 ConvertSecs2Utc(time_t t, INT32 offset, struct tm *tp)
{
    time_t days;
    time_t rem;
    time_t year;
    time_t month;
    time_t yearGuess;

    days = t / SECS_PER_DAY;
    rem = t % SECS_PER_DAY;
    rem += offset;
    while (rem < 0) {
        rem += SECS_PER_DAY;
        --days;
    }
    while (rem >= SECS_PER_DAY) {
        rem -= SECS_PER_DAY;
        ++days;
    }
    tp->tm_hour = rem / SECS_PER_HOUR;
    rem %= SECS_PER_HOUR;
    tp->tm_min = rem / SECS_PER_MIN;
    tp->tm_sec = rem % SECS_PER_MIN;
    /* January 1, 1970 was a Thursday.  */
    tp->tm_wday = (BEGIN_WEEKDAY + days) % DAYS_PER_WEEK;
    if (tp->tm_wday < 0) {
        tp->tm_wday += DAYS_PER_WEEK;
    }
    year = EPOCH_YEAR;

    while ((days < 0) ||
           (days >= (IS_LEAP_YEAR (year) ? DAYS_PER_LEAP_YEAR : DAYS_PER_NORMAL_YEAR))) {
        /* Guess a corrected year, assuming 365 days per year.  */
        yearGuess = year + days / DAYS_PER_NORMAL_YEAR - (days % DAYS_PER_NORMAL_YEAR < 0);

        /* Adjust days and year to match the guessed year.  */
        days -= ((yearGuess - year) * DAYS_PER_NORMAL_YEAR +
                 LEAPS_THRU_END_OF (yearGuess - 1) -
                 LEAPS_THRU_END_OF (year - 1));
        year = yearGuess;
    }
    tp->tm_year = year - TM_YEAR_BASE;
    if (tp->tm_year != year - TM_YEAR_BASE) {
        return 0;
    }
    tp->tm_yday = days;
    const UINT16 *daysInMonth = g_daysInMonth[IS_LEAP_YEAR(year)];
    /* valid month value is 0-11 */
    for (month = 11; days < (long int) daysInMonth[month]; --month) {
        continue;
    }
    days -= daysInMonth[month];
    tp->tm_mon = month;
    tp->tm_mday = days + 1;
    return 1;
}

struct tm *gmtime_r(const time_t *timer, struct tm *tp)
{
    time_t t64;
    UINT32 intSave;
    if ((timer == NULL) || (tp == NULL)) {
        return NULL;
    }
    intSave = LOS_IntLock();
    t64 = *timer;
    if (!ConvertSecs2Utc(t64, 0, tp)) {
        tp = NULL;
    }
    (void)LOS_IntRestore(intSave);
    return tp;
}

struct tm *gmtime(const time_t *timer)
{
    return gmtime_r(timer, &g_tm);
}

struct tm *localtime_r(const time_t *timer, struct tm *tp)
{
    UINT32 intSave;
    time_t t64;
    INT32 offset;
    if ((timer == NULL) || (tp == NULL)) {
        return NULL;
    }
    intSave = LOS_IntLock();
    t64 = *timer;
    offset = -(g_rtcTimeZone * SECS_PER_MIN);
    if (!ConvertSecs2Utc(t64, offset, tp)) {
        tp = NULL;
    }
    (void)LOS_IntRestore(intSave);
    return tp;
}

struct tm *localtime(const time_t *timer)
{
    return localtime_r(timer, &g_tm);
}

static time_t ConvertUtc2Secs(struct tm *tm)
{
    time_t seconds = 0;
    INT32 month = 0;
    UINT8 leap = 0;

    INT32 year = (EPOCH_YEAR - TM_YEAR_BASE);
    while (year < tm->tm_year) {
        seconds += SECS_PER_NORMAL_YEAR;
        if (IS_LEAP_YEAR(year + TM_YEAR_BASE)) {
            seconds += SECS_PER_DAY;
        }
        year++;
    }

    if (IS_LEAP_YEAR(tm->tm_year + TM_YEAR_BASE)) {
        leap = 1;
    }
    while (month < tm->tm_mon) {
        if ((month == 1) && leap) {
            seconds += (g_montbl[month] + 1) * SECS_PER_DAY;
        } else {
            seconds += g_montbl[month] * SECS_PER_DAY;
        }
        month++;
    }

    seconds += (tm->tm_mday - 1) * SECS_PER_DAY;

    seconds += tm->tm_hour * SECS_PER_HOUR + tm->tm_min * SECS_PER_MIN + tm->tm_sec;

    return seconds;
}

time_t mktime(struct tm *tmptr)
{
    struct tm tempTime;
    time_t timeInSeconds;
    if (tmptr == NULL) {
        return 0;
    }
    if (tmptr->tm_year < (EPOCH_YEAR - TM_YEAR_BASE)) {
        return 0;
    }
    tempTime = *tmptr;
    timeInSeconds = ConvertUtc2Secs(&tempTime);
    timeInSeconds += g_rtcTimeZone * SECS_PER_MIN;
    return timeInSeconds;
}

#ifndef LPLAYER_NEED_POSIX
int gettimeofday(struct timeval *tv, void *ptz)
{
    INT32 rtcRet;
    INT32 timeZone = 0;
    UINT64 usec = 0;
    UINT64 currentTime;
    UINT64 tickDelta = 0;
    UINT64 currentTick;

    struct timezone *tz = (struct timezone *)ptz;
    if ((tv == NULL) && (tz == NULL)) {
        return -1;
    }
    if (tv != NULL) {
        rtcRet = HalGetRtcTime(&usec);
        if (rtcRet != 0) {
            currentTick = HalGetExpandTick();
            if ((g_systickBase != 0) && (currentTick > g_systickBase)) {
                tickDelta = currentTick - g_systickBase;
            }
            currentTime = g_rtcTimeBase + tickDelta;
            tv->tv_sec = currentTime / OS_SYS_MS_PER_SECOND;
            tv->tv_usec = (currentTime % OS_SYS_MS_PER_SECOND) * OS_SYS_MS_PER_SECOND;
        } else {
            tv->tv_sec = usec / OS_SYS_US_PER_SECOND;
            tv->tv_usec = usec % OS_SYS_US_PER_SECOND;
        }
    }
    HalGetRtcTimeZone(&timeZone);
    if (tz != NULL) {
        tz->tz_minuteswest = timeZone;
    }
    return 0;
}
#endif

int settimeofday(const struct timeval *tv, const struct timezone *tz)
{
    UINT64 usec;
    if ((tv == NULL) || (tz == NULL)) {
        return -1;
    }
    g_rtcTimeBase = tv->tv_sec * OS_SYS_MS_PER_SECOND + tv->tv_usec / OS_SYS_MS_PER_SECOND;
    g_systickBase = HalGetExpandTick();
    if ((tz->tz_minuteswest > TIME_ZONE_MIN) &&
        (tz->tz_minuteswest < TIME_ZONE_MAX)) {
        g_rtcTimeZone = tz->tz_minuteswest;
    }
    usec = tv->tv_sec * OS_SYS_US_PER_SECOND + tv->tv_usec;
    HalSetRtcTime(g_rtcTimeBase, &usec);
    HalSetRtcTimeZone(g_rtcTimeZone);
    return 0;
}
