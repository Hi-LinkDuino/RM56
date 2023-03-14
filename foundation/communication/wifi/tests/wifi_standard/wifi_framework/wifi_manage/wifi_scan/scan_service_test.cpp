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
#include "scan_service.h"
#include <gtest/gtest.h>
#include "mock_wifi_manager.h"
#include "mock_wifi_settings.h"
#include "mock_scan_state_machine.h"
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
constexpr int FREQ_2_DOT_4_GHZ = 2450;
constexpr int FREQ_5_GHZ = 5200;
class ScanServiceTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    virtual void SetUp()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), StopPnoScan()).Times(AtLeast(0));
        pScanService = std::make_unique<ScanService>();
        pScanService->pScanStateMachine = new MockScanStateMachine();
        pScanService->RegisterScanCallbacks(WifiManager::GetInstance().GetScanCallback());
    }
    virtual void TearDown()
    {
        pScanService.reset();
    }

public:
    std::unique_ptr<ScanService> pScanService;

    void InitScanServiceSuccess1()
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
        EXPECT_EQ(pScanService->InitScanService(WifiManager::GetInstance().GetScanCallback()), true);
    }

    void InitScanServiceSuccess2()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), GetSupportFrequencies(_, _))
            .WillRepeatedly(Return(WIFI_IDL_OPT_FAILED));
        EXPECT_CALL(WifiSettings::GetInstance(), GetSupportHwPnoFlag()).Times(AtLeast(1));
        EXPECT_CALL(WifiSupplicantHalInterface::GetInstance(), RegisterSupplicantEventCallback(_)).Times(AtLeast(1));
        EXPECT_CALL(WifiSettings::GetInstance(), GetScanControlInfo(_)).Times(AtLeast(1));
        EXPECT_CALL(WifiSettings::GetInstance(), GetScreenState()).Times(AtLeast(1));
        EXPECT_CALL(WifiManager::GetInstance(), DealScanOpenRes()).Times(AtLeast(0));
        EXPECT_CALL(WifiSupplicantHalInterface::GetInstance(), UnRegisterSupplicantEventCallback()).Times(AtLeast(1));
        EXPECT_CALL(WifiSettings::GetInstance(), GetWhetherToAllowNetworkSwitchover()).Times(AtLeast(0));
        EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_)).Times(AtLeast(0));
        EXPECT_EQ(pScanService->InitScanService(WifiManager::GetInstance().GetScanCallback()), true);
    }

    void UnInitScanServiceSuccess()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), StopPnoScan()).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        pScanService->UnInitScanService();
    }

    void HandleScanStatusReportSuccess1()
    {
        EXPECT_CALL(WifiSettings::GetInstance(), GetWhetherToAllowNetworkSwitchover())
            .Times(AtLeast(1))
            .WillRepeatedly(Return(true));
        EXPECT_CALL(WifiManager::GetInstance(), DealScanOpenRes()).Times(AtLeast(1));
        EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_)).Times(AtLeast(0));
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), StopPnoScan()).Times(AtLeast(0));
        ScanStatusReport scanStatusReport;
        scanStatusReport.status = SCAN_STARTED_STATUS;
        pScanService->HandleScanStatusReport(scanStatusReport);
    }

    void HandleScanStatusReportSuccess2()
    {
        EXPECT_CALL(WifiManager::GetInstance(), DealScanCloseRes()).Times(AtLeast(1));
        ScanStatusReport scanStatusReport;
        scanStatusReport.status = SCAN_FINISHED_STATUS;
        pScanService->HandleScanStatusReport(scanStatusReport);
    }

    void HandleScanStatusReportSuccess3()
    {
        EXPECT_CALL(WifiManager::GetInstance(), DealScanFinished(_)).Times(AtLeast(0));
        EXPECT_CALL(WifiSettings::GetInstance(), SaveScanInfoList(_)).Times(AtLeast(0));
        EXPECT_CALL(WifiManager::GetInstance(), DealScanInfoNotify(_)).Times(AtLeast(1));
        ScanStatusReport scanStatusReport;
        scanStatusReport.status = COMMON_SCAN_SUCCESS;
        pScanService->HandleScanStatusReport(scanStatusReport);
    }

    void HandleScanStatusReportSuccess4()
    {
        EXPECT_CALL(WifiManager::GetInstance(), DealScanFinished(_)).Times(AtLeast(0));
        ScanStatusReport scanStatusReport;
        scanStatusReport.status = COMMON_SCAN_FAILED;
        pScanService->HandleScanStatusReport(scanStatusReport);
    }

    void HandleScanStatusReportSuccess5()
    {
        EXPECT_CALL(WifiManager::GetInstance(), DealScanInfoNotify(_)).Times(AtLeast(1));
        ScanStatusReport scanStatusReport;
        scanStatusReport.status = PNO_SCAN_INFO;
        pScanService->HandleScanStatusReport(scanStatusReport);
    }

    void HandleScanStatusReportSuccess6()
    {
        ScanStatusReport scanStatusReport;
        scanStatusReport.status = PNO_SCAN_FAILED;
        pScanService->HandleScanStatusReport(scanStatusReport);
    }

    void HandleScanStatusReportSuccess7()
    {
        ScanStatusReport scanStatusReport;
        scanStatusReport.status = SCAN_INNER_EVENT;
        pScanService->HandleScanStatusReport(scanStatusReport);
    }

    void HandleScanStatusReportFail()
    {
        ScanStatusReport scanStatusReport;
        scanStatusReport.status = SCAN_STATUS_INVALID;
        pScanService->HandleScanStatusReport(scanStatusReport);
    }

    void HandleInnerEventReportSuccess1()
    {
        EXPECT_CALL(WifiSettings::GetInstance(), GetWhetherToAllowNetworkSwitchover())
            .Times(AtLeast(1))
            .WillRepeatedly(Return(true));
        ScanInnerEventType innerEvent;
        innerEvent = SYSTEM_SCAN_TIMER;
        pScanService->HandleInnerEventReport(innerEvent);
    }

    void HandleInnerEventReportSuccess2()
    {
        ScanInnerEventType innerEvent;
        innerEvent = DISCONNECTED_SCAN_TIMER;
        pScanService->HandleInnerEventReport(innerEvent);
    }

    void HandleInnerEventReportSuccess3()
    {
        EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_)).Times(AtLeast(0));
        EXPECT_CALL(WifiSettings::GetInstance(), GetMinRssi2Dot4Ghz()).Times(AtLeast(0));
        EXPECT_CALL(WifiSettings::GetInstance(), GetMinRssi5Ghz()).Times(AtLeast(0));
        ScanInnerEventType innerEvent;
        innerEvent = RESTART_PNO_SCAN_TIMER;
        pScanService->HandleInnerEventReport(innerEvent);
    }

    void HandleInnerEventReportFail()
    {
        ScanInnerEventType innerEvent;
        innerEvent = SCAN_INNER_EVENT_INVALID;
        pScanService->HandleInnerEventReport(innerEvent);
    }

    void ScanSuccess()
    {
        EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_)).WillRepeatedly(Return(true));
        pScanService->scanStartedFlag = true;
        pScanService->Scan(false);
    }

    void ScanFail()
    {
        pScanService->scanStartedFlag = false;
        pScanService->Scan(false);
    }

    void ScanWithParamSuccess()
    {
        EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_)).WillRepeatedly(Return(true));
        pScanService->scanStartedFlag = true;
        WifiScanParams params;
        params.band = SCAN_BAND_BOTH_WITH_DFS;
        EXPECT_EQ(WIFI_OPT_SUCCESS, pScanService->ScanWithParam(params));
    }

    void ScanWithParamFail1()
    {
        pScanService->scanStartedFlag = false;
        WifiScanParams params;
        params.band = SCAN_BAND_BOTH_WITH_DFS;
        EXPECT_EQ(WIFI_OPT_FAILED, pScanService->ScanWithParam(params));
    }

    void ScanWithParamFail2()
    {
        pScanService->scanStartedFlag = false;
        WifiScanParams params;
        params.band = -1;
        EXPECT_EQ(WIFI_OPT_FAILED, pScanService->ScanWithParam(params));
    }

    void ScanWithParamFail3()
    {
        pScanService->scanStartedFlag = true;
        WifiScanParams params;
        params.band = SCAN_BAND_UNSPECIFIED;
        EXPECT_EQ(WIFI_OPT_FAILED, pScanService->ScanWithParam(params));
    }

    void ScanWithParamFail4()
    {
        EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_)).WillRepeatedly(Return(false));
        pScanService->scanStartedFlag = true;
        WifiScanParams params;
        params.band = SCAN_BAND_UNSPECIFIED;
        EXPECT_EQ(WIFI_OPT_FAILED, pScanService->ScanWithParam(params));
    }

    void SingleScanSuccess1()
    {
        ScanConfig scanConfig;
        scanConfig.scanBand = SCAN_BAND_BOTH_WITH_DFS;
        scanConfig.externFlag = true;
        scanConfig.scanStyle = SCAN_TYPE_HIGH_ACCURACY;
        EXPECT_EQ(true, pScanService->SingleScan(scanConfig));
    }

    void SingleScanSuccess2()
    {
        ScanConfig scanConfig;
        scanConfig.scanBand = SCAN_BAND_UNSPECIFIED;
        scanConfig.externFlag = true;
        scanConfig.scanStyle = SCAN_TYPE_HIGH_ACCURACY;
        scanConfig.scanFreqs.push_back(0);
        EXPECT_EQ(true, pScanService->SingleScan(scanConfig));
    }

    void SingleScanSuccess3()
    {
        ScanConfig scanConfig;
        scanConfig.scanBand = SCAN_BAND_24_GHZ;
        scanConfig.externFlag = true;
        scanConfig.scanStyle = SCAN_TYPE_HIGH_ACCURACY;
        EXPECT_EQ(true, pScanService->SingleScan(scanConfig));
    }

    void SingleScanFail1()
    {
        ScanConfig scanConfig;
        scanConfig.scanBand = SCAN_BAND_UNSPECIFIED;
        scanConfig.externFlag = true;
        scanConfig.scanStyle = SCAN_TYPE_HIGH_ACCURACY;
        EXPECT_EQ(false, pScanService->SingleScan(scanConfig));
    }

    void GetBandFreqsSuccess1()
    {
        ScanBandType band = SCAN_BAND_24_GHZ;
        std::vector<int> freqs;
        EXPECT_EQ(true, pScanService->GetBandFreqs(band, freqs));
    }

    void GetBandFreqsSuccess2()
    {
        ScanBandType band = SCAN_BAND_5_GHZ;
        std::vector<int> freqs;
        EXPECT_EQ(true, pScanService->GetBandFreqs(band, freqs));
    }

    void GetBandFreqsSuccess3()
    {
        ScanBandType band = SCAN_BAND_BOTH;
        std::vector<int> freqs;
        EXPECT_EQ(true, pScanService->GetBandFreqs(band, freqs));
    }

    void GetBandFreqsSuccess4()
    {
        ScanBandType band = SCAN_BAND_5_GHZ_DFS_ONLY;
        std::vector<int> freqs;
        EXPECT_EQ(true, pScanService->GetBandFreqs(band, freqs));
    }

    void GetBandFreqsSuccess5()
    {
        ScanBandType band = SCAN_BAND_5_GHZ_WITH_DFS;
        std::vector<int> freqs;
        EXPECT_EQ(true, pScanService->GetBandFreqs(band, freqs));
    }

    void GetBandFreqsSuccess6()
    {
        ScanBandType band = SCAN_BAND_BOTH_WITH_DFS;
        std::vector<int> freqs;
        EXPECT_EQ(true, pScanService->GetBandFreqs(band, freqs));
    }

    void GetBandFreqsFail()
    {
        ScanBandType band = SCAN_BAND_UNSPECIFIED;
        std::vector<int> freqs;
        EXPECT_EQ(false, pScanService->GetBandFreqs(band, freqs));
    }

    void AddScanMessageBodySuccess()
    {
        InternalMessage msg;
        InterScanConfig interConfig;
        interConfig.hiddenNetworkSsid.push_back("hmwifi");
        interConfig.scanFreqs.push_back(FREQ_2_DOT_4_GHZ);
        EXPECT_EQ(true, pScanService->AddScanMessageBody(&msg, interConfig));
    }

    void AddScanMessageBodyFail()
    {
        InterScanConfig interConfig;
        EXPECT_EQ(false, pScanService->AddScanMessageBody(nullptr, interConfig));
    }

    void StoreRequestScanConfigSuccess()
    {
        ScanConfig scanConfig;
        InterScanConfig interConfig;
        pScanService->StoreRequestScanConfig(scanConfig, interConfig);
    }

    void HandleCommonScanFailedSuccess1()
    {
        EXPECT_CALL(WifiManager::GetInstance(), DealScanFinished(_)).Times(AtLeast(0));
        pScanService->staStatus = static_cast<int>(OperateResState::CONNECT_CONNECTING);
        std::vector<int> requestIndexList;
        pScanService->HandleCommonScanFailed(requestIndexList);
    }

    void HandleCommonScanFailedSuccess2()
    {
        EXPECT_CALL(WifiManager::GetInstance(), DealScanFinished(_)).Times(AtLeast(0));
        pScanService->staStatus = static_cast<int>(OperateResState::DISCONNECT_DISCONNECTED);
        std::vector<int> requestIndexList;
        requestIndexList.push_back(0);
        pScanService->HandleCommonScanFailed(requestIndexList);
    }

    void HandleCommonScanFailedSuccess3()
    {
        EXPECT_CALL(WifiManager::GetInstance(), DealScanFinished(_));
        pScanService->staStatus = static_cast<int>(OperateResState::DISCONNECT_DISCONNECTED);
        StoreScanConfig storeScanConfig;
        storeScanConfig.fullScanFlag = true;
        pScanService->scanConfigMap.emplace(0, storeScanConfig);
        std::vector<int> requestIndexList;
        requestIndexList.push_back(0);
        pScanService->HandleCommonScanFailed(requestIndexList);
    }

    void HandleCommonScanFailedSuccess4()
    {
        EXPECT_CALL(WifiManager::GetInstance(), DealScanFinished(_)).Times(AtLeast(0));
        pScanService->staStatus = static_cast<int>(OperateResState::DISCONNECT_DISCONNECTED);
        StoreScanConfig storeScanConfig;
        storeScanConfig.fullScanFlag = false;
        pScanService->scanConfigMap.emplace(0, storeScanConfig);
        std::vector<int> requestIndexList;
        requestIndexList.push_back(0);
        pScanService->HandleCommonScanFailed(requestIndexList);
    }

    void HandleCommonScanInfoSuccess1()
    {
        std::vector<int> requestIndexList;
        requestIndexList.push_back(0);
        std::vector<InterScanInfo> scanInfoList;
        pScanService->HandleCommonScanInfo(requestIndexList, scanInfoList);
    }

    void HandleCommonScanInfoSuccess2()
    {
        EXPECT_CALL(WifiSettings::GetInstance(), SaveScanInfoList(_)).Times(AtLeast(1));
        StoreScanConfig storeScanConfig0;
        storeScanConfig0.fullScanFlag = true;
        StoreScanConfig storeScanConfig1;
        storeScanConfig1.fullScanFlag = true;
        pScanService->scanConfigMap.emplace(0, storeScanConfig0);
        pScanService->scanConfigMap.emplace(1, storeScanConfig1);
        std::vector<int> requestIndexList { 0, 1 };
        std::vector<InterScanInfo> scanInfoList;
        pScanService->HandleCommonScanInfo(requestIndexList, scanInfoList);
    }

    void HandleCommonScanInfoSuccess3()
    {
        EXPECT_CALL(WifiSettings::GetInstance(), SaveScanInfoList(_)).Times(AtLeast(0));
        StoreScanConfig storeScanConfig;
        storeScanConfig.fullScanFlag = false;
        pScanService->scanConfigMap.emplace(0, storeScanConfig);
        std::vector<int> requestIndexList { 0 };
        std::vector<InterScanInfo> scanInfoList;
        pScanService->HandleCommonScanInfo(requestIndexList, scanInfoList);
    }

    void StoreFullScanInfoSuccess()
    {
        EXPECT_CALL(WifiSettings::GetInstance(), SaveScanInfoList(_)).WillRepeatedly(Return(0));
        StoreScanConfig scanConfig;
        std::vector<InterScanInfo> scanInfoList { InterScanInfo() };
        EXPECT_EQ(true, pScanService->StoreFullScanInfo(scanConfig, scanInfoList));
    }

    void StoreFullScanInfoFail()
    {
        EXPECT_CALL(WifiSettings::GetInstance(), SaveScanInfoList(_)).WillRepeatedly(Return(-1));
        StoreScanConfig scanConfig;
        std::vector<InterScanInfo> scanInfoList { InterScanInfo() };
        EXPECT_EQ(false, pScanService->StoreFullScanInfo(scanConfig, scanInfoList));
    }

    void StoreUserScanInfoSuccess1()
    {
        StoreScanConfig scanConfig;
        std::vector<InterScanInfo> scanInfoList { InterScanInfo() };
        EXPECT_EQ(true, pScanService->StoreUserScanInfo(scanConfig, scanInfoList));
    }

    void StoreUserScanInfoSuccess2()
    {
        StoreScanConfig scanConfig;
        scanConfig.scanFreqs.push_back(FREQ_2_DOT_4_GHZ);
        std::vector<InterScanInfo> scanInfoList;
        InterScanInfo interScanInfo;
        interScanInfo.timestamp = 1;
        scanInfoList.push_back(interScanInfo);
        EXPECT_EQ(true, pScanService->StoreUserScanInfo(scanConfig, scanInfoList));
    }

    void ReportScanInfosSuccess()
    {
        EXPECT_CALL(WifiManager::GetInstance(), DealScanInfoNotify(_));
        std::vector<InterScanInfo> scanInfoList;
        pScanService->ReportScanInfos(scanInfoList);
    }

    void BeginPnoScanSuccess1()
    {
        pScanService->isPnoScanBegined = false;
        std::vector<WifiDeviceConfig> results;
        WifiDeviceConfig cfg;
        cfg.status = static_cast<int>(WifiDeviceConfigStatus::ENABLED);
        cfg.isPasspoint = false;
        cfg.isEphemeral = false;
        results.push_back(cfg);
        EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_))
            .WillRepeatedly(DoAll(SetArgReferee<0>(results), Return(0)));
        EXPECT_CALL(WifiSettings::GetInstance(), SaveScanInfoList(_)).WillRepeatedly(Return(0));
        EXPECT_CALL(WifiSettings::GetInstance(), GetMinRssi2Dot4Ghz());
        EXPECT_CALL(WifiSettings::GetInstance(), GetMinRssi5Ghz());
        EXPECT_EQ(true, pScanService->BeginPnoScan());
    }

    void BeginPnoScanFail1()
    {
        pScanService->isPnoScanBegined = false;
        EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_)).Times(AtLeast(1));
        EXPECT_CALL(WifiSettings::GetInstance(), SaveScanInfoList(_)).WillRepeatedly(Return(0));
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), StopPnoScan()).WillRepeatedly(Return(WIFI_IDL_OPT_OK));
        EXPECT_EQ(false, pScanService->BeginPnoScan());
    }

    void BeginPnoScanFail2()
    {
        pScanService->isPnoScanBegined = false;
        pScanService->staStatus = static_cast<int>(OperateResState::OPEN_WIFI_OPENING);
        EXPECT_CALL(WifiSettings::GetInstance(), SaveScanInfoList(_)).WillRepeatedly(Return(0));
        EXPECT_EQ(false, pScanService->BeginPnoScan());
    }

    void BeginPnoScanFail3()
    {
        pScanService->isPnoScanBegined = true;
        EXPECT_EQ(false, pScanService->BeginPnoScan());
    }

    void PnoScanSuccess()
    {
        PnoScanConfig pnoScanConfig;
        InterScanConfig interScanConfig;
        EXPECT_EQ(true, pScanService->PnoScan(pnoScanConfig, interScanConfig));
    }

    void AddPnoScanMessageBodySuccess()
    {
        InternalMessage interMessage;
        PnoScanConfig pnoScanConfig;
        EXPECT_EQ(true, pScanService->AddPnoScanMessageBody(&interMessage, pnoScanConfig));
    }

    void AddPnoScanMessageBodyFail()
    {
        PnoScanConfig pnoScanConfig;
        EXPECT_EQ(false, pScanService->AddPnoScanMessageBody(nullptr, pnoScanConfig));
    }

    void HandlePnoScanInfoSuccess()
    {
        EXPECT_CALL(WifiManager::GetInstance(), DealScanInfoNotify(_));
        std::vector<InterScanInfo> scanInfoList;
        InterScanInfo interScanInfo;
        interScanInfo.timestamp = 1000;
        scanInfoList.push_back(interScanInfo);
        pScanService->pnoScanStartTime = 0;
        pScanService->HandlePnoScanInfo(scanInfoList);
    }

    void EndPnoScanSuccess()
    {
        EXPECT_CALL(WifiSettings::GetInstance(), GetWhetherToAllowNetworkSwitchover()).Times(AtLeast(0));
        pScanService->isPnoScanBegined = true;
        pScanService->EndPnoScan();
    }

    void EndPnoScanFail()
    {
        pScanService->EndPnoScan();
    }

    void HandleScreenStatusChangedSuccess()
    {
        pScanService->HandleScreenStatusChanged();
    }

    void HandleStaStatusChangedSuccess1()
    {
        EXPECT_CALL(WifiSettings::GetInstance(), GetWhetherToAllowNetworkSwitchover());
        int status = static_cast<int>(OperateResState::DISCONNECT_DISCONNECTED);
        pScanService->HandleStaStatusChanged(status);
    }

    void HandleStaStatusChangedSuccess2()
    {
        EXPECT_CALL(WifiSettings::GetInstance(), GetWhetherToAllowNetworkSwitchover());
        int status = static_cast<int>(OperateResState::CONNECT_AP_CONNECTED);
        pScanService->HandleStaStatusChanged(status);
    }

    void HandleStaStatusChangedFail()
    {
        int status = static_cast<int>(OperateResState::OPEN_WIFI_FAILED);
        pScanService->HandleStaStatusChanged(status);
    }

    void HandleCustomStatusChangedSuccess1()
    {
        EXPECT_CALL(WifiSettings::GetInstance(), GetWhetherToAllowNetworkSwitchover()).Times(AtLeast(0));
        int customScene = 0;
        int customSceneStatus = MODE_STATE_CLOSE;
        pScanService->HandleCustomStatusChanged(customScene, customSceneStatus);
    }

    void HandleCustomStatusChangedSuccess2()
    {
        EXPECT_CALL(WifiSettings::GetInstance(), GetWhetherToAllowNetworkSwitchover()).Times(AtLeast(0));
        int customScene = 0;
        int customSceneStatus = MODE_STATE_OPEN;
        pScanService->HandleCustomStatusChanged(customScene, customSceneStatus);
    }

    void SystemScanProcessSuccess1()
    {
        EXPECT_CALL(WifiSettings::GetInstance(), GetWhetherToAllowNetworkSwitchover());
        ScanIntervalMode mode;
        mode.scanScene = SCAN_SCENE_ALL;
        mode.scanMode = ScanMode::SYSTEM_TIMER_SCAN;
        mode.isSingle = false;
        pScanService->scanControlInfo.scanIntervalList.push_back(mode);
        pScanService->SystemScanProcess(true);
    }

    void SystemScanProcessSuccess2()
    {
        EXPECT_CALL(WifiSettings::GetInstance(), GetWhetherToAllowNetworkSwitchover());
        EXPECT_CALL(WifiSettings::GetInstance(), GetMinRssi2Dot4Ghz()).Times(AtLeast(0));
        EXPECT_CALL(WifiSettings::GetInstance(), GetMinRssi5Ghz()).Times(AtLeast(0));
        pScanService->SystemScanProcess(true);
    }

    void StopSystemScanSuccess()
    {
        pScanService->StopSystemScan();
    }

    void StartSystemTimerScanSuccess()
    {
        pScanService->StartSystemTimerScan(true);
    }

    void HandleSystemScanTimeoutSuccess()
    {
        EXPECT_CALL(WifiSettings::GetInstance(), GetWhetherToAllowNetworkSwitchover());
        pScanService->HandleSystemScanTimeout();
    }

    void DisconnectedTimerScanSuccess()
    {
        pScanService->DisconnectedTimerScan();
    }

    void HandleDisconnectedScanTimeoutSuccess()
    {
        pScanService->HandleDisconnectedScanTimeout();
    }

    void RestartPnoScanTimeOutSuccess()
    {
        EXPECT_CALL(WifiSettings::GetInstance(), GetMinRssi2Dot4Ghz()).Times(AtLeast(0));
        EXPECT_CALL(WifiSettings::GetInstance(), GetMinRssi5Ghz()).Times(AtLeast(0));
        pScanService->RestartPnoScanTimeOut();
    }

    void GetScanControlInfoSuccess()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), StopPnoScan()).Times(AtLeast(0));
        EXPECT_CALL(WifiSettings::GetInstance(), GetScanControlInfo(_)).WillRepeatedly(Return(0));
        pScanService->GetScanControlInfo();
    }

    void GetScanControlInfoFail()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), StopPnoScan()).Times(AtLeast(0));
        EXPECT_CALL(WifiSettings::GetInstance(), GetScanControlInfo(_)).WillRepeatedly(Return(-1));
        pScanService->GetScanControlInfo();
    }
    
    void AllowExternScanSuccess()
    {
        pScanService->AllowExternScan();
    }

    void AllowExternScanFail1()
    {
        int staScene = 0;
        StoreScanConfig cfg;
        cfg.externFlag = true;
        pScanService->scanConfigMap.emplace(staScene, cfg);

        ScanMode scanMode = ScanMode::SYS_FOREGROUND_SCAN;
        ScanForbidMode forbidMode;
        forbidMode.scanScene = SCAN_SCENE_SCANNING;
        forbidMode.scanMode = scanMode;
        pScanService->scanControlInfo.scanForbidList.push_back(forbidMode);

        pScanService->AllowExternScan();
    }

    void AllowSystemTimerScanSuccess()
    {
        EXPECT_CALL(WifiSettings::GetInstance(), GetWhetherToAllowNetworkSwitchover());
        ScanIntervalMode mode;
        mode.scanScene = SCAN_SCENE_ALL;
        mode.scanMode = ScanMode::SYSTEM_TIMER_SCAN;
        mode.isSingle = false;
        pScanService->scanControlInfo.scanIntervalList.push_back(mode);
        pScanService->AllowSystemTimerScan();
    }

    void AllowPnoScanSuccess()
    {
        ScanIntervalMode mode;
        mode.scanScene = SCAN_SCENE_ALL;
        mode.scanMode = ScanMode::PNO_SCAN;
        mode.isSingle = false;
        pScanService->scanControlInfo.scanIntervalList.push_back(mode);
        pScanService->AllowPnoScan();
    }

    void AllowExternScanByForbidSuccess1()
    {
        int staScene = 0;
        StoreScanConfig cfg;
        cfg.externFlag = true;
        pScanService->scanConfigMap.emplace(staScene, cfg);
        ScanMode scanMode = ScanMode::SYS_FOREGROUND_SCAN;
        EXPECT_EQ(pScanService->AllowExternScanByForbid(staScene, scanMode), true);
    }

    void AllowExternScanByForbidFail1()
    {
        int staScene = 0;
        StoreScanConfig cfg;
        cfg.externFlag = true;
        pScanService->scanConfigMap.emplace(staScene, cfg);

        ScanMode scanMode = ScanMode::SYS_FOREGROUND_SCAN;
        ScanForbidMode forbidMode;
        forbidMode.scanScene = SCAN_SCENE_SCANNING;
        forbidMode.scanMode = scanMode;
        pScanService->scanControlInfo.scanForbidList.push_back(forbidMode);

        EXPECT_EQ(pScanService->AllowExternScanByForbid(staScene, scanMode), false);
    }

    void AllowExternScanByForbidFail2()
    {
        int staScene = 0;
        StoreScanConfig cfg;
        cfg.externFlag = true;
        pScanService->scanConfigMap.emplace(staScene, cfg);

        ScanMode scanMode = ScanMode::SYS_FOREGROUND_SCAN;
        ScanForbidMode forbidMode;
        forbidMode.scanScene = SCAN_SCENE_SCANNING;
        forbidMode.scanMode = ScanMode::ALL_EXTERN_SCAN;
        pScanService->scanControlInfo.scanForbidList.push_back(forbidMode);

        EXPECT_EQ(pScanService->AllowExternScanByForbid(staScene, scanMode), false);
    }

    void AllowExternScanByForbidFail3()
    {
        int staScene = 0;
        ScanMode scanMode = ScanMode::SYS_FOREGROUND_SCAN;
        ScanForbidMode forbidMode;
        forbidMode.scanScene = SCAN_SCENE_SCREEN_OFF;
        forbidMode.scanMode = scanMode;
        pScanService->scanControlInfo.scanForbidList.push_back(forbidMode);
        EXPECT_EQ(pScanService->AllowExternScanByForbid(staScene, scanMode), false);
    }

    void AllowExternScanByForbidFail4()
    {
        int staScene = 0;
        ScanMode scanMode = ScanMode::SYS_FOREGROUND_SCAN;
        ScanForbidMode forbidMode;
        forbidMode.scanScene = SCAN_SCENE_SCREEN_OFF;
        forbidMode.scanMode = ScanMode::ALL_EXTERN_SCAN;
        pScanService->scanControlInfo.scanForbidList.push_back(forbidMode);
        EXPECT_EQ(pScanService->AllowExternScanByForbid(staScene, scanMode), false);
    }

    void AllowExternScanByForbidFail5()
    {
        int staScene = 0;
        ScanMode scanMode = ScanMode::SYS_FOREGROUND_SCAN;
        ScanForbidMode scanForbidMode;
        scanForbidMode.scanScene = staScene;
        scanForbidMode.scanMode = scanMode;
        scanForbidMode.forbidTime = 0;
        scanForbidMode.forbidCount = 0;
        pScanService->scanControlInfo.scanForbidList.push_back(scanForbidMode);
        EXPECT_EQ(pScanService->AllowExternScanByForbid(staScene, scanMode), false);
    }

    void AllowExternScanByForbidFail6()
    {
        int staScene = 0;
        ScanMode scanMode = ScanMode::SYS_FOREGROUND_SCAN;
        ScanForbidMode scanForbidMode;
        scanForbidMode.scanScene = staScene;
        scanForbidMode.scanMode = ScanMode::ALL_EXTERN_SCAN;
        scanForbidMode.forbidTime = 0;
        scanForbidMode.forbidCount = 0;
        pScanService->scanControlInfo.scanForbidList.push_back(scanForbidMode);
        EXPECT_EQ(pScanService->AllowExternScanByForbid(staScene, scanMode), false);
    }

    void AllowExternScanByForbidFail7()
    {
        int staScene = 0;
        ScanMode scanMode = ScanMode::SYS_FOREGROUND_SCAN;
        time_t now = time(nullptr);
        if (now < 0) {
            return;
        }
        pScanService->customSceneTimeMap.emplace(staScene, now);
        ScanForbidMode scanForbidMode;
        scanForbidMode.scanScene = staScene;
        scanForbidMode.scanMode = scanMode;
        scanForbidMode.forbidTime = 0;
        scanForbidMode.forbidCount = 0;
        pScanService->scanControlInfo.scanForbidList.push_back(scanForbidMode);
        EXPECT_EQ(pScanService->AllowExternScanByForbid(staScene, scanMode), false);
    }

    void AllowExternScanByForbidFail8()
    {
        int staScene = 0;
        ScanMode scanMode = ScanMode::SYS_FOREGROUND_SCAN;
        time_t now = time(nullptr);
        if (now < 0) {
            return;
        }
        pScanService->customSceneTimeMap.emplace(staScene, now);
        ScanForbidMode scanForbidMode;
        scanForbidMode.scanScene = staScene;
        scanForbidMode.scanMode = ScanMode::ALL_EXTERN_SCAN;
        scanForbidMode.forbidTime = 0;
        scanForbidMode.forbidCount = 0;
        pScanService->scanControlInfo.scanForbidList.push_back(scanForbidMode);
        EXPECT_EQ(pScanService->AllowExternScanByForbid(staScene, scanMode), false);
    }

    void AllowExternScanByIntervaluccess()
    {
        pScanService->AllowExternScanByInterval(0, 0, ScanMode::SYS_FOREGROUND_SCAN);
    }

    void GetStaSceneSuccess1()
    {
        pScanService->staStatus = static_cast<int>(OperateResState::CONNECT_AP_CONNECTED);
        EXPECT_EQ(SCAN_SCENE_CONNECTED, pScanService->GetStaScene());
    }

    void GetStaSceneSuccess2()
    {
        pScanService->staStatus = static_cast<int>(OperateResState::DISCONNECT_DISCONNECTED);
        EXPECT_EQ(SCAN_SCENE_DISCONNCTED, pScanService->GetStaScene());
    }

    void GetStaSceneSuccess3()
    {
        pScanService->staStatus = static_cast<int>(OperateResState::CONNECT_CONNECTING);
        EXPECT_EQ(SCAN_SCENE_CONNECTING, pScanService->GetStaScene());
    }

    void GetStaSceneSuccess4()
    {
        pScanService->staStatus = static_cast<int>(OperateResState::CONNECT_OBTAINING_IP);
        EXPECT_EQ(SCAN_SCENE_OBTAINING_IP, pScanService->GetStaScene());
    }

    void GetStaSceneSuccess5()
    {
        pScanService->staStatus = static_cast<int>(OperateResState::CONNECT_ASSOCIATING);
        EXPECT_EQ(SCAN_SCENE_ASSOCIATING, pScanService->GetStaScene());
    }

    void GetStaSceneSuccess6()
    {
        pScanService->staStatus = static_cast<int>(OperateResState::CONNECT_ASSOCIATED);
        EXPECT_EQ(SCAN_SCENE_ASSOCIATED, pScanService->GetStaScene());
    }

    void GetStaSceneFail()
    {
        pScanService->staStatus = static_cast<int>(OperateResState::OPEN_WIFI_FAILED);
        EXPECT_EQ(SCAN_SCENE_MAX, pScanService->GetStaScene());
    }

    void IsExternScanningSuccess()
    {
        StoreScanConfig storeScanConfig;
        storeScanConfig.externFlag = true;
        pScanService->scanConfigMap.emplace(0, storeScanConfig);
        EXPECT_EQ(true, pScanService->IsExternScanning());
    }

    void IsExternScanningFail()
    {
        StoreScanConfig storeScanConfig;
        storeScanConfig.externFlag = false;
        pScanService->scanConfigMap.emplace(0, storeScanConfig);
        EXPECT_EQ(false, pScanService->IsExternScanning());
    }

    void GetAllowBandFreqsControlInfoSuccess()
    {
        std::vector<int> freqs;
        freqs.push_back(FREQ_2_DOT_4_GHZ);
        ScanBandType scanBand = SCAN_BAND_24_GHZ;
        pScanService->GetAllowBandFreqsControlInfo(scanBand, freqs);
    }

    void ConvertBandNotAllow24GSuccess1()
    {
        ScanBandType scanBand = SCAN_BAND_24_GHZ;
        EXPECT_EQ(SCAN_BAND_UNSPECIFIED, pScanService->ConvertBandNotAllow24G(scanBand));
    }

    void ConvertBandNotAllow24GSuccess2()
    {
        ScanBandType scanBand = SCAN_BAND_BOTH;
        EXPECT_EQ(SCAN_BAND_5_GHZ, pScanService->ConvertBandNotAllow24G(scanBand));
    }

    void ConvertBandNotAllow24GSuccess3()
    {
        ScanBandType scanBand = SCAN_BAND_BOTH_WITH_DFS;
        EXPECT_EQ(SCAN_BAND_5_GHZ_WITH_DFS, pScanService->ConvertBandNotAllow24G(scanBand));
    }

    void ConvertBandNotAllow24GSuccess4()
    {
        ScanBandType scanBand = SCAN_BAND_5_GHZ;
        EXPECT_EQ(SCAN_BAND_5_GHZ, pScanService->ConvertBandNotAllow24G(scanBand));
    }

    void ConvertBandNotAllow24GFail()
    {
        ScanBandType scanBand = SCAN_BAND_UNSPECIFIED;
        EXPECT_EQ(SCAN_BAND_UNSPECIFIED, pScanService->ConvertBandNotAllow24G(scanBand));
    }

    void ConvertBandNotAllow5GSuccess()
    {
        ScanBandType scanBand = SCAN_BAND_24_GHZ;
        EXPECT_EQ(SCAN_BAND_24_GHZ, pScanService->ConvertBandNotAllow5G(scanBand));
    }

    void ConvertBandNotAllow5GFail()
    {
        ScanBandType scanBand = SCAN_BAND_5_GHZ;
        EXPECT_EQ(SCAN_BAND_UNSPECIFIED, pScanService->ConvertBandNotAllow5G(scanBand));
    }

    void Delete24GhzFreqsSuccess()
    {
        std::vector<int> freqs;
        freqs.push_back(FREQ_2_DOT_4_GHZ);
        pScanService->Delete24GhzFreqs(freqs);
    }

    void Delete5GhzFreqsSuccess()
    {
        std::vector<int> freqs;
        freqs.push_back(FREQ_5_GHZ);
        pScanService->Delete5GhzFreqs(freqs);
    }

    void GetSavedNetworkSsidListSuccess()
    {
        EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_)).WillRepeatedly(Return(0));
        std::vector<std::string> savedNetworkSsid;
        EXPECT_EQ(true, pScanService->GetSavedNetworkSsidList(savedNetworkSsid));
    }

    void GetSavedNetworkSsidListFail()
    {
        EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_)).WillRepeatedly(Return(-1));
        std::vector<std::string> savedNetworkSsid;
        EXPECT_EQ(false, pScanService->GetSavedNetworkSsidList(savedNetworkSsid));
    }

    void GetHiddenNetworkSsidListSuccess1()
    {
        EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_)).WillRepeatedly(Return(0));
        std::vector<std::string> hiddenNetworkSsid;
        EXPECT_EQ(true, pScanService->GetHiddenNetworkSsidList(hiddenNetworkSsid));
    }

    void GetHiddenNetworkSsidListSuccess2()
    {
        std::vector<WifiDeviceConfig> deviceConfigs;
        WifiDeviceConfig cfg;
        cfg.hiddenSSID = true;
        deviceConfigs.push_back(cfg);
        EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_))
            .WillRepeatedly(DoAll(SetArgReferee<0>(deviceConfigs), Return(0)));
        std::vector<std::string> hiddenNetworkSsid;
        EXPECT_EQ(true, pScanService->GetHiddenNetworkSsidList(hiddenNetworkSsid));
    }

    void GetHiddenNetworkSsidListFail()
    {
        EXPECT_CALL(WifiSettings::GetInstance(), GetDeviceConfig(_)).WillRepeatedly(Return(-1));
        std::vector<std::string> hiddenNetworkSsid;
        EXPECT_EQ(false, pScanService->GetHiddenNetworkSsidList(hiddenNetworkSsid));
    }

    void ClearScanControlValueSuccess()
    {
        pScanService->ClearScanControlValue();
    }

    void SetStaCurrentTimeSuccess()
    {
        pScanService->SetStaCurrentTime();
    }

    void AllowScanDuringScanningSuccess()
    {
        ScanMode scanMode = ScanMode::SYS_FOREGROUND_SCAN;
        EXPECT_EQ(pScanService->AllowScanDuringScanning(scanMode), true);
    }

    void AllowScanDuringScanningFail()
    {
        ScanMode scanMode = ScanMode::SYS_FOREGROUND_SCAN;
        ScanForbidMode forbidMode;
        forbidMode.scanScene = SCAN_SCENE_SCANNING;
        forbidMode.scanMode = scanMode;
        pScanService->scanControlInfo.scanForbidList.push_back(forbidMode);
        EXPECT_EQ(pScanService->AllowScanDuringScanning(scanMode), false);
    }

    void AllowScanDuringScreenOffSuccess()
    {
        ScanMode scanMode = ScanMode::SYS_FOREGROUND_SCAN;
        EXPECT_EQ(pScanService->AllowScanDuringScreenOff(scanMode), true);
    }

    void AllowScanDuringScreenOffFail1()
    {
        ScanMode scanMode = ScanMode::SYS_FOREGROUND_SCAN;
        ScanForbidMode forbidMode;
        forbidMode.scanScene = SCAN_SCENE_SCREEN_OFF;
        forbidMode.scanMode = scanMode;
        pScanService->scanControlInfo.scanForbidList.push_back(forbidMode);

        EXPECT_EQ(pScanService->AllowScanDuringScreenOff(scanMode), false);
    }

    void AllowScanDuringStaSceneSuccess1()
    {
        const int staScene = 0;
        ScanMode scanMode = ScanMode::SYS_FOREGROUND_SCAN;
        ScanForbidMode scanForbidMode;
        scanForbidMode.scanScene = staScene;
        scanForbidMode.scanMode = scanMode;
        scanForbidMode.forbidTime = 1;
        scanForbidMode.forbidCount = 1;
        pScanService->staSceneForbidCount = 1;
        time_t nowTime = time(nullptr);
        const int timeForTest = 2;
        pScanService->staCurrentTime = nowTime - timeForTest;
        pScanService->scanControlInfo.scanForbidList.push_back(scanForbidMode);
        EXPECT_EQ(pScanService->AllowScanDuringStaScene(staScene, scanMode), true);
    }

    void AllowScanDuringStaSceneFail1()
    {
        const int staScene = 0;
        ScanMode scanMode = ScanMode::SYS_FOREGROUND_SCAN;
        ScanForbidMode scanForbidMode;
        scanForbidMode.scanScene = staScene;
        scanForbidMode.scanMode = scanMode;
        scanForbidMode.forbidTime = 0;
        scanForbidMode.forbidCount = 0;
        pScanService->scanControlInfo.scanForbidList.push_back(scanForbidMode);
        EXPECT_EQ(pScanService->AllowScanDuringStaScene(staScene, scanMode), false);
    }

    void AllowScanDuringStaSceneFail2()
    {
        const int staScene = 0;
        ScanMode scanMode = ScanMode::SYS_FOREGROUND_SCAN;
        ScanForbidMode scanForbidMode;
        scanForbidMode.scanScene = staScene;
        scanForbidMode.scanMode = scanMode;
        scanForbidMode.forbidTime = 1;
        scanForbidMode.forbidCount = 1;
        pScanService->staSceneForbidCount = 0;
        pScanService->scanControlInfo.scanForbidList.push_back(scanForbidMode);
        EXPECT_EQ(pScanService->AllowScanDuringStaScene(staScene, scanMode), false);
    }

    void AllowScanDuringStaSceneFail3()
    {
        const int staScene = 0;
        ScanMode scanMode = ScanMode::SYS_FOREGROUND_SCAN;
        ScanForbidMode scanForbidMode;
        scanForbidMode.scanScene = staScene;
        scanForbidMode.scanMode = scanMode;
        scanForbidMode.forbidTime = 1;
        scanForbidMode.forbidCount = 1;
        pScanService->staSceneForbidCount = 1;
        pScanService->staSceneForbidCount = 1;
        time_t nowTime = time(nullptr);
        const int timeForTest = 2;
        pScanService->staCurrentTime = nowTime + timeForTest;
        pScanService->scanControlInfo.scanForbidList.push_back(scanForbidMode);
        EXPECT_EQ(pScanService->AllowScanDuringStaScene(staScene, scanMode), false);
    }

    void AllowScanDuringCustomSceneSuccess()
    {
        ScanMode scanMode = ScanMode::SYS_FOREGROUND_SCAN;
        EXPECT_EQ(pScanService->AllowScanDuringCustomScene(scanMode), true);
    }

    void AllowScanDuringCustomSceneFail1()
    {
        const int staScene = 0;
        ScanMode scanMode = ScanMode::SYS_FOREGROUND_SCAN;
        time_t now = time(nullptr);
        if (now < 0) {
            return;
        }
        pScanService->customSceneTimeMap.emplace(staScene, now);
        ScanForbidMode scanForbidMode;
        scanForbidMode.scanScene = staScene;
        scanForbidMode.scanMode = scanMode;
        scanForbidMode.forbidTime = 0;
        scanForbidMode.forbidCount = 0;
        pScanService->scanControlInfo.scanForbidList.push_back(scanForbidMode);
        EXPECT_EQ(pScanService->AllowScanDuringCustomScene(scanMode), false);
    }

    void AllowScanDuringCustomSceneFail2()
    {
        const int staScene = 0;
        ScanMode scanMode = ScanMode::SYS_FOREGROUND_SCAN;
        time_t now = time(nullptr);
        if (now < 0) {
            return;
        }
        pScanService->customSceneTimeMap.emplace(staScene, now);
        ScanForbidMode scanForbidMode;
        scanForbidMode.scanScene = staScene;
        scanForbidMode.scanMode = scanMode;
        scanForbidMode.forbidTime = 1;
        scanForbidMode.forbidCount = 1;
        pScanService->staSceneForbidCount = 0;
        pScanService->scanControlInfo.scanForbidList.push_back(scanForbidMode);
        EXPECT_EQ(pScanService->AllowScanDuringCustomScene(scanMode), false);
    }

    void AllowScanDuringCustomSceneFail3()
    {
        const int staScene = 0;
        ScanMode scanMode = ScanMode::SYS_FOREGROUND_SCAN;
        time_t now = time(nullptr);
        if (now < 0) {
            return;
        }
        pScanService->customSceneTimeMap.emplace(staScene, now);
        ScanForbidMode scanForbidMode;
        scanForbidMode.scanScene = staScene;
        scanForbidMode.scanMode = scanMode;
        scanForbidMode.forbidTime = 1;
        scanForbidMode.forbidCount = 1;
        time_t nowTime = time(nullptr);
        const int timeForTest = 2;
        pScanService->staCurrentTime = nowTime + timeForTest;
        pScanService->scanControlInfo.scanForbidList.push_back(scanForbidMode);
        EXPECT_EQ(pScanService->AllowScanDuringCustomScene(scanMode), false);
    }

    void AllowExternScanByIntervalMode()
    {
        pScanService->AllowExternScanByIntervalMode(0, 0, ScanMode::SYS_FOREGROUND_SCAN);
    }

    void AllowExternScanByCustomSceneSuccess()
    {
        pScanService->AllowExternScanByCustomScene(0, ScanMode::SYS_FOREGROUND_SCAN);
    }

    void PnoScanByIntervalSuccess1()
    {
        int fixedScanCount = 0;
        time_t fixedScanTime = 0;
        int interval = 0;
        int count = 0;
        EXPECT_EQ(pScanService->PnoScanByInterval(fixedScanCount, fixedScanTime, interval, count), true);
    }

    void PnoScanByIntervalSuccess2()
    {
        int fixedScanCount = 1;
        time_t fixedScanTime = time(nullptr) - 1;
        int interval = 0;
        int count = 0;
        EXPECT_EQ(pScanService->PnoScanByInterval(fixedScanCount, fixedScanTime, interval, count), true);
    }

    void PnoScanByIntervalSuccess3()
    {
        int fixedScanCount = 1;
        time_t fixedScanTime = time(nullptr) + 1;
        int interval = 1;
        int count = 1;
        EXPECT_EQ(pScanService->PnoScanByInterval(fixedScanCount, fixedScanTime, interval, count), true);
    }

    void PnoScanByIntervalFail1()
    {
        int fixedScanCount = 2;
        time_t fixedScanTime = time(nullptr) + 1;
        int interval = 1;
        int count = 1;
        EXPECT_EQ(pScanService->PnoScanByInterval(fixedScanCount, fixedScanTime, interval, count), false);
    }

    void SystemScanByIntervalSuccess()
    {
        int expScanCount = 1;
        int interval = 1;
        const int constTest = 2;
        int count = constTest;
        EXPECT_EQ(pScanService->SystemScanByInterval(expScanCount, interval, count), true);
    }

    void ExternScanByIntervalSuccess1()
    {
        SingleAppForbid singleAppForbid;
        singleAppForbid.scanIntervalMode.intervalMode = IntervalMode::INTERVAL_FIXED;
        EXPECT_EQ(pScanService->ExternScanByInterval(0, singleAppForbid), true);
    }

    void ExternScanByIntervalSuccess2()
    {
        SingleAppForbid singleAppForbid;
        singleAppForbid.scanIntervalMode.intervalMode = IntervalMode::INTERVAL_EXP;
        EXPECT_EQ(pScanService->ExternScanByInterval(0, singleAppForbid), true);
    }

    void ExternScanByIntervalSuccess3()
    {
        SingleAppForbid singleAppForbid;
        singleAppForbid.scanIntervalMode.intervalMode = IntervalMode::INTERVAL_CONTINUE;
        EXPECT_EQ(pScanService->ExternScanByInterval(0, singleAppForbid), true);
    }

    void ExternScanByIntervalSuccess4()
    {
        SingleAppForbid singleAppForbid;
        singleAppForbid.scanIntervalMode.intervalMode = IntervalMode::INTERVAL_BLOCKLIST;
        EXPECT_EQ(pScanService->ExternScanByInterval(0, singleAppForbid), true);
    }

    void ExternScanByIntervalSuccess5()
    {
        SingleAppForbid singleAppForbid;
        singleAppForbid.scanIntervalMode.intervalMode = IntervalMode::INTERVAL_MAX;
        EXPECT_EQ(pScanService->ExternScanByInterval(0, singleAppForbid), true);
    }

    void AllowSingleAppScanByIntervalSuccess1()
    {
        int appId = 0;
        ScanIntervalMode scanIntervalMode;
        EXPECT_EQ(pScanService->AllowSingleAppScanByInterval(appId, scanIntervalMode), true);
    }

    void AllowSingleAppScanByIntervalSuccess2()
    {
        int appId = 0;
        SingleAppForbid sAppForbid;
        sAppForbid.appID = appId;
        sAppForbid.scanIntervalMode.scanScene = SCAN_SCENE_ALL;
        sAppForbid.scanIntervalMode.scanMode = ScanMode::SCAN_MODE_MAX;
        pScanService->appForbidList.push_back(sAppForbid);
        ScanIntervalMode scanIntervalMode = sAppForbid.scanIntervalMode;
        EXPECT_EQ(pScanService->AllowSingleAppScanByInterval(appId, scanIntervalMode), true);
    }

    void AllowSingleAppScanByIntervalFail1()
    {
        int appId = 0;
        SingleAppForbid sAppForbid;
        sAppForbid.appID = appId;
        sAppForbid.scanIntervalMode.scanScene = SCAN_SCENE_ALL;
        sAppForbid.scanIntervalMode.scanMode = ScanMode::SCAN_MODE_MAX;

        sAppForbid.fixedScanCount = 1;
        sAppForbid.fixedCurrentTime = time(nullptr) - 1;
        const int intervalOrCount = 2;
        sAppForbid.scanIntervalMode.interval = intervalOrCount;
        sAppForbid.scanIntervalMode.count = 1;
        sAppForbid.scanIntervalMode.intervalMode = IntervalMode::INTERVAL_FIXED;

        pScanService->appForbidList.push_back(sAppForbid);

        ScanIntervalMode scanIntervalMode = sAppForbid.scanIntervalMode;
        EXPECT_EQ(pScanService->AllowSingleAppScanByInterval(appId, scanIntervalMode), false);
    }

    void AllowFullAppScanByIntervalSuccess1()
    {
        int appId = 0;
        ScanIntervalMode scanIntervalMode;
        EXPECT_EQ(pScanService->AllowFullAppScanByInterval(appId, scanIntervalMode), true);
    }

    void AllowFullAppScanByIntervalSuccess2()
    {
        int appId = 0;
        SingleAppForbid sAppForbid;
        sAppForbid.appID = appId;
        sAppForbid.scanIntervalMode.scanScene = SCAN_SCENE_ALL;
        sAppForbid.scanIntervalMode.scanMode = ScanMode::SCAN_MODE_MAX;
        pScanService->fullAppForbidList.push_back(sAppForbid);
        ScanIntervalMode scanIntervalMode = sAppForbid.scanIntervalMode;
        EXPECT_EQ(pScanService->AllowFullAppScanByInterval(appId, scanIntervalMode), true);
    }

    void AllowFullAppScanByIntervalFail1()
    {
        int appId = 0;
        SingleAppForbid sAppForbid;
        sAppForbid.appID = appId;
        sAppForbid.scanIntervalMode.scanScene = SCAN_SCENE_ALL;
        sAppForbid.scanIntervalMode.scanMode = ScanMode::SCAN_MODE_MAX;

        sAppForbid.fixedScanCount = 1;
        sAppForbid.fixedCurrentTime = time(nullptr) - 1;
        const int intervalOrCount = 2;
        sAppForbid.scanIntervalMode.interval = intervalOrCount;
        sAppForbid.scanIntervalMode.count = 1;
        sAppForbid.scanIntervalMode.intervalMode = IntervalMode::INTERVAL_FIXED;

        pScanService->fullAppForbidList.push_back(sAppForbid);

        ScanIntervalMode scanIntervalMode = sAppForbid.scanIntervalMode;
        EXPECT_EQ(pScanService->AllowFullAppScanByInterval(appId, scanIntervalMode), false);
    }

    void AllowScanByIntervalFixedSuccess1()
    {
        int fixedScanCount = 0;
        time_t fixedScanTime = 0;
        int interval = 0;
        int count = 0;
        bool rlt = pScanService->AllowScanByIntervalFixed(fixedScanCount, fixedScanTime, interval, count);
        EXPECT_EQ(rlt, true);
    }

    void AllowScanByIntervalFixedSuccess2()
    {
        int fixedScanCount = 1;
        time_t fixedScanTime = 0;
        int interval = 0;
        int count = 0;
        bool rlt = pScanService->AllowScanByIntervalFixed(fixedScanCount, fixedScanTime, interval, count);
        EXPECT_EQ(rlt, true);
        fixedScanTime = time(nullptr);
        interval = 0;
        count = 1;
        rlt = pScanService->AllowScanByIntervalFixed(fixedScanCount, fixedScanTime, interval, count);
        EXPECT_EQ(rlt, true);
    }

    void AllowScanByIntervalFixedSuccess3()
    {
        int fixedScanCount = 1;
        time_t fixedScanTime = time(nullptr) - 1;
        const int intervalOrCount = 2;
        int interval = intervalOrCount;
        int count = intervalOrCount;
        bool rlt = pScanService->AllowScanByIntervalFixed(fixedScanCount, fixedScanTime, interval, count);
        EXPECT_EQ(rlt, true);
    }

    void AllowScanByIntervalFixedFail1()
    {
        int fixedScanCount = 1;
        time_t fixedScanTime = time(nullptr) - 1;
        const int intervalOrCount = 2;
        int interval = intervalOrCount;
        int count = 1;
        bool rlt = pScanService->AllowScanByIntervalFixed(fixedScanCount, fixedScanTime, interval, count);
        EXPECT_EQ(rlt, false);
    }

    void AllowScanByIntervalExpSuccess()
    {
        int expScanCount = 1;
        int interval = 1;
        const int conutTest = 2;
        int count = conutTest;
        EXPECT_EQ(pScanService->AllowScanByIntervalExp(expScanCount, interval, count), true);
    }

    void AllowScanByIntervalContinueSuccess1()
    {
        time_t continueScanTime = 0;
        int lessThanIntervalCount = 0;
        int interval = 0;
        int count = 0;
        bool rlt = pScanService->AllowScanByIntervalContinue(continueScanTime, lessThanIntervalCount, interval, count);
        EXPECT_EQ(rlt, true);
    }

    void AllowScanByIntervalContinueSuccess2()
    {
        const int timeTest = 2;
        time_t continueScanTime = time(nullptr) - timeTest;
        const int intervalTest = 5;
        int interval = intervalTest;
        int lessThanIntervalCount = 0;
        const int countTest = 2;
        int count = countTest;
        bool rlt = pScanService->AllowScanByIntervalContinue(continueScanTime, lessThanIntervalCount, interval, count);
        EXPECT_EQ(rlt, true);
    }

    void AllowScanByIntervalContinueSuccess3()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), StopPnoScan()).Times(AtLeast(0));

        const int timeTest = 5;
        time_t continueScanTime = time(nullptr) - timeTest;
        int lessThanIntervalCount = 0;
        const int intervalTest = 1;
        int interval = intervalTest;
        int count = 0;
        bool rlt = pScanService->AllowScanByIntervalContinue(continueScanTime, lessThanIntervalCount, interval, count);
        EXPECT_EQ(rlt, true);
    }

    void AllowScanByIntervalContinueFail1()
    {
        const int timeTest = 2;
        time_t continueScanTime = time(nullptr) - timeTest;
        const int intervalTest = 5;
        int interval = intervalTest;
        int lessThanIntervalCount = 0;
        const int countTest = 1;
        int count = countTest;
        // lessThanIntervalCount = 0 branch.
        bool rlt = pScanService->AllowScanByIntervalContinue(continueScanTime, lessThanIntervalCount, interval, count);
        EXPECT_EQ(rlt, false);
    }

    void AllowScanByIntervalBlocklistSuccess1()
    {
        int appId = 0;
        const int timeTest = 2;
        time_t blockListScanTime = time(nullptr) - timeTest;
        int lessThanIntervalCount = 0;
        int interval = 0;
        int count = 0;
        pScanService->scanBlocklist.push_back(appId);
        pScanService->scanBlocklist.push_back(1);
        bool rlt = pScanService->AllowScanByIntervalBlocklist(
            appId, blockListScanTime, lessThanIntervalCount, interval, count);
        EXPECT_EQ(rlt, true);
    }

    void AllowScanByIntervalBlocklistSuccess2()
    {
        EXPECT_CALL(WifiStaHalInterface::GetInstance(), StopPnoScan()).Times(AtLeast(0));

        int appId = 0;
        const int timeTest = 2;
        time_t blockListScanTime = 0;
        int lessThanIntervalCount = 0;
        int interval = time(nullptr) + timeTest;
        int count = 0;
        bool rlt = pScanService->AllowScanByIntervalBlocklist(
            appId, blockListScanTime, lessThanIntervalCount, interval, count);
        EXPECT_EQ(rlt, true);
    }

    void AllowScanByIntervalBlocklistSuccess3()
    {
        int appId = 0;
        const int timeTest = 5;
        time_t blockListScanTime = time(nullptr) - timeTest;
        int lessThanIntervalCount = 0;
        const int intervalTest = 10;
        int interval = intervalTest;
        const int countTest = 2;
        int count = countTest;
        bool rlt = pScanService->AllowScanByIntervalBlocklist(
            appId, blockListScanTime, lessThanIntervalCount, interval, count);
        EXPECT_EQ(rlt, true);
    }

    void AllowScanByIntervalBlocklistFail1()
    {
        int appId = 0;
        const int timeTest = 5;
        time_t blockListScanTime = time(nullptr) - timeTest;
        int lessThanIntervalCount = 0;
        const int intervalTest = 10;
        int interval = intervalTest;
        int count = 0;
        pScanService->scanBlocklist.push_back(appId);
        bool rlt = pScanService->AllowScanByIntervalBlocklist(
            appId, blockListScanTime, lessThanIntervalCount, interval, count);
        EXPECT_EQ(rlt, false);
    }

    void AllowScanByIntervalBlocklistFail2()
    {
        int appId = 0;
        const int timeTest = 5;
        time_t blockListScanTime = time(nullptr) - timeTest;
        int lessThanIntervalCount = 0;
        const int intervalTest = 10;
        int interval = intervalTest;
        int count = 0;
        bool rlt = pScanService->AllowScanByIntervalBlocklist(
            appId, blockListScanTime, lessThanIntervalCount, interval, count);
        EXPECT_EQ(rlt, false);
    }
};

