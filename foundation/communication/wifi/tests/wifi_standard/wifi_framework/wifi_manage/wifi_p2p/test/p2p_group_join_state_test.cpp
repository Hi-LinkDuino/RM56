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
#include "p2p_group_join_state.h"
#include "mock_p2p_monitor.h"
#include "mock_wifi_settings.h"

using ::testing::_;
using ::testing::Return;
using ::testing::ext::TestSize;

namespace OHOS {
namespace Wifi {
class P2pGroupJoinStateTest : public testing::Test {
public:
    static void SetUpTestCase()
    {}
    static void TearDownTestCase()
    {}
    virtual void SetUp()
    {
        pMockP2pPendant.reset(new MockP2pPendant());
        pP2pGroupJoinState.reset(
            new P2pGroupJoinState(pMockP2pPendant->GetP2pStateMachine(), groupManager, deviceManager));
        pGroupFormedState.reset(
            new GroupFormedState(pMockP2pPendant->GetP2pStateMachine(), groupManager, deviceManager));
    }
    virtual void TearDown()
    {
        pP2pGroupJoinState.reset();
        EXPECT_CALL(WifiP2PHalInterface::GetInstance(), RegisterP2pCallback(_));
        pGroupFormedState.reset();
        pMockP2pPendant.reset();
    }

public:
    void AddSaveP2pConfig1()
    {
        InternalMessage msg;
        msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::P2P_EVENT_PROV_DISC_PBC_REQ));

        WifiP2pTempDiscEvent procDisc;

        WifiP2pDevice device;
        device.SetDeviceAddress("AA:BB:CC:DD:EE:FF");
        procDisc.SetDevice(device);
        msg.SetMessageObj(procDisc);
        pGroupFormedState->ExecuteStateMsg(&msg);
    }
    void AddSaveP2pConfig2()
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
    std::unique_ptr<P2pGroupJoinState> pP2pGroupJoinState;
    std::unique_ptr<MockP2pPendant> pMockP2pPendant;
    std::unique_ptr<GroupFormedState> pGroupFormedState;
    WifiP2pGroupManager groupManager;
    WifiP2pDeviceManager deviceManager;
};

HWTEST_F(P2pGroupJoinStateTest, GoInState, TestSize.Level1)
{
    pP2pGroupJoinState->GoInState();
}

HWTEST_F(P2pGroupJoinStateTest, GoOutState, TestSize.Level1)
{
    pP2pGroupJoinState->GoOutState();
}

HWTEST_F(P2pGroupJoinStateTest, ExecuteStateMsg, TestSize.Level1)
{
    InternalMessage msg;
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::P2P_EVENT_PROV_DISC_FAILURE));
    pP2pGroupJoinState->ExecuteStateMsg(&msg);

    AddSaveP2pConfig2();
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::INTERNAL_CONN_USER_ACCEPT));
    std::string inputPin;
    msg.SetMessageObj(inputPin);
    EXPECT_CALL(WifiP2PHalInterface::GetInstance(), P2pStopFind())
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_FAILED))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_FAILED));
    EXPECT_CALL(WifiP2PHalInterface::GetInstance(), StartWpsPin(_, _, _, _))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_FAILED));
    EXPECT_CALL(WifiP2PHalInterface::GetInstance(), StartWpsPbc(_, _))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_FAILED));
    pP2pGroupJoinState->ExecuteStateMsg(&msg);
    AddSaveP2pConfig1();
    pP2pGroupJoinState->ExecuteStateMsg(&msg);
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::PEER_CONNECTION_USER_REJECT));
    pP2pGroupJoinState->ExecuteStateMsg(&msg);
}

HWTEST_F(P2pGroupJoinStateTest, ProcessCmdP2pDisable, TestSize.Level1)
{
    InternalMessage msg;
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::CMD_P2P_DISABLE));
    pP2pGroupJoinState->ExecuteStateMsg(&msg);
}
}  // namespace Wifi
}  // namespace OHOS