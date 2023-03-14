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
#include "Iwifi_hal.h"
#include "wlan_hal_proxy.h"
#include <gtest/gtest.h>
#include <servmgr_hdi.h>

#define HDF_LOG_TAG   service_manager_test
using namespace OHOS;
using namespace testing::ext;
using namespace OHOS::HDI::WLAN::V1_0;

namespace HdiTest {
const int32_t WLAN_FREQ_MAX_NUM = 14;
const int32_t WLAN_TX_POWER = 160;

constexpr const char *WLAN_SERVICE_NAME = "wlan_hal_service";
class WifiHdiHalServiceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void WifiHdiHalServiceTest::SetUpTestCase()
{
    auto wlanObj = IWlan::Get(WLAN_SERVICE_NAME);
    ASSERT_TRUE(wlanObj != nullptr);
    int32_t rc = wlanObj->wifiConstruct();
    ASSERT_EQ(rc, HDF_SUCCESS);
}

void WifiHdiHalServiceTest::TearDownTestCase()
{
    auto wlanObj = IWlan::Get(WLAN_SERVICE_NAME);
    ASSERT_TRUE(wlanObj != nullptr);
    int32_t rc = wlanObj->wifiDestruct();
    ASSERT_EQ(rc, HDF_SUCCESS);
}

void WifiHdiHalServiceTest::SetUp()
{
    auto wlanObj = IWlan::Get(WLAN_SERVICE_NAME);
    ASSERT_TRUE(wlanObj != nullptr);
    int32_t rc = wlanObj->start();
    ASSERT_EQ(rc, HDF_SUCCESS);
}

void WifiHdiHalServiceTest::TearDown()
{
    auto wlanObj = IWlan::Get(WLAN_SERVICE_NAME);
    ASSERT_TRUE(wlanObj != nullptr);
    int32_t rc = wlanObj->stop();
    ASSERT_EQ(rc, HDF_SUCCESS);
}

/**
 * @tc.name: GetSupportFeatureComboTest_001
 * @tc.desc: Wifi hdi get support feature and combo function test
 * @tc.type: FUNC
 * @tc.require: AR000FRMJB
 */
HWTEST_F(WifiHdiHalServiceTest, GetSupportFeatureComboTest_001, TestSize.Level1)
{

    std::vector<uint8_t> supType;
    std::vector<uint64_t> combo;

    auto wlanObj = IWlan::Get(WLAN_SERVICE_NAME);
    ASSERT_TRUE(wlanObj != nullptr);
    int32_t rc = wlanObj->getSupportFeature(supType);
    ASSERT_EQ(rc, HDF_SUCCESS);
    rc = wlanObj->getSupportCombo(combo);
    ASSERT_EQ(rc, HDF_ERR_NOT_SUPPORT);
}

/**
 * @tc.name: CreateFeatureTest_002
 * @tc.desc: Wifi hdi create feature function test
 * @tc.type: FUNC
 * @tc.require: AR000FRMJB
 */
HWTEST_F(WifiHdiHalServiceTest, CreateFeatureTest_002, TestSize.Level1)
{
    std::shared_ptr<WifiFeatureInfo> ifeature = nullptr;
    int32_t wlan_type = PROTOCOL_80211_IFTYPE_AP;

    auto wlanObj = IWlan::Get(WLAN_SERVICE_NAME);
    ASSERT_TRUE(wlanObj != nullptr);
    int32_t rc = wlanObj->createFeature(wlan_type, ifeature);
    ASSERT_EQ(rc, HDF_SUCCESS);
    rc = wlanObj->destroyFeature(ifeature);
    ASSERT_EQ(rc, HDF_SUCCESS);
}

/**
 * @tc.name: GetFeatureByIfNameTest_003
 * @tc.desc: Wifi hdi get feature by ifname function test
 * @tc.type: FUNC
 * @tc.require: AR000FRMJB
 */
HWTEST_F(WifiHdiHalServiceTest, GetFeatureByIfNameTest_003, TestSize.Level1)
{
    std::string ifName = "wlan0";
    int32_t wlan_type = PROTOCOL_80211_IFTYPE_AP;
    std::shared_ptr<WifiFeatureInfo> ifeature = nullptr;

    auto wlanObj = IWlan::Get(WLAN_SERVICE_NAME);
    ASSERT_TRUE(wlanObj != nullptr);
    int32_t rc = wlanObj->createFeature(wlan_type, ifeature);
    ASSERT_EQ(rc, HDF_SUCCESS);
    rc = wlanObj->getFeatureByIfName(ifName, ifeature);
    ASSERT_EQ(rc, HDF_SUCCESS);
    rc = wlanObj->destroyFeature(ifeature);
    ASSERT_EQ(rc, HDF_SUCCESS);
}

/**
 * @tc.name: GetAsscociatedStasTest_004
 * @tc.desc: Wifi hdi get assoc stas function test
 * @tc.type: FUNC
 * @tc.require: AR000FRMJB
 */
HWTEST_F(WifiHdiHalServiceTest, GetAsscociatedStasTest_004, TestSize.Level1)
{
    int32_t wlan_type = PROTOCOL_80211_IFTYPE_AP;
    std::shared_ptr<WifiFeatureInfo> ifeature = nullptr;
    std::shared_ptr<StaInfo> staInfo = nullptr;
    std::vector<uint32_t> num;
    uint32_t count = 0;

    auto wlanObj = IWlan::Get(WLAN_SERVICE_NAME);
    ASSERT_TRUE(wlanObj != nullptr);
    int32_t rc = wlanObj->createFeature(wlan_type, ifeature);
    ASSERT_EQ(rc, HDF_SUCCESS);
    rc = wlanObj->getAsscociatedStas(ifeature, staInfo, count, num);
    ASSERT_EQ(rc, HDF_SUCCESS);
    rc = wlanObj->destroyFeature(ifeature);
    ASSERT_EQ(rc, HDF_SUCCESS);
}

/**
 * @tc.name: SetCountryCodeTest_005
 * @tc.desc: Wifi hdi set country code function test
 * @tc.type: FUNC
 * @tc.require: AR000FRMJB
 */
HWTEST_F(WifiHdiHalServiceTest, SetCountryCodeTest_005, TestSize.Level1)
{
    std::string code = "CN";
    int32_t wlan_type = PROTOCOL_80211_IFTYPE_AP;
    std::shared_ptr<WifiFeatureInfo> ifeature = nullptr;

    auto wlanObj = IWlan::Get(WLAN_SERVICE_NAME);
    ASSERT_TRUE(wlanObj != nullptr);
    int32_t rc = wlanObj->createFeature(wlan_type, ifeature);
    ASSERT_EQ(rc, HDF_SUCCESS);
    rc = wlanObj->setCountryCode(ifeature, code, sizeof(code));
    ASSERT_EQ(rc, HDF_SUCCESS);
    rc = wlanObj->destroyFeature(ifeature);
    ASSERT_EQ(rc, HDF_SUCCESS);
}

/**
 * @tc.name: GetNetworkIfaceNameTest_006
 * @tc.desc: Wifi hdi get network interface name function test
 * @tc.type: FUNC
 * @tc.require: AR000FRMJB
 */
HWTEST_F(WifiHdiHalServiceTest, GetNetworkIfaceNameTest_006, TestSize.Level1)
{
    int32_t wlan_type = PROTOCOL_80211_IFTYPE_AP;
    std::shared_ptr<WifiFeatureInfo> ifeature = nullptr;

    auto wlanObj = IWlan::Get(WLAN_SERVICE_NAME);
    ASSERT_TRUE(wlanObj != nullptr);
    int32_t rc = wlanObj->createFeature(wlan_type, ifeature);
    ASSERT_EQ(rc, HDF_SUCCESS);
    rc = wlanObj->getNetworkIfaceName(ifeature);
    ASSERT_EQ(rc, HDF_SUCCESS);
    rc = wlanObj->destroyFeature(ifeature);
    ASSERT_EQ(rc, HDF_SUCCESS);
}

/**
 * @tc.name: GetFeatureTypeTest_007
 * @tc.desc: Wifi hdi get feature type function test
 * @tc.type: FUNC
 * @tc.require: AR000FRMJB
 */
HWTEST_F(WifiHdiHalServiceTest, GetFeatureTypeTest_007, TestSize.Level1)
{
    int32_t wlan_type = PROTOCOL_80211_IFTYPE_AP;
    std::shared_ptr<WifiFeatureInfo> ifeature = nullptr;

    auto wlanObj = IWlan::Get(WLAN_SERVICE_NAME);
    ASSERT_TRUE(wlanObj != nullptr);
    int32_t rc = wlanObj->createFeature(wlan_type, ifeature);
    ASSERT_EQ(rc, HDF_SUCCESS);
    rc = wlanObj->getFeatureType(ifeature);
    ASSERT_EQ(rc, HDF_SUCCESS);
    rc = wlanObj->destroyFeature(ifeature);
    ASSERT_EQ(rc, HDF_SUCCESS);
}

/**
 * @tc.name: SetMacAddressTest_008
 * @tc.desc: Wifi hdi set mac addr function test
 * @tc.type: FUNC
 * @tc.require: AR000FRMJB
 */
HWTEST_F(WifiHdiHalServiceTest, SetMacAddressTest_008, TestSize.Level1)
{
    std::vector<uint8_t> mac = {0x12, 0x34, 0x56, 0x78, 0xab, 0xcd};
    int32_t wlan_type = PROTOCOL_80211_IFTYPE_AP;
    std::shared_ptr<WifiFeatureInfo> ifeature = nullptr;

    auto wlanObj = IWlan::Get(WLAN_SERVICE_NAME);
    ASSERT_TRUE(wlanObj != nullptr);
    int32_t rc = wlanObj->createFeature(wlan_type, ifeature);
    ASSERT_EQ(rc, HDF_SUCCESS);
    rc = wlanObj->setMacAddress(ifeature, mac);
    ASSERT_EQ(rc, HDF_SUCCESS);
    rc = wlanObj->destroyFeature(ifeature);
    ASSERT_EQ(rc, HDF_SUCCESS);
}

/**
 * @tc.name: GetDeviceMacAddressTest_009
 * @tc.desc: Wifi hdi get device mac addr function test
 * @tc.type: FUNC
 * @tc.require: AR000FRMJB
 */
HWTEST_F(WifiHdiHalServiceTest, GetDeviceMacAddressTest_009, TestSize.Level1)
{
    int32_t wlan_type = PROTOCOL_80211_IFTYPE_AP;
    std::shared_ptr<WifiFeatureInfo> ifeature = nullptr;
    std::vector<uint8_t> mac = {0};

    auto wlanObj = IWlan::Get(WLAN_SERVICE_NAME);
    ASSERT_TRUE(wlanObj != nullptr);
    int32_t rc = wlanObj->createFeature(wlan_type, ifeature);
    ASSERT_EQ(rc, HDF_SUCCESS);
    rc = wlanObj->getDeviceMacAddress(ifeature, mac, ETH_ADDR_LEN);
    ASSERT_EQ(rc, HDF_SUCCESS);
    rc = wlanObj->destroyFeature(ifeature);
    ASSERT_EQ(rc, HDF_SUCCESS);
}

/**
 * @tc.name: GetFreqsWithBandTest_010
 * @tc.desc: Wifi hdi get freqs function test
 * @tc.type: FUNC
 * @tc.require: AR000FRMJB
 */
HWTEST_F(WifiHdiHalServiceTest, GetFreqsWithBandTest_010, TestSize.Level1)
{
    int32_t wlan_type = PROTOCOL_80211_IFTYPE_AP;
    std::shared_ptr<WifiFeatureInfo> ifeature = nullptr;
    std::vector<int32_t> freq = {0};
    int32_t wlanBand = 0;
    uint32_t count = 0;

    auto wlanObj = IWlan::Get(WLAN_SERVICE_NAME);
    ASSERT_TRUE(wlanObj != nullptr);
    int32_t rc = wlanObj->createFeature(wlan_type, ifeature);
    ASSERT_EQ(rc, HDF_SUCCESS);
    rc = wlanObj->getFreqsWithBand(ifeature, wlanBand, freq, WLAN_FREQ_MAX_NUM, count);
    ASSERT_EQ(rc, HDF_SUCCESS);
    rc = wlanObj->destroyFeature(ifeature);
    ASSERT_EQ(rc, HDF_SUCCESS);
}

/**
 * @tc.name: SetTxPowerTest_011
 * @tc.desc: Wifi hdi set tx power function test
 * @tc.type: FUNC
 * @tc.require: AR000FRMJB
 */
HWTEST_F(WifiHdiHalServiceTest, SetTxPowerTest_011, TestSize.Level1)
{
    int32_t wlan_type = PROTOCOL_80211_IFTYPE_AP;
    std::shared_ptr<WifiFeatureInfo> feature = nullptr;
    int32_t power = WLAN_TX_POWER;

    auto wlanObj = IWlan::Get(WLAN_SERVICE_NAME);
    ASSERT_TRUE(wlanObj != nullptr);
    int32_t rc = wlanObj->createFeature(wlan_type, feature);
    ASSERT_EQ(rc, HDF_SUCCESS);
    rc = wlanObj->setTxPower(feature, power);
    ASSERT_EQ(rc, HDF_SUCCESS);
    rc = wlanObj->destroyFeature(feature);
    ASSERT_EQ(rc, HDF_SUCCESS);
}

/**
 * @tc.name: GetChipIdTest_012
 * @tc.desc: Wifi hdi get chip id function test
 * @tc.type: FUNC
 * @tc.require: AR000FRMJB
 */
HWTEST_F(WifiHdiHalServiceTest, GetChipIdTest_012, TestSize.Level1)
{
    int32_t wlan_type = PROTOCOL_80211_IFTYPE_STATION;
    std::shared_ptr<WifiFeatureInfo> feature = nullptr;
    uint8_t chipId = 0;
    unsigned int num = 0;
    std::string ifName;

    auto wlanObj = IWlan::Get(WLAN_SERVICE_NAME);
    ASSERT_TRUE(wlanObj != nullptr);
    int32_t rc = wlanObj->createFeature(wlan_type, feature);
    ASSERT_EQ(rc, HDF_SUCCESS);
    rc = wlanObj->getChipId(feature, chipId);
    ASSERT_EQ(rc, HDF_SUCCESS);
    rc = wlanObj->getIfNamesByChipId(chipId, ifName, num);
    ASSERT_EQ(rc, HDF_SUCCESS);
    rc = wlanObj->destroyFeature(feature);
    ASSERT_EQ(rc, HDF_SUCCESS);
}

/**
 * @tc.name: SetScanningMacAddressTest_013
 * @tc.desc: Wifi hdi set scanning mac addr function test
 * @tc.type: FUNC
 * @tc.require: AR000FRMJB
 */
HWTEST_F(WifiHdiHalServiceTest, SetScanningMacAddressTest_013, TestSize.Level1)
{
    int32_t wlan_type = PROTOCOL_80211_IFTYPE_STATION;
    std::shared_ptr<WifiFeatureInfo> feature = nullptr;
    std::vector<uint8_t> scanMac = {0x12, 0x34, 0x56, 0x78, 0xab, 0xcd};

    auto wlanObj = IWlan::Get(WLAN_SERVICE_NAME);
    ASSERT_TRUE(wlanObj != nullptr);
    int32_t rc = wlanObj->createFeature(wlan_type, feature);
    ASSERT_EQ(rc, HDF_SUCCESS);
    rc = wlanObj->setScanningMacAddress(feature, scanMac, ETH_ADDR_LEN);
    ASSERT_EQ(rc, HDF_ERR_NOT_SUPPORT);
    rc = wlanObj->destroyFeature(feature);
    ASSERT_EQ(rc, HDF_SUCCESS);
}

static int32_t g_status = -1;

namespace{
    int32_t HalResetCallback(int32_t event, struct HdfSBuf *reqData) {
        HdfSbufReadInt32(reqData, &g_status);
        printf("status is %d\n", g_status);
        return HDF_SUCCESS;
    }
}

/**
 * @tc.name: RegisterEventCallbackTest_014
 * @tc.desc: Wifi hdi reister event call back function test
 * @tc.type: FUNC
 * @tc.require: AR000FRMJB
 */
HWTEST_F(WifiHdiHalServiceTest, RegisterEventCallbackTest_014, TestSize.Level1)
{
    auto wlanObj = IWlan::Get(WLAN_SERVICE_NAME);
    ASSERT_TRUE(wlanObj != nullptr);
    int32_t rc = wlanObj->registerEventCallback(HalResetCallback);
    ASSERT_EQ(rc, HDF_SUCCESS);
}

/**
 * @tc.name: UnregisterEventCallbackTest_015
 * @tc.desc: Wifi hdi unreister event call back function test
 * @tc.type: FUNC
 * @tc.require: AR000FRMJB
 */
HWTEST_F(WifiHdiHalServiceTest, UnregisterEventCallbackTest_015, TestSize.Level1)
{
    auto wlanObj = IWlan::Get(WLAN_SERVICE_NAME);
    ASSERT_TRUE(wlanObj != nullptr);
    int32_t rc = wlanObj->unregisterEventCallback();
    ASSERT_EQ(rc, HDF_SUCCESS);
}

/**
 * @tc.name: ResetDriverTest_016
 * @tc.desc: Wifi hdi reset driver function test
 * @tc.type: FUNC
 * @tc.require: AR000FRMJB
 */
HWTEST_F(WifiHdiHalServiceTest, ResetDriverTest_016, TestSize.Level1)
{
    int32_t wlan_type = PROTOCOL_80211_IFTYPE_STATION;
    std::shared_ptr<WifiFeatureInfo> feature = nullptr;
    uint8_t chipId = 0;

    auto wlanObj = IWlan::Get(WLAN_SERVICE_NAME);
    ASSERT_TRUE(wlanObj != nullptr);
    int32_t rc = wlanObj->registerEventCallback(HalResetCallback);
    ASSERT_EQ(rc, HDF_SUCCESS);
    rc = wlanObj->createFeature(wlan_type, feature);
    ASSERT_EQ(rc, HDF_SUCCESS);
    rc = wlanObj->getChipId(feature, chipId);
    ASSERT_EQ(rc, HDF_SUCCESS);
    rc = wlanObj->resetDriver(chipId);
    ASSERT_EQ(rc, HDF_SUCCESS);
    EXPECT_EQ(HDF_SUCCESS, g_status);
    rc = wlanObj->unregisterEventCallback();
    ASSERT_EQ(rc, HDF_SUCCESS);
    rc = wlanObj->destroyFeature(feature);
    ASSERT_EQ(rc, HDF_SUCCESS);
    sleep(20);
}
};