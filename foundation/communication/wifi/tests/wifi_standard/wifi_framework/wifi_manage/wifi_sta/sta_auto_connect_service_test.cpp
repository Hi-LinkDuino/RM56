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
#include "sta_auto_connect_service.h"
#include "mock_sta_state_machine.h"
#include "mock_wifi_sta_hal_interface.h"
#include "mock_wifi_settings.h"
#include "mock_device_appraisal.h"
#include <gtest/gtest.h>
#include <vector>

#include "wifi_errcode.h"
#include "wifi_internal_msg.h"
#include "wifi_msg.h"
#include "sta_device_appraisal.h"
#include "wifi_idl_struct.h"
#include "wifi_error_no.h"

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
constexpr int SMALLER_THAN_RSSI_DELIMITING_VALUE = -66;

class StaAutoConnectServiceTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    virtual void SetUp()
    {
        pStaStateMachine = new (std::nothrow) MockStaStateMachine();
        pStaAutoConnectService = new (std::nothrow) StaAutoConnectService(pStaStateMachine);
        pMockDeviceAppraisal = new (std::nothrow) MockDeviceAppraisal();
        InitAutoConnectService();
    }
    virtual void TearDown()
    {
        if (pStaStateMachine != nullptr) {
            delete pStaStateMachine;
            pStaStateMachine = nullptr;
        }

        if (pStaAutoConnectService != nullptr) {
            delete pStaAutoConnectService;
            pStaAutoConnectService = nullptr;
        }

        if (pMockDeviceAppraisal != nullptr) {
            delete pMockDeviceAppraisal;
            pMockDeviceAppraisal = nullptr;
        }
    }

public:
    StaStateMachine *pStaStateMachine;
    StaAutoConnectService *pStaAutoConnectService;
    MockDeviceAppraisal *pMockDeviceAppraisal;

public:
    void InitAutoConnectService();
    void GetScanInfoConfig(InterScanInfo &scanInfo) const;
    void GetWifiDeviceConfig(WifiDeviceConfig &deviceConfig) const;
    void GetInterScanInfoVector(std::vector<InterScanInfo> &scanInfos) const;
    void GetWifiLinkedInfo(WifiLinkedInfo &info);
    void GetAllDeviceInfos(WifiDeviceConfig &deviceConfig, std::vector<InterScanInfo> &scanInfos,
        std::vector<std::string> &blockedBssids, WifiLinkedInfo &info);

    void InitAutoConnectServiceSuccess();
    void ClearSecondaryStateMachinePtrSuccess();
    void OnScanResultsReadyHandlerSuccess1();
    void OnScanResultsReadyHandlerSuccess2();
    void OnScanResultsReadyHandlerSuccess3();
    void OnScanResultsReadyHandlerFail1();
    void OnScanResultsReadyHandlerFail2();
    void OnScanResultsReadyHandlerFail3();
    void OnScanResultsReadyHandlerFail4();
    void OnScanResultsReadyHandlerFail5();
    void OnScanResultsReadyHandlerFail6();
    void OnScanResultsReadyHandlerFail7();
    void EnableOrDisableBssidSuccess1();
    void EnableOrDisableBssidSuccess2();
    void EnableOrDisableBssidFail1();
    void EnableOrDisableBssidFail2();
    void EnableOrDisableBssidFail3();
    void EnableOrDisableBssidFail4();
    void AutoSelectDeviceSuccess1();
    void AutoSelectDeviceSuccess2();
    void AutoSelectDeviceSuccess3();
    void AutoSelectDeviceFail1();
    void AutoSelectDeviceFail2();
    void AutoSelectDeviceFail3();
    void AutoSelectDeviceFail4();
    void AutoSelectDeviceFail5();
    void AutoSelectDeviceFail6();
    void RegisterDeviceAppraisalSuccess();
    void RegisterDeviceAppraisalFail1();
    void RegisterDeviceAppraisalFail2();
    void ClearOvertimeBlockedBssidSuccess1();
    void ClearOvertimeBlockedBssidSuccess2();
    void GetBlockedBssidsSuccess();
    void AddOrDelBlockedBssidsSuccess();
    void AddOrDelBlockedBssidsFail();
    void ObtainRoamCapFromFirmwareSuccess();
    void ObtainRoamCapFromFirmwareFail1();
    void ObtainRoamCapFromFirmwareFail2();
    void SetRoamBlockedBssidFirmwareSuccess();
    void SetRoamBlockedBssidFirmwareFail1();
    void SetRoamBlockedBssidFirmwareFail2();
    void SetRoamBlockedBssidFirmwareFail3();
    void SetRoamBlockedBssidFirmwareFail4();
    void ConnectElectedDeviceSuccess1();
    void ConnectElectedDeviceSuccess2();
    void ConnectElectedDeviceFail1();
    void GetAvailableScanInfosSuccess();
    void AllowAutoSelectDeviceSuccess1();
    void AllowAutoSelectDeviceSuccess2();
    void AllowAutoSelectDeviceSuccess3();
    void AllowAutoSelectDeviceFail1();
    void AllowAutoSelectDeviceFail2();
    void AllowAutoSelectDeviceFail3();
    void AllowAutoSelectDeviceFail4();
    void CurrentDeviceGoodEnoughSuccess();
    void CurrentDeviceGoodEnoughFail1();
    void CurrentDeviceGoodEnoughFail2();
    void CurrentDeviceGoodEnoughFail3();
    void CurrentDeviceGoodEnoughFail4();
    void CurrentDeviceGoodEnoughFail5();
    void WhetherDevice5GAvailableSuccess();
    void WhetherDevice5GAvailableFail();
    void RoamingEncryptionModeCheckSuccess();
    void RoamingEncryptionModeCheckFail1();
    void RoamingEncryptionModeCheckFail2();
    void RoamingEncryptionModeCheckFail3();
    void RoamingEncryptionModeCheckFail4();
    void RoamingSelectionSuccess1();
    void RoamingSelectionFail1();
    void RoamingSelectionFail2();
    void RoamingSelectionFail3();
    void RoamingSelectionFail4();
    void SyncBlockedSsidFirmwareSuccess();
    void SyncBlockedSsidFirmwareFail();
};

