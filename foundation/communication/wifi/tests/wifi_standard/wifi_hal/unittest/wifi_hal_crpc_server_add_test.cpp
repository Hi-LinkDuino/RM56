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
#include "wifi_hal_crpc_server_add_test.h"
#include "wifi_log.h"
#include "wifi_hal_crpc_server.h"
#include "wifi_hal_crpc_p2p.h"
#include "mock_wpa_ctrl.h"

using namespace testing::ext;

namespace OHOS {
namespace Wifi {
static std::string g_rpcSockPath = "./unix_sock_test.sock";
RpcServer *WifiHalCRpcServerAddTest::mServer = nullptr;
Context *WifiHalCRpcServerAddTest::mContext = nullptr;

void WifiHalCRpcServerAddTest::SetUpTestCase()
{
    if (access(g_rpcSockPath.c_str(), 0) == 0) {
        unlink(g_rpcSockPath.c_str());
    }
    mServer = CreateRpcServer(g_rpcSockPath.c_str());
    mContext = CreateContext(CONTEXT_BUFFER_MIN_SIZE);
    if (mServer == nullptr || mContext == nullptr) {
        printf("Init rpc server failed or create context failed!");
        exit(-1);
    }
    InitCallbackMsg();
    SetRpcServerInited(mServer);
    MockInitGlobalCmd();
    MockInitP2pSupportedCmd();
}

void WifiHalCRpcServerAddTest::TearDownTestCase()
{
    if (mServer != nullptr) {
        ReleaseRpcServer(mServer);
        mServer = nullptr;
    }
    SetRpcServerInited(NULL);
    ReleaseCallbackMsg();
    if (mContext != nullptr) {
        ReleaseContext(mContext);
        mContext = nullptr;
    }
}

void WifiHalCRpcServerAddTest::SetUp()
{
    if (mContext != nullptr) {
        mContext->wBegin = mContext->wEnd = 0;
    }
}

void WifiHalCRpcServerAddTest::TearDown()
{
    if (mContext != nullptr) {
        mContext->wBegin = mContext->wEnd = 0;
    }
}

HWTEST_F(WifiHalCRpcServerAddTest, RpcP2pStartTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcP2pStart(nullptr, nullptr) < 0);
    EXPECT_TRUE(RpcP2pStart(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerAddTest, RpcP2pStopTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcP2pStop(nullptr, nullptr) < 0);
    EXPECT_TRUE(RpcP2pStop(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerAddTest, RpcP2pSetRandomMacTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcP2pSetRandomMac(nullptr, nullptr) < 0);
    char buff[] = "N|P2pSetRandomMac|1";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|P2pSetRandomMac|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcP2pSetRandomMac(mServer, mContext) < 0);
    char buff1[] = "N|P2pSetRandomMac|1|";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|P2pSetRandomMac|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcP2pSetRandomMac(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerAddTest, RpcP2pSetDeviceNameTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcP2pSetDeviceName(nullptr, nullptr) < 0);
    char buff[] = "N|P2pSetDeviceName|p2p_device_name";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|P2pSetDeviceName|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcP2pSetDeviceName(mServer, mContext) < 0);
    char buff1[] = "N|P2pSetDeviceName|p2p_device_name|";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|P2pSetDeviceName|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcP2pSetDeviceName(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerAddTest, RpcP2pSetSsidPostfixNameTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcP2pSetSsidPostfixName(nullptr, nullptr) < 0);
    char buff[] = "N|P2pSetSsidPostfixName|p2p_postfix_name";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|P2pSetSsidPostfixName|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcP2pSetSsidPostfixName(mServer, mContext) < 0);
    char buff1[] = "N|P2pSetSsidPostfixName|p2p_postfix_name|";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|P2pSetSsidPostfixName|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcP2pSetSsidPostfixName(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerAddTest, RpcP2pSetWpsDeviceTypeTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcP2pSetWpsDeviceType(nullptr, nullptr) < 0);
    char buff[] = "N|P2pSetWpsDeviceType|p2p_device_type";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|P2pSetWpsDeviceType|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcP2pSetWpsDeviceType(mServer, mContext) < 0);
    char buff1[] = "N|P2pSetWpsDeviceType|p2p_device_type|";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|P2pSetWpsDeviceType|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcP2pSetWpsDeviceType(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerAddTest, RpcP2pSetWpsConfigMethodsTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcP2pSetWpsConfigMethods(nullptr, nullptr) < 0);
    char buff[] = "N|P2pSetWpsConfigMethods|p2p_config_methods";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|P2pSetWpsConfigMethods|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcP2pSetWpsConfigMethods(mServer, mContext) < 0);
    char buff1[] = "N|P2pSetWpsConfigMethods|p2p_config_methods|";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|P2pSetWpsConfigMethods|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcP2pSetWpsConfigMethods(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerAddTest, RpcP2pGetDeviceAddressTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcP2pGetDeviceAddress(nullptr, nullptr) < 0);
    char buff[] = "N|P2pGetDeviceAddress|17";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|P2pGetDeviceAddress|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcP2pGetDeviceAddress(mServer, mContext) < 0);
    char buff1[] = "N|P2pGetDeviceAddress|17|";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|P2pGetDeviceAddress|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcP2pGetDeviceAddress(mServer, mContext) == 0);
    char buff2[] = "N|P2pGetDeviceAddress|-1|";
    mContext->oneProcess = buff2;
    mContext->nPos = strlen("N|P2pGetDeviceAddress|");
    mContext->nSize = strlen(buff2);
    EXPECT_TRUE(RpcP2pGetDeviceAddress(mServer, mContext) < 0);
}

