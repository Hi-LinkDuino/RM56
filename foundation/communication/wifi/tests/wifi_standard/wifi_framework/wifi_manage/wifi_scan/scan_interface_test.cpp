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
#include "scan_interface.h"
#include <gtest/gtest.h>
#include "mock_wifi_manager.h"
#include "mock_scan_service.h"
#include "mock_wifi_settings.h"
#include "mock_wifi_supplicant_hal_interface.h"
#include "mock_wifi_sta_hal_interface.h"

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
class ScanInterfaceTest : public testing::Test {
public:
    static void SetUpTestCase()
    {}
    static void TearDownTestCase()
    {}
    void SetUp() override
    {
        pScanInterface = std::make_unique<ScanInterface>();
        pScanInterface->pScanService = new MockScanService();
        pScanInterface->mScanSerivceCallbacks = WifiManager::GetInstance().GetScanCallback();
    }
    void TearDown() override
    {
        pScanInterface.reset();
    }

public:
    std::unique_ptr<ScanInterface> pScanInterface;
};

extern "C" IScanService *Create(void);
extern "C" void Destroy(IScanService *pservice);
HWTEST_F(ScanInterfaceTest, GCreateDestroy, TestSize.Level1)
{
    auto p = Create();
    Destroy(p);
}

HWTEST_F(ScanInterfaceTest, InitTest, TestSize.Level1)
{
    EXPECT_CALL(WifiSettings::GetInstance(), GetSupportHwPnoFlag()).Times(AtLeast(1));
    EXPECT_CALL(WifiSupplicantHalInterface::GetInstance(), RegisterSupplicantEventCallback(_)).Times(AtLeast(1));
    EXPECT_CALL(WifiStaHalInterface::GetInstance(), GetSupportFrequencies(_, _)).Times(AtLeast(1));
    EXPECT_CALL(WifiSettings::GetInstance(), GetScanControlInfo(_)).Times(AtLeast(1));
    EXPECT_CALL(WifiSettings::GetInstance(), GetScreenState()).Times(AtLeast(1));
    EXPECT_CALL(WifiManager::GetInstance(), DealScanOpenRes()).Times(AtLeast(0));
    EXPECT_CALL(WifiSupplicantHalInterface::GetInstance(), UnRegisterSupplicantEventCallback()).Times(AtLeast(1));
    EXPECT_CALL(WifiSettings::GetInstance(), GetWhetherToAllowNetworkSwitchover()).Times(AtLeast(0));
    EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_)).Times(AtLeast(0));
    pScanInterface->Init();
}

HWTEST_F(ScanInterfaceTest, UnInitTest, TestSize.Level1)
{
    pScanInterface->UnInit();
}

HWTEST_F(ScanInterfaceTest, ScanSuccess, TestSize.Level1)
{
    EXPECT_EQ(WIFI_OPT_SUCCESS, pScanInterface->Scan(true));
}

HWTEST_F(ScanInterfaceTest, ScanWithParamSuccess, TestSize.Level1)
{
    WifiScanParams wifiScanParams;
    wifiScanParams.ssid = "hmwifi";
    wifiScanParams.bssid = "a3:b1:d7:f2:d2:c2";
    EXPECT_EQ(WIFI_OPT_SUCCESS, pScanInterface->ScanWithParam(wifiScanParams));
}

HWTEST_F(ScanInterfaceTest, OnScreenStateChangedSuccess1, TestSize.Level1)
{
    int screenState = MODE_STATE_OPEN;
    EXPECT_EQ(WIFI_OPT_SUCCESS, pScanInterface->OnScreenStateChanged(screenState));
}

HWTEST_F(ScanInterfaceTest, OnScreenStateChangedSuccess2, TestSize.Level1)
{
    int screenState = MODE_STATE_CLOSE;
    EXPECT_EQ(WIFI_OPT_SUCCESS, pScanInterface->OnScreenStateChanged(screenState));
}

HWTEST_F(ScanInterfaceTest, OnScreenStateChangedFail, TestSize.Level1)
{
    int screenState = 0;
    EXPECT_EQ(WIFI_OPT_INVALID_PARAM, pScanInterface->OnScreenStateChanged(screenState));
}

HWTEST_F(ScanInterfaceTest, OnClientModeStatusChangedSuccess, TestSize.Level1)
{
    EXPECT_EQ(WIFI_OPT_SUCCESS, pScanInterface->OnClientModeStatusChanged(0));
}

HWTEST_F(ScanInterfaceTest, OnAppRunningModeChangedSuccess, TestSize.Level1)
{
    EXPECT_EQ(WIFI_OPT_SUCCESS, pScanInterface->OnAppRunningModeChanged(ScanMode::APP_FOREGROUND_SCAN));
}

HWTEST_F(ScanInterfaceTest, OnCustomControlStateChangedSuccess, TestSize.Level1)
{
    int customSceneStatus = MODE_STATE_OPEN;
    EXPECT_EQ(WIFI_OPT_SUCCESS, pScanInterface->OnCustomControlStateChanged(0, customSceneStatus));
}

HWTEST_F(ScanInterfaceTest, OnCustomControlStateChangedFail, TestSize.Level1)
{
    int customSceneStatus = 0;
    EXPECT_EQ(WIFI_OPT_INVALID_PARAM, pScanInterface->OnCustomControlStateChanged(0, customSceneStatus));
}

HWTEST_F(ScanInterfaceTest, OnControlStrategyChangedSuccess, TestSize.Level1)
{
    EXPECT_EQ(WIFI_OPT_SUCCESS, pScanInterface->OnControlStrategyChanged());
}

HWTEST_F(ScanInterfaceTest, RegisterScanCallbacksTest, TestSize.Level1)
{
    IScanSerivceCallbacks callbacks;
    EXPECT_EQ(WIFI_OPT_SUCCESS, pScanInterface->RegisterScanCallbacks(callbacks));
}
}  // namespace Wifi
}  // namespace OHOS
