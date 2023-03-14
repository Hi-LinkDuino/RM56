/*
 * Copyright (c) 2021 Huawei Device Co., Ltd. All rights reserved.
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

#include <los_pm.h>
#include "devmgr_service.h"
#include "hdf_base.h"
#include "hdf_log.h"

#define HDF_LOG_TAG devmgr_pm

UINT32 DevmgrPmSuspend(UINT32 mode)
{
    if (mode != LOS_SYS_LIGHT_SLEEP) {
        HDF_LOGI("%s: hdf Suspend %u", __func__, mode);
    }

    HDF_LOGI("%s hdf suspend start", __func__);
    struct IDevmgrService *devmgrService = DevmgrServiceGetInstance();
    if (devmgrService == NULL) {
        return LOS_NOK;
    }

    if (devmgrService->PowerStateChange(devmgrService, POWER_STATE_SUSPEND) != HDF_SUCCESS) {
        HDF_LOGE("%s drivers suspend failed", __func__);
        devmgrService->PowerStateChange(devmgrService, POWER_STATE_RESUME);
        return LOS_NOK;
    }

    HDF_LOGI("%s hdf suspend done", __func__);
    return LOS_OK;
}

void DevmgrPmResume(UINT32 mode)
{
    if (mode != LOS_SYS_LIGHT_SLEEP) {
        HDF_LOGI("%s: hdf resume %d", __func__, mode);
    }

    HDF_LOGI("%s hdf resume start", __func__);
    struct IDevmgrService *devmgrService = DevmgrServiceGetInstance();
    if (devmgrService == NULL) {
        return;
    }

    devmgrService->PowerStateChange(devmgrService, POWER_STATE_RESUME);
    HDF_LOGI("%s hdf resume done", __func__);
}

int DevMgrPmRegister(void)
{
    static LosPmDevice pmOpt = {
        .suspend = DevmgrPmSuspend,
        .resume = DevmgrPmResume,
    };

    HDF_LOGI("%s hdf enter", __func__);

    if (LOS_PmRegister(LOS_PM_TYPE_DEVICE, &pmOpt) != 0) {
        HDF_LOGE("failed to register los pm opt");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

