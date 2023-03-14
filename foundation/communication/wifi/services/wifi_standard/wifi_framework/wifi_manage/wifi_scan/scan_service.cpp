/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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
#include <cinttypes>
#include "wifi_logger.h"
#include "wifi_settings.h"
#include "wifi_sta_hal_interface.h"
#include "wifi_internal_msg.h"
#include "log_helper.h"
#include "wifi_global_func.h"

DEFINE_WIFILOG_SCAN_LABEL("ScanService");

namespace OHOS {
namespace Wifi {
ScanService::ScanService()
    : pScanStateMachine(nullptr),
      pScanMonitor(nullptr),
      scanStartedFlag(false),
      scanConfigStoreIndex(0),
      pnoScanStartTime(0),
      staStatus(static_cast<int>(OperateResState::DISCONNECT_DISCONNECTED)),
      isPnoScanBegined(false),
      autoNetworkSelection(false),
      lastSystemScanTime(0),
      pnoScanFailedNum(0),
      disableScanFlag(false),
      staCurrentTime(0),
      customCurrentTime(0),
      staSceneForbidCount(0),
      customSceneForbidCount(0),
      scanTrustMode(false),
      isAbsFreezeState(false),
      isAbsFreezeScaned(false),
      scanResultBackup(-1)
{}

ScanService::~ScanService()
{
    WIFI_LOGI("Enter ScanService::~ScanService.\n");

    if (pScanMonitor != nullptr) {
        delete pScanMonitor;
        pScanMonitor = nullptr;
    }

    if (pScanStateMachine != nullptr) {
        delete pScanStateMachine;
        pScanStateMachine = nullptr;
    }
}

bool ScanService::InitScanService(const IScanSerivceCallbacks &scanSerivceCallbacks)
{
    WIFI_LOGI("Enter ScanService::InitScanService.\n");

    mScanSerivceCallbacks = scanSerivceCallbacks;
    pScanStateMachine = new (std::nothrow) ScanStateMachine();
    if (pScanStateMachine == nullptr) {
        WIFI_LOGE("Alloc pScanStateMachine failed.\n");
        return false;
    }

    if (!pScanStateMachine->InitScanStateMachine()) {
        WIFI_LOGE("InitScanStateMachine failed.\n");
        return false;
    }

    if (!pScanStateMachine->EnrollScanStatusListener(
        std::bind(&ScanService::HandleScanStatusReport, this, std::placeholders::_1))) {
        WIFI_LOGE("ScanStateMachine_->EnrollScanStatusListener failed.\n");
        return false;
    }
    pScanMonitor = new (std::nothrow) ScanMonitor();
    if (pScanMonitor == nullptr) {
        WIFI_LOGE("Alloc pScanMonitor failed.\n");
        return false;
    }

    if (!pScanMonitor->InitScanMonitor()) {
        WIFI_LOGE("InitScanMonitor failed.\n");
        return false;
    }

    if ((WifiStaHalInterface::GetInstance().GetSupportFrequencies(SCAN_BAND_24_GHZ, freqs2G) != WIFI_IDL_OPT_OK) ||
        (WifiStaHalInterface::GetInstance().GetSupportFrequencies(SCAN_BAND_5_GHZ, freqs5G) != WIFI_IDL_OPT_OK) ||
        (WifiStaHalInterface::GetInstance().GetSupportFrequencies(SCAN_BAND_5_GHZ_DFS_ONLY, freqsDfs) !=
        WIFI_IDL_OPT_OK)) {
        WIFI_LOGE("GetSupportFrequencies failed.\n");
    }

    pScanMonitor->SetScanStateMachine(pScanStateMachine);
    pScanStateMachine->SendMessage(static_cast<int>(CMD_SCAN_PREPARE));
    GetScanControlInfo();

    return true;
}

void ScanService::UnInitScanService()
{
    WIFI_LOGI("Enter ScanService::UnInitScanService.\n");
    pScanStateMachine->StopTimer(static_cast<int>(SYSTEM_SCAN_TIMER));
    pScanStateMachine->StopTimer(static_cast<int>(DISCONNECTED_SCAN_TIMER));
    pScanStateMachine->StopTimer(static_cast<int>(RESTART_PNO_SCAN_TIMER));
    pScanStateMachine->SendMessage(static_cast<int>(CMD_SCAN_FINISH));
    scanStartedFlag = false;
    return;
}

void ScanService::RegisterScanCallbacks(const IScanSerivceCallbacks &iScanSerivceCallbacks)
{
    mScanSerivceCallbacks = iScanSerivceCallbacks;
}

void ScanService::HandleScanStatusReport(ScanStatusReport &scanStatusReport)
{
    WIFI_LOGI("Enter ScanService::HandleScanStatusReport.\n");

    switch (scanStatusReport.status) {
        case SCAN_STARTED_STATUS: {
            scanStartedFlag = true;
            /* Pno scan maybe has started, stop it first. */
            pScanStateMachine->SendMessage(CMD_STOP_PNO_SCAN);
            mScanSerivceCallbacks.OnScanStartEvent();
            SystemScanProcess(true);
            break;
        }
        case SCAN_FINISHED_STATUS: {
            mScanSerivceCallbacks.OnScanStopEvent();
            break;
        }
        case COMMON_SCAN_SUCCESS: {
            HandleCommonScanInfo(scanStatusReport.requestIndexList, scanStatusReport.scanInfoList);
            break;
        }
        case COMMON_SCAN_FAILED: {
            HandleCommonScanFailed(scanStatusReport.requestIndexList);
            break;
        }
        case PNO_SCAN_INFO: {
            pnoScanFailedNum = 0;
            HandlePnoScanInfo(scanStatusReport.scanInfoList);
            break;
        }
        case PNO_SCAN_FAILED: {
            /* Start the timer and restart the PNO scanning after a delay. */
            pScanStateMachine->StartTimer(static_cast<int>(RESTART_PNO_SCAN_TIMER), RESTART_PNO_SCAN_TIME);
            EndPnoScan();
            break;
        }
        case SCAN_INNER_EVENT: {
            HandleInnerEventReport(scanStatusReport.innerEvent);
            break;
        }
        default: {
            WIFI_LOGI("HandleStatusReport: status is error.\n");
            break;
        }
    }
    return;
}

void ScanService::HandleInnerEventReport(ScanInnerEventType innerEvent)
{
    WIFI_LOGI("Enter ScanService::HandleInnerEventReport.\n");

    switch (innerEvent) {
        case SYSTEM_SCAN_TIMER: {
            HandleSystemScanTimeout();
            break;
        }
        case DISCONNECTED_SCAN_TIMER: {
            HandleDisconnectedScanTimeout();
            break;
        }
        case RESTART_PNO_SCAN_TIMER: {
            RestartPnoScanTimeOut();
            break;
        }
        default: {
            break;
        }
    }
}

ErrCode ScanService::Scan(bool externFlag)
{
    WIFI_LOGI("Enter ScanService::Scan.\n");
    if (!scanStartedFlag) {
        WIFI_LOGE("Scan service has not started.\n");
        return WIFI_OPT_FAILED;
    }

    if (externFlag) {
        ErrCode rlt = ApplyScanPolices(ScanType::SCAN_TYPE_EXTERN);
        if (rlt != WIFI_OPT_SUCCESS) {
            return rlt;
        }
    }

    ScanConfig scanConfig;
    /*
     * Invoke the interface provided by the configuration center to obtain the
     * hidden network list.
     */
    if (!GetHiddenNetworkSsidList(scanConfig.hiddenNetworkSsid)) {
        WIFI_LOGE("GetHiddenNetworkSsidList failed.\n");
    }

    scanConfig.scanBand = SCAN_BAND_BOTH_WITH_DFS;
    scanConfig.fullScanFlag = true;
    scanConfig.externFlag = externFlag;
    scanConfig.scanStyle = SCAN_TYPE_HIGH_ACCURACY;
    if (!SingleScan(scanConfig)) {
        WIFI_LOGE("SingleScan failed.\n");
        return WIFI_OPT_FAILED;
    }

    return WIFI_OPT_SUCCESS;
}

ErrCode ScanService::ScanWithParam(const WifiScanParams &params)
{
    WIFI_LOGI("Enter ScanService::Scan.\n");

    if (!scanStartedFlag) {
        WIFI_LOGE("Scan service has not started.\n");
        return WIFI_OPT_FAILED;
    }

    ErrCode rlt = ApplyScanPolices(ScanType::SCAN_TYPE_EXTERN);
    if (rlt != WIFI_OPT_SUCCESS) {
        return rlt;
    }

    if ((params.band < static_cast<int>(SCAN_BAND_UNSPECIFIED)) ||
        (params.band > static_cast<int>(SCAN_BAND_BOTH_WITH_DFS))) {
        WIFI_LOGE("params.band is error.\n");
        return WIFI_OPT_FAILED;
    }

    /* When the frequency is specified, the band must be SCAN_BAND_UNSPECIFIED. */
    if (params.freqs.empty() && (params.band == static_cast<int>(SCAN_BAND_UNSPECIFIED))) {
        WIFI_LOGE("params is error.\n");
        return WIFI_OPT_FAILED;
    }

    ScanConfig scanConfig;
    if (params.ssid.empty() && params.bssid.empty() && (params.band == static_cast<int>(SCAN_BAND_BOTH_WITH_DFS))) {
        scanConfig.fullScanFlag = true;
    }

    if (!params.ssid.empty()) {
        scanConfig.hiddenNetworkSsid.push_back(params.ssid);
    } else {
        /*
         * Invoke the interface provided by the configuration center to obtain the
         * hidden network list.
         */
        if (!GetHiddenNetworkSsidList(scanConfig.hiddenNetworkSsid)) {
            WIFI_LOGE("GetHiddenNetworkSsidList failed.\n");
        }
    }

    scanConfig.scanBand = static_cast<ScanBandType>(params.band);
    scanConfig.scanFreqs.assign(params.freqs.begin(), params.freqs.end());
    scanConfig.ssid = params.ssid;
    scanConfig.bssid = params.bssid;
    scanConfig.externFlag = true;
    scanConfig.scanningWithParamFlag = true;
    scanConfig.scanStyle = SCAN_TYPE_HIGH_ACCURACY;

    if (!SingleScan(scanConfig)) {
        WIFI_LOGE("SingleScan failed.\n");
        return WIFI_OPT_FAILED;
    }

    return WIFI_OPT_SUCCESS;
}

ErrCode ScanService::DisableScan(bool disable)
{
    LOGI("Enter ScanService::DisableScan");
    std::unique_lock<std::mutex> lock(scanControlInfoMutex);
    disableScanFlag = disable;
    return WIFI_OPT_SUCCESS;
}

bool ScanService::SingleScan(ScanConfig &scanConfig)
{
    WIFI_LOGI("Enter ScanService::SingleScan.\n");

    GetAllowBandFreqsControlInfo(scanConfig.scanBand, scanConfig.scanFreqs);
    if ((scanConfig.scanBand == SCAN_BAND_UNSPECIFIED) && (scanConfig.scanFreqs.empty())) {
        WIFI_LOGE("Have no allowed band or freq.\n");
        return false;
    }

    InterScanConfig interConfig;
    interConfig.fullScanFlag = scanConfig.fullScanFlag;
    interConfig.hiddenNetworkSsid.assign(scanConfig.hiddenNetworkSsid.begin(), scanConfig.hiddenNetworkSsid.end());
    interConfig.scanStyle = scanConfig.scanStyle;

    /* Specified frequency */
    if (scanConfig.scanBand == SCAN_BAND_UNSPECIFIED) {
        interConfig.scanFreqs.assign(scanConfig.scanFreqs.begin(), scanConfig.scanFreqs.end());
        /*
         * When band is SCAN_BAND_BOTH_WITH_DFS, need to scan all frequency,
         * scanFreqs can be empty.
         */
    } else if (scanConfig.scanBand != SCAN_BAND_BOTH_WITH_DFS) {
        /* Converting frequency bands to frequencies. */
        if (!GetBandFreqs(scanConfig.scanBand, interConfig.scanFreqs)) {
            WIFI_LOGE("GetBandFreqs failed.\n");
            return false;
        }
    }

    /* Save the configuration. */
    int requestIndex = StoreRequestScanConfig(scanConfig, interConfig);
    if (requestIndex == MAX_SCAN_CONFIG_STORE_INDEX) {
        WIFI_LOGE("StoreRequestScanConfig failed.\n");
        return false;
    }

    std::unique_lock<std::mutex> lock(scanConfigMapMutex);
    /* Construct a message. */
    InternalMessage *interMessage =
        pScanStateMachine->CreateMessage(static_cast<int>(CMD_START_COMMON_SCAN), requestIndex);
    if (interMessage == nullptr) {
        scanConfigMap.erase(requestIndex);
        WIFI_LOGE("CreateMessage failed.\n");
        return false;
    }

    if (!AddScanMessageBody(interMessage, interConfig)) {
        scanConfigMap.erase(requestIndex);
        MessageManage::GetInstance().ReclaimMsg(interMessage);
        WIFI_LOGE("AddScanMessageBody failed.\n");
        return false;
    }
    pScanStateMachine->SendMessage(interMessage);

    return true;
}

bool ScanService::GetBandFreqs(ScanBandType band, std::vector<int> &freqs)
{
    WIFI_LOGI("Enter ScanService::GetBandFreqs.\n");

    switch (band) {
        case SCAN_BAND_24_GHZ: {
            freqs.assign(freqs2G.begin(), freqs2G.end());
            return true;
        }

        case SCAN_BAND_5_GHZ: {
            freqs.assign(freqs5G.begin(), freqs5G.end());
            return true;
        }

        case SCAN_BAND_BOTH: {
            freqs.insert(freqs.end(), freqs2G.begin(), freqs2G.end());
            freqs.insert(freqs.end(), freqs5G.begin(), freqs5G.end());
            return true;
        }

        case SCAN_BAND_5_GHZ_DFS_ONLY: {
            freqs.assign(freqsDfs.begin(), freqsDfs.end());
            return true;
        }

        case SCAN_BAND_5_GHZ_WITH_DFS: {
            freqs.insert(freqs.end(), freqs5G.begin(), freqs5G.end());
            freqs.insert(freqs.end(), freqsDfs.begin(), freqsDfs.end());
            return true;
        }

        case SCAN_BAND_BOTH_WITH_DFS: {
            freqs.insert(freqs.end(), freqs2G.begin(), freqs2G.end());
            freqs.insert(freqs.end(), freqs5G.begin(), freqs5G.end());
            freqs.insert(freqs.end(), freqsDfs.begin(), freqsDfs.end());
            return true;
        }

        default:
            WIFI_LOGE("bandType(%{public}d) is error.\n", band);
            return false;
    }
}

bool ScanService::AddScanMessageBody(InternalMessage *interMessage, const InterScanConfig &interConfig)
{
    WIFI_LOGI("Enter ScanService::AddScanMessageBody.\n");

    if (interMessage == nullptr) {
        WIFI_LOGE("interMessage is null.\n");
        return false;
    }

    interMessage->AddIntMessageBody(interConfig.hiddenNetworkSsid.size());
    std::vector<std::string>::const_iterator iter = interConfig.hiddenNetworkSsid.begin();
    for (; iter != interConfig.hiddenNetworkSsid.end(); ++iter) {
        interMessage->AddStringMessageBody(*iter);
    }

    interMessage->AddIntMessageBody(interConfig.scanFreqs.size());
    std::vector<int>::const_iterator iterFreq = interConfig.scanFreqs.begin();
    for (; iterFreq != interConfig.scanFreqs.end(); ++iterFreq) {
        interMessage->AddIntMessageBody(*iterFreq);
    }

    interMessage->AddIntMessageBody(static_cast<int>(interConfig.fullScanFlag));
    interMessage->AddIntMessageBody(interConfig.backScanPeriod);
    interMessage->AddIntMessageBody(interConfig.bssidsNumPerScan);
    interMessage->AddIntMessageBody(interConfig.maxScansCache);
    interMessage->AddIntMessageBody(interConfig.maxBackScanPeriod);
    interMessage->AddIntMessageBody(interConfig.scanStyle);

    return true;
}

int ScanService::StoreRequestScanConfig(const ScanConfig &scanConfig, const InterScanConfig &interConfig)
{
    WIFI_LOGI("Enter ScanService::StoreRequestScanConfig.\n");

    int i = 0;
    for (i = 0; i < MAX_SCAN_CONFIG_STORE_INDEX; i++) {
        scanConfigStoreIndex++;
        if (scanConfigStoreIndex >= MAX_SCAN_CONFIG_STORE_INDEX) {
            scanConfigStoreIndex = 0;
        }

        ScanConfigMap::iterator iter = scanConfigMap.find(scanConfigStoreIndex);
        if (iter == scanConfigMap.end()) {
            break;
        }
    }

    if (i == MAX_SCAN_CONFIG_STORE_INDEX) {
        return MAX_SCAN_CONFIG_STORE_INDEX;
    }

    StoreScanConfig storeScanConfig;
    storeScanConfig.ssid = scanConfig.ssid;
    storeScanConfig.bssid = scanConfig.bssid;
    storeScanConfig.scanFreqs.assign(interConfig.scanFreqs.begin(), interConfig.scanFreqs.end());

    struct timespec times = {0, 0};
    clock_gettime(CLOCK_MONOTONIC, &times);
    storeScanConfig.scanTime =
        static_cast<int64_t>(times.tv_sec) * SECOND_TO_MICRO_SECOND + times.tv_nsec / SECOND_TO_MILLI_SECOND;
    storeScanConfig.fullScanFlag = scanConfig.fullScanFlag;
    storeScanConfig.externFlag = scanConfig.externFlag;
    storeScanConfig.scanningWithParamFlag = scanConfig.scanningWithParamFlag;

    std::unique_lock<std::mutex> lock(scanConfigMapMutex);
    scanConfigMap.insert(std::pair<int, StoreScanConfig>(scanConfigStoreIndex, storeScanConfig));

    return scanConfigStoreIndex;
}

void ScanService::HandleCommonScanFailed(std::vector<int> &requestIndexList)
{
    WIFI_LOGI("Enter ScanService::HandleCommonScanFailed.\n");

    if (staStatus != static_cast<int>(OperateResState::DISCONNECT_DISCONNECTED) &&
        staStatus != static_cast<int>(OperateResState::CONNECT_AP_CONNECTED)) {
        return;
    }

    std::unique_lock<std::mutex> lock(scanConfigMapMutex);
    for (std::vector<int>::iterator reqIter = requestIndexList.begin(); reqIter != requestIndexList.end(); ++reqIter) {
        ScanConfigMap::iterator configIter = scanConfigMap.find(*reqIter);
        /* No configuration found. */
        if (configIter == scanConfigMap.end()) {
            continue;
        }

        /* Notification of the end of scanning. */
        mScanSerivceCallbacks.OnScanFinishEvent(static_cast<int>(ScanHandleNotify::SCAN_FAIL));
        scanResultBackup = static_cast<int>(ScanHandleNotify::SCAN_FAIL);

        scanConfigMap.erase(*reqIter);
    }

    return;
}

void ScanService::HandleCommonScanInfo(
    std::vector<int> &requestIndexList, std::vector<InterScanInfo> &scanInfoList)
{
    WIFI_LOGI("Enter ScanService::HandleCommonScanInfo.\n");

    bool fullScanStored = false;
    {
        std::unique_lock<std::mutex> lock(scanConfigMapMutex);
        for (std::vector<int>::iterator reqIter = requestIndexList.begin(); reqIter != requestIndexList.end();
            ++reqIter) {
            ScanConfigMap::iterator configIter = scanConfigMap.find(*reqIter);
            /* No configuration found. */
            if (configIter == scanConfigMap.end()) {
                continue;
            }

            /* Full Scan Info. */
            if (configIter->second.fullScanFlag) {
                if (fullScanStored) {
                    scanConfigMap.erase(*reqIter);
                    continue;
                }

                if (StoreFullScanInfo(configIter->second, scanInfoList)) {
                    fullScanStored = true;
                    mScanSerivceCallbacks.OnScanFinishEvent(static_cast<int>(ScanHandleNotify::SCAN_OK));
                    scanResultBackup = static_cast<int>(ScanHandleNotify::SCAN_OK);
                } else {
                    WIFI_LOGE("StoreFullScanInfo failed.\n");
                }
                /* Specify Scan Info. */
            } else {
                if (!StoreUserScanInfo(configIter->second, scanInfoList)) {
                    WIFI_LOGE("StoreUserScanInfo failed.\n");
                }
                mScanSerivceCallbacks.OnScanFinishEvent(static_cast<int>(ScanHandleNotify::SCAN_OK));
                scanResultBackup = static_cast<int>(ScanHandleNotify::SCAN_OK);
            }

            scanConfigMap.erase(*reqIter);
        }
    }

    /* Send the scanning result to the module registered for listening. */
    ScanInfoHandlerMap::iterator handleIter = scanInfoHandlerMap.begin();
    for (; handleIter != scanInfoHandlerMap.end(); ++handleIter) {
        if (handleIter->second) {
            handleIter->second(scanInfoList);
        }
    }

    /* Send the result to the interface service. */
    ReportScanInfos(scanInfoList);

    return;
}

bool ScanService::StoreFullScanInfo(
    const StoreScanConfig &scanConfig, const std::vector<InterScanInfo> &scanInfoList)
{
    WIFI_LOGI("Enter ScanService::StoreFullScanInfo.\n");

    /* Filtering result. */
    WIFI_LOGI("scanConfig.scanTime is %" PRIu64 ".\n", scanConfig.scanTime);
    WIFI_LOGI("Receive %{public}d scan results.\n", (int)(scanInfoList.size()));

    std::vector<WifiScanInfo> storeInfoList;
    for (auto iter = scanInfoList.begin(); iter != scanInfoList.end(); ++iter) {
        WifiScanInfo scanInfo;
        scanInfo.bssid = iter->bssid;
        scanInfo.ssid = iter->ssid;
        scanInfo.capabilities = iter->capabilities;
        scanInfo.frequency = iter->frequency;
        scanInfo.channelWidth = iter->channelWidth;
        scanInfo.centerFrequency0 = iter->centerFrequency0;
        scanInfo.centerFrequency1 = iter->centerFrequency1;
        scanInfo.rssi = iter->rssi;
        scanInfo.securityType = iter->securityType;
        scanInfo.infoElems = iter->infoElems;
        scanInfo.features = iter->features;
        scanInfo.timestamp = iter->timestamp;
        scanInfo.band = iter->band;
        storeInfoList.push_back(scanInfo);
    }

    if (WifiSettings::GetInstance().SaveScanInfoList(storeInfoList) != 0) {
        WIFI_LOGE("WifiSettings::GetInstance().SaveScanInfoList failed.\n");
        return false;
    }

    return true;
}

bool ScanService::StoreUserScanInfo(const StoreScanConfig &scanConfig, std::vector<InterScanInfo> &scanInfoList)
{
    WIFI_LOGI("Enter ScanService::StoreUserScanInfo.\n");

    std::vector<InterScanInfo>::const_iterator iter = scanInfoList.begin();
    for (; iter != scanInfoList.end(); ++iter) {
        /* Timestamp filtering */
        if ((iter->timestamp) <= scanConfig.scanTime) {
            continue;
        }

        /* frequency filtering. */
        if (!scanConfig.scanFreqs.empty()) {
            if (std::find(scanConfig.scanFreqs.begin(), scanConfig.scanFreqs.end(), iter->frequency) ==
                scanConfig.scanFreqs.end()) {
                continue;
            }
        }

        /* SSID filtering. */
        if ((!scanConfig.ssid.empty()) && (scanConfig.ssid != iter->ssid)) {
            continue;
        }

        /* BSSID filtering. */
        if ((!scanConfig.bssid.empty()) && (scanConfig.bssid != iter->bssid)) {
            continue;
        }
    }

    /*
     * The specified parameter scanning is initiated by the system and
     * store in the configuration center.
     */
    ReportStoreScanInfos(scanInfoList);

    return true;
}

void ScanService::ReportScanInfos(std::vector<InterScanInfo> &interScanList)
{
    WIFI_LOGI("Enter ScanService::ReportScanInfos.\n");

    mScanSerivceCallbacks.OnScanInfoEvent(interScanList);
    return;
}

void ScanService::ReportStoreScanInfos(std::vector<InterScanInfo> &interScanList)
{
    WIFI_LOGI("Enter ScanService::ReportStoreScanInfos.\n");

    mScanSerivceCallbacks.OnStoreScanInfoEvent(interScanList);
    return;
}

bool ScanService::BeginPnoScan()
{
    WIFI_LOGI("Enter ScanService::BeginPnoScan.\n");

    if (isPnoScanBegined) {
        WIFI_LOGI("PNO scan has started.\n");
        return false;
    }

    ErrCode rlt = ApplyScanPolices(ScanType::SCAN_TYPE_PNO);
    if (rlt != WIFI_OPT_SUCCESS) {
        return false;
    }

    PnoScanConfig pnoScanConfig;
    /* Obtain the network list from the configuration center. */
    if (!GetSavedNetworkSsidList(pnoScanConfig.savedNetworkSsid)) {
        WIFI_LOGE("GetSavedNetworkSsidList failed.\n");
        return false;
    }
    if (pnoScanConfig.savedNetworkSsid.size() == 0) {
        WIFI_LOGE("Have no saved network, not need to start PNO scan.\n");
        return false;
    }
    if (!GetHiddenNetworkSsidList(pnoScanConfig.hiddenNetworkSsid)) {
        WIFI_LOGE("GetHiddenNetworkSsidList failed.\n");
        return false;
    }

    pnoScanConfig.scanInterval = MIN_SYSTEM_SCAN_INTERVAL;
    /* Querying a Scan Policy */
    if (pnoScanIntervalMode.scanIntervalMode.interval > 0) {
        pnoScanConfig.scanInterval = pnoScanIntervalMode.scanIntervalMode.interval;
    }

    pnoScanConfig.minRssi2Dot4Ghz = WifiSettings::GetInstance().GetMinRssi2Dot4Ghz();
    pnoScanConfig.minRssi5Ghz = WifiSettings::GetInstance().GetMinRssi5Ghz();

    InterScanConfig interConfig;
    interConfig.fullScanFlag = true;
    if (!GetBandFreqs(SCAN_BAND_BOTH_WITH_DFS, interConfig.scanFreqs)) {
        WIFI_LOGE("GetBandFreqs failed.\n");
        return false;
    }

    if (!PnoScan(pnoScanConfig, interConfig)) {
        WIFI_LOGE("PnoScan failed.\n");
        return false;
    }
    isPnoScanBegined = true;

    return true;
}

bool ScanService::PnoScan(const PnoScanConfig &pnoScanConfig, const InterScanConfig &interScanConfig)
{
    WIFI_LOGI("Enter ScanService::PnoScan.\n");

    /* Construct a message. */
    InternalMessage *interMessage = pScanStateMachine->CreateMessage(CMD_START_PNO_SCAN);
    if (interMessage == nullptr) {
        WIFI_LOGE("CreateMessage failed.\n");
        return false;
    }

    if (!AddPnoScanMessageBody(interMessage, pnoScanConfig)) {
        MessageManage::GetInstance().ReclaimMsg(interMessage);
        WIFI_LOGE("AddPnoScanMessageBody failed.\n");
        return false;
    }

    if (!AddScanMessageBody(interMessage, interScanConfig)) {
        MessageManage::GetInstance().ReclaimMsg(interMessage);
        WIFI_LOGE("AddScanMessageBody failed.\n");
        return false;
    }

    WIFI_LOGI("Begin: send message.");
    pScanStateMachine->SendMessage(interMessage);
    WIFI_LOGI("End: send message.");

    struct timespec times = {0, 0};
    clock_gettime(CLOCK_MONOTONIC, &times);
    pnoScanStartTime =
        static_cast<int64_t>(times.tv_sec) * SECOND_TO_MILLI_SECOND + times.tv_nsec / SECOND_TO_MICRO_SECOND;

    return true;
}

bool ScanService::AddPnoScanMessageBody(InternalMessage *interMessage, const PnoScanConfig &pnoScanConfig)
{
    WIFI_LOGI("Enter ScanService::AddPnoScanMessageBody.\n");

    if (interMessage == nullptr) {
        WIFI_LOGE("interMessage is null.\n");
        return false;
    }

    interMessage->AddIntMessageBody(pnoScanConfig.scanInterval);
    interMessage->AddIntMessageBody(pnoScanConfig.minRssi2Dot4Ghz);
    interMessage->AddIntMessageBody(pnoScanConfig.minRssi5Ghz);

    interMessage->AddIntMessageBody(pnoScanConfig.hiddenNetworkSsid.size());
    auto iter = pnoScanConfig.hiddenNetworkSsid.begin();
    for (; iter != pnoScanConfig.hiddenNetworkSsid.end(); ++iter) {
        interMessage->AddStringMessageBody(*iter);
    }

    interMessage->AddIntMessageBody(pnoScanConfig.savedNetworkSsid.size());
    auto iter2 = pnoScanConfig.savedNetworkSsid.begin();
    for (; iter2 != pnoScanConfig.savedNetworkSsid.end(); ++iter2) {
        interMessage->AddStringMessageBody(*iter2);
    }

    interMessage->AddIntMessageBody(pnoScanConfig.freqs.size());
    auto iter3 = pnoScanConfig.freqs.begin();
    for (; iter3 != pnoScanConfig.freqs.end(); ++iter3) {
        interMessage->AddIntMessageBody(*iter3);
    }

    return true;
}

void ScanService::HandlePnoScanInfo(std::vector<InterScanInfo> &scanInfoList)
{
    WIFI_LOGI("Enter ScanService::HandlePnoScanInfo.\n");

    std::vector<InterScanInfo> filterScanInfo;
    std::vector<InterScanInfo>::iterator iter = scanInfoList.begin();
    for (; iter != scanInfoList.end(); ++iter) {
        if ((iter->timestamp / SECOND_TO_MILLI_SECOND) > pnoScanStartTime) {
            filterScanInfo.push_back(*iter);
            WIFI_LOGI("InterScanInfo.bssid is %{private}s.\n", iter->bssid.c_str());
            WIFI_LOGI("InterScanInfo.ssid is %{public}s.\n", iter->ssid.c_str());
            WIFI_LOGI("InterScanInfo.capabilities is %{public}s.\n", iter->capabilities.c_str());
            WIFI_LOGI("InterScanInfo.frequency is %{public}d.\n", iter->frequency);
            WIFI_LOGI("InterScanInfo.rssi is %{public}d.\n", iter->rssi);
            WIFI_LOGI("InterScanInfo.timestamp is %" PRIu64 ".\n", iter->timestamp);
        }
    }

    /* Send the scanning result to the module registered for listening. */
    PnoScanInfoHandlerMap::iterator handleIter = pnoScanInfoHandlerMap.begin();
    for (; handleIter != pnoScanInfoHandlerMap.end(); ++handleIter) {
        if (handleIter->second) {
            handleIter->second(filterScanInfo);
        }
    }

    /* send message to main service. */
    ReportScanInfos(filterScanInfo);

    return;
}

void ScanService::EndPnoScan()
{
    WIFI_LOGI("Enter ScanService::EndPnoScan.\n");

    if (!isPnoScanBegined) {
        return;
    }

    pScanStateMachine->SendMessage(CMD_STOP_PNO_SCAN);
    isPnoScanBegined = false;
    return;
}

void ScanService::HandleScreenStatusChanged()
{
    WIFI_LOGI("Enter ScanService::HandleScreenStatusChanged.");
    SystemScanProcess(false);
    return;
}

void ScanService::HandleStaStatusChanged(int status)
{
    WIFI_LOGI("Enter ScanService::HandleStaStatusChanged.");

    staStatus = status;
    switch (staStatus) {
        case static_cast<int>(OperateResState::DISCONNECT_DISCONNECTED): {
            DisconnectedTimerScan();
            SystemScanProcess(true);
            break;
        }
        case static_cast<int>(OperateResState::CONNECT_AP_CONNECTED): {
            SystemScanProcess(false);
            break;
        }
        default: {
            StopSystemScan();
        }
    }

    staSceneForbidCount = 0;
    return;
}

void ScanService::HandleMovingFreezeChanged()
{
    LOGI("Enter ScanService::HandleMovingFreezeChanged.");
    ScanMode appRunMode = WifiSettings::GetInstance().GetAppRunningState();
    int freezeState = WifiSettings::GetInstance().GetFreezeModeState();
    int noChargerPlugModeState = WifiSettings::GetInstance().GetNoChargerPlugModeState();

    bool movingFreeze = (appRunMode == ScanMode::APP_BACKGROUND_SCAN || appRunMode == ScanMode::SYS_BACKGROUND_SCAN) &&
        (freezeState == MODE_STATE_OPEN) && (noChargerPlugModeState == MODE_STATE_OPEN);
    bool movingFreezeBakup = IsMovingFreezeState();

    SetMovingFreezeState(movingFreeze);
    WIFI_LOGD("moving freeze changed: movingFreeze=%{public}d, movingFreezeBakup=%{public}d", movingFreeze,
        movingFreezeBakup);
    /* Moving -> Freeze, set the scanned flag to false. */
    if (!movingFreezeBakup && movingFreeze) {
        WIFI_LOGD("set movingFreeze scanned false.");
        SetMovingFreezeScaned(false);
    }
}

void ScanService::HandleCustomStatusChanged(int customScene, int customSceneStatus)
{
    WIFI_LOGI("Enter ScanService::HandleCustomStatusChanged.");
    WIFI_LOGD("sizeof(time_t):%{public}d", int(sizeof(time_t)));

    time_t now = time(nullptr);
    WIFI_LOGD("customScene:%{public}d, status:%{public}d", customScene, customSceneStatus);
    if (customSceneStatus == MODE_STATE_OPEN) {
        customSceneTimeMap.insert(std::pair<int, int>(customScene, now));
    }
    if (customSceneStatus == MODE_STATE_CLOSE) {
        customSceneTimeMap.erase(customScene);
    }
    SystemScanProcess(false);
    customSceneForbidCount = 0;

    return;
}

void ScanService::HandleGetCustomSceneState(std::map<int, time_t>& sceneMap) const
{
    sceneMap = customSceneTimeMap;
}

void ScanService::SystemScanProcess(bool scanAtOnce)
{
    WIFI_LOGI("Enter ScanService::SystemScanProcess.");
    StopSystemScan();

    int state = WifiSettings::GetInstance().GetScreenState();
    if (state == MODE_STATE_OPEN) {
        WIFI_LOGD("Screen is on.");
    } else {
        WIFI_LOGD("ScreenOn is off.");
    }

    if (state == MODE_STATE_OPEN) {
        {
            std::unique_lock<std::mutex> lock(scanControlInfoMutex);
            for (auto iter = scanControlInfo.scanIntervalList.begin(); iter != scanControlInfo.scanIntervalList.end();
                ++iter) {
                if (iter->scanScene == SCAN_SCENE_ALL && iter->scanMode == ScanMode::SYSTEM_TIMER_SCAN &&
                    iter->isSingle == false) {
                    WIFI_LOGD("iter->intervalMode is:%{public}d", iter->intervalMode);
                    WIFI_LOGD("iter->interval is:%{public}d", iter->interval);
                    WIFI_LOGD("iter->count is:%{public}d", iter->count);
                    systemScanIntervalMode.scanIntervalMode.intervalMode = iter->intervalMode;
                    systemScanIntervalMode.scanIntervalMode.interval = iter->interval;
                    systemScanIntervalMode.scanIntervalMode.count = iter->count;
                    systemScanIntervalMode.expScanCount = 0;
                }
            }
        }
        StartSystemTimerScan(scanAtOnce);
    } else {
        if (!BeginPnoScan()) {
            WIFI_LOGE("BeginPnoScan failed.");
            return;
        }
    }

    return;
}

void ScanService::StopSystemScan()
{
    WIFI_LOGI("Enter ScanService::StopSystemScan.");

    pScanStateMachine->StopTimer(static_cast<int>(SYSTEM_SCAN_TIMER));
    EndPnoScan();
    pnoScanFailedNum = 0;
    pScanStateMachine->StopTimer(static_cast<int>(RESTART_PNO_SCAN_TIMER));
    return;
}

void ScanService::StartSystemTimerScan(bool scanAtOnce)
{
    WIFI_LOGI("Enter ScanService::StartSystemTimerScan.");

    ErrCode rlt = ApplyScanPolices(ScanType::SCAN_TYPE_SYSTEMTIMER);
    if (rlt != WIFI_OPT_SUCCESS) {
        return;
    }

    struct timespec times = { 0, 0 };
    clock_gettime(CLOCK_MONOTONIC, &times);
    int64_t nowTime =
        static_cast<int64_t>(times.tv_sec) * SECOND_TO_MILLI_SECOND + times.tv_nsec / SECOND_TO_MICRO_SECOND;
    int sinceLastScan = 0;
    if (lastSystemScanTime != 0) {
        sinceLastScan = nowTime - lastSystemScanTime;
    }

    /*
     * The scan is performed immediately, the first scan is required,
     * or the time since the last scan is longer than the scan interval.
     */
    int scanTime = SYSTEM_SCAN_INIT_TIME;
    WIFI_LOGD("interval:%{public}d", systemScanIntervalMode.scanIntervalMode.interval);
    if (systemScanIntervalMode.scanIntervalMode.interval > 0) {
        scanTime = systemScanIntervalMode.scanIntervalMode.interval;
    }
    if (scanAtOnce || (lastSystemScanTime == 0) ||
        (sinceLastScan >= systemScanIntervalMode.scanIntervalMode.interval)) {
        if (Scan(false) != WIFI_OPT_SUCCESS) {
            WIFI_LOGE("Scan failed.");
        }
        lastSystemScanTime = nowTime;
    } else {
        scanTime = systemScanIntervalMode.scanIntervalMode.interval - sinceLastScan;
    }
    WIFI_LOGD("scanTime: %{public}d,  interval:%{public}d,  count:%{public}d",
        scanTime,
        systemScanIntervalMode.scanIntervalMode.interval,
        systemScanIntervalMode.scanIntervalMode.count);
    pScanStateMachine->StartTimer(static_cast<int>(SYSTEM_SCAN_TIMER), scanTime * SECOND_TO_MILLI_SECOND);

    return;
}

void ScanService::HandleSystemScanTimeout()
{
    StartSystemTimerScan(true);
    return;
}

void ScanService::DisconnectedTimerScan()
{
    WIFI_LOGI("Enter ScanService::DisconnectedTimerScan.\n");

    pScanStateMachine->StartTimer(static_cast<int>(DISCONNECTED_SCAN_TIMER), DISCONNECTED_SCAN_INTERVAL);
    return;
}

void ScanService::HandleDisconnectedScanTimeout()
{
    WIFI_LOGI("Enter ScanService::HandleDisconnectedScanTimeout.\n");

    if (staStatus != static_cast<int>(OperateResState::DISCONNECT_DISCONNECTED)) {
        return;
    }

    if (Scan(false) != WIFI_OPT_SUCCESS) {
        WIFI_LOGE("Scan failed.");
    }
    pScanStateMachine->StartTimer(static_cast<int>(DISCONNECTED_SCAN_TIMER), DISCONNECTED_SCAN_INTERVAL);

    return;
}

void ScanService::RestartPnoScanTimeOut()
{
    WIFI_LOGI("Enter ScanService::RestartPnoScanTimeOut.\n");
    pnoScanFailedNum++;
    if (pnoScanFailedNum > MAX_PNO_SCAN_FAILED_NUM) {
        WIFI_LOGE("Over max pno failed number.");
        return;
    }

    if (!BeginPnoScan()) {
        WIFI_LOGE("BeginPnoScan failed.");
        return;
    }

    return;
}

void ScanService::GetScanControlInfo()
{
    WIFI_LOGI("Enter ScanService::GetScanControlInfo.\n");

    std::unique_lock<std::mutex> lock(scanControlInfoMutex);
    if (WifiSettings::GetInstance().GetScanControlInfo(scanControlInfo) != 0) {
        WIFI_LOGE("WifiSettings::GetInstance().GetScanControlInfo failed");
    }
    return;
}

ErrCode ScanService::AllowExternScan()
{
    WIFI_LOGI("Enter ScanService::AllowExternScan.\n");
    int staScene = GetStaScene();
    ScanMode scanMode = WifiSettings::GetInstance().GetAppRunningState();
    WIFI_LOGD("staScene is %{public}d, scanMode is %{public}d", staScene, (int)scanMode);

    if (!AllowExternScanByForbid(staScene, scanMode)) {
        WIFI_LOGD("extern scan not allow by forbid mode");
        return WIFI_OPT_FAILED;
    }
    int appId = 0;
    if (!AllowExternScanByInterval(appId, staScene, scanMode)) {
        WIFI_LOGD("extern scan not allow by interval mode");
        return WIFI_OPT_FAILED;
    }
    if (!AllowScanByMovingFreeze()) {
        WIFI_LOGD("extern scan not allow by moving freeze mode");
        return WIFI_OPT_MOVING_FREEZE_CTRL;
    }

    if (!AllowScanByDisableScanCtrl()) {
        WIFI_LOGD("extern scan not allow by disable scan control.");
        return WIFI_OPT_FAILED;
    }

    WIFI_LOGD("extern scan has allowed");
    return WIFI_OPT_SUCCESS;
}

ErrCode ScanService::AllowSystemTimerScan()
{
    WIFI_LOGI("Enter ScanService::AllowSystemTimerScan.\n");

    if (staStatus != static_cast<int>(OperateResState::DISCONNECT_DISCONNECTED) &&
        staStatus != static_cast<int>(OperateResState::CONNECT_AP_CONNECTED)) {
        return WIFI_OPT_FAILED;
    }

    /* The network is connected and cannot be automatically switched. */
    autoNetworkSelection = WifiSettings::GetInstance().GetWhetherToAllowNetworkSwitchover();
    if ((staStatus == static_cast<int>(OperateResState::CONNECT_AP_CONNECTED)) && (!autoNetworkSelection)) {
        return WIFI_OPT_FAILED;
    }

    int staScene = GetStaScene();
    /* Determines whether to allow scanning based on the STA status. */
    if (staScene == SCAN_SCENE_MAX) {
        return WIFI_OPT_FAILED;
    }

    if (!AllowScanDuringStaScene(staScene, ScanMode::SYSTEM_TIMER_SCAN)) {
        WIFI_LOGD("system timer scan not allowed, staScene is %{public}d", staScene);
        return WIFI_OPT_FAILED;
    }

    if (!AllowScanDuringCustomScene(ScanMode::SYSTEM_TIMER_SCAN)) {
        WIFI_LOGD("system timer scan not allowed");
        return WIFI_OPT_FAILED;
    }

    if (!AllowScanByMovingFreeze()) {
        return WIFI_OPT_MOVING_FREEZE_CTRL;
    }

    {
        std::unique_lock<std::mutex> lock(scanControlInfoMutex);
        for (auto iter = scanControlInfo.scanIntervalList.begin(); iter != scanControlInfo.scanIntervalList.end();
            ++iter) {
            if (iter->scanScene == SCAN_SCENE_ALL && iter->scanMode == ScanMode::SYSTEM_TIMER_SCAN &&
                iter->isSingle == false) {
                if (!SystemScanByInterval(systemScanIntervalMode.expScanCount,
                    systemScanIntervalMode.scanIntervalMode.interval, systemScanIntervalMode.scanIntervalMode.count)) {
                    return WIFI_OPT_FAILED;
                }
            }
        }
    }

    if (!AllowScanByDisableScanCtrl()) {
        WIFI_LOGD("extern scan not allow by disable scan control.");
        return WIFI_OPT_FAILED;
    }

    WIFI_LOGD("allow system timer scan");
    return WIFI_OPT_SUCCESS;
}

ErrCode ScanService::AllowPnoScan()
{
    WIFI_LOGI("Enter ScanService::AllowPnoScan.\n");

    if (staStatus != static_cast<int>(OperateResState::DISCONNECT_DISCONNECTED)) {
        return WIFI_OPT_FAILED;
    }

    int staScene = GetStaScene();
    if (staScene == SCAN_SCENE_MAX) {
        return WIFI_OPT_FAILED;
    }
    if (!AllowScanDuringStaScene(staScene, ScanMode::PNO_SCAN)) {
        WIFI_LOGD("pnoScan is not allowed for forbid map, staScene is %{public}d", staScene);
        return WIFI_OPT_FAILED;
    }
    if (!AllowScanDuringCustomScene(ScanMode::PNO_SCAN)) {
        WIFI_LOGD("pnoScan is not allowed for forbid map");
        return WIFI_OPT_FAILED;
    }
    if (!AllowScanByMovingFreeze()) {
        return WIFI_OPT_MOVING_FREEZE_CTRL;
    }

    {
        std::unique_lock<std::mutex> lock(scanControlInfoMutex);
        for (auto iter = scanControlInfo.scanIntervalList.begin(); iter != scanControlInfo.scanIntervalList.end();
            ++iter) {
            if (iter->scanScene == SCAN_SCENE_ALL && iter->scanMode == ScanMode::PNO_SCAN && iter->isSingle == false) {
                pnoScanIntervalMode.scanIntervalMode.intervalMode = iter->intervalMode;
                pnoScanIntervalMode.scanIntervalMode.interval = iter->interval;
                pnoScanIntervalMode.scanIntervalMode.count = iter->count;
                if (!PnoScanByInterval(pnoScanIntervalMode.fixedScanCount, pnoScanIntervalMode.fixedCurrentTime,
                    pnoScanIntervalMode.scanIntervalMode.interval, pnoScanIntervalMode.scanIntervalMode.count)) {
                    WIFI_LOGD("pnoScan is not allowed for interval mode");
                    return WIFI_OPT_FAILED;
                }
            }
        }
    }

    if (!AllowScanByDisableScanCtrl()) {
        WIFI_LOGD("extern scan not allow by disable scan control.");
        return WIFI_OPT_FAILED;
    }

    WIFI_LOGD("pno scan is allowed");
    return WIFI_OPT_SUCCESS;
}

ErrCode ScanService::AllowScanByType(ScanType scanType)
{
    LOGI("Enter ScanService::AllowScanByType.");

    ErrCode allScanResult = WIFI_OPT_SUCCESS;
    switch (scanType) {
        case ScanType::SCAN_TYPE_EXTERN:
            allScanResult = AllowExternScan();
            break;
        case ScanType::SCAN_TYPE_SYSTEMTIMER:
            allScanResult = AllowSystemTimerScan();
            break;
        case ScanType::SCAN_TYPE_PNO:
            allScanResult = AllowPnoScan();
            break;
        default:
            LOGE("scanType error.\n");
            break;
    }

    WIFI_LOGD("AllowScanByType ErrCode=%{public}d.", static_cast<int>(allScanResult));
    return allScanResult;
}

void ScanService::SetScanTrustMode()
{
    std::unique_lock<std::mutex> lock(scanControlInfoMutex);
    scanTrustMode = true;
}

void ScanService::ResetToNonTrustMode()
{
    std::unique_lock<std::mutex> lock(scanControlInfoMutex);
    scanTrustMode = false;
}

bool ScanService::IsScanTrustMode() const
{
    std::unique_lock<std::mutex> lock(scanControlInfoMutex);
    return scanTrustMode;
}

void ScanService::AddScanTrustSceneId(int sceneId)
{
    std::unique_lock<std::mutex> lock(scanControlInfoMutex);
    scanTrustSceneIds.emplace(sceneId);
}

void ScanService::ClearScanTrustSceneIds()
{
    std::unique_lock<std::mutex> lock(scanControlInfoMutex);
    scanTrustSceneIds.clear();
}

bool ScanService::IsInScanTrust(int sceneId) const
{
    std::unique_lock<std::mutex> lock(scanControlInfoMutex);
    if (scanTrustSceneIds.find(sceneId) != scanTrustSceneIds.end()) {
        return true;
    }

    return false;
}

void ScanService::SetMovingFreezeState(bool state)
{
    std::unique_lock<std::mutex> lock(scanControlInfoMutex);
    isAbsFreezeState = state;
}

bool ScanService::IsMovingFreezeState() const
{
    std::unique_lock<std::mutex> lock(scanControlInfoMutex);
    return isAbsFreezeState;
}

void ScanService::SetMovingFreezeScaned(bool scanned)
{
    std::unique_lock<std::mutex> lock(scanControlInfoMutex);
    isAbsFreezeScaned = scanned;
}

bool ScanService::IsMovingFreezeScaned() const
{
    std::unique_lock<std::mutex> lock(scanControlInfoMutex);
    return isAbsFreezeScaned;
}

ErrCode ScanService::ApplyTrustListPolicy(ScanType scanType)
{
    LOGI("Enter ScanService::ApplyTrustListPolicy.");
    ErrCode policyResult = WIFI_OPT_SUCCESS;

    SetScanTrustMode();
    policyResult = AllowScanByType(scanType);
    if (policyResult != WIFI_OPT_SUCCESS) {
        WIFI_LOGD("AllowScanByType failed.");
    }
    ResetToNonTrustMode();
    WIFI_LOGD("ResetToNonTrustMode");
    
    WIFI_LOGD("apply trust list policy, ErrCode=%{public}d", static_cast<int>(policyResult));

    return policyResult;
}

ErrCode ScanService::ApplyScanPolices(ScanType type)
{
    LOGI("Enter ScanService::ApplyScanPolices.");
    /* Obtains app parameters and scenario status parameters. */
    auto &appPackageName = WifiSettings::GetInstance().GetAppPackageName();
    auto &trustListPolicies = WifiSettings::GetInstance().ReloadTrustListPolicies();
    auto &movingFreezePolicy = WifiSettings::GetInstance().ReloadMovingFreezePolicy();
    ErrCode rlt = WIFI_OPT_SUCCESS;
    if (appPackageName.empty()) {
        rlt = AllowScanByType(type);
        WIFI_LOGD("appPackageName empty, apply scan polices ErrCode=%{public}d.", static_cast<int>(rlt));
        if (scanResultBackup != -1 && rlt == WIFI_OPT_MOVING_FREEZE_CTRL) {
            mScanSerivceCallbacks.OnScanFinishEvent(scanResultBackup);
        }
        return rlt;
    }

    /* Generates an scene id list based on appPackageName. */
    ClearScanTrustSceneIds();
    for (auto &policy : trustListPolicies) {
        if (IsPackageInTrustList(policy.trustList, policy.sceneId, appPackageName)) {
            AddScanTrustSceneId(policy.sceneId);
        }
    }
    const int movingFreezeSceneId = -1;
    if (IsPackageInTrustList(movingFreezePolicy.trustList, movingFreezeSceneId, appPackageName)) {
        AddScanTrustSceneId(movingFreezeSceneId);
    }

    rlt = ApplyTrustListPolicy(type);
    if (rlt != WIFI_OPT_SUCCESS) {
        if (scanResultBackup != -1 && rlt == WIFI_OPT_MOVING_FREEZE_CTRL) {
            LOGE("trust list policy, but moving freeze ctrl failed.");
            mScanSerivceCallbacks.OnScanFinishEvent(scanResultBackup);
        }
        return rlt;
    }
    WIFI_LOGD("apply scan policies result: scan control ok.");
    return WIFI_OPT_SUCCESS;
}

bool ScanService::AllowExternScanByForbid(int staScene, ScanMode scanMode)
{
    WIFI_LOGI("Enter ScanService::AllowExternScanByForbid.\n");

    if (IsExternScanning()) {
        if (!AllowScanDuringScanning(scanMode)) {
            return false;
        }
        if (!AllowScanDuringScanning(ScanMode::ALL_EXTERN_SCAN)) {
            return false;
        }
    }

    int state = WifiSettings::GetInstance().GetScreenState();
    if (state == MODE_STATE_CLOSE) {
        if (!AllowScanDuringScreenOff(scanMode)) {
            return false;
        }
        if (!AllowScanDuringScreenOff(ScanMode::ALL_EXTERN_SCAN)) {
            return false;
        }
    }
    if (!AllowScanDuringStaScene(staScene, scanMode)) {
        return false;
    }
    if (!AllowScanDuringStaScene(staScene, ScanMode::ALL_EXTERN_SCAN)) {
        return false;
    }
    if (!AllowScanDuringCustomScene(scanMode)) {
        return false;
    }
    if (!AllowScanDuringCustomScene(ScanMode::ALL_EXTERN_SCAN)) {
        return false;
    }

    return true;
}

bool ScanService::AllowExternScanByInterval(int appId, int staScene, ScanMode scanMode)
{
    WIFI_LOGI("Enter ScanService::AllowExternScanByInterval.\n");

    if (!AllowExternScanByIntervalMode(appId, staScene, scanMode)) {
        return false;
    }
    if (!AllowExternScanByIntervalMode(appId, staScene, ScanMode::ALL_EXTERN_SCAN)) {
        return false;
    }
    if (!AllowExternScanByIntervalMode(appId, SCAN_SCENE_FREQUENCY_ORIGIN, scanMode)) {
        return false;
    }
    if (!AllowExternScanByIntervalMode(appId, SCAN_SCENE_FREQUENCY_ORIGIN, ScanMode::ALL_EXTERN_SCAN)) {
        return false;
    }
    int noChargerPlugModeState = WifiSettings::GetInstance().GetNoChargerPlugModeState();
    if (noChargerPlugModeState == MODE_STATE_OPEN) {
        WIFI_LOGI("No charger plug mode state.");
        if (!AllowExternScanByIntervalMode(appId, SCAN_SCENE_FREQUENCY_CUSTOM, scanMode)) {
            return false;
        }
        if (!AllowExternScanByIntervalMode(appId, SCAN_SCENE_FREQUENCY_CUSTOM, ScanMode::ALL_EXTERN_SCAN)) {
            return false;
        }
    }
    if (!AllowExternScanByCustomScene(appId, scanMode)) {
        return false;
    }
    if (!AllowExternScanByCustomScene(appId, ScanMode::ALL_EXTERN_SCAN)) {
        return false;
    }
    return true;
}

int ScanService::GetStaScene()
{
    WIFI_LOGI("Enter ScanService::GetStaScene.\n");

    switch (staStatus) {
        case static_cast<int>(OperateResState::CONNECT_AP_CONNECTED):
            return SCAN_SCENE_CONNECTED;

        case static_cast<int>(OperateResState::DISCONNECT_DISCONNECTED):
            return SCAN_SCENE_DISCONNCTED;

        case static_cast<int>(OperateResState::CONNECT_CONNECTING):
            return SCAN_SCENE_CONNECTING;

        case static_cast<int>(OperateResState::CONNECT_OBTAINING_IP):
            return SCAN_SCENE_OBTAINING_IP;

        case static_cast<int>(OperateResState::CONNECT_ASSOCIATING):
            return SCAN_SCENE_ASSOCIATING;

        case static_cast<int>(OperateResState::CONNECT_ASSOCIATED):
            return SCAN_SCENE_ASSOCIATED;

        default:
            return SCAN_SCENE_MAX;
    }
}

bool ScanService::IsExternScanning() const
{
    WIFI_LOGI("Enter ScanService::IsExternScanning.\n");

    std::unique_lock<std::mutex> lock(scanConfigMapMutex);
    for (auto iter = scanConfigMap.begin(); iter != scanConfigMap.end(); ++iter) {
        if (iter->second.externFlag) {
            return true;
        }
    }
    return false;
}

bool ScanService::IsScanningWithParam()
{
    WIFI_LOGI("Enter ScanService::IsScanningWithParam.\n");

    std::unique_lock<std::mutex> lock(scanConfigMapMutex);
    for (auto iter = scanConfigMap.begin(); iter != scanConfigMap.end(); ++iter) {
        if (iter->second.scanningWithParamFlag) {
            return true;
        }
    }
    return false;
}

void ScanService::GetAllowBandFreqsControlInfo(ScanBandType &scanBand, std::vector<int> &freqs)
{
    WIFI_LOGI("Enter ScanService::GetAllowBandFreqsControlInfo.\n");

    int staScene = GetStaScene();

    bool allow24Ghz = true;
    bool allow5Ghz = true;
    if (!AllowScanDuringStaScene(staScene, ScanMode::BAND_24GHZ_SCAN)) {
        allow24Ghz = false;
    }
    if (!AllowScanDuringStaScene(staScene, ScanMode::BAND_5GHZ_SCAN)) {
        allow5Ghz = false;
    }
    if (!AllowScanDuringCustomScene(ScanMode::BAND_24GHZ_SCAN)) {
        allow24Ghz = false;
    }
    if (!AllowScanDuringCustomScene(ScanMode::BAND_5GHZ_SCAN)) {
        allow5Ghz = false;
    }

    {
        std::unique_lock<std::mutex> lock(scanControlInfoMutex);
        for (auto iter = scanControlInfo.scanForbidList.begin(); iter != scanControlInfo.scanForbidList.end(); ++iter) {
            if (iter->scanScene == SCAN_SCENE_ALL) {
                if (iter->scanMode == ScanMode::BAND_24GHZ_SCAN) {
                    allow24Ghz = false;
                }
                if (iter->scanMode == ScanMode::BAND_5GHZ_SCAN) {
                    allow5Ghz = false;
                }
            }
        }
    }

    if ((!allow24Ghz) && (!allow5Ghz)) {
        WIFI_LOGE("Both 2.4G and 5G are not allowed");
        scanBand = SCAN_BAND_UNSPECIFIED;
        freqs.clear();
        return;
    }

    if (!allow24Ghz) {
        scanBand = ConvertBandNotAllow24G(scanBand);
        Delete24GhzFreqs(freqs);
    }

    if (!allow5Ghz) {
        scanBand = ConvertBandNotAllow5G(scanBand);
        Delete5GhzFreqs(freqs);
    }

    return;
}

ScanBandType ScanService::ConvertBandNotAllow24G(ScanBandType scanBand)
{
    WIFI_LOGI("Enter ScanService::ConvertBandNotAllow24G.\n");

    switch (scanBand) {
        case SCAN_BAND_24_GHZ:
            return SCAN_BAND_UNSPECIFIED;

        case SCAN_BAND_5_GHZ:
        case SCAN_BAND_5_GHZ_DFS_ONLY:
        case SCAN_BAND_5_GHZ_WITH_DFS:
            return scanBand;

        case SCAN_BAND_BOTH:
            return SCAN_BAND_5_GHZ;

        case SCAN_BAND_BOTH_WITH_DFS:
            return SCAN_BAND_5_GHZ_WITH_DFS;

        default:
            return SCAN_BAND_UNSPECIFIED;
    }
}

ScanBandType ScanService::ConvertBandNotAllow5G(ScanBandType scanBand)
{
    WIFI_LOGI("Enter ScanService::ConvertBandNotAllow5G.\n");

    switch (scanBand) {
        case SCAN_BAND_24_GHZ:
        case SCAN_BAND_BOTH:
        case SCAN_BAND_BOTH_WITH_DFS:
            return SCAN_BAND_24_GHZ;

        case SCAN_BAND_5_GHZ:
        case SCAN_BAND_5_GHZ_DFS_ONLY:
        case SCAN_BAND_5_GHZ_WITH_DFS:
        default:
            return SCAN_BAND_UNSPECIFIED;
    }
}

void ScanService::Delete24GhzFreqs(std::vector<int> &freqs)
{
    WIFI_LOGI("Enter ScanService::Delete24GhzFreqs.\n");

    auto iter = freqs.begin();
    while (iter != freqs.end()) {
        if (*iter < FREQS_24G_MAX_VALUE) {
            freqs.erase(iter);
        } else {
            ++iter;
        }
    }

    return;
}

void ScanService::Delete5GhzFreqs(std::vector<int> &freqs)
{
    WIFI_LOGI("Enter ScanService::Delete24GhzFreqs.\n");

    auto iter = freqs.begin();
    while (iter != freqs.end()) {
        if (*iter > FREQS_5G_MIN_VALUE) {
            freqs.erase(iter);
        } else {
            ++iter;
        }
    }

    return;
}

bool ScanService::GetSavedNetworkSsidList(std::vector<std::string> &savedNetworkSsid)
{
    WIFI_LOGI("Enter ScanService::GetSavedNetworkSsidList.\n");

    std::vector<WifiDeviceConfig> deviceConfigs;
    if (WifiSettings::GetInstance().GetDeviceConfig(deviceConfigs) != 0) {
        WIFI_LOGE("WifiSettings::GetInstance().GetDeviceConfig failed");
        return false;
    }

    for (auto iter = deviceConfigs.begin(); iter != deviceConfigs.end(); ++iter) {
        if ((iter->status == static_cast<int>(WifiDeviceConfigStatus::ENABLED)) && (!(iter->isPasspoint)) &&
            (!(iter->isEphemeral))) {
            savedNetworkSsid.push_back(iter->ssid);
        }
    }

    return true;
}

bool ScanService::GetHiddenNetworkSsidList(std::vector<std::string> &hiddenNetworkSsid)
{
    WIFI_LOGI("Enter ScanService::GetHiddenNetworkSsidList.\n");

    std::vector<WifiDeviceConfig> deviceConfigs;
    if (WifiSettings::GetInstance().GetDeviceConfig(deviceConfigs) != 0) {
        WIFI_LOGE("WifiSettings::GetInstance().GetDeviceConfig failed");
        return false;
    }

    for (auto iter = deviceConfigs.begin(); iter != deviceConfigs.end(); ++iter) {
        if (iter->hiddenSSID) {
            hiddenNetworkSsid.push_back(iter->ssid);
        }
    }

    return true;
}

void ScanService::ClearScanControlValue()
{
    WIFI_LOGI("Enter ScanService::ClearScanControlValue.\n");

    staCurrentTime = 0;
    customCurrentTime = 0;
    appForbidList.clear();
    scanBlocklist.clear();
    fullAppForbidList.clear();
    customSceneTimeMap.clear();
}

void ScanService::SetStaCurrentTime()
{
    WIFI_LOGI("Enter ScanService::SetStaCurrentTime.\n");
    time_t now = time(0);
    staCurrentTime = now;

    int state = WifiSettings::GetInstance().GetScreenState();
    if (state == MODE_STATE_CLOSE) {
        if (ApplyScanPolices(ScanType::SCAN_TYPE_PNO) != WIFI_OPT_SUCCESS) {
            EndPnoScan();
            pnoScanFailedNum = 0;
            pScanStateMachine->StopTimer(static_cast<int>(RESTART_PNO_SCAN_TIMER));
        }
    }
    return;
}

bool ScanService::AllowScanDuringScanning(ScanMode scanMode) const
{
    WIFI_LOGI("Enter ScanService::AllowScanDuringScanning.\n");

    std::unique_lock<std::mutex> lock(scanControlInfoMutex);
    for (auto iter = scanControlInfo.scanForbidList.begin(); iter != scanControlInfo.scanForbidList.end(); ++iter) {
        if (iter->scanScene == SCAN_SCENE_SCANNING && iter->scanMode == scanMode) {
            WIFI_LOGD("scan not allow by scanning scene.");
            return false;
        }
    }
    return true;
}

bool ScanService::AllowScanDuringScreenOff(ScanMode scanMode) const
{
    WIFI_LOGI("Enter ScanService::AllowScanDuringScreenOff.\n");

    bool isTrustListMode = IsScanTrustMode();
    bool isInList = IsInScanTrust(SCAN_SCENE_SCREEN_OFF);
    if (isTrustListMode && isInList) {
        WIFI_LOGD("Trust list mode,sceneId(SCAN_SCENE_SCREEN_OFF) in the list,return true.");
        return true;
    }

    std::unique_lock<std::mutex> lock(scanControlInfoMutex);
    for (auto iter = scanControlInfo.scanForbidList.begin(); iter != scanControlInfo.scanForbidList.end(); ++iter) {
        if (iter->scanScene == SCAN_SCENE_SCREEN_OFF && iter->scanMode == scanMode) {
            WIFI_LOGD("scan not allow by screen off scene.");
            return false;
        }
    }
    return true;
}

bool ScanService::AllowScanDuringStaScene(int staScene, ScanMode scanMode)
{
    WIFI_LOGI("Enter ScanService::AllowScanDuringStaScene.\n");

    time_t now = time(nullptr);
    if (now < 0) {
        return false;
    }
    std::unique_lock<std::mutex> lock(scanControlInfoMutex);
    for (auto iter = scanControlInfo.scanForbidList.begin(); iter != scanControlInfo.scanForbidList.end(); ++iter) {
        WIFI_LOGD("now - staCurrentTime:%{public}d, iter->forbidTime:%{public}d", int(now - staCurrentTime),
            iter->forbidTime);
        /* forbid scan mode found in scan scene. */
        if (iter->scanScene == staScene && iter->scanMode == scanMode) {
            /* forbidCount=0 and forbidTime=0, directly forbid scan. */
            if ((iter->forbidTime == 0) && (iter->forbidCount == 0)) {
                WIFI_LOGD("Scan is forbidden by staScene.");
                return false;
            }
            /* Unconditional scan control for forbidCount times */
            if ((iter->forbidCount > 0) && (iter->forbidCount - staSceneForbidCount > 0)) {
                WIFI_LOGD("Scan is forbidden in forbidCount.");
                staSceneForbidCount++;
                return false;
            }
            /* Scan interval less than forbidTime, forbid scan. */
            if ((iter->forbidTime > 0) && (now - staCurrentTime <= iter->forbidTime)) {
                WIFI_LOGD("Scan is forbidden in forbidTime.");
                return false;
            }
        }
    }

    return true;
}

bool ScanService::AllowScanDuringCustomScene(ScanMode scanMode)
{
    WIFI_LOGI("Enter ScanService::AllowScanDuringCustomScene.\n");

    bool isTrustListMode = IsScanTrustMode();
    for (auto customIter = customSceneTimeMap.begin(); customIter != customSceneTimeMap.end(); ++customIter) {
        if (isTrustListMode && IsInScanTrust(customIter->first)) {
            WIFI_LOGD("Trust list mode,sceneId(%{public}d) in the list, continue.", customIter->first);
            continue;
        }

        if (!AllowCustomSceneCheck(customIter, scanMode)) {
            return false;
        }
    }
    return true;
}

bool ScanService::AllowCustomSceneCheck(const std::map<int, time_t>::const_iterator &customIter, ScanMode scanMode)
{
    std::unique_lock<std::mutex> lock(scanControlInfoMutex);
    for (auto iter = scanControlInfo.scanForbidList.begin(); iter != scanControlInfo.scanForbidList.end(); ++iter) {
        if (iter->scanScene == customIter->first && iter->scanMode == scanMode) {
            /* forbidCount=0 and forbidTime=0, directly forbid scan. */
            if ((iter->forbidTime == 0) && (iter->forbidCount == 0)) {
                WIFI_LOGD("Scan is forbidden by staScene.");
                return false;
            }
            /* Unconditional scan control for forbidCount times. */
            if (iter->forbidCount > 0 && iter->forbidCount - customSceneForbidCount > 0) {
                customSceneForbidCount++;
                return false;
            }
            /* Scan interval less than forbidTime, forbid scan. */
            time_t now = time(nullptr);
            if (iter->forbidTime > 0 && iter->forbidTime > now - customIter->second) {
                return false;
            }
        }
    }

    return true;
}

bool ScanService::AllowExternScanByIntervalMode(int appId, int scanScene, ScanMode scanMode)
{
    WIFI_LOGI("Enter ScanService::AllowExternScanByIntervalMode.\n");

    bool isTrustListMode = IsScanTrustMode();
    if (isTrustListMode && IsInScanTrust(scanScene)) {
        WIFI_LOGD("Trust list mode,sceneId(%{public}d) in the list, return true.", scanScene);
        return true;
    }

    std::unique_lock<std::mutex> lock(scanControlInfoMutex);
    for (auto intervalListIter = scanControlInfo.scanIntervalList.begin();
         intervalListIter != scanControlInfo.scanIntervalList.end();
         ++intervalListIter) {
        /* Determine whether control is required in the current scene and scan mode. */
        if (intervalListIter->scanScene == scanScene && intervalListIter->scanMode == scanMode) {
            /* If a single application is distinguished. */
            if (intervalListIter->isSingle) {
                if (!AllowSingleAppScanByInterval(appId, *intervalListIter)) {
                    return false;
                }
            } else {
                if (!AllowFullAppScanByInterval(appId, *intervalListIter)) {
                    return false;
                }
            }
        }
    }
    return true;
}

bool ScanService::AllowExternScanByCustomScene(int appId, ScanMode scanMode)
{
    WIFI_LOGI("Enter ScanService::AllowExternScanByCustomScene.\n");

    bool isTrustListMode = IsScanTrustMode();
    for (auto customIter = customSceneTimeMap.begin(); customIter != customSceneTimeMap.end(); ++customIter) {
        if (isTrustListMode && IsInScanTrust(customIter->first)) {
            WIFI_LOGD("Trust list mode,sceneId(%{public}d) in the list, continue.", customIter->first);
            continue;
        }

        if (!AllowExternCustomSceneCheck(customIter, appId, scanMode)) {
            return false;
        }
    }
    return true;
}

bool ScanService::AllowExternCustomSceneCheck(const std::map<int, time_t>::const_iterator &customIter, int appId,
    ScanMode scanMode)
{
    std::unique_lock<std::mutex> lock(scanControlInfoMutex);
    for (auto intervalListIter = scanControlInfo.scanIntervalList.begin();
        intervalListIter != scanControlInfo.scanIntervalList.end(); ++intervalListIter) {
        /* Determine whether control is required in the current scene and scan mode. */
        if (intervalListIter->scanScene == customIter->first && intervalListIter->scanMode == scanMode &&
            intervalListIter->isSingle) {
            /* If a single application is distinguished. */
            if (!AllowSingleAppScanByInterval(appId, *intervalListIter)) {
                return false;
            }
        } else if (intervalListIter->scanScene == customIter->first && intervalListIter->scanMode == scanMode &&
            !intervalListIter->isSingle) {
            if (!AllowFullAppScanByInterval(appId, *intervalListIter))
                return false;
        }
    }
    return true;
}

bool ScanService::PnoScanByInterval(int &fixedScanCount, time_t &fixedScanTime, int interval, int count)
{
    WIFI_LOGI("Enter ScanService::PnoScanByInterval.\n");

    time_t now = time(nullptr);
    /* First scan */
    if (fixedScanCount == 0) {
        fixedScanCount++;
        fixedScanTime = now;
        return true;
    }
    if (now - fixedScanTime >= interval) {
        fixedScanCount = 1;
        fixedScanTime = now;
        return true;
    }
    if (fixedScanCount > count) {
        return false;
    }
    fixedScanCount++;
    return true;
}

bool ScanService::SystemScanByInterval(int &expScanCount, int &interval, int &count)
{
    WIFI_LOGI("Enter ScanService::SystemScanByInterval.\n");
    /*
     * Exponential interval. The value of interval is the initial value.
     * After the value is multiplied by 2, the last fixed interval is used.
     */
    if (expScanCount > 0 && count > 1) {
        interval *= DOUBLE_SCAN_INTERVAL;
        count--;
    }
    expScanCount++;
    return true;
}

bool ScanService::ExternScanByInterval(int appId, SingleAppForbid &singleAppForbid)
{
    WIFI_LOGI("Enter ScanService::ExternScanByInterval.\n");

    switch (singleAppForbid.scanIntervalMode.intervalMode) {
        case IntervalMode::INTERVAL_FIXED:
            return AllowScanByIntervalFixed(singleAppForbid.fixedScanCount, singleAppForbid.fixedCurrentTime,
                singleAppForbid.scanIntervalMode.interval, singleAppForbid.scanIntervalMode.count);

        case IntervalMode::INTERVAL_EXP:
            return AllowScanByIntervalExp(singleAppForbid.expScanCount, singleAppForbid.scanIntervalMode.interval,
                singleAppForbid.scanIntervalMode.count);

        case IntervalMode::INTERVAL_CONTINUE:
            return AllowScanByIntervalContinue(singleAppForbid.continueScanTime, singleAppForbid.lessThanIntervalCount,
                singleAppForbid.scanIntervalMode.interval, singleAppForbid.scanIntervalMode.count);

        case IntervalMode::INTERVAL_BLOCKLIST:
            WIFI_LOGI("INTERVAL_BLOCKLIST IntervalMode.\n");
            return AllowScanByIntervalBlocklist(appId, singleAppForbid.blockListScanTime,
                singleAppForbid.lessThanIntervalCount, singleAppForbid.scanIntervalMode.interval,
                singleAppForbid.scanIntervalMode.count);

        default:
            return true;
    }
}

bool ScanService::AllowSingleAppScanByInterval(int appId, ScanIntervalMode scanIntervalMode)
{
    WIFI_LOGI("Enter ScanService::AllowSingleAppScanByInterval.\n");

    bool appIdExisted = false;
    for (auto forbidListIter = appForbidList.begin(); forbidListIter != appForbidList.end(); ++forbidListIter) {
        if (forbidListIter->appID == appId &&
            forbidListIter->scanIntervalMode.scanScene == scanIntervalMode.scanScene &&
            forbidListIter->scanIntervalMode.scanMode == scanIntervalMode.scanMode) {
            appIdExisted = true;
        }
    }
    /* If the appId is the first scan request, add it to appForbidList. */
    if (!appIdExisted) {
        SingleAppForbid singleAppForbid;
        singleAppForbid.appID = appId;
        singleAppForbid.scanIntervalMode.scanScene = scanIntervalMode.scanScene;
        singleAppForbid.scanIntervalMode.scanMode = scanIntervalMode.scanMode;
        singleAppForbid.scanIntervalMode.interval = scanIntervalMode.interval;
        singleAppForbid.scanIntervalMode.intervalMode = scanIntervalMode.intervalMode;
        singleAppForbid.scanIntervalMode.count = scanIntervalMode.count;
        appForbidList.push_back(singleAppForbid);
    }
    for (auto iter = appForbidList.begin(); iter != appForbidList.end(); ++iter) {
        if (iter->appID == appId && iter->scanIntervalMode.scanScene == scanIntervalMode.scanScene &&
            iter->scanIntervalMode.scanMode == scanIntervalMode.scanMode) {
            if (!ExternScanByInterval(appId, *iter)) {
                WIFI_LOGI("AllowSingleAppScanByInterval:false.");
                return false;
            }
        }
    }
    WIFI_LOGI("AllowSingleAppScanByInterval:true.");
    return true;
}

bool ScanService::AllowFullAppScanByInterval(int appId, ScanIntervalMode scanIntervalMode)
{
    WIFI_LOGI("Enter ScanService::AllowFullAppScanByInterval.\n");

    bool fullAppExisted = false;
    for (auto fullAppForbidIter = fullAppForbidList.begin(); fullAppForbidIter != fullAppForbidList.end();
        ++fullAppForbidIter) {
        if (fullAppForbidIter->scanIntervalMode.scanScene == scanIntervalMode.scanScene &&
            fullAppForbidIter->scanIntervalMode.scanMode == scanIntervalMode.scanMode) {
            fullAppExisted = true;
        }
    }
    if (!fullAppExisted) {
        SingleAppForbid singleAppForbid;
        singleAppForbid.scanIntervalMode.scanScene = scanIntervalMode.scanScene;
        singleAppForbid.scanIntervalMode.scanMode = scanIntervalMode.scanMode;
        singleAppForbid.scanIntervalMode.interval = scanIntervalMode.interval;
        singleAppForbid.scanIntervalMode.intervalMode = scanIntervalMode.intervalMode;
        singleAppForbid.scanIntervalMode.count = scanIntervalMode.count;
        fullAppForbidList.push_back(singleAppForbid);
    }
    for (auto iter = fullAppForbidList.begin(); iter != fullAppForbidList.end(); ++iter) {
        if (iter->scanIntervalMode.scanScene == scanIntervalMode.scanScene &&
            iter->scanIntervalMode.scanMode == scanIntervalMode.scanMode) {
            if (!ExternScanByInterval(appId, *iter)) {
                WIFI_LOGI("AllowFullAppScanByInterval:false.");
                return false;
            }
        }
    }
    WIFI_LOGI("AllowFullAppScanByInterval:true.");
    return true;
}

bool ScanService::AllowScanByIntervalFixed(int &fixedScanCount, time_t &fixedScanTime, int &interval, int &count)
{
    WIFI_LOGI("Enter ScanService::AllowScanByIntervalFixed.\n");

    time_t now = time(nullptr);
    /* First scan */
    if (fixedScanCount == 0) {
        fixedScanCount++;
        fixedScanTime = now;
        return true;
    }
    /* The scanning interval is greater than interval, and counting is restarted. */
    if (now - fixedScanTime >= interval) {
        fixedScanCount = 1;
        fixedScanTime = now;
        return true;
    }
    /* *
     * Scan is forbidden because the scanning interval is less than interval
     * and the number of scan times exceeds count.
     */
    if (fixedScanCount >= count) {
        return false;
    }
    fixedScanCount++;
    return true;
}

bool ScanService::AllowScanByIntervalExp(int &expScanCount, int &interval, int &count)
{
    WIFI_LOGI("Enter ScanService::AllowScanByIntervalExp.\n");

    /*
     * Exponential interval. The value of interval is the initial value.
     * After the value is multiplied by 2, the last fixed interval is used.
     */
    if (expScanCount > 0 && count > 1) {
        interval *= DOUBLE_SCAN_INTERVAL;
        count--;
    }
    expScanCount++;
    return true;
}

bool ScanService::AllowScanByIntervalContinue(time_t &continueScanTime, int &lessThanIntervalCount, int &interval,
    int &count)
{
    WIFI_LOGI("Enter ScanService::AllowScanByIntervalContinue.\n");

    WIFI_LOGD("lessThanIntervalCount:%{public}d, interval:%{public}d, count:%{public}d", lessThanIntervalCount,
        interval, count);
    time_t now = time(nullptr);
    /* First scan */
    if (continueScanTime == 0) {
        continueScanTime = now;
        return true;
    }
    /* If count is less than interval, the subsequent interval must be greater than interval. */
    if (now - continueScanTime < interval) {
        lessThanIntervalCount++;
        if (lessThanIntervalCount < count) {
            continueScanTime = now;
            return true;
        }
        /* If the scanning interval is not exceeded continuously, the counter is cleared. */
        lessThanIntervalCount = 0;
        return false;
    }
    /* If the scanning interval is not exceeded continuously, the counter is cleared. */
    lessThanIntervalCount = 0;
    continueScanTime = now;
    return true;
}

bool ScanService::AllowScanByIntervalBlocklist(
    int appId, time_t &blockListScanTime, int &lessThanIntervalCount, int &interval, int &count)
{
    WIFI_LOGI("Enter ScanService::AllowScanByIntervalBlocklist.\n");

    time_t now = time(nullptr);
    if (now - blockListScanTime >= interval) {
        for (auto iter = scanBlocklist.begin(); iter != scanBlocklist.end();) {
            if (*iter == appId) {
                iter = scanBlocklist.erase(iter);
            } else {
                ++iter;
            }
        }
        blockListScanTime = now;
        return true;
    }
    /* If the app ID is in the blocklist, extern scan is forbidden. */
    if (std::find(scanBlocklist.begin(), scanBlocklist.end(), appId) != scanBlocklist.end()) {
        WIFI_LOGD("extern scan not allowed by blocklist");
        return false;
    }
    /* First scan */
    if (blockListScanTime == 0) {
        blockListScanTime = now;
        WIFI_LOGD("blockListScanTime, first scan.");
        return true;
    }
    /**
     * If the number of consecutive count times is less than the value of interval,
     * the user is added to the blocklist and cannot be scanned.
     */
    if (now - blockListScanTime < interval) {
        lessThanIntervalCount++;
        if (lessThanIntervalCount < count) {
            blockListScanTime = now;
            WIFI_LOGD("blockListScanTime, lessThanIntervalCount(%{public}d),return true.", lessThanIntervalCount);
            return true;
        }
        /**
         * If the accumulated scanning interval is less than interval and the number of times
         * is greater than count, the user is blocklisted forbidding scanning.
         */
        scanBlocklist.push_back(appId);
        WIFI_LOGD("scanBlocklist.push_back(appId), return false.");
        return false;
    }
    blockListScanTime = now;
    return true;
}

bool ScanService::AllowScanByDisableScanCtrl()
{
    std::unique_lock<std::mutex> lock(scanControlInfoMutex);
    return !disableScanFlag;
}

bool ScanService::AllowScanByMovingFreeze()
{
    LOGI("Enter ScanService::AllowScanByMovingFreeze.\n");

    /* moving freeze trust mode. */
    bool isTrustListMode = IsScanTrustMode();
    if (isTrustListMode && IsInScanTrust(-1)) {
        WIFI_LOGD("Trust list mode,sceneId(MovingFreeze) in the list, return true.");
        return true;
    }

    if (!IsMovingFreezeState()) {
        WIFI_LOGD("It's not in the movingfreeze mode, return true.");
        return true;
    }

    if (!IsMovingFreezeScaned()) {
        SetMovingFreezeScaned(true);
        WIFI_LOGD("In movingfreeze mode, return true for the first scan.");
        return true;
    } else {
        WIFI_LOGD("In movingfreeze mode, return false for the already scanned.");
        return false;
    }

    return true;
}

bool ScanService::IsPackageInTrustList(const std::string &trustList, int sceneId,
    const std::string &appPackageName) const
{
    std::vector<std::string> trustPackages;
    SplitString(trustList, "|", trustPackages);

    bool bFind = false;
    for (const auto &package : trustPackages) {
        if (package == appPackageName) {
            WIFI_LOGD("IsPackageInTrustList=true");
            bFind = true;
            break;
        }
    }

    if (!bFind) {
        WIFI_LOGD("sceneId=%{public}d, appName=%{public}s trustList=%{public}s, not in the lists.", sceneId,
            appPackageName.c_str(), trustList.c_str());
    }

    return bFind;
}
}  // namespace Wifi
}  // namespace OHOS
