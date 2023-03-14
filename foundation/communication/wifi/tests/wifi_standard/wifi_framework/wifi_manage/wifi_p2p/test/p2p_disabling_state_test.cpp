/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "mock_p2p_pendant.h"
#include "mock_wifi_p2p_hal_interface.h"
#include "p2p_disabling_state.h"
#include "mock_p2p_monitor.h"
#include "mock_wifi_settings.h"

using ::testing::_;
using ::testing::Return;
using ::testing::ext::TestSize;

namespace OHOS {
namespace Wifi {
class P2pDisablingStateTest : public testing::Test {
public:
    static void SetUpTestCase()
    {}
    static void TearDownTestCase()
    {}
    virtual void SetUp()
    {
        pMockP2pPendant.reset(new MockP2pPendant());
        pP2pDisablingState.reset(
            new P2pDisablingState(pMockP2pPendant->GetP2pStateMachine(), groupManager, deviceManager));
    }
    virtual void TearDown()
    {
        pP2pDisablingState.reset();
        EXPECT_CALL(WifiP2PHalInterface::GetInstance(), RegisterP2pCallback(_));
        pMockP2pPendant.reset();
    }

public:
    std::unique_ptr<P2pDisablingState> pP2pDisablingState;
    std::unique_ptr<MockP2pPendant> pMockP2pPendant;
    WifiP2pGroupManager groupManager;
    WifiP2pDeviceManager deviceManager;
};

HWTEST_F(P2pDisablingStateTest, GoInState, TestSize.Level1)
{
    pP2pDisablingState->GoInState();
}

HWTEST_F(P2pDisablingStateTest, GoOutState, TestSize.Level1)
{
    pP2pDisablingState->GoOutState();
}

HWTEST_F(P2pDisablingStateTest, ExecuteStateMsg, TestSize.Level1)
{
    InternalMessage msg;
    msg.SetParam1(1);
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::WPA_CONNECTED_EVENT));
    pP2pDisablingState->ExecuteStateMsg(&msg);

    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::CMD_P2P_DISABLE));
    pP2pDisablingState->ExecuteStateMsg(&msg);
}

HWTEST_F(P2pDisablingStateTest, ExecuteStateMsg2, TestSize.Level1)
{
    InternalMessage msg;
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::WPA_CONNECTED_EVENT));
    msg.SetParam1(0);
    EXPECT_CALL(pMockP2pPendant->GetMockP2pMonitor(), MonitorEnds(_)).WillOnce(Return());
    EXPECT_CALL(pMockP2pPendant->GetP2pStateMachine(), BroadcastP2pStatusChanged(_)).WillOnce(Return());
    pP2pDisablingState->ExecuteStateMsg(&msg);
}

HWTEST_F(P2pDisablingStateTest, ProcessDisableP2pTimedOut, TestSize.Level1)
{
    InternalMessage msg;
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::DISABLE_P2P_TIMED_OUT));
    msg.SetParam1(0);
    EXPECT_CALL(pMockP2pPendant->GetMockP2pMonitor(), MonitorEnds(_)).WillOnce(Return());
    EXPECT_CALL(pMockP2pPendant->GetP2pStateMachine(), BroadcastP2pStatusChanged(_)).WillOnce(Return());
    EXPECT_TRUE(pP2pDisablingState->ExecuteStateMsg(&msg));
}
}  // namespace Wifi
}  // namespace OHOS