void StaAutoConnectServiceTest::InitAutoConnectService()
{
    WifiIdlRoamCapability capability;
    capability.maxBlocklistSize = 2;

    EXPECT_CALL(WifiStaHalInterface::GetInstance(), RegisterStaEventCallback(_))
        .WillRepeatedly(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    EXPECT_CALL(WifiStaHalInterface::GetInstance(), StartWifi()).WillRepeatedly(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    EXPECT_CALL(WifiStaHalInterface::GetInstance(), WpaAutoConnect(_))
        .WillRepeatedly(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    EXPECT_CALL(WifiStaHalInterface::GetInstance(), GetStaDeviceMacAddress(_))
        .WillRepeatedly(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    EXPECT_CALL(WifiSettings::GetInstance(), SaveLinkedInfo(_)).WillRepeatedly(Return(0));
    EXPECT_CALL(WifiStaHalInterface::GetInstance(), GetStaCapabilities(_)).Times(AtLeast(0));
    EXPECT_CALL(WifiSettings::GetInstance(), GetScoretacticsScoreSlope()).Times(AtLeast(0));
    EXPECT_CALL(WifiSettings::GetInstance(), GetScoretacticsInitScore()).Times(AtLeast(0));
    EXPECT_CALL(WifiSettings::GetInstance(), GetScoretacticsSameBssidScore()).Times(AtLeast(0));
    EXPECT_CALL(WifiSettings::GetInstance(), GetScoretacticsSameNetworkScore()).Times(AtLeast(0));
    EXPECT_CALL(WifiSettings::GetInstance(), GetScoretacticsFrequency5GHzScore()).Times(AtLeast(0));
    EXPECT_CALL(WifiSettings::GetInstance(), GetScoretacticsLastSelectionScore()).Times(AtLeast(0));
    EXPECT_CALL(WifiSettings::GetInstance(), GetScoretacticsSecurityScore()).Times(AtLeast(0));
    EXPECT_CALL(WifiSettings::GetInstance(), GetSavedDeviceAppraisalPriority()).Times(AtLeast(1));
    EXPECT_CALL(WifiSettings::GetInstance(), GetExternDeviceAppraisalPriority()).Times(AtLeast(0));
    EXPECT_CALL(WifiSettings::GetInstance(), ReloadDeviceConfig()).Times(AtLeast(0));
    EXPECT_CALL(WifiStaHalInterface::GetInstance(), ClearDeviceConfig()).Times(AtLeast(0));

    EXPECT_CALL(WifiStaHalInterface::GetInstance(), GetRoamingCapabilities(_))
        .WillRepeatedly(DoAll(SetArgReferee<0>(capability), Return(WIFI_IDL_OPT_OK)));

    pStaAutoConnectService->InitAutoConnectService();
    for (int i = 0; i < MAX_APPRAISAL_NUM; i++) {
        pStaAutoConnectService->pAppraisals[i] = nullptr;
    }
    pStaAutoConnectService->pAppraisals[0] = pMockDeviceAppraisal;
}

void StaAutoConnectServiceTest::GetScanInfoConfig(InterScanInfo &scanInfo) const
{
    scanInfo.bssid = "2a:76:93:47:e2:8a";
    scanInfo.ssid = "HMWIFI_W2_EAP_G2_03";
    scanInfo.band = NETWORK_ID;
    scanInfo.rssi = RSSI;
    scanInfo.securityType = WifiSecurity::OPEN;
}

void StaAutoConnectServiceTest::GetWifiDeviceConfig(WifiDeviceConfig &deviceConfig) const
{
    deviceConfig.bssid = "2a:76:93:47:e2:8a";
    deviceConfig.band = BAND;
    deviceConfig.networkId = NETWORK_ID;
    deviceConfig.ssid = "HMWIFI_W2_EAP_G2_03";
    deviceConfig.keyMgmt = "123456";
}

void StaAutoConnectServiceTest::GetInterScanInfoVector(std::vector<InterScanInfo> &scanInfos) const
{
    InterScanInfo scanInfo;
    GetScanInfoConfig(scanInfo);
    scanInfos.push_back(scanInfo);
}

void StaAutoConnectServiceTest::GetWifiLinkedInfo(WifiLinkedInfo &info)
{
    info.detailedState = DetailedState::WORKING; // WORKING
    info.bssid = "2a:76:93:47:e2:8a";
    info.ssid = "HMWIFI_W2_EAP_G2_03";
    info.networkId = NETWORK_ID;
    info.connState = ConnState::CONNECTED;
}

void StaAutoConnectServiceTest::GetAllDeviceInfos(WifiDeviceConfig &deviceConfig, std::vector<InterScanInfo> &scanInfos,
    std::vector<std::string> &blockedBssids, WifiLinkedInfo &info)
{
    GetWifiDeviceConfig(deviceConfig);
    GetInterScanInfoVector(scanInfos);
    pStaAutoConnectService->GetBlockedBssids(blockedBssids);
    GetWifiLinkedInfo(info);
}

void StaAutoConnectServiceTest::InitAutoConnectServiceSuccess()
{
    EXPECT_CALL(WifiStaHalInterface::GetInstance(), RegisterStaEventCallback(_))
        .WillRepeatedly(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    EXPECT_CALL(WifiStaHalInterface::GetInstance(), StartWifi()).WillRepeatedly(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    EXPECT_CALL(WifiStaHalInterface::GetInstance(), WpaAutoConnect(_))
        .WillRepeatedly(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    EXPECT_CALL(WifiStaHalInterface::GetInstance(), GetStaDeviceMacAddress(_))
        .WillRepeatedly(Return(WifiErrorNo::WIFI_IDL_OPT_OK));
    EXPECT_CALL(WifiSettings::GetInstance(), SaveLinkedInfo(_)).WillRepeatedly(Return(0));
    EXPECT_CALL(WifiSettings::GetInstance(), GetScoretacticsScoreSlope())
        .Times(AtLeast(1))
        .WillOnce(Return(WIFI_IDL_OPT_OK));
    EXPECT_CALL(WifiSettings::GetInstance(), GetScoretacticsInitScore()).Times(AtLeast(0));
    EXPECT_CALL(WifiSettings::GetInstance(), GetScoretacticsSameBssidScore()).Times(AtLeast(0));
    EXPECT_CALL(WifiSettings::GetInstance(), GetScoretacticsSameNetworkScore()).Times(AtLeast(0));
    EXPECT_CALL(WifiSettings::GetInstance(), GetScoretacticsFrequency5GHzScore()).Times(AtLeast(0));
    EXPECT_CALL(WifiSettings::GetInstance(), GetScoretacticsLastSelectionScore()).Times(AtLeast(0));
    EXPECT_CALL(WifiSettings::GetInstance(), GetScoretacticsSecurityScore()).Times(AtLeast(0));
    EXPECT_CALL(WifiSettings::GetInstance(), ReloadDeviceConfig()).Times(AtLeast(0));
    EXPECT_CALL(WifiStaHalInterface::GetInstance(), ClearDeviceConfig()).Times(AtLeast(0));

    int capabilities = 1;
    EXPECT_CALL(WifiStaHalInterface::GetInstance(), GetStaCapabilities(_))
        .WillOnce(DoAll(SetArgReferee<0>(capabilities), Return(WIFI_IDL_OPT_OK)));

    WifiIdlRoamCapability capability;
    capability.maxBlocklistSize = 2;
    EXPECT_CALL(WifiStaHalInterface::GetInstance(), GetRoamingCapabilities(_))
        .WillRepeatedly(DoAll(SetArgReferee<0>(capability), Return(WIFI_IDL_OPT_OK)));
    EXPECT_CALL(WifiStaHalInterface::GetInstance(), SetRoamConfig(_)).Times(AtLeast(0));
    EXPECT_CALL(WifiSettings::GetInstance(), GetSavedDeviceAppraisalPriority()).Times(AtLeast(1));
    EXPECT_CALL(WifiSettings::GetInstance(), GetExternDeviceAppraisalPriority()).Times(AtLeast(0));

    EXPECT_TRUE(pStaAutoConnectService->InitAutoConnectService() == WIFI_OPT_SUCCESS);
}

void StaAutoConnectServiceTest::OnScanResultsReadyHandlerSuccess1()
{
    WifiDeviceConfig deviceConfig;
    std::vector<InterScanInfo> scanInfos;
    std::vector<std::string> blockedBssids;
    WifiLinkedInfo infoPrimary, infoSecondary;
    GetAllDeviceInfos(deviceConfig, scanInfos, blockedBssids, infoPrimary);

    EXPECT_CALL(WifiSettings::GetInstance(), GetLinkedInfo(_))
        .WillOnce(DoAll(SetArgReferee<0>(infoPrimary), Return(0)));
    EXPECT_CALL(WifiSettings::GetInstance(), GetWhetherToAllowNetworkSwitchover()).WillOnce(Return(true));
    EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_, _))
        .WillRepeatedly(Return(-1)); // if it is false, it will do process.
    EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_)).Times(AtLeast(0));
    EXPECT_CALL(*(pMockDeviceAppraisal), DeviceAppraisals(_, _, _)).WillOnce(Return(WIFI_OPT_SUCCESS));
    pStaAutoConnectService->OnScanInfosReadyHandler(scanInfos);
}


void StaAutoConnectServiceTest::OnScanResultsReadyHandlerSuccess2()
{
    WifiDeviceConfig deviceConfig;
    std::vector<InterScanInfo> scanInfos;
    std::vector<std::string> blockedBssids;
    WifiLinkedInfo infoPrimary, infoSecondary;
    GetAllDeviceInfos(deviceConfig, scanInfos, blockedBssids, infoPrimary);
    infoPrimary.detailedState = DetailedState::DISCONNECTED; // DISCONNECTED

    EXPECT_CALL(WifiSettings::GetInstance(), GetLinkedInfo(_))
        .WillOnce(DoAll(SetArgReferee<0>(infoPrimary), Return(0)));
    EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_)).Times(AtLeast(0));
    EXPECT_CALL(*(pMockDeviceAppraisal), DeviceAppraisals(_, _, _)).WillOnce(Return(WIFI_OPT_SUCCESS));

    pStaAutoConnectService->OnScanInfosReadyHandler(scanInfos);
}


void StaAutoConnectServiceTest::OnScanResultsReadyHandlerSuccess3()
{
    WifiDeviceConfig deviceConfig;
    std::vector<InterScanInfo> scanInfos;
    std::vector<std::string> blockedBssids;
    WifiLinkedInfo infoPrimary, infoSecondary;
    GetAllDeviceInfos(deviceConfig, scanInfos, blockedBssids, infoPrimary);
    infoPrimary.detailedState = DetailedState::NOTWORKING; // NOTWORKING

    EXPECT_CALL(WifiSettings::GetInstance(), GetLinkedInfo(_))
        .WillOnce(DoAll(SetArgReferee<0>(infoPrimary), Return(0)));
    EXPECT_CALL(WifiSettings::GetInstance(), GetWhetherToAllowNetworkSwitchover())
        .Times(AtLeast(1))
        .WillOnce(Return(true));
    EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_)).Times(AtLeast(0));
    EXPECT_CALL(*(pMockDeviceAppraisal), DeviceAppraisals(_, _, _)).WillOnce(Return(WIFI_OPT_SUCCESS));

    pStaAutoConnectService->OnScanInfosReadyHandler(scanInfos);
}

void StaAutoConnectServiceTest::OnScanResultsReadyHandlerFail1()
{
    WifiDeviceConfig deviceConfig;
    std::vector<InterScanInfo> scanInfos;
    std::vector<std::string> blockedBssids;
    WifiLinkedInfo infoPrimary, infoSecondary;
    GetAllDeviceInfos(deviceConfig, scanInfos, blockedBssids, infoPrimary);

    EXPECT_CALL(WifiSettings::GetInstance(), GetLinkedInfo(_))
        .WillOnce(DoAll(SetArgReferee<0>(infoPrimary), Return(0)));
    EXPECT_CALL(WifiSettings::GetInstance(), GetWhetherToAllowNetworkSwitchover()).WillOnce(Return(false));

    pStaAutoConnectService->OnScanInfosReadyHandler(scanInfos);
}

void StaAutoConnectServiceTest::OnScanResultsReadyHandlerFail2()
{
    WifiDeviceConfig deviceConfig;
    std::vector<InterScanInfo> scanInfos;
    std::vector<std::string> blockedBssids;
    WifiLinkedInfo infoPrimary, infoSecondary;
    GetAllDeviceInfos(deviceConfig, scanInfos, blockedBssids, infoPrimary);

    EXPECT_CALL(WifiSettings::GetInstance(), GetLinkedInfo(_))
        .WillOnce(DoAll(SetArgReferee<0>(infoPrimary), Return(0)));
    EXPECT_CALL(WifiSettings::GetInstance(), GetWhetherToAllowNetworkSwitchover()).WillOnce(Return(true));
    /* CurrentDeviceGoodEnough:: There is enough devices, so need not devices at start. */
    EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_, _))
        .WillRepeatedly(Return(0)); // if it is true, it will do not process.
    EXPECT_CALL(WifiSettings::GetInstance(), GetUserLastSelectedNetworkId()).Times(AtLeast(1)).WillOnce(Return(0));
    EXPECT_CALL(WifiSettings::GetInstance(), GetUserLastSelectedNetworkTimeVal()).Times(AtLeast(0));
    EXPECT_CALL(*(pMockDeviceAppraisal), DeviceAppraisals(_, _, _)).Times(AtLeast(0));
    pStaAutoConnectService->OnScanInfosReadyHandler(scanInfos);
}

