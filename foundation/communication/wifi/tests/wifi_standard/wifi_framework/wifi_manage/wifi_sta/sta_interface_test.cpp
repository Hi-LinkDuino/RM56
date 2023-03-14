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
#include "sta_interface.h"
#include <mutex>
#include <condition_variable>
#include <gtest/gtest.h>
#include <sys/time.h>
#include "mock_sta_service.h"
#include "mock_wifi_settings.h"
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
bool operator == (const WifiDeviceConfig &lhs, const WifiDeviceConfig &rhs)
{
    return lhs.networkId == rhs.networkId;
}

class StaInterfaceTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    virtual void SetUp() override
    {
        pStaInterface.reset(new StaInterface);
        pStaInterface->pStaService = new MockWifiStaService();
        pMockStaService = (MockWifiStaService *)pStaInterface->pStaService;
    }
    virtual void TearDown() override
    {
        pStaInterface.reset();
    }

    void SleepMs(const int sleepMs)
    {
        std::unique_lock<std::mutex> lck(mMtxBlock);
        mCvTest.wait_for(lck, std::chrono::milliseconds(sleepMs));
    }

public:
    std::unique_ptr<StaInterface> pStaInterface;
    MockWifiStaService *pMockStaService = nullptr;
    std::mutex mMtxBlock;
    std::condition_variable mCvTest;

    void EnableWifiSuccess()
    {
        EXPECT_CALL(*pMockStaService, InitStaService(_)).WillRepeatedly(Return(WIFI_OPT_SUCCESS));
        EXPECT_CALL(*pMockStaService, EnableWifi()).WillRepeatedly(Return(WIFI_OPT_SUCCESS));
        EXPECT_TRUE(pStaInterface->EnableWifi() == WIFI_OPT_SUCCESS);
    }

    void DisableWifiSuceess()
    {
        EXPECT_CALL(*pMockStaService, DisableWifi()).WillRepeatedly(Return(WIFI_OPT_SUCCESS));
        EXPECT_TRUE(pStaInterface->DisableWifi() == WIFI_OPT_SUCCESS);
    }

    void DisableWifiFail()
    {
        EXPECT_CALL(*pMockStaService, DisableWifi()).WillRepeatedly(Return(WIFI_OPT_FAILED));
        EXPECT_TRUE(pStaInterface->DisableWifi() == WIFI_OPT_FAILED);
    }

    void ConnectToNetworkIdSuceess()
    {
        int networkId = 0;
        EXPECT_CALL(*pMockStaService, ConnectToNetwork(networkId)).WillRepeatedly(Return(WIFI_OPT_SUCCESS));
        EXPECT_TRUE(pStaInterface->ConnectToNetwork(0) == WIFI_OPT_SUCCESS);
    }

    void ConnectToNetworkIdFail1()
    {
        int networkId = 0;
        EXPECT_CALL(*pMockStaService, ConnectToNetwork(networkId)).WillRepeatedly(Return(WIFI_OPT_FAILED));
        EXPECT_TRUE(pStaInterface->ConnectToNetwork(0) == WIFI_OPT_FAILED);
    }

    void ConnectToConfigSuceess()
    {
        WifiDeviceConfig config;
        EXPECT_CALL(*pMockStaService, ConnectToDevice(config)).WillRepeatedly(Return(WIFI_OPT_SUCCESS));
        EXPECT_TRUE(pStaInterface->ConnectToDevice(config) == WIFI_OPT_SUCCESS);
    }

    void ConnectToConfigFail1()
    {
        WifiDeviceConfig config;
        EXPECT_CALL(*pMockStaService, ConnectToDevice(config)).WillRepeatedly(Return(WIFI_OPT_FAILED));
        EXPECT_TRUE(pStaInterface->ConnectToDevice(config) == WIFI_OPT_FAILED);
    }

    void ReAssociateSuceess()
    {
        WifiDeviceConfig config;
        EXPECT_CALL(*pMockStaService, ReAssociate()).WillRepeatedly(Return(WIFI_OPT_SUCCESS));
        EXPECT_TRUE(pStaInterface->ReAssociate() == WIFI_OPT_SUCCESS);
    }

    void ReAssociateFail1()
    {
        WifiDeviceConfig config;
        EXPECT_CALL(*pMockStaService, ReAssociate()).WillRepeatedly(Return(WIFI_OPT_FAILED));
        EXPECT_TRUE(pStaInterface->ReAssociate() == WIFI_OPT_FAILED);
    }

    void DisconnectSuceess()
    {
        WifiDeviceConfig config;
        EXPECT_CALL(*pMockStaService, Disconnect()).WillRepeatedly(Return(WIFI_OPT_SUCCESS));
        EXPECT_TRUE(pStaInterface->Disconnect() == WIFI_OPT_SUCCESS);
    }

    void DisconnectFail1()
    {
        WifiDeviceConfig config;
        EXPECT_CALL(*pMockStaService, Disconnect()).WillRepeatedly(Return(WIFI_OPT_FAILED));
        EXPECT_TRUE(pStaInterface->Disconnect() == WIFI_OPT_FAILED);
    }

    void AddDeviceConfigSuceess()
    {
        WifiDeviceConfig config;
        EXPECT_CALL(*pMockStaService, AddDeviceConfig(_)).WillRepeatedly(Return(WIFI_OPT_SUCCESS));
        EXPECT_TRUE(pStaInterface->AddDeviceConfig(config) == WIFI_OPT_SUCCESS);
    }

    void AddDeviceConfigFail1()
    {
        WifiDeviceConfig config;
        EXPECT_CALL(*pMockStaService, AddDeviceConfig(_)).WillRepeatedly(Return(WIFI_OPT_FAILED));
        EXPECT_TRUE(pStaInterface->AddDeviceConfig(config) == WIFI_OPT_FAILED);
    }

    void UpdateDeviceConfigSuceess()
    {
        WifiDeviceConfig config;
        EXPECT_CALL(*pMockStaService, UpdateDeviceConfig(_)).WillRepeatedly(Return(WIFI_OPT_SUCCESS));
        EXPECT_TRUE(pStaInterface->UpdateDeviceConfig(config) == WIFI_OPT_SUCCESS);
    }

    void UpdateDeviceConfigFail1()
    {
        WifiDeviceConfig config;
        EXPECT_CALL(*pMockStaService, UpdateDeviceConfig(_)).WillRepeatedly(Return(WIFI_OPT_FAILED));
        EXPECT_TRUE(pStaInterface->UpdateDeviceConfig(config) == WIFI_OPT_FAILED);
    }

    void RemoveDeviceConfigSuceess()
    {
        EXPECT_CALL(*pMockStaService, RemoveDevice(_)).WillRepeatedly(Return(WIFI_OPT_SUCCESS));
        EXPECT_TRUE(pStaInterface->RemoveDevice(0) == WIFI_OPT_SUCCESS);
    }

    void RemoveDeviceConfigFail1()
    {
        EXPECT_CALL(*pMockStaService, RemoveDevice(_)).WillRepeatedly(Return(WIFI_OPT_FAILED));
        EXPECT_TRUE(pStaInterface->RemoveDevice(0) == WIFI_OPT_FAILED);
    }

    void EnableDeviceConfigSuceess()
    {
        EXPECT_CALL(*pMockStaService, EnableDeviceConfig(_, _)).WillRepeatedly(Return(WIFI_OPT_SUCCESS));
        EXPECT_TRUE(pStaInterface->EnableDeviceConfig(0, true) == WIFI_OPT_SUCCESS);
    }

    void EnableDeviceConfigFail1()
    {
        EXPECT_CALL(*pMockStaService, EnableDeviceConfig(_, _)).WillRepeatedly(Return(WIFI_OPT_FAILED));
        EXPECT_TRUE(pStaInterface->EnableDeviceConfig(0, true) == WIFI_OPT_FAILED);
    }

    void DisableDeviceConfigSuceess()
    {
        EXPECT_CALL(*pMockStaService, DisableDeviceConfig(_)).WillRepeatedly(Return(WIFI_OPT_SUCCESS));
        EXPECT_TRUE(pStaInterface->DisableDeviceConfig(0) == WIFI_OPT_SUCCESS);
    }

    void DisableDeviceConfigFail1()
    {
        EXPECT_CALL(*pMockStaService, DisableDeviceConfig(_)).WillRepeatedly(Return(WIFI_OPT_FAILED));
        EXPECT_TRUE(pStaInterface->DisableDeviceConfig(0) == WIFI_OPT_FAILED);
    }

    void StartWpsSuceess()
    {
        WpsConfig config;
        EXPECT_CALL(*pMockStaService, StartWps(_)).WillRepeatedly(Return(WIFI_OPT_SUCCESS));
        EXPECT_TRUE(pStaInterface->StartWps(config) == WIFI_OPT_SUCCESS);
    }

    void StartWpsFail1()
    {
        WpsConfig config;
        EXPECT_CALL(*pMockStaService, StartWps(_)).WillRepeatedly(Return(WIFI_OPT_FAILED));
        EXPECT_TRUE(pStaInterface->StartWps(config) == WIFI_OPT_FAILED);
    }

    void CancelWpsSuceess()
    {
        EXPECT_CALL(*pMockStaService, CancelWps()).WillRepeatedly(Return(WIFI_OPT_SUCCESS));
        EXPECT_TRUE(pStaInterface->CancelWps() == WIFI_OPT_SUCCESS);
    }

    void CancelWpsFail1()
    {
        EXPECT_CALL(*pMockStaService, CancelWps()).WillRepeatedly(Return(WIFI_OPT_FAILED));
        EXPECT_TRUE(pStaInterface->CancelWps() == WIFI_OPT_FAILED);
    }
    
    void AutoConnectServiceSuceess()
    {
        std::vector<InterScanInfo> scanInfos;
        EXPECT_CALL(*pMockStaService, AutoConnectService(_)).WillRepeatedly(Return(WIFI_OPT_SUCCESS));
        EXPECT_TRUE(pStaInterface->ConnectivityManager(scanInfos) == WIFI_OPT_SUCCESS);
    }

    void AutoConnectServiceFail()
    {
        std::vector<InterScanInfo> scanInfos;
        EXPECT_CALL(*pMockStaService, AutoConnectService(_)).WillRepeatedly(Return(WIFI_OPT_FAILED));
        EXPECT_TRUE(pStaInterface->ConnectivityManager(scanInfos) == WIFI_OPT_FAILED);
    }

    void SetCountryCodeSuceess()
    {
        std::string countryCode;
        EXPECT_CALL(*pMockStaService, SetCountryCode(_)).WillRepeatedly(Return(WIFI_OPT_SUCCESS));
        EXPECT_TRUE(pStaInterface->SetCountryCode(countryCode) == WIFI_OPT_SUCCESS);
    }

    void SetCountryCodeFail()
    {
        std::string countryCode;
        EXPECT_CALL(*pMockStaService, SetCountryCode(_)).WillRepeatedly(Return(WIFI_OPT_FAILED));
        EXPECT_TRUE(pStaInterface->SetCountryCode(countryCode) == WIFI_OPT_FAILED);
    }
};

