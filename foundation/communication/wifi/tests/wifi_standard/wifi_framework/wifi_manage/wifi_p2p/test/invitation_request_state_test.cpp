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

#include "invitation_request_state.h"
#include "mock_p2p_pendant.h"
#include "mock_wifi_p2p_hal_interface.h"

using ::testing::_;
using ::testing::Return;
using ::testing::ext::TestSize;

namespace OHOS {
namespace Wifi {
class InvitationRequestStateTest : public testing::Test {
public:
    static void SetUpTestCase()
    {}
    static void TearDownTestCase()
    {}
    virtual void SetUp()
    {
        pMockP2pPendant.reset(new MockP2pPendant());
        pInvitationRequestState.reset(
            new InvitationRequestState(pMockP2pPendant->GetP2pStateMachine(), groupManager, deviceManager));
    }
    virtual void TearDown()
    {
        pInvitationRequestState.reset();
        EXPECT_CALL(WifiP2PHalInterface::GetInstance(), RegisterP2pCallback(_));
        pMockP2pPendant.reset();
    }

public:
    void AddGroup()
    {
        WifiP2pGroupInfo group;
        group.SetNetworkId(0);
        pInvitationRequestState->groupManager.SetCurrentGroup(group);
    }
    std::unique_ptr<InvitationRequestState> pInvitationRequestState;
    std::unique_ptr<MockP2pPendant> pMockP2pPendant;
    WifiP2pGroupManager groupManager;
    WifiP2pDeviceManager deviceManager;
};

HWTEST_F(InvitationRequestStateTest, GoInState, TestSize.Level1)
{
    EXPECT_CALL(WifiP2PHalInterface::GetInstance(), Invite(_, _))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_OK))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_FAILED));
    pInvitationRequestState->GoInState();
    pInvitationRequestState->GoInState();
}

HWTEST_F(InvitationRequestStateTest, GoOutState, TestSize.Level1)
{
    pInvitationRequestState->GoOutState();
}

HWTEST_F(InvitationRequestStateTest, ProcessInvitationResultEvt1, TestSize.Level1)
{
    InternalMessage msg;
    msg.SetParam1(0);
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::P2P_EVENT_INVITATION_RESULT));
    EXPECT_TRUE(pInvitationRequestState->ExecuteStateMsg(&msg));
}

HWTEST_F(InvitationRequestStateTest, ProcessInvitationResultEvt2, TestSize.Level1)
{
    InternalMessage msg;
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::P2P_EVENT_INVITATION_RESULT));
    msg.SetParam1(8);
    EXPECT_TRUE(pInvitationRequestState->ExecuteStateMsg(&msg));
}

HWTEST_F(InvitationRequestStateTest, ProcessInvitationResultEvt3, TestSize.Level1)
{
    WifiP2pGroupInfo group;
    group.SetNetworkId(0);
    groupManager.SetCurrentGroup(group);
    InternalMessage msg;
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::P2P_EVENT_INVITATION_RESULT));
    msg.SetParam1(8);
    EXPECT_TRUE(pInvitationRequestState->ExecuteStateMsg(&msg));
}

HWTEST_F(InvitationRequestStateTest, ProcessCmdP2pDisable, TestSize.Level1)
{
    InternalMessage msg;
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::CMD_P2P_DISABLE));
    EXPECT_TRUE(pInvitationRequestState->ExecuteStateMsg(&msg));
}

HWTEST_F(InvitationRequestStateTest, ExecuteStateMsgFailed, TestSize.Level1)
{
    InternalMessage msg;
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::P2P_EVENT_GROUP_FORMATION_SUCCESS));
    EXPECT_FALSE(pInvitationRequestState->ExecuteStateMsg(&msg));
}
}  // namespace Wifi
}  // namespace OHOS