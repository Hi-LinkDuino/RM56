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
#include "sta_saved_device_appraisal.h"
#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include "wifi_errcode.h"
#include "wifi_internal_msg.h"
#include "wifi_msg.h"

#include "mock_wifi_settings.h"
#include "mock_device_appraisal.h"

using ::testing::_;
using ::testing::AtLeast;
using ::testing::DoAll;
using ::testing::Eq;
using ::testing::Return;
using ::testing::SetArgReferee;
using ::testing::StrEq;
using ::testing::TypedEq;
using ::testing::ext::TestSize;

namespace OHOS {
namespace Wifi {
constexpr int NETWORK_ID = 15;
constexpr int BAND = 2;
constexpr int RSSI = 8;

class StaSavedDeviceAppraisalTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    virtual void SetUp()
    {
        EXPECT_CALL(WifiSettings::GetInstance(), GetScoretacticsScoreSlope()).Times(AtLeast(0));
        EXPECT_CALL(WifiSettings::GetInstance(), GetScoretacticsInitScore()).Times(AtLeast(0));
        EXPECT_CALL(WifiSettings::GetInstance(), GetScoretacticsSameBssidScore()).Times(AtLeast(0));
        EXPECT_CALL(WifiSettings::GetInstance(), GetScoretacticsSameNetworkScore()).Times(AtLeast(0));
        EXPECT_CALL(WifiSettings::GetInstance(), GetScoretacticsFrequency5GHzScore()).Times(AtLeast(0));
        EXPECT_CALL(WifiSettings::GetInstance(), GetScoretacticsLastSelectionScore()).Times(AtLeast(0));
        EXPECT_CALL(WifiSettings::GetInstance(), GetScoretacticsSecurityScore()).Times(AtLeast(0));
        pStaSavedDeviceAppraisal = std::make_unique<StaSavedDeviceAppraisal>(false);
    }
    virtual void TearDown()
    {
        pStaSavedDeviceAppraisal.reset();
    }

public:
    void SaveNetworkEvaluatorSuccess1();
    void SaveNetworkEvaluatorSuccess2();
    void SaveNetworkEvaluatorFail1();
    void AppraiseDeviceQualitySuccess1();
    void AppraiseDeviceQualitySuccess2();
    void WhetherSkipDeviceSuccess1();
    void WhetherSkipDeviceFail1();

public:
    std::unique_ptr<StaSavedDeviceAppraisal> pStaSavedDeviceAppraisal;

private:
    void GetScanInfoConfig(InterScanInfo &scanInfo) const;
    void GetWifiDeviceConfig(WifiDeviceConfig &deviceConfig) const;
    void GetInterScanInfoVector(std::vector<InterScanInfo> &scanInfos) const;
    void GetWifiLinkedInfo(WifiLinkedInfo &info);
};


void StaSavedDeviceAppraisalTest::GetScanInfoConfig(InterScanInfo &scanInfo) const
{
    scanInfo.bssid = "2a:76:93:47:e2:8a";
    scanInfo.ssid = "HMWIFI_W2_EAP_G2_03";
    scanInfo.band = NETWORK_24G_BAND;
    scanInfo.rssi = RSSI;
    scanInfo.securityType = WifiSecurity::OPEN;
}

void StaSavedDeviceAppraisalTest::GetWifiDeviceConfig(WifiDeviceConfig &deviceConfig) const
{
    deviceConfig.bssid = "2a:76:93:47:e2:8a";
    deviceConfig.band = BAND;
    deviceConfig.networkId = NETWORK_ID;
    deviceConfig.ssid = "HMWIFI_W2_EAP_G2_03";
    deviceConfig.keyMgmt = "123456";
}

void StaSavedDeviceAppraisalTest::GetInterScanInfoVector(std::vector<InterScanInfo> &scanInfos) const
{
    InterScanInfo scanInfo;
    GetScanInfoConfig(scanInfo);
    scanInfos.push_back(scanInfo);
}

void StaSavedDeviceAppraisalTest::GetWifiLinkedInfo(WifiLinkedInfo &info)
{
    info.detailedState = DetailedState::WORKING; // WORKING
    info.bssid = "2a:76:93:47:e2:8a";
    info.ssid = "HMWIFI_W2_EAP_G2_03";
    info.networkId = NETWORK_ID;
    info.connState = ConnState::CONNECTED;
}

void StaSavedDeviceAppraisalTest::SaveNetworkEvaluatorSuccess1()
{
    WifiDeviceConfig deviceConfig;
    std::vector<InterScanInfo> scanInfos;
    WifiLinkedInfo info;
    GetWifiDeviceConfig(deviceConfig);
    GetInterScanInfoVector(scanInfos);
    GetWifiLinkedInfo(info);

    deviceConfig.status = static_cast<int>(WifiDeviceConfigStatus::ENABLED);
    deviceConfig.networkId = 0;

    EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(scanInfos[0].bssid, DEVICE_CONFIG_INDEX_BSSID, _))
        .WillOnce(DoAll(SetArgReferee<2>(deviceConfig), Return(0)));
    EXPECT_CALL(WifiSettings::GetInstance(), GetSignalLevel(_, _)).Times(AtLeast(0));
    EXPECT_CALL(WifiSettings::GetInstance(), GetUserLastSelectedNetworkId()).Times(AtLeast(1)).WillOnce(Return(0));

