/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <cstdint>
#include <gtest/gtest.h>
#include <securec.h>
#include "hdf_base.h"
#include "hdf_uhdf_test.h"
#include "hdf_wifi_test.h"
#include "hdf_sbuf.h"
#include "wifi_hal.h"
#include "wifi_hal_ap_feature.h"
#include "wifi_hal_base_feature.h"
#include "wifi_hal_sta_feature.h"

using namespace testing::ext;

namespace HalTest {
struct IWiFi *g_wifi = nullptr;
const int32_t WLAN_TX_POWER = 160;
const uint32_t WLAN_MIN_CHIPID = 0;
const uint32_t WLAN_MAX_CHIPID = 2;
const uint32_t IFNAME_MIN_NUM = 0;
const uint32_t IFNAME_MAX_NUM = 32;
const uint32_t MAX_IF_NAME_LENGTH = 16;
const uint32_t SIZE = 4;
const uint32_t SLEEP_TIME = 5;
const uint32_t RESET_TIME = 15;
const int32_t WLAN_BAND_2G = 0;
const int32_t WLAN_FREQ_MAX_NUM = 14;
const int32_t WLAN_MAX_NUM_STA_WITH_AP = 4;
const uint32_t DEFAULT_COMBO_SIZE = 10;

class WifiHalTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void WifiHalTest::SetUpTestCase()
{
    int ret;

