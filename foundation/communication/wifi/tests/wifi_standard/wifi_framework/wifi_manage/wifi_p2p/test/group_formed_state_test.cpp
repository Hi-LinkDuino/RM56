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

#include "group_formed_state.h"
#include "mock_p2p_pendant.h"
#include "mock_wifi_p2p_hal_interface.h"

using ::testing::_;
using ::testing::Eq;
using ::testing::Return;
using ::testing::ext::TestSize;

namespace OHOS {
namespace Wifi {
class GroupFormedStateTest : public testing::Test {
public:
    static void SetUpTestCase()
    {}
    static void TearDownTestCase()
    {}
    virtual void SetUp()
    {
        pMockP2pPendant.reset(new MockP2pPendant());
        pGroupFormedState.reset(
            new GroupFormedState(pMockP2pPendant->GetP2pStateMachine(), groupManager, deviceManager));
        pGroupFormedState->Init();
    }
    virtual void TearDown()
    {
        pGroupFormedState.reset();
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
        pGroupFormedState->groupManager.AddGroup(groupInfo);
        pGroupFormedState->groupManager.SetCurrentGroup(groupInfo);
    }
    void AddDeviceStateMachine()
    {
        WifiP2pDevice device;
        device.SetDeviceName("device");
        device.SetDeviceAddress("AA:BB:CC:DD:EE:FF");
        device.SetGroupCapabilitys(static_cast<int>(P2pGroupCapability::PGC_GROUP_OWNER));
        device.SetPrimaryDeviceType("10-0050F204-5");
        pGroupFormedState->deviceManager.AddDevice(device);
    }
    void AddDeviceManager()
    {
        WifiP2pDevice device;
        device.SetDeviceName("device");
        device.SetDeviceAddress("AA:BB:CC:DD:EE:FF");
        device.SetGroupCapabilitys(static_cast<int>(P2pGroupCapability::PGC_GROUP_OWNER));
        device.SetPrimaryDeviceType("10-0050F204-5");
        pGroupFormedState->deviceManager.AddDevice(device);
    }
    std::unique_ptr<GroupFormedState> pGroupFormedState;
    std::unique_ptr<MockP2pPendant> pMockP2pPendant;
    WifiP2pGroupManager groupManager;
    WifiP2pDeviceManager deviceManager;
};

HWTEST_F(GroupFormedStateTest, GoInState, TestSize.Level1)
{
    AddGroupManager();
    pGroupFormedState->GoInState();
}

HWTEST_F(GroupFormedStateTest, GoOutState, TestSize.Level1)
{
    pGroupFormedState->GoOutState();
}

HWTEST_F(GroupFormedStateTest, ExecuteStateMsg1, TestSize.Level1)
{
    InternalMessage msg;
    WifiP2pDevice device;
    AddGroupManager();
    AddDeviceManager();
    device.SetDeviceAddress("AA:BB:CC:DD:EE:FF");
    msg.SetMessageObj(device);
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::AP_STA_CONNECTED));
    EXPECT_CALL(WifiP2PHalInterface::GetInstance(), SetP2pGroupIdle(_, _))
        .WillRepeatedly(Return(WifiErrorNo::WIFI_IDL_OPT_FAILED));
    EXPECT_TRUE(pGroupFormedState->ExecuteStateMsg(&msg));
}

HWTEST_F(GroupFormedStateTest, ExecuteStateMsg1break, TestSize.Level1)
{
    InternalMessage msg;
    AddGroupManager();
    WifiP2pDevice device;
    msg.SetMessageObj(device);
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::AP_STA_CONNECTED));
    EXPECT_TRUE(pGroupFormedState->ExecuteStateMsg(&msg));
}

HWTEST_F(GroupFormedStateTest, ExecuteStateMsg2, TestSize.Level1)
{
    InternalMessage msg;
    WifiP2pDevice device;
    AddGroupManager();
    device.SetDeviceAddress("AA:BB:CC:DD:EE:FF");
    msg.SetMessageObj(device);
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::AP_STA_DISCONNECTED));
    EXPECT_TRUE(pGroupFormedState->ExecuteStateMsg(&msg));
}

HWTEST_F(GroupFormedStateTest, ExecuteStateMsg2break, TestSize.Level1)
{
    InternalMessage msg;
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::WPA_CONNECTED_EVENT));
    EXPECT_FALSE(pGroupFormedState->ExecuteStateMsg(&msg));
    WifiP2pDevice device;
    msg.SetMessageObj(device);
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::AP_STA_DISCONNECTED));
    EXPECT_TRUE(pGroupFormedState->ExecuteStateMsg(&msg));
}

