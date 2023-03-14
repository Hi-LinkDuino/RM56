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

#include "osal_time.h"
#include <sys/time.h>
#include <time.h>
#include "los_sys.h"
#include "los_task.h"
#include "hdf_log.h"

#define HDF_LOG_TAG osal_time

int32_t OsalGetTime(OsalTimespec *time)
{
    uint64_t ms;

    if (time == NULL) {
        HDF_LOGE("%s invalid para", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    ms = LOS_TickCountGet() * (HDF_KILO_UNIT / LOSCFG_BASE_CORE_TICK_PER_SECOND);

    time->sec = ms / HDF_KILO_UNIT;
    time->usec = (ms % HDF_KILO_UNIT) * HDF_KILO_UNIT;

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

uint64_t OsalGetSysTimeMs(void)
{
    return LOS_TickCountGet() * (HDF_KILO_UNIT / LOSCFG_BASE_CORE_TICK_PER_SECOND);
}

void OsalSleep(uint32_t sec)
{
    LOS_Msleep(sec * HDF_KILO_UNIT);
}

void OsalMSleep(uint32_t ms)
{
    LOS_Msleep(ms);
}

void OsalUDelay(uint32_t us)
{
    LOS_Udelay(us);
}

void OsalMDelay(uint32_t ms)
{
    LOS_Mdelay(ms);
}

