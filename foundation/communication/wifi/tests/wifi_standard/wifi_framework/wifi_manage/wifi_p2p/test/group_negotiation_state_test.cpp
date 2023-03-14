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

#include "group_negotiation_state.h"
#include "mock_p2p_pendant.h"
#include "mock_wifi_p2p_hal_interface.h"

using ::testing::_;
using ::testing::Return;
using ::testing::ext::TestSize;

namespace OHOS {
namespace Wifi {
class GroupNegotiationStateTest : public testing::Test {
public:
    static void SetUpTestCase()
    {}
    static void TearDownTestCase()
    {}
    virtual void SetUp()
    {
        pMockP2pPendant.reset(new MockP2pPendant());
        pGroupNegotiationState.reset(
            new GroupNegotiationState(pMockP2pPendant->GetP2pStateMachine(), groupManager, deviceManager));
        pGroupNegotiationState->Init();
    }
    virtual void TearDown()
    {
        pGroupNegotiationState.reset();
        EXPECT_CALL(WifiP2PHalInterface::GetInstance(), RegisterP2pCallback(_));
        pMockP2pPendant.reset();
    }

public:
    void AddGroupManager()
    {
        WifiP2pGroupInfo groupInfo;
        groupInfo.SetNetworkId(0);
        groupInfo.SetGroupName("AAA");
        groupInfo.SetIsGroupOwner(true);
        WifiP2pDevice owner;
        owner.SetDeviceAddress("AA:BB:CC:DD:EE:FF");
        groupInfo.SetOwner(owner);
        pGroupNegotiationState->groupManager.AddGroup(groupInfo);
        pGroupNegotiationState->groupManager.SetCurrentGroup(groupInfo);
    }
    void AddDeviceManager()
    {
        WifiP2pDevice device;
        device.SetDeviceName("device");
        device.SetDeviceAddress("AA:BB:CC:DD:EE:FF");
        device.SetGroupCapabilitys(static_cast<int>(P2pGroupCapability::PGC_GROUP_OWNER));
        device.SetPrimaryDeviceType("10-0050F204-5");
        pGroupNegotiationState->deviceManager.AddDevice(device);
    }
    std::unique_ptr<GroupNegotiationState> pGroupNegotiationState;
    std::unique_ptr<MockP2pPendant> pMockP2pPendant;
    WifiP2pGroupManager groupManager;
    WifiP2pDeviceManager deviceManager;
};

HWTEST_F(GroupNegotiationStateTest, GoInState, TestSize.Level1)
{
    pGroupNegotiationState->GoInState();
}

HWTEST_F(GroupNegotiationStateTest, GoOutState, TestSize.Level1)
{
    pGroupNegotiationState->GoOutState();
}

HWTEST_F(GroupNegotiationStateTest, ExecuteStateMsg1, TestSize.Level1)
{
    InternalMessage msg;
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::P2P_EVENT_GROUP_FORMATION_SUCCESS));
    EXPECT_TRUE(pGroupNegotiationState->ExecuteStateMsg(&msg));
}

HWTEST_F(GroupNegotiationStateTest, ProcessGroupStartedEvt1, TestSize.Level1)
{
    InternalMessage msg;
    WifiP2pGroupInfo group;
    group.SetIsGroupOwner(true);
    group.SetIsPersistent(false);
    msg.SetMessageObj(group);
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::P2P_EVENT_GROUP_STARTED));
    EXPECT_TRUE(pGroupNegotiationState->ExecuteStateMsg(&msg));
}

HWTEST_F(GroupNegotiationStateTest, ProcessGroupStartedEvt2, TestSize.Level1)
{
    InternalMessage msg;
    WifiP2pGroupInfo group;
    WifiP2pDevice device;
    group.SetIsGroupOwner(true);
    group.SetIsPersistent(true);
    device.SetDeviceName("device");
    device.SetDeviceAddress("AA:BB:CC:DD:EE:FF");
    group.SetOwner(device);
    msg.SetMessageObj(group);
    EXPECT_CALL(pMockP2pPendant->GetP2pStateMachine(), UpdatePersistentGroups()).WillOnce(Return());
    EXPECT_CALL(WifiP2PHalInterface::GetInstance(), SetP2pGroupIdle(_, _))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_FAILED));
    AddDeviceManager();
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::P2P_EVENT_GROUP_STARTED));
    EXPECT_TRUE(pGroupNegotiationState->ExecuteStateMsg(&msg));
}

