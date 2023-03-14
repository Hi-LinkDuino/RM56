/*
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

#include "osal_spinlock.h"
#include "linux/spinlock.h"
#include "hdf_log.h"
#include "osal_mem.h"

#define HDF_LOG_TAG osal_spinlock

int32_t OsalSpinInit(OsalSpinlock *spinlock)
{
    spinlock_t *spin = NULL;

    if (spinlock == NULL) {
        HDF_LOGE("%s invalid param", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    spin = (spinlock_t *)OsalMemCalloc(sizeof(*spin));
    if (spin == NULL) {
        HDF_LOGE("%s malloc fail", __func__);
        spinlock->realSpinlock = NULL;
        return HDF_ERR_MALLOC_FAIL;
    }

    spin_lock_init(spin);
    spinlock->realSpinlock = spin;

    return HDF_SUCCESS;
}

int32_t OsalSpinDestroy(OsalSpinlock *spinlock)
{
    if (spinlock == NULL || spinlock->realSpinlock == NULL) {
        HDF_LOGE("%s invalid param", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    OsalMemFree(spinlock->realSpinlock);
    spinlock->realSpinlock = NULL;

    return HDF_SUCCESS;
}

int32_t OsalSpinLock(OsalSpinlock *spinlock)
{
    if (spinlock == NULL || spinlock->realSpinlock == NULL) {
        HDF_LOGE("%s invalid param", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    spin_lock((spinlock_t *)spinlock->realSpinlock);

    return HDF_SUCCESS;
}

int32_t OsalSpinUnlock(OsalSpinlock *spinlock)
{
    if (spinlock == NULL || spinlock->realSpinlock == NULL) {
        HDF_LOGE("%s invalid param", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    spin_unlock((spinlock_t *)spinlock->realSpinlock);

    return HDF_SUCCESS;
}

int32_t OsalSpinLockIrq(OsalSpinlock *spinlock)
{
#ifdef LOSCFG_KERNEL_SMP
    if (spinlock == NULL || spinlock->realSpinlock == NULL) {
        HDF_LOGE("%s invalid param %d", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    LOS_IntLock();
    LOS_SpinLock((spinlock_t *)spinlock->realSpinlock);
#else
    (void)spinlock;
    LOS_IntLock();
#endif
    return HDF_SUCCESS;
}

int32_t OsalSpinUnlockIrq(OsalSpinlock *spinlock)
{
#ifdef LOSCFG_KERNEL_SMP
    if (spinlock == NULL || spinlock->realSpinlock == NULL) {
        HDF_LOGE("%s invalid param %d", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }

    LOS_SpinUnlock((spinlock_t *)spinlock->realSpinlock);
    LOS_IntUnLock();
#else
    (void)spinlock;
    LOS_IntUnLock();
#endif

    return HDF_SUCCESS;
}

int32_t OsalSpinLockIrqSave(OsalSpinlock *spinlock, uint32_t *flags)
{
    uint32_t temp = 0;
    if (spinlock == NULL || spinlock->realSpinlock == NULL || flags == NULL) {
        HDF_LOGE("%s invalid param %d", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    spin_lock_irqsave((spinlock_t *)spinlock->realSpinlock, temp);
    *flags = temp;

    return HDF_SUCCESS;
}

int32_t OsalSpinUnlockIrqRestore(OsalSpinlock *spinlock, uint32_t *flags)
{
    if (spinlock == NULL || spinlock->realSpinlock == NULL || flags == NULL) {
        HDF_LOGE("%s invalid param %d", __func__, __LINE__);
        return HDF_ERR_INVALID_PARAM;
    }
    spin_unlock_irqrestore((spinlock_t *)spinlock->realSpinlock, *flags);

    return HDF_SUCCESS;
}