HWTEST_F(WifiHalCRpcServerAddTest, RpcP2pFlushTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcP2pFlush(nullptr, nullptr) < 0);
    EXPECT_TRUE(RpcP2pFlush(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerAddTest, RpcP2pFlushServiceTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcP2pFlushService(nullptr, nullptr) < 0);
    EXPECT_TRUE(RpcP2pFlushService(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerAddTest, RpcP2pSaveConfigTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcP2pSaveConfig(nullptr, nullptr) < 0);
    EXPECT_TRUE(RpcP2pSaveConfig(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerAddTest, RpcP2pSetupWpsPbcTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcP2pSetupWpsPbc(nullptr, nullptr) < 0);
    char buff[] = "N|P2pSetupWpsPbc|p2p-dev-wlan0|00:00:00:00:00:00";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|P2pSetupWpsPbc|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcP2pSetupWpsPbc(mServer, mContext) < 0);
    char buff1[] = "N|P2pSetupWpsPbc|p2p-dev-wlan0|00:00:00:00:00:00|";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|P2pSetupWpsPbc|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcP2pSetupWpsPbc(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerAddTest, RpcP2pSetupWpsPinTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcP2pSetupWpsPin(nullptr, nullptr) < 0);
    char buff[] = "N|P2pSetupWpsPin|p2p-dev-wlan0|00:00:00:00:00:00|123456789|8|";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|P2pSetupWpsPin|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcP2pSetupWpsPin(mServer, mContext) < 0);
    char buff1[] = "N|P2pSetupWpsPin|p2p-dev-wlan0|00:00:00:00:00:00|12345678|8|";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|P2pSetupWpsPin|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcP2pSetupWpsPin(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerAddTest, RpcP2pRemoveNetworkTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcP2pRemoveNetwork(nullptr, nullptr) < 0);
    char buff[] = "N|P2pRemoveNetwork|1";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|P2pRemoveNetwork|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcP2pRemoveNetwork(mServer, mContext) < 0);
    char buff1[] = "N|P2pRemoveNetwork|1|";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|P2pRemoveNetwork|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcP2pRemoveNetwork(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerAddTest, RpcP2pListNetworksTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcP2pListNetworks(nullptr, nullptr) < 0);
    EXPECT_TRUE(RpcP2pListNetworks(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerAddTest, RpcP2pSetGroupMaxIdleTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcP2pSetGroupMaxIdle(nullptr, nullptr) < 0);
    char buff[] = "N|P2pSetGroupMaxIdle|p2p-dev-wlan0|1";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|P2pSetGroupMaxIdle|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcP2pSetGroupMaxIdle(mServer, mContext) < 0);
    char buff1[] = "N|P2pSetGroupMaxIdle|p2p-dev-wlan0|1|";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|P2pSetGroupMaxIdle|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcP2pSetGroupMaxIdle(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerAddTest, RpcP2pSetPowerSaveTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcP2pSetPowerSave(nullptr, nullptr) < 0);
    char buff[] = "N|P2pSetPowerSave|p2p-dev-wlan0|1";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|P2pSetPowerSave|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcP2pSetPowerSave(mServer, mContext) < 0);
    char buff1[] = "N|P2pSetPowerSave|p2p-dev-wlan0|1|";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|P2pSetPowerSave|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcP2pSetPowerSave(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerAddTest, RpcP2pSetWfdEnableTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcP2pSetWfdEnable(nullptr, nullptr) < 0);
    char buff[] = "N|P2pSetWfdEnable|1";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|P2pSetWfdEnable|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcP2pSetWfdEnable(mServer, mContext) < 0);
    char buff1[] = "N|P2pSetWfdEnable|1|";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|P2pSetWfdEnable|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcP2pSetWfdEnable(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerAddTest, RpcP2pSetWfdDeviceConfigTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcP2pSetWfdDeviceConfig(nullptr, nullptr) < 0);
    char buff[] = "N|P2pSetWfdDeviceConfig|p2p_device_config";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|P2pSetWfdDeviceConfig|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcP2pSetWfdDeviceConfig(mServer, mContext) < 0);
    char buff1[] = "N|P2pSetWfdDeviceConfig|p2p_device_config|";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|P2pSetWfdDeviceConfig|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcP2pSetWfdDeviceConfig(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerAddTest, RpcP2pStartFindTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcP2pStartFind(nullptr, nullptr) < 0);
    char buff[] = "N|P2pStartFind|120";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|P2pStartFind|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcP2pStartFind(mServer, mContext) < 0);
    char buff1[] = "N|P2pStartFind|120|";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|P2pStartFind|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcP2pStartFind(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerAddTest, RpcP2pStopFindTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcP2pStopFind(nullptr, nullptr) < 0);
    EXPECT_TRUE(RpcP2pStopFind(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerAddTest, RpcP2pSetExtListenTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcP2pSetExtListen(nullptr, nullptr) < 0);
    char buff[] = "N|P2pSetExtListen|0|0|0";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|P2pSetExtListen|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcP2pSetExtListen(mServer, mContext) < 0);
    char buff1[] = "N|P2pSetExtListen|0|0|0|";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|P2pSetExtListen|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcP2pSetExtListen(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerAddTest, RpcP2pSetListenChannelTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcP2pSetListenChannel(nullptr, nullptr) < 0);
    char buff[] = "N|P2pSetListenChannel|0|0";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|P2pSetListenChannel|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcP2pSetListenChannel(mServer, mContext) < 0);
    char buff1[] = "N|P2pSetListenChannel|0|0|";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|P2pSetListenChannel|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcP2pSetListenChannel(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerAddTest, RpcP2pConnectTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcP2pConnect(nullptr, nullptr) < 0);
    char buff[] = "N|P2pConnect|0|0|0|0|00:00:00:00:00:00|12345678";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|P2pConnect|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcP2pConnect(mServer, mContext) < 0);
    char buff1[] = "N|P2pConnect|0|0|0|0|00:00:00:00:00:00|12345678|";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|P2pConnect|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcP2pConnect(mServer, mContext) == 0);
    char buff2[] = "N|P2pConnect|0|1|0|0|00:00:00:00:00:00|pin|";
    mContext->oneProcess = buff2;
    mContext->nPos = strlen("N|P2pConnect|");
    mContext->nSize = strlen(buff2);
    EXPECT_TRUE(RpcP2pConnect(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerAddTest, RpcP2pCancelConnectTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcP2pCancelConnect(nullptr, nullptr) < 0);
    EXPECT_TRUE(RpcP2pCancelConnect(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerAddTest, RpcP2pProvisionDiscoveryTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcP2pProvisionDiscovery(nullptr, nullptr) < 0);
    char buff[] = "N|P2pProvisionDiscovery|00:00:00:00:00:00|1";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|P2pProvisionDiscovery|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcP2pProvisionDiscovery(mServer, mContext) < 0);
    char buff1[] = "N|P2pProvisionDiscovery|00:00:00:00:00:00|1|";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|P2pProvisionDiscovery|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcP2pProvisionDiscovery(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerAddTest, RpcP2pAddGroupTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcP2pAddGroup(nullptr, nullptr) < 0);
    char buff[] = "N|P2pAddGroup|0|1|0";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|P2pAddGroup|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcP2pAddGroup(mServer, mContext) < 0);
    char buff1[] = "N|P2pAddGroup|0|1|0|";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|P2pAddGroup|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcP2pAddGroup(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerAddTest, RpcP2pRemoveGroupTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcP2pRemoveGroup(nullptr, nullptr) < 0);
    char buff[] = "N|P2pRemoveGroup|p2p-dev-wlan0";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|P2pRemoveGroup|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcP2pRemoveGroup(mServer, mContext) < 0);
    char buff1[] = "N|P2pRemoveGroup|p2p-dev-wlan0|";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|P2pRemoveGroup|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcP2pRemoveGroup(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerAddTest, RpcP2pInviteTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcP2pInvite(nullptr, nullptr) < 0);
    char buff[] = "N|P2pInvite|0|||p2p-dev-wlan0";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|P2pInvite|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcP2pInvite(mServer, mContext) < 0);
    char buff1[] = "N|P2pInvite|0|||p2p-dev-wlan0|";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|P2pInvite|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcP2pInvite(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerAddTest, RpcP2pReinvokeTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcP2pReinvoke(nullptr, nullptr) < 0);
    char buff[] = "N|P2pReinvoke|0|00:00:00:00:00:00";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|P2pReinvoke|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcP2pReinvoke(mServer, mContext) < 0);
    char buff1[] = "N|P2pReinvoke|0|00:00:00:00:00:00|";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|P2pReinvoke|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcP2pReinvoke(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerAddTest, RpcP2pGetGroupCapabilityTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcP2pGetGroupCapability(nullptr, nullptr) < 0);
    char buff[] = "N|P2pGetGroupCapability|00:00:00:00:00:00";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|P2pGetGroupCapability|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcP2pGetGroupCapability(mServer, mContext) < 0);
    char buff1[] = "N|P2pGetGroupCapability|00:00:00:00:00:00|";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|P2pGetGroupCapability|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcP2pGetGroupCapability(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerAddTest, RpcP2pAddServiceTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcP2pAddService(nullptr, nullptr) < 0);
    char buff[] = "N|P2pAddService|x|0|service_name";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|P2pAddService|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcP2pAddService(mServer, mContext) < 0);
    char buff1[] = "N|P2pAddService|0|0|service_name";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|P2pAddService|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcP2pAddService(mServer, mContext) < 0);
    char buff2[] = "N|P2pAddService|0|0|service_name|";
    mContext->oneProcess = buff2;
    mContext->nPos = strlen("N|P2pAddService|");
    mContext->nSize = strlen(buff2);
    EXPECT_TRUE(RpcP2pAddService(mServer, mContext) == 0);
    char buff3[] = "N|P2pAddService|1|query_message|resp_message";
    mContext->oneProcess = buff3;
    mContext->nPos = strlen("N|P2pAddService|");
    mContext->nSize = strlen(buff3);
    EXPECT_TRUE(RpcP2pAddService(mServer, mContext) < 0);
    char buff4[] = "N|P2pAddService|1|query_message|resp_message|";
    mContext->oneProcess = buff4;
    mContext->nPos = strlen("N|P2pAddService|");
    mContext->nSize = strlen(buff4);
    EXPECT_TRUE(RpcP2pAddService(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerAddTest, RpcP2pRemoveServiceTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcP2pRemoveService(nullptr, nullptr) < 0);
    char buff[] = "N|P2pRemoveService|x|0|service_name";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|P2pRemoveService|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcP2pRemoveService(mServer, mContext) < 0);
    char buff1[] = "N|P2pRemoveService|0|0|service_name";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|P2pRemoveService|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcP2pRemoveService(mServer, mContext) < 0);
    char buff2[] = "N|P2pRemoveService|0|0|service_name|";
    mContext->oneProcess = buff2;
    mContext->nPos = strlen("N|P2pRemoveService|");
    mContext->nSize = strlen(buff2);
    EXPECT_TRUE(RpcP2pRemoveService(mServer, mContext) == 0);
    char buff3[] = "N|P2pRemoveService|1|query_message";
    mContext->oneProcess = buff3;
    mContext->nPos = strlen("N|P2pRemoveService|");
    mContext->nSize = strlen(buff3);
    EXPECT_TRUE(RpcP2pRemoveService(mServer, mContext) < 0);
    char buff4[] = "N|P2pRemoveService|1|query_message|";
    mContext->oneProcess = buff4;
    mContext->nPos = strlen("N|P2pRemoveService|");
    mContext->nSize = strlen(buff4);
    EXPECT_TRUE(RpcP2pRemoveService(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerAddTest, RpcP2pReqServiceDiscoveryTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcP2pReqServiceDiscovery(nullptr, nullptr) < 0);
    char buff[] = "N|P2pReqServiceDiscovery|00:00:00:00:00:00|discover message";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|P2pReqServiceDiscovery|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcP2pReqServiceDiscovery(mServer, mContext) < 0);
    char buff1[] = "N|P2pReqServiceDiscovery|00:00:00:00:00:00|discover message|32|";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|P2pReqServiceDiscovery|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcP2pReqServiceDiscovery(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerAddTest, RpcP2pCancelServiceDiscoveryTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcP2pCancelServiceDiscovery(nullptr, nullptr) < 0);
    char buff[] = "N|P2pCancelServiceDiscovery|discover message";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|P2pCancelServiceDiscovery|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcP2pCancelServiceDiscovery(mServer, mContext) < 0);
    char buff1[] = "N|P2pCancelServiceDiscovery|discover message|";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|P2pCancelServiceDiscovery|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcP2pCancelServiceDiscovery(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerAddTest, RpcP2pSetMiracastTypeTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcP2pSetMiracastType(nullptr, nullptr) < 0);
    char buff[] = "N|P2pSetMiracastType|x|";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|P2pSetMiracastType|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcP2pSetMiracastType(mServer, mContext) < 0);
    char buff1[] = "N|P2pSetMiracastType|1|";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|P2pSetMiracastType|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcP2pSetMiracastType(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerAddTest, RpcP2pRespServerDiscoveryTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcP2pRespServerDiscovery(nullptr, nullptr) < 0);
    char buff[] = "N|P2pRespServerDiscovery|0|0|00:00:00:00:00:00|tlvs message";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|P2pRespServerDiscovery|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcP2pRespServerDiscovery(mServer, mContext) < 0);
    char buff1[] = "N|P2pRespServerDiscovery|0|0|00:00:00:00:00:00|tlvs message|";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|P2pRespServerDiscovery|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcP2pRespServerDiscovery(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerAddTest, RpcP2pSetServDiscExternalTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcP2pSetServDiscExternal(nullptr, nullptr) < 0);
    char buff[] = "N|P2pSetServDiscExternal|x|";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|P2pSetServDiscExternal|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcP2pSetServDiscExternal(mServer, mContext) < 0);
    char buff1[] = "N|P2pSetServDiscExternal|1|";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|P2pSetServDiscExternal|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcP2pSetServDiscExternal(mServer, mContext) == 0);
}

HWTEST_F(WifiHalCRpcServerAddTest, RpcP2pSetPersistentReconnectTest, TestSize.Level1)
{
    EXPECT_TRUE(RpcP2pSetPersistentReconnect(nullptr, nullptr) < 0);
    char buff[] = "N|P2pSetPersistentReconnect|x|";
    mContext->oneProcess = buff;
    mContext->nPos = strlen("N|P2pSetPersistentReconnect|");
    mContext->nSize = strlen(buff);
    EXPECT_TRUE(RpcP2pSetPersistentReconnect(mServer, mContext) < 0);
    char buff1[] = "N|P2pSetPersistentReconnect|1|";
    mContext->oneProcess = buff1;
    mContext->nPos = strlen("N|P2pSetPersistentReconnect|");
    mContext->nSize = strlen(buff1);
    EXPECT_TRUE(RpcP2pSetPersistentReconnect(mServer, mContext) == 0);
}
}  // namespace Wifi
}  // namespace OHOS