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

#include "wifi_global_func_test.h"
#include "wifi_global_func.h"

using namespace testing::ext;

namespace OHOS {
namespace Wifi {
HWTEST_F(WifiGlobalFuncTest, CfgCheckSsid, TestSize.Level1)
{
    HotspotConfig config;
    EXPECT_TRUE(CfgCheckSsid(config) != ErrCode::WIFI_OPT_SUCCESS);
    config.SetSsid("1");
    EXPECT_TRUE(CfgCheckSsid(config) == ErrCode::WIFI_OPT_SUCCESS);
    config.SetSsid("12");
    EXPECT_TRUE(CfgCheckSsid(config) == ErrCode::WIFI_OPT_SUCCESS);
    config.SetSsid("123456789 123456789 123456789 1");
    EXPECT_TRUE(CfgCheckSsid(config) == ErrCode::WIFI_OPT_SUCCESS);
    config.SetSsid("123456789 123456789 123456789 12");
    EXPECT_TRUE(CfgCheckSsid(config) == ErrCode::WIFI_OPT_SUCCESS);
    config.SetSsid("123456789 123456789 123456789 123");
    EXPECT_TRUE(CfgCheckSsid(config) != ErrCode::WIFI_OPT_SUCCESS);
}

HWTEST_F(WifiGlobalFuncTest, CfgCheckPsk, TestSize.Level1)
{
    HotspotConfig config;
    EXPECT_TRUE(CfgCheckPsk(config) != ErrCode::WIFI_OPT_SUCCESS);
    config.SetPreSharedKey("1234567");
    EXPECT_TRUE(CfgCheckPsk(config) != ErrCode::WIFI_OPT_SUCCESS);
    config.SetPreSharedKey("12345678");
    EXPECT_TRUE(CfgCheckPsk(config) == ErrCode::WIFI_OPT_SUCCESS);
    config.SetPreSharedKey("123456789");
    EXPECT_TRUE(CfgCheckPsk(config) == ErrCode::WIFI_OPT_SUCCESS);
    config.SetPreSharedKey("123456789 123456789 123456789 123456789 123456789 123456789 123");
    EXPECT_TRUE(CfgCheckPsk(config) == ErrCode::WIFI_OPT_SUCCESS);
    config.SetPreSharedKey("123456789 123456789 123456789 123456789 123456789 123456789 1234");
    EXPECT_TRUE(CfgCheckPsk(config) != ErrCode::WIFI_OPT_SUCCESS);
}

HWTEST_F(WifiGlobalFuncTest, CfgCheckBand, TestSize.Level1)
{
    HotspotConfig config;
    std::vector<BandType> bands;
    EXPECT_TRUE(CfgCheckBand(config, bands) != ErrCode::WIFI_OPT_SUCCESS);
    bands.push_back(BandType::BAND_2GHZ);
    config.SetBand(BandType::BAND_2GHZ);
    EXPECT_TRUE(CfgCheckBand(config, bands) == ErrCode::WIFI_OPT_SUCCESS);
    config.SetBand(BandType::BAND_NONE);
    EXPECT_TRUE(CfgCheckBand(config, bands) != ErrCode::WIFI_OPT_SUCCESS);
}

HWTEST_F(WifiGlobalFuncTest, CfgCheckChannel, TestSize.Level1)
{
    HotspotConfig config;
    ChannelsTable table;
    table[BandType::BAND_2GHZ].push_back(1);
    EXPECT_TRUE(CfgCheckChannel(config, table) != ErrCode::WIFI_OPT_SUCCESS);
    config.SetBand(BandType::BAND_2GHZ);
    config.SetChannel(1);
    EXPECT_TRUE(CfgCheckChannel(config, table) == ErrCode::WIFI_OPT_SUCCESS);
}

HWTEST_F(WifiGlobalFuncTest, IsValidHotspotConfig, TestSize.Level1)
{
    HotspotConfig config, centerConfig;
    std::vector<BandType> bands;
    ChannelsTable table;
    EXPECT_TRUE(IsValidHotspotConfig(config, centerConfig, bands, table) == ErrCode::WIFI_OPT_INVALID_PARAM);
    config.SetSsid("123456");
    config.SetSecurityType(KeyMgmt::NONE);
    config.SetPreSharedKey("12345678");
    EXPECT_TRUE(IsValidHotspotConfig(config, centerConfig, bands, table) == ErrCode::WIFI_OPT_INVALID_PARAM);
    config.SetSecurityType(KeyMgmt::FT_EAP);
    EXPECT_TRUE(IsValidHotspotConfig(config, centerConfig, bands, table) == ErrCode::WIFI_OPT_INVALID_PARAM);
    config.SetSecurityType(KeyMgmt::WPA_EAP);
    EXPECT_TRUE(IsValidHotspotConfig(config, centerConfig, bands, table) == ErrCode::WIFI_OPT_INVALID_PARAM);
    config.SetSecurityType(KeyMgmt::IEEE8021X);
    EXPECT_TRUE(IsValidHotspotConfig(config, centerConfig, bands, table) == ErrCode::WIFI_OPT_INVALID_PARAM);
    config.SetSecurityType(KeyMgmt::OSEN);
    EXPECT_TRUE(IsValidHotspotConfig(config, centerConfig, bands, table) == ErrCode::WIFI_OPT_INVALID_PARAM);
    config.SetSecurityType(KeyMgmt::FT_PSK);
    EXPECT_TRUE(IsValidHotspotConfig(config, centerConfig, bands, table) == ErrCode::WIFI_OPT_INVALID_PARAM);
    config.SetSecurityType(KeyMgmt::WPA2_PSK);
    config.SetPreSharedKey("");
    EXPECT_TRUE(IsValidHotspotConfig(config, centerConfig, bands, table) == ErrCode::WIFI_OPT_INVALID_PARAM);
    config.SetPreSharedKey("12345678");
    centerConfig.SetChannel(1024);
    EXPECT_TRUE(IsValidHotspotConfig(config, centerConfig, bands, table) == ErrCode::WIFI_OPT_INVALID_PARAM);
    config.SetChannel(1024);
    EXPECT_TRUE(IsValidHotspotConfig(config, centerConfig, bands, table) == ErrCode::WIFI_OPT_SUCCESS);
}

HWTEST_F(WifiGlobalFuncTest, GetRandomStr, TestSize.Level1)
{
    std::string str = GetRandomStr(0);
    EXPECT_TRUE(str.length() == 0);
    str = GetRandomStr(1);
    EXPECT_TRUE(str.length() == 1);
    str = GetRandomStr(MAX_PSK_LEN);
    EXPECT_TRUE(str.length() == MAX_PSK_LEN);
    str = GetRandomStr(MAX_PSK_LEN + 1);
    EXPECT_TRUE(str.length() == MAX_PSK_LEN);
}

HWTEST_F(WifiGlobalFuncTest, CheckMacIsValid, TestSize.Level1)
{
    std::string str;
    EXPECT_TRUE(CheckMacIsValid(str) == -1);
    str = "00:00:00:00:00:00";
    EXPECT_TRUE(CheckMacIsValid(str) == 0);
    str = "ah:00:00:00:00:00";
    EXPECT_TRUE(CheckMacIsValid(str) == -1);
    str = "AH:00:00:00:00:00";
    EXPECT_TRUE(CheckMacIsValid(str) == -1);
    str = "00.00.00.00.00.00";
    EXPECT_TRUE(CheckMacIsValid(str) == -1);
}

HWTEST_F(WifiGlobalFuncTest, ConvertConnStateInternalTest, TestSize.Level1)
{
    bool isReport = true;
    EXPECT_TRUE(ConvertConnStateInternal(OperateResState::CONNECT_CONNECTING, isReport) == ConnState::CONNECTING);
    EXPECT_TRUE(
        ConvertConnStateInternal(OperateResState::CONNECT_AP_CONNECTED, isReport) == ConnState::CONNECTED);

    EXPECT_TRUE(ConvertConnStateInternal(OperateResState::DISCONNECT_DISCONNECTING, isReport) ==
                ConnState::DISCONNECTING);
    EXPECT_TRUE(
        ConvertConnStateInternal(OperateResState::DISCONNECT_DISCONNECTED, isReport) == ConnState::DISCONNECTED);
}

HWTEST_F(WifiGlobalFuncTest, SplitStringTest, TestSize.Level1)
{
    std::string str = "upnp 10 uuid:xxxxxxxxxxxxx-xxxxx";
    std::vector<std::string> vec;
    OHOS::Wifi::SplitString(str, "", vec);
    ASSERT_TRUE(vec.size() == 1);
    EXPECT_TRUE(vec[0] == str);
    vec.clear();
    OHOS::Wifi::SplitString(str, " ", vec);
    ASSERT_TRUE(vec.size() == 3);
    EXPECT_TRUE(vec[0] == "upnp");
    EXPECT_TRUE(vec[1] == "10");
    EXPECT_TRUE(vec[2] == "uuid:xxxxxxxxxxxxx-xxxxx");
}

HWTEST_F(WifiGlobalFuncTest, Vec2StreamTest, TestSize.Level1)
{
    std::string prefix = "head|";
    std::vector<char> vecChar;
    std::string sufffix = "|tail";
    std::string result = OHOS::Wifi::Vec2Stream(prefix, vecChar, sufffix);
    std::string expect = "head||tail";
    EXPECT_TRUE(result == expect);
    char tmp = (char)255;
    vecChar.push_back(tmp);
    result = OHOS::Wifi::Vec2Stream(prefix, vecChar, sufffix);
    expect = "head|FF |tail";
    EXPECT_TRUE(result == expect);
    tmp = (char)0;
    vecChar.push_back(tmp);
    result = OHOS::Wifi::Vec2Stream(prefix, vecChar, sufffix);
    expect = "head|FF 00 |tail";
    EXPECT_TRUE(result == expect);
}
}  // namespace Wifi
}  // namespace OHOS