HWTEST_F(GroupFormedStateTest, ExecuteStateMsg3, TestSize.Level1)
{
    InternalMessage msg;
    WifiP2pDevice device;
    AddGroupManager();
    device.SetDeviceAddress("AA:BB:CC:DD:EE:FF");
    msg.SetMessageObj(device);
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::P2P_EVENT_DEVICE_LOST));
    EXPECT_TRUE(pGroupFormedState->ExecuteStateMsg(&msg));
}

HWTEST_F(GroupFormedStateTest, ExecuteStateMsg3break, TestSize.Level1)
{
    InternalMessage msg;
    WifiP2pDevice device;
    msg.SetMessageObj(device);
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::P2P_EVENT_DEVICE_LOST));
    EXPECT_TRUE(pGroupFormedState->ExecuteStateMsg(&msg));
}

HWTEST_F(GroupFormedStateTest, ExecuteStateMsg4, TestSize.Level1)
{
    InternalMessage msg;
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::CMD_P2P_DISABLE));
    EXPECT_TRUE(pGroupFormedState->ExecuteStateMsg(&msg));
}

HWTEST_F(GroupFormedStateTest, ProcessCmdConnect1, TestSize.Level1)
{
    WifiP2pConfigInternal config;
    InternalMessage msg;
    msg.SetMessageObj(config);
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::CMD_CONNECT));
    EXPECT_TRUE(pGroupFormedState->ExecuteStateMsg(&msg));
}

HWTEST_F(GroupFormedStateTest, ProcessCmdConnect2, TestSize.Level1)
{
    WifiP2pConfigInternal config;
    AddDeviceManager();
    InternalMessage msg;
    config.SetDeviceAddress("AA:BB:CC:DD:EE:FF");
    msg.SetMessageObj(config);
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::CMD_CONNECT));
    EXPECT_TRUE(pGroupFormedState->ExecuteStateMsg(&msg));
    AddDeviceStateMachine();
    EXPECT_TRUE(pGroupFormedState->ExecuteStateMsg(&msg));
}

HWTEST_F(GroupFormedStateTest, ProcessCmdConnect3, TestSize.Level1)
{
    WifiP2pConfigInternal config;
    InternalMessage msg;
    msg.SetMessageObj(config);
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::CMD_CONNECT));
    EXPECT_CALL(pMockP2pPendant->GetP2pStateMachine(), IsConfigUnusable(_))
        .WillOnce(Return(P2pConfigErrCode::MAC_EMPTY));
    EXPECT_CALL(pMockP2pPendant->GetP2pStateMachine(), BroadcastActionResult(_, _)).WillOnce(Return());
    EXPECT_TRUE(pGroupFormedState->ExecuteStateMsg(&msg));
}

HWTEST_F(GroupFormedStateTest, ProcessCmdConnect4, TestSize.Level1)
{
    WifiP2pConfigInternal config;
    InternalMessage msg;
    msg.SetMessageObj(config);
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::CMD_CONNECT));
    EXPECT_CALL(pMockP2pPendant->GetP2pStateMachine(), IsConfigUnusable(_))
        .WillOnce(Return(P2pConfigErrCode::MAC_NOT_FOUND));
    EXPECT_CALL(pMockP2pPendant->GetP2pStateMachine(), BroadcastActionResult(_, _)).WillOnce(Return());
    EXPECT_TRUE(pGroupFormedState->ExecuteStateMsg(&msg));
}

HWTEST_F(GroupFormedStateTest, ProcessCmdConnect5, TestSize.Level1)
{
    WifiP2pConfigInternal config;
    InternalMessage msg;
    msg.SetMessageObj(config);
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::CMD_CONNECT));
    EXPECT_CALL(pMockP2pPendant->GetP2pStateMachine(), IsConfigUnusable(_))
        .WillOnce(Return(P2pConfigErrCode::ERR_MAC_FORMAT));
    EXPECT_CALL(pMockP2pPendant->GetP2pStateMachine(), BroadcastActionResult(_, _)).WillOnce(Return());
    EXPECT_TRUE(pGroupFormedState->ExecuteStateMsg(&msg));
}