void StaAutoConnectServiceTest::OnScanResultsReadyHandlerFail3()
{
    WifiDeviceConfig deviceConfig;
    std::vector<InterScanInfo> scanInfos;
    std::vector<std::string> blockedBssids;
    WifiLinkedInfo infoPrimary, infoSecondary;
    GetAllDeviceInfos(deviceConfig, scanInfos, blockedBssids, infoPrimary);

    EXPECT_CALL(WifiSettings::GetInstance(), GetLinkedInfo(_))
        .WillOnce(DoAll(SetArgReferee<0>(infoPrimary), Return(0)));
    EXPECT_CALL(WifiSettings::GetInstance(), GetWhetherToAllowNetworkSwitchover()).WillOnce(Return(true));
    EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_, _))
        .WillRepeatedly(Return(-1)); // if it is false, it will do process.
    EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_)).Times(AtLeast(0));
    EXPECT_CALL(*(pMockDeviceAppraisal), DeviceAppraisals(_, _, _)).WillOnce(Return(WIFI_OPT_FAILED));

    pStaAutoConnectService->OnScanInfosReadyHandler(scanInfos);
}

void StaAutoConnectServiceTest::OnScanResultsReadyHandlerFail4()
{
    WifiDeviceConfig deviceConfig;
    std::vector<InterScanInfo> scanInfos;
    std::vector<std::string> blockedBssids;
    WifiLinkedInfo infoPrimary, infoSecondary;
    GetAllDeviceInfos(deviceConfig, scanInfos, blockedBssids, infoPrimary);
    infoPrimary.detailedState = DetailedState::DISCONNECTED; // DISCONNECTED

    EXPECT_CALL(WifiSettings::GetInstance(), GetLinkedInfo(_))
        .WillOnce(DoAll(SetArgReferee<0>(infoPrimary), Return(0)));
    EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_)).Times(AtLeast(0));
    EXPECT_CALL(*(pMockDeviceAppraisal), DeviceAppraisals(_, _, _)).WillOnce(Return(WIFI_OPT_FAILED));

    pStaAutoConnectService->OnScanInfosReadyHandler(scanInfos);
}

void StaAutoConnectServiceTest::OnScanResultsReadyHandlerFail5()
{
    WifiDeviceConfig deviceConfig;
    std::vector<InterScanInfo> scanInfos;
    std::vector<std::string> blockedBssids;
    WifiLinkedInfo infoPrimary, infoSecondary;
    GetAllDeviceInfos(deviceConfig, scanInfos, blockedBssids, infoPrimary);
    infoPrimary.detailedState = DetailedState::NOTWORKING; // NOTWORKING

    EXPECT_CALL(WifiSettings::GetInstance(), GetLinkedInfo(_))
        .WillOnce(DoAll(SetArgReferee<0>(infoPrimary), Return(0)));
    EXPECT_CALL(WifiSettings::GetInstance(), GetWhetherToAllowNetworkSwitchover())
        .Times(AtLeast(1))
        .WillOnce(Return(false));

    pStaAutoConnectService->OnScanInfosReadyHandler(scanInfos);
}

void StaAutoConnectServiceTest::OnScanResultsReadyHandlerFail6()
{
    WifiDeviceConfig deviceConfig;
    std::vector<InterScanInfo> scanInfos;
    std::vector<std::string> blockedBssids;
    WifiLinkedInfo infoPrimary, infoSecondary;
    GetAllDeviceInfos(deviceConfig, scanInfos, blockedBssids, infoPrimary);
    infoPrimary.detailedState = DetailedState::NOTWORKING; // NOTWORKING

    EXPECT_CALL(WifiSettings::GetInstance(), GetLinkedInfo(_))
        .WillOnce(DoAll(SetArgReferee<0>(infoPrimary), Return(0)));
    EXPECT_CALL(WifiSettings::GetInstance(), GetWhetherToAllowNetworkSwitchover())
        .Times(AtLeast(1))
        .WillOnce(Return(true));
    EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_)).Times(AtLeast(1));
    EXPECT_CALL(*(pMockDeviceAppraisal), DeviceAppraisals(_, _, _)).WillOnce(Return(WIFI_OPT_FAILED));

    pStaAutoConnectService->OnScanInfosReadyHandler(scanInfos);
}


void StaAutoConnectServiceTest::OnScanResultsReadyHandlerFail7()
{
    std::vector<InterScanInfo> scanInfos;
    WifiLinkedInfo infoPrimary, infoSecondary;
    GetInterScanInfoVector(scanInfos);
    infoPrimary.supplicantState = SupplicantState::ASSOCIATING;

    EXPECT_CALL(WifiSettings::GetInstance(), GetLinkedInfo(_))
        .WillOnce(DoAll(SetArgReferee<0>(infoPrimary), Return(0)));
    EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_)).Times(AtLeast(0));
    pStaAutoConnectService->OnScanInfosReadyHandler(scanInfos);
}

void StaAutoConnectServiceTest::EnableOrDisableBssidSuccess1()
{
    std::string bssid = "d8:c7:71:2f:14:d9";

    EXPECT_CALL(WifiStaHalInterface::GetInstance(), SetRoamConfig(_)).Times(AtLeast(0));
    EXPECT_TRUE(pStaAutoConnectService->EnableOrDisableBssid(bssid, false, AP_CANNOT_HANDLE_NEW_STA));
}

void StaAutoConnectServiceTest::EnableOrDisableBssidSuccess2()
{
    std::string bssid = "d8:c7:71:2f:14:d9";
    EXPECT_TRUE(pStaAutoConnectService->EnableOrDisableBssid(bssid, false, AP_CANNOT_HANDLE_NEW_STA));
    EXPECT_TRUE(pStaAutoConnectService->EnableOrDisableBssid(bssid, true, AP_CANNOT_HANDLE_NEW_STA));
}

void StaAutoConnectServiceTest::EnableOrDisableBssidFail1()
{
    std::string bssid = "";
    EXPECT_FALSE(pStaAutoConnectService->EnableOrDisableBssid(bssid, true, AP_CANNOT_HANDLE_NEW_STA));
}

void StaAutoConnectServiceTest::EnableOrDisableBssidFail2()
{
    std::string bssid = "";
    EXPECT_FALSE(pStaAutoConnectService->EnableOrDisableBssid(bssid, false, AP_CANNOT_HANDLE_NEW_STA));
}

void StaAutoConnectServiceTest::EnableOrDisableBssidFail3()
{
    std::string bssid = "d8:c7:71:2f:14:d9";
    EXPECT_FALSE(pStaAutoConnectService->EnableOrDisableBssid(bssid, true, AP_CANNOT_HANDLE_NEW_STA));
}

void StaAutoConnectServiceTest::EnableOrDisableBssidFail4()
{
    std::string bssid = "d8:c7:71:2f:14:d9";
    const int AP_CANNOT_HANDLE_NEW_STA_ERR = 1;
    EXPECT_FALSE(pStaAutoConnectService->EnableOrDisableBssid(bssid, false, AP_CANNOT_HANDLE_NEW_STA_ERR));
}

void StaAutoConnectServiceTest::AutoSelectDeviceSuccess1()
{
    WifiDeviceConfig deviceConfig;
    std::vector<InterScanInfo> scanInfos;
    std::vector<std::string> blockedBssids;
    WifiLinkedInfo info;
    GetAllDeviceInfos(deviceConfig, scanInfos, blockedBssids, info);

    EXPECT_CALL(WifiSettings::GetInstance(), GetWhetherToAllowNetworkSwitchover()).WillOnce(Return(true));
    EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_, _))
        .WillRepeatedly(Return(-1)); // if it is false, it will do process.
    EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_)).Times(AtLeast(0));
    EXPECT_CALL(*(pMockDeviceAppraisal), DeviceAppraisals(_, _, _)).WillOnce(Return(WIFI_OPT_SUCCESS));

    EXPECT_TRUE(pStaAutoConnectService->AutoSelectDevice(deviceConfig, scanInfos, blockedBssids, info) ==
        WIFI_OPT_SUCCESS);
}

void StaAutoConnectServiceTest::AutoSelectDeviceSuccess2()
{
    WifiDeviceConfig deviceConfig;
    std::vector<InterScanInfo> scanInfos;
    std::vector<std::string> blockedBssids;
    WifiLinkedInfo info;
    GetAllDeviceInfos(deviceConfig, scanInfos, blockedBssids, info);
    info.detailedState = DetailedState::DISCONNECTED; // DISCONNECTED

    EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_)).Times(AtLeast(0));
    EXPECT_CALL(*(pMockDeviceAppraisal), DeviceAppraisals(_, _, _)).WillOnce(Return(WIFI_OPT_SUCCESS));

    EXPECT_TRUE(pStaAutoConnectService->AutoSelectDevice(deviceConfig, scanInfos, blockedBssids, info) ==
        WIFI_OPT_SUCCESS);
}


void StaAutoConnectServiceTest::AutoSelectDeviceSuccess3()
{
    WifiDeviceConfig deviceConfig;
    std::vector<InterScanInfo> scanInfos;
    std::vector<std::string> blockedBssids;
    WifiLinkedInfo info;
    GetAllDeviceInfos(deviceConfig, scanInfos, blockedBssids, info);
    info.detailedState = DetailedState::NOTWORKING; // NOTWORKING

    EXPECT_CALL(WifiSettings::GetInstance(), GetWhetherToAllowNetworkSwitchover())
        .Times(AtLeast(1))
        .WillOnce(Return(true));
    EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_)).Times(AtLeast(0));
    EXPECT_CALL(*(pMockDeviceAppraisal), DeviceAppraisals(_, _, _)).WillOnce(Return(WIFI_OPT_SUCCESS));

    EXPECT_TRUE(pStaAutoConnectService->AutoSelectDevice(deviceConfig, scanInfos, blockedBssids, info) ==
        WIFI_OPT_SUCCESS);
}

void StaAutoConnectServiceTest::AutoSelectDeviceFail1()
{
    WifiDeviceConfig deviceConfig;
    std::vector<InterScanInfo> scanInfos;
    std::vector<std::string> blockedBssids;
    WifiLinkedInfo info;
    GetAllDeviceInfos(deviceConfig, scanInfos, blockedBssids, info);

    EXPECT_CALL(WifiSettings::GetInstance(), GetWhetherToAllowNetworkSwitchover()).WillOnce(Return(false));

    EXPECT_TRUE(pStaAutoConnectService->AutoSelectDevice(deviceConfig, scanInfos, blockedBssids, info) ==
        WIFI_OPT_FAILED);
}

