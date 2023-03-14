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
#include "wifi_sta_hal_interface_test.h"
#include "wifi_sta_hal_interface.h"
#include "wifi_supplicant_hal_interface.h"
#include "wifi_log.h"
#undef LOG_TAG
#define LOG_TAG "OHWIFI_IDLCLIENT_STA_TEST"

using namespace testing::ext;

namespace OHOS {
namespace Wifi {
void OnConnectChanged(int status, int networkId, const std::string &bssid)
{
    LOGD("OnConnectChanged status %d, networkid %d, bssid %s", status, networkId, bssid.c_str());
}

void OnWpaStateChanged(int status)
{
    LOGD("OnWpaStateChanged status %d", status);
}

void OnWpaSsidWrongKey(int status)
{
    LOGD("OnWpaSsidWrongKey status %d", status);
}

void OnWpsOverlap(int status)
{
    LOGD("OnWpsOverlap status %d", status);
}

void OnWpsTimeOut(int status)
{
    LOGD("OnWpsTimeOut status %d", status);
}

void OnScanNotify(int result)
{
    LOGD("OnScanNotify result %d", result);
}

void WifiStaHalInterfaceTest::SetUpTestCase()
{
    WifiEventCallback callback;
    callback.onConnectChanged = OnConnectChanged;
    callback.onWpaStateChanged = OnWpaStateChanged;
    callback.onWpaSsidWrongKey = OnWpaSsidWrongKey;
    callback.onWpsOverlap = OnWpsOverlap;
    callback.onWpsTimeOut = OnWpsTimeOut;
    WifiStaHalInterface::GetInstance().RegisterStaEventCallback(callback);

    SupplicantEventCallback cbk;
    cbk.onScanNotify = OnScanNotify;
    WifiSupplicantHalInterface::GetInstance().RegisterSupplicantEventCallback(cbk);
}

void WifiStaHalInterfaceTest::TearDownTestCase()
{
    WifiEventCallback callback;
    WifiStaHalInterface::GetInstance().RegisterStaEventCallback(callback);
    WifiSupplicantHalInterface::GetInstance().UnRegisterSupplicantEventCallback();
}

HWTEST_F(WifiStaHalInterfaceTest, StartWifiTest, TestSize.Level1)
{
    WifiErrorNo ret = WifiStaHalInterface::GetInstance().StartWifi();
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiStaHalInterfaceTest, ConnectTest, TestSize.Level1)
{
    WifiErrorNo ret = WifiStaHalInterface::GetInstance().Connect(1);
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiStaHalInterfaceTest, ReconnectTest, TestSize.Level1)
{
    WifiErrorNo ret = WifiStaHalInterface::GetInstance().Reconnect();
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiStaHalInterfaceTest, ReassociateTest, TestSize.Level1)
{
    WifiErrorNo ret = WifiStaHalInterface::GetInstance().Reassociate();
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiStaHalInterfaceTest, DisconnectTest, TestSize.Level1)
{
    WifiErrorNo ret = WifiStaHalInterface::GetInstance().Disconnect();
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiStaHalInterfaceTest, GetStaCapabilitiesTest, TestSize.Level1)
{
    unsigned int capabilities = 0;
    WifiErrorNo ret = WifiStaHalInterface::GetInstance().GetStaCapabilities(capabilities);
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiStaHalInterfaceTest, GetStaDeviceMacAddressTest, TestSize.Level1)
{
    std::string mac;
    WifiErrorNo ret = WifiStaHalInterface::GetInstance().GetStaDeviceMacAddress(mac);
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiStaHalInterfaceTest, GetSupportFrequenciesTest, TestSize.Level1)
{
    std::vector<int> freq;
    WifiErrorNo ret = WifiStaHalInterface::GetInstance().GetSupportFrequencies(0, freq);
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiStaHalInterfaceTest, SetConnectMacAddrTest, TestSize.Level1)
{
    std::string mac = "abcdefghijklmn";
    WifiErrorNo ret = WifiStaHalInterface::GetInstance().SetConnectMacAddr(mac);
    EXPECT_TRUE(ret == WIFI_IDL_OPT_INPUT_MAC_INVALID);
    mac = "38:d2:69:ef:47:59";
    ret = WifiStaHalInterface::GetInstance().SetConnectMacAddr(mac);
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiStaHalInterfaceTest, SetScanMacAddressTest, TestSize.Level1)
{
    std::string mac = "abcdefghijklmn";
    WifiErrorNo ret = WifiStaHalInterface::GetInstance().SetScanMacAddress(mac);
    EXPECT_TRUE(ret == WIFI_IDL_OPT_INPUT_MAC_INVALID);
    mac = "38:d2:69:ef:47:59";
    ret = WifiStaHalInterface::GetInstance().SetScanMacAddress(mac);
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiStaHalInterfaceTest, DisconnectLastRoamingBssidTest, TestSize.Level1)
{
    std::string mac = "abcdefghijklmn";
    WifiErrorNo ret = WifiStaHalInterface::GetInstance().DisconnectLastRoamingBssid(mac);
    EXPECT_TRUE(ret == WIFI_IDL_OPT_INPUT_MAC_INVALID);
    mac = "38:d2:69:ef:47:59";
    ret = WifiStaHalInterface::GetInstance().DisconnectLastRoamingBssid(mac);
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiStaHalInterfaceTest, GetSupportFeatureTest, TestSize.Level1)
{
    long feature = 0;
    WifiErrorNo ret = WifiStaHalInterface::GetInstance().GetSupportFeature(feature);
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiStaHalInterfaceTest, SendRequestTest, TestSize.Level1)
{
    WifiStaRequest request;
    WifiErrorNo ret = WifiStaHalInterface::GetInstance().SendRequest(request);
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiStaHalInterfaceTest, SetTxPowerTest, TestSize.Level1)
{
    WifiErrorNo ret = WifiStaHalInterface::GetInstance().SetTxPower(0);
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiStaHalInterfaceTest, ScanTest, TestSize.Level1)
{
    WifiScanParam scanParam;
    scanParam.hiddenNetworkSsid.push_back("OHOS_testAp");
    scanParam.scanFreqs.push_back(2412);
    scanParam.scanStyle = 0;
    WifiErrorNo ret = WifiStaHalInterface::GetInstance().Scan(scanParam);
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiStaHalInterfaceTest, QueryScanInfosTest, TestSize.Level1)
{
    std::vector<InterScanInfo> vec;
    WifiErrorNo ret = WifiStaHalInterface::GetInstance().QueryScanInfos(vec);
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiStaHalInterfaceTest, GetNetworkListTest, TestSize.Level1)
{
    std::vector<WifiWpaNetworkInfo> vec;
    WifiErrorNo ret = WifiStaHalInterface::GetInstance().GetNetworkList(vec);
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiStaHalInterfaceTest, StartPnoScanTest, TestSize.Level1)
{
    WifiPnoScanParam scanParam;
    scanParam.scanFreqs.push_back(2412);
    scanParam.hiddenSsid.push_back("OHOS_testAp");
    scanParam.savedSsid.push_back("cmcc");
    WifiErrorNo ret = WifiStaHalInterface::GetInstance().StartPnoScan(scanParam);
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiStaHalInterfaceTest, StopPnoScanTest, TestSize.Level1)
{
    WifiErrorNo ret = WifiStaHalInterface::GetInstance().StopPnoScan();
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiStaHalInterfaceTest, RemoveDeviceConfigTest, TestSize.Level1)
{
    WifiErrorNo ret = WifiStaHalInterface::GetInstance().RemoveDevice(1);
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiStaHalInterfaceTest, ClearDeviceConfigTest, TestSize.Level1)
{
    WifiErrorNo ret = WifiStaHalInterface::GetInstance().ClearDeviceConfig();
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiStaHalInterfaceTest, GetNextNetworkIdTest, TestSize.Level1)
{
    int id = 0;
    WifiErrorNo ret = WifiStaHalInterface::GetInstance().GetNextNetworkId(id);
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiStaHalInterfaceTest, EnableNetworkTest, TestSize.Level1)
{
    WifiErrorNo ret = WifiStaHalInterface::GetInstance().EnableNetwork(1);
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiStaHalInterfaceTest, DisableNetworkTest, TestSize.Level1)
{
    WifiErrorNo ret = WifiStaHalInterface::GetInstance().DisableNetwork(1);
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiStaHalInterfaceTest, SetDeviceConfigTest, TestSize.Level1)
{
    WifiIdlDeviceConfig cfg;
    cfg.psk = "1234567";
    WifiErrorNo ret = WifiStaHalInterface::GetInstance().SetDeviceConfig(1, cfg);
    EXPECT_TRUE(ret == WIFI_IDL_OPT_FAILED);
    cfg.psk = "01234567890123456789012345678901234567890123456789012345678901234";
    ret = WifiStaHalInterface::GetInstance().SetDeviceConfig(1, cfg);
    EXPECT_TRUE(ret == WIFI_IDL_OPT_FAILED);
    cfg.psk = "12345678";
    ret = WifiStaHalInterface::GetInstance().SetDeviceConfig(1, cfg);
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
    cfg.authAlgorithms = 8;
    ret = WifiStaHalInterface::GetInstance().SetDeviceConfig(1, cfg);
    EXPECT_TRUE(ret == WIFI_IDL_OPT_FAILED);
    cfg.authAlgorithms = 7;
    ret = WifiStaHalInterface::GetInstance().SetDeviceConfig(1, cfg);
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
    cfg.ssid = "Honor";
    cfg.priority = 5;
    cfg.scanSsid = 1;
    cfg.wepKeyIdx = 2;
    ret = WifiStaHalInterface::GetInstance().SetDeviceConfig(1, cfg);
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiStaHalInterfaceTest, GetDeviceConfigTest, TestSize.Level1)
{
    WifiIdlGetDeviceConfig cfg;
    WifiErrorNo ret = WifiStaHalInterface::GetInstance().GetDeviceConfig(cfg);
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiStaHalInterfaceTest, SaveDeviceConfigTest, TestSize.Level1)
{
    WifiErrorNo ret = WifiStaHalInterface::GetInstance().SaveDeviceConfig();
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiStaHalInterfaceTest, StartWpsPbcModeTest, TestSize.Level1)
{
    WifiIdlWpsConfig cfg;
    WifiErrorNo ret = WifiStaHalInterface::GetInstance().StartWpsPbcMode(cfg);
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiStaHalInterfaceTest, StartWpsPinModeTest, TestSize.Level1)
{
    WifiIdlWpsConfig cfg;
    int pinCode = 0;
    cfg.pinCode = "0000";
    WifiErrorNo ret = WifiStaHalInterface::GetInstance().StartWpsPinMode(cfg, pinCode);
    EXPECT_TRUE(ret == WIFI_IDL_OPT_INVALID_PARAM);
    cfg.pinCode = "12345678";
    ret = WifiStaHalInterface::GetInstance().StartWpsPinMode(cfg, pinCode);
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiStaHalInterfaceTest, StopWpsTest, TestSize.Level1)
{
    WifiErrorNo ret = WifiStaHalInterface::GetInstance().StopWps();
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiStaHalInterfaceTest, GetRoamingCapabilitiesTest, TestSize.Level1)
{
    WifiIdlRoamCapability capability;
    WifiErrorNo ret = WifiStaHalInterface::GetInstance().GetRoamingCapabilities(capability);
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiStaHalInterfaceTest, SetRoamConfigTest, TestSize.Level1)
{
    WifiIdlRoamConfig cfg;
    WifiErrorNo ret = WifiStaHalInterface::GetInstance().SetRoamConfig(cfg);
    EXPECT_TRUE(ret == WIFI_IDL_OPT_FAILED);
    cfg.blocklistBssids.push_back("00:00:00:00:00:00");
    cfg.trustlistBssids.push_back("10:00:00:00:00:00");
    ret = WifiStaHalInterface::GetInstance().SetRoamConfig(cfg);
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiStaHalInterfaceTest, WpaAutoConnectTest, TestSize.Level1)
{
    WifiErrorNo ret = WifiStaHalInterface::GetInstance().WpaAutoConnect(1);
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
    ret = WifiStaHalInterface::GetInstance().WpaAutoConnect(0);
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiStaHalInterfaceTest, WpaBlocklistClearTest, TestSize.Level1)
{
    WifiErrorNo ret = WifiStaHalInterface::GetInstance().WpaBlocklistClear();
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiStaHalInterfaceTest, GetConnectSignalInfoTest, TestSize.Level1)
{
    std::string endBssid;
    WifiWpaSignalInfo info;
    WifiErrorNo ret = WifiStaHalInterface::GetInstance().GetConnectSignalInfo(endBssid, info);
    EXPECT_TRUE(ret == WIFI_IDL_OPT_INPUT_MAC_INVALID);
    endBssid = "00:00:00:00:00:00";
    ret = WifiStaHalInterface::GetInstance().GetConnectSignalInfo(endBssid, info);
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiStaHalInterfaceTest, StopWifiTest, TestSize.Level1)
{
    WifiErrorNo ret = WifiStaHalInterface::GetInstance().StopWifi();
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
}
}  // namespace Wifi
}  // namespace OHOS