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
#include "wifi_hal_wpa_sta_test.h"
#include "securec.h"
#include "mock_wpa_ctrl.h"
#include "wifi_hal_crpc_server.h"
#include "wifi_hal_common_func.h"
#include "wifi_log.h"
#undef LOG_TAG
#define LOG_TAG "WifiHalWpaStaTest"

using namespace testing::ext;

namespace OHOS {
namespace Wifi {
const std::string g_rpcSockPath = "./unix_sock_test.sock";
WifiWpaInterface *WifiHalWpaStaTest::mGlobalInterface = nullptr;
WifiWpaStaInterface *WifiHalWpaStaTest::mInterface = nullptr;
RpcServer *WifiHalWpaStaTest::mServer = nullptr;

void WifiHalWpaStaTest::SetUpTestCase()
{
    if (access(g_rpcSockPath.c_str(), 0) == 0) {
        unlink(g_rpcSockPath.c_str());
    }
    mServer = CreateRpcServer(g_rpcSockPath.c_str());
    if (mServer == nullptr) {
        printf("Init rpc server failed!");
        exit(-1);
    }
    InitCallbackMsg();
    SetRpcServerInited(mServer);
    MockInitGlobalCmd();
    MockInitStaSupportedCmd();
    mGlobalInterface = GetWifiWapGlobalInterface();
    mGlobalInterface->wpaCliConnect(mGlobalInterface);
    mInterface = GetWifiStaInterface(0);
}

void WifiHalWpaStaTest::TearDownTestCase()
{
    sleep(1);
    ReleaseWpaGlobalInterface();
    ReleaseWifiStaInterface(0);
    if (mServer != nullptr) {
        ReleaseRpcServer(mServer);
        mServer = nullptr;
    }
    SetRpcServerInited(NULL);
    ReleaseCallbackMsg();
}

HWTEST_F(WifiHalWpaStaTest, WpaCliCmdStatusTest, TestSize.Level1)
{
    EXPECT_TRUE(mInterface->wpaCliCmdStatus(mInterface, nullptr) < 0);
    MockEraseSupportedCmd("STATUS");
    WpaHalCmdStatus status;
    ASSERT_TRUE(memset_s(&status, sizeof(status), 0, sizeof(status)) == EOK);
    EXPECT_TRUE(mInterface->wpaCliCmdStatus(mInterface, &status) < 0);
    char buf1[] = "wpa_state=SCANNING\n"
                  "ip_address=192.168.xxx.xxx\n"
                  "freq=1024\n"
                  "key_mgmt=WPA-PSK\n";
    MockSetWpaExpectCmdResponse("STATUS", buf1);
    EXPECT_TRUE(mInterface->wpaCliCmdStatus(mInterface, &status) < 0);
    EXPECT_TRUE(status.freq == 1024);
    char buf2[] = "wpa_state=SCANNING\n"
                  "ip_address=192.168.xxx.xxx\n"
                  "freq=1024\n"
                  "key_mgmt=WPA-PSK\n"
                  "address=00:00:00:00:00:00\n";
    MockSetWpaExpectCmdResponse("STATUS", buf2);
    EXPECT_TRUE(mInterface->wpaCliCmdStatus(mInterface, &status) == 1);
    EXPECT_TRUE(strcmp(status.address, "00:00:00:00:00:00") == 0);
    char buf3[] = "wpa_state=SCANNING\n"
                  "ip_address=192.168.xxx.xxx\n"
                  "freq=1024\n"
                  "key_mgmt=WPA-PSK\n"
                  "address=00:00:00:00:00:00\n"
                  "bssid=00:00:00:00:00:00\n"
                  "ssid=testAp\n"
                  "id=1\n";
    MockSetWpaExpectCmdResponse("STATUS", buf3);
    EXPECT_TRUE(mInterface->wpaCliCmdStatus(mInterface, &status) == 0);
    EXPECT_TRUE(strcmp(status.address, "00:00:00:00:00:00") == 0);
}

HWTEST_F(WifiHalWpaStaTest, WpaCliCmdAddNetworksTest, TestSize.Level1)
{
    MockEraseSupportedCmd("ADD_NETWORK");
    EXPECT_TRUE(mInterface->wpaCliCmdAddNetworks(mInterface) < 0);
    MockSetWpaExpectCmdResponse("ADD_NETWORK", "1");
    int ret = mInterface->wpaCliCmdAddNetworks(mInterface);
    EXPECT_TRUE(ret == 1);
}

HWTEST_F(WifiHalWpaStaTest, WpaCliCmdReconnectTest, TestSize.Level1)
{
    MockEraseSupportedCmd("RECONNECT");
    EXPECT_TRUE(mInterface->wpaCliCmdReconnect(mInterface) < 0);
    MockSetWpaExpectCmdResponse("RECONNECT", "FAIL\n");
    EXPECT_TRUE(mInterface->wpaCliCmdReconnect(mInterface) < 0);
    MockSetWpaExpectCmdResponse("RECONNECT", "OK");
    EXPECT_TRUE(mInterface->wpaCliCmdReconnect(mInterface) == 0);
    MockWpaCallback(mGlobalInterface->wpaCtrl.pSend,
        "<3>CTRL-EVENT-CONNECTED - Connection to 00:00:00:00:00:00 completed [id=1 id_str=]\n");
    MockWpaCallback(mGlobalInterface->wpaCtrl.pSend, "<3>CTRL-EVENT-STATE-CHANGE state=2\n");
}

HWTEST_F(WifiHalWpaStaTest, WpaCliCmdReassociateTest, TestSize.Level1)
{
    MockEraseSupportedCmd("REASSOCIATE");
    EXPECT_TRUE(mInterface->wpaCliCmdReassociate(mInterface) < 0);
    MockSetWpaExpectCmdResponse("REASSOCIATE", "FAIL\n");
    EXPECT_TRUE(mInterface->wpaCliCmdReassociate(mInterface) < 0);
    MockSetWpaExpectCmdResponse("REASSOCIATE", "OK");
    EXPECT_TRUE(mInterface->wpaCliCmdReassociate(mInterface) == 0);
    MockWpaCallback(mGlobalInterface->wpaCtrl.pSend, "<3>CTRL-EVENT-SSID-TEMP-DISABLED reason=WRONG_KEY\n");
}

HWTEST_F(WifiHalWpaStaTest, WpaCliCmdDisconnectTest, TestSize.Level1)
{
    MockEraseSupportedCmd("DISCONNECT");
    EXPECT_TRUE(mInterface->wpaCliCmdDisconnect(mInterface) < 0);
    MockSetWpaExpectCmdResponse("DISCONNECT", "FAIL\n");
    EXPECT_TRUE(mInterface->wpaCliCmdDisconnect(mInterface) < 0);
    MockSetWpaExpectCmdResponse("DISCONNECT", "OK");
    EXPECT_TRUE(mInterface->wpaCliCmdDisconnect(mInterface) == 0);
    MockWpaCallback(mGlobalInterface->wpaCtrl.pSend,
        "<3>CTRL-EVENT-DISCONNECTED bssid=00:00:00:00:00:00 reason=4 locally_generated=1\n");
    MockWpaCallback(mGlobalInterface->wpaCtrl.pSend, "<3>CTRL-EVENT-STATE-CHANGE state=3\n");
}

HWTEST_F(WifiHalWpaStaTest, WpaCliCmdSaveConfigTest, TestSize.Level1)
{
    MockEraseSupportedCmd("SAVE_CONFIG");
    EXPECT_TRUE(mInterface->wpaCliCmdSaveConfig(mInterface) < 0);
    MockSetWpaExpectCmdResponse("SAVE_CONFIG", "FAIL\n");
    EXPECT_TRUE(mInterface->wpaCliCmdSaveConfig(mInterface) < 0);
    MockSetWpaExpectCmdResponse("SAVE_CONFIG", "OK");
    EXPECT_TRUE(mInterface->wpaCliCmdSaveConfig(mInterface) == 0);
}

HWTEST_F(WifiHalWpaStaTest, WpaCliCmdSetNetworkTest, TestSize.Level1)
{
    EXPECT_TRUE(mInterface->wpaCliCmdSetNetwork(mInterface, nullptr) < 0);
    WpaSetNetworkArgv argv;
    ASSERT_TRUE(memset_s(&argv, sizeof(argv), 0, sizeof(argv)) == EOK);
    argv.id = 0;
    argv.param = DEVICE_CONFIG_END_POS;
    EXPECT_TRUE(mInterface->wpaCliCmdSetNetwork(mInterface, &argv) < 0);
    argv.param = DEVICE_CONFIG_SSID;
    EXPECT_TRUE(mInterface->wpaCliCmdSetNetwork(mInterface, &argv) == 0);
    MockEraseSupportedCmd("SET_NETWORK");
    EXPECT_TRUE(mInterface->wpaCliCmdSetNetwork(mInterface, &argv) < 0);
    MockSetWpaExpectCmdResponse("SET_NETWORK", "FAIL\n");
    EXPECT_TRUE(mInterface->wpaCliCmdSetNetwork(mInterface, &argv) < 0);
    MockSetWpaExpectCmdResponse("SET_NETWORK", "OK");
    EXPECT_TRUE(mInterface->wpaCliCmdSetNetwork(mInterface, &argv) == 0);
    argv.param = DEVICE_CONFIG_KEYMGMT;
    EXPECT_TRUE(mInterface->wpaCliCmdSetNetwork(mInterface, &argv) == 0);
}

HWTEST_F(WifiHalWpaStaTest, WpaCliCmdEnableNetworkTest, TestSize.Level1)
{
    MockEraseSupportedCmd("ENABLE_NETWORK");
    EXPECT_TRUE(mInterface->wpaCliCmdEnableNetwork(mInterface, 0) < 0);
    MockSetWpaExpectCmdResponse("ENABLE_NETWORK", "FAIL\n");
    EXPECT_TRUE(mInterface->wpaCliCmdEnableNetwork(mInterface, 0) < 0);
    MockSetWpaExpectCmdResponse("ENABLE_NETWORK", "OK");
    EXPECT_TRUE(mInterface->wpaCliCmdEnableNetwork(mInterface, 0) == 0);
}

HWTEST_F(WifiHalWpaStaTest, WpaCliCmdSelectNetworkTest, TestSize.Level1)
{
    MockEraseSupportedCmd("SELECT_NETWORK");
    EXPECT_TRUE(mInterface->wpaCliCmdSelectNetwork(mInterface, 0) < 0);
    MockSetWpaExpectCmdResponse("SELECT_NETWORK", "FAIL\n");
    EXPECT_TRUE(mInterface->wpaCliCmdSelectNetwork(mInterface, 0) < 0);
    MockSetWpaExpectCmdResponse("SELECT_NETWORK", "OK");
    EXPECT_TRUE(mInterface->wpaCliCmdSelectNetwork(mInterface, 0) == 0);
}

HWTEST_F(WifiHalWpaStaTest, WpaCliCmdDisableNetworkTest, TestSize.Level1)
{
    MockEraseSupportedCmd("DISABLE_NETWORK");
    EXPECT_TRUE(mInterface->wpaCliCmdDisableNetwork(mInterface, 0) < 0);
    MockSetWpaExpectCmdResponse("DISABLE_NETWORK", "FAIL\n");
    EXPECT_TRUE(mInterface->wpaCliCmdDisableNetwork(mInterface, 0) < 0);
    MockSetWpaExpectCmdResponse("DISABLE_NETWORK", "OK");
    EXPECT_TRUE(mInterface->wpaCliCmdDisableNetwork(mInterface, 0) == 0);
}

HWTEST_F(WifiHalWpaStaTest, WpaCliCmdRemoveNetworkTest, TestSize.Level1)
{
    MockEraseSupportedCmd("REMOVE_NETWORK");
    EXPECT_TRUE(mInterface->wpaCliCmdRemoveNetwork(mInterface, 0) < 0);
    MockSetWpaExpectCmdResponse("REMOVE_NETWORK", "FAIL\n");
    EXPECT_TRUE(mInterface->wpaCliCmdRemoveNetwork(mInterface, 0) < 0);
    MockSetWpaExpectCmdResponse("REMOVE_NETWORK", "OK");
    EXPECT_TRUE(mInterface->wpaCliCmdRemoveNetwork(mInterface, 0) == 0);
    EXPECT_TRUE(mInterface->wpaCliCmdRemoveNetwork(mInterface, -1) == 0);
    EXPECT_TRUE(mInterface->wpaCliCmdRemoveNetwork(mInterface, -2) < 0);
}

HWTEST_F(WifiHalWpaStaTest, WpaCliCmdGetNetworkTest, TestSize.Level1)
{
    EXPECT_TRUE(mInterface->wpaCliCmdGetNetwork(mInterface, nullptr, nullptr, 0) < 0);
    WpaGetNetworkArgv argv;
    ASSERT_TRUE(memset_s(&argv, sizeof(argv), 0, sizeof(argv)) == EOK);
    char getBuffer[256] = {0};
    MockEraseSupportedCmd("GET_NETWORK");
    EXPECT_TRUE(mInterface->wpaCliCmdGetNetwork(mInterface, &argv, getBuffer, sizeof(getBuffer)) < 0);
    MockSetWpaExpectCmdResponse("GET_NETWORK", "OK");
    EXPECT_TRUE(mInterface->wpaCliCmdGetNetwork(mInterface, &argv, getBuffer, sizeof(getBuffer)) == 0);
    EXPECT_TRUE(strcmp(getBuffer, "OK") == 0);
}

HWTEST_F(WifiHalWpaStaTest, WpaCliCmdWpsPbcTest, TestSize.Level1)
{
    EXPECT_TRUE(mInterface->wpaCliCmdWpsPbc(mInterface, nullptr) == 0);
    WpaWpsPbcArgv argv;
    ASSERT_TRUE(memset_s(&argv, sizeof(argv), 0, sizeof(argv)) == EOK);
    MockEraseSupportedCmd("WPS_PBC");
    EXPECT_TRUE(mInterface->wpaCliCmdWpsPbc(mInterface, &argv) < 0);
    MockSetWpaExpectCmdResponse("WPS_PBC", "FAIL\n");
    EXPECT_TRUE(mInterface->wpaCliCmdWpsPbc(mInterface, &argv) < 0);
    MockSetWpaExpectCmdResponse("WPS_PBC", "FAIL-PBC-OVERLAP");
    EXPECT_TRUE(mInterface->wpaCliCmdWpsPbc(mInterface, &argv) == 3);
    MockWpaCallback(mGlobalInterface->wpaCtrl.pSend, "<3>WPS-OVERLAP-DETECTED PBC session overlap\n");
    MockSetWpaExpectCmdResponse("WPS_PBC", "OK");
    EXPECT_TRUE(mInterface->wpaCliCmdWpsPbc(mInterface, &argv) == 0);
    MockWpaCallback(mGlobalInterface->wpaCtrl.pSend, "<3>WPS-TIMEOUT Requested operation timed out\n");
    argv.anyFlag = 1;
    argv.multiAp = 1;
    EXPECT_TRUE(mInterface->wpaCliCmdWpsPbc(mInterface, &argv) == 0);
    argv.anyFlag = 0;
    StrSafeCopy(argv.bssid, sizeof(argv.bssid), "00:00:00:00:00:00");
    EXPECT_TRUE(mInterface->wpaCliCmdWpsPbc(mInterface, &argv) == 0);
}

HWTEST_F(WifiHalWpaStaTest, WpaCliCmdWpsPinTest, TestSize.Level1)
{
    EXPECT_TRUE(mInterface->wpaCliCmdWpsPin(mInterface, nullptr, nullptr) < 0);
    WpaWpsPinArgv argv;
    ASSERT_TRUE(memset_s(&argv, sizeof(argv), 0, sizeof(argv)) == EOK);
    int pinCode = 0;
    MockEraseSupportedCmd("WPS_PIN");
    EXPECT_TRUE(mInterface->wpaCliCmdWpsPin(mInterface, &argv, &pinCode) < 0);
    MockSetWpaExpectCmdResponse("WPS_PIN", "12345678");
    EXPECT_TRUE(mInterface->wpaCliCmdWpsPin(mInterface, &argv, &pinCode) == 0);
    EXPECT_TRUE(pinCode == 12345678);
    StrSafeCopy(argv.bssid, sizeof(argv.bssid), "00:00:00:00:00:00");
    StrSafeCopy(argv.pinCode, sizeof(argv.pinCode), "87654321");
    MockSetWpaExpectCmdResponse("WPS_PIN", argv.pinCode);
    EXPECT_TRUE(mInterface->wpaCliCmdWpsPin(mInterface, &argv, &pinCode) == 0);
    EXPECT_TRUE(pinCode == 87654321);
}

HWTEST_F(WifiHalWpaStaTest, WpaCliCmdWpsCancelTest, TestSize.Level1)
{
    MockEraseSupportedCmd("WPS_CANCEL");
    EXPECT_TRUE(mInterface->wpaCliCmdWpsCancel(mInterface) < 0);
    MockSetWpaExpectCmdResponse("WPS_CANCEL", "FAIL\n");
    EXPECT_TRUE(mInterface->wpaCliCmdWpsCancel(mInterface) < 0);
    MockSetWpaExpectCmdResponse("WPS_CANCEL", "OK");
    EXPECT_TRUE(mInterface->wpaCliCmdWpsCancel(mInterface) == 0);
}

HWTEST_F(WifiHalWpaStaTest, WpaCliCmdPowerSaveTest, TestSize.Level1)
{
    MockEraseSupportedCmd("SET");
    EXPECT_TRUE(mInterface->wpaCliCmdPowerSave(mInterface, 0) < 0);
    MockSetWpaExpectCmdResponse("SET", "FAIL\n");
    EXPECT_TRUE(mInterface->wpaCliCmdPowerSave(mInterface, 0) < 0);
    MockSetWpaExpectCmdResponse("SET", "OK");
    EXPECT_TRUE(mInterface->wpaCliCmdPowerSave(mInterface, 0) == 0);
    EXPECT_TRUE(mInterface->wpaCliCmdPowerSave(mInterface, 1) == 0);
}

HWTEST_F(WifiHalWpaStaTest, WpaCliCmdSetCountryCodeTest, TestSize.Level1)
{
    EXPECT_TRUE(mInterface->wpaCliCmdSetCountryCode(mInterface, nullptr) < 0);
    MockEraseSupportedCmd("SET");
    EXPECT_TRUE(mInterface->wpaCliCmdSetCountryCode(mInterface, "CN") < 0);
    MockSetWpaExpectCmdResponse("SET", "FAIL\n");
    EXPECT_TRUE(mInterface->wpaCliCmdSetCountryCode(mInterface, "CN") < 0);
    MockSetWpaExpectCmdResponse("SET", "OK");
    EXPECT_TRUE(mInterface->wpaCliCmdSetCountryCode(mInterface, "CN") == 0);
}

HWTEST_F(WifiHalWpaStaTest, WpaCliCmdGetCountryCodeTest, TestSize.Level1)
{
    EXPECT_TRUE(mInterface->wpaCliCmdGetCountryCode(mInterface, nullptr, 0) < 0);
    char countryCode[3] = {0};
    int codeSize = 3;
    MockEraseSupportedCmd("GET");
    EXPECT_TRUE(mInterface->wpaCliCmdGetCountryCode(mInterface, countryCode, codeSize) < 0);
    MockSetWpaExpectCmdResponse("GET", "FAIL\n");
    EXPECT_TRUE(mInterface->wpaCliCmdGetCountryCode(mInterface, countryCode, codeSize) < 0);
    MockSetWpaExpectCmdResponse("GET", "OK");
    EXPECT_TRUE(mInterface->wpaCliCmdGetCountryCode(mInterface, countryCode, codeSize) == 0);
    EXPECT_TRUE(strcmp(countryCode, "OK") == 0);
}

HWTEST_F(WifiHalWpaStaTest, WpaCliCmdSetAutoConnectTest, TestSize.Level1)
{
    MockEraseSupportedCmd("STA_AUTOCONNECT");
    EXPECT_TRUE(mInterface->wpaCliCmdSetAutoConnect(mInterface, 0) < 0);
    MockSetWpaExpectCmdResponse("STA_AUTOCONNECT", "FAIL\n");
    EXPECT_TRUE(mInterface->wpaCliCmdSetAutoConnect(mInterface, 0) < 0);
    MockSetWpaExpectCmdResponse("STA_AUTOCONNECT", "OK");
    EXPECT_TRUE(mInterface->wpaCliCmdSetAutoConnect(mInterface, 0) == 0);
}

HWTEST_F(WifiHalWpaStaTest, WpaCliCmdWpaBlockListClearTest, TestSize.Level1)
{
    EXPECT_TRUE(mInterface->wpaCliCmdWpaBlockListClear(mInterface) < 0);
}

HWTEST_F(WifiHalWpaStaTest, WpaCliCmdListNetworksTest, TestSize.Level1)
{
    EXPECT_TRUE(mInterface->wpaCliCmdListNetworks(mInterface, nullptr, nullptr) < 0);
    WifiNetworkInfo a[256];
    ASSERT_TRUE(memset_s(a, sizeof(a), 0, sizeof(a)) == EOK);
    int size = 256;
    MockEraseSupportedCmd("LIST_NETWORKS");
    EXPECT_TRUE(mInterface->wpaCliCmdListNetworks(mInterface, a, &size) < 0);
    char buf[] = "network id / ssid / bssid / flags\n"
                 "1\tssid1\t00:xx:xx:xx:xx:00\t[DISABLED]\n"
                 "2\tssid2\tany\t[DISABLED]\n"
                 "3\tssid3\t00:00:00:00:00:00\t[CURRENT]\n"
                 "4\tssid4\tany\t[DISABLED]\n";
    MockSetWpaExpectCmdResponse("LIST_NETWORKS", buf);
    EXPECT_TRUE(mInterface->wpaCliCmdListNetworks(mInterface, a, &size) == 0);
    EXPECT_TRUE(size == 4);
    size = 2;
    EXPECT_TRUE(mInterface->wpaCliCmdListNetworks(mInterface, a, &size) == 0);
    EXPECT_TRUE(size == 2);
    EXPECT_TRUE(a[0].id == 1);
    EXPECT_TRUE(strcmp(a[0].ssid, "ssid1") == 0);
    EXPECT_TRUE(strcmp(a[0].bssid, "00:xx:xx:xx:xx:00") == 0);
    EXPECT_TRUE(strcmp(a[0].flags, "[DISABLED]") == 0);
    EXPECT_TRUE(a[1].id == 2);
    EXPECT_TRUE(strcmp(a[1].ssid, "ssid2") == 0);
    EXPECT_TRUE(strcmp(a[1].bssid, "any") == 0);
    EXPECT_TRUE(strcmp(a[1].flags, "[DISABLED]") == 0);
}

HWTEST_F(WifiHalWpaStaTest, WpaCliCmdGetSignalInfoTest, TestSize.Level1)
{
    EXPECT_TRUE(mInterface->wpaCliCmdGetSignalInfo(mInterface, nullptr) < 0);
    WpaSignalInfo info;
    ASSERT_TRUE(memset_s(&info, sizeof(info), 0, sizeof(info)) == EOK);
    EXPECT_TRUE(mInterface->wpaCliCmdGetSignalInfo(mInterface, &info) == 0);
    MockEraseSupportedCmd("SIGNAL_POLL");
    EXPECT_TRUE(mInterface->wpaCliCmdGetSignalInfo(mInterface, &info) < 0);
    MockSetWpaExpectCmdResponse("SIGNAL_POLL", "FAIL\n");
    EXPECT_TRUE(mInterface->wpaCliCmdGetSignalInfo(mInterface, &info) < 0);
    char buf[] = "RSSI=-86\nLINKSPEED=1024\nNOISE=128\nFREQUENCY=5555\n";
    MockSetWpaExpectCmdResponse("SIGNAL_POLL", buf);
    EXPECT_TRUE(mInterface->wpaCliCmdGetSignalInfo(mInterface, &info) == 0);
    EXPECT_TRUE(info.signal == -86);
    EXPECT_TRUE(info.txrate == 1024);
    EXPECT_TRUE(info.noise == 128);
    EXPECT_TRUE(info.frequency == 5555);
}
}  // namespace Wifi
}  // namespace OHOS