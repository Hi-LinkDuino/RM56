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
#include "sta_service.h"
#include <gtest/gtest.h>
#include "mock_wifi_manager.h"
#include "mock_wifi_settings.h"
#include "mock_sta_state_machine.h"
#include "mock_wifi_sta_hal_interface.h"
#include "mock_wifi_supplicant_hal_interface.h"
#include "mock_dhcp_service.h"
#include "mock_sta_auto_connect_service.h"
#include "sta_define.h"
#include "wifi_msg.h"
#include "wifi_internal_msg.h"

using ::testing::_;
using ::testing::AtLeast;
using ::testing::DoAll;
using ::testing::Eq;
using ::testing::Ref;
using ::testing::Return;
using ::testing::SetArgReferee;
using ::testing::StrEq;
using ::testing::TypedEq;
using ::testing::ext::TestSize;

namespace OHOS {
namespace Wifi {
constexpr int NETWORK_ID = 15;
constexpr int BAND = 2;
constexpr int FREQUENCY = 2437;
constexpr int TIMESTAMP = -750366468;

class StaServiceTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    virtual void SetUp()
    {
        pStaService = std::make_unique<StaService>();
        pStaService->pStaStateMachine = new MockStaStateMachine();
        pStaService->pStaAutoConnectService = new MockStaAutoConnectService(pStaService->pStaStateMachine);
    }
    virtual void TearDown()
    {
        pStaService.reset();
    }