void StaAutoConnectServiceTest::AutoSelectDeviceFail2()
{
    WifiDeviceConfig deviceConfig;
    std::vector<InterScanInfo> scanInfos;
    std::vector<std::string> blockedBssids;
    WifiLinkedInfo info;
    GetAllDeviceInfos(deviceConfig, scanInfos, blockedBssids, info);

    EXPECT_CALL(WifiSettings::GetInstance(), GetWhetherToAllowNetworkSwitchover()).WillOnce(Return(true));
    /* CurrentDeviceGoodEnough:: There is enough devices, so need not devices at start. */
    EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_, _))
        .WillOnce(DoAll(SetArgReferee<1>(deviceConfig), Return(0))); // if it is true, it will do not process.
    EXPECT_CALL(WifiSettings::GetInstance(), GetUserLastSelectedNetworkId()).Times(AtLeast(0)).WillOnce(Return(0));
    EXPECT_CALL(WifiSettings::GetInstance(), GetUserLastSelectedNetworkTimeVal()).Times(AtLeast(0));
    EXPECT_TRUE(pStaAutoConnectService->AutoSelectDevice(deviceConfig, scanInfos, blockedBssids, info) ==
        WIFI_OPT_FAILED);
}


void StaAutoConnectServiceTest::AutoSelectDeviceFail3()
{
    WifiDeviceConfig deviceConfig;
    std::vector<InterScanInfo> scanInfos;
    std::vector<std::string> blockedBssids;
    WifiLinkedInfo info;
    GetAllDeviceInfos(deviceConfig, scanInfos, blockedBssids, info);

    EXPECT_CALL(WifiSettings::GetInstance(), GetWhetherToAllowNetworkSwitchover()).WillOnce(Return(true));
    EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_, _))
        .WillRepeatedly(Return(-1)); // if it is false, it will do process.
    EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_)).Times(AtLeast(0));
    EXPECT_CALL(*(pMockDeviceAppraisal), DeviceAppraisals(_, _, _)).WillOnce(Return(WIFI_OPT_FAILED));

    EXPECT_TRUE(pStaAutoConnectService->AutoSelectDevice(deviceConfig, scanInfos, blockedBssids, info) ==
        WIFI_OPT_FAILED);
}

void StaAutoConnectServiceTest::AutoSelectDeviceFail4()
{
    WifiDeviceConfig deviceConfig;
    std::vector<InterScanInfo> scanInfos;
    std::vector<std::string> blockedBssids;
    WifiLinkedInfo info;
    GetAllDeviceInfos(deviceConfig, scanInfos, blockedBssids, info);
    info.detailedState = DetailedState::DISCONNECTED; // DISCONNECTED

    EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_)).Times(AtLeast(0));
    EXPECT_CALL(*(pMockDeviceAppraisal), DeviceAppraisals(_, _, _)).WillOnce(Return(WIFI_OPT_FAILED));

    EXPECT_TRUE(pStaAutoConnectService->AutoSelectDevice(deviceConfig, scanInfos, blockedBssids, info) ==
        WIFI_OPT_FAILED);
}

void StaAutoConnectServiceTest::AutoSelectDeviceFail5()
{
    WifiDeviceConfig deviceConfig;
    std::vector<InterScanInfo> scanInfos;
    std::vector<std::string> blockedBssids;
    WifiLinkedInfo info;
    GetAllDeviceInfos(deviceConfig, scanInfos, blockedBssids, info);
    info.detailedState = DetailedState::NOTWORKING; // NOTWORKING

    EXPECT_CALL(WifiSettings::GetInstance(), GetWhetherToAllowNetworkSwitchover())
        .Times(AtLeast(1))
        .WillOnce(Return(false));

    EXPECT_TRUE(pStaAutoConnectService->AutoSelectDevice(deviceConfig, scanInfos, blockedBssids, info) ==
        WIFI_OPT_FAILED);
}

void StaAutoConnectServiceTest::AutoSelectDeviceFail6()
{
    WifiDeviceConfig deviceConfig;
    std::vector<InterScanInfo> scanInfos;
    std::vector<std::string> blockedBssids;
    WifiLinkedInfo info;
    GetAllDeviceInfos(deviceConfig, scanInfos, blockedBssids, info);
    info.detailedState = DetailedState::NOTWORKING; // NOTWORKING

    EXPECT_CALL(WifiSettings::GetInstance(), GetWhetherToAllowNetworkSwitchover())
        .Times(AtLeast(1))
        .WillOnce(Return(true));
    EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_)).Times(AtLeast(0));
    EXPECT_CALL(*(pMockDeviceAppraisal), DeviceAppraisals(_, _, _)).WillOnce(Return(WIFI_OPT_FAILED));

    EXPECT_TRUE(pStaAutoConnectService->AutoSelectDevice(deviceConfig, scanInfos, blockedBssids, info) ==
        WIFI_OPT_FAILED);
}

void StaAutoConnectServiceTest::RegisterDeviceAppraisalSuccess()
{
    StaDeviceAppraisal *appraisal = nullptr;
    int priority = 1; // 0~6
    EXPECT_TRUE(pStaAutoConnectService->RegisterDeviceAppraisal(appraisal, priority) == true);
}

void StaAutoConnectServiceTest::RegisterDeviceAppraisalFail1()
{
    StaDeviceAppraisal *appraisal = nullptr;
    int priority = -1; // 0~6
    EXPECT_TRUE(pStaAutoConnectService->RegisterDeviceAppraisal(appraisal, priority) == false);
}


void StaAutoConnectServiceTest::RegisterDeviceAppraisalFail2()
{
    StaDeviceAppraisal *appraisal = nullptr;
    int priority = 7; // 0~6
    EXPECT_TRUE(pStaAutoConnectService->RegisterDeviceAppraisal(appraisal, priority) == false);
}

void StaAutoConnectServiceTest::ClearOvertimeBlockedBssidSuccess1()
{
    pStaAutoConnectService->ClearOvertimeBlockedBssid();
}

void StaAutoConnectServiceTest::ClearOvertimeBlockedBssidSuccess2()
{
    std::string bssid = "2a:76:93:47:e2:8a";
    bool enable = false;
    int reason = AP_CANNOT_HANDLE_NEW_STA;
    pStaAutoConnectService->AddOrDelBlockedBssids(bssid, enable, reason);
    pStaAutoConnectService->ClearOvertimeBlockedBssid();
}

void StaAutoConnectServiceTest::GetBlockedBssidsSuccess()
{
    std::vector<std::string> blockedBssids;
    std::string bssid = "2a:76:93:47:e2:8a";
    blockedBssids.push_back(bssid);
    pStaAutoConnectService->GetBlockedBssids(blockedBssids);
}

void StaAutoConnectServiceTest::AddOrDelBlockedBssidsSuccess()
{
    std::string bssid = "2a:76:93:47:e2:8a";
    bool enable = false;
    int reason = AP_CANNOT_HANDLE_NEW_STA;
    EXPECT_TRUE(pStaAutoConnectService->AddOrDelBlockedBssids(bssid, enable, reason) == true);
}


void StaAutoConnectServiceTest::AddOrDelBlockedBssidsFail()
{
    std::string bssid = "2a:76:93:47:e2:8a";
    bool enable = true;
    int reason = AP_CANNOT_HANDLE_NEW_STA;
    EXPECT_TRUE(pStaAutoConnectService->AddOrDelBlockedBssids(bssid, enable, reason) == false);
}

void StaAutoConnectServiceTest::ObtainRoamCapFromFirmwareSuccess()
{
    WifiIdlRoamCapability capability;
    capability.maxBlocklistSize = 2;

    EXPECT_CALL(WifiStaHalInterface::GetInstance(), GetStaCapabilities(_))
        .WillOnce(Return(WIFI_IDL_OPT_FAILED)); // if it is false, it will do process.
    EXPECT_CALL(WifiStaHalInterface::GetInstance(), GetRoamingCapabilities(_))
        .WillOnce(DoAll(SetArgReferee<0>(capability), Return(WIFI_IDL_OPT_OK)));
    EXPECT_TRUE(pStaAutoConnectService->ObtainRoamCapFromFirmware() == true);
}

void StaAutoConnectServiceTest::ObtainRoamCapFromFirmwareFail1()
{
    unsigned int capabilities = 0;

    EXPECT_CALL(WifiStaHalInterface::GetInstance(), GetStaCapabilities(_))
        .WillOnce(
        DoAll(SetArgReferee<0>(capabilities), Return(WIFI_IDL_OPT_OK))); // if it is true, it will do not process.

    EXPECT_TRUE(pStaAutoConnectService->ObtainRoamCapFromFirmware() == false);
}


void StaAutoConnectServiceTest::ObtainRoamCapFromFirmwareFail2()
{
    unsigned int capabilities = 0;
    WifiIdlRoamCapability capability;
    capability.maxBlocklistSize = 1;

    EXPECT_CALL(WifiStaHalInterface::GetInstance(), GetStaCapabilities(_))
        .WillOnce(
        DoAll(SetArgReferee<0>(capabilities), Return(WIFI_IDL_OPT_FAILED))); // if it is true, it will do not process.
    EXPECT_CALL(WifiStaHalInterface::GetInstance(), GetRoamingCapabilities(_))
        .WillOnce(DoAll(SetArgReferee<0>(capability), Return(WIFI_IDL_OPT_FAILED)));

    EXPECT_TRUE(pStaAutoConnectService->ObtainRoamCapFromFirmware() == false);
}

void StaAutoConnectServiceTest::SetRoamBlockedBssidFirmwareSuccess()
{
    ObtainRoamCapFromFirmwareSuccess();
    std::vector<std::string> blockedBssids;
    std::string bssid = "2a:76:93:47:e2:8a";
    blockedBssids.push_back(bssid);

    EXPECT_CALL(WifiStaHalInterface::GetInstance(), SetRoamConfig(_)).WillOnce(Return(WIFI_IDL_OPT_OK));
    EXPECT_TRUE(pStaAutoConnectService->SetRoamBlockedBssidFirmware(blockedBssids) == true);
}

