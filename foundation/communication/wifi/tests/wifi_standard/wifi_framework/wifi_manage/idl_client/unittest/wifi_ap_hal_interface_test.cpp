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
#include "wifi_ap_hal_interface_test.h"
#include "wifi_ap_hal_interface.h"
#include "wifi_log.h"
#undef LOG_TAG
#define LOG_TAG "OHWIFI_IDLCLIENT_AP_TEST"

using namespace testing::ext;

namespace OHOS {
namespace Wifi {
void ApHalTestStationCbk(const WifiApConnectionNofify &info)
{
    LOGD("apHalTestStationCbk");
    LOGD("station : %s, %s", info.mac.c_str(), ((info.type == 105) ? "join" : "leave"));
}

void ApHalTestStateCbk(int state)
{
    LOGD("apHalTestStateCbk state: %d", state);
}

void WifiApHalInterfaceTest::SetUpTestCase()
{
    IWifiApMonitorEventCallback callback;
    callback.onStaJoinOrLeave = ApHalTestStationCbk;
    callback.onApEnableOrDisable = ApHalTestStateCbk;
    WifiApHalInterface::GetInstance().RegisterApEvent(callback);
}

void WifiApHalInterfaceTest::TearDownTestCase()
{
    IWifiApMonitorEventCallback callback;
    WifiApHalInterface::GetInstance().RegisterApEvent(callback);
}

HWTEST_F(WifiApHalInterfaceTest, StartApTest, TestSize.Level1)
{
    WifiErrorNo ret = WifiApHalInterface::GetInstance().StartAp();
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiApHalInterfaceTest, SetSoftApConfigTest, TestSize.Level1)
{
    HotspotConfig config;
    config.SetSecurityType(KeyMgmt::NONE);
    config.SetSsid("myWifiTest");
    config.SetBand(BandType::BAND_2GHZ);
    config.SetChannel(0);
    config.SetMaxConn(1);
    WifiErrorNo ret = WifiApHalInterface::GetInstance().SetSoftApConfig(config);
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiApHalInterfaceTest, GetStationListTest, TestSize.Level1)
{
    std::vector<std::string> vec;
    WifiErrorNo ret = WifiApHalInterface::GetInstance().GetStationList(vec);
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiApHalInterfaceTest, AddBlockByMacTest, TestSize.Level1)
{
    std::string mac = "abcdefghijklmn";
    WifiErrorNo ret = WifiApHalInterface::GetInstance().AddBlockByMac(mac);
    EXPECT_TRUE(ret == WIFI_IDL_OPT_INPUT_MAC_INVALID);
    mac = "00:00:00:00:00:00";
    ret = WifiApHalInterface::GetInstance().AddBlockByMac(mac);
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiApHalInterfaceTest, DelBlockByMacTest, TestSize.Level1)
{
    std::string mac = "abcdefghijklmn";
    WifiErrorNo ret = WifiApHalInterface::GetInstance().DelBlockByMac(mac);
    EXPECT_TRUE(ret == WIFI_IDL_OPT_INPUT_MAC_INVALID);
    mac = "00:00:00:00:00:00";
    ret = WifiApHalInterface::GetInstance().DelBlockByMac(mac);
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiApHalInterfaceTest, RemoveStationTest, TestSize.Level1)
{
    std::string mac = "abcdefghijklmn";
    WifiErrorNo ret = WifiApHalInterface::GetInstance().RemoveStation(mac);
    EXPECT_TRUE(ret == WIFI_IDL_OPT_INPUT_MAC_INVALID);
    mac = "00:00:00:00:00:00";
    ret = WifiApHalInterface::GetInstance().RemoveStation(mac);
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiApHalInterfaceTest, DisconnectStaByMacTest, TestSize.Level1)
{
    std::string mac = "abcdefghijklmn";
    WifiErrorNo ret = WifiApHalInterface::GetInstance().DisconnectStaByMac(mac);
    EXPECT_TRUE(ret == WIFI_IDL_OPT_INPUT_MAC_INVALID);
    mac = "00:00:00:00:00:00";
    ret = WifiApHalInterface::GetInstance().DisconnectStaByMac(mac);
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiApHalInterfaceTest, GetFrequenciesByBandTest, TestSize.Level1)
{
    std::vector<int> freq;
    WifiErrorNo ret = WifiApHalInterface::GetInstance().GetFrequenciesByBand(1, freq);
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiApHalInterfaceTest, SetWifiCountryCodeTest, TestSize.Level1)
{
    std::string code = "CN";
    WifiErrorNo ret = WifiApHalInterface::GetInstance().SetWifiCountryCode(code);
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiApHalInterfaceTest, StopApTest, TestSize.Level1)
{
    WifiErrorNo ret = WifiApHalInterface::GetInstance().StopAp();
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
}
}  // namespace Wifi
}  // namespace OHOS