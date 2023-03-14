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
#include "wifi_hal_ap_interface_test.h"
#include "securec.h"
#include "wifi_log.h"
#include "wifi_hal_ap_interface.h"
#include "wifi_hal_common_func.h"
#include "mock_wpa_ctrl.h"

using namespace testing::ext;

namespace OHOS {
namespace Wifi {
void WifiHalApInterfaceTest::SetUpTestCase()
{
    MockInitApSupportedCmd();
}

HWTEST_F(WifiHalApInterfaceTest, StartSoftApTest, TestSize.Level1)
{
    EXPECT_TRUE(StartSoftAp() == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalApInterfaceTest, GetStaInfosTest, TestSize.Level1)
{
    char infos[4096] = {0};
    int size = 4096;
    EXPECT_TRUE(GetStaInfos(NULL, NULL) == WIFI_HAL_FAILED);
    EXPECT_TRUE(GetStaInfos(infos, NULL) == WIFI_HAL_FAILED);
    EXPECT_TRUE(GetStaInfos(NULL, &size) == WIFI_HAL_FAILED);
    EXPECT_TRUE(GetStaInfos(infos, &size) == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalApInterfaceTest, SetCountryCodeTest, TestSize.Level1)
{
    EXPECT_TRUE(SetCountryCode(NULL) == WIFI_HAL_INVALID_PARAM);
    EXPECT_TRUE(SetCountryCode("") == WIFI_HAL_INVALID_PARAM);
    EXPECT_TRUE(SetCountryCode("C") == WIFI_HAL_INVALID_PARAM);
    EXPECT_TRUE(SetCountryCode("CN") == WIFI_HAL_SUCCESS);
    EXPECT_TRUE(SetCountryCode("CHINA") == WIFI_HAL_INVALID_PARAM);
}

HWTEST_F(WifiHalApInterfaceTest, SetHostapdConfigTest, TestSize.Level1)
{
    EXPECT_TRUE(SetHostapdConfig(NULL) == WIFI_HAL_FAILED);
    HostapdConfig config;
    ASSERT_TRUE(memset_s(&config, sizeof(config), 0, sizeof(config)) == EOK);
    config.band = AP_2GHZ_BAND;
    config.channel = 6;
    config.maxConn = 20;
    config.securityType = WPA_PSK;
    StrSafeCopy(config.ssid, sizeof(config.ssid), "test_ssid");
    config.ssidLen = strlen(config.ssid);
    StrSafeCopy(config.preSharedKey, sizeof(config.preSharedKey), "adc123456");
    config.preSharedKeyLen = strlen(config.preSharedKey);
    EXPECT_TRUE(SetHostapdConfig(&config) == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalApInterfaceTest, SetMacFilterTest, TestSize.Level1)
{
    EXPECT_TRUE(SetMacFilter(NULL, 0) == WIFI_HAL_FAILED);
    unsigned char tmpMac[] = "00:00:00:00:00";
    EXPECT_TRUE(SetMacFilter(tmpMac, strlen((const char *)tmpMac)) == WIFI_HAL_FAILED);
    unsigned char tmpMac2[] = "00.00.00.00.00.00";
    EXPECT_TRUE(SetMacFilter(tmpMac2, strlen((const char *)tmpMac2)) == WIFI_HAL_INPUT_MAC_INVALID);
    unsigned char mac[] = "00:00:00:00:00:00";
    EXPECT_TRUE(SetMacFilter(mac, strlen((const char *)mac)) == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalApInterfaceTest, DelMacFilterTest, TestSize.Level1)
{
    EXPECT_TRUE(DelMacFilter(NULL, 0) == WIFI_HAL_FAILED);
    unsigned char tmpMac[] = "00:00:00:00:00";
    EXPECT_TRUE(DelMacFilter(tmpMac, strlen((const char *)tmpMac)) == WIFI_HAL_FAILED);
    unsigned char tmpMac2[] = "00.00.00.00.00.00";
    EXPECT_TRUE(DelMacFilter(tmpMac2, strlen((const char *)tmpMac2)) == WIFI_HAL_INPUT_MAC_INVALID);
    unsigned char mac[] = "00:00:00:00:00:00";
    EXPECT_TRUE(DelMacFilter(mac, strlen((const char *)mac)) == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalApInterfaceTest, DisassociateStaTest, TestSize.Level1)
{
    EXPECT_TRUE(DisassociateSta(NULL, 0) == WIFI_HAL_FAILED);
    unsigned char tmpMac[] = "00:00:00:00:00";
    EXPECT_TRUE(DisassociateSta(tmpMac, strlen((const char *)tmpMac)) == WIFI_HAL_FAILED);
    unsigned char tmpMac2[] = "00.00.00.00.00.00";
    EXPECT_TRUE(DisassociateSta(tmpMac2, strlen((const char *)tmpMac2)) == WIFI_HAL_INPUT_MAC_INVALID);
    unsigned char mac[] = "00:00:00:00:00:00";
    EXPECT_TRUE(DisassociateSta(mac, strlen((const char *)mac)) == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalApInterfaceTest, GetValidFrequenciesForBandTest, TestSize.Level1)
{
    int32_t band = AP_2GHZ_BAND;
    int frequencies[20] = {0};
    int size = 20;
    EXPECT_TRUE(GetValidFrequenciesForBand(band, NULL, NULL) == WIFI_HAL_FAILED);
    EXPECT_TRUE(GetValidFrequenciesForBand(band, frequencies, NULL) == WIFI_HAL_FAILED);
    EXPECT_TRUE(GetValidFrequenciesForBand(band, NULL, &size) == WIFI_HAL_FAILED);
    WifiErrorNo err = GetValidFrequenciesForBand(band, frequencies, &size);
    EXPECT_TRUE(err == WIFI_HAL_SUCCESS || err == WIFI_HAL_NOT_SUPPORT);
}

HWTEST_F(WifiHalApInterfaceTest, StopSoftApTest, TestSize.Level1)
{
    EXPECT_TRUE(StopSoftAp() == WIFI_HAL_SUCCESS);
}
}  // namespace Wifi
}  // namespace OHOS