void StaAutoConnectServiceTest::SetRoamBlockedBssidFirmwareFail1()
{
    std::vector<std::string> blockedBssids;
    std::string bssid = "2a:76:93:47:e2:8a";
    blockedBssids.push_back(bssid);

    EXPECT_CALL(WifiStaHalInterface::GetInstance(), SetRoamConfig(_)).WillOnce(Return(WIFI_IDL_OPT_FAILED));
    EXPECT_TRUE(pStaAutoConnectService->SetRoamBlockedBssidFirmware(blockedBssids) == false);
}

void StaAutoConnectServiceTest::SetRoamBlockedBssidFirmwareFail2()
{
    ObtainRoamCapFromFirmwareSuccess();
    std::vector<std::string> blockedBssids = {}; // NULL
    EXPECT_TRUE(pStaAutoConnectService->SetRoamBlockedBssidFirmware(blockedBssids) == false);
}

void StaAutoConnectServiceTest::SetRoamBlockedBssidFirmwareFail3()
{
    ObtainRoamCapFromFirmwareSuccess();
    std::vector<std::string> blockedBssids;
    std::string bssid1 = "2a:76:93:47:e2:8a";
    std::string bssid2 = "2a:76:93:47:e2:8a";
    std::string bssid3 = "2a:76:93:47:e2:8a";
    blockedBssids.push_back(bssid1);
    blockedBssids.push_back(bssid2);
    blockedBssids.push_back(bssid3);

    EXPECT_TRUE(pStaAutoConnectService->SetRoamBlockedBssidFirmware(blockedBssids) == false);
}

void StaAutoConnectServiceTest::SetRoamBlockedBssidFirmwareFail4()
{
    ObtainRoamCapFromFirmwareSuccess();
    std::vector<std::string> blockedBssids;
    std::string bssid1 = "2a:76:93:47:e2:8a";
    blockedBssids.push_back(bssid1);
    EXPECT_CALL(WifiStaHalInterface::GetInstance(), SetRoamConfig(_)).WillOnce(Return(WIFI_IDL_OPT_FAILED));
    EXPECT_TRUE(pStaAutoConnectService->SetRoamBlockedBssidFirmware(blockedBssids) == false);
}

void StaAutoConnectServiceTest::ConnectElectedDeviceSuccess1()
{
    WifiLinkedInfo info;
    WifiDeviceConfig deviceConfig;
    GetWifiLinkedInfo(info);
    GetWifiDeviceConfig(deviceConfig);
    info.connState = ConnState::CONNECTED;
    info.bssid = "2a:76:93:47:e2:8b";
    deviceConfig.networkId = INVALID_NETWORK_ID;

    EXPECT_CALL(WifiSettings::GetInstance(), GetLinkedInfo(_)).WillOnce(DoAll(SetArgReferee<0>(info), Return(0)));
    pStaAutoConnectService->ConnectElectedDevice(deviceConfig);
}

void StaAutoConnectServiceTest::ConnectElectedDeviceSuccess2()
{
    WifiLinkedInfo info;
    WifiDeviceConfig deviceConfig;
    GetWifiLinkedInfo(info);
    GetWifiDeviceConfig(deviceConfig);
    info.connState = ConnState::CONNECTED;
    info.detailedState = DetailedState::DISCONNECTED;

    EXPECT_CALL(WifiSettings::GetInstance(), GetLinkedInfo(_)).WillOnce(DoAll(SetArgReferee<0>(info), Return(0)));
    pStaAutoConnectService->ConnectElectedDevice(deviceConfig);
}

void StaAutoConnectServiceTest::ConnectElectedDeviceFail1()
{
    WifiLinkedInfo info;
    WifiDeviceConfig deviceConfig;
    GetWifiLinkedInfo(info);
    GetWifiDeviceConfig(deviceConfig);
    info.detailedState = DetailedState::INVALID;

    EXPECT_CALL(WifiSettings::GetInstance(), GetLinkedInfo(_)).WillOnce(DoAll(SetArgReferee<0>(info), Return(0)));
    pStaAutoConnectService->ConnectElectedDevice(deviceConfig);
}

void StaAutoConnectServiceTest::GetAvailableScanInfosSuccess()
{
    std::vector<InterScanInfo> availableScanInfos;
    std::vector<InterScanInfo> scanInfos;
    std::vector<std::string> blockedBssids;
    WifiLinkedInfo info;
    GetInterScanInfoVector(availableScanInfos);
    GetInterScanInfoVector(scanInfos);
    std::string bssid1 = "2a:76:93:47:e2:8a";
    blockedBssids.push_back(bssid1);
    GetWifiLinkedInfo(info);
    WifiDeviceConfig deviceConfig;
    GetWifiDeviceConfig(deviceConfig);

    EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(deviceConfig.bssid, DEVICE_CONFIG_INDEX_BSSID, _))
    .Times(AtLeast(0)).WillOnce(DoAll(SetArgReferee<2>(deviceConfig), Return(0)));

    pStaAutoConnectService->GetAvailableScanInfos(availableScanInfos, scanInfos, blockedBssids, info);
}

void StaAutoConnectServiceTest::AllowAutoSelectDeviceSuccess1()
{
    std::vector<InterScanInfo> scanInfos;
    WifiLinkedInfo info;
    GetInterScanInfoVector(scanInfos);
    GetWifiLinkedInfo(info);

    EXPECT_CALL(WifiSettings::GetInstance(), GetWhetherToAllowNetworkSwitchover()).WillOnce(Return(true));
    EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_, _))
        .WillRepeatedly(Return(-1)); // if it is false, it will do process.
    EXPECT_TRUE(pStaAutoConnectService->AllowAutoSelectDevice(scanInfos, info) == true);
}

void StaAutoConnectServiceTest::AllowAutoSelectDeviceSuccess2()
{
    std::vector<InterScanInfo> scanInfos;
    WifiLinkedInfo info;
    GetInterScanInfoVector(scanInfos);
    GetWifiLinkedInfo(info);
    info.detailedState = DetailedState::DISCONNECTED;
    EXPECT_TRUE(pStaAutoConnectService->AllowAutoSelectDevice(scanInfos, info) == true);
}

void StaAutoConnectServiceTest::AllowAutoSelectDeviceSuccess3()
{
    std::vector<InterScanInfo> scanInfos;
    WifiLinkedInfo info;
    GetInterScanInfoVector(scanInfos);
    GetWifiLinkedInfo(info);
    info.detailedState = DetailedState::NOTWORKING;

    EXPECT_CALL(WifiSettings::GetInstance(), GetWhetherToAllowNetworkSwitchover()).WillOnce(Return(true));
    EXPECT_TRUE(pStaAutoConnectService->AllowAutoSelectDevice(scanInfos, info) == true);
}

void StaAutoConnectServiceTest::AllowAutoSelectDeviceFail1()
{
    std::vector<InterScanInfo> scanInfos;
    WifiLinkedInfo info;
    GetInterScanInfoVector(scanInfos);
    GetWifiLinkedInfo(info);

    EXPECT_CALL(WifiSettings::GetInstance(), GetWhetherToAllowNetworkSwitchover()).WillOnce(Return(false));
    EXPECT_TRUE(pStaAutoConnectService->AllowAutoSelectDevice(scanInfos, info) == false);
}

void StaAutoConnectServiceTest::AllowAutoSelectDeviceFail2()
{
    WifiDeviceConfig deviceConfig;
    std::vector<InterScanInfo> scanInfos;
    WifiLinkedInfo info;
    GetWifiDeviceConfig(deviceConfig);
    GetInterScanInfoVector(scanInfos);
    GetWifiLinkedInfo(info);
    deviceConfig.networkId = 0;

    EXPECT_CALL(WifiSettings::GetInstance(), GetWhetherToAllowNetworkSwitchover()).WillOnce(Return(true));
    EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_, _))
        .WillOnce(DoAll(SetArgReferee<1>(deviceConfig), Return(0))); // if it is true, it will do not process.
    EXPECT_CALL(WifiSettings::GetInstance(), GetUserLastSelectedNetworkId()).Times(AtLeast(1)).WillOnce(Return(0));
    EXPECT_CALL(WifiSettings::GetInstance(), GetUserLastSelectedNetworkTimeVal()).Times(AtLeast(0));
    EXPECT_TRUE(pStaAutoConnectService->AllowAutoSelectDevice(scanInfos, info) == false);
}

void StaAutoConnectServiceTest::AllowAutoSelectDeviceFail3()
{
    std::vector<InterScanInfo> scanInfos;
    WifiLinkedInfo info;
    GetInterScanInfoVector(scanInfos);
    GetWifiLinkedInfo(info);
    info.detailedState = DetailedState::NOTWORKING;

    EXPECT_CALL(WifiSettings::GetInstance(), GetWhetherToAllowNetworkSwitchover()).WillOnce(Return(false));
    EXPECT_TRUE(pStaAutoConnectService->AllowAutoSelectDevice(scanInfos, info) == false);
}

void StaAutoConnectServiceTest::AllowAutoSelectDeviceFail4()
{
    std::vector<InterScanInfo> scanInfos;
    WifiLinkedInfo info;
    GetInterScanInfoVector(scanInfos);
    GetWifiLinkedInfo(info);
    info.detailedState = DetailedState::INVALID;

    EXPECT_TRUE(pStaAutoConnectService->AllowAutoSelectDevice(scanInfos, info) == false);
}

void StaAutoConnectServiceTest::CurrentDeviceGoodEnoughSuccess()
{
    WifiDeviceConfig deviceConfig;
    std::vector<InterScanInfo> scanInfos;
    WifiLinkedInfo info;
    GetWifiDeviceConfig(deviceConfig);
    GetInterScanInfoVector(scanInfos);
    GetWifiLinkedInfo(info);
    deviceConfig.networkId = 0;

    EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_, _))
        .WillOnce(DoAll(SetArgReferee<1>(deviceConfig), Return(0)));
    EXPECT_CALL(WifiSettings::GetInstance(), GetUserLastSelectedNetworkId()).Times(AtLeast(1)).WillOnce(Return(0));
    EXPECT_CALL(WifiSettings::GetInstance(), GetUserLastSelectedNetworkTimeVal()).Times(AtLeast(1));
    EXPECT_TRUE(pStaAutoConnectService->CurrentDeviceGoodEnough(scanInfos, info) == true);
}