extern "C" IStaService *Create(void);
extern "C" void Destroy(IStaService *pservice);

HWTEST_F(StaInterfaceTest, CreateSuccess, TestSize.Level1)
{
    Create();
}

HWTEST_F(StaInterfaceTest, DestroySuccess, TestSize.Level1)
{
    IStaService *pservice = nullptr;
    Destroy(pservice);
}

HWTEST_F(StaInterfaceTest, EnableWifiSuccess, TestSize.Level1)
{
    EnableWifiSuccess();
}

HWTEST_F(StaInterfaceTest, DisableWifiSuceess, TestSize.Level1)
{
    DisableWifiSuceess();
}

HWTEST_F(StaInterfaceTest, DisableWifiFail, TestSize.Level1)
{
    DisableWifiFail();
}

HWTEST_F(StaInterfaceTest, ConnectToNetworkIdSuceess, TestSize.Level1)
{
    ConnectToNetworkIdSuceess();
}

HWTEST_F(StaInterfaceTest, ConnectToNetworkIdFail1, TestSize.Level1)
{
    ConnectToNetworkIdFail1();
}

HWTEST_F(StaInterfaceTest, ConnectToConfigSuceess, TestSize.Level1)
{
    ConnectToConfigSuceess();
}

HWTEST_F(StaInterfaceTest, ConnectToConfigFail1, TestSize.Level1)
{
    ConnectToConfigFail1();
}