HWTEST_F(ScanServiceTest, InitScanServiceSuccess1, TestSize.Level1)
{
    InitScanServiceSuccess1();
}

HWTEST_F(ScanServiceTest, InitScanServiceSuccess2, TestSize.Level1)
{
    InitScanServiceSuccess2();
}

HWTEST_F(ScanServiceTest, UnInitScanServiceSuccess, TestSize.Level1)
{
    UnInitScanServiceSuccess();
}

HWTEST_F(ScanServiceTest, HandleScanStatusReportSuccess1, TestSize.Level1)
{
    HandleScanStatusReportSuccess1();
}

HWTEST_F(ScanServiceTest, HandleScanStatusReportSuccess2, TestSize.Level1)
{
    HandleScanStatusReportSuccess2();
}

HWTEST_F(ScanServiceTest, HandleScanStatusReportSuccess3, TestSize.Level1)
{
    HandleScanStatusReportSuccess3();
}

HWTEST_F(ScanServiceTest, HandleScanStatusReportSuccess4, TestSize.Level1)
{
    HandleScanStatusReportSuccess4();
}

HWTEST_F(ScanServiceTest, HandleScanStatusReportSuccess5, TestSize.Level1)
{
    HandleScanStatusReportSuccess5();
}

HWTEST_F(ScanServiceTest, HandleScanStatusReportSuccess6, TestSize.Level1)
{
    HandleScanStatusReportSuccess6();
}