void StaAutoConnectServiceTest::CurrentDeviceGoodEnoughFail1()
{
    WifiDeviceConfig deviceConfig;
    std::vector<InterScanInfo> scanInfos;
    WifiLinkedInfo info;
    GetWifiDeviceConfig(deviceConfig);
    GetInterScanInfoVector(scanInfos);
    GetWifiLinkedInfo(info);

    EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_, _))
        .WillOnce(DoAll(SetArgReferee<1>(deviceConfig), Return(-1)));
    EXPECT_TRUE(pStaAutoConnectService->CurrentDeviceGoodEnough(scanInfos, info) == false);
}

void StaAutoConnectServiceTest::CurrentDeviceGoodEnoughFail2()
{
    WifiDeviceConfig deviceConfig;
    std::vector<InterScanInfo> scanInfos;
    WifiLinkedInfo info;
    GetWifiDeviceConfig(deviceConfig);
    GetInterScanInfoVector(scanInfos);
    GetWifiLinkedInfo(info);
    deviceConfig.isEphemeral = true;

    EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_, _))
        .WillOnce(DoAll(SetArgReferee<1>(deviceConfig), Return(0)));
    EXPECT_CALL(WifiSettings::GetInstance(), GetUserLastSelectedNetworkId())
        .Times(AtLeast(1))
        .WillOnce(Return(INVALID_NETWORK_ID));

    EXPECT_TRUE(pStaAutoConnectService->CurrentDeviceGoodEnough(scanInfos, info) == false);
}

void StaAutoConnectServiceTest::CurrentDeviceGoodEnoughFail3()
{
    WifiDeviceConfig deviceConfig;
    std::vector<InterScanInfo> scanInfos;
    WifiLinkedInfo info;
    GetWifiDeviceConfig(deviceConfig);
    GetInterScanInfoVector(scanInfos);
    GetWifiLinkedInfo(info);
    deviceConfig.isEphemeral = false;
    deviceConfig.keyMgmt = "NONE";
    deviceConfig.wepTxKeyIndex = -1;

    EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_, _))
        .WillOnce(DoAll(SetArgReferee<1>(deviceConfig), Return(0)));
    EXPECT_CALL(WifiSettings::GetInstance(), GetUserLastSelectedNetworkId())
        .Times(AtLeast(1))
        .WillOnce(Return(INVALID_NETWORK_ID));

    EXPECT_TRUE(pStaAutoConnectService->CurrentDeviceGoodEnough(scanInfos, info) == false);
}


void StaAutoConnectServiceTest::CurrentDeviceGoodEnoughFail4()
{
    WifiDeviceConfig deviceConfig;
    std::vector<InterScanInfo> scanInfos;
    WifiLinkedInfo info;
    GetWifiDeviceConfig(deviceConfig);
    GetInterScanInfoVector(scanInfos);
    GetWifiLinkedInfo(info);
    info.rssi = SMALLER_THAN_RSSI_DELIMITING_VALUE;

    EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_, _))
        .WillOnce(DoAll(SetArgReferee<1>(deviceConfig), Return(0)));
    EXPECT_CALL(WifiSettings::GetInstance(), GetUserLastSelectedNetworkId())
        .Times(AtLeast(1))
        .WillOnce(Return(INVALID_NETWORK_ID));

    EXPECT_TRUE(pStaAutoConnectService->CurrentDeviceGoodEnough(scanInfos, info) == false);
}

void StaAutoConnectServiceTest::CurrentDeviceGoodEnoughFail5()
{
    WifiDeviceConfig deviceConfig;
    std::vector<InterScanInfo> scanInfos;
    WifiLinkedInfo info;
    GetWifiDeviceConfig(deviceConfig);
    GetInterScanInfoVector(scanInfos);
    GetWifiLinkedInfo(info);
    info.band = static_cast<int>(BandType::BAND_2GHZ);
    info.frequency = MIN_24_FREQUENCY + 1;
    scanInfos[0].band = NETWORK_5G_BAND;
    scanInfos[0].frequency = MIN_5_FREQUENCY + 1;

    EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_, _))
        .WillOnce(DoAll(SetArgReferee<1>(deviceConfig), Return(0)));
    EXPECT_CALL(WifiSettings::GetInstance(), GetUserLastSelectedNetworkId())
        .Times(AtLeast(1))
        .WillOnce(Return(INVALID_NETWORK_ID));

    EXPECT_TRUE(pStaAutoConnectService->CurrentDeviceGoodEnough(scanInfos, info) == false);
}

void StaAutoConnectServiceTest::WhetherDevice5GAvailableSuccess()
{
    std::vector<InterScanInfo> scanInfos;
    GetInterScanInfoVector(scanInfos);
    scanInfos[0].band = NETWORK_5G_BAND;
    scanInfos[0].frequency = MIN_5_FREQUENCY + 1;

    EXPECT_TRUE(pStaAutoConnectService->WhetherDevice5GAvailable(scanInfos) == true);
}

void StaAutoConnectServiceTest::WhetherDevice5GAvailableFail()
{
    std::vector<InterScanInfo> scanInfos;
    GetInterScanInfoVector(scanInfos);

    EXPECT_TRUE(pStaAutoConnectService->WhetherDevice5GAvailable(scanInfos) == false);
}

void StaAutoConnectServiceTest::RoamingEncryptionModeCheckSuccess()
{
    WifiDeviceConfig deviceConfig;
    InterScanInfo scanInfo;
    WifiLinkedInfo info;
    GetWifiDeviceConfig(deviceConfig);
    GetScanInfoConfig(scanInfo);
    GetWifiLinkedInfo(info);
    info.connState = ConnState::CONNECTED;
    scanInfo.securityType = WifiSecurity::WEP;
    deviceConfig.wepTxKeyIndex = 0;
    deviceConfig.keyMgmt = "NONE";
    scanInfo.rssi = 8;
    info.rssi = 1;
    int indexType = DEVICE_CONFIG_INDEX_SSID;

    EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(scanInfo.ssid, indexType, _))
        .WillOnce(DoAll(SetArgReferee<2>(deviceConfig), Return(0)));

    EXPECT_TRUE(pStaAutoConnectService->RoamingEncryptionModeCheck(deviceConfig, scanInfo, info) == true);
}

void StaAutoConnectServiceTest::RoamingEncryptionModeCheckFail1()
{
    WifiDeviceConfig deviceConfig;
    InterScanInfo scanInfo;
    WifiLinkedInfo info;
    GetWifiDeviceConfig(deviceConfig);
    GetScanInfoConfig(scanInfo);
    GetWifiLinkedInfo(info);
    int indexType = DEVICE_CONFIG_INDEX_SSID;

    EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(scanInfo.ssid, indexType, _))
        .WillOnce(DoAll(SetArgReferee<2>(deviceConfig), Return(0)));

    EXPECT_TRUE(pStaAutoConnectService->RoamingEncryptionModeCheck(deviceConfig, scanInfo, info) == false);
}

void StaAutoConnectServiceTest::RoamingEncryptionModeCheckFail2()
{
    WifiDeviceConfig deviceConfig;
    InterScanInfo scanInfo;
    WifiLinkedInfo info;
    GetWifiDeviceConfig(deviceConfig);
    GetScanInfoConfig(scanInfo);
    GetWifiLinkedInfo(info);
    scanInfo.securityType = WifiSecurity::WEP;
    deviceConfig.wepTxKeyIndex = -1;
    int indexType = DEVICE_CONFIG_INDEX_SSID;

    EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(scanInfo.ssid, indexType, _))
        .WillOnce(DoAll(SetArgReferee<2>(deviceConfig), Return(0)));

    EXPECT_TRUE(pStaAutoConnectService->RoamingEncryptionModeCheck(deviceConfig, scanInfo, info) == false);
}

void StaAutoConnectServiceTest::RoamingEncryptionModeCheckFail3()
{
    WifiDeviceConfig deviceConfig;
    InterScanInfo scanInfo;
    WifiLinkedInfo info;
    GetWifiDeviceConfig(deviceConfig);
    GetScanInfoConfig(scanInfo);
    GetWifiLinkedInfo(info);
    info.connState = ConnState::CONNECTED;
    scanInfo.securityType = WifiSecurity::PSK;
    deviceConfig.wepTxKeyIndex = 1;
    deviceConfig.keyMgmt = "NONE";
    int indexType = DEVICE_CONFIG_INDEX_SSID;

    EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(scanInfo.ssid, indexType, _))
        .WillOnce(DoAll(SetArgReferee<2>(deviceConfig), Return(0)));

    EXPECT_TRUE(pStaAutoConnectService->RoamingEncryptionModeCheck(deviceConfig, scanInfo, info) == false);
}

void StaAutoConnectServiceTest::RoamingEncryptionModeCheckFail4()
{
    WifiDeviceConfig deviceConfig;
    InterScanInfo scanInfo;
    WifiLinkedInfo info;
    GetWifiDeviceConfig(deviceConfig);
    GetScanInfoConfig(scanInfo);
    GetWifiLinkedInfo(info);
    info.connState = ConnState::CONNECTED;
    scanInfo.securityType = WifiSecurity::WEP;
    deviceConfig.wepTxKeyIndex = 1;
    deviceConfig.keyMgmt = "WPA-PSK";
    int indexType = DEVICE_CONFIG_INDEX_SSID;

    EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(scanInfo.ssid, indexType, _))
        .WillOnce(DoAll(SetArgReferee<2>(deviceConfig), Return(0)));

    EXPECT_TRUE(pStaAutoConnectService->RoamingEncryptionModeCheck(deviceConfig, scanInfo, info) == false);
}

