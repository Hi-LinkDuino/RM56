/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "osal_spinlock.h"
#include <pthread.h>
#include "hdf_log.h"
#include "osal_mem.h"

#define HDF_LOG_TAG osal_spinlock

int32_t OsalSpinInit(OsalSpinlock *spinlock)
{
    pthread_spinlock_t *spinTmp = NULL;
    int ret;

    if (spinlock == NULL) {
        HDF_LOGE("%s invalid param", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    spinlock->realSpinlock = NULL;

    spinTmp = (pthread_spinlock_t *)OsalMemCalloc(sizeof(pthread_spinlock_t));
    if (spinTmp == NULL) {
        HDF_LOGE("malloc fail");
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = pthread_spin_init(spinTmp, PTHREAD_PROCESS_PRIVATE);
    if (ret != 0) {
        HDF_LOGE("pthread_spin_init fail %d %d", ret, __LINE__);
        OsalMemFree(spinTmp);
        return HDF_FAILURE;
    }
    spinlock->realSpinlock = (void *)spinTmp;

    return HDF_SUCCESS;
}

int32_t OsalSpinDestroy(OsalSpinlock *spinlock)
{
    int ret;

    if (spinlock == NULL || spinlock->realSpinlock == NULL) {
        HDF_LOGE("%s invalid param", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    ret = pthread_spin_destroy((pthread_spinlock_t *)spinlock->realSpinlock);
    if (ret != 0) {
        HDF_LOGE("pthread_spin_destroy fail %d %d", ret, __LINE__);
        return HDF_FAILURE;
    }

    OsalMemFree(spinlock->realSpinlock);
    spinlock->realSpinlock = NULL;

    return HDF_SUCCESS;
}

int32_t OsalSpinLock(OsalSpinlock *spinlock)
{
    int ret;

    if (spinlock == NULL || spinlock->realSpinlock == NULL) {
        HDF_LOGE("%s invalid param", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    ret = pthread_spin_lock((pthread_spinlock_t *)spinlock->realSpinlock);
    if (ret != 0) {
        HDF_LOGE("pthread_spin_lock fail %d %d", ret, __LINE__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t OsalSpinUnlock(OsalSpinlock *spinlock)
{
    int ret;

    if (spinlock == NULL || spinlock->realSpinlock == NULL) {
        HDF_LOGE("%s invalid param", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    ret = pthread_spin_unlock((pthread_spinlock_t *)spinlock->realSpinlock);
    if (ret != 0) {
        HDF_LOGE("pthread_spin_unlock fail %d %d", ret, __LINE__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