HWTEST_F(ScanServiceTest, HandleScanStatusReportSuccess7, TestSize.Level1)
{
    HandleScanStatusReportSuccess7();
}

HWTEST_F(ScanServiceTest, HandleScanStatusReportFail, TestSize.Level1)
{
    HandleScanStatusReportFail();
}

HWTEST_F(ScanServiceTest, HandleInnerEventReportSuccess1, TestSize.Level1)
{
    HandleInnerEventReportSuccess1();
}

HWTEST_F(ScanServiceTest, HandleInnerEventReportSuccess2, TestSize.Level1)
{
    HandleInnerEventReportSuccess2();
}

HWTEST_F(ScanServiceTest, HandleInnerEventReportSuccess3, TestSize.Level1)
{
    HandleInnerEventReportSuccess3();
}

HWTEST_F(ScanServiceTest, HandleInnerEventReportFail, TestSize.Level1)
{
    HandleInnerEventReportFail();
}

HWTEST_F(ScanServiceTest, ScanSuccess, TestSize.Level1)
{
    ScanSuccess();
}

HWTEST_F(ScanServiceTest, ScanFail, TestSize.Level1)
{
    ScanFail();
}

HWTEST_F(ScanServiceTest, ScanWithParamSuccess, TestSize.Level1)
{
    ScanWithParamSuccess();
}

