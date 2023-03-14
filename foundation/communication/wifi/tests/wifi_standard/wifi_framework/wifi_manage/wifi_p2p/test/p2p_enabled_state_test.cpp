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

#include "p2p_enabled_state.h"
#include "mock_p2p_pendant.h"
#include "mock_wifi_settings.h"
#include "mock_wifi_p2p_hal_interface.h"

using ::testing::_;
using ::testing::DoAll;
using ::testing::Eq;
using ::testing::Return;
using ::testing::SetArgReferee;
using ::testing::ext::TestSize;

namespace OHOS {
namespace Wifi {
class P2pEnabledStateTest : public testing::Test {
public:
    static void SetUpTestCase()
    {}
    static void TearDownTestCase()
    {}
    virtual void SetUp()
    {
        pMockP2pPendant.reset(new MockP2pPendant());
        pP2pEnabledState.reset(new P2pEnabledState(pMockP2pPendant->GetP2pStateMachine(), groupManager, deviceManager));
        pP2pEnabledState->Init();
    }
    virtual void TearDown()
    {
        pP2pEnabledState.reset();
        EXPECT_CALL(WifiP2PHalInterface::GetInstance(), RegisterP2pCallback(_));
        pMockP2pPendant.reset();
    }

public:
    void AddDevice()
    {
        const int testCfgMethod = 128;
        WifiP2pDevice device;
        device.SetDeviceName("device");
        device.SetWpsConfigMethod(testCfgMethod);
        device.SetDeviceAddress("AA:BB:CC:DD:EE:FF");
        device.SetGroupCapabilitys(static_cast<int>(P2pGroupCapability::PGC_GROUP_OWNER));
        device.SetPrimaryDeviceType("10-0050F204-5");
        pP2pEnabledState->deviceManager.AddDevice(device);
    }
    std::unique_ptr<P2pEnabledState> pP2pEnabledState;
    std::unique_ptr<MockP2pPendant> pMockP2pPendant;
    WifiP2pGroupManager groupManager;
    WifiP2pDeviceManager deviceManager;
    WifiP2pServiceManager serviceManager;
};

HWTEST_F(P2pEnabledStateTest, GoInState, TestSize.Level1)
{
    EXPECT_CALL(WifiP2PHalInterface::GetInstance(), SetP2pDeviceName(_))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_FAILED))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    EXPECT_CALL(WifiP2PHalInterface::GetInstance(), SetP2pSsidPostfix(_))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_FAILED))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    EXPECT_CALL(WifiP2PHalInterface::GetInstance(), SetP2pDeviceType(_))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_FAILED))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    EXPECT_CALL(WifiP2PHalInterface::GetInstance(), SetP2pConfigMethods(_))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_FAILED))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    EXPECT_CALL(WifiP2PHalInterface::GetInstance(), GetDeviceAddress(_))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_FAILED))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    EXPECT_CALL(WifiP2PHalInterface::GetInstance(), P2pFlush())
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_FAILED))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    EXPECT_CALL(WifiP2PHalInterface::GetInstance(), FlushService())
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_FAILED))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    EXPECT_CALL(WifiP2PHalInterface::GetInstance(), SetServiceDiscoveryExternal(Eq(false)))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_FAILED))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    EXPECT_CALL(WifiP2PHalInterface::GetInstance(), ListNetworks(_))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_FAILED))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_FAILED));
    EXPECT_CALL(WifiP2PHalInterface::GetInstance(), SetPersistentReconnect(Eq(1)))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_FAILED))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    EXPECT_CALL(pMockP2pPendant->GetP2pStateMachine(), UpdateGroupInfoToWpa()).WillOnce(Return()).WillOnce(Return());
    EXPECT_CALL(pMockP2pPendant->GetP2pStateMachine(), UpdatePersistentGroups()).WillOnce(Return()).WillOnce(Return());
    pP2pEnabledState->GoInState();
    pP2pEnabledState->GoInState();
}

HWTEST_F(P2pEnabledStateTest, GoOutState, TestSize.Level1)
{
    pP2pEnabledState->GoOutState();
}

HWTEST_F(P2pEnabledStateTest, ExecuteStateMsg, TestSize.Level1)
{
    EXPECT_FALSE(pP2pEnabledState->ExecuteStateMsg(nullptr));
}

