/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <los_pm.h>

#include "hilog_wrapper.h"
#include "power/suspend_ops.h"

static BOOL AutoSuspendLoopFunc(AutoSuspendWait waitFunc)
{
    POWER_HILOGD("Prepare suspend");
    uint32_t wakeCount = LOS_PmReadLock();
    POWER_HILOGI("Lock count: %{public}u", wakeCount);

    // wait callback to check if suspend block counter is ready
    waitFunc();

    POWER_HILOGD("Enter suspend");
    uint32_t ret = LOS_PmSuspend(wakeCount);
    if (ret != LOS_OK) {
        return FALSE;
    }

    POWER_HILOGD("Exit suspend");
    return TRUE;
}

AutoSuspendLoop AutoSuspendLoopInit()
{
    return AutoSuspendLoopFunc;
}
