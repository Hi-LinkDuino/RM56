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

#include "hdf_wifi_test.h"
#include <gtest/gtest.h>
#include <osal_mem.h>
#include "hdf_uhdf_test.h"
#include "wifi_driver_client.h"

using namespace testing::ext;

namespace ClientTest {
const uint32_t DEFAULT_COMBO_SIZE = 10;
class WifiClientTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void WifiClientTest::SetUpTestCase()
{
}

void WifiClientTest::TearDownTestCase()
{
}

void WifiClientTest::SetUp()
{
    WifiDriverClientInit();
}

void WifiClientTest::TearDown()
{
    WifiDriverClientDeinit();
}

/**
 * @tc.name: WifiClientSetCountryCode001
 * @tc.desc: Wifi client set country code function test
 * @tc.type: FUNC
 * @tc.require: AR000FRMJC
 */
HWTEST_F(WifiClientTest, WifiClientSetCountryCode001, TestSize.Level1)
{
    int ret;

    ret = WifiSetCountryCode("wlan0", "CN", 2);
    EXPECT_EQ(RET_CODE_SUCCESS, ret);
}

/**
 * @tc.name: WifiClientGetUsableNetworkInfo001
 * @tc.desc: Wifi client get usable networkInfo function test
 * @tc.type: FUNC
 * @tc.require: AR000FRMJC
 */
HWTEST_F(WifiClientTest, WifiClientGetUsableNetworkInfo001, TestSize.Level1)
{
    int ret;
    struct NetworkInfoResult networkInfo;

    ret = GetUsableNetworkInfo(&networkInfo);
    EXPECT_EQ(RET_CODE_SUCCESS, ret);
}

/**
 * @tc.name: WifiClientIsSupportCombo001
 * @tc.desc: Wifi client is support combo function test
 * @tc.type: FUNC
 * @tc.require: AR000FRMJC
 */
HWTEST_F(WifiClientTest, WifiClientIsSupportCombo001, TestSize.Level1)
{
    int ret;
    uint8_t isSupportCombo;

    ret = IsSupportCombo(&isSupportCombo);
    EXPECT_EQ(RET_CODE_SUCCESS, ret);
}

/**
 * @tc.name: WifiClientGetComboInfo001
 * @tc.desc: Wifi client get combo info function test
 * @tc.type: FUNC
 * @tc.require: AR000FRMJC
 */
HWTEST_F(WifiClientTest, WifiClientGetComboInfo001, TestSize.Level1)
{
    int ret;
    uint64_t comboInfo[DEFAULT_COMBO_SIZE] = {};

    ret = GetComboInfo(comboInfo, DEFAULT_COMBO_SIZE);
    EXPECT_EQ(RET_CODE_NOT_SUPPORT, ret);
}

/**
 * @tc.name: WifiClientSetMacAddr001
 * @tc.desc: Wifi client set mac addr function test
 * @tc.type: FUNC
 * @tc.require: AR000FRMJC
 */
HWTEST_F(WifiClientTest, WifiClientSetMacAddr001, TestSize.Level1)
{
    int ret;
    unsigned char mac[ETH_ADDR_LEN] = {0x12, 0x34, 0x56, 0x78, 0xab, 0xcd};

    ret = SetMacAddr("wlan0", mac, ETH_ADDR_LEN);
    EXPECT_EQ(RET_CODE_SUCCESS, ret);
}

/**
 * @tc.name: WifiClientGetDevMacAddr001
 * @tc.desc: Wifi client get mac addr function test
 * @tc.type: FUNC
 * @tc.require: AR000FRMJC
 */
HWTEST_F(WifiClientTest, WifiClientGetDevMacAddr001, TestSize.Level1)
{
    int ret;
    unsigned char mac[ETH_ADDR_LEN] = {};
    int32_t type = WIFI_IFTYPE_STATION;

    ret = GetDevMacAddr("wlan0", type, mac, ETH_ADDR_LEN);
    EXPECT_NE(RET_CODE_FAILURE, ret);
}

/**
 * @tc.name: WifiClientGetValidFreqByBand001
 * @tc.desc: Wifi client get valid freq function test
 * @tc.type: FUNC
 * @tc.require: AR000FRMJC
 */
HWTEST_F(WifiClientTest, WifiClientGetValidFreqByBand001, TestSize.Level1)
{
    int32_t ret;
    int32_t band = IEEE80211_BAND_2GHZ;
    struct FreqInfoResult result;
    uint32_t size = 14;
    uint32_t i;

    result.freqs = (int32_t *)OsalMemCalloc(35 * sizeof(int32_t));
    if (result.freqs == NULL) {
        printf("%s: OsalMemCalloc failed", __FUNCTION__);
        return;
    }

    result.txPower = (int32_t *)OsalMemCalloc(35 * sizeof(int32_t));
    if (result.txPower == NULL) {
        printf("%s: OsalMemCalloc failed", __FUNCTION__);
        OsalMemFree(result.freqs);
        return;
    }

    ret = GetValidFreqByBand("wlan0", band, &result, size);
    EXPECT_EQ(RET_CODE_SUCCESS, ret);
    if (ret == RET_CODE_SUCCESS) {
        printf("%s: num = %u\n", __func__, result.nums);
        for (i = 0; i < result.nums; i++) {
            printf("%s: freq[%d] = %d\n", __func__, i, result.freqs[i]);
        }
    }

    OsalMemFree(result.txPower);
    OsalMemFree(result.freqs);
}

/**
 * @tc.name: WifiClientSetTxPower001
 * @tc.desc: Wifi client set tx power function test
 * @tc.type: FUNC
 * @tc.require: AR000FRMJC
 */
HWTEST_F(WifiClientTest, WifiClientSetTxPower001, TestSize.Level1)
{
    int ret;
    int32_t power = 10;

    ret = SetTxPower("wlan0", power);
    EXPECT_EQ(RET_CODE_SUCCESS, ret);
}
};