HWTEST_F(P2pEnabledStateTest, ExecuteStateMsg1, TestSize.Level1)
{
    InternalMessage msg;
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::CMD_P2P_DISABLE));
    EXPECT_CALL(WifiP2PHalInterface::GetInstance(), P2pStopFind()).WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_FAILED));
    EXPECT_CALL(WifiP2PHalInterface::GetInstance(), StopP2p()).WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_FAILED));
    EXPECT_TRUE(pP2pEnabledState->ExecuteStateMsg(&msg));
}

HWTEST_F(P2pEnabledStateTest, ExecuteStateMsg2, TestSize.Level1)
{
    InternalMessage msg;
    msg.SetParam1(1);
    msg.SetParam2(1);
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::CMD_START_LISTEN));
    EXPECT_CALL(WifiP2PHalInterface::GetInstance(), P2pFlush())
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_FAILED))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_FAILED))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_FAILED));
    EXPECT_CALL(WifiP2PHalInterface::GetInstance(), SetListenChannel(_, _))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_FAILED))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_OK))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    EXPECT_CALL(WifiP2PHalInterface::GetInstance(), P2pConfigureListen(_, _, _))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_FAILED))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    EXPECT_TRUE(pP2pEnabledState->ExecuteStateMsg(&msg));
    EXPECT_TRUE(pP2pEnabledState->ExecuteStateMsg(&msg));
    EXPECT_TRUE(pP2pEnabledState->ExecuteStateMsg(&msg));
}

HWTEST_F(P2pEnabledStateTest, ExecuteStateMsg3, TestSize.Level1)
{
    InternalMessage msg;
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::CMD_STOP_LISTEN));
    EXPECT_CALL(WifiP2PHalInterface::GetInstance(), P2pConfigureListen(_, _, _))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_OK))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_FAILED));
    EXPECT_CALL(WifiP2PHalInterface::GetInstance(), P2pFlush())
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_FAILED))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    pP2pEnabledState->ExecuteStateMsg(&msg);
    pP2pEnabledState->ExecuteStateMsg(&msg);
}

HWTEST_F(P2pEnabledStateTest, ExecuteStateMsg4, TestSize.Level1)
{
    InternalMessage msg;
    EXPECT_CALL(WifiP2PHalInterface::GetInstance(), P2pFind(_)).WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_FAILED));
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::CMD_DEVICE_DISCOVERS));
    pP2pEnabledState->ExecuteStateMsg(&msg);
}

HWTEST_F(P2pEnabledStateTest, ExecuteStateMsg5, TestSize.Level1)
{
    InternalMessage msg;
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::CMD_STOP_DEVICE_DISCOVERS));
    EXPECT_CALL(WifiP2PHalInterface::GetInstance(), P2pStopFind())
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_OK))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_FAILED));
    pP2pEnabledState->ExecuteStateMsg(&msg);
    pP2pEnabledState->ExecuteStateMsg(&msg);
}

HWTEST_F(P2pEnabledStateTest, ProcessDeviceFoundEvt1, TestSize.Level1)
{
    InternalMessage msg;
    WifiP2pDevice device;
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::P2P_EVENT_DEVICE_FOUND));
    msg.SetMessageObj(device);
    pP2pEnabledState->ExecuteStateMsg(&msg);
}

HWTEST_F(P2pEnabledStateTest, ProcessDeviceFoundEvt2, TestSize.Level1)
{
    InternalMessage msg;
    WifiP2pDevice device;
    device.SetDeviceAddress(std::string("aa:bb:cc:dd:ee:22"));
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::P2P_EVENT_DEVICE_FOUND));
    msg.SetMessageObj(device);
    EXPECT_CALL(pMockP2pPendant->GetP2pStateMachine(), BroadcastP2pPeersChanged()).WillOnce(Return());
    pP2pEnabledState->ExecuteStateMsg(&msg);
}

HWTEST_F(P2pEnabledStateTest, ExecuteStateMsg7, TestSize.Level1)
{
    InternalMessage msg;
    WifiP2pDevice device;
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::P2P_EVENT_DEVICE_LOST));
    msg.SetMessageObj(device);
    pP2pEnabledState->ExecuteStateMsg(&msg);

    AddDevice();
    device.SetDeviceAddress("AA:BB:CC:DD:EE:FF");
    msg.SetMessageObj(device);
    pP2pEnabledState->ExecuteStateMsg(&msg);
}

