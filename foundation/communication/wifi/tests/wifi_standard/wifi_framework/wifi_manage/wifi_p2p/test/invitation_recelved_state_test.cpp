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

#include "invitation_received_state.h"
#include "mock_p2p_pendant.h"
#include "mock_wifi_p2p_hal_interface.h"

using ::testing::_;
using ::testing::Return;
using ::testing::ext::TestSize;

namespace OHOS {
namespace Wifi {
class InvitationReceivedStateTest : public testing::Test {
public:
    static void SetUpTestCase()
    {}
    static void TearDownTestCase()
    {}
    virtual void SetUp()
    {
        pMockP2pPendant.reset(new MockP2pPendant());
        pInvitationReceivedState.reset(
            new InvitationReceivedState(pMockP2pPendant->GetP2pStateMachine(), groupManager, deviceManager));
        pGroupFormedState.reset(
            new GroupFormedState(pMockP2pPendant->GetP2pStateMachine(), groupManager, deviceManager));
    }
    virtual void TearDown()
    {
        pInvitationReceivedState.reset();
        EXPECT_CALL(WifiP2PHalInterface::GetInstance(), RegisterP2pCallback(_));
        pMockP2pPendant.reset();
        pGroupFormedState.reset();
    }

public:
    void AddSaveP2pConfig()
    {
        InternalMessage msg;
        msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::P2P_EVENT_PROV_DISC_ENTER_PIN));
        WifiP2pTempDiscEvent procDisc;
        WifiP2pDevice device;
        device.SetDeviceAddress("AA:BB:CC:DD:EE:FF");
        procDisc.SetDevice(device);
        msg.SetMessageObj(procDisc);
        pGroupFormedState->ExecuteStateMsg(&msg);
    }
    std::unique_ptr<GroupFormedState> pGroupFormedState;
    std::unique_ptr<InvitationReceivedState> pInvitationReceivedState;
    std::unique_ptr<MockP2pPendant> pMockP2pPendant;
    WifiP2pGroupManager groupManager;
    WifiP2pDeviceManager deviceManager;
};

HWTEST_F(InvitationReceivedStateTest, GoInState, TestSize.Level1)
{
    pInvitationReceivedState->GoInState();
}

HWTEST_F(InvitationReceivedStateTest, GoOutState, TestSize.Level1)
{
    pInvitationReceivedState->GoOutState();
}

HWTEST_F(InvitationReceivedStateTest, ExecuteStateMsg1, TestSize.Level1)
{
    InternalMessage msg;
    AddSaveP2pConfig();
    std::string inputPin;
    msg.SetMessageObj(inputPin);
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::INTERNAL_CONN_USER_ACCEPT));
    EXPECT_CALL(WifiP2PHalInterface::GetInstance(), P2pStopFind()).WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_FAILED));
    pInvitationReceivedState->ExecuteStateMsg(&msg);
}

HWTEST_F(InvitationReceivedStateTest, ExecuteStateMsg2, TestSize.Level1)
{
    InternalMessage msg;
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::PEER_CONNECTION_USER_REJECT));
    pInvitationReceivedState->ExecuteStateMsg(&msg);
}

HWTEST_F(InvitationReceivedStateTest, ExecuteStateMsg3, TestSize.Level1)
{
    InternalMessage msg;
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::CREATE_GROUP_TIMED_OUT));
    pInvitationReceivedState->ExecuteStateMsg(&msg);
}
}  // namespace Wifi
}  // namespace OHOS