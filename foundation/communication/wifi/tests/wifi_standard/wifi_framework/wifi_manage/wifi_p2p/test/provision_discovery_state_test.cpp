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
#include "provision_discovery_state.h"
#include "mock_p2p_monitor.h"
#include "mock_wifi_settings.h"

using ::testing::_;
using ::testing::Return;
using ::testing::ext::TestSize;

namespace OHOS {
namespace Wifi {
class ProvisionDiscoveryStateTest : public testing::Test {
public:
    static void SetUpTestCase()
    {}
    static void TearDownTestCase()
    {}
    virtual void SetUp()
    {
        pMockP2pPendant.reset(new MockP2pPendant());
        pProvisionDiscoveryState.reset(
            new ProvisionDiscoveryState(pMockP2pPendant->GetP2pStateMachine(), groupManager, deviceManager));
        pGroupFormedState.reset(
            new GroupFormedState(pMockP2pPendant->GetP2pStateMachine(), groupManager, deviceManager));
        pProvisionDiscoveryState->Init();
    }
    virtual void TearDown()
    {
        pProvisionDiscoveryState.reset();
        pGroupFormedState.reset();
        EXPECT_CALL(WifiP2PHalInterface::GetInstance(), RegisterP2pCallback(_));
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
    void AddSaveP2pConfig3()
    {
        InternalMessage msg;
        msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::P2P_EVENT_PROV_DISC_SHOW_PIN));

        WifiP2pTempDiscEvent procDisc;
        WifiP2pDevice device;
        device.SetDeviceAddress("AA:BB:CC:DD:EE:FF");
        procDisc.SetDevice(device);
        msg.SetMessageObj(procDisc);
        pGroupFormedState->ExecuteStateMsg(&msg);
    }
    std::unique_ptr<ProvisionDiscoveryState> pProvisionDiscoveryState;
    std::unique_ptr<GroupFormedState> pGroupFormedState;
    std::unique_ptr<MockP2pPendant> pMockP2pPendant;
    WifiP2pGroupManager groupManager;
    WifiP2pDeviceManager deviceManager;
};

HWTEST_F(ProvisionDiscoveryStateTest, GoInState, TestSize.Level1)
{
    EXPECT_CALL(WifiP2PHalInterface::GetInstance(), ProvisionDiscovery(_))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_FAILED));
    pProvisionDiscoveryState->GoInState();
}

HWTEST_F(ProvisionDiscoveryStateTest, GoOutState, TestSize.Level1)
{
    pProvisionDiscoveryState->GoOutState();
}

HWTEST_F(ProvisionDiscoveryStateTest, ExecuteStateMsg1, TestSize.Level1)
{
    InternalMessage msg;
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::CMD_DEVICE_DISCOVERS));
    EXPECT_TRUE(pProvisionDiscoveryState->ExecuteStateMsg(&msg));

    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::P2P_EVENT_PROV_DISC_PBC_RESP));

    WifiP2pTempDiscEvent provDisc;
    WifiP2pDevice device;
    device.SetDeviceAddress("AA:BB:CC:DD:EE:FF");
    provDisc.SetDevice(device);
    msg.SetMessageObj(provDisc);
    EXPECT_TRUE(pProvisionDiscoveryState->ExecuteStateMsg(&msg));

    AddSaveP2pConfig1();
    EXPECT_TRUE(pProvisionDiscoveryState->ExecuteStateMsg(&msg));
}

HWTEST_F(ProvisionDiscoveryStateTest, ExecuteStateMsg2, TestSize.Level1)
{
    InternalMessage msg;
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::P2P_EVENT_PROV_DISC_ENTER_PIN));

    WifiP2pTempDiscEvent provDisc;
    msg.SetMessageObj(provDisc);
    EXPECT_TRUE(pProvisionDiscoveryState->ExecuteStateMsg(&msg));

    WifiP2pDevice device;
    device.SetDeviceAddress("AA:BB:CC:DD:EE:FF");
    provDisc.SetDevice(device);
    msg.SetMessageObj(provDisc);
    AddSaveP2pConfig2();
    EXPECT_TRUE(pProvisionDiscoveryState->ExecuteStateMsg(&msg));

    device.SetDeviceAddress("AA:BB:CC:DD:EE:FA");
    provDisc.SetDevice(device);
    msg.SetMessageObj(provDisc);
    EXPECT_TRUE(pProvisionDiscoveryState->ExecuteStateMsg(&msg));
}

HWTEST_F(ProvisionDiscoveryStateTest, ExecuteStateMsg3, TestSize.Level1)
{
    InternalMessage msg;
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::P2P_EVENT_PROV_DISC_SHOW_PIN));

    WifiP2pTempDiscEvent provDisc;
    msg.SetMessageObj(provDisc);
    EXPECT_TRUE(pProvisionDiscoveryState->ExecuteStateMsg(&msg));
    WifiP2pDevice device;
    device.SetDeviceAddress("AA:BB:CC:DD:EE:FF");
    provDisc.SetDevice(device);
    msg.SetMessageObj(provDisc);
    EXPECT_TRUE(pProvisionDiscoveryState->ExecuteStateMsg(&msg));

    AddSaveP2pConfig3();
    EXPECT_TRUE(pProvisionDiscoveryState->ExecuteStateMsg(&msg));

    provDisc.SetPin(std::string("12345678"));
    msg.SetMessageObj(provDisc);
    EXPECT_TRUE(pProvisionDiscoveryState->ExecuteStateMsg(&msg));
}

HWTEST_F(ProvisionDiscoveryStateTest, ExecuteStateMsg4, TestSize.Level1)
{
    InternalMessage msg;
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::AP_STA_CONNECTED));
    EXPECT_FALSE(pProvisionDiscoveryState->ExecuteStateMsg(&msg));
    EXPECT_FALSE(pProvisionDiscoveryState->ExecuteStateMsg(nullptr));
}

HWTEST_F(ProvisionDiscoveryStateTest, ExecuteStateMsgFailed, TestSize.Level1)
{
    InternalMessage msg;
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::PEER_CONNECTION_USER_REJECT));
    EXPECT_FALSE(pProvisionDiscoveryState->ExecuteStateMsg(&msg));
}

HWTEST_F(ProvisionDiscoveryStateTest, ProcessProvDiscFailEvt, TestSize.Level1)
{
    InternalMessage msg;
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::P2P_EVENT_PROV_DISC_FAILURE));
    EXPECT_CALL(pMockP2pPendant->GetP2pStateMachine(), DealGroupCreationFailed()).WillOnce(Return());
    EXPECT_TRUE(pProvisionDiscoveryState->ExecuteStateMsg(&msg));
}

HWTEST_F(ProvisionDiscoveryStateTest, ProcessCmdCancelConnect, TestSize.Level1)
{
    InternalMessage msg;
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::CMD_CANCEL_CONNECT));
    EXPECT_CALL(WifiP2PHalInterface::GetInstance(), CancelConnect()).WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    EXPECT_CALL(pMockP2pPendant->GetP2pStateMachine(), DealGroupCreationFailed()).WillOnce(Return());
    EXPECT_CALL(pMockP2pPendant->GetP2pStateMachine(), BroadcastActionResult(_, _)).WillOnce(Return());
    EXPECT_TRUE(pProvisionDiscoveryState->ExecuteStateMsg(&msg));
}
}  // namespace Wifi
}  // namespace OHOS