    EXPECT_TRUE(pStaSavedDeviceAppraisal->DeviceAppraisals(deviceConfig, scanInfos, info) == WIFI_OPT_SUCCESS);
}

void StaSavedDeviceAppraisalTest::SaveNetworkEvaluatorSuccess2()
{
    WifiDeviceConfig deviceConfig;
    std::vector<InterScanInfo> scanInfos;
    WifiLinkedInfo info;
    GetWifiDeviceConfig(deviceConfig);
    GetInterScanInfoVector(scanInfos);
    GetWifiLinkedInfo(info);

    deviceConfig.isPasspoint = false;
    deviceConfig.isEphemeral = false;
    deviceConfig.status = static_cast<int>(WifiDeviceConfigStatus::ENABLED);
    deviceConfig.networkId = 0;

    EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(scanInfos[0].bssid, DEVICE_CONFIG_INDEX_BSSID, _))
        .WillOnce(DoAll(SetArgReferee<2>(deviceConfig), Return(0)));
    EXPECT_CALL(WifiSettings::GetInstance(), GetSignalLevel(_, _)).Times(AtLeast(0));
    EXPECT_CALL(WifiSettings::GetInstance(), GetUserLastSelectedNetworkId()).Times(AtLeast(1)).WillOnce(Return(0));
    EXPECT_CALL(WifiSettings::GetInstance(), GetUserLastSelectedNetworkTimeVal()).Times(AtLeast(1));

    EXPECT_TRUE(pStaSavedDeviceAppraisal->DeviceAppraisals(deviceConfig, scanInfos, info) == WIFI_OPT_SUCCESS);
}


void StaSavedDeviceAppraisalTest::SaveNetworkEvaluatorFail1()
{
    WifiDeviceConfig deviceConfig;
    std::vector<InterScanInfo> scanInfos;
    WifiLinkedInfo info;
    GetWifiDeviceConfig(deviceConfig);
    GetInterScanInfoVector(scanInfos);
    GetWifiLinkedInfo(info);

    info.connState = ConnState::DISCONNECTED;

    EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(scanInfos[0].bssid, DEVICE_CONFIG_INDEX_BSSID, _))
        .Times(AtLeast(1))
        .WillRepeatedly(Return(0));
    EXPECT_CALL(WifiSettings::GetInstance(), GetSignalLevel(_, _)).Times(AtLeast(0));
    EXPECT_CALL(WifiSettings::GetInstance(), GetUserLastSelectedNetworkId()).Times(AtLeast(0)).WillOnce(Return(0));

    EXPECT_TRUE(pStaSavedDeviceAppraisal->DeviceAppraisals(deviceConfig, scanInfos, info) != WIFI_OPT_SUCCESS);
}

