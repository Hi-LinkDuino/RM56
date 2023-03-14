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

#include "wlan_hdi_service_stub.h"
#include "wlan_hal_c_proxy.h"
#include <gtest/gtest.h>
#include <servmgr_hdi.h>

#define HDF_LOG_TAG   service_manager_test
using namespace testing::ext;

namespace HdiTest {
const int32_t WLAN_FREQ_MAX_NUM = 35;
const int32_t WLAN_TX_POWER = 160;
const int32_t DEFAULT_COMBO_SIZE = 6;
const int32_t WLAN_MAX_NUM_STA_WITH_AP = 4;
const uint32_t RESET_TIME = 20;

const char *WLAN_SERVICE_NAME = "wlan_hal_c_service";

class HdfWifiServiceCTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

static struct IWifiInterface *g_wlanObj = nullptr;
static int32_t g_resetStatus = -1;

void HdfWifiServiceCTest::SetUpTestCase()
{
    g_wlanObj = HdIWifiInterfaceGet(WLAN_SERVICE_NAME);
    ASSERT_TRUE(g_wlanObj != nullptr);
    int32_t rc = g_wlanObj->construct(g_wlanObj);
    ASSERT_EQ(rc, HDF_SUCCESS);
}

void HdfWifiServiceCTest::TearDownTestCase()
{
    int32_t rc = g_wlanObj->destruct(g_wlanObj);
    ASSERT_EQ(rc, HDF_SUCCESS);
    HdIWifiInterfaceRelease(g_wlanObj);
}

void HdfWifiServiceCTest::SetUp()
{
    int32_t rc = g_wlanObj->start(g_wlanObj);
    ASSERT_EQ(rc, HDF_SUCCESS);
}

void HdfWifiServiceCTest::TearDown()
{
    int32_t rc = g_wlanObj->stop(g_wlanObj);
    ASSERT_EQ(rc, HDF_SUCCESS);
}

static void HdiProcessScanResult(struct HdfSBuf *dataBuf)
{
    WifiScanResult *scanResult = nullptr;
    uint32_t dataSize = 0;

    if (!HdfSbufReadBuffer(dataBuf, (const void **)(&scanResult), &dataSize) || dataSize != sizeof(WifiScanResult)) {
        HDF_LOGE("%s: HdfSbufReadBuffer scanResult failed!", __func__);
        return;
    }
    printf("HdiProcessScanResult: flags=%d, caps=%d, freq=%d, beaconInt=%d,\n",
        scanResult->flags, scanResult->caps, scanResult->freq, scanResult->beaconInt);
    printf("HdiProcessScanResult: qual=%d, beaconIeLen=%d, level=%d, age=%d, ieLen=%d,\n",
        scanResult->qual, scanResult->beaconIeLen, scanResult->level, scanResult->age, scanResult->ieLen);
}

static int32_t HalResetCallbackEvent(uint32_t eventId, void *data, const char *ifName)
{
    (void)ifName;
    struct HdfSBuf *dataBuf = (struct HdfSBuf*)data;

    switch (eventId) {
        case WIFI_EVENT_RESET_DRIVER:
            if (!HdfSbufReadInt32(dataBuf, &g_resetStatus)) {
                HDF_LOGE("%s: feature->type write failed!", __func__);
            }
            printf("HalResetCallbackEvent: receive resetStatus=%d \n", g_resetStatus);
            break;
        case WIFI_EVENT_SCAN_RESULT:
            HdiProcessScanResult(dataBuf);
            break;
        default:
            break;
    }
    return HDF_SUCCESS;
}

/**
 * @tc.name: GetSupportFeatureComboTest_001
 * @tc.desc: Wifi hdi get support feature and combo function test
 * @tc.type: FUNC
 * @tc.require: AR000FRMJB
 */
HWTEST_F(HdfWifiServiceCTest, GetSupportFeatureComboTest_001, TestSize.Level1)
{
    uint8_t supType[PROTOCOL_80211_IFTYPE_NUM + 1] = {0};
    uint64_t combo[DEFAULT_COMBO_SIZE] = {0};

    int32_t rc = g_wlanObj->getSupportFeature(g_wlanObj, supType);
    ASSERT_EQ(rc, HDF_SUCCESS);
    rc = g_wlanObj->getSupportCombo(g_wlanObj, combo);
    ASSERT_NE(rc, HDF_FAILURE);
}

/**
 * @tc.name: CreateFeatureTest_002
 * @tc.desc: Wifi hdi create feature function test
 * @tc.type: FUNC
 * @tc.require: AR000FRMJB
 */
HWTEST_F(HdfWifiServiceCTest, CreateFeatureTest_002, TestSize.Level1)
{
    struct WlanFeatureInfo *ifeature = nullptr;
    const int32_t wlan_type = PROTOCOL_80211_IFTYPE_AP;

    int32_t rc = g_wlanObj->createFeature(g_wlanObj, wlan_type, (struct WlanFeatureInfo **)&ifeature);
    ASSERT_EQ(rc, HDF_SUCCESS);
    printf("ifname = %s\n", ifeature->ifName);
    printf("type = %d\n", ifeature->wlanType);
    rc = g_wlanObj->destroyFeature(g_wlanObj, (struct WlanFeatureInfo *)ifeature);
    ASSERT_EQ(rc, HDF_SUCCESS);
}

/**
 * @tc.name: GetFeatureByIfNameTest_003
 * @tc.desc: Wifi hdi get feature by ifname function test
 * @tc.type: FUNC
 * @tc.require: AR000FRMJB
 */
HWTEST_F(HdfWifiServiceCTest, GetFeatureByIfNameTest_003, TestSize.Level1)
{
    const int32_t wlan_type = PROTOCOL_80211_IFTYPE_AP;
    struct WlanFeatureInfo *ifeature = nullptr;

    int32_t rc = g_wlanObj->createFeature(g_wlanObj, wlan_type, (struct WlanFeatureInfo **)&ifeature);
    ASSERT_EQ(rc, HDF_SUCCESS);
    rc = g_wlanObj->getFeatureByIfName(g_wlanObj, ifeature->ifName, (struct WlanFeatureInfo **)&ifeature);
    ASSERT_EQ(rc, HDF_SUCCESS);
    rc = g_wlanObj->destroyFeature(g_wlanObj, (struct WlanFeatureInfo *)ifeature);
    ASSERT_EQ(rc, HDF_SUCCESS);
}

/**
 * @tc.name: GetAsscociatedStasTest_004
 * @tc.desc: Wifi hdi get assoc stas function test
 * @tc.type: FUNC
 * @tc.require: AR000FRMJB
 */
HWTEST_F(HdfWifiServiceCTest, GetAsscociatedStasTest_004, TestSize.Level1)
{
    const int32_t wlan_type = PROTOCOL_80211_IFTYPE_AP;
    struct WlanFeatureInfo *ifeature = nullptr;
    struct StaInfo staInfo[WLAN_MAX_NUM_STA_WITH_AP] = {{0}};
    uint32_t num = 0;

    int32_t rc = g_wlanObj->createFeature(g_wlanObj, wlan_type, (struct WlanFeatureInfo **)&ifeature);
    ASSERT_EQ(rc, HDF_SUCCESS);
    rc = g_wlanObj->getAsscociatedStas(g_wlanObj, ifeature, staInfo, WLAN_MAX_NUM_STA_WITH_AP, &num);
    ASSERT_EQ(rc, HDF_SUCCESS);
    rc = g_wlanObj->destroyFeature(g_wlanObj, (struct WlanFeatureInfo *)ifeature);
    ASSERT_EQ(rc, HDF_SUCCESS);
}

/**
 * @tc.name: SetCountryCodeTest_005
 * @tc.desc: Wifi hdi set country code function test
 * @tc.type: FUNC
 * @tc.require: AR000FRMJB
 */
HWTEST_F(HdfWifiServiceCTest, SetCountryCodeTest_005, TestSize.Level1)
{
    const char *code = "CN";
    const int32_t wlan_type = PROTOCOL_80211_IFTYPE_AP;
    struct WlanFeatureInfo *ifeature = nullptr;

    int32_t rc = g_wlanObj->createFeature(g_wlanObj, wlan_type, (struct WlanFeatureInfo **)&ifeature);
    ASSERT_EQ(rc, HDF_SUCCESS);
    rc = g_wlanObj->setCountryCode(g_wlanObj, ifeature, code, sizeof(code));
    ASSERT_EQ(rc, HDF_SUCCESS);
    rc = g_wlanObj->destroyFeature(g_wlanObj, (struct WlanFeatureInfo *)ifeature);
    ASSERT_EQ(rc, HDF_SUCCESS);
}

/**
 * @tc.name: GetNetworkIfaceNameTest_006
 * @tc.desc: Wifi hdi get network interface name function test
 * @tc.type: FUNC
 * @tc.require: AR000FRMJB
 */
HWTEST_F(HdfWifiServiceCTest, GetNetworkIfaceNameTest_006, TestSize.Level1)
{
    const int32_t wlan_type = PROTOCOL_80211_IFTYPE_AP;
    struct WlanFeatureInfo *ifeature = nullptr;

    int32_t rc = g_wlanObj->createFeature(g_wlanObj, wlan_type, (struct WlanFeatureInfo **)&ifeature);
    ASSERT_EQ(rc, HDF_SUCCESS);
    rc = g_wlanObj->getNetworkIfaceName(g_wlanObj, (struct WlanFeatureInfo *)ifeature);
    ASSERT_EQ(rc, HDF_SUCCESS);
    rc = g_wlanObj->destroyFeature(g_wlanObj, (struct WlanFeatureInfo *)ifeature);
    ASSERT_EQ(rc, HDF_SUCCESS);
}

/**
 * @tc.name: GetFeatureTypeTest_007
 * @tc.desc: Wifi hdi get feature type function test
 * @tc.type: FUNC
 * @tc.require: AR000FRMJB
 */
HWTEST_F(HdfWifiServiceCTest, GetFeatureTypeTest_007, TestSize.Level1)
{
    const int32_t wlan_type = PROTOCOL_80211_IFTYPE_AP;
    struct WlanFeatureInfo *ifeature = nullptr;

    int32_t rc = g_wlanObj->createFeature(g_wlanObj, wlan_type, (struct WlanFeatureInfo **)&ifeature);
    ASSERT_EQ(rc, HDF_SUCCESS);
    rc = g_wlanObj->getFeatureType(g_wlanObj, (struct WlanFeatureInfo *)ifeature);
    ASSERT_EQ(rc, HDF_SUCCESS);
    rc = g_wlanObj->destroyFeature(g_wlanObj, (struct WlanFeatureInfo *)ifeature);
    ASSERT_EQ(rc, HDF_SUCCESS);
}

/**
 * @tc.name: SetMacAddressTest_008
 * @tc.desc: Wifi hdi set mac addr function test
 * @tc.type: FUNC
 * @tc.require: AR000FRMJB
 */
HWTEST_F(HdfWifiServiceCTest, SetMacAddressTest_008, TestSize.Level1)
{
    uint8_t mac[ETH_ADDR_LEN] = {0x12, 0x34, 0x56, 0x78, 0xab, 0xcd};
    const int32_t wlan_type = PROTOCOL_80211_IFTYPE_AP;
    struct WlanFeatureInfo *ifeature = nullptr;

    int32_t rc = g_wlanObj->createFeature(g_wlanObj, wlan_type, (struct WlanFeatureInfo **)&ifeature);
    ASSERT_EQ(rc, HDF_SUCCESS);
    rc = g_wlanObj->setMacAddress(g_wlanObj, (struct WlanFeatureInfo *)ifeature, mac, ETH_ADDR_LEN);
    ASSERT_EQ(rc, HDF_SUCCESS);
    rc = g_wlanObj->destroyFeature(g_wlanObj, (struct WlanFeatureInfo *)ifeature);
    ASSERT_EQ(rc, HDF_SUCCESS);
}

/**
 * @tc.name: GetDeviceMacAddressTest_009
 * @tc.desc: Wifi hdi get device mac addr function test
 * @tc.type: FUNC
 * @tc.require: AR000FRMJB
 */
HWTEST_F(HdfWifiServiceCTest, GetDeviceMacAddressTest_009, TestSize.Level1)
{
    const int32_t wlan_type = PROTOCOL_80211_IFTYPE_AP;
    struct WlanFeatureInfo *ifeature = nullptr;
    uint8_t mac[ETH_ADDR_LEN] = {0};

    int32_t rc = g_wlanObj->createFeature(g_wlanObj, wlan_type, (struct WlanFeatureInfo **)&ifeature);
    ASSERT_EQ(rc, HDF_SUCCESS);
    rc = g_wlanObj->getDeviceMacAddress(g_wlanObj, (struct WlanFeatureInfo *)ifeature, mac, ETH_ADDR_LEN);
    ASSERT_EQ(rc, HDF_SUCCESS);
    rc = g_wlanObj->destroyFeature(g_wlanObj, (struct WlanFeatureInfo *)ifeature);
    ASSERT_EQ(rc, HDF_SUCCESS);
}

/**
 * @tc.name: GetFreqsWithBandTest_010
 * @tc.desc: Wifi hdi get freqs function test
 * @tc.type: FUNC
 * @tc.require: AR000FRMJB
 */
HWTEST_F(HdfWifiServiceCTest, GetFreqsWithBandTest_010, TestSize.Level1)
{
    const int32_t wlan_type = PROTOCOL_80211_IFTYPE_AP;
    struct WlanFeatureInfo *ifeature = nullptr;
    int32_t freq[WLAN_FREQ_MAX_NUM] = {0};
    int32_t wlanBand = IEEE80211_BAND_2GHZ;
    uint32_t count = 0;
    int i;

    int32_t rc = g_wlanObj->createFeature(g_wlanObj, wlan_type, (struct WlanFeatureInfo **)&ifeature);
    ASSERT_EQ(rc, HDF_SUCCESS);
    rc = g_wlanObj->getFreqsWithBand(g_wlanObj, (struct WlanFeatureInfo *)ifeature, wlanBand, freq,
        WLAN_FREQ_MAX_NUM, &count);
    ASSERT_EQ(rc, HDF_SUCCESS);
    if (rc == HDF_SUCCESS) {
        printf("%s: count = %u\n", __func__, count);
        for (i = 0; i < count; i++) {
            printf("%s: freq[%d] = %d\n", __func__, i, freq[i]);
        }
    }

    rc = g_wlanObj->destroyFeature(g_wlanObj, (struct WlanFeatureInfo *)ifeature);
    ASSERT_EQ(rc, HDF_SUCCESS);
}

/**
 * @tc.name: SetTxPowerTest_011
 * @tc.desc: Wifi hdi set tx power function test
 * @tc.type: FUNC
 * @tc.require: AR000FRMJB
 */
HWTEST_F(HdfWifiServiceCTest, SetTxPowerTest_011, TestSize.Level1)
{
    const int32_t wlan_type = PROTOCOL_80211_IFTYPE_AP;
    struct WlanFeatureInfo *ifeature = nullptr;
    int32_t power = WLAN_TX_POWER;

    int32_t rc = g_wlanObj->createFeature(g_wlanObj, wlan_type, (struct WlanFeatureInfo **)&ifeature);
    ASSERT_EQ(rc, HDF_SUCCESS);
    rc = g_wlanObj->setTxPower(g_wlanObj, (struct WlanFeatureInfo *)ifeature, power);
    ASSERT_EQ(rc, HDF_SUCCESS);
    rc = g_wlanObj->destroyFeature(g_wlanObj, (struct WlanFeatureInfo *)ifeature);
    ASSERT_EQ(rc, HDF_SUCCESS);
}

/**
 * @tc.name: GetChipIdTest_012
 * @tc.desc: Wifi hdi get chip id function test
 * @tc.type: FUNC
 * @tc.require: AR000FRMJB
 */
HWTEST_F(HdfWifiServiceCTest, GetChipIdTest_012, TestSize.Level1)
{
    const int32_t wlan_type = PROTOCOL_80211_IFTYPE_STATION;
    struct WlanFeatureInfo *ifeature = nullptr;
    uint8_t chipId = 0;
    unsigned int num = 0;
    char *ifNames = nullptr;

    int32_t rc = g_wlanObj->createFeature(g_wlanObj, wlan_type, (struct WlanFeatureInfo **)&ifeature);
    ASSERT_EQ(rc, HDF_SUCCESS);
    rc = g_wlanObj->getChipId(g_wlanObj, (struct WlanFeatureInfo *)ifeature, &chipId);
    ASSERT_EQ(rc, HDF_SUCCESS);
    rc = g_wlanObj->getIfNamesByChipId(g_wlanObj, chipId, &ifNames, &num);
    printf("ifnames = %s\n", ifNames);
    ASSERT_EQ(rc, HDF_SUCCESS);
    rc = g_wlanObj->destroyFeature(g_wlanObj, (struct WlanFeatureInfo *)ifeature);
    ASSERT_EQ(rc, HDF_SUCCESS);
}

/**
 * @tc.name: SetScanningMacAddressTest_013
 * @tc.desc: Wifi hdi set scanning mac addr function test
 * @tc.type: FUNC
 * @tc.require: AR000FRMJB
 */
HWTEST_F(HdfWifiServiceCTest, SetScanningMacAddressTest_013, TestSize.Level1)
{
    const int32_t wlan_type = PROTOCOL_80211_IFTYPE_STATION;
    struct WlanFeatureInfo *ifeature = nullptr;
    uint8_t scanMac[ETH_ADDR_LEN] = {0x12, 0x34, 0x56, 0x78, 0xab, 0xcd};

    int32_t rc = g_wlanObj->createFeature(g_wlanObj, wlan_type, (struct WlanFeatureInfo **)&ifeature);
    ASSERT_EQ(rc, HDF_SUCCESS);
    rc = g_wlanObj->setScanningMacAddress(g_wlanObj, (struct WlanFeatureInfo *)ifeature, scanMac, ETH_ADDR_LEN);
    ASSERT_NE(rc, HDF_FAILURE);
    rc = g_wlanObj->destroyFeature(g_wlanObj, (struct WlanFeatureInfo *)ifeature);
    ASSERT_EQ(rc, HDF_SUCCESS);
}

/**
 * @tc.name: GetNetdevInfoTest_014
 * @tc.desc: Wifi hdi get netdev info function test
 * @tc.type: FUNC
 * @tc.require: AR000FRMJB
 */
HWTEST_F(HdfWifiServiceCTest, GetNetdevInfoTest_014, TestSize.Level1)
{
    int32_t rc;
    struct NetDeviceInfoResult netDeviceInfoResult;

    (void)memset_s(&netDeviceInfoResult, sizeof(struct NetDeviceInfoResult), 0, sizeof(struct NetDeviceInfoResult));
    rc = g_wlanObj->getNetDevInfo(g_wlanObj, (struct NetDeviceInfoResult *)&netDeviceInfoResult);
    ASSERT_EQ(rc, HDF_SUCCESS);
}

/**
 * @tc.name: RegisterEventCallbackTest_015
 * @tc.desc: Wifi hdi reister event call back function test
 * @tc.type: FUNC
 * @tc.require: AR000FRMJB
 */
HWTEST_F(HdfWifiServiceCTest, RegisterEventCallbackTest_015, TestSize.Level1)
{
    int32_t rc = g_wlanObj->registerEventCallback(g_wlanObj, HalResetCallbackEvent);
    ASSERT_EQ(rc, HDF_SUCCESS);
}

/**
 * @tc.name: ResetDriverTest_016
 * @tc.desc: Wifi hdi reset driver function test
 * @tc.type: FUNC
 * @tc.require: AR000FRMJB
 */
HWTEST_F(HdfWifiServiceCTest, ResetDriverTest_016, TestSize.Level1)
{
    int32_t wlan_type = PROTOCOL_80211_IFTYPE_STATION;
    struct WlanFeatureInfo *ifeature = nullptr;
    uint8_t chipId = 0;
    int32_t rc;

    rc = g_wlanObj->createFeature(g_wlanObj, wlan_type, (struct WlanFeatureInfo **)&ifeature);
    ASSERT_EQ(rc, HDF_SUCCESS);
    rc = g_wlanObj->getChipId(g_wlanObj, (struct WlanFeatureInfo *)ifeature, &chipId);
    ASSERT_EQ(rc, HDF_SUCCESS);
    rc = g_wlanObj->destroyFeature(g_wlanObj, (struct WlanFeatureInfo *)ifeature);
    ASSERT_EQ(rc, HDF_SUCCESS);
    rc = g_wlanObj->resetDriver(g_wlanObj, chipId);
    ASSERT_EQ(rc, HDF_SUCCESS);
    sleep(RESET_TIME);
}

/**
 * @tc.name: StartScanTest_017
 * @tc.desc: Wifi hdi start scan function test
 * @tc.type: FUNC
 * @tc.require: AR000FRMJB
 */
HWTEST_F(HdfWifiServiceCTest, StartScanTest_017, TestSize.Level1)
{
    int32_t rc;
    const int32_t wlan_type = PROTOCOL_80211_IFTYPE_STATION;
    struct WlanFeatureInfo *ifeature = nullptr;
    WifiScan scan = {0};

    rc = g_wlanObj->createFeature(g_wlanObj, wlan_type, (struct WlanFeatureInfo **)&ifeature);
    ASSERT_EQ(rc, HDF_SUCCESS);
    rc = g_wlanObj->startScan(g_wlanObj, (struct WlanFeatureInfo *)ifeature, &scan);
    ASSERT_EQ(rc, HDF_SUCCESS);
    rc = g_wlanObj->destroyFeature(g_wlanObj, (struct WlanFeatureInfo *)ifeature);
    ASSERT_EQ(rc, HDF_SUCCESS);
    sleep(10);
}

/**
 * @tc.name: UnregisterEventCallbackTest_018
 * @tc.desc: Wifi hdi unreister event call back function test
 * @tc.type: FUNC
 * @tc.require: AR000FRMJB
 */
HWTEST_F(HdfWifiServiceCTest, UnregisterEventCallbackTest_018, TestSize.Level1)
{
    int32_t rc = g_wlanObj->unregisterEventCallback(g_wlanObj);
    ASSERT_EQ(rc, HDF_SUCCESS);
}
};