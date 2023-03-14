/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef BATTERY_THREAD_H
#define BATTERY_THREAD_H

#include <map>
#include <memory>
#include <thread>
#include <vector>
#include "battery_callback_impl.h"
#include "power_supply_provider.h"

namespace OHOS {
namespace HDI {
namespace Battery {
namespace V1_0 {
enum EventType {
    EVENT_UEVENT_FD,
    EVENT_TIMER_FD,
};

class BatteryThread {
public:
    virtual ~BatteryThread() = default;

    void StartThread(void* service);
    void InitCallback(const sptr<OHOS::HDI::Battery::V1_0::IBatteryCallback>& callback);
protected:
    int32_t LoopingThreadEntry(void* arg);
    virtual void Run(void* service);
    virtual void UpdateBatteryInfo(void* service, char* msg);
    virtual void UpdateBatteryInfo(void* service);
    virtual void HandleStates() {}
    virtual int32_t UpdateWaitInterval();
    void UpdateEpollInterval(int32_t chargeState);
    virtual void CycleMatters() {}
private:
    int32_t OpenUeventSocket();
    bool IsPowerSupplyEvent(const char* msg);
    int32_t Init([[maybe_unused]]void* service);
    int32_t InitUevent();
    int32_t InitTimer();
    void TimerCallback(void* service);
    void UeventCallback(void* service);
    void SetTimerInterval(int32_t interval);
    int32_t RegisterCallback(int32_t fd, EventType et);
    static constexpr int32_t INVALID_FD = -1;
    int32_t ueventFd_ = INVALID_FD;
    int32_t timerFd_ = INVALID_FD;
    int32_t epFd_ = INVALID_FD;
    int32_t timerInterval_ = -1;
    int32_t epollInterval_ = -1;
    using Callback = std::function<void(BatteryThread*, void*)>;
    std::map<int32_t, Callback> callbacks_;
    std::unique_ptr<PowerSupplyProvider> provider_ = nullptr;
};
}  // namespace V1_0
}  // namespace Battery
}  // namespace HDI
}  // namespace OHOS

#endif // BATTERY_THREAD_H