HWTEST_F(P2pEnabledStateTest, ExecuteStateMsg8, TestSize.Level1)
{
    InternalMessage msg;
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::P2P_EVENT_FIND_STOPPED));
    pP2pEnabledState->ExecuteStateMsg(&msg);
}

HWTEST_F(P2pEnabledStateTest, ExecuteStateMsg9, TestSize.Level1)
{
    InternalMessage msg;
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::CMD_DELETE_GROUP));
    pP2pEnabledState->ExecuteStateMsg(&msg);

}

HWTEST_F(P2pEnabledStateTest, ExecuteStateMsg10, TestSize.Level1)
{
    InternalMessage msg;
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::CMD_PUT_LOCAL_SERVICE));
    WifiP2pServiceInfo service;
    msg.SetMessageObj(service);
    EXPECT_CALL(WifiP2PHalInterface::GetInstance(), P2pServiceAdd(_))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_OK))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_FAILED));
    pP2pEnabledState->ExecuteStateMsg(&msg);
    pP2pEnabledState->ExecuteStateMsg(&msg);
}

HWTEST_F(P2pEnabledStateTest, ExecuteStateMsg11, TestSize.Level1)
{
    InternalMessage msg;
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::CMD_DEL_LOCAL_SERVICE));
    WifiP2pServiceInfo service;
    msg.SetMessageObj(service);
    EXPECT_CALL(WifiP2PHalInterface::GetInstance(), P2pServiceRemove(_))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_OK))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_FAILED));
    pP2pEnabledState->ExecuteStateMsg(&msg);
    pP2pEnabledState->ExecuteStateMsg(&msg);
}

HWTEST_F(P2pEnabledStateTest, ExecuteStateMsg12, TestSize.Level1)
{
    InternalMessage msg;
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::CMD_STOP_DISCOVER_SERVICES));
    EXPECT_CALL(WifiP2PHalInterface::GetInstance(), P2pStopFind())
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_OK))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_FAILED));
    pP2pEnabledState->ExecuteStateMsg(&msg);
    pP2pEnabledState->ExecuteStateMsg(&msg);
}

HWTEST_F(P2pEnabledStateTest, ExecuteStateMsg13, TestSize.Level1)
{
    InternalMessage msg;
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::CMD_REQUEST_SERVICE));
    std::pair<WifiP2pDevice, WifiP2pServiceRequest> info;
    msg.SetMessageObj(info);
    EXPECT_CALL(WifiP2PHalInterface::GetInstance(), ReqServiceDiscovery(_, _, _))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_FAILED))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_OK))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    pP2pEnabledState->ExecuteStateMsg(&msg);
    EXPECT_CALL(WifiP2PHalInterface::GetInstance(), P2pFind(_))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_OK))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_FAILED));
    pP2pEnabledState->ExecuteStateMsg(&msg);
    pP2pEnabledState->ExecuteStateMsg(&msg);
}

HWTEST_F(P2pEnabledStateTest, ExecuteStateMsg14, TestSize.Level1)
{
    InternalMessage msg;
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::P2P_EVENT_SERV_DISC_REQ));
    WifiP2pServiceRequestList reqList;
    const WifiP2pServiceRequest req;
    reqList.AddServiceRequest(req);
    WifiP2pDevice device;
    device.SetDeviceAddress("AA:BB:CC:DD:EE:FF");
    reqList.SetDevice(device);
    msg.SetMessageObj(reqList);
    EXPECT_CALL(WifiP2PHalInterface::GetInstance(), RespServiceDiscovery(_, _, _, _))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_FAILED));
    pP2pEnabledState->ExecuteStateMsg(&msg);
    pP2pEnabledState->ExecuteStateMsg(&msg);
}