HWTEST_F(GroupFormedStateTest, ExecuteStateMsg7, TestSize.Level1)
{
    WifiP2pTempDiscEvent procDisc;
    InternalMessage msg;
    WifiP2pDevice device;
    device.SetDeviceAddress("AA:BB:CC:DD:EE:FF");
    procDisc.SetDevice(device);
    msg.SetMessageObj(procDisc);
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::P2P_EVENT_PROV_DISC_PBC_REQ));
    EXPECT_TRUE(pGroupFormedState->ExecuteStateMsg(&msg));
}

HWTEST_F(GroupFormedStateTest, ExecuteStateMsg8, TestSize.Level1)
{
    WifiP2pTempDiscEvent procDisc;
    InternalMessage msg;
    WifiP2pDevice device;
    device.SetDeviceAddress("AA:BB:CC:DD:EE:FF");
    procDisc.SetDevice(device);
    msg.SetMessageObj(procDisc);
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::P2P_EVENT_PROV_DISC_ENTER_PIN));
    EXPECT_TRUE(pGroupFormedState->ExecuteStateMsg(&msg));
}

HWTEST_F(GroupFormedStateTest, ExecuteStateMsg9, TestSize.Level1)
{
    WifiP2pTempDiscEvent procDisc;
    InternalMessage msg;
    WifiP2pDevice device;
    device.SetDeviceAddress("AA:BB:CC:DD:EE:FF");
    procDisc.SetDevice(device);
    msg.SetMessageObj(procDisc);
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::P2P_EVENT_PROV_DISC_SHOW_PIN));
    EXPECT_TRUE(pGroupFormedState->ExecuteStateMsg(&msg));
    AddGroupManager();
    EXPECT_TRUE(pGroupFormedState->ExecuteStateMsg(&msg));
}

HWTEST_F(GroupFormedStateTest, ExecuteStateMsg9break, TestSize.Level1)
{
    InternalMessage msg;
    WifiP2pTempDiscEvent procDisc;
    msg.SetMessageObj(procDisc);
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::P2P_EVENT_PROV_DISC_SHOW_PIN));
    EXPECT_TRUE(pGroupFormedState->ExecuteStateMsg(&msg));
}

HWTEST_F(GroupFormedStateTest, ExecuteStateMsg10, TestSize.Level1)
{
    InternalMessage msg;
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::P2P_EVENT_GROUP_STARTED));
    EXPECT_TRUE(pGroupFormedState->ExecuteStateMsg(&msg));
}

HWTEST_F(GroupFormedStateTest, ExecuteStateMsg11, TestSize.Level1)
{
    InternalMessage msg;
    EXPECT_CALL(WifiP2PHalInterface::GetInstance(), P2pFind(DISC_TIMEOUT_S))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::CMD_DEVICE_DISCOVERS));
    EXPECT_TRUE(pGroupFormedState->ExecuteStateMsg(&msg));
}

HWTEST_F(GroupFormedStateTest, ExecuteStateMsg12, TestSize.Level1)
{
    InternalMessage msg;
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::P2P_EVENT_GROUP_REMOVED));
    EXPECT_TRUE(pGroupFormedState->ExecuteStateMsg(&msg));
}

HWTEST_F(GroupFormedStateTest, ProcessCmdRemoveGroup1, TestSize.Level1)
{
    InternalMessage msg;
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::CMD_REMOVE_GROUP));
    EXPECT_TRUE(pGroupFormedState->ExecuteStateMsg(&msg));
}

HWTEST_F(GroupFormedStateTest, ProcessCmdRemoveGroup2, TestSize.Level1)
{
    InternalMessage msg;
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::CMD_DELETE_GROUP));
    EXPECT_TRUE(pGroupFormedState->ExecuteStateMsg(&msg));
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::CMD_DISCONNECT));
    EXPECT_FALSE(pGroupFormedState->ExecuteStateMsg(&msg));
    EXPECT_FALSE(pGroupFormedState->ExecuteStateMsg(nullptr));
}

HWTEST_F(GroupFormedStateTest, ProcessCmdCancelConnect, TestSize.Level1)
{
    InternalMessage msg;
    EXPECT_CALL(pMockP2pPendant->GetP2pStateMachine(), BroadcastActionResult(_, _)).WillOnce(Return());
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::CMD_CANCEL_CONNECT));
    EXPECT_TRUE(pGroupFormedState->ExecuteStateMsg(&msg));
}
}  // namespace Wifi
}  // namespace OHOS