    void StaServiceInitStaServiceSuccess();
    void StaServiceEnableWifiSuccess();
    void StaServiceEnableWifiFail();
    void StaServiceDisableWifiSuccess();
    void StaServiceDisableWifiFail();
    void StaServiceConnectToWifiDeviceConfigSuccess();
    void StaServiceConnectToWifiDeviceConfigFail1();
    void StaServiceConnectToWifiDeviceConfigFail2();
    void StaServiceConnectToWifiDeviceConfigFail3();
    void StaServiceConnectToNetworkIdSuccess();
    void StaServiceConnectToNetworkIdFail1();
    void StaServiceReAssociateSuccess();
    void StaServiceAddDeviceConfigSuccess();
    void StaServiceAddDeviceConfigFail1();
    void StaServiceAddDeviceConfigFail2();
    void StaServiceAddDeviceConfigFail3();
    void StaServiceUpdateDeviceConfigSuccess();
    void StaServiceRemoveDeviceConfigSuccess();
    void StaServiceRemoveDeviceConfigFail1();
    void StaServiceEnableDeviceConfigSuccess();
    void StaServiceEnableDeviceConfigFail1();
    void StaServiceEnableDeviceConfigFail2();
    void StaServiceDisableDeviceConfigSuccess();
    void StaServiceDisableDeviceConfigFail1();
    void StaServiceDisconnectSuccess();
    void StaServiceStartWpsSuccess();
    void StaServiceCancelWpsSuccess();
    void StaServiceSetCountryCodeSuccess();
    void StaServiceSetCountryCodeFail();
    void StaServiceAutoConnectServiceSuccess();
    void StaServiceRegisterStaServiceCallbackSuccess();
    void StaServiceRegisterStaServiceCallbackFail();
public:
    std::unique_ptr<StaService> pStaService;
};

void StaServiceTest::StaServiceInitStaServiceSuccess()
{
    WifiIdlRoamCapability capability;
    capability.maxBlocklistSize = 1;

    EXPECT_CALL(WifiStaHalInterface::GetInstance(), RegisterStaEventCallback(_))
        .WillRepeatedly(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    EXPECT_CALL(WifiStaHalInterface::GetInstance(), StartWifi()).WillRepeatedly(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    EXPECT_CALL(WifiStaHalInterface::GetInstance(), WpaAutoConnect(_))
        .WillRepeatedly(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    EXPECT_CALL(WifiStaHalInterface::GetInstance(), GetStaDeviceMacAddress(_))
        .WillRepeatedly(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    EXPECT_CALL(WifiSettings::GetInstance(), SaveLinkedInfo(_)).WillRepeatedly(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    EXPECT_CALL(WifiStaHalInterface::GetInstance(), GetStaCapabilities(_)).Times(AtLeast(0));
    EXPECT_CALL(WifiSettings::GetInstance(), GetScoretacticsScoreSlope()).Times(AtLeast(0));
    EXPECT_CALL(WifiSettings::GetInstance(), GetScoretacticsInitScore()).Times(AtLeast(0));
    EXPECT_CALL(WifiSettings::GetInstance(), GetScoretacticsSameBssidScore()).Times(AtLeast(0));
    EXPECT_CALL(WifiSettings::GetInstance(), GetScoretacticsSameNetworkScore()).Times(AtLeast(0));
    EXPECT_CALL(WifiSettings::GetInstance(), GetScoretacticsFrequency5GHzScore()).Times(AtLeast(0));
    EXPECT_CALL(WifiSettings::GetInstance(), GetScoretacticsLastSelectionScore()).Times(AtLeast(0));
    EXPECT_CALL(WifiSettings::GetInstance(), GetScoretacticsSecurityScore()).Times(AtLeast(0));
    EXPECT_CALL(WifiSettings::GetInstance(), GetSavedDeviceAppraisalPriority()).Times(AtLeast(0));
    EXPECT_CALL(WifiSettings::GetInstance(), GetExternDeviceAppraisalPriority()).Times(AtLeast(0));
    EXPECT_CALL(WifiSettings::GetInstance(), ReloadDeviceConfig()).Times(AtLeast(0));
    EXPECT_CALL(WifiStaHalInterface::GetInstance(), ClearDeviceConfig()).Times(AtLeast(0));
    EXPECT_CALL(WifiStaHalInterface::GetInstance(), SaveDeviceConfig()).Times(AtLeast(0));
    EXPECT_CALL(WifiStaHalInterface::GetInstance(), GetDeviceConfig(_)).Times(AtLeast(0));

    EXPECT_TRUE(pStaService->InitStaService(WifiManager::GetInstance().GetStaCallback()) == WIFI_OPT_SUCCESS);
}

void StaServiceTest::StaServiceEnableWifiSuccess()
{
    EXPECT_TRUE(pStaService->EnableWifi() == WIFI_OPT_SUCCESS);
}

void StaServiceTest::StaServiceEnableWifiFail()
{
}

void StaServiceTest::StaServiceDisableWifiSuccess()
{
    EXPECT_TRUE(pStaService->DisableWifi() == WIFI_OPT_SUCCESS);
}

void StaServiceTest::StaServiceDisableWifiFail()
{
}

void StaServiceTest::StaServiceConnectToWifiDeviceConfigSuccess()
{
    WifiDeviceConfig config;
    config.bssid = "01:23:45:67:89:AB";
    config.band = BAND;
    config.networkId = NETWORK_ID;
    config.ssid = "networkId";
    config.keyMgmt = "123456";

    WifiLinkedInfo info;
    EXPECT_CALL(WifiSettings::GetInstance(), GetLinkedInfo(_)).Times(AtLeast(0));
    EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_, _))
    .Times(AtLeast(0)).WillOnce(DoAll(SetArgReferee<1>(config), Return(0)));

    EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(config.ssid, config.keyMgmt, _))
        .WillOnce(DoAll(SetArgReferee<2>(config), Return(0)));
    EXPECT_CALL(WifiStaHalInterface::GetInstance(), SetDeviceConfig(_, _))
        .WillRepeatedly(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    EXPECT_CALL(WifiSettings::GetInstance(), AddDeviceConfig(_)).Times(AtLeast(1));
    EXPECT_CALL(WifiSettings::GetInstance(), SyncDeviceConfig()).Times(AtLeast(1));
    EXPECT_CALL(WifiStaHalInterface::GetInstance(), SaveDeviceConfig())
        .WillRepeatedly(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    EXPECT_TRUE(pStaService->ConnectToDevice(config) == WIFI_OPT_SUCCESS);
}

void StaServiceTest::StaServiceConnectToWifiDeviceConfigFail1()
{
    WifiDeviceConfig config;
    config.bssid = "01:23:45:67:89:AB";
    config.band = BAND;
    config.networkId = NETWORK_ID;
    config.keyMgmt = "123456";
    config.ssid = "networkId";

    WifiLinkedInfo info;
    EXPECT_CALL(WifiSettings::GetInstance(), GetLinkedInfo(_)).Times(AtLeast(0));
    EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_, _))
    .Times(AtLeast(0)).WillOnce(DoAll(SetArgReferee<1>(config), Return(0)));

    EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(config.ssid, config.keyMgmt, _))
        .WillOnce(DoAll(SetArgReferee<2>(config), Return(-1)));
    EXPECT_CALL(WifiStaHalInterface::GetInstance(), GetNextNetworkId(_))
        .WillRepeatedly(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    EXPECT_CALL(WifiStaHalInterface::GetInstance(), SetDeviceConfig(_, _))
        .WillRepeatedly(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    EXPECT_CALL(WifiSettings::GetInstance(), AddDeviceConfig(_)).Times(AtLeast(1));
    EXPECT_CALL(WifiSettings::GetInstance(), SyncDeviceConfig()).Times(AtLeast(1));
    EXPECT_CALL(WifiStaHalInterface::GetInstance(), SaveDeviceConfig())
        .WillRepeatedly(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    EXPECT_TRUE(pStaService->ConnectToDevice(config) == WIFI_OPT_FAILED);
}

void StaServiceTest::StaServiceConnectToWifiDeviceConfigFail2()
{
    WifiDeviceConfig config;
    config.bssid = "01:23:45:67:89:AB";
    config.band = BAND;
    config.networkId = NETWORK_ID;
    config.keyMgmt = "123456";
    config.ssid = "networkId";

    WifiLinkedInfo info;
    EXPECT_CALL(WifiSettings::GetInstance(), GetLinkedInfo(_)).Times(AtLeast(0));
    EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_, _))
    .Times(AtLeast(0)).WillOnce(DoAll(SetArgReferee<1>(config), Return(0)));

    EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(config.ssid, config.keyMgmt, _))
        .WillOnce(DoAll(SetArgReferee<2>(config), Return(-1)));
    EXPECT_CALL(WifiStaHalInterface::GetInstance(), GetNextNetworkId(_))
        .WillRepeatedly(Return(WifiErrorNo::WIFI_IDL_OPT_FAILED));
    EXPECT_TRUE(pStaService->ConnectToDevice(config) == WIFI_OPT_FAILED);
}


void StaServiceTest::StaServiceConnectToWifiDeviceConfigFail3()
{
    WifiDeviceConfig config;
    config.bssid = "01:23:45:67:89:AB";
    config.band = BAND;
    config.networkId = NETWORK_ID;
    config.keyMgmt = "123456";
    config.ssid = "networkId";

    WifiLinkedInfo info;
    EXPECT_CALL(WifiSettings::GetInstance(), GetLinkedInfo(_)).Times(AtLeast(0));
    EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_, _))
    .Times(AtLeast(0)).WillOnce(DoAll(SetArgReferee<1>(config), Return(0)));

    EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(config.ssid, config.keyMgmt, _))
        .WillOnce(DoAll(SetArgReferee<2>(config), Return(0)));
    EXPECT_CALL(WifiStaHalInterface::GetInstance(), SetDeviceConfig(_, _))
        .WillRepeatedly(Return(WifiErrorNo::WIFI_IDL_OPT_FAILED));
    EXPECT_TRUE(pStaService->ConnectToDevice(config) == WIFI_OPT_FAILED);
}

void StaServiceTest::StaServiceConnectToNetworkIdSuccess()
{
    int networkId = NETWORK_ID;
    EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_, _)).WillRepeatedly(Return(0));
    EXPECT_TRUE(pStaService->ConnectToNetwork(networkId) == WIFI_OPT_SUCCESS);
}

void StaServiceTest::StaServiceConnectToNetworkIdFail1()
{
    int networkId = NETWORK_ID;
    EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_, _)).WillRepeatedly(Return(-1));
    EXPECT_TRUE(pStaService->ConnectToNetwork(networkId) == WIFI_OPT_FAILED);
}

void StaServiceTest::StaServiceReAssociateSuccess()
{
    EXPECT_TRUE(pStaService->ReAssociate() == WIFI_OPT_SUCCESS);
}

void StaServiceTest::StaServiceAddDeviceConfigSuccess()
{
    WifiDeviceConfig config;
    config.bssid = "01:23:45:67:89:AB";
    config.band = 1;
    config.networkId = NETWORK_ID;
    config.ssid = "networkId";
    config.keyMgmt = "123456";

    WifiLinkedInfo info;
    EXPECT_CALL(WifiSettings::GetInstance(), GetLinkedInfo(_)).Times(AtLeast(0));
    EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_, _))
    .Times(AtLeast(0)).WillOnce(DoAll(SetArgReferee<1>(config), Return(0)));

    EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(config.ssid, config.keyMgmt, _))
        .WillOnce(DoAll(SetArgReferee<2>(config), Return(0)));
    EXPECT_CALL(WifiStaHalInterface::GetInstance(), GetNextNetworkId(_))
        .WillRepeatedly(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    EXPECT_CALL(WifiStaHalInterface::GetInstance(), SetDeviceConfig(_, _))
        .WillRepeatedly(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    EXPECT_CALL(WifiSettings::GetInstance(), AddDeviceConfig(_)).Times(AtLeast(1));
    EXPECT_CALL(WifiSettings::GetInstance(), SyncDeviceConfig()).Times(AtLeast(1));
    EXPECT_CALL(WifiStaHalInterface::GetInstance(), SaveDeviceConfig())
        .WillRepeatedly(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    EXPECT_TRUE(pStaService->AddDeviceConfig(config) != INVALID_NETWORK_ID);
}

void StaServiceTest::StaServiceAddDeviceConfigFail1()
{
    WifiDeviceConfig config;
    config.bssid = "01:23:45:67:89:AB";
    config.band = BAND;
    config.networkId = NETWORK_ID;
    config.ssid = "networkId";
    config.keyMgmt = "123456";

    WifiLinkedInfo info;
    EXPECT_CALL(WifiSettings::GetInstance(), GetLinkedInfo(_)).Times(AtLeast(0));
    EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_, _))
    .Times(AtLeast(0)).WillOnce(DoAll(SetArgReferee<1>(config), Return(0)));

    EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(config.ssid, config.keyMgmt, _))
        .WillOnce(DoAll(SetArgReferee<2>(config), Return(-1)));
    EXPECT_CALL(WifiStaHalInterface::GetInstance(), GetNextNetworkId(_))
        .WillRepeatedly(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    EXPECT_CALL(WifiStaHalInterface::GetInstance(), SetDeviceConfig(_, _))
        .WillRepeatedly(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    EXPECT_CALL(WifiSettings::GetInstance(), AddDeviceConfig(_)).Times(AtLeast(1));
    EXPECT_CALL(WifiSettings::GetInstance(), SyncDeviceConfig()).Times(AtLeast(1));
    EXPECT_CALL(WifiStaHalInterface::GetInstance(), SaveDeviceConfig())
        .WillRepeatedly(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    EXPECT_TRUE(pStaService->AddDeviceConfig(config) == INVALID_NETWORK_ID);
}

void StaServiceTest::StaServiceAddDeviceConfigFail2()
{
    WifiDeviceConfig config;
    config.bssid = "01:23:45:67:89:AB";
    config.band = BAND;
    config.networkId = NETWORK_ID;
    config.ssid = "networkId";
    config.keyMgmt = "123456";

    WifiLinkedInfo info;
    EXPECT_CALL(WifiSettings::GetInstance(), GetLinkedInfo(_)).Times(AtLeast(0));
    EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_, _))
    .Times(AtLeast(0)).WillOnce(DoAll(SetArgReferee<1>(config), Return(0)));

    EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(config.ssid, config.keyMgmt, _))
        .WillOnce(DoAll(SetArgReferee<2>(config), Return(-1)));
    EXPECT_CALL(WifiStaHalInterface::GetInstance(), GetNextNetworkId(_))
        .WillRepeatedly(Return(WifiErrorNo::WIFI_IDL_OPT_FAILED));
    EXPECT_TRUE(pStaService->AddDeviceConfig(config) == INVALID_NETWORK_ID);
}