HWTEST_F(StaInterfaceTest, ReAssociateSuceess, TestSize.Level1)
{
    ReAssociateSuceess();
}

HWTEST_F(StaInterfaceTest, ReAssociateFail1, TestSize.Level1)
{
    ReAssociateFail1();
}

HWTEST_F(StaInterfaceTest, DisconnectSuceess, TestSize.Level1)
{
    DisconnectSuceess();
}

HWTEST_F(StaInterfaceTest, DisconnectFail1, TestSize.Level1)
{
    DisconnectFail1();
}

HWTEST_F(StaInterfaceTest, AddDeviceConfigSuceess, TestSize.Level1)
{
    AddDeviceConfigSuceess();
}

HWTEST_F(StaInterfaceTest, AddDeviceConfigFail1, TestSize.Level1)
{
    AddDeviceConfigFail1();
}

HWTEST_F(StaInterfaceTest, UpdateDeviceConfigSuceess, TestSize.Level1)
{
    UpdateDeviceConfigSuceess();
}

HWTEST_F(StaInterfaceTest, UpdateDeviceConfigFail1, TestSize.Level1)
{
    UpdateDeviceConfigFail1();
}

HWTEST_F(StaInterfaceTest, RemoveDeviceConfigSuceess, TestSize.Level1)
{
    RemoveDeviceConfigSuceess();
}

