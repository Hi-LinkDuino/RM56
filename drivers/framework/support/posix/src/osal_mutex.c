/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "osal_mutex.h"
#include <errno.h>
#include <pthread.h>
#include "securec.h"
#include "hdf_log.h"
#include "osal_mem.h"

#define HDF_LOG_TAG osal_mutex
#define HDF_NANO_UNITS 1000000000

int32_t OsalMutexInit(struct OsalMutex *mutex)
{
    pthread_mutex_t *mutexTmp = NULL;
    int32_t ret;

    if (mutex == NULL) {
        HDF_LOGE("%s invalid param", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    mutex->realMutex = NULL;

    mutexTmp = (pthread_mutex_t *)OsalMemCalloc(sizeof(pthread_mutex_t));
    if (mutexTmp == NULL) {
        HDF_LOGE("%s malloc fail", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = pthread_mutex_init(mutexTmp, NULL);
    if (ret != 0) {
        HDF_LOGE("%s fail %d %d", __func__, ret, __LINE__);
        OsalMemFree(mutexTmp);
        return HDF_FAILURE;
    }

    mutex->realMutex = (void *)mutexTmp;

    return HDF_SUCCESS;
}

int32_t OsalMutexDestroy(struct OsalMutex *mutex)
{
    int32_t ret;

    if (mutex == NULL || mutex->realMutex == NULL) {
        HDF_LOGE("%s invalid param", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    ret = pthread_mutex_destroy((pthread_mutex_t *)mutex->realMutex);
    if (ret != 0) {
        HDF_LOGE("%s fail %d %d", __func__, ret, __LINE__);
        return HDF_FAILURE;
    }

    OsalMemFree(mutex->realMutex);
    mutex->realMutex = NULL;

    return HDF_SUCCESS;
}

int32_t OsalMutexLock(struct OsalMutex *mutex)
{
    if (mutex == NULL || mutex->realMutex == NULL) {
        HDF_LOGE("%s invalid param", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    return OsalMutexTimedLock(mutex, HDF_WAIT_FOREVER);
}

int32_t OsalMutexTimedLock(struct OsalMutex *mutex, uint32_t ms)
{
    int32_t ret;

    if (mutex == NULL || mutex->realMutex == NULL) {
        HDF_LOGE("%s invalid param", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (ms == HDF_WAIT_FOREVER) {
        ret = pthread_mutex_lock((pthread_mutex_t *)mutex->realMutex);
        if (ret != 0) {
            HDF_LOGE("pthread_mutex_lock fail %d", ret);
            return HDF_FAILURE;
        }
    } else {
        struct timespec time;
        (void)memset_s(&time, sizeof(time), 0, sizeof(time));
        clock_gettime(CLOCK_REALTIME, &time);
        time.tv_sec += (time_t)ms / HDF_KILO_UNIT;
        time.tv_nsec += (time_t)(ms % HDF_KILO_UNIT) * HDF_KILO_UNIT * HDF_KILO_UNIT;
        if (time.tv_nsec >= HDF_NANO_UNITS) {
            time.tv_nsec -= HDF_NANO_UNITS;
            time.tv_sec += 1;
        }
        ret = pthread_mutex_timedlock((pthread_mutex_t *)mutex->realMutex, &time);
        if (ret != 0) {
            if (ret == ETIMEDOUT) {
                return HDF_ERR_TIMEOUT;
            } else {
                HDF_LOGE("%s time_out time:%u ret:%d", __func__, ms, ret);
                return HDF_FAILURE;
            }
        }
    }

    return HDF_SUCCESS;
}

int32_t OsalMutexUnlock(struct OsalMutex *mutex)
{
    int32_t ret;

    if (mutex == NULL || mutex->realMutex == NULL) {
        HDF_LOGE("%s invalid param", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    ret = pthread_mutex_unlock((pthread_mutex_t *)mutex->realMutex);
    if (ret != 0) {
        HDF_LOGE("%s fail %d %d", __func__, ret, __LINE__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