HWTEST_F(P2pEnabledStateTest, ExecuteStateMsg15, TestSize.Level1)
{
    InternalMessage msg;
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::P2P_EVENT_SERV_DISC_RESP));

    WifiP2pServiceResponseList respList;
    std::vector<unsigned char> data;
    WifiP2pServiceResponse resp(
        P2pServicerProtocolType::SERVICE_TYPE_BONJOUR, P2pServiceStatus::PSRS_SERVICE_PROTOCOL_NOT_AVAILABLE, 0, data);
    respList.AddServiceResponse(resp);
    msg.SetMessageObj(respList);
    pP2pEnabledState->ExecuteStateMsg(&msg);
}

HWTEST_F(P2pEnabledStateTest, ExecuteStateMsg16, TestSize.Level1)
{
    InternalMessage msg;
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::P2P_EVENT_PROV_DISC_FAILURE));
    pP2pEnabledState->ExecuteStateMsg(&msg);
}

HWTEST_F(P2pEnabledStateTest, ExecuteStateMsg17, TestSize.Level1)
{
    InternalMessage msg;
    WifiP2pServiceInfo service;
    msg.SetMessageObj(service);
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::CMD_PUT_LOCAL_SERVICE));
    pP2pEnabledState->ExecuteStateMsg(&msg);
}

HWTEST_F(P2pEnabledStateTest, ExecuteStateMsg18, TestSize.Level1)
{
    InternalMessage msg;
    WifiP2pServiceInfo service;
    msg.SetMessageObj(service);
    EXPECT_CALL(WifiP2PHalInterface::GetInstance(), P2pServiceRemove(_)).WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::CMD_DEL_LOCAL_SERVICE));
    pP2pEnabledState->ExecuteStateMsg(&msg);
    EXPECT_CALL(WifiP2PHalInterface::GetInstance(), ReqServiceDiscovery(_, _, _))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_FAILED))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_OK))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    EXPECT_CALL(WifiP2PHalInterface::GetInstance(), P2pFind(_))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_OK))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_FAILED));
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::CMD_DISCOVER_SERVICES));
    pP2pEnabledState->ExecuteStateMsg(&msg);
    pP2pEnabledState->ExecuteStateMsg(&msg);
    pP2pEnabledState->ExecuteStateMsg(&msg);
}

HWTEST_F(P2pEnabledStateTest, ExecuteStateMsg19, TestSize.Level1)
{
    InternalMessage msg;
    EXPECT_CALL(WifiP2PHalInterface::GetInstance(), P2pStopFind())
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_OK))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_FAILED));
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::CMD_STOP_DISCOVER_SERVICES));
    pP2pEnabledState->ExecuteStateMsg(&msg);
    pP2pEnabledState->ExecuteStateMsg(&msg);
}

HWTEST_F(P2pEnabledStateTest, ExecuteStateMsg20, TestSize.Level1)
{
    InternalMessage msg;
    std::pair<WifiP2pDevice, WifiP2pServiceRequest> info;
    msg.SetMessageObj(info);
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::CMD_REQUEST_SERVICE));
    EXPECT_CALL(WifiP2PHalInterface::GetInstance(), ReqServiceDiscovery(_, _, _))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_FAILED))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_OK))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    EXPECT_CALL(WifiP2PHalInterface::GetInstance(), P2pFind(_))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_FAILED))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    pP2pEnabledState->ExecuteStateMsg(&msg);
    pP2pEnabledState->ExecuteStateMsg(&msg);
    pP2pEnabledState->ExecuteStateMsg(&msg);
}

HWTEST_F(P2pEnabledStateTest, ExecuteStateMsg21, TestSize.Level1)
{
    InternalMessage msg;
    WifiP2pServiceRequestList reqList;
    msg.SetMessageObj(reqList);
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::P2P_EVENT_SERV_DISC_REQ));
    pP2pEnabledState->ExecuteStateMsg(&msg);
}

HWTEST_F(P2pEnabledStateTest, ExecuteStateMsg22, TestSize.Level1)
{
    InternalMessage msg;
    WifiP2pServiceResponseList respList;
    msg.SetMessageObj(respList);
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::P2P_EVENT_SERV_DISC_RESP));
    pP2pEnabledState->ExecuteStateMsg(&msg);

    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::P2P_EVENT_PROV_DISC_FAILURE));
    pP2pEnabledState->ExecuteStateMsg(&msg);
}