void StaSavedDeviceAppraisalTest::AppraiseDeviceQualitySuccess1()
{
    int score = 1;
    InterScanInfo scanInfo;
    WifiDeviceConfig deviceConfig;
    WifiLinkedInfo info;
    GetScanInfoConfig(scanInfo);
    GetWifiDeviceConfig(deviceConfig);
    GetWifiLinkedInfo(info);
    scanInfo.band = NETWORK_24G_BAND;
    deviceConfig.networkId = 0;

    EXPECT_CALL(WifiSettings::GetInstance(), GetSignalLevel(_, _)).Times(AtLeast(0));
    EXPECT_CALL(WifiSettings::GetInstance(), GetUserLastSelectedNetworkId()).Times(AtLeast(1)).WillOnce(Return(0));
    EXPECT_CALL(WifiSettings::GetInstance(), GetUserLastSelectedNetworkTimeVal()).Times(AtLeast(1));
    pStaSavedDeviceAppraisal->AppraiseDeviceQuality(score, scanInfo, deviceConfig, info);
}

void StaSavedDeviceAppraisalTest::AppraiseDeviceQualitySuccess2()
{
    int score = 1;
    InterScanInfo scanInfo;
    WifiDeviceConfig deviceConfig;
    WifiLinkedInfo info;
    GetScanInfoConfig(scanInfo);
    GetWifiDeviceConfig(deviceConfig);
    GetWifiLinkedInfo(info);
    scanInfo.band = NETWORK_5G_BAND;
    deviceConfig.networkId = 0;

    EXPECT_CALL(WifiSettings::GetInstance(), GetSignalLevel(_, _)).Times(AtLeast(0));
    EXPECT_CALL(WifiSettings::GetInstance(), GetUserLastSelectedNetworkId()).Times(AtLeast(1)).WillOnce(Return(0));
    EXPECT_CALL(WifiSettings::GetInstance(), GetUserLastSelectedNetworkTimeVal()).Times(AtLeast(1));
    pStaSavedDeviceAppraisal->AppraiseDeviceQuality(score, scanInfo, deviceConfig, info);
}

void StaSavedDeviceAppraisalTest::WhetherSkipDeviceSuccess1()
{
    WifiDeviceConfig deviceConfig;
    GetWifiDeviceConfig(deviceConfig);
    deviceConfig.isPasspoint = false;
    deviceConfig.isEphemeral = false;
    deviceConfig.status = static_cast<int>(WifiDeviceConfigStatus::ENABLED);

    EXPECT_TRUE(pStaSavedDeviceAppraisal->WhetherSkipDevice(deviceConfig) == true);
}

void StaSavedDeviceAppraisalTest::WhetherSkipDeviceFail1()
{
    WifiDeviceConfig deviceConfig;
    GetWifiDeviceConfig(deviceConfig);
    deviceConfig.isPasspoint = true;
    deviceConfig.isEphemeral = true;
    deviceConfig.status = static_cast<int>(WifiDeviceConfigStatus::ENABLED);
    EXPECT_TRUE(pStaSavedDeviceAppraisal->WhetherSkipDevice(deviceConfig) == false);
}

HWTEST_F(StaSavedDeviceAppraisalTest, SaveNetworkEvaluatorSuccess1, TestSize.Level1)
{
    SaveNetworkEvaluatorSuccess1();
}

HWTEST_F(StaSavedDeviceAppraisalTest, SaveNetworkEvaluatorSuccess2, TestSize.Level1)
{
    SaveNetworkEvaluatorSuccess2();
}

HWTEST_F(StaSavedDeviceAppraisalTest, SaveNetworkEvaluatorFail1, TestSize.Level1)
{
    SaveNetworkEvaluatorFail1();
}

HWTEST_F(StaSavedDeviceAppraisalTest, AppraiseDeviceQualitySuccess1, TestSize.Level1)
{
    AppraiseDeviceQualitySuccess1();
}

HWTEST_F(StaSavedDeviceAppraisalTest, AppraiseDeviceQualitySuccess2, TestSize.Level1)
{
    AppraiseDeviceQualitySuccess2();
}

HWTEST_F(StaSavedDeviceAppraisalTest, WhetherSkipDeviceSuccess1, TestSize.Level1)
{
    WhetherSkipDeviceSuccess1();
}

HWTEST_F(StaSavedDeviceAppraisalTest, WhetherSkipDeviceFail1, TestSize.Level1)
{
    WhetherSkipDeviceFail1();
}
} // Wifi
} // OHOS