    ret = WifiConstruct(&g_wifi);
    ASSERT_EQ(HDF_SUCCESS, ret);
}

void WifiHalTest::TearDownTestCase()
{
    int ret;

    ret = WifiDestruct(&g_wifi);
    ASSERT_EQ(HDF_SUCCESS, ret);
}

void WifiHalTest::SetUp()
{
    int ret;

    ret = g_wifi->start(nullptr);
    ASSERT_EQ(HDF_ERR_INVALID_PARAM, ret);
    ret = g_wifi->start(g_wifi);
    ASSERT_EQ(HDF_SUCCESS, ret);
}

void WifiHalTest::TearDown()
{
    int ret;

    ret = g_wifi->stop(nullptr);
    ASSERT_EQ(HDF_ERR_INVALID_PARAM, ret);
    ret = g_wifi->stop(g_wifi);
    ASSERT_EQ(HDF_SUCCESS, ret);
}

static void ParseScanResult(WifiScanResult *scanResult)
{
    printf("ParseScanResult: flags=%d, caps=%d, freq=%d, beaconInt=%d,\n",
        scanResult->flags, scanResult->caps, scanResult->freq, scanResult->beaconInt);
    printf("ParseScanResult: qual=%d, beaconIeLen=%d, level=%d, age=%d, ieLen=%d,\n",
        scanResult->qual, scanResult->beaconIeLen, scanResult->level, scanResult->age, scanResult->ieLen);
}

static int32_t HalCallbackEvent(uint32_t event, void *respData, const char *ifName)
{
    (void)event;
    if (respData == nullptr) {
        return HDF_FAILURE;
    }
    printf("HalCallbackEvent ifName = %s, event = %d\n", ifName, event);
    switch (event) {
        case WIFI_EVENT_SCAN_DONE:
            printf("HalCallbackEvent WIFI_EVENT_SCAN_DONE Process\n");
            break;
        case WIFI_EVENT_SCAN_RESULT:
            ParseScanResult((WifiScanResult *)respData);
            break;
        default:
            break;
    }
    return HDF_SUCCESS;
}

/**
 * @tc.name: WifiHalCreateAndDestroyFeature001
 * @tc.desc: Wifi hal create and destroy feature function test
 * @tc.type: FUNC
 */
HWTEST_F(WifiHalTest, WifiHalCreateAndDestroyFeature001, Function | MediumTest | Level1)
{
    int ret;
    struct IWiFiAp *apFeature = nullptr;

    ret = g_wifi->createFeature(PROTOCOL_80211_IFTYPE_AP, (struct IWiFiBaseFeature **)&apFeature);
    EXPECT_EQ(HDF_SUCCESS, ret);
    EXPECT_NE(nullptr, apFeature);

    ret = g_wifi->destroyFeature(nullptr);
    EXPECT_NE(HDF_SUCCESS, ret);
    ret = g_wifi->destroyFeature((struct IWiFiBaseFeature *)apFeature);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.name: WifiHalCreateAndDestroyFeature002
 * @tc.desc: Wifi hal create and destroy feature function test
 * @tc.type: FUNC
 */
HWTEST_F(WifiHalTest, WifiHalCreateAndDestroyFeature002, Function | MediumTest | Level1)
{
    int ret;
    struct IWiFiSta *staFeature = nullptr;

    ret = g_wifi->createFeature(PROTOCOL_80211_IFTYPE_STATION, (struct IWiFiBaseFeature **)&staFeature);
    EXPECT_EQ(HDF_SUCCESS, ret);
    EXPECT_NE(nullptr, staFeature);

    ret = g_wifi->destroyFeature(nullptr);
    EXPECT_NE(HDF_SUCCESS, ret);
    ret = g_wifi->destroyFeature((struct IWiFiBaseFeature *)staFeature);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.name: WifiHalGetFeatureByIfName001
 * @tc.desc: Wifi hal get feature by ifname function test
 * @tc.type: FUNC
 */
HWTEST_F(WifiHalTest, WifiHalGetFeatureByIfName001, Function | MediumTest | Level1)
{
    int ret;
    struct IWiFiAp *apFeature = nullptr;
    struct IWiFiAp *apFeatureGet = nullptr;
    const char *ifName0 = "wlanTest";
    const char *ifName1 = "wlan0";

    ret = g_wifi->createFeature(PROTOCOL_80211_IFTYPE_AP, (struct IWiFiBaseFeature **)&apFeature);
    EXPECT_EQ(HDF_SUCCESS, ret);
    EXPECT_NE(nullptr, apFeature);
    ret = g_wifi->getFeatureByIfName(ifName0, (struct IWiFiBaseFeature **)&apFeatureGet);
    EXPECT_NE(HDF_SUCCESS, ret);
    ret = g_wifi->getFeatureByIfName(ifName1, (struct IWiFiBaseFeature **)&apFeatureGet);
    EXPECT_EQ(HDF_SUCCESS, ret);
    EXPECT_NE(nullptr, apFeatureGet);

    ret = g_wifi->destroyFeature((struct IWiFiBaseFeature *)apFeature);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.name: WifiHalRegisterEventCallback001
 * @tc.desc: Wifi hal register callback function test
 * @tc.type: FUNC
 */
HWTEST_F(WifiHalTest, WifiHalRegisterEventCallback001, Function | MediumTest | Level1)
{
    int ret;

    ret = g_wifi->registerEventCallback(HalCallbackEvent, "wlan0");
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.name: WifiHalUnRegisterEventCallback001
 * @tc.desc: Wifi hal unregister callback function test
 * @tc.type: FUNC
 */
HWTEST_F(WifiHalTest, WifiHalUnRegisterEventCallback001, Function | MediumTest | Level1)
{
    int ret;

    ret = g_wifi->unregisterEventCallback(HalCallbackEvent, "wlan0");
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.name: WifiHalGetNetworkIfaceName001
 * @tc.desc: Wifi hal get network iface name function test
 * @tc.type: FUNC
 */
HWTEST_F(WifiHalTest, WifiHalGetNetworkIfaceName001, Function | MediumTest | Level1)
{
    int ret;
    struct IWiFiAp *apFeature = nullptr;

    ret = g_wifi->createFeature(PROTOCOL_80211_IFTYPE_AP, (struct IWiFiBaseFeature **)&apFeature);
    EXPECT_EQ(HDF_SUCCESS, ret);
    EXPECT_NE(nullptr, apFeature);
    const char *ifName = apFeature->baseFeature.getNetworkIfaceName((const struct IWiFiBaseFeature *)apFeature);
    EXPECT_NE(nullptr, ifName);
    ret = strcmp(ifName, "wlan0");
    EXPECT_EQ(0, ret);

    ret = g_wifi->destroyFeature((struct IWiFiBaseFeature *)apFeature);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.name: WifiHalGetGetFeatureType001
 * @tc.desc: Wifi hal get feature type function test
 * @tc.type: FUNC
 */
HWTEST_F(WifiHalTest, WifiHalGetGetFeatureType001, Function | MediumTest | Level1)
{
    int ret;
    struct IWiFiAp *apFeature = nullptr;
    int32_t type;

    ret = g_wifi->createFeature(PROTOCOL_80211_IFTYPE_AP, (struct IWiFiBaseFeature **)&apFeature);
    EXPECT_EQ(HDF_SUCCESS, ret);
    EXPECT_NE(nullptr, apFeature);
    type = apFeature->baseFeature.getFeatureType((struct IWiFiBaseFeature *)apFeature);
    EXPECT_EQ(PROTOCOL_80211_IFTYPE_AP, type);

    ret = g_wifi->destroyFeature((struct IWiFiBaseFeature *)apFeature);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.name: WifiHalSetMacAddress001
 * @tc.desc: Wifi hal set Mac address function test
 * @tc.type: FUNC
 */
HWTEST_F(WifiHalTest, WifiHalSetMacAddress001, Function | MediumTest | Level1)
{
    int ret;
    struct IWiFiAp *apFeature = nullptr;
    unsigned char errorMac[ETH_ADDR_LEN] = {0x11, 0x34, 0x56, 0x78, 0xab, 0xcd};
    unsigned char mac[ETH_ADDR_LEN] = {0x12, 0x34, 0x56, 0x78, 0xab, 0xcd};

    ret = g_wifi->createFeature(PROTOCOL_80211_IFTYPE_AP, (struct IWiFiBaseFeature **)&apFeature);
    EXPECT_EQ(HDF_SUCCESS, ret);
    EXPECT_NE(nullptr, apFeature);
    ret = apFeature->baseFeature.setMacAddress((struct IWiFiBaseFeature *)apFeature, nullptr, 0);
    EXPECT_NE(HDF_SUCCESS, ret);
    ret = apFeature->baseFeature.setMacAddress((struct IWiFiBaseFeature *)apFeature, errorMac, ETH_ADDR_LEN);
    EXPECT_NE(HDF_SUCCESS, ret);
    ret = apFeature->baseFeature.setMacAddress((struct IWiFiBaseFeature *)apFeature, mac, ETH_ADDR_LEN);
    EXPECT_EQ(HDF_SUCCESS, ret);

    ret = g_wifi->destroyFeature((struct IWiFiBaseFeature *)apFeature);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.name: WifiHalSetMacAddress002
 * @tc.desc: Wifi hal set Mac address function test
 * @tc.type: FUNC
 */
HWTEST_F(WifiHalTest, WifiHalSetMacAddress002, Function | MediumTest | Level1)
{
    int ret;
    struct IWiFiSta *staFeature = nullptr;
    unsigned char errorMac[ETH_ADDR_LEN] = {0};
    unsigned char mac[ETH_ADDR_LEN] = {0x12, 0x34, 0x56, 0x78, 0xab, 0xcd};

    ret = g_wifi->createFeature(PROTOCOL_80211_IFTYPE_STATION, (struct IWiFiBaseFeature **)&staFeature);
    EXPECT_EQ(HDF_SUCCESS, ret);
    EXPECT_NE(nullptr, staFeature);
    ret = staFeature->baseFeature.setMacAddress((struct IWiFiBaseFeature *)staFeature, nullptr, 0);
    EXPECT_NE(HDF_SUCCESS, ret);
    ret = staFeature->baseFeature.setMacAddress((struct IWiFiBaseFeature *)staFeature, errorMac, ETH_ADDR_LEN);
    EXPECT_NE(HDF_SUCCESS, ret);
    ret = staFeature->baseFeature.setMacAddress((struct IWiFiBaseFeature *)staFeature, mac, ETH_ADDR_LEN);
    EXPECT_EQ(HDF_SUCCESS, ret);

    ret = g_wifi->destroyFeature((struct IWiFiBaseFeature *)staFeature);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.name: WifiHalSetTxPower001
 * @tc.desc: Wifi hal set transmit power function test
 * @tc.type: FUNC
 */
HWTEST_F(WifiHalTest, WifiHalSetTxPower001, Function | MediumTest | Level1)
{
    int ret;
    struct IWiFiAp *apFeature = nullptr;

    ret = g_wifi->createFeature(PROTOCOL_80211_IFTYPE_AP, (struct IWiFiBaseFeature **)&apFeature);
    EXPECT_EQ(HDF_SUCCESS, ret);
    EXPECT_NE(nullptr, apFeature);
    ret = apFeature->baseFeature.setTxPower((struct IWiFiBaseFeature *)apFeature, 0);
    EXPECT_NE(HDF_SUCCESS, ret);
    ret = apFeature->baseFeature.setTxPower((struct IWiFiBaseFeature *)apFeature, WLAN_TX_POWER);
    EXPECT_EQ(HDF_SUCCESS, ret);

    ret = g_wifi->destroyFeature((struct IWiFiBaseFeature *)apFeature);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.name: WifiHalSetCountryCode001
 * @tc.desc: Wifi hal set country code function test
 * @tc.type: FUNC
 */
HWTEST_F(WifiHalTest, WifiHalSetCountryCode001, Function | MediumTest | Level1)
{
    int ret;
    struct IWiFiAp *apFeature = nullptr;

    ret = g_wifi->createFeature(PROTOCOL_80211_IFTYPE_AP, (struct IWiFiBaseFeature **)&apFeature);
    EXPECT_EQ(HDF_SUCCESS, ret);
    EXPECT_NE(nullptr, apFeature);
    ret = apFeature->setCountryCode(apFeature, nullptr, 0);
    EXPECT_NE(HDF_SUCCESS, ret);
    ret = apFeature->setCountryCode(apFeature, "CN", 2);
    EXPECT_EQ(HDF_SUCCESS, ret);

    ret = g_wifi->destroyFeature((struct IWiFiBaseFeature *)apFeature);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.name: WifiHalGetIfNamesByChipId001
 * @tc.desc: Obtain all ifNames and the number of the current chip
 * @tc.type: FUNC
 */
HWTEST_F(WifiHalTest, WifiHalGetIfNamesByChipId001, Function | MediumTest | Level1)
{
    int ret;
    struct IWiFiSta *staFeature = nullptr;
    char *ifNames = nullptr;
    unsigned int num = 0;
    unsigned char chipId = 0;
    uint8_t i;

    ret = g_wifi->createFeature(PROTOCOL_80211_IFTYPE_STATION, (struct IWiFiBaseFeature **)&staFeature);
    EXPECT_EQ(HDF_SUCCESS, ret);
    EXPECT_NE(nullptr, staFeature);
    ret = staFeature->baseFeature.getChipId((struct IWiFiBaseFeature *)staFeature, &chipId);
    ASSERT_TRUE(chipId <= WLAN_MAX_CHIPID && chipId >= WLAN_MIN_CHIPID);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = staFeature->baseFeature.getIfNamesByChipId(chipId, nullptr, nullptr);
    EXPECT_NE(HDF_SUCCESS, ret);
    ret = staFeature->baseFeature.getIfNamesByChipId(chipId, &ifNames, &num);
    EXPECT_NE(nullptr, ifNames);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ASSERT_TRUE(num <= IFNAME_MAX_NUM && num >= IFNAME_MIN_NUM);
    for (i = 0; i < num; i++) {
        EXPECT_EQ(0, strncmp("wlan", ifNames + i * MAX_IF_NAME_LENGTH, SIZE));
    }
    free(ifNames);

    ret = g_wifi->destroyFeature((struct IWiFiBaseFeature *)staFeature);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.name: WifiHalGetSupportFeature001
 * @tc.desc: Get supported features
 * @tc.type: FUNC
 */
HWTEST_F(WifiHalTest, WifiHalGetSupportFeature001, Function | MediumTest | Level1)
{
    int ret;
    uint8_t supportTest[PROTOCOL_80211_IFTYPE_NUM] = {0};
    uint8_t support[PROTOCOL_80211_IFTYPE_NUM + 1] = {0};

    ret = g_wifi->getSupportFeature(nullptr, 0);
    EXPECT_NE(HDF_SUCCESS, ret);
    ret = g_wifi->getSupportFeature(supportTest, PROTOCOL_80211_IFTYPE_NUM);
    EXPECT_NE(HDF_SUCCESS, ret);
    ret = g_wifi->getSupportFeature(support, PROTOCOL_80211_IFTYPE_NUM + 1);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.name: WifiHalGetSupportCombo001
 * @tc.desc: Get supported combo
 * @tc.type: FUNC
 */
HWTEST_F(WifiHalTest, WifiHalGetSupportCombo001, Function | MediumTest | Level1)
{
    int ret;
    uint8_t support[PROTOCOL_80211_IFTYPE_NUM + 1] = {0};
    uint64_t combo[DEFAULT_COMBO_SIZE] = {0};

    ret = g_wifi->getSupportFeature(support, PROTOCOL_80211_IFTYPE_NUM + 1);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = g_wifi->getSupportCombo(nullptr, 0);
    EXPECT_NE(HDF_SUCCESS, ret);
    ret = g_wifi->getSupportCombo(combo, DEFAULT_COMBO_SIZE);
    if (support[PROTOCOL_80211_IFTYPE_NUM] == 0) {
        EXPECT_EQ(HDF_ERR_NOT_SUPPORT, ret);
    } else {
        EXPECT_EQ(HDF_SUCCESS, ret);
    }
}

/**
 * @tc.name: WifiHalGetDeviceMacAddress001
 * @tc.desc: Get device Mac address for ap mode
 * @tc.type: FUNC
 * @tc.require: AR000F869G
 */
HWTEST_F(WifiHalTest, WifiHalGetDeviceMacAddress001, Function | MediumTest | Level1)
{
    int ret;
    struct IWiFiAp *apFeature = nullptr;
    unsigned char mac[ETH_ADDR_LEN] = {0};

    ret = g_wifi->createFeature(PROTOCOL_80211_IFTYPE_AP, (struct IWiFiBaseFeature **)&apFeature);
    EXPECT_EQ(HDF_SUCCESS, ret);
    EXPECT_NE(nullptr, apFeature);
    ret = apFeature->baseFeature.getDeviceMacAddress((struct IWiFiBaseFeature *)apFeature, nullptr, 0);
    EXPECT_NE(HDF_SUCCESS, ret);
    ret = apFeature->baseFeature.getDeviceMacAddress((struct IWiFiBaseFeature *)apFeature, mac, ETH_ADDR_LEN - 1);
    EXPECT_NE(HDF_SUCCESS, ret);
    ret = apFeature->baseFeature.getDeviceMacAddress((struct IWiFiBaseFeature *)apFeature, mac, ETH_ADDR_LEN);
    EXPECT_NE(HDF_FAILURE, ret);

    ret = g_wifi->destroyFeature((struct IWiFiBaseFeature *)apFeature);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.name: WifiHalGetDeviceMacAddress002
 * @tc.desc: Get device Mac address for sta mode
 * @tc.type: FUNC
 */
HWTEST_F(WifiHalTest, WifiHalGetDeviceMacAddress002, Function | MediumTest | Level1)
{
    int ret;
    struct IWiFiSta *staFeature = nullptr;
    unsigned char mac[ETH_ADDR_LEN] = {0};

    ret = g_wifi->createFeature(PROTOCOL_80211_IFTYPE_STATION, (struct IWiFiBaseFeature **)&staFeature);
    EXPECT_EQ(HDF_SUCCESS, ret);
    EXPECT_NE(nullptr, staFeature);
    ret = staFeature->baseFeature.getDeviceMacAddress((struct IWiFiBaseFeature *)staFeature, nullptr, 0);
    EXPECT_NE(HDF_SUCCESS, ret);
    ret = staFeature->baseFeature.getDeviceMacAddress((struct IWiFiBaseFeature *)staFeature, mac, ETH_ADDR_LEN - 1);
    EXPECT_NE(HDF_SUCCESS, ret);
    ret = staFeature->baseFeature.getDeviceMacAddress((struct IWiFiBaseFeature *)staFeature, mac, ETH_ADDR_LEN);
    EXPECT_NE(HDF_FAILURE, ret);

    ret = g_wifi->destroyFeature((struct IWiFiBaseFeature *)staFeature);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.name: WifiHaGetValidFreqsWithBand001
 * @tc.desc: Get available frequencies
 * @tc.type: FUNC
 */
HWTEST_F(WifiHalTest, WifiHaGetValidFreqsWithBand001, Function | MediumTest | Level1)
{
    int ret;
    struct IWiFiAp *apFeature = nullptr;
    int32_t freq[WLAN_FREQ_MAX_NUM] = {0};
    uint32_t num = 0;

    ret = g_wifi->createFeature(PROTOCOL_80211_IFTYPE_AP, (struct IWiFiBaseFeature **)&apFeature);
    EXPECT_EQ(HDF_SUCCESS, ret);
    EXPECT_NE(nullptr, apFeature);
    ret = apFeature->baseFeature.getValidFreqsWithBand((struct IWiFiBaseFeature *)apFeature,
    WLAN_BAND_2G, nullptr, 0, nullptr);
    EXPECT_NE(HDF_SUCCESS, ret);
    ret = apFeature->baseFeature.getValidFreqsWithBand((struct IWiFiBaseFeature *)apFeature,
    WLAN_BAND_2G, freq, WLAN_FREQ_MAX_NUM, &num);
    EXPECT_EQ(HDF_SUCCESS, ret);

    ret = g_wifi->destroyFeature((struct IWiFiBaseFeature *)apFeature);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.name: WifiHalGetAssociatedStas001
 * @tc.desc: Get asscociated STA info
 * @tc.type: FUNC
 */
HWTEST_F(WifiHalTest, WifiHalGetAsscociatedStas001, Function | MediumTest | Level1)
{
    int ret;
    struct IWiFiAp *apFeature = nullptr;
    struct StaInfo staInfo[WLAN_MAX_NUM_STA_WITH_AP] = {{0}};
    uint32_t num = 0;

    ret = g_wifi->createFeature(PROTOCOL_80211_IFTYPE_AP, (struct IWiFiBaseFeature **)&apFeature);
    EXPECT_EQ(HDF_SUCCESS, ret);
    EXPECT_NE(nullptr, apFeature);
    ret = apFeature->getAsscociatedStas(apFeature, nullptr, 0, nullptr);
    EXPECT_NE(HDF_SUCCESS, ret);
    ret = apFeature->getAsscociatedStas(apFeature, staInfo, WLAN_MAX_NUM_STA_WITH_AP, &num);
    EXPECT_EQ(HDF_SUCCESS, ret);

    ret = g_wifi->destroyFeature((struct IWiFiBaseFeature *)apFeature);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.name: WifiHalSetScanningMacAddress001
 * @tc.desc: Set Mac address scanning
 * @tc.type: FUNC
 */
HWTEST_F(WifiHalTest, WifiHalSetScanningMacAddress001, Function | MediumTest | Level1)
{
    int ret;
    struct IWiFiSta *staFeature = nullptr;
    unsigned char scanMac[ETH_ADDR_LEN] = {0x12, 0x34, 0x56, 0x78, 0xab, 0xcd};

    ret = g_wifi->createFeature(PROTOCOL_80211_IFTYPE_STATION, (struct IWiFiBaseFeature **)&staFeature);
    EXPECT_EQ(HDF_SUCCESS, ret);
    EXPECT_NE(nullptr, staFeature);
    ret = staFeature->setScanningMacAddress(staFeature, nullptr, 0);
    EXPECT_NE(HDF_SUCCESS, ret);
    ret = staFeature->setScanningMacAddress(staFeature, scanMac, ETH_ADDR_LEN);
    EXPECT_EQ(HDF_ERR_NOT_SUPPORT, ret);

    ret = g_wifi->destroyFeature((struct IWiFiBaseFeature *)staFeature);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.name: WifiHalGetNetdevInfo001
 * @tc.desc: Wifi hdi get netdev info function test
 * @tc.type: FUNC
 */
HWTEST_F(WifiHalTest, WifiHalGetNetdevInfo001, Function | MediumTest | Level1)
{
    int ret;
    struct NetDeviceInfoResult netDeviceInfoResult;

    (void)memset_s(&netDeviceInfoResult, sizeof(struct NetDeviceInfoResult), 0, sizeof(struct NetDeviceInfoResult));
    ret = g_wifi->getNetDevInfo(&netDeviceInfoResult);
    EXPECT_EQ(ret, HDF_SUCCESS);
}

/**
 * @tc.number: WifiHalResetDriver001
 * @tc.name: Reset the WiFi driver
 * @tc.size:Medium
 * @tc.level: Level 3
 */
HWTEST_F(WifiHalTest, WifiHalResetDriver001, Function | MediumTest | Level3)
{
    int ret;
    struct IWiFiSta *staFeature = nullptr;
    const char *ifName = "wlan0";
    unsigned char chipId = 0;

    ret = g_wifi->createFeature(PROTOCOL_80211_IFTYPE_STATION, (struct IWiFiBaseFeature **)&staFeature);
    EXPECT_EQ(HDF_SUCCESS, ret);
    EXPECT_NE(nullptr, staFeature);
    ret = staFeature->baseFeature.getChipId((struct IWiFiBaseFeature *)staFeature, &chipId);
    ASSERT_TRUE(chipId <= WLAN_MAX_CHIPID && chipId >= WLAN_MIN_CHIPID);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = g_wifi->resetDriver(chipId, ifName);
    EXPECT_EQ(HDF_SUCCESS, ret);
    printf("wait 15 for reset test now\n");
    sleep(RESET_TIME);
}

/**
 * @tc.name: WifiHalStartScan001
 * @tc.desc: start scan is successfully
 * @tc.type: FUNC
 */
HWTEST_F(WifiHalTest, WifiHalStartScan001, Function | MediumTest | Level3)
{
    int ret;
    struct IWiFiSta *staFeature = nullptr;
    const char *ifName = "wlan0";
    WifiScan scan = {0};

    ret = g_wifi->registerEventCallback(HalCallbackEvent, ifName);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = g_wifi->createFeature(PROTOCOL_80211_IFTYPE_STATION, (struct IWiFiBaseFeature **)&staFeature);
    EXPECT_EQ(HDF_SUCCESS, ret);
    EXPECT_NE(nullptr, staFeature);
    ret = staFeature->startScan(ifName, &scan);
    EXPECT_EQ(HDF_SUCCESS, ret);
    printf("wait 5 for Scan test now\n");
    sleep(SLEEP_TIME);
    ret = g_wifi->unregisterEventCallback(HalCallbackEvent, ifName);
    EXPECT_EQ(HDF_SUCCESS, ret);
}
};