HWTEST_F(ScanServiceTest, ScanWithParamFail1, TestSize.Level1)
{
    ScanWithParamFail1();
}

HWTEST_F(ScanServiceTest, ScanWithParamFail2, TestSize.Level1)
{
    ScanWithParamFail2();
}

HWTEST_F(ScanServiceTest, ScanWithParamFail3, TestSize.Level1)
{
    ScanWithParamFail3();
}

HWTEST_F(ScanServiceTest, ScanWithParamFail4, TestSize.Level1)
{
    ScanWithParamFail4();
}

HWTEST_F(ScanServiceTest, SingleScanSuccess1, TestSize.Level1)
{
    SingleScanSuccess1();
}

HWTEST_F(ScanServiceTest, SingleScanSuccess2, TestSize.Level1)
{
    SingleScanSuccess2();
}

HWTEST_F(ScanServiceTest, SingleScanFail1, TestSize.Level1)
{
    SingleScanFail1();
}

HWTEST_F(ScanServiceTest, GetBandFreqsSuccess1, TestSize.Level1)
{
    GetBandFreqsSuccess1();
}

HWTEST_F(ScanServiceTest, GetBandFreqsSuccess2, TestSize.Level1)
{
    GetBandFreqsSuccess2();
}

HWTEST_F(ScanServiceTest, GetBandFreqsSuccess3, TestSize.Level1)
{
    GetBandFreqsSuccess3();
}

