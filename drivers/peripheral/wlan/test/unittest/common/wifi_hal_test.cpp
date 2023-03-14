/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>
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

/**
 * @tc.name: WifiHalCreateAndDestroyFeature001
 * @tc.desc: Wifi hal create and destroy feature function test
 * @tc.type: FUNC
 * @tc.require: AR000F869G
 */
HWTEST_F(WifiHalTest, WifiHalCreateAndDestroyFeature001, TestSize.Level1)
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
 * @tc.require: AR000F869E
 */
HWTEST_F(WifiHalTest, WifiHalCreateAndDestroyFeature002, TestSize.Level1)
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
 * @tc.require: AR000F869G
 */
HWTEST_F(WifiHalTest, WifiHalGetFeatureByIfName001, TestSize.Level1)
{
    int ret;
    struct IWiFiAp *apFeature = nullptr;
    struct IWiFiAp *apFeatureGet = nullptr;
    const char *ifName0 = "wlanTest";

    ret = g_wifi->createFeature(PROTOCOL_80211_IFTYPE_AP, (struct IWiFiBaseFeature **)&apFeature);
    EXPECT_EQ(HDF_SUCCESS, ret);
    EXPECT_NE(nullptr, apFeature);
    ret = g_wifi->getFeatureByIfName(ifName0, (struct IWiFiBaseFeature **)&apFeatureGet);
    EXPECT_NE(HDF_SUCCESS, ret);
    ret = g_wifi->getFeatureByIfName(apFeature->baseFeature.ifName, (struct IWiFiBaseFeature **)&apFeatureGet);
    EXPECT_EQ(HDF_SUCCESS, ret);
    EXPECT_NE(nullptr, apFeatureGet);

    ret = g_wifi->destroyFeature((struct IWiFiBaseFeature *)apFeature);
    EXPECT_EQ(HDF_SUCCESS, ret);
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
 * @tc.name: WifiHalRegisterEventCallback001
 * @tc.desc: Wifi hal register callback function test
 * @tc.type: FUNC
 * @tc.require: AR000F869G
 */
HWTEST_F(WifiHalTest, WifiHalRegisterEventCallback001, TestSize.Level1)
{
    int ret;

    ret = g_wifi->registerEventCallback(HalCallbackEvent, "wlan0");
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.name: WifiHalUnRegisterEventCallback001
 * @tc.desc: Wifi hal unregister callback function test
 * @tc.type: FUNC
 * @tc.require: AR000F869G
 */
HWTEST_F(WifiHalTest, WifiHalUnRegisterEventCallback001, TestSize.Level1)
{
    int ret;

    ret = g_wifi->unregisterEventCallback(HalCallbackEvent, "wlan0");
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.name: WifiHalGetNetworkIfaceName001
 * @tc.desc: Wifi hal get network iface name function test
 * @tc.type: FUNC
 * @tc.require: AR000F869G
 */
HWTEST_F(WifiHalTest, WifiHalGetNetworkIfaceName001, TestSize.Level1)
{
    int ret;
    struct IWiFiAp *apFeature = nullptr;

    ret = g_wifi->createFeature(PROTOCOL_80211_IFTYPE_AP, (struct IWiFiBaseFeature **)&apFeature);
    EXPECT_EQ(HDF_SUCCESS, ret);
    EXPECT_NE(nullptr, apFeature);
    const char *ifName = apFeature->baseFeature.getNetworkIfaceName((const struct IWiFiBaseFeature *)apFeature);
    EXPECT_NE(nullptr, ifName);

    if (strncmp(ifName, "wlan", 4) == 0 || strncmp(ifName, "nan", 3) == 0 || strncmp(ifName, "p2p", 3) == 0) {
        ret = 0;
    } else {
        ret = -1;
    }
    EXPECT_EQ(0, ret);

    ret = g_wifi->destroyFeature((struct IWiFiBaseFeature *)apFeature);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.name: WifiHalGetGetFeatureType001
 * @tc.desc: Wifi hal get feature type function test
 * @tc.type: FUNC
 * @tc.require: AR000F869G
 */
HWTEST_F(WifiHalTest, WifiHalGetGetFeatureType001, TestSize.Level1)
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
 * @tc.require: AR000F869G
 */
HWTEST_F(WifiHalTest, WifiHalSetMacAddress001, TestSize.Level1)
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
 * @tc.require: AR000F869E
 */
HWTEST_F(WifiHalTest, WifiHalSetMacAddress002, TestSize.Level1)
{
    int ret;
    struct IWiFiSta *staFeature = nullptr;
    unsigned char mac[ETH_ADDR_LEN] = {0x12, 0x34, 0x56, 0x78, 0xab, 0xcd};

    ret = g_wifi->createFeature(PROTOCOL_80211_IFTYPE_STATION, (struct IWiFiBaseFeature **)&staFeature);
    EXPECT_EQ(HDF_SUCCESS, ret);
    EXPECT_NE(nullptr, staFeature);
    ret = staFeature->baseFeature.setMacAddress((struct IWiFiBaseFeature *)staFeature, nullptr, 0);
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
 * @tc.require: AR000F869G
 */
HWTEST_F(WifiHalTest, WifiHalSetTxPower001, TestSize.Level1)
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
 * @tc.require: AR000F869K
 */
HWTEST_F(WifiHalTest, WifiHalSetCountryCode001, TestSize.Level1)
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
 * @tc.require: AR000F869G
 */
HWTEST_F(WifiHalTest, WifiHalGetIfNamesByChipId001, TestSize.Level1)
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

HWTEST_F(WifiHalTest, GetNetDevInfo001, TestSize.Level1)
{
    int ret;
    struct IWiFiAp *apFeature = nullptr;
    struct NetDeviceInfoResult netDeviceInfoResult;

    ret = g_wifi->createFeature(PROTOCOL_80211_IFTYPE_AP, (struct IWiFiBaseFeature **)&apFeature);
    EXPECT_EQ(HDF_SUCCESS, ret);
    EXPECT_NE(nullptr, apFeature);
    ret = g_wifi->getNetDevInfo(nullptr);
    EXPECT_NE(HDF_SUCCESS, ret);
    ret = g_wifi->getNetDevInfo(&netDeviceInfoResult);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = g_wifi->destroyFeature((struct IWiFiBaseFeature *)apFeature);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

HWTEST_F(WifiHalTest, GetNetDevInfo002, TestSize.Level1)
{
    int ret;
    struct IWiFiSta *staFeature = nullptr;
    struct NetDeviceInfoResult netDeviceInfoResult;

    ret = g_wifi->createFeature(PROTOCOL_80211_IFTYPE_STATION, (struct IWiFiBaseFeature **)&staFeature);
    EXPECT_EQ(HDF_SUCCESS, ret);
    EXPECT_NE(nullptr, staFeature);
    ret = g_wifi->getNetDevInfo(nullptr);
    EXPECT_NE(HDF_SUCCESS, ret);
    ret = g_wifi->getNetDevInfo(&netDeviceInfoResult);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = g_wifi->destroyFeature((struct IWiFiBaseFeature *)staFeature);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

HWTEST_F(WifiHalTest, WifiHalStartScan001, TestSize.Level1)
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
    sleep(10);
}
};
