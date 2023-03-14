/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "osal_time.h"
#include <errno.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include "securec.h"
#include "hdf_log.h"

#define HDF_LOG_TAG osal_time

int32_t OsalGetTime(OsalTimespec *time)
{
    struct timespec ts;

    if (time == NULL) {
        HDF_LOGE("%s invalid para", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    (void)memset_s(&ts, sizeof(ts), 0, sizeof(ts));
    clock_gettime(CLOCK_MONOTONIC, &ts);
    time->sec = (uint64_t)ts.tv_sec;
    time->usec = (uint64_t)ts.tv_nsec / HDF_KILO_UNIT;

    return HDF_SUCCESS;
}

int32_t OsalDiffTime(const OsalTimespec *start, const OsalTimespec *end, OsalTimespec *diff)
{
    uint32_t usec = 0;
    uint32_t sec = 0;
    if (start == NULL || end == NULL || diff == NULL) {
        HDF_LOGE("%s invalid para", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (start->sec > end->sec) {
        HDF_LOGE("%s start time later then end time", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (end->usec < start->usec) {
        usec = (HDF_KILO_UNIT * HDF_KILO_UNIT);
        sec = 1;
    }
    diff->usec = usec + end->usec - start->usec;
    diff->sec = end->sec - start->sec - sec;

    return HDF_SUCCESS;
}

void OsalSleep(uint32_t sec)
{
    sleep(sec);
}

void OsalMSleep(uint32_t ms)
{
    int result;
    struct timespec ts;

    ts.tv_sec = (time_t)ms / HDF_KILO_UNIT;
    ts.tv_nsec = (time_t)HDF_KILO_UNIT * HDF_KILO_UNIT * ((long)(ms % HDF_KILO_UNIT));
    result = nanosleep(&ts, &ts);
    if (result != 0) {
        HDF_LOGE("%s OsalMSleep failed %d", __func__, errno);
    }
}

void OsalUSleep(uint32_t us)
{
    int result;
    struct timespec ts;

    ts.tv_sec = (time_t)us / ((long)HDF_KILO_UNIT * HDF_KILO_UNIT);
    ts.tv_nsec = (time_t)HDF_KILO_UNIT * ((long)(us % HDF_KILO_UNIT));
    result = nanosleep(&ts, &ts);
    if (result != 0) {
        HDF_LOGE("%s OsalUSleep failed %d", __func__, errno);
    }
}

void OsalUDelay(uint32_t us)
{
    (void)us;
}

void OsalMDelay(uint32_t ms)
{
    (void)ms;
}

uint64_t OsalGetSysTimeMs()
{
    OsalTimespec time;

    (void)memset_s(&time, sizeof(time), 0, sizeof(time));
    (void)OsalGetTime(&time);

    return (time.sec * HDF_KILO_UNIT + time.usec / HDF_KILO_UNIT);
}