HWTEST_F(StaInterfaceTest, RemoveDeviceConfigFail1, TestSize.Level1)
{
    RemoveDeviceConfigFail1();
}

HWTEST_F(StaInterfaceTest, EnableDeviceConfigSuceess, TestSize.Level1)
{
    EnableDeviceConfigSuceess();
}

HWTEST_F(StaInterfaceTest, EnableDeviceConfigFail1, TestSize.Level1)
{
    EnableDeviceConfigFail1();
}

HWTEST_F(StaInterfaceTest, DisableDeviceConfigSuceess, TestSize.Level1)
{
    DisableDeviceConfigSuceess();
}

HWTEST_F(StaInterfaceTest, DisableDeviceConfigFail1, TestSize.Level1)
{
    DisableDeviceConfigFail1();
}

HWTEST_F(StaInterfaceTest, StartWpsSuceess, TestSize.Level1)
{
    StartWpsSuceess();
}

HWTEST_F(StaInterfaceTest, StartWpsFail1, TestSize.Level1)
{
    StartWpsFail1();
}

HWTEST_F(StaInterfaceTest, CancelWpsSuceess, TestSize.Level1)
{
    CancelWpsSuceess();
}

HWTEST_F(StaInterfaceTest, CancelWpsFail1, TestSize.Level1)
{
    CancelWpsFail1();
}

HWTEST_F(StaInterfaceTest, AutoConnectServiceSuceess, TestSize.Level1)
{
    AutoConnectServiceSuceess();
}

HWTEST_F(StaInterfaceTest, AutoConnectServiceFail, TestSize.Level1)
{
    AutoConnectServiceFail();
}

HWTEST_F(StaInterfaceTest, SetCountryCodeSuceess, TestSize.Level1)
{
    SetCountryCodeSuceess();
}

HWTEST_F(StaInterfaceTest, SetCountryCodeFail, TestSize.Level1)
{
    SetCountryCodeFail();
}

HWTEST_F(StaInterfaceTest, RegisterStaServiceCallbackSuceess, TestSize.Level1)
{
    StaServiceCallback callbacks;
    pStaInterface->RegisterStaServiceCallback(callbacks);
}
} // namespace Wifi
} // namespace OHOS