HWTEST_F(ScanServiceTest, GetBandFreqsSuccess4, TestSize.Level1)
{
    GetBandFreqsSuccess4();
}

HWTEST_F(ScanServiceTest, GetBandFreqsSuccess5, TestSize.Level1)
{
    GetBandFreqsSuccess5();
}

HWTEST_F(ScanServiceTest, GetBandFreqsSuccess6, TestSize.Level1)
{
    GetBandFreqsSuccess6();
}

HWTEST_F(ScanServiceTest, GetBandFreqsFail, TestSize.Level1)
{
    GetBandFreqsFail();
}

HWTEST_F(ScanServiceTest, AddScanMessageBodySuccess, TestSize.Level1)
{
    AddScanMessageBodySuccess();
}

HWTEST_F(ScanServiceTest, AddScanMessageBodyFail, TestSize.Level1)
{
    AddScanMessageBodyFail();
}

HWTEST_F(ScanServiceTest, StoreRequestScanConfigSuccess, TestSize.Level1)
{
    StoreRequestScanConfigSuccess();
}

HWTEST_F(ScanServiceTest, HandleCommonScanFailedSuccess1, TestSize.Level1)
{
    HandleCommonScanFailedSuccess1();
}

HWTEST_F(ScanServiceTest, HandleCommonScanFailedSuccess2, TestSize.Level1)
{
    HandleCommonScanFailedSuccess2();
}