void StaServiceTest::StaServiceAddDeviceConfigFail3()
{
    WifiDeviceConfig config;
    config.bssid = "01:23:45:67:89:AB";
    config.band = BAND;
    config.networkId = NETWORK_ID;
    config.ssid = "networkId";
    config.keyMgmt = "123456";

    WifiLinkedInfo info;
    EXPECT_CALL(WifiSettings::GetInstance(), GetLinkedInfo(_)).Times(AtLeast(0));
    EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_, _))
    .Times(AtLeast(0)).WillOnce(DoAll(SetArgReferee<1>(config), Return(0)));

    EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(config.ssid, config.keyMgmt, _))
        .WillOnce(DoAll(SetArgReferee<2>(config), Return(0)));
    EXPECT_CALL(WifiStaHalInterface::GetInstance(), SetDeviceConfig(_, _))
        .WillRepeatedly(Return(WifiErrorNo::WIFI_IDL_OPT_FAILED));
    EXPECT_TRUE(pStaService->AddDeviceConfig(config) == INVALID_NETWORK_ID);
}

void StaServiceTest::StaServiceUpdateDeviceConfigSuccess()
{
    WifiDeviceConfig config;
    config.bssid = "01:23:45:67:89:A1";
    config.band = 2;
    config.networkId = NETWORK_ID;
    config.ssid = "networkId";
    config.keyMgmt = "12345678";

    WifiLinkedInfo info;
    EXPECT_CALL(WifiSettings::GetInstance(), GetLinkedInfo(_)).Times(AtLeast(0));
    EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_, _))
    .Times(AtLeast(0)).WillOnce(DoAll(SetArgReferee<1>(config), Return(0)));

    EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(config.ssid, config.keyMgmt, _))
        .WillOnce(DoAll(SetArgReferee<2>(config), Return(0)));
    EXPECT_CALL(WifiStaHalInterface::GetInstance(), GetNextNetworkId(_))
        .WillRepeatedly(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    EXPECT_CALL(WifiStaHalInterface::GetInstance(), SetDeviceConfig(_, _))
        .WillRepeatedly(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    EXPECT_CALL(WifiSettings::GetInstance(), AddDeviceConfig(_)).Times(AtLeast(1));
    EXPECT_CALL(WifiSettings::GetInstance(), SyncDeviceConfig()).Times(AtLeast(1));
    EXPECT_CALL(WifiStaHalInterface::GetInstance(), SaveDeviceConfig())
        .WillRepeatedly(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    EXPECT_TRUE(pStaService->UpdateDeviceConfig(config) != INVALID_NETWORK_ID);
}

void StaServiceTest::StaServiceRemoveDeviceConfigSuccess()
{
    int networkId = NETWORK_ID;
    EXPECT_CALL(WifiStaHalInterface::GetInstance(), RemoveDevice(_))
        .WillRepeatedly(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    EXPECT_CALL(WifiStaHalInterface::GetInstance(), SaveDeviceConfig())
        .WillRepeatedly(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_, _)).Times(AtLeast(0)).WillRepeatedly(Return(0));
    EXPECT_CALL(WifiSettings::GetInstance(), RemoveDevice(_)).Times(AtLeast(1));
    EXPECT_CALL(WifiSettings::GetInstance(), SyncDeviceConfig()).Times(AtLeast(1));
    EXPECT_TRUE(pStaService->RemoveDevice(networkId) == WIFI_OPT_SUCCESS);
}

void StaServiceTest::StaServiceRemoveDeviceConfigFail1()
{
    int networkId = NETWORK_ID;
    EXPECT_CALL(WifiStaHalInterface::GetInstance(), RemoveDevice(_))
        .WillRepeatedly(Return(WifiErrorNo::WIFI_IDL_OPT_FAILED));
    EXPECT_TRUE(pStaService->RemoveDevice(networkId) == WIFI_OPT_FAILED);
}

void StaServiceTest::StaServiceEnableDeviceConfigSuccess()
{
    int networkId = NETWORK_ID;
    bool attemptEnable = true;
    EXPECT_CALL(WifiSettings::GetInstance(),
        SetDeviceState(networkId, (int)WifiDeviceConfigStatus::ENABLED, attemptEnable))
        .WillRepeatedly(Return(0));
    EXPECT_TRUE(pStaService->EnableDeviceConfig(networkId, attemptEnable) == WIFI_OPT_SUCCESS);
}

void StaServiceTest::StaServiceEnableDeviceConfigFail1()
{
    int networkId = NETWORK_ID;
    bool attemptEnable = true;
    EXPECT_CALL(WifiSettings::GetInstance(),
        SetDeviceState(networkId, (int)WifiDeviceConfigStatus::ENABLED, attemptEnable))
        .WillRepeatedly(Return(-1));
    EXPECT_TRUE(pStaService->EnableDeviceConfig(networkId, attemptEnable) == WIFI_OPT_FAILED);
}

void StaServiceTest::StaServiceEnableDeviceConfigFail2()
{
    int networkId = NETWORK_ID;
    bool attemptEnable = true;
    EXPECT_TRUE(pStaService->EnableDeviceConfig(networkId, attemptEnable) == WIFI_OPT_FAILED);
}

void StaServiceTest::StaServiceDisableDeviceConfigSuccess()
{
    int networkId = NETWORK_ID;
    bool attemptEnable = false;
    EXPECT_CALL(WifiSettings::GetInstance(),
        SetDeviceState(networkId, (int)WifiDeviceConfigStatus::DISABLED, attemptEnable))
        .WillRepeatedly(Return(0));
    EXPECT_CALL(WifiStaHalInterface::GetInstance(), SaveDeviceConfig())
        .WillRepeatedly(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    EXPECT_TRUE(pStaService->DisableDeviceConfig(networkId) == WIFI_OPT_SUCCESS);
}

void StaServiceTest::StaServiceDisableDeviceConfigFail1()
{
    int networkId = NETWORK_ID;
    bool attemptEnable = false;
    EXPECT_CALL(WifiSettings::GetInstance(),
        SetDeviceState(networkId, (int)WifiDeviceConfigStatus::DISABLED, attemptEnable))
        .WillRepeatedly(Return(-1));
    EXPECT_CALL(WifiStaHalInterface::GetInstance(), SaveDeviceConfig())
        .WillRepeatedly(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    EXPECT_TRUE(pStaService->DisableDeviceConfig(networkId) == WIFI_OPT_FAILED);
}

void StaServiceTest::StaServiceDisconnectSuccess()
{
    EXPECT_CALL(WifiSettings::GetInstance(), GetLinkedInfo(_)).Times(AtLeast(1));
    EXPECT_TRUE(pStaService->Disconnect() == WIFI_OPT_SUCCESS);
}

void StaServiceTest::StaServiceStartWpsSuccess()
{
    WpsConfig config;
    config.setup = SetupMethod::PBC;
    config.pin = "12345678";
    config.bssid = "01:23:45:67:89:AB";
    EXPECT_TRUE(pStaService->StartWps(config) == WIFI_OPT_SUCCESS);
}

void StaServiceTest::StaServiceCancelWpsSuccess()
{
    EXPECT_TRUE(pStaService->CancelWps() == WIFI_OPT_SUCCESS);
}

void StaServiceTest::StaServiceSetCountryCodeSuccess()
{
    std::string countryCode = "123456";
    EXPECT_CALL(WifiSupplicantHalInterface::GetInstance(), WpaSetCountryCode(_))
        .WillRepeatedly(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    EXPECT_CALL(WifiSettings::GetInstance(), SetCountryCode(_)).Times(AtLeast(1));
    EXPECT_TRUE(pStaService->SetCountryCode(countryCode) == WIFI_OPT_SUCCESS);
}

void StaServiceTest::StaServiceSetCountryCodeFail()
{
    std::string countryCode = "123456";
    EXPECT_CALL(WifiSupplicantHalInterface::GetInstance(), WpaSetCountryCode(_))
        .WillRepeatedly(Return(WifiErrorNo::WIFI_IDL_OPT_FAILED));
    EXPECT_TRUE(pStaService->SetCountryCode(countryCode) == WIFI_OPT_FAILED);
}

void StaServiceTest::StaServiceAutoConnectServiceSuccess()
{
    std::vector<InterScanInfo> scanInfos;
    InterScanInfo scanInfo;
    scanInfo.bssid = "2a:76:93:47:e2:8a";
    scanInfo.ssid = "HMWIFI_W2_EAP_G2_03";
    scanInfo.capabilities = "[RSN-EAP-CCMP][WPA2-EAP-CCMP][ESS]";
    scanInfo.frequency = FREQUENCY;
    scanInfo.timestamp = TIMESTAMP;
    scanInfos.push_back(scanInfo);
    EXPECT_TRUE(pStaService->AutoConnectService(scanInfos) == WIFI_OPT_SUCCESS);
}

void StaServiceTest::StaServiceRegisterStaServiceCallbackSuccess()
{
    WifiManager &instance = WifiManager::GetInstance();
    pStaService->RegisterStaServiceCallback(instance.GetStaCallback());
}

void StaServiceTest::StaServiceRegisterStaServiceCallbackFail()
{
    WifiManager &instance = WifiManager::GetInstance();
    pStaService->RegisterStaServiceCallback(instance.GetStaCallback());
}

HWTEST_F(StaServiceTest, StaServiceInitStaServiceSuccess, TestSize.Level1)
{
    StaServiceInitStaServiceSuccess();
}

HWTEST_F(StaServiceTest, StaServiceEnableWifiSuccess, TestSize.Level1)
{
    StaServiceEnableWifiSuccess();
}

HWTEST_F(StaServiceTest, StaServiceEnableWifiFail, TestSize.Level1)
{
    StaServiceEnableWifiFail();
}

HWTEST_F(StaServiceTest, StaServiceDisableWifiSuccess, TestSize.Level1)
{
    StaServiceDisableWifiSuccess();
}

HWTEST_F(StaServiceTest, StaServiceDisableWifiFail, TestSize.Level1)
{
    StaServiceDisableWifiFail();
}

HWTEST_F(StaServiceTest, StaServiceConnectToWifiDeviceConfigSuccess, TestSize.Level1)
{
    StaServiceConnectToWifiDeviceConfigSuccess();
}

HWTEST_F(StaServiceTest, StaServiceConnectToWifiDeviceConfigFail1, TestSize.Level1)
{
    StaServiceConnectToWifiDeviceConfigFail1();
}

HWTEST_F(StaServiceTest, StaServiceConnectToWifiDeviceConfigFail2, TestSize.Level1)
{
    StaServiceConnectToWifiDeviceConfigFail2();
}

HWTEST_F(StaServiceTest, StaServiceConnectToWifiDeviceConfigFail3, TestSize.Level1)
{
    StaServiceConnectToWifiDeviceConfigFail3();
}

HWTEST_F(StaServiceTest, StaServiceConnectToNetworkIdSuccess, TestSize.Level1)
{
    StaServiceConnectToNetworkIdSuccess();
}

HWTEST_F(StaServiceTest, StaServiceConnectToNetworkIdFail1, TestSize.Level1)
{
    StaServiceConnectToNetworkIdFail1();
}

HWTEST_F(StaServiceTest, StaServiceReAssociateSuccess, TestSize.Level1)
{
    StaServiceReAssociateSuccess();
}

HWTEST_F(StaServiceTest, StaServiceAddDeviceConfigSuccess, TestSize.Level1)
{
    StaServiceAddDeviceConfigSuccess();
}

HWTEST_F(StaServiceTest, StaServiceAddDeviceConfigFail1, TestSize.Level1)
{
    StaServiceAddDeviceConfigFail1();
}

HWTEST_F(StaServiceTest, StaServiceAddDeviceConfigFail2, TestSize.Level1)
{
    StaServiceAddDeviceConfigFail2();
}

HWTEST_F(StaServiceTest, StaServiceAddDeviceConfigFail3, TestSize.Level1)
{
    StaServiceAddDeviceConfigFail3();
}

HWTEST_F(StaServiceTest, StaServiceUpdateDeviceConfigSuccess, TestSize.Level1)
{
    StaServiceUpdateDeviceConfigSuccess();
}

HWTEST_F(StaServiceTest, StaServiceRemoveDeviceConfigSuccess, TestSize.Level1)
{
    StaServiceRemoveDeviceConfigSuccess();
}

HWTEST_F(StaServiceTest, StaServiceRemoveDeviceConfigFail1, TestSize.Level1)
{
    StaServiceRemoveDeviceConfigFail1();
}

HWTEST_F(StaServiceTest, StaServiceEnableDeviceConfigSuccess, TestSize.Level1)
{
    StaServiceEnableDeviceConfigSuccess();
}

HWTEST_F(StaServiceTest, StaServiceEnableDeviceConfigFail1, TestSize.Level1)
{
    StaServiceEnableDeviceConfigFail1();
}

HWTEST_F(StaServiceTest, StaServiceEnableDeviceConfigFail2, TestSize.Level1)
{
    StaServiceEnableDeviceConfigFail2();
}

HWTEST_F(StaServiceTest, StaServiceDisableDeviceConfigSuccess, TestSize.Level1)
{
    StaServiceDisableDeviceConfigSuccess();
}

HWTEST_F(StaServiceTest, StaServiceDisableDeviceConfigFail1, TestSize.Level1)
{
    StaServiceDisableDeviceConfigFail1();
}

HWTEST_F(StaServiceTest, StaServiceDisconnectSuccess, TestSize.Level1)
{
    StaServiceDisconnectSuccess();
}

HWTEST_F(StaServiceTest, StaServiceStartWpsSuccess, TestSize.Level1)
{
    StaServiceStartWpsSuccess();
}

HWTEST_F(StaServiceTest, StaServiceCancelWpsSuccess, TestSize.Level1)
{
    StaServiceCancelWpsSuccess();
}

HWTEST_F(StaServiceTest, StaServiceSetCountryCodeSuccess, TestSize.Level1)
{
    StaServiceSetCountryCodeSuccess();
}

HWTEST_F(StaServiceTest, StaServiceSetCountryCodeFail, TestSize.Level1)
{
    StaServiceSetCountryCodeFail();
}

HWTEST_F(StaServiceTest, StaServiceAutoConnectServiceSuccess, TestSize.Level1)
{
    StaServiceAutoConnectServiceSuccess();
}

HWTEST_F(StaServiceTest, StaServiceRegisterStaServiceCallbackSuccess, TestSize.Level1)
{
    StaServiceRegisterStaServiceCallbackSuccess();
}

HWTEST_F(StaServiceTest, StaServiceRegisterStaServiceCallbackFail, TestSize.Level1)
{
    StaServiceRegisterStaServiceCallbackFail();
}
} // namespace Wifi
} // namespace OHOS