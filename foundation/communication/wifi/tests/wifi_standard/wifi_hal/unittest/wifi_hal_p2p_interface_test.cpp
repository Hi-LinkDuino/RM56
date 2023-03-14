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

#include "wifi_hal_p2p_interface_test.h"
#include "securec.h"
#include "wifi_log.h"
#include "wifi_hal_p2p_interface.h"
#include "wifi_p2p_hal.h"
#include "mock_wpa_ctrl.h"
#include "wifi_hal_common_func.h"

using namespace testing::ext;

namespace OHOS {
namespace Wifi {
static char P2pTestInterface[32] = "wlan0";

void WifiHalP2pInterfaceTest::SetUpTestCase()
{
    MockInitGlobalCmd();
    MockInitP2pSupportedCmd();
}

HWTEST_F(WifiHalP2pInterfaceTest, P2pStartTest, TestSize.Level1)
{
    EXPECT_TRUE(P2pStart() == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalP2pInterfaceTest, P2pSetRandomMacTest, TestSize.Level1)
{
    MockSetWpaExpectCmdResponse("DRIVER_FLAGS", "DEDICATED_P2P_DEVICE\n");
    EXPECT_TRUE(P2pSetRandomMac(0) == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalP2pInterfaceTest, P2pSetDeviceNameTest, TestSize.Level1)
{
    EXPECT_TRUE(P2pSetDeviceName(NULL) == WIFI_HAL_FAILED);
    EXPECT_TRUE(P2pSetDeviceName("p2p-device") == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalP2pInterfaceTest, P2pSetSsidPostfixNameTest, TestSize.Level1)
{
    EXPECT_TRUE(P2pSetSsidPostfixName(NULL) == WIFI_HAL_FAILED);
    EXPECT_TRUE(P2pSetSsidPostfixName("p2p-postfix") == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalP2pInterfaceTest, P2pSetWpsDeviceTypeTest, TestSize.Level1)
{
    EXPECT_TRUE(P2pSetWpsDeviceType(NULL) == WIFI_HAL_FAILED);
    EXPECT_TRUE(P2pSetWpsDeviceType("6-0050F204-1") == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalP2pInterfaceTest, P2pSetWpsConfigMethodsTest, TestSize.Level1)
{
    EXPECT_TRUE(P2pSetWpsConfigMethods(NULL) == WIFI_HAL_FAILED);
    EXPECT_TRUE(P2pSetWpsConfigMethods("label display push_button keypad") == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalP2pInterfaceTest, P2pGetDeviceAddressTest, TestSize.Level1)
{
    EXPECT_TRUE(P2pGetDeviceAddress(nullptr, 0) == WIFI_HAL_FAILED);
    MockSetWpaExpectCmdResponse("STATUS", "p2p_device_address=00:00:00:00:00:00\n");
    char deviceAddress[128] = {0};
    EXPECT_TRUE(P2pGetDeviceAddress(deviceAddress, 128) == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalP2pInterfaceTest, P2pFlushTest, TestSize.Level1)
{
    MockEraseSupportedCmd("P2P_FLUSH");
    EXPECT_TRUE(P2pFlush() == WIFI_HAL_FAILED);
    MockSetWpaExpectCmdResponse("P2P_FLUSH", "OK");
    EXPECT_TRUE(P2pFlush() == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalP2pInterfaceTest, P2pFlushServiceTest, TestSize.Level1)
{
    EXPECT_TRUE(P2pFlushService() == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalP2pInterfaceTest, P2pSaveConfigTest, TestSize.Level1)
{
    EXPECT_TRUE(P2pSaveConfig() == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalP2pInterfaceTest, P2pSetupWpsPbcTest, TestSize.Level1)
{
    EXPECT_TRUE(P2pSetupWpsPbc(nullptr, nullptr) == WIFI_HAL_FAILED);
    char bssid[32] = {0};
    EXPECT_TRUE(P2pSetupWpsPbc(P2pTestInterface, bssid) == WIFI_HAL_SUCCESS);
    StrSafeCopy(bssid, sizeof(bssid), "dc:f0:9e:9b:06:c5");
    EXPECT_TRUE(P2pSetupWpsPbc(P2pTestInterface, bssid) == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalP2pInterfaceTest, P2pSetupWpsPinTest, TestSize.Level1)
{
    EXPECT_TRUE(P2pSetupWpsPin(nullptr, nullptr, nullptr, nullptr, 0) == WIFI_HAL_FAILED);
    char bssid[32] = {0};
    char pin[32] = {0};
    char result[32] = {0};
    EXPECT_TRUE(P2pSetupWpsPin(P2pTestInterface, bssid, pin, result, sizeof(result)) == WIFI_HAL_SUCCESS);
    StrSafeCopy(pin, sizeof(pin), "12345678");
    EXPECT_TRUE(P2pSetupWpsPin(P2pTestInterface, bssid, pin, result, sizeof(result)) == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalP2pInterfaceTest, P2pRemoveNetworkTest, TestSize.Level1)
{
    EXPECT_TRUE(P2pRemoveNetwork(0) == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalP2pInterfaceTest, P2pListNetworksTest, TestSize.Level1)
{
    EXPECT_TRUE(P2pListNetworks(nullptr) == WIFI_HAL_FAILED);
    char replyBuff[] = "network id / ssid / bssid / flags\n"
                       "0\tDIRECT-OHOS\t00:00:00:00:00:00\t[DISABLED][P2P-PERSISTENT]\n";
    MockSetWpaExpectCmdResponse("LIST_NETWORKS", replyBuff);
    P2pNetworkList info;
    ASSERT_TRUE(memset_s(&info, sizeof(info), 0, sizeof(info)) == EOK);
    EXPECT_TRUE(P2pListNetworks(&info) == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalP2pInterfaceTest, P2PSetGroupIdleTest, TestSize.Level1)
{
    int time = 10;
    EXPECT_TRUE(P2pSetGroupMaxIdle(NULL, time) == WIFI_HAL_FAILED);
    EXPECT_TRUE(P2pSetGroupMaxIdle(P2pTestInterface, time) == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalP2pInterfaceTest, P2pSetPowerSaveTest, TestSize.Level1)
{
    int enable = 0;
    EXPECT_TRUE(P2pSetPowerSave(NULL, enable) == WIFI_HAL_FAILED);
    EXPECT_TRUE(P2pSetPowerSave(P2pTestInterface, enable) == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalP2pInterfaceTest, P2pSetWfdEnableTest, TestSize.Level1)
{
    EXPECT_TRUE(P2pSetWfdEnable(0) == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalP2pInterfaceTest, P2pSetWfdDeviceConfigTest, TestSize.Level1)
{
    EXPECT_TRUE(P2pSetWfdDeviceConfig(nullptr) == WIFI_HAL_FAILED);
    EXPECT_TRUE(P2pSetWfdDeviceConfig("input what?") == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalP2pInterfaceTest, P2pStartFindTest, TestSize.Level1)
{
    EXPECT_TRUE(P2pStartFind(0) == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalP2pInterfaceTest, P2pStopP2pFindTest, TestSize.Level1)
{
    EXPECT_TRUE(P2pStopFind() == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalP2pInterfaceTest, P2pSetExtListenTest, TestSize.Level1)
{
    EXPECT_TRUE(P2pSetExtListen(1, 500, 500) == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalP2pInterfaceTest, P2pSetListenChannelTest, TestSize.Level1)
{
    EXPECT_TRUE(P2pSetListenChannel(1, 500) == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalP2pInterfaceTest, P2pConnectTest, TestSize.Level1)
{
    EXPECT_TRUE(P2pConnect(NULL) == WIFI_HAL_FAILED);
    P2pConnectInfo info;
    ASSERT_TRUE(memset_s(&info, sizeof(info), 0, sizeof(info)) == EOK);
    EXPECT_TRUE(P2pConnect(&info) == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalP2pInterfaceTest, P2pCancelConnectTest, TestSize.Level1)
{
    EXPECT_TRUE(P2pCancelConnect() == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalP2pInterfaceTest, P2pProvisionDiscoveryTest, TestSize.Level1)
{
    EXPECT_TRUE(P2pProvisionDiscovery(nullptr, 0) == WIFI_HAL_FAILED);
    EXPECT_TRUE(P2pProvisionDiscovery("00:00:00:00:00:00", (int)HAL_WPS_METHOD_PBC) == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalP2pInterfaceTest, P2pAddGroupTest, TestSize.Level1)
{
    EXPECT_TRUE(P2pAddGroup(1, -1, 0) == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalP2pInterfaceTest, P2pRemoveGroupTest, TestSize.Level1)
{
    EXPECT_TRUE(P2pRemoveGroup(nullptr) == WIFI_HAL_FAILED);
    const char *name = "p2p-wlan0-0";
    EXPECT_TRUE(P2pRemoveGroup(name) == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalP2pInterfaceTest, P2pInviteTest, TestSize.Level1)
{
    EXPECT_TRUE(P2pInvite(0, nullptr, nullptr, nullptr) == WIFI_HAL_FAILED);
    EXPECT_TRUE(P2pInvite(1, "00:00:00:00:00:00", "00:00:00:00:00:00", "p2p-dev-wlan0") == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalP2pInterfaceTest, P2pReinvokeTest, TestSize.Level1)
{
    EXPECT_TRUE(P2pReinvoke(0, nullptr) == WIFI_HAL_FAILED);
    EXPECT_TRUE(P2pReinvoke(0, "00:00:00:00:00:00") == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalP2pInterfaceTest, P2pGetGroupCapabilityTest, TestSize.Level1)
{
    EXPECT_TRUE(P2pGetGroupCapability(nullptr, nullptr) == WIFI_HAL_FAILED);
    int capability = 0;
    EXPECT_TRUE(P2pGetGroupCapability("00:00:00:00:00:00", &capability) == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalP2pInterfaceTest, P2pAddServiceTest, TestSize.Level1)
{
    EXPECT_TRUE(P2pAddService(nullptr) == WIFI_HAL_FAILED);
    P2pServiceInfo info;
    ASSERT_TRUE(memset_s(&info, sizeof(info), 0, sizeof(info)) == EOK);
    EXPECT_TRUE(P2pAddService(&info) == WIFI_HAL_INVALID_PARAM);
    info.mode = 0;
    StrSafeCopy(info.name, sizeof(info.name), "test_name");
    EXPECT_TRUE(P2pAddService(&info) == WIFI_HAL_SUCCESS);
    info.mode = 1;
    StrSafeCopy(info.query, sizeof(info.query), "query");
    StrSafeCopy(info.resp, sizeof(info.resp), "response");
    EXPECT_TRUE(P2pAddService(&info) == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalP2pInterfaceTest, P2pRemoveServiceTest, TestSize.Level1)
{
    EXPECT_TRUE(P2pRemoveService(nullptr) == WIFI_HAL_FAILED);
    P2pServiceInfo info;
    ASSERT_TRUE(memset_s(&info, sizeof(info), 0, sizeof(info)) == EOK);
    EXPECT_TRUE(P2pRemoveService(&info) == WIFI_HAL_INVALID_PARAM);
    info.mode = 0;
    StrSafeCopy(info.name, sizeof(info.name), "test_name");
    EXPECT_TRUE(P2pRemoveService(&info) == WIFI_HAL_SUCCESS);
    info.mode = 1;
    StrSafeCopy(info.query, sizeof(info.query), "query");
    StrSafeCopy(info.resp, sizeof(info.resp), "response");
    EXPECT_TRUE(P2pRemoveService(&info) == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalP2pInterfaceTest, P2pReqServiceDiscoveryTest, TestSize.Level1)
{
    EXPECT_TRUE(P2pReqServiceDiscovery(nullptr, nullptr, nullptr, 0) == WIFI_HAL_FAILED);
    char disc[1024] = {0};
    EXPECT_TRUE(P2pReqServiceDiscovery("00:00:00:00:00:00", "HELLO,MESSAGE", disc, 1024) == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalP2pInterfaceTest, P2pCancelServiceDiscoveryTest, TestSize.Level1)
{
    EXPECT_TRUE(P2pCancelServiceDiscovery(nullptr) == WIFI_HAL_FAILED);
    EXPECT_TRUE(P2pCancelServiceDiscovery("string id") == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalP2pInterfaceTest, P2pSetMiracastTypeTest, TestSize.Level1)
{
    EXPECT_TRUE(P2pSetMiracastType(0) == WIFI_HAL_NOT_SUPPORT);
}

HWTEST_F(WifiHalP2pInterfaceTest, P2pRespServerDiscoveryTest, TestSize.Level1)
{
    EXPECT_TRUE(P2pRespServerDiscovery(nullptr) == WIFI_HAL_FAILED);
    P2pServDiscReqInfo info;
    ASSERT_TRUE(memset_s(&info, sizeof(info), 0, sizeof(info)) == EOK);
    info.tlvs = (char *)calloc(32, sizeof(char));
    EXPECT_TRUE(P2pRespServerDiscovery(&info) == WIFI_HAL_SUCCESS);
    free(info.tlvs);
}

HWTEST_F(WifiHalP2pInterfaceTest, P2pSetServDiscExternalTest, TestSize.Level1)
{
    EXPECT_TRUE(P2pSetServDiscExternal(0) == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalP2pInterfaceTest, P2pSetPersistentReconnectTest, TestSize.Level1)
{
    EXPECT_TRUE(P2pSetPersistentReconnect(0) == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalP2pInterfaceTest, P2pStopTest, TestSize.Level1)
{
    EXPECT_TRUE(P2pStop() == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalP2pInterfaceTest, P2pForceStopTest, TestSize.Level1)
{
    EXPECT_TRUE(P2pForceStop() == WIFI_HAL_SUCCESS);
}
}  // namespace Wifi
}  // namespace OHOS