HWTEST_F(GroupNegotiationStateTest, ProcessGroupStartedEvt3, TestSize.Level1)
{
    InternalMessage msg;
    WifiP2pGroupInfo group;
    group.SetIsGroupOwner(false);
    group.SetIsPersistent(true);
    msg.SetMessageObj(group);
    EXPECT_CALL(WifiP2PHalInterface::GetInstance(), SetP2pGroupIdle(_, _))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_FAILED));
    EXPECT_CALL(pMockP2pPendant->GetP2pStateMachine(), UpdatePersistentGroups()).WillOnce(Return());
    AddDeviceManager();
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::P2P_EVENT_GROUP_STARTED));
    EXPECT_TRUE(pGroupNegotiationState->ExecuteStateMsg(&msg));
}

HWTEST_F(GroupNegotiationStateTest, ExecuteStateMsg5, TestSize.Level1)
{
    InternalMessage msg;
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::P2P_EVENT_GO_NEG_FAILURE));
    EXPECT_TRUE(pGroupNegotiationState->ExecuteStateMsg(&msg));
}

HWTEST_F(GroupNegotiationStateTest, ProcessInvitationResultEvt1, TestSize.Level1)
{
    InternalMessage msg;
    msg.SetParam1(1);
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::P2P_EVENT_INVITATION_RESULT));
    EXPECT_TRUE(pGroupNegotiationState->ExecuteStateMsg(&msg));
}

HWTEST_F(GroupNegotiationStateTest, ProcessInvitationResultEvt2, TestSize.Level1)
{
    InternalMessage msg;
    msg.SetParam1(0);
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::P2P_EVENT_INVITATION_RESULT));
    EXPECT_TRUE(pGroupNegotiationState->ExecuteStateMsg(&msg));
}

HWTEST_F(GroupNegotiationStateTest, ProcessInvitationResultEvt3, TestSize.Level1)
{
    InternalMessage msg;
    msg.SetParam1(8);
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::P2P_EVENT_INVITATION_RESULT));
    EXPECT_TRUE(pGroupNegotiationState->ExecuteStateMsg(&msg));
}

HWTEST_F(GroupNegotiationStateTest, ProcessInvitationResultEvt4, TestSize.Level1)
{
    InternalMessage msg;
    msg.SetParam1(1);
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::P2P_EVENT_INVITATION_RESULT));
    EXPECT_TRUE(pGroupNegotiationState->ExecuteStateMsg(&msg));
}

HWTEST_F(GroupNegotiationStateTest, ProcessInvitationResultEvt5, TestSize.Level1)
{
    InternalMessage msg;
    msg.SetParam1(7);
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::P2P_EVENT_INVITATION_RESULT));
    EXPECT_TRUE(pGroupNegotiationState->ExecuteStateMsg(&msg));
}

HWTEST_F(GroupNegotiationStateTest, ProcessInvitationResultEvt6, TestSize.Level1)
{
    InternalMessage msg;
    msg.SetParam1(3);
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::P2P_EVENT_INVITATION_RESULT));
    EXPECT_CALL(pMockP2pPendant->GetP2pStateMachine(), DealGroupCreationFailed()).WillOnce(Return());
    EXPECT_TRUE(pGroupNegotiationState->ExecuteStateMsg(&msg));
}

HWTEST_F(GroupNegotiationStateTest, ProcessNegotSucessEvt, TestSize.Level1)
{
    InternalMessage msg;
    msg.SetParam1(0);
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::P2P_EVENT_GO_NEG_SUCCESS));
    EXPECT_TRUE(pGroupNegotiationState->ExecuteStateMsg(&msg));
    EXPECT_FALSE(pGroupNegotiationState->ExecuteStateMsg(nullptr));
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::P2P_EVENT_PROV_DISC_PBC_REQ));
    EXPECT_FALSE(pGroupNegotiationState->ExecuteStateMsg(&msg));
}

HWTEST_F(GroupNegotiationStateTest, ProcessGroupFormationFailEvt, TestSize.Level1)
{
    InternalMessage msg;
    msg.SetParam1(0);
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::P2P_EVENT_GROUP_FORMATION_FAILURE));
    EXPECT_TRUE(pGroupNegotiationState->ExecuteStateMsg(&msg));
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::P2P_EVENT_PROV_DISC_PBC_REQ));
    EXPECT_FALSE(pGroupNegotiationState->ExecuteStateMsg(&msg));
}

HWTEST_F(GroupNegotiationStateTest, ProcessGroupRemovedEvt, TestSize.Level1)
{
    InternalMessage msg;
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::P2P_EVENT_GROUP_REMOVED));
    EXPECT_CALL(pMockP2pPendant->GetP2pStateMachine(), DealGroupCreationFailed()).WillOnce(Return());
    EXPECT_TRUE(pGroupNegotiationState->ExecuteStateMsg(&msg));
}
}  // namespace Wifi
}  // namespace OHOS