HWTEST_F(P2pEnabledStateTest, ExecuteStateMsg23, TestSize.Level1)
{
    InternalMessage msg;
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::EXCEPTION_TIMED_OUT));
    pP2pEnabledState->ExecuteStateMsg(&msg);
}

HWTEST_F(P2pEnabledStateTest, ProcessCmdSetDeviceName1, TestSize.Level1)
{
    InternalMessage msg;
    std::string devAddr;
    msg.SetMessageObj(devAddr);
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::CMD_SET_DEVICE_NAME));
    EXPECT_CALL(WifiP2PHalInterface::GetInstance(), SetP2pDeviceName(Eq(devAddr)))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_FAILED));
    EXPECT_CALL(pMockP2pPendant->GetP2pStateMachine(), BroadcastActionResult(_, _)).WillOnce(Return());
    EXPECT_TRUE(pP2pEnabledState->ExecuteStateMsg(&msg));
}

HWTEST_F(P2pEnabledStateTest, ProcessCmdSetDeviceName2, TestSize.Level1)
{
    InternalMessage msg;
    std::string devAddr;
    msg.SetMessageObj(devAddr);
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::CMD_SET_DEVICE_NAME));
    EXPECT_CALL(WifiP2PHalInterface::GetInstance(), SetP2pDeviceName(Eq(devAddr)))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    EXPECT_CALL(pMockP2pPendant->GetP2pStateMachine(), BroadcastThisDeviceChanaged(_)).WillOnce(Return());
    EXPECT_CALL(pMockP2pPendant->GetP2pStateMachine(), BroadcastActionResult(_, _)).WillOnce(Return());
    EXPECT_CALL(WifiP2PHalInterface::GetInstance(), SetP2pSsidPostfix(_))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_FAILED));
    EXPECT_TRUE(pP2pEnabledState->ExecuteStateMsg(&msg));
}

HWTEST_F(P2pEnabledStateTest, ProcessCmdSetWfdInfo1, TestSize.Level1)
{
    InternalMessage msg;
    WifiP2pWfdInfo wfdInfo;
    msg.SetMessageObj(wfdInfo);
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::CMD_SET_WFD_INFO));
    EXPECT_CALL(WifiP2PHalInterface::GetInstance(), SetWfdDeviceConfig(_))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_FAILED));
    EXPECT_TRUE(pP2pEnabledState->ExecuteStateMsg(&msg));
}

HWTEST_F(P2pEnabledStateTest, ProcessCmdSetWfdInfo2, TestSize.Level1)
{
    InternalMessage msg;
    WifiP2pWfdInfo wfdInfo;
    msg.SetMessageObj(wfdInfo);
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::CMD_SET_WFD_INFO));
    EXPECT_CALL(WifiP2PHalInterface::GetInstance(), SetWfdDeviceConfig(_))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    EXPECT_CALL(WifiP2PHalInterface::GetInstance(), SetWfdEnable(_)).WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_FAILED));
    EXPECT_TRUE(pP2pEnabledState->ExecuteStateMsg(&msg));
}

HWTEST_F(P2pEnabledStateTest, ProcessCmdSetWfdInfo3, TestSize.Level1)
{
    InternalMessage msg;
    WifiP2pWfdInfo wfdInfo;
    msg.SetMessageObj(wfdInfo);
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::CMD_SET_WFD_INFO));
    EXPECT_CALL(WifiP2PHalInterface::GetInstance(), SetWfdDeviceConfig(_))
        .WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    EXPECT_CALL(WifiP2PHalInterface::GetInstance(), SetWfdEnable(_)).WillOnce(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    EXPECT_TRUE(pP2pEnabledState->ExecuteStateMsg(&msg));
}

HWTEST_F(P2pEnabledStateTest, ProcessCmdCancelConnect, TestSize.Level1)
{
    InternalMessage msg;
    msg.SetMessageName(static_cast<int>(P2P_STATE_MACHINE_CMD::CMD_CANCEL_CONNECT));
    EXPECT_CALL(pMockP2pPendant->GetP2pStateMachine(), BroadcastActionResult(_, _)).WillOnce(Return());
    EXPECT_TRUE(pP2pEnabledState->ExecuteStateMsg(&msg));
}
} // namespace Wifi
} // namespace OHOS