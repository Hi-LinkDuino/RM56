/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OHOS_MOCK_AP_PENDANT_H
#define OHOS_MOCK_AP_PENDANT_H
#include "mock_ap_service.h"
#include "mock_ap_monitor.h"
#include "mock_ap_config_use.h"
#include "mock_ap_stations_manager.h"

#include "mock_ap_root_state.h"
#include "mock_ap_idle_state.h"
#include "mock_ap_started_state.h"
#include "mock_ap_state_machine.h"
#include "mock_ap_service.h"

namespace OHOS {
namespace Wifi {

class MockPendant {
public:
    MockPendant()
        : m_MockApRootState(),
          m_MockApIdleState(m_MockApStateMachine),
          m_MockApStartedState(m_MockApStateMachine, m_MockApConfigUse, m_MockApMonitor),
          m_MockApService(m_MockApStateMachine),
          m_MockApStateMachine(
              m_MockApStationsManager, m_MockApRootState, m_MockApIdleState, m_MockApStartedState, m_MockApMonitor),
          m_MockApConfigUse(),
          m_MockApStationsManager()
    {}

public:
    MockApRootState &GetMockApRootState()
    {
        return m_MockApRootState;
    }
    MockApIdleState &GetMockApIdleState()
    {
        return m_MockApIdleState;
    }
    MockApStartedState &GetMockApStartedState()
    {
        return m_MockApStartedState;
    }
    MockApService &GetMockApService()
    {
        return m_MockApService;
    }
    MockApStateMachine &GetMockApStateMachine()
    {
        return m_MockApStateMachine;
    }
    MockApMonitor &GetMockApMonitor()
    {
        return m_MockApMonitor;
    }
    MockApConfigUse &GetMockApConfigUse()
    {
        return m_MockApConfigUse;
    }
    MockApStationsManager &GetMockApStationsManager()
    {
        return m_MockApStationsManager;
    }

private:
    MockApRootState m_MockApRootState;
    MockApIdleState m_MockApIdleState;
    MockApStartedState m_MockApStartedState;

    MockApService m_MockApService;
    MockApStateMachine m_MockApStateMachine;
    MockApMonitor m_MockApMonitor;

    MockApConfigUse m_MockApConfigUse;
    MockApStationsManager m_MockApStationsManager;
};
}  // namespace Wifi
}  // namespace OHOS
#endif  // OHOS_MOCK_AP_PENDANT_H
