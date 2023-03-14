/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef POWERMGR_SCREEN_SAVER_HANDLER_H
#define POWERMGR_SCREEN_SAVER_HANDLER_H

#include <mutex>

#include <ability_service_interface.h>
#include <input_event_listener_proxy.h>

#include "power_mgr_time_util.h"
#include "power_mgr_timer_util.h"

namespace OHOS {
class ScreenSaverHandler : public InputEventListenerProxy::RawEventListener {
public:
    ScreenSaverHandler() = default;
    ~ScreenSaverHandler() override;

    void Init();
    void SetInterval(int64_t intervalMsec);
    int32_t SetState(bool enable);
    void OnRawEvent(const RawEvent &event) override;

private:
    friend void PowerTimerCallback(void *data);

    static AmsInterface *GetAmsInterface();

    bool SetEnableLocked();
    bool SetDisableLocked();
    void StartScreenSaver();
    bool StartScreenSaverLocked();

    static constexpr int64_t DEFAULT_INTERVAL_MSECS = 5 * MSEC_PER_MIN;

    std::mutex mutex_;
    bool enabled_{false};
    bool screenSaverStarted_{false};
    int64_t intervalMsec_{DEFAULT_INTERVAL_MSECS};
    PowerTimer *timer_{nullptr};
};
} // namespace OHOS
#endif // POWERMGR_SCREEN_SAVER_HANDLER_H