void StaAutoConnectServiceTest::RoamingSelectionSuccess1()
{
    WifiDeviceConfig deviceConfig;
    std::vector<InterScanInfo> scanInfos;
    WifiLinkedInfo info;
    GetWifiDeviceConfig(deviceConfig);
    GetInterScanInfoVector(scanInfos);
    GetWifiLinkedInfo(info);
    deviceConfig.wepTxKeyIndex = 0;
    deviceConfig.keyMgmt = "NONE";
    info.connState = ConnState::CONNECTED;
    scanInfos[0].securityType = WifiSecurity::WEP;
    scanInfos[0].rssi = 8;
    scanInfos[0].ssid = "ohos";
    scanInfos[0].bssid = "2a:76:93:47:e2:8e";
    info.ssid = "ohos";
    info.bssid = "2a:76:93:47:e2:8b";
    info.rssi = 1;
    int indexType = DEVICE_CONFIG_INDEX_SSID;

    EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(scanInfos[0].ssid, indexType, _))
        .WillOnce(DoAll(SetArgReferee<2>(deviceConfig), Return(0)));

    EXPECT_TRUE(pStaAutoConnectService->RoamingSelection(deviceConfig, scanInfos, info) == true);
}

void StaAutoConnectServiceTest::RoamingSelectionFail1()
{
    WifiDeviceConfig deviceConfig;
    std::vector<InterScanInfo> scanInfos;
    WifiLinkedInfo info;
    GetWifiDeviceConfig(deviceConfig);
    GetInterScanInfoVector(scanInfos);
    GetWifiLinkedInfo(info);
    info.connState = ConnState::CONNECTED;
    info.bssid = "2a:76:93:47:e2:8b";

    int indexType = DEVICE_CONFIG_INDEX_SSID;


    EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(scanInfos[0].ssid, indexType, _))
        .WillOnce(DoAll(SetArgReferee<2>(deviceConfig), Return(0)));

    EXPECT_TRUE(pStaAutoConnectService->RoamingSelection(deviceConfig, scanInfos, info) == false);
}

void StaAutoConnectServiceTest::RoamingSelectionFail2()
{
    WifiDeviceConfig deviceConfig;
    std::vector<InterScanInfo> scanInfos;
    WifiLinkedInfo info;
    GetWifiDeviceConfig(deviceConfig);
    GetInterScanInfoVector(scanInfos);
    GetWifiLinkedInfo(info);
    deviceConfig.wepTxKeyIndex = -1;
    scanInfos[0].securityType = WifiSecurity::WEP;
    info.connState = ConnState::CONNECTED;
    info.bssid = "2a:76:93:47:e2:8b";
    int indexType = DEVICE_CONFIG_INDEX_SSID;

    EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(scanInfos[0].ssid, indexType, _))
        .WillOnce(DoAll(SetArgReferee<2>(deviceConfig), Return(0)));

    EXPECT_TRUE(pStaAutoConnectService->RoamingSelection(deviceConfig, scanInfos, info) == false);
}

void StaAutoConnectServiceTest::RoamingSelectionFail3()
{
    WifiDeviceConfig deviceConfig;
    std::vector<InterScanInfo> scanInfos;
    WifiLinkedInfo info;
    GetWifiDeviceConfig(deviceConfig);
    GetInterScanInfoVector(scanInfos);
    GetWifiLinkedInfo(info);
    deviceConfig.wepTxKeyIndex = 1;
    deviceConfig.keyMgmt = "NONE";
    scanInfos[0].securityType = WifiSecurity::PSK;
    info.connState = ConnState::CONNECTED;
    info.bssid = "2a:76:93:47:e2:8b";
    info.connState = ConnState::CONNECTED;
    int indexType = DEVICE_CONFIG_INDEX_SSID;

    EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(scanInfos[0].ssid, indexType, _))
        .WillOnce(DoAll(SetArgReferee<2>(deviceConfig), Return(0)));

    EXPECT_TRUE(pStaAutoConnectService->RoamingSelection(deviceConfig, scanInfos, info) == false);
}

void StaAutoConnectServiceTest::RoamingSelectionFail4()
{
    WifiDeviceConfig deviceConfig;
    std::vector<InterScanInfo> scanInfos;
    WifiLinkedInfo info;
    GetWifiDeviceConfig(deviceConfig);
    GetInterScanInfoVector(scanInfos);
    GetWifiLinkedInfo(info);
    deviceConfig.wepTxKeyIndex = 1;
    deviceConfig.keyMgmt = "WPA-PSK";
    scanInfos[0].securityType = WifiSecurity::WEP;
    info.connState = ConnState::CONNECTED;
    info.bssid = "2a:76:93:47:e2:8b";
    info.connState = ConnState::CONNECTED;
    int indexType = DEVICE_CONFIG_INDEX_SSID;

    EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(scanInfos[0].ssid, indexType, _))
        .WillOnce(DoAll(SetArgReferee<2>(deviceConfig), Return(0)));

    EXPECT_TRUE(pStaAutoConnectService->RoamingSelection(deviceConfig, scanInfos, info) == false);
}

void StaAutoConnectServiceTest::SyncBlockedSsidFirmwareSuccess()
{
    std::string bssid = "2a:76:93:47:e2:8a";
    bool enable = false;
    int reason = AP_CANNOT_HANDLE_NEW_STA;
    WifiIdlRoamCapability capability;
    capability.maxBlocklistSize = 2;

    pStaAutoConnectService->AddOrDelBlockedBssids(bssid, enable, reason);

    EXPECT_CALL(WifiStaHalInterface::GetInstance(), GetStaCapabilities(_))
        .WillOnce(Return(WIFI_IDL_OPT_FAILED)); // if it is false, it will do process.
    EXPECT_CALL(WifiStaHalInterface::GetInstance(), GetRoamingCapabilities(_))
        .WillOnce(DoAll(SetArgReferee<0>(capability), Return(WIFI_IDL_OPT_OK)));
    pStaAutoConnectService->ObtainRoamCapFromFirmware();

    EXPECT_CALL(WifiStaHalInterface::GetInstance(), SetRoamConfig(_)).WillOnce(Return(WIFI_IDL_OPT_OK));
    pStaAutoConnectService->SyncBlockedSsidFirmware();
}

void StaAutoConnectServiceTest::SyncBlockedSsidFirmwareFail()
{
    WifiIdlRoamCapability capability;
    capability.maxBlocklistSize = 2;

    EXPECT_CALL(WifiStaHalInterface::GetInstance(), GetStaCapabilities(_))
        .WillOnce(Return(WIFI_IDL_OPT_FAILED)); // if it is false, it will do process.
    EXPECT_CALL(WifiStaHalInterface::GetInstance(), GetRoamingCapabilities(_))
        .WillRepeatedly(DoAll(SetArgReferee<0>(capability), Return(WIFI_IDL_OPT_OK)));
    pStaAutoConnectService->ObtainRoamCapFromFirmware();

    pStaAutoConnectService->SyncBlockedSsidFirmware();
}

/* ************************ HWTEST_F  ************************************ */

HWTEST_F(StaAutoConnectServiceTest, InitAutoConnectServiceSuccess, TestSize.Level1)
{
    InitAutoConnectServiceSuccess();
}

HWTEST_F(StaAutoConnectServiceTest, OnScanResultsReadyHandlerSuccess1, TestSize.Level1)
{
    OnScanResultsReadyHandlerSuccess1();
}

HWTEST_F(StaAutoConnectServiceTest, OnScanResultsReadyHandlerSuccess2, TestSize.Level1)
{
    OnScanResultsReadyHandlerSuccess2();
}

HWTEST_F(StaAutoConnectServiceTest, OnScanResultsReadyHandlerSuccess3, TestSize.Level1)
{
    OnScanResultsReadyHandlerSuccess3();
}

HWTEST_F(StaAutoConnectServiceTest, OnScanResultsReadyHandlerFail1, TestSize.Level1)
{
    OnScanResultsReadyHandlerFail1();
}

HWTEST_F(StaAutoConnectServiceTest, OnScanResultsReadyHandlerFail2, TestSize.Level1)
{
    OnScanResultsReadyHandlerFail2();
}

HWTEST_F(StaAutoConnectServiceTest, OnScanResultsReadyHandlerFail3, TestSize.Level1)
{
    OnScanResultsReadyHandlerFail3();
}

HWTEST_F(StaAutoConnectServiceTest, OnScanResultsReadyHandlerFail4, TestSize.Level1)
{
    OnScanResultsReadyHandlerFail4();
}


HWTEST_F(StaAutoConnectServiceTest, OnScanResultsReadyHandlerFail5, TestSize.Level1)
{
    OnScanResultsReadyHandlerFail5();
}


HWTEST_F(StaAutoConnectServiceTest, OnScanResultsReadyHandlerFail6, TestSize.Level1)
{
    OnScanResultsReadyHandlerFail6();
}

HWTEST_F(StaAutoConnectServiceTest, OnScanResultsReadyHandlerFail7, TestSize.Level1)
{
    OnScanResultsReadyHandlerFail7();
}

HWTEST_F(StaAutoConnectServiceTest, EnableOrDisableBssidSuccess1, TestSize.Level1)
{
    EnableOrDisableBssidSuccess1();
}

HWTEST_F(StaAutoConnectServiceTest, EnableOrDisableBssidSuccess2, TestSize.Level1)
{
    EnableOrDisableBssidSuccess2();
}

HWTEST_F(StaAutoConnectServiceTest, EnableOrDisableBssidFail1, TestSize.Level1)
{
    EnableOrDisableBssidFail1();
}

HWTEST_F(StaAutoConnectServiceTest, EnableOrDisableBssidFail2, TestSize.Level1)
{
    EnableOrDisableBssidFail2();
}

HWTEST_F(StaAutoConnectServiceTest, EnableOrDisableBssidFail3, TestSize.Level1)
{
    EnableOrDisableBssidFail3();
}

HWTEST_F(StaAutoConnectServiceTest, EnableOrDisableBssidFail4, TestSize.Level1)
{
    EnableOrDisableBssidFail4();
}