HWTEST_F(ScanServiceTest, HandleCommonScanFailedSuccess3, TestSize.Level1)
{
    HandleCommonScanFailedSuccess3();
}

HWTEST_F(ScanServiceTest, HandleCommonScanFailedSuccess4, TestSize.Level1)
{
    HandleCommonScanFailedSuccess4();
}

HWTEST_F(ScanServiceTest, HandleCommonScanInfoSuccess1, TestSize.Level1)
{
    HandleCommonScanInfoSuccess1();
}

HWTEST_F(ScanServiceTest, HandleCommonScanInfoSuccess2, TestSize.Level1)
{
    HandleCommonScanInfoSuccess2();
}

HWTEST_F(ScanServiceTest, HandleCommonScanInfoSuccess3, TestSize.Level1)
{
    HandleCommonScanInfoSuccess3();
}

HWTEST_F(ScanServiceTest, StoreFullScanInfoSuccess, TestSize.Level1)
{
    StoreFullScanInfoSuccess();
}

HWTEST_F(ScanServiceTest, StoreFullScanInfoFail, TestSize.Level1)
{
    StoreFullScanInfoFail();
}

HWTEST_F(ScanServiceTest, StoreUserScanInfoSuccess1, TestSize.Level1)
{
    StoreUserScanInfoSuccess1();
}

HWTEST_F(ScanServiceTest, StoreUserScanInfoSuccess2, TestSize.Level1)
{
    StoreUserScanInfoSuccess2();
}

HWTEST_F(ScanServiceTest, ReportScanInfosSuccess, TestSize.Level1)
{
    ReportScanInfosSuccess();
}

HWTEST_F(ScanServiceTest, BeginPnoScanSuccess1, TestSize.Level1)
{
    BeginPnoScanSuccess1();
}

HWTEST_F(ScanServiceTest, BeginPnoScanFail1, TestSize.Level1)
{
    BeginPnoScanFail1();
}

HWTEST_F(ScanServiceTest, BeginPnoScanFail2, TestSize.Level1)
{
    BeginPnoScanFail2();
}

HWTEST_F(ScanServiceTest, BeginPnoScanFail3, TestSize.Level1)
{
    BeginPnoScanFail3();
}

HWTEST_F(ScanServiceTest, PnoScanSuccess, TestSize.Level1)
{
    PnoScanSuccess();
}

HWTEST_F(ScanServiceTest, AddPnoScanMessageBodySuccess, TestSize.Level1)
{
    AddPnoScanMessageBodySuccess();
}

HWTEST_F(ScanServiceTest, AddPnoScanMessageBodyFail, TestSize.Level1)
{
    AddPnoScanMessageBodyFail();
}

HWTEST_F(ScanServiceTest, HandlePnoScanInfoSuccess, TestSize.Level1)
{
    HandlePnoScanInfoSuccess();
}

HWTEST_F(ScanServiceTest, EndPnoScanSuccess, TestSize.Level1)
{
    EndPnoScanSuccess();
}

HWTEST_F(ScanServiceTest, EndPnoScanFail, TestSize.Level1)
{
    EndPnoScanFail();
}

HWTEST_F(ScanServiceTest, HandleScreenStatusChangedSuccess, TestSize.Level1)
{
    HandleScreenStatusChangedSuccess();
}

HWTEST_F(ScanServiceTest, HandleStaStatusChangedSuccess1, TestSize.Level1)
{
    HandleStaStatusChangedSuccess1();
}

HWTEST_F(ScanServiceTest, HandleStaStatusChangedSuccess2, TestSize.Level1)
{
    HandleStaStatusChangedSuccess2();
}

HWTEST_F(ScanServiceTest, HandleStaStatusChangedFail, TestSize.Level1)
{
    HandleStaStatusChangedFail();
}

HWTEST_F(ScanServiceTest, HandleCustomStatusChangedSuccess1, TestSize.Level1)
{
    HandleCustomStatusChangedSuccess1();
}

HWTEST_F(ScanServiceTest, HandleCustomStatusChangedSuccess2, TestSize.Level1)
{
    HandleCustomStatusChangedSuccess2();
}

HWTEST_F(ScanServiceTest, SystemScanProcessSuccess1, TestSize.Level1)
{
    SystemScanProcessSuccess1();
}

HWTEST_F(ScanServiceTest, SystemScanProcessSuccess2, TestSize.Level1)
{
    SystemScanProcessSuccess2();
}

HWTEST_F(ScanServiceTest, StopSystemScanSuccess, TestSize.Level1)
{
    StopSystemScanSuccess();
}

HWTEST_F(ScanServiceTest, StartSystemTimerScanSuccess, TestSize.Level1)
{
    StartSystemTimerScanSuccess();
}

HWTEST_F(ScanServiceTest, HandleSystemScanTimeoutSuccess, TestSize.Level1)
{
    HandleSystemScanTimeoutSuccess();
}

HWTEST_F(ScanServiceTest, DisconnectedTimerScanSuccess, TestSize.Level1)
{
    DisconnectedTimerScanSuccess();
}

HWTEST_F(ScanServiceTest, HandleDisconnectedScanTimeoutSuccess, TestSize.Level1)
{
    HandleDisconnectedScanTimeoutSuccess();
}

HWTEST_F(ScanServiceTest, RestartPnoScanTimeOutSuccess, TestSize.Level1)
{
    RestartPnoScanTimeOutSuccess();
}

HWTEST_F(ScanServiceTest, GetScanControlInfoSuccess, TestSize.Level1)
{
    GetScanControlInfoSuccess();
}

HWTEST_F(ScanServiceTest, GetScanControlInfoFail, TestSize.Level1)
{
    GetScanControlInfoFail();
}

HWTEST_F(ScanServiceTest, AllowExternScanSuccess, TestSize.Level1)
{
    AllowExternScanSuccess();
}

HWTEST_F(ScanServiceTest, AllowExternScanFail1, TestSize.Level1)
{
    AllowExternScanFail1();
}

HWTEST_F(ScanServiceTest, AllowSystemTimerScanSuccess, TestSize.Level1)
{
    AllowSystemTimerScanSuccess();
}

HWTEST_F(ScanServiceTest, AllowPnoScanSuccess, TestSize.Level1)
{
    AllowPnoScanSuccess();
}

HWTEST_F(ScanServiceTest, AllowExternScanByForbidSuccess1, TestSize.Level1)
{
    AllowExternScanByForbidSuccess1();
}

HWTEST_F(ScanServiceTest, AllowExternScanByForbidFail1, TestSize.Level1)
{
    AllowExternScanByForbidFail1();
}

HWTEST_F(ScanServiceTest, AllowExternScanByForbidFail2, TestSize.Level1)
{
    AllowExternScanByForbidFail2();
}

HWTEST_F(ScanServiceTest, AllowExternScanByForbidFail3, TestSize.Level1)
{
    AllowExternScanByForbidFail3();
}

HWTEST_F(ScanServiceTest, AllowExternScanByForbidFail4, TestSize.Level1)
{
    AllowExternScanByForbidFail4();
}

HWTEST_F(ScanServiceTest, AllowExternScanByForbidFail5, TestSize.Level1)
{
    AllowExternScanByForbidFail5();
}

HWTEST_F(ScanServiceTest, AllowExternScanByForbidFail6, TestSize.Level1)
{
    AllowExternScanByForbidFail6();
}

HWTEST_F(ScanServiceTest, AllowExternScanByForbidFail7, TestSize.Level1)
{
    AllowExternScanByForbidFail7();
}

HWTEST_F(ScanServiceTest, AllowExternScanByForbidFail8, TestSize.Level1)
{
    AllowExternScanByForbidFail8();
}

HWTEST_F(ScanServiceTest, AllowExternScanByIntervaluccess, TestSize.Level1)
{
    AllowExternScanByIntervaluccess();
}

HWTEST_F(ScanServiceTest, GetStaSceneSuccess1, TestSize.Level1)
{
    GetStaSceneSuccess1();
}

HWTEST_F(ScanServiceTest, GetStaSceneSuccess2, TestSize.Level1)
{
    GetStaSceneSuccess2();
}

HWTEST_F(ScanServiceTest, GetStaSceneSuccess3, TestSize.Level1)
{
    GetStaSceneSuccess3();
}

HWTEST_F(ScanServiceTest, GetStaSceneSuccess4, TestSize.Level1)
{
    GetStaSceneSuccess4();
}

HWTEST_F(ScanServiceTest, GetStaSceneSuccess5, TestSize.Level1)
{
    GetStaSceneSuccess5();
}

HWTEST_F(ScanServiceTest, GetStaSceneSuccess6, TestSize.Level1)
{
    GetStaSceneSuccess6();
}

