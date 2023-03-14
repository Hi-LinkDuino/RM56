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
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>
#include "ap_define.h"
#include "ap_state_machine.h"
#include "wifi_log.h"
#include "mock_wifi_settings.h"
#include "mock_pendant.h"
#include "mock_wifi_ap_hal_interface.h"
#include "operator_overload.h"

using ::testing::_;
using ::testing::Eq;
using ::testing::Return;
namespace OHOS {
namespace Wifi {
class ApStateMachine_test : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    virtual void SetUp()
    {
        const int SLEEP_TIME = 20;
        pMockPendant = new MockPendant();
        pMockApStationsManager = &(pMockPendant->GetMockApStationsManager());
        pMockApRootState = &(pMockPendant->GetMockApRootState());
        pMockApIdleState = &(pMockPendant->GetMockApIdleState());
        pMockApStartedState = &(pMockPendant->GetMockApStartedState());
        pMockApMonitor = &(pMockPendant->GetMockApMonitor());

        pMockPendant->GetMockApStateMachine().InitialStateMachine();

        pApStateMachine = new ApStateMachine(*pMockApStationsManager, *pMockApRootState, *pMockApIdleState,
            *pMockApStartedState, *pMockApMonitor);

        pApStateMachine->InitialStateMachine();
        RegisterApServiceCallbacks();
        EXPECT_CALL(WifiApHalInterface::GetInstance(), RegisterApEvent(_))
            .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
        usleep(SLEEP_TIME);
    }
    virtual void TearDown()
    {
        delete pApStateMachine;
        delete pMockPendant;
        pMockPendant = nullptr;
        pApStateMachine = nullptr;
        pMockApStationsManager = nullptr;
        pMockApRootState = nullptr;
        pMockApIdleState = nullptr;
        pMockApStartedState = nullptr;
        pMockApMonitor = nullptr;
    }

public:
    ErrCode RegisterApServiceCallbacks()
    {
        std::function<void(const StationInfo &)> OnStationEvent = [&](const StationInfo &sta) { m_sta = sta; };

        IApServiceCallbacks callbacks = {[&](ApState state) { mBState = state; }, OnStationEvent, OnStationEvent};
        return pApStateMachine->RegisterApServiceCallbacks(callbacks);
    }
    ErrCode UnRegisterApServiceCallbacks()
    {
        IApServiceCallbacks callbacks;
        return pApStateMachine->RegisterApServiceCallbacks(callbacks);
    }

public:
    void WrapOnApStateChange(ApState state)
    {
        EXPECT_CALL(WifiSettings::GetInstance(), SetHotspotState(Eq(static_cast<int>(state)))).WillOnce(Return(0));
        pApStateMachine->OnApStateChange(state);
    }
    void WrapOnApStateChangeFail(ApState state)
    {
        EXPECT_CALL(WifiSettings::GetInstance(), SetHotspotState(Eq(static_cast<int>(state)))).WillOnce(Return(1));
        pApStateMachine->OnApStateChange(state);
    }

    void WrapBroadCastStationJoin(const StationInfo &staInfo)
    {
        pApStateMachine->BroadCastStationChange(staInfo, ApStatemachineEvent::CMD_STATION_JOIN);
    }

    void WrapBroadCastStationLeave(const StationInfo &staInfo)
    {
        pApStateMachine->BroadCastStationChange(staInfo, ApStatemachineEvent::CMD_STATION_LEAVE);
    }
    void WrapBroadCastStationChangeDefult(const StationInfo &staInfo)
    {
        pApStateMachine->BroadCastStationChange(staInfo, ApStatemachineEvent::CMD_START_HOTSPOT);
    }
    void WarpRegisterEventHandler() const
    {
        pApStateMachine->RegisterEventHandler();
    }

public:
    MockPendant *pMockPendant;
    MockApStationsManager *pMockApStationsManager;
    MockApRootState *pMockApRootState;
    MockApIdleState *pMockApIdleState;
    MockApStartedState *pMockApStartedState;
    MockApMonitor *pMockApMonitor;

    ApStateMachine *pApStateMachine;
    ApState mBState;
    StationInfo m_sta;
};
TEST_F(ApStateMachine_test, OnApStateChange)
{
    ApState BroadState = ApState::AP_STATE_IDLE;
    WrapOnApStateChange(BroadState);

    BroadState = ApState::AP_STATE_STARTING;
    WrapOnApStateChange(BroadState);

    BroadState = ApState::AP_STATE_STARTED;
    WrapOnApStateChange(BroadState);

    BroadState = ApState::AP_STATE_CLOSING;
    WrapOnApStateChange(BroadState);

    BroadState = ApState::AP_STATE_CLOSED;
    WrapOnApStateChange(BroadState);

    BroadState = ApState::AP_STATE_CLOSED;
    WrapOnApStateChangeFail(BroadState);
}

TEST_F(ApStateMachine_test, BroadCastStationJoin)
{
    const StationInfo BroadCastStation = {"test1", "aa:bb:cc:dd:ee:ff", "127.0.0.1"};
    WrapBroadCastStationJoin(BroadCastStation);
    EXPECT_EQ(BroadCastStation, m_sta);
}

TEST_F(ApStateMachine_test, BroadCastStationLeave)
{
    const StationInfo BroadCastStation = {"test1", "aa:bb:cc:dd:ee:ff", "127.0.0.1"};
    WrapBroadCastStationLeave(BroadCastStation);
    EXPECT_EQ(BroadCastStation, m_sta);
}
TEST_F(ApStateMachine_test, BroadCastStationChange_DEFAULT)
{
    const StationInfo BroadCastStation = {"test1", "aa:bb:cc:dd:ee:ff", "127.0.0.1"};
    WrapBroadCastStationChangeDefult(BroadCastStation);
}
TEST_F(ApStateMachine_test, RegisterEventHandler)
{
    WarpRegisterEventHandler();
}
} // namespace Wifi
} // namespace OHOS