HWTEST_F(StaAutoConnectServiceTest, AutoSelectDeviceSuccess1, TestSize.Level1)
{
    AutoSelectDeviceSuccess1();
}

HWTEST_F(StaAutoConnectServiceTest, AutoSelectDeviceSuccess2, TestSize.Level1)
{
    AutoSelectDeviceSuccess2();
}

HWTEST_F(StaAutoConnectServiceTest, AutoSelectDeviceSuccess3, TestSize.Level1)
{
    AutoSelectDeviceSuccess3();
}

HWTEST_F(StaAutoConnectServiceTest, AutoSelectDeviceFail1, TestSize.Level1)
{
    AutoSelectDeviceFail1();
}

HWTEST_F(StaAutoConnectServiceTest, AutoSelectDeviceFail2, TestSize.Level1)
{
    AutoSelectDeviceFail2();
}

HWTEST_F(StaAutoConnectServiceTest, AutoSelectDeviceFail3, TestSize.Level1)
{
    AutoSelectDeviceFail3();
}

HWTEST_F(StaAutoConnectServiceTest, AutoSelectDeviceFail4, TestSize.Level1)
{
    AutoSelectDeviceFail4();
}

HWTEST_F(StaAutoConnectServiceTest, AutoSelectDeviceFail5, TestSize.Level1)
{
    AutoSelectDeviceFail5();
}

HWTEST_F(StaAutoConnectServiceTest, AutoSelectDeviceFail6, TestSize.Level1)
{
    AutoSelectDeviceFail6();
}

HWTEST_F(StaAutoConnectServiceTest, RegisterDeviceAppraisalSuccess, TestSize.Level1)
{
    RegisterDeviceAppraisalSuccess();
}

HWTEST_F(StaAutoConnectServiceTest, RegisterDeviceAppraisalFail1, TestSize.Level1)
{
    RegisterDeviceAppraisalFail1();
}

HWTEST_F(StaAutoConnectServiceTest, RegisterDeviceAppraisalFail2, TestSize.Level1)
{
    RegisterDeviceAppraisalFail2();
}

HWTEST_F(StaAutoConnectServiceTest, ClearOvertimeBlockedBssidSuccess1, TestSize.Level1)
{
    ClearOvertimeBlockedBssidSuccess1();
}

HWTEST_F(StaAutoConnectServiceTest, ClearOvertimeBlockedBssidSuccess2, TestSize.Level1)
{
    ClearOvertimeBlockedBssidSuccess2();
}

HWTEST_F(StaAutoConnectServiceTest, GetBlockedBssidsSuccess, TestSize.Level1)
{
    GetBlockedBssidsSuccess();
}

HWTEST_F(StaAutoConnectServiceTest, AddOrDelBlockedBssidsSuccess, TestSize.Level1)
{
    AddOrDelBlockedBssidsSuccess();
}

HWTEST_F(StaAutoConnectServiceTest, AddOrDelBlockedBssidsFail, TestSize.Level1)
{
    AddOrDelBlockedBssidsFail();
}

HWTEST_F(StaAutoConnectServiceTest, ObtainRoamCapFromFirmwareSuccess, TestSize.Level1)
{
    ObtainRoamCapFromFirmwareSuccess();
}

HWTEST_F(StaAutoConnectServiceTest, ObtainRoamCapFromFirmwareFail1, TestSize.Level1)
{
    ObtainRoamCapFromFirmwareFail1();
}

HWTEST_F(StaAutoConnectServiceTest, ObtainRoamCapFromFirmwareFail2, TestSize.Level1)
{
    ObtainRoamCapFromFirmwareFail2();
}

HWTEST_F(StaAutoConnectServiceTest, SetRoamBlockedBssidFirmwareSuccess, TestSize.Level1)
{
    SetRoamBlockedBssidFirmwareSuccess();
}

HWTEST_F(StaAutoConnectServiceTest, SetRoamBlockedBssidFirmwareFail1, TestSize.Level1)
{
    SetRoamBlockedBssidFirmwareFail1();
}

HWTEST_F(StaAutoConnectServiceTest, SetRoamBlockedBssidFirmwareFail2, TestSize.Level1)
{
    SetRoamBlockedBssidFirmwareFail2();
}

HWTEST_F(StaAutoConnectServiceTest, SetRoamBlockedBssidFirmwareFail3, TestSize.Level1)
{
    SetRoamBlockedBssidFirmwareFail3();
}

HWTEST_F(StaAutoConnectServiceTest, SetRoamBlockedBssidFirmwareFail4, TestSize.Level1)
{
    SetRoamBlockedBssidFirmwareFail4();
}

HWTEST_F(StaAutoConnectServiceTest, ConnectElectedDeviceSuccess1, TestSize.Level1)
{
    ConnectElectedDeviceSuccess1();
}

HWTEST_F(StaAutoConnectServiceTest, ConnectElectedDeviceSuccess2, TestSize.Level1)
{
    ConnectElectedDeviceSuccess2();
}

HWTEST_F(StaAutoConnectServiceTest, ConnectElectedDeviceFail1, TestSize.Level1)
{
    ConnectElectedDeviceFail1();
}

HWTEST_F(StaAutoConnectServiceTest, GetAvailableScanInfosSuccess, TestSize.Level1)
{
    GetAvailableScanInfosSuccess();
}

HWTEST_F(StaAutoConnectServiceTest, AllowAutoSelectDeviceSuccess1, TestSize.Level1)
{
    AllowAutoSelectDeviceSuccess1();
}

HWTEST_F(StaAutoConnectServiceTest, AllowAutoSelectDeviceSuccess2, TestSize.Level1)
{
    AllowAutoSelectDeviceSuccess2();
}

HWTEST_F(StaAutoConnectServiceTest, AllowAutoSelectDeviceSuccess3, TestSize.Level1)
{
    AllowAutoSelectDeviceSuccess3();
}

HWTEST_F(StaAutoConnectServiceTest, AllowAutoSelectDeviceFail1, TestSize.Level1)
{
    AllowAutoSelectDeviceFail1();
}

HWTEST_F(StaAutoConnectServiceTest, AllowAutoSelectDeviceFail2, TestSize.Level1)
{
    AllowAutoSelectDeviceFail2();
}

HWTEST_F(StaAutoConnectServiceTest, AllowAutoSelectDeviceFail3, TestSize.Level1)
{
    AllowAutoSelectDeviceFail3();
}

HWTEST_F(StaAutoConnectServiceTest, AllowAutoSelectDeviceFail4, TestSize.Level1)
{
    AllowAutoSelectDeviceFail4();
}

HWTEST_F(StaAutoConnectServiceTest, CurrentDeviceGoodEnoughSuccess, TestSize.Level1)
{
    CurrentDeviceGoodEnoughSuccess();
}

HWTEST_F(StaAutoConnectServiceTest, CurrentDeviceGoodEnoughFail1, TestSize.Level1)
{
    CurrentDeviceGoodEnoughFail1();
}

HWTEST_F(StaAutoConnectServiceTest, CurrentDeviceGoodEnoughFail2, TestSize.Level1)
{
    CurrentDeviceGoodEnoughFail2();
}

HWTEST_F(StaAutoConnectServiceTest, CurrentDeviceGoodEnoughFail3, TestSize.Level1)
{
    CurrentDeviceGoodEnoughFail3();
}


HWTEST_F(StaAutoConnectServiceTest, CurrentDeviceGoodEnoughFail4, TestSize.Level1)
{
    CurrentDeviceGoodEnoughFail4();
}

HWTEST_F(StaAutoConnectServiceTest, CurrentDeviceGoodEnoughFail5, TestSize.Level1)
{
    CurrentDeviceGoodEnoughFail5();
}

HWTEST_F(StaAutoConnectServiceTest, WhetherDevice5GAvailableSuccess, TestSize.Level1)
{
    WhetherDevice5GAvailableSuccess();
}

HWTEST_F(StaAutoConnectServiceTest, WhetherDevice5GAvailableFail, TestSize.Level1)
{
    WhetherDevice5GAvailableFail();
}

HWTEST_F(StaAutoConnectServiceTest, RoamingEncryptionModeCheckSuccess, TestSize.Level1)
{
    RoamingEncryptionModeCheckSuccess();
}

HWTEST_F(StaAutoConnectServiceTest, RoamingEncryptionModeCheckFail1, TestSize.Level1)
{
    RoamingEncryptionModeCheckFail1();
}

HWTEST_F(StaAutoConnectServiceTest, RoamingEncryptionModeCheckFail2, TestSize.Level1)
{
    RoamingEncryptionModeCheckFail2();
}

HWTEST_F(StaAutoConnectServiceTest, RoamingEncryptionModeCheckFail3, TestSize.Level1)
{
    RoamingEncryptionModeCheckFail3();
}

HWTEST_F(StaAutoConnectServiceTest, RoamingEncryptionModeCheckFail4, TestSize.Level1)
{
    RoamingEncryptionModeCheckFail4();
}

HWTEST_F(StaAutoConnectServiceTest, RoamingSelectionSuccess1, TestSize.Level1)
{
    RoamingSelectionSuccess1();
}

HWTEST_F(StaAutoConnectServiceTest, RoamingSelectionFail1, TestSize.Level1)
{
    RoamingSelectionFail1();
}

HWTEST_F(StaAutoConnectServiceTest, RoamingSelectionFail2, TestSize.Level1)
{
    RoamingSelectionFail2();
}

HWTEST_F(StaAutoConnectServiceTest, RoamingSelectionFail3, TestSize.Level1)
{
    RoamingSelectionFail3();
}

HWTEST_F(StaAutoConnectServiceTest, RoamingSelectionFail4, TestSize.Level1)
{
    RoamingSelectionFail4();
}

HWTEST_F(StaAutoConnectServiceTest, SyncBlockedSsidFirmwareSuccess, TestSize.Level1)
{
    SyncBlockedSsidFirmwareSuccess();
}

HWTEST_F(StaAutoConnectServiceTest, SyncBlockedSsidFirmwareFail, TestSize.Level1)
{
    SyncBlockedSsidFirmwareFail();
}
} // Wifi
} // OHOS