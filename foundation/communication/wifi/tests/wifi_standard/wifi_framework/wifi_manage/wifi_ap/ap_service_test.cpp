/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "ap_service.h"
#include "mock_wifi_settings.h"
#include "mock_pendant.h"
#include "operator_overload.h"
#include "mock_wifi_ap_hal_interface.h"
#include "i_ap_service_callbacks.h"

using namespace OHOS;
using ::testing::_;
using ::testing::DoAll;
using ::testing::Eq;
using ::testing::Return;
using ::testing::SetArgReferee;
using ::testing::StrEq;
using ::testing::TypedEq;
using ::testing::ext::TestSize;

namespace OHOS {
namespace Wifi {
const StationInfo staInfo = {
    "test_deviceName",
    "AA:BB:CC:DD:EE:FF",
    "127.0.0.1",
};
class ApService_test : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    virtual void SetUp()
    {
        const int SLEEP_TIME = 20;
        pMockPendant = new MockPendant();
        pMockPendant->GetMockApStateMachine().InitialStateMachine();
        pApService = new ApService(pMockPendant->GetMockApStateMachine());
        int testMaxConn = 10;
        int channel = 6;
        apInfo.SetSsid(std::string("TEST"));
        apInfo.SetPreSharedKey(std::string("123456789"));
        apInfo.SetSecurityType(KeyMgmt::WPA2_PSK);
        apInfo.SetBand(BandType::BAND_2GHZ);
        apInfo.SetChannel(channel);
        apInfo.SetMaxConn(testMaxConn);
        usleep(SLEEP_TIME);
    }
    virtual void TearDown()
    {
        delete pApService;
        pApService = nullptr;
        delete pMockPendant;
        pMockPendant = nullptr;
    }

public:
    ErrCode WarpRegisterApServiceCallbacks(const IApServiceCallbacks &callbacks)
    {
        return pApService->RegisterApServiceCallbacks(callbacks);
    }

public:
    MockPendant *pMockPendant;
    ApService *pApService;
    HotspotConfig apInfo;
};
/* EnableHotspot */
HWTEST_F(ApService_test, EnableHotspot_SUCCESS, TestSize.Level1)
{
    EXPECT_CALL(WifiApHalInterface::GetInstance(), RegisterApEvent(_))
        .WillRepeatedly(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    EXPECT_EQ(ErrCode::WIFI_OPT_SUCCESS, pApService->EnableHotspot());
}
/* DisableHotspot */
HWTEST_F(ApService_test, DisableHotspotSUCCESS, TestSize.Level1)
{
    EXPECT_CALL(WifiApHalInterface::GetInstance(), RegisterApEvent(_))
        .WillRepeatedly(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    EXPECT_EQ(ErrCode::WIFI_OPT_SUCCESS, pApService->DisableHotspot());
}
/* SetHotspotConfig */
HWTEST_F(ApService_test, SetHotspotConfig_SUCCESS, TestSize.Level1)
{
    EXPECT_CALL(WifiApHalInterface::GetInstance(), RegisterApEvent(_))
        .WillRepeatedly(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    EXPECT_EQ(ErrCode::WIFI_OPT_SUCCESS, pApService->SetHotspotConfig(apInfo));
}
/* AddBlockList */
HWTEST_F(ApService_test, AddBlockList_SUCCESS, TestSize.Level1)
{
    EXPECT_CALL(WifiApHalInterface::GetInstance(), RegisterApEvent(_))
        .WillRepeatedly(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    EXPECT_EQ(ErrCode::WIFI_OPT_SUCCESS, pApService->AddBlockList(staInfo));
}
/* DelBlockList */
HWTEST_F(ApService_test, DelBlockList_SUCCESS, TestSize.Level1)
{
    EXPECT_CALL(WifiApHalInterface::GetInstance(), RegisterApEvent(_))
        .WillRepeatedly(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    EXPECT_EQ(ErrCode::WIFI_OPT_SUCCESS, pApService->DelBlockList(staInfo));
}
/* DisconnetStation */
HWTEST_F(ApService_test, DisconnetStation_SUCCESS, TestSize.Level1)
{
    EXPECT_CALL(WifiApHalInterface::GetInstance(), RegisterApEvent(_))
        .WillRepeatedly(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    EXPECT_EQ(ErrCode::WIFI_OPT_SUCCESS, pApService->DisconnetStation(staInfo));
}

/* GetValidBands */
HWTEST_F(ApService_test, GetValidBands_SUCCESS, TestSize.Level1)
{
    std::vector<BandType> vecSta;
    std::vector<BandType> temp = {
        BandType::BAND_2GHZ,
        BandType::BAND_5GHZ,
    };

    EXPECT_CALL(WifiSettings::GetInstance(), GetValidBands(Eq(vecSta)))
        .WillOnce(DoAll(SetArgReferee<0>(temp), Return(0)));
    EXPECT_CALL(WifiApHalInterface::GetInstance(), RegisterApEvent(_))
        .WillRepeatedly(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    EXPECT_EQ(ErrCode::WIFI_OPT_SUCCESS, pApService->GetValidBands(vecSta));
    EXPECT_EQ(temp, vecSta);
}
HWTEST_F(ApService_test, GetValidBands_FAILED, TestSize.Level1)
{
    std::vector<BandType> vecSta;
    std::vector<BandType> temp = {
        BandType::BAND_2GHZ,
        BandType::BAND_5GHZ,
    };

    EXPECT_CALL(WifiSettings::GetInstance(), GetValidBands(Eq(vecSta)))
        .WillOnce(DoAll(SetArgReferee<0>(temp), Return(-1)));
    EXPECT_CALL(WifiApHalInterface::GetInstance(), RegisterApEvent(_))
        .WillRepeatedly(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    EXPECT_EQ(ErrCode::WIFI_OPT_FAILED, pApService->GetValidBands(vecSta));
}

/* GetValidChannels */
HWTEST_F(ApService_test, GetValidChannels_SUCCESS, TestSize.Level1)
{
    std::vector<int32_t> vecChannels;
    std::vector<int32_t> band_2G_channel = { 1, 2, 3, 4, 5, 6, 7 };
    std::vector<int32_t> band_5G_channel = { 149, 168, 169 };
    ChannelsTable temp = { { BandType::BAND_2GHZ, band_2G_channel }, { BandType::BAND_5GHZ, band_5G_channel } };
    EXPECT_CALL(WifiApHalInterface::GetInstance(), RegisterApEvent(_))
        .WillRepeatedly(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    EXPECT_CALL(WifiSettings::GetInstance(), GetValidChannels(_)).WillOnce(DoAll(SetArgReferee<0>(temp), Return(0)));
    EXPECT_EQ(ErrCode::WIFI_OPT_SUCCESS, pApService->GetValidChannels(BandType::BAND_2GHZ, vecChannels));
    EXPECT_EQ(vecChannels, band_2G_channel);
    vecChannels.clear();

    EXPECT_CALL(WifiSettings::GetInstance(), GetValidChannels(_)).WillOnce(DoAll(SetArgReferee<0>(temp), Return(0)));
    EXPECT_EQ(ErrCode::WIFI_OPT_SUCCESS, pApService->GetValidChannels(BandType::BAND_5GHZ, vecChannels));
    EXPECT_EQ(vecChannels, band_5G_channel);
    vecChannels.clear();

    EXPECT_CALL(WifiSettings::GetInstance(), GetValidChannels(_)).WillOnce(DoAll(SetArgReferee<0>(temp), Return(1)));
    EXPECT_EQ(ErrCode::WIFI_OPT_FAILED, pApService->GetValidChannels(BandType::BAND_5GHZ, vecChannels));
}

/* RegisterApServiceCallbacks */
HWTEST_F(ApService_test, RegisterApServiceCallbacks_SUCCESS, TestSize.Level1)
{
    EXPECT_CALL(WifiApHalInterface::GetInstance(), RegisterApEvent(_))
        .WillRepeatedly(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    IApServiceCallbacks callbacks;
    EXPECT_EQ(ErrCode::WIFI_OPT_SUCCESS, WarpRegisterApServiceCallbacks(callbacks));
}
} // namespace Wifi
} // namespace OHOS