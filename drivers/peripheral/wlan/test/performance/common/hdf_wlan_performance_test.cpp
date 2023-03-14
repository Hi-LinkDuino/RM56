/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include "hdf_base.h"
#include "hdf_uhdf_test.h"
#include "hdf_sbuf.h"
#include "wifi_hal.h"
#include "wifi_hal_ap_feature.h"
#include "wifi_hal_base_feature.h"
#include "wifi_hal_sta_feature.h"

using namespace testing::ext;

namespace HalTest {
struct IWiFi *g_wifi = nullptr;
const int32_t USEC_TIME = 1000000;
const int32_t MSEC_TIME = 1000;
const int32_t COMMON_TIME = 18000;
const int32_t MEDIUM_TIME = 20000;
const int32_t LONG_TIME = 200000;
const int32_t WLAN_BAND_2G = 0;
const int32_t WLAN_FREQ_MAX_NUM = 14;
const int32_t WLAN_MAX_NUM_STA_WITH_AP = 4;
const uint32_t DEFAULT_COMBO_SIZE = 10;
const uint32_t RESET_TIME = 20;
const uint32_t WLAN_MIN_CHIPID = 0;
const uint32_t WLAN_MAX_CHIPID = 2;

static int32_t g_resetStatus = -1;

class HdfWlanPerformanceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void HdfWlanPerformanceTest::SetUpTestCase()
{
    int ret;

    ret = WifiConstruct(&g_wifi);
    ASSERT_EQ(HDF_SUCCESS, ret);
}

void HdfWlanPerformanceTest::TearDownTestCase()
{
    int ret;

    ret = WifiDestruct(&g_wifi);
    ASSERT_EQ(HDF_SUCCESS, ret);
}

void HdfWlanPerformanceTest::SetUp()
{
    int ret;

    ret = g_wifi->start(nullptr);
    ASSERT_EQ(HDF_ERR_INVALID_PARAM, ret);
    ret = g_wifi->start(g_wifi);
    ASSERT_EQ(HDF_SUCCESS, ret);
}

void HdfWlanPerformanceTest::TearDown()
{
    int ret;

    ret = g_wifi->stop(nullptr);
    ASSERT_EQ(HDF_ERR_INVALID_PARAM, ret);
    ret = g_wifi->stop(g_wifi);
    ASSERT_EQ(HDF_SUCCESS, ret);
}

static int32_t HalResetCallbackEvent(uint32_t event, void *data, const char *ifName)
{
    (void)event;
    (void)ifName;
    int *resetStatus = nullptr;
    resetStatus = (int *)data;
    printf("HalResetCallbackEvent: receive resetStatus=%d \n", *resetStatus);
    g_resetStatus = *resetStatus;
    return HDF_SUCCESS;
}

/**
 * @tc.name: WifiHalGetSupportFeature001
 * @tc.desc: Get supported features
 * @tc.type: FUNC
 * @tc.require: AR000F869J
 */
HWTEST_F(HdfWlanPerformanceTest, WifiHalGetSupportFeature001, TestSize.Level1)
{
    int ret;
    uint8_t supportTest[PROTOCOL_80211_IFTYPE_NUM] = {0};
    uint8_t support[PROTOCOL_80211_IFTYPE_NUM + 1] = {0};
    struct timespec tv1 = (struct timespec){0};
    struct timespec tv2 = (struct timespec){0};
    int timeUsed = 0;

    ret = g_wifi->getSupportFeature(nullptr, 0);
    EXPECT_NE(HDF_SUCCESS, ret);
    ret = g_wifi->getSupportFeature(supportTest, PROTOCOL_80211_IFTYPE_NUM);
    EXPECT_NE(HDF_SUCCESS, ret);
    clock_gettime(CLOCK_REALTIME, &tv1);
    ret = g_wifi->getSupportFeature(support, PROTOCOL_80211_IFTYPE_NUM + 1);
    clock_gettime(CLOCK_REALTIME, &tv2);
    timeUsed = ((tv2.tv_sec * USEC_TIME + tv2.tv_nsec / MSEC_TIME) -
        (tv1.tv_sec * USEC_TIME + tv1.tv_nsec / MSEC_TIME));
    EXPECT_GE(COMMON_TIME, timeUsed);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.name: WifiHalGetSupportCombo001
 * @tc.desc: Get supported combo
 * @tc.type: FUNC
 * @tc.require: AR000F869J
 */
HWTEST_F(HdfWlanPerformanceTest, WifiHalGetSupportCombo001, TestSize.Level1)
{
    int ret;
    uint8_t support[PROTOCOL_80211_IFTYPE_NUM + 1] = {0};
    uint64_t combo[DEFAULT_COMBO_SIZE] = {0};
    struct timespec tv1 = (struct timespec){0};
    struct timespec tv2 = (struct timespec){0};
    int timeUsed = 0;

    ret = g_wifi->getSupportFeature(support, PROTOCOL_80211_IFTYPE_NUM + 1);
    EXPECT_EQ(HDF_SUCCESS, ret);
    ret = g_wifi->getSupportCombo(nullptr, 0);
    EXPECT_NE(HDF_SUCCESS, ret);
    clock_gettime(CLOCK_REALTIME, &tv1);
    ret = g_wifi->getSupportCombo(combo, DEFAULT_COMBO_SIZE);
    clock_gettime(CLOCK_REALTIME, &tv2);
    timeUsed = ((tv2.tv_sec * USEC_TIME + tv2.tv_nsec / MSEC_TIME) -
        (tv1.tv_sec * USEC_TIME + tv1.tv_nsec / MSEC_TIME));
    EXPECT_GE(COMMON_TIME, timeUsed);
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
HWTEST_F(HdfWlanPerformanceTest, WifiHalGetDeviceMacAddress001, TestSize.Level1)
{
    int ret;
    struct IWiFiAp *apFeature = nullptr;
    unsigned char mac[ETH_ADDR_LEN] = {0};
    struct timespec tv1 = (struct timespec){0};
    struct timespec tv2 = (struct timespec){0};
    int timeUsed = 0;

    ret = g_wifi->createFeature(PROTOCOL_80211_IFTYPE_AP, (struct IWiFiBaseFeature **)&apFeature);
    EXPECT_EQ(HDF_SUCCESS, ret);
    EXPECT_NE(nullptr, apFeature);
    ret = apFeature->baseFeature.getDeviceMacAddress((struct IWiFiBaseFeature *)apFeature, nullptr, 0);
    EXPECT_NE(HDF_SUCCESS, ret);
    ret = apFeature->baseFeature.getDeviceMacAddress((struct IWiFiBaseFeature *)apFeature, mac, ETH_ADDR_LEN - 1);
    EXPECT_NE(HDF_SUCCESS, ret);
    clock_gettime(CLOCK_REALTIME, &tv1);
    ret = apFeature->baseFeature.getDeviceMacAddress((struct IWiFiBaseFeature *)apFeature, mac, ETH_ADDR_LEN);
    clock_gettime(CLOCK_REALTIME, &tv2);
    timeUsed = ((tv2.tv_sec * USEC_TIME + tv2.tv_nsec / MSEC_TIME) -
        (tv1.tv_sec * USEC_TIME + tv1.tv_nsec / MSEC_TIME));
    EXPECT_GE(LONG_TIME, timeUsed);
    EXPECT_NE(HDF_FAILURE, ret);

    ret = g_wifi->destroyFeature((struct IWiFiBaseFeature *)apFeature);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.name: WifiHalGetDeviceMacAddress002
 * @tc.desc: Get device Mac address for sta mode
 * @tc.type: FUNC
 * @tc.require: AR000F869E
 */
HWTEST_F(HdfWlanPerformanceTest, WifiHalGetDeviceMacAddress002, TestSize.Level1)
{
    int ret;
    struct IWiFiSta *staFeature = nullptr;
    unsigned char mac[ETH_ADDR_LEN] = {0};
    struct timespec tv1 = (struct timespec){0};
    struct timespec tv2 = (struct timespec){0};
    int timeUsed = 0;

    ret = g_wifi->createFeature(PROTOCOL_80211_IFTYPE_STATION, (struct IWiFiBaseFeature **)&staFeature);
    EXPECT_EQ(HDF_SUCCESS, ret);
    EXPECT_NE(nullptr, staFeature);
    ret = staFeature->baseFeature.getDeviceMacAddress((struct IWiFiBaseFeature *)staFeature, nullptr, 0);
    EXPECT_NE(HDF_SUCCESS, ret);
    ret = staFeature->baseFeature.getDeviceMacAddress((struct IWiFiBaseFeature *)staFeature, mac, ETH_ADDR_LEN - 1);
    EXPECT_NE(HDF_SUCCESS, ret);
    clock_gettime(CLOCK_REALTIME, &tv1);
    ret = staFeature->baseFeature.getDeviceMacAddress((struct IWiFiBaseFeature *)staFeature, mac, ETH_ADDR_LEN);
    clock_gettime(CLOCK_REALTIME, &tv2);
    timeUsed = ((tv2.tv_sec * USEC_TIME + tv2.tv_nsec / MSEC_TIME) -
        (tv1.tv_sec * USEC_TIME + tv1.tv_nsec / MSEC_TIME));
    EXPECT_GE(LONG_TIME, timeUsed);
    EXPECT_NE(HDF_FAILURE, ret);

    ret = g_wifi->destroyFeature((struct IWiFiBaseFeature *)staFeature);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.name: WifiHaGetValidFreqsWithBand001
 * @tc.desc: Get available frequencies
 * @tc.type: FUNC
 * @tc.require: AR000F869G
 */
HWTEST_F(HdfWlanPerformanceTest, WifiHaGetValidFreqsWithBand001, TestSize.Level1)
{
    int ret;
    struct IWiFiAp *apFeature = nullptr;
    int32_t freq[WLAN_FREQ_MAX_NUM] = {0};
    uint32_t num = 0;
    struct timespec tv1 = (struct timespec){0};
    struct timespec tv2 = (struct timespec){0};
    int timeUsed = 0;

    ret = g_wifi->createFeature(PROTOCOL_80211_IFTYPE_AP, (struct IWiFiBaseFeature **)&apFeature);
    EXPECT_EQ(HDF_SUCCESS, ret);
    EXPECT_NE(nullptr, apFeature);
    ret = apFeature->baseFeature.getValidFreqsWithBand((struct IWiFiBaseFeature *)apFeature,
    WLAN_BAND_2G, nullptr, 0, nullptr);
    EXPECT_NE(HDF_SUCCESS, ret);
    clock_gettime(CLOCK_REALTIME, &tv1);
    ret = apFeature->baseFeature.getValidFreqsWithBand((struct IWiFiBaseFeature *)apFeature,
    WLAN_BAND_2G, freq, WLAN_FREQ_MAX_NUM, &num);
    clock_gettime(CLOCK_REALTIME, &tv2);
    timeUsed = ((tv2.tv_sec * USEC_TIME + tv2.tv_nsec / MSEC_TIME) -
        (tv1.tv_sec * USEC_TIME + tv1.tv_nsec / MSEC_TIME));
    EXPECT_GE(COMMON_TIME, timeUsed);
    EXPECT_EQ(HDF_SUCCESS, ret);

    ret = g_wifi->destroyFeature((struct IWiFiBaseFeature *)apFeature);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.name: WifiHalGetAssociatedStas001
 * @tc.desc: Get asscociated STA info
 * @tc.type: FUNC
 * @tc.require: AR000F869K
 */
HWTEST_F(HdfWlanPerformanceTest, WifiHalGetAsscociatedStas001, TestSize.Level1)
{
    int ret;
    struct IWiFiAp *apFeature = nullptr;
    struct StaInfo staInfo[WLAN_MAX_NUM_STA_WITH_AP] = {{0}};
    uint32_t num = 0;
    struct timespec tv1 = (struct timespec){0};
    struct timespec tv2 = (struct timespec){0};
    int timeUsed = 0;

    ret = g_wifi->createFeature(PROTOCOL_80211_IFTYPE_AP, (struct IWiFiBaseFeature **)&apFeature);
    EXPECT_EQ(HDF_SUCCESS, ret);
    EXPECT_NE(nullptr, apFeature);
    ret = apFeature->getAsscociatedStas(apFeature, nullptr, 0, nullptr);
    EXPECT_NE(HDF_SUCCESS, ret);
    clock_gettime(CLOCK_REALTIME, &tv1);
    ret = apFeature->getAsscociatedStas(apFeature, staInfo, WLAN_MAX_NUM_STA_WITH_AP, &num);
    clock_gettime(CLOCK_REALTIME, &tv2);
    timeUsed = ((tv2.tv_sec * USEC_TIME + tv2.tv_nsec / MSEC_TIME) -
        (tv1.tv_sec * USEC_TIME + tv1.tv_nsec / MSEC_TIME));
    EXPECT_GE(COMMON_TIME, timeUsed);
    EXPECT_EQ(HDF_SUCCESS, ret);

    ret = g_wifi->destroyFeature((struct IWiFiBaseFeature *)apFeature);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.name: WifiHalSetScanningMacAddress001
 * @tc.desc: Set Mac address scanning
 * @tc.type: FUNC
 * @tc.require: AR000F869K
 */
HWTEST_F(HdfWlanPerformanceTest, WifiHalSetScanningMacAddress001, TestSize.Level1)
{
    int ret;
    struct IWiFiSta *staFeature = nullptr;
    unsigned char scanMac[ETH_ADDR_LEN] = {0x12, 0x34, 0x56, 0x78, 0xab, 0xcd};
    struct timespec tv1 = (struct timespec){0};
    struct timespec tv2 = (struct timespec){0};
    int timeUsed = 0;

    ret = g_wifi->createFeature(PROTOCOL_80211_IFTYPE_STATION, (struct IWiFiBaseFeature **)&staFeature);
    EXPECT_EQ(HDF_SUCCESS, ret);
    EXPECT_NE(nullptr, staFeature);
    ret = staFeature->setScanningMacAddress(staFeature, nullptr, 0);
    EXPECT_NE(HDF_SUCCESS, ret);
    clock_gettime(CLOCK_REALTIME, &tv1);
    ret = staFeature->setScanningMacAddress(staFeature, scanMac, ETH_ADDR_LEN);
    clock_gettime(CLOCK_REALTIME, &tv2);
    timeUsed = ((tv2.tv_sec * USEC_TIME + tv2.tv_nsec / MSEC_TIME) -
        (tv1.tv_sec * USEC_TIME + tv1.tv_nsec / MSEC_TIME));
    EXPECT_GE(LONG_TIME, timeUsed);
    EXPECT_NE(HDF_FAILURE, ret);

    ret = g_wifi->destroyFeature((struct IWiFiBaseFeature *)staFeature);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.name: WifiHalGetChipId001
 * @tc.desc: Obtain the chip ID
 * @tc.type: FUNC
 * @tc.require: AR000F869G
 */
HWTEST_F(HdfWlanPerformanceTest, WifiHalGetChipId001, TestSize.Level1)
{
    int ret;
    struct IWiFiSta *staFeature = nullptr;
    unsigned char chipId = 0;
    struct timespec tv1 = (struct timespec){0};
    struct timespec tv2 = (struct timespec){0};
    int timeUsed = 0;

    ret = g_wifi->createFeature(PROTOCOL_80211_IFTYPE_STATION, (struct IWiFiBaseFeature **)&staFeature);
    EXPECT_EQ(HDF_SUCCESS, ret);
    EXPECT_NE(nullptr, staFeature);
    ret = staFeature->baseFeature.getChipId((struct IWiFiBaseFeature *)staFeature, nullptr);
    EXPECT_NE(HDF_SUCCESS, ret);
    clock_gettime(CLOCK_REALTIME, &tv1);
    ret = staFeature->baseFeature.getChipId((struct IWiFiBaseFeature *)staFeature, &chipId);
    clock_gettime(CLOCK_REALTIME, &tv2);
    timeUsed = ((tv2.tv_sec * USEC_TIME + tv2.tv_nsec / MSEC_TIME) -
        (tv1.tv_sec * USEC_TIME + tv1.tv_nsec / MSEC_TIME));
    EXPECT_GE(MEDIUM_TIME, timeUsed);
    ASSERT_TRUE(chipId <= WLAN_MAX_CHIPID && chipId >= WLAN_MIN_CHIPID);
    EXPECT_EQ(HDF_SUCCESS, ret);

    ret = g_wifi->destroyFeature((struct IWiFiBaseFeature *)staFeature);
    EXPECT_EQ(HDF_SUCCESS, ret);
}

/**
 * @tc.name: WifiHalRegisterEventCallback001
 * @tc.desc: Wifi hal register callback function test
 * @tc.type: FUNC
 * @tc.require: AR000F869G
 */
HWTEST_F(HdfWlanPerformanceTest, WifiHalRegisterEventCallback001, TestSize.Level1)
{
    int ret;

    ret = g_wifi->registerEventCallback(HalResetCallbackEvent, "wlan0");
    sleep(RESET_TIME);
    EXPECT_EQ(HDF_SUCCESS, ret);
}
};