HWTEST_F(ScanServiceTest, GetStaSceneFail, TestSize.Level1)
{
    GetStaSceneFail();
}

HWTEST_F(ScanServiceTest, IsExternScanningSuccess, TestSize.Level1)
{
    IsExternScanningSuccess();
}

HWTEST_F(ScanServiceTest, IsExternScanningFail, TestSize.Level1)
{
    IsExternScanningFail();
}

HWTEST_F(ScanServiceTest, GetAllowBandFreqsControlInfoSuccess, TestSize.Level1)
{
    GetAllowBandFreqsControlInfoSuccess();
}

HWTEST_F(ScanServiceTest, ConvertBandNotAllow24GSuccess1, TestSize.Level1)
{
    ConvertBandNotAllow24GSuccess1();
}

HWTEST_F(ScanServiceTest, ConvertBandNotAllow24GSuccess2, TestSize.Level1)
{
    ConvertBandNotAllow24GSuccess2();
}

HWTEST_F(ScanServiceTest, ConvertBandNotAllow24GSuccess3, TestSize.Level1)
{
    ConvertBandNotAllow24GSuccess3();
}

HWTEST_F(ScanServiceTest, ConvertBandNotAllow24GSuccess4, TestSize.Level1)
{
    ConvertBandNotAllow24GSuccess4();
}

HWTEST_F(ScanServiceTest, ConvertBandNotAllow24GFail, TestSize.Level1)
{
    ConvertBandNotAllow24GFail();
}

HWTEST_F(ScanServiceTest, ConvertBandNotAllow5GSuccess, TestSize.Level1)
{
    ConvertBandNotAllow5GSuccess();
}

HWTEST_F(ScanServiceTest, ConvertBandNotAllow5GFail, TestSize.Level1)
{
    ConvertBandNotAllow5GFail();
}

HWTEST_F(ScanServiceTest, Delete24GhzFreqsSuccess, TestSize.Level1)
{
    Delete24GhzFreqsSuccess();
}

HWTEST_F(ScanServiceTest, Delete5GhzFreqsSuccess, TestSize.Level1)
{
    Delete5GhzFreqsSuccess();
}

HWTEST_F(ScanServiceTest, GetSavedNetworkSsidListSuccess, TestSize.Level1)
{
    GetSavedNetworkSsidListSuccess();
}

HWTEST_F(ScanServiceTest, GetSavedNetworkSsidListFail, TestSize.Level1)
{
    GetSavedNetworkSsidListFail();
}

HWTEST_F(ScanServiceTest, GetHiddenNetworkSsidListSuccess, TestSize.Level1)
{
    GetHiddenNetworkSsidListSuccess1();
}

HWTEST_F(ScanServiceTest, GetHiddenNetworkSsidListSuccess2, TestSize.Level1)
{
    GetHiddenNetworkSsidListSuccess2();
}

HWTEST_F(ScanServiceTest, GetHiddenNetworkSsidListFail, TestSize.Level1)
{
    GetHiddenNetworkSsidListFail();
}

HWTEST_F(ScanServiceTest, ClearScanControlValueSuccess, TestSize.Level1)
{
    ClearScanControlValueSuccess();
}

HWTEST_F(ScanServiceTest, SetStaCurrentTimeSuccess, TestSize.Level1)
{
    SetStaCurrentTimeSuccess();
}

HWTEST_F(ScanServiceTest, AllowScanDuringScanningSuccess, TestSize.Level1)
{
    AllowScanDuringScanningSuccess();
}

HWTEST_F(ScanServiceTest, AllowScanDuringScanningFail, TestSize.Level1)
{
    AllowScanDuringScanningFail();
}

HWTEST_F(ScanServiceTest, AllowScanDuringScreenOffSuccess, TestSize.Level1)
{
    AllowScanDuringScreenOffSuccess();
}

HWTEST_F(ScanServiceTest, AllowScanDuringScreenOffFail1, TestSize.Level1)
{
    AllowScanDuringScreenOffFail1();
}

HWTEST_F(ScanServiceTest, AllowScanDuringStaSceneSuccess, TestSize.Level1)
{
    AllowScanDuringStaSceneSuccess1();
}

HWTEST_F(ScanServiceTest, AllowScanDuringStaSceneFail1, TestSize.Level1)
{
    AllowScanDuringStaSceneFail1();
}

HWTEST_F(ScanServiceTest, AllowScanDuringStaSceneFail2, TestSize.Level1)
{
    AllowScanDuringStaSceneFail2();
}

HWTEST_F(ScanServiceTest, AllowScanDuringStaSceneFail3, TestSize.Level1)
{
    AllowScanDuringStaSceneFail3();
}

HWTEST_F(ScanServiceTest, AllowScanDuringCustomSceneSuccess, TestSize.Level1)
{
    AllowScanDuringCustomSceneSuccess();
}

HWTEST_F(ScanServiceTest, AllowScanDuringCustomSceneFail1, TestSize.Level1)
{
    AllowScanDuringCustomSceneFail1();
}

HWTEST_F(ScanServiceTest, AllowScanDuringCustomSceneFail2, TestSize.Level1)
{
    AllowScanDuringCustomSceneFail2();
}

HWTEST_F(ScanServiceTest, AllowScanDuringCustomSceneFail3, TestSize.Level1)
{
    AllowScanDuringCustomSceneFail3();
}

HWTEST_F(ScanServiceTest, AllowExternScanByIntervalMode, TestSize.Level1)
{
    AllowExternScanByIntervalMode();
}

HWTEST_F(ScanServiceTest, AllowExternScanByCustomSceneSuccess, TestSize.Level1)
{
    AllowExternScanByCustomSceneSuccess();
}

HWTEST_F(ScanServiceTest, PnoScanByIntervalSuccess1, TestSize.Level1)
{
    PnoScanByIntervalSuccess1();
}

HWTEST_F(ScanServiceTest, PnoScanByIntervalSuccess2, TestSize.Level1)
{
    PnoScanByIntervalSuccess2();
}

HWTEST_F(ScanServiceTest, PnoScanByIntervalSuccess3, TestSize.Level1)
{
    PnoScanByIntervalSuccess3();
}

HWTEST_F(ScanServiceTest, PnoScanByIntervalFail1, TestSize.Level1)
{
    PnoScanByIntervalFail1();
}

HWTEST_F(ScanServiceTest, SystemScanByIntervalSuccess, TestSize.Level1)
{
    SystemScanByIntervalSuccess();
}

HWTEST_F(ScanServiceTest, ExternScanByIntervalSuccess1, TestSize.Level1)
{
    ExternScanByIntervalSuccess1();
}

HWTEST_F(ScanServiceTest, ExternScanByIntervalSuccess2, TestSize.Level1)
{
    ExternScanByIntervalSuccess2();
}

HWTEST_F(ScanServiceTest, ExternScanByIntervalSuccess3, TestSize.Level1)
{
    ExternScanByIntervalSuccess3();
}

HWTEST_F(ScanServiceTest, ExternScanByIntervalSuccess4, TestSize.Level1)
{
    ExternScanByIntervalSuccess4();
}

HWTEST_F(ScanServiceTest, ExternScanByIntervalSuccess5, TestSize.Level1)
{
    ExternScanByIntervalSuccess5();
}

HWTEST_F(ScanServiceTest, AllowSingleAppScanByIntervalSuccess1, TestSize.Level1)
{
    AllowSingleAppScanByIntervalSuccess1();
}

HWTEST_F(ScanServiceTest, AllowSingleAppScanByIntervalSuccess2, TestSize.Level1)
{
    AllowSingleAppScanByIntervalSuccess2();
}

HWTEST_F(ScanServiceTest, AllowSingleAppScanByIntervalFail1, TestSize.Level1)
{
    AllowSingleAppScanByIntervalFail1();
}

HWTEST_F(ScanServiceTest, AllowFullAppScanByIntervalSuccess1, TestSize.Level1)
{
    AllowFullAppScanByIntervalSuccess1();
}

HWTEST_F(ScanServiceTest, AllowFullAppScanByIntervalSuccess2, TestSize.Level1)
{
    AllowFullAppScanByIntervalSuccess2();
}

HWTEST_F(ScanServiceTest, AllowFullAppScanByIntervalFail1, TestSize.Level1)
{
    AllowFullAppScanByIntervalFail1();
}

HWTEST_F(ScanServiceTest, AllowScanByIntervalFixedSuccess1, TestSize.Level1)
{
    AllowScanByIntervalFixedSuccess1();
}

HWTEST_F(ScanServiceTest, AllowScanByIntervalFixedSuccess2, TestSize.Level1)
{
    AllowScanByIntervalFixedSuccess2();
}

HWTEST_F(ScanServiceTest, AllowScanByIntervalFixedSuccess3, TestSize.Level1)
{
    AllowScanByIntervalFixedSuccess3();
}

HWTEST_F(ScanServiceTest, AllowScanByIntervalFixedFail1, TestSize.Level1)
{
    AllowScanByIntervalFixedFail1();
}

HWTEST_F(ScanServiceTest, AllowScanByIntervalExpSuccess, TestSize.Level1)
{
    AllowScanByIntervalExpSuccess();
}

HWTEST_F(ScanServiceTest, AllowScanByIntervalContinueSuccess1, TestSize.Level1)
{
    AllowScanByIntervalContinueSuccess1();
}

HWTEST_F(ScanServiceTest, AllowScanByIntervalContinueSuccess2, TestSize.Level1)
{
    AllowScanByIntervalContinueSuccess2();
}

HWTEST_F(ScanServiceTest, AllowScanByIntervalContinueSuccess3, TestSize.Level1)
{
    AllowScanByIntervalContinueSuccess3();
}

HWTEST_F(ScanServiceTest, AllowScanByIntervalContinueFail1, TestSize.Level1)
{
    AllowScanByIntervalContinueFail1();
}

HWTEST_F(ScanServiceTest, AllowScanByIntervalBlocklistSuccess1, TestSize.Level1)
{
    AllowScanByIntervalBlocklistSuccess1();
}

HWTEST_F(ScanServiceTest, AllowScanByIntervalBlocklistSuccess2, TestSize.Level1)
{
    AllowScanByIntervalBlocklistSuccess2();
}

HWTEST_F(ScanServiceTest, AllowScanByIntervalBlocklistSuccess3, TestSize.Level1)
{
    AllowScanByIntervalBlocklistSuccess3();
}

HWTEST_F(ScanServiceTest, AllowScanByIntervalBlocklistFail1, TestSize.Level1)
{
    AllowScanByIntervalBlocklistFail1();
}

HWTEST_F(ScanServiceTest, AllowScanByIntervalBlocklistFail2, TestSize.Level1)
{
    AllowScanByIntervalBlocklistFail2();
}
} // namespace Wifi
} // namespace OHOS