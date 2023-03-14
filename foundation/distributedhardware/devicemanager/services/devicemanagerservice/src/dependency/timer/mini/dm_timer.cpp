/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "dm_timer.h"
#include "cmsis_os2.h"
#include "securec.h"

namespace OHOS {
namespace DistributedHardware {
namespace {
const int32_t MILL_SECONDS_PER_SECOND = 1000;
}
DmTimer::DmTimer(const std::string &name)
{
    if (name.empty()) {
        LOGI("DmTimer name is null");
        return;
    }
    mStatus_ = DmTimerStatus::DM_STATUS_INIT;
    mTimeOutSec_ = 0;
    mTimerName_ = name;
}

DmTimer::~DmTimer()
{
    if (mTimerName_.empty()) {
        LOGI("DmTimer is not init");
        return;
    }
    LOGI("DmTimer %s Destroy in", mTimerName_.c_str());
}

struct DmTimst {
    TimeoutHandle handle;
    DmTimer *dmtimer;
    void *data;
};

static void handler(void *data)
{
    struct DmTimst *myTimer = (struct DmTimst *)data;
    myTimer->handle(myTimer->data, *(myTimer->dmtimer));
}

DmTimerStatus DmTimer::Start(uint32_t timeOut, TimeoutHandle handle, void *data)
{
    if (mTimerName_.empty() || handle == nullptr || data == nullptr) {
        LOGI("DmTimer is not init or param empty");
        return DmTimerStatus::DM_STATUS_FINISH;
    }
    LOGI("DmTimer %s start timeout(%d)", mTimerName_.c_str(), timeOut);
    if (mStatus_ != DmTimerStatus::DM_STATUS_INIT) {
        return DmTimerStatus::DM_STATUS_BUSY;
    }

    static struct DmTimst myTimer = {
        .handle = handle,
        .dmtimer = this,
        .data = data,
    };
    mTimeOutSec_ = timeOut;

    void *id = osTimerNew((osTimerFunc_t)handler, osTimerType_t::osTimerOnce, (void *)&myTimer, NULL);
    if (id != NULL) {
        LOGI("Get name of a timer success ");
    } else {
        LOGI("Get name of a timer failed");
        return DmTimerStatus::DM_STATUS_CREATE_ERROR;
    }

    timerId = id;
    int stOk = osTimerStart(timerId, timeOut * osKernelGetTickFreq());
    if (stOk == 0) {
        LOGI("DmTimer %s start timer at (%d)s", mTimerName_.c_str(), mTimeOutSec_);
        mStatus_ = DmTimerStatus::DM_STATUS_RUNNING;
    } else {
        LOGI("Start a timer failed");
        (void)osTimerDelete(timerId);
    }

    return mStatus_;
}

void DmTimer::Stop(int32_t code)
{
    LOGI("DmTimer Stop code (%d)", code);
    if (timerId != NULL) {
        osTimerDelete(timerId);
        timerId = NULL;
    }
    LOGI("DmTimer %s end timer at  (%d)s", mTimerName_.c_str(), mTimeOutSec_);
    return;
}

std::string DmTimer::GetTimerName()
{
    return mTimerName_;
}
} // namespace DistributedHardware
} // namespace OHOS
