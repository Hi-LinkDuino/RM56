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

#ifndef THERMAL_HDF_TIMER_H
#define THERMAL_HDF_TIMER_H

#include <map>
#include "thermal_hdf_config.h"
#include "thermal_simulation_node.h"
#include "thermal_zone_manager.h"
#include "v1_0/ithermal_callback.h"

namespace OHOS {
namespace HDI {
namespace Thermal {
namespace V1_0 {
enum EventType {
    EVENT_UEVENT_FD,
    EVENT_TIMER_FD,
};
class ThermalHdfTimer {
public:
    using Callback = std::function<void(ThermalHdfTimer*, void*)>;

    ThermalHdfTimer(const std::shared_ptr<ThermalSimulationNode> &node,
        const std::shared_ptr<ThermalZoneManager> &thermalZoneMgr);
    ~ThermalHdfTimer() {}

    int32_t Init();
    void ReportThermalData();
    int32_t LoopingThreadEntry(void *arg, int32_t epfd);
    void Run(void *service, int32_t epfd);
    void StartThread(void *service);
    int32_t RegisterCallback(const int32_t fd, const EventType et, int32_t epfd);
    void SetTimerInterval(int32_t interval, int32_t timerfd);
    void ResetCount();
    void SetSimluationFlag();
    int32_t GetSimluationFlag();
    void SetSimFlag(int32_t flag);
    void SetThermalEventCb(const sptr<IThermalCallback> &thermalCb);
    void DumpSensorConfigInfo();
private:
    int32_t InitProviderTimer();
    void TimerProviderCallback(void *service);
    int32_t CreateProviderFd();

    int32_t epFd_ {-1};
    int32_t timerFd_ {-1};
    int32_t timerInterval_ {-1};
    std::map<int32_t, Callback> callbackHandler_;
    std::shared_ptr<ThermalSimulationNode> node_;
    std::shared_ptr<ThermalZoneManager> thermalZoneMgr_;
    HdfThermalCallbackInfo tzInfoEventV1_;
    HdfThermalCallbackInfo tzInfoEventV2_;
    HdfThermalCallbackInfo tzInfoEvent_;
    sptr<IThermalCallback> thermalCb_;
    std::vector<int32_t> multipleList_;
    int32_t reportTime_;
    int32_t isSim_;
};
} // V1_0
} // Thermal
} // HDI
} // OHOS
#endif // THERMAL_HDF_TIMER_H