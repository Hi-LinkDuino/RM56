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

#include "screen_saver_handler.h"

#include <thread>

#include <element_name.h>
#include <samgr_lite.h>
#include <securec.h>
#include <want.h>

#include "hilog_wrapper.h"
#include "running_lock_mgr.h"

namespace OHOS {
ScreenSaverHandler::~ScreenSaverHandler()
{
    if (timer_ != nullptr) {
        PowerMgrDestroyTimer(timer_);
        timer_ = nullptr;
    }
}

AmsInterface *ScreenSaverHandler::GetAmsInterface()
{
    static struct AmsInterface *ams = nullptr;
    if (ams != nullptr) {
        return ams;
    }

    IUnknown *iUnknown = SAMGR_GetInstance()->GetFeatureApi(AMS_SERVICE, AMS_FEATURE);
    if (iUnknown == nullptr) {
        POWER_HILOGE("Failed to get ams iUnknown");
        return nullptr;
    }

    int ret = iUnknown->QueryInterface(iUnknown, DEFAULT_VERSION, (void **)&ams);
    if ((ret != EC_SUCCESS) || (ams == nullptr)) {
        POWER_HILOGE("Failed to query ams interface");
        return nullptr;
    }
    POWER_HILOGI("Succeed to get ams interface");
    return ams;
}

bool ScreenSaverHandler::StartScreenSaverLocked()
{
    AmsInterface *ams = GetAmsInterface();
    if (ams == nullptr) {
        POWER_HILOGE("Failed to get ams interface");
        return false;
    }

    Want want = { nullptr };
    ElementName element = { nullptr };
    SetElementBundleName(&element, "com.huawei.screensaver");
    SetElementAbilityName(&element, "ScreensaverAbility");
    SetWantElement(&want, element);
    SetWantData(&want, "WantData", strlen("WantData") + 1);
    int32_t ret = ams->StartAbility(&want);
    ClearElement(&element);
    ClearWant(&want);
    return ret == EC_SUCCESS;
}

void ScreenSaverHandler::StartScreenSaver()
{
    POWER_HILOGI("Time to start screen saver");
    std::lock_guard<std::mutex> lock(mutex_);
    if (!enabled_ || screenSaverStarted_ || (RunningLockMgrIsLockHolding(RUNNINGLOCK_SCREEN) == TRUE)) {
        return;
    }
    screenSaverStarted_ = StartScreenSaverLocked();
    POWER_HILOGI("Start screen saver: %d", static_cast<int32_t>(screenSaverStarted_));
}

void PowerTimerCallback(void *data)
{
    if (data == nullptr) {
        POWER_HILOGE("Invalid timer data");
        return;
    }
    ScreenSaverHandler *handler = static_cast<ScreenSaverHandler *>(data);
    handler->StartScreenSaver();
}

void ScreenSaverHandler::Init()
{
    if (timer_ == nullptr) {
        timer_ = PowerMgrCreateTimer(intervalMsec_, intervalMsec_, PowerTimerCallback);
    }
    std::thread lateInit([this] {
        sleep(20); // set delay to avoid registing input evnet listener failure
        SetState(true);
    });
    lateInit.detach();
}

void ScreenSaverHandler::SetInterval(int64_t intervalMsec)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (intervalMsec_ == intervalMsec) {
        return;
    }
    if (timer_ != nullptr) {
        PowerMgrResetTimer(timer_, intervalMsec, intervalMsec);
        if (enabled_) {
            SetEnableLocked();
        }
    }
    intervalMsec_ = intervalMsec;
}

bool ScreenSaverHandler::SetEnableLocked()
{
    if (PowerMgrStartTimer(timer_, (void *)this) == FALSE) {
        POWER_HILOGE("Failed to start timer");
        return false;
    }
    if (!InputEventListenerProxy::GetInstance()->RegisterInputEventListener(this)) {
        PowerMgrStopTimer(timer_);
        POWER_HILOGE("Failed to register input event listener");
        return false;
    }
    POWER_HILOGI("Succeed to enable screen saver");
    return true;
}

bool ScreenSaverHandler::SetDisableLocked()
{
    InputEventListenerProxy::GetInstance()->UnregisterInputEventListener();
    PowerMgrStopTimer(timer_);
    POWER_HILOGI("Succeed to disable screen saver");
    return true;
}

int32_t ScreenSaverHandler::SetState(bool enable)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (timer_ == nullptr) {
        return EC_FAILURE;
    }
    if (enabled_ == enable) {
        return EC_SUCCESS;
    }
    bool ret = enable ? SetEnableLocked() : SetDisableLocked();
    if (!ret) {
        POWER_HILOGE("Failed to set state: %d", static_cast<int32_t>(enable));
        return EC_FAILURE;
    }
    enabled_ = enable;
    return EC_SUCCESS;
}

void ScreenSaverHandler::OnRawEvent(const RawEvent &event)
{
    static int64_t lastUpdateTime = 0;
    std::lock_guard<std::mutex> lock(mutex_);
    screenSaverStarted_ = false;
    int64_t currentTime = GetCurrentTimeMsec(CLOCK_MONOTONIC);
    if ((currentTime - lastUpdateTime) > MSEC_PER_SEC) {
        PowerMgrRestartTimer(timer_, (void *)this);
        lastUpdateTime = currentTime;
    }
}
} // namespace OHOS
