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
#include <gtest/gtest.h>

#include "wifi_p2p_service.h"
#include "wifi_p2p_msg.h"
#include "wifi_settings.h"
#include "mock_p2p_pendant.h"
#include "mock_wifi_p2p_hal_interface.h"

using ::testing::Return;
using ::testing::ext::TestSize;

namespace OHOS {
namespace Wifi {
class WifiP2pServiceTest : public testing::Test {
public:
    WifiP2pServiceTest() : groupManager(), deviceManager(), svrManager()
    {}
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    virtual void SetUp()
    {
        pMockP2pPendant.reset(new MockP2pPendant());
        pWifiP2pService.reset(
            new WifiP2pService(pMockP2pPendant->GetP2pStateMachine(), deviceManager, groupManager, svrManager));
    }
    virtual void TearDown()
    {
        EXPECT_CALL(WifiP2PHalInterface::GetInstance(), RegisterP2pCallback(testing::_));
        pWifiP2pService.reset();
        pMockP2pPendant.reset();
    }

public:
    std::unique_ptr<WifiP2pService> pWifiP2pService;
    std::unique_ptr<MockP2pPendant> pMockP2pPendant;
    WifiP2pGroupManager groupManager;
    WifiP2pDeviceManager deviceManager;
    WifiP2pServiceManager svrManager;
};
HWTEST_F(WifiP2pServiceTest, EnableP2p, TestSize.Level1)
{
    EXPECT_EQ(pWifiP2pService->EnableP2p(), ErrCode::WIFI_OPT_SUCCESS);
}

HWTEST_F(WifiP2pServiceTest, DisableP2p, TestSize.Level1)
{
    EXPECT_EQ(pWifiP2pService->DisableP2p(), ErrCode::WIFI_OPT_SUCCESS);
}

HWTEST_F(WifiP2pServiceTest, DiscoverDevices, TestSize.Level1)
{
    EXPECT_EQ(pWifiP2pService->DiscoverDevices(), ErrCode::WIFI_OPT_SUCCESS);
}

HWTEST_F(WifiP2pServiceTest, StopDiscoverDevices, TestSize.Level1)
{
    EXPECT_EQ(pWifiP2pService->StopDiscoverDevices(), ErrCode::WIFI_OPT_SUCCESS);
}

HWTEST_F(WifiP2pServiceTest, DiscoverServices, TestSize.Level1)
{
    EXPECT_EQ(pWifiP2pService->DiscoverServices(), ErrCode::WIFI_OPT_SUCCESS);
}
HWTEST_F(WifiP2pServiceTest, StopDiscoverServices, TestSize.Level1)
{
    EXPECT_EQ(pWifiP2pService->StopDiscoverServices(), ErrCode::WIFI_OPT_SUCCESS);
}

HWTEST_F(WifiP2pServiceTest, PutLocalP2pService, TestSize.Level1)
{
    WifiP2pServiceInfo srvInfo;
    EXPECT_EQ(pWifiP2pService->PutLocalP2pService(srvInfo), ErrCode::WIFI_OPT_SUCCESS);
}

HWTEST_F(WifiP2pServiceTest, DeleteLocalP2pService, TestSize.Level1)
{
    WifiP2pServiceInfo srvInfo;
    EXPECT_EQ(pWifiP2pService->DeleteLocalP2pService(srvInfo), ErrCode::WIFI_OPT_SUCCESS);
}

HWTEST_F(WifiP2pServiceTest, RequestService, TestSize.Level1)
{
    WifiP2pDevice device;
    WifiP2pServiceRequest request;
    EXPECT_EQ(pWifiP2pService->RequestService(device, request), ErrCode::WIFI_OPT_SUCCESS);
}

HWTEST_F(WifiP2pServiceTest, StartP2pListen, TestSize.Level1)
{
    int period = 0;
    int interval = 0;
    EXPECT_EQ(pWifiP2pService->StartP2pListen(period, interval), ErrCode::WIFI_OPT_SUCCESS);
}

HWTEST_F(WifiP2pServiceTest, StopP2pListen, TestSize.Level1)
{
    EXPECT_EQ(pWifiP2pService->StopP2pListen(), ErrCode::WIFI_OPT_SUCCESS);
}

HWTEST_F(WifiP2pServiceTest, CreateGroup, TestSize.Level1)
{
    WifiP2pConfig config;
    EXPECT_EQ(pWifiP2pService->CreateGroup(config), ErrCode::WIFI_OPT_SUCCESS);
}

HWTEST_F(WifiP2pServiceTest, RemoveGroup, TestSize.Level1)
{
    EXPECT_EQ(pWifiP2pService->RemoveGroup(), ErrCode::WIFI_OPT_SUCCESS);
}

HWTEST_F(WifiP2pServiceTest, DeleteGroup, TestSize.Level1)
{
    WifiP2pGroupInfo group;
    EXPECT_EQ(pWifiP2pService->DeleteGroup(group), ErrCode::WIFI_OPT_SUCCESS);
}

HWTEST_F(WifiP2pServiceTest, P2pConnect, TestSize.Level1)
{
    WifiP2pConfig config;
    EXPECT_EQ(pWifiP2pService->P2pConnect(config), ErrCode::WIFI_OPT_SUCCESS);
    EXPECT_EQ(pWifiP2pService->P2pCancelConnect(), ErrCode::WIFI_OPT_SUCCESS);
}

HWTEST_F(WifiP2pServiceTest, QueryP2pLinkedInfo, TestSize.Level1)
{
    WifiP2pLinkedInfo linkedInfo;
    EXPECT_EQ(pWifiP2pService->QueryP2pLinkedInfo(linkedInfo), ErrCode::WIFI_OPT_SUCCESS);
}

HWTEST_F(WifiP2pServiceTest, GetCurrentGroup, TestSize.Level1)
{
    WifiP2pLinkedInfo p2pInfo;
    p2pInfo.SetConnectState(P2pConnectedState::P2P_DISCONNECTED);
    WifiSettings::GetInstance().SaveP2pInfo(p2pInfo);
    WifiP2pGroupInfo group;
    EXPECT_EQ(pWifiP2pService->GetCurrentGroup(group), ErrCode::WIFI_OPT_FAILED);
}

HWTEST_F(WifiP2pServiceTest, GetP2pEnableStatus, TestSize.Level1)
{
    int status;
    EXPECT_EQ(pWifiP2pService->GetP2pEnableStatus(status), ErrCode::WIFI_OPT_SUCCESS);
}

HWTEST_F(WifiP2pServiceTest, GetP2pDiscoverStatus, TestSize.Level1)
{
    int status;
    EXPECT_EQ(pWifiP2pService->GetP2pDiscoverStatus(status), ErrCode::WIFI_OPT_SUCCESS);
}

HWTEST_F(WifiP2pServiceTest, GetP2pConnectedStatus, TestSize.Level1)
{
    int status;
    EXPECT_EQ(pWifiP2pService->GetP2pConnectedStatus(status), ErrCode::WIFI_OPT_SUCCESS);
}

HWTEST_F(WifiP2pServiceTest, QueryP2pDevices, TestSize.Level1)
{
    std::vector<WifiP2pDevice> devices;
    EXPECT_EQ(pWifiP2pService->QueryP2pDevices(devices), ErrCode::WIFI_OPT_SUCCESS);
}

HWTEST_F(WifiP2pServiceTest, QueryP2pGroups, TestSize.Level1)
{
    std::vector<WifiP2pGroupInfo> groups;
    EXPECT_EQ(pWifiP2pService->QueryP2pGroups(groups), ErrCode::WIFI_OPT_SUCCESS);
}

HWTEST_F(WifiP2pServiceTest, QueryP2pServices, TestSize.Level1)
{
    std::vector<WifiP2pServiceInfo> services;
    EXPECT_EQ(pWifiP2pService->QueryP2pServices(services), ErrCode::WIFI_OPT_SUCCESS);
}

HWTEST_F(WifiP2pServiceTest, SetP2pDeviceName, TestSize.Level1)
{
    std::string deviceName("TestName");
    EXPECT_EQ(pWifiP2pService->SetP2pDeviceName(deviceName), ErrCode::WIFI_OPT_SUCCESS);
}

HWTEST_F(WifiP2pServiceTest, SetP2pWfdInfo, TestSize.Level1)
{
    WifiP2pWfdInfo wfd;
    EXPECT_EQ(pWifiP2pService->SetP2pWfdInfo(wfd), ErrCode::WIFI_OPT_SUCCESS);
}
}  // namespace Wifi
}  // namespace OHOS