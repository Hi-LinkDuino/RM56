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
#ifndef OHOS_WIFI_SCAN_SERVICE_H
#define OHOS_WIFI_SCAN_SERVICE_H

#include <string>
#include <map>
#include <mutex>
#include <ctime>
#include <set>
#include <unordered_set>
#include "iscan_service_callbacks.h"
#include "define.h"
#include "wifi_scan_msg.h"
#include "wifi_errcode.h"
#include "scan_common.h"
#include "scan_monitor.h"
#include "scan_state_machine.h"

namespace OHOS {
namespace Wifi {
const int DISCONNECTED_SCAN_INTERVAL = 20 * 60 * 1000;
const int RESTART_PNO_SCAN_TIME = 5 * 1000;
const int FREQS_24G_MAX_VALUE = 2500;
const int FREQS_5G_MIN_VALUE = 5000;
const int SECOND_TO_MICRO_SECOND = 1000000;
const int MAX_PNO_SCAN_FAILED_NUM = 5;
const int DOUBLE_SCAN_INTERVAL = 2;
const int STA_SCAN_SCENE = 1;
const int CUSTOM_SCAN_SCENE = 2;
const int SCREEN_SCAN_SCENE = 3;
const int OTHER_SCAN_SCENE = 4;
const int SYSTEM_SCAN_INIT_TIME = 20;

class ScanService {
    FRIEND_GTEST(ScanService);
public:
    ScanService();
    virtual ~ScanService();
    /**
     * @Description  Initializing the Scan Service.
     *
     * @param scanSerivceCallbacks Callback function registered with the wifiManager[in].
     * @return success: true, failed: false
     */
    virtual bool InitScanService(const IScanSerivceCallbacks &scanSerivceCallbacks);
    /**
     * @Description Stopping the Scan Service.
     *
     */
    virtual void UnInitScanService();
    /**
     * @Description Registers the callback function of the scanning module to the interface service.
     *
     * @param scanSerivceCallbacks callback function
     */
    virtual void RegisterScanCallbacks(const IScanSerivceCallbacks &iScanSerivceCallbacks);
    /**
     * @Description Start a complete Wi-Fi scan.
     *
     * @param externFlag - Externally initiated scanning[in]
     * @return success: WIFI_OPT_SUCCESS, failed: WIFI_OPT_FAILED
     */
    virtual ErrCode Scan(bool externFlag);
    /**
     * @Description Start Wi-Fi scanning based on specified parameters.
     *
     * @param params - Scan specified parameters[in]
     * @return success: WIFI_OPT_SUCCESS, failed: WIFI_OPT_FAILED
     */
    virtual ErrCode ScanWithParam(const WifiScanParams &params);
    /**
     * @Description Disable/Restore the scanning operation.
     *
     * * @param params - disable or not.
     * @return WIFI_OPT_SUCCESS: success, WIFI_OPT_FAILED: failed
     */
    ErrCode DisableScan(bool disable);
    /**
     * @Description Starting a Single Scan.
     *
     * @param scanConfig - Scanning parameters[in]
     * @return success: true, failed: false
     */
    bool SingleScan(ScanConfig &scanConfig);
    /**
     * @Description  Start PNO scanning
     * @return success: true, failed: false
     */
    bool BeginPnoScan();
    /**
     * @Description Start PNO scanning
     *
     * @param pnoScanConfig - PNO scanning parameters[in]
     * @param interScanConfig - common scanning parameters[in]
     * @return success: true, failed: false
     */
    bool PnoScan(const PnoScanConfig &pnoScanConfig, const InterScanConfig &interScanConfig);
    /**
     * @Description Disable PNO scanning.
     *
     */
    void EndPnoScan();
    /**
     * @Description The system scans and selects a scanning mode
     *              based on the current screen status and STA status.
     *
     * @param scanAtOnce - Whether to start scanning immediately[in]
     */
    virtual void SystemScanProcess(bool scanAtOnce);
    /**
     * @Description Status reported by the state machine.
     *
     * @param scanStatusReport - Structure of the reported status.[in]
     */
    virtual void HandleScanStatusReport(ScanStatusReport &scanStatusReport);
    /**
     * @Description Internal event reporting and processing.
     *
     * @param innerEvent - Internal event[in]
     */
    void HandleInnerEventReport(ScanInnerEventType innerEvent);
    /**
     * @Description Screen State (On/Off) Change Handler
     *
     */
    virtual void HandleScreenStatusChanged();
    /**
     * @Description STA status change processing
     *
     * @param state - STA state[in]
     */
    virtual void HandleStaStatusChanged(int status);
    /**
     * @Description movingfreeze status change processing
     *
     */
    virtual void HandleMovingFreezeChanged();
    /**
     * @Description custom scene status change processing
     *
     * @param customScene custom scene[in]
     * @param customSceneStatus custom scene status[in]
     */
    virtual void HandleCustomStatusChanged(int customScene, int customSceneStatus);
    /**
     * @Description Get custom scene state.
     *
     * @param customState custom scene state map[out]
     * @return
     */
    virtual void HandleGetCustomSceneState(std::map<int, time_t>& sceneMap) const;
    /**
     * @Description Query and save the scan control policy.
     *
     */
    virtual void GetScanControlInfo();
    /**
     * @Description When scanning control changes, the count data needs to be cleared.
     *
     */
    void ClearScanControlValue();
    /**
     * @Description When scanning control changes, the count data needs to be cleared.
     *
     */
    virtual void SetStaCurrentTime();

private:
    using ScanConfigMap = std::map<int, StoreScanConfig>;
    using ScanInfoHandlerMap = std::map<std::string, ScanInfoHandler>;
    using PnoScanInfoHandlerMap = std::map<std::string, PnoScanInfoHandler>;

    IScanSerivceCallbacks mScanSerivceCallbacks;
    ScanStateMachine *pScanStateMachine;             /* Scanning state machine pointer */
    ScanMonitor *pScanMonitor;                       /* Scanning Monitor Pointer */
    bool scanStartedFlag;                            /* The scanning is started */
    ScanInfoHandlerMap scanInfoHandlerMap;              /* Map of obtaining the scanning result */
    PnoScanInfoHandlerMap pnoScanInfoHandlerMap;        /* Map of obtaining PNO scanning results */
    ScanConfigMap scanConfigMap;                     /* Save Scan Configuration */
    int scanConfigStoreIndex;                        /* Index for saving the scan configuration */
    int64_t pnoScanStartTime;                        /* PNO scanning start time */
    int staStatus;                                   /* STA state */
    bool isPnoScanBegined;                           /* The PNO scanning has been started */
    bool autoNetworkSelection;                       /* Automatic network selection */
    int64_t lastSystemScanTime;                      /* Last System Scan Time */
    int pnoScanFailedNum;                            /* Number of PNO Scan Failures */
    ScanControlInfo scanControlInfo;                 /* Scan Control Policy */
    bool disableScanFlag;                            /* Disable Scan Flag. */
    std::vector<int> freqs2G;                        /* The support frequencys for 2.4G */
    std::vector<int> freqs5G;                        /* The support frequencys for 5G */
    std::vector<int> freqsDfs;                       /* The support frequencys for DFS */
    SystemScanIntervalMode systemScanIntervalMode;   /* Store system scan data */
    PnoScanIntervalMode pnoScanIntervalMode;         /* Store pno scan data */
    time_t staCurrentTime;                           /* Indicates the time when the STA enters the STA scenario */
    time_t customCurrentTime; /* Indicates the time when the STA enters the Customer-defined scenario */
    std::vector<SingleAppForbid> appForbidList; /* Store extern app scan data */
    /*
     * If the number of consecutive count times is less than the value of
     * interval, the user is added to the blocklist and cannot be scanned.
     */
    std::vector<int> scanBlocklist;
    /* Stores data that is scanned and controlled regardless of applications. */
    std::vector<SingleAppForbid> fullAppForbidList;
    std::map<int, time_t> customSceneTimeMap; /* Record the time when a scene is entered. */
    int staSceneForbidCount;
    int customSceneForbidCount;
    mutable std::mutex scanConfigMapMutex;
    mutable std::mutex scanControlInfoMutex;
    enum class ScanType {
        SCAN_TYPE_EXTERN = 0,
        SCAN_TYPE_SYSTEMTIMER,
        SCAN_TYPE_PNO,
    };
    bool scanTrustMode;                              /* scan trustlist mode */
    std::unordered_set<int> scanTrustSceneIds;       /* scan scene ids */
    bool isAbsFreezeState;                           /* absolute freeze state. */
    bool isAbsFreezeScaned;                          /* scanned in freeze state. */
    int scanResultBackup;                            /* scan result backup. */

    /**
     * @Description Obtains the frequency of a specified band.
     *
     * @param band - Scan frequency bands. Obtain the frequency list based on the frequency bands.[in]
     * @param freqs - Frequency list[out]
     * @return success: true, failed: false
     */
    bool GetBandFreqs(ScanBandType band, std::vector<int> &freqs);
    /**
     * @Description Enter the scanning message body.
     *
     * @param interMessage - Message pointer[in]
     * @param interConfig - Scan Configuration[in]
     * @return success: true, failed: false
     */
    bool AddScanMessageBody(InternalMessage *interMessage, const InterScanConfig &interConfig);
    /**
     * @Description Save Request Configuration
     *
     * @param scanConfig - Scanning parameters[in]
     * @param interConfig - Internal Scanning Parameters[in]
     * @return success: Saved request index, failed: MAX_SCAN_CONFIG_STORE_INDEX
     */
    int StoreRequestScanConfig(const ScanConfig &scanConfig, const InterScanConfig &interConfig);
    /**
     * @Description Save the scanning result in the configuration center.
     *
     * @param scanConfig - scan Config[in]
     * @param scanInfoList - scan result list[in]
     * @return success: true, failed: false
     */
    bool StoreFullScanInfo(const StoreScanConfig &scanConfig, const std::vector<InterScanInfo> &scanInfoList);
    /**
     * @Description Saves the scanning result of specified parameters in the configuration center.
     *
     * @param scanConfig - scan Config[in]
     * @param scanInfoList - scan result list[in]
     * @return success: true, failed: false
     */
    bool StoreUserScanInfo(const StoreScanConfig &scanConfig, std::vector<InterScanInfo> &scanInfoList);
    /**
     * @Description Sends the scanning result to the interface service,
     *              which then sends the scanning result to the connection
     *              management module for processing.
     *
     * @param scanInfoList - scan result list[in]
     */
    void ReportScanInfos(std::vector<InterScanInfo> &interScanList);

    /**
     * @Description Sends the store scanning result to the interface service,
     *              which then sends the store scanning result to the connection
     *              management module for processing.
     *
     * @param scanInfoList - scan result list[in]
     */
    void ReportStoreScanInfos(std::vector<InterScanInfo> &interScanList);

    /**
     * @Description Enter the PNO scanning message body.
     *
     * @param interMessage - Message pointer[in]
     * @param pnoScanConfig - PNO Scanning Configuration[in]
     * @return success: true, failed: false
     */
    bool AddPnoScanMessageBody(InternalMessage *interMessage, const PnoScanConfig &pnoScanConfig);
    /**
     * @Description Stopping System Scanning
     *
     */
    void StopSystemScan();
    /**
     * @Description Start the periodic scanning.
     *
     * @param scanAtOnce - Whether to start a scan immediately[in]
     */
    void StartSystemTimerScan(bool scanAtOnce);
    /**
     * @Description System scanning timer expiration processing.
     *
     */
    void HandleSystemScanTimeout();
    /**
     * @Description Detected disconnections at intervals and started scanning.
     *
     */
    void DisconnectedTimerScan();
    /**
     * @Description Detected disconnections at intervals and started scanning.
     *
     */
    void HandleDisconnectedScanTimeout();
    /**
     * @Description Callback function for obtaining the scanning result
     *
     * @param requestIndexList - Request Index List[in]
     * @param scanInfoList - Scan Info List[in]
     */
    void HandleCommonScanInfo(std::vector<int> &requestIndexList, std::vector<InterScanInfo> &scanInfoList);
    /**
     * @Description Common scanning failure processing
     *
     * @param requestIndexList - Request Index List[in]
     */
    void HandleCommonScanFailed(std::vector<int> &requestIndexList);
    /**
     * @Description Callback function for obtaining the PNO scanning result
     *
     * @param scanInfoList - Scan Info List[in]
     */
    void HandlePnoScanInfo(std::vector<InterScanInfo> &scanInfoList);
    /**
     * @Description PNO scanning failed, Restart after a delay.
     *
     */
    void RestartPnoScanTimeOut();
    /**
     * @Description Determines whether external scanning is allowed based on the scanning policy.
     *
     * @return success: true, failed: false
     */
    ErrCode AllowExternScan();
    /**
     * @Description Determine whether to allow scheduled system scanning.
     *
     * @return success: true, failed: false
     */
    ErrCode AllowSystemTimerScan();
    /**
     * @Description Determines whether to allow PNO scanning based on the scanning policy.
     *
     * @return success: true, failed: false
     */
    ErrCode AllowPnoScan();
    /**
     * @Description Determines whether to allow scanning based on the scanning type..
     *
     * @param scanType - scan type: 0 - Extern; 1 - SystemTimer 2 Pno
     * @return true: allow, false: not allowed.
     */
    ErrCode AllowScanByType(ScanType scanType);
    /**
     * @Description Set the current mode to trust list mode.
     *
     */
    void SetScanTrustMode();
    /**
     * @Description Reset to non scan trust list mode.
     *
     */
    void ResetToNonTrustMode();
    /**
     * @Description Is it the trust list mode?
     *
     * @param sceneId - current scene id[out].
     * @return true: success, false: failed
     */
    bool IsScanTrustMode() const;
    /**
     * @Description Add the scene id to trust list.
     * Assume that the scene id of moving freeze is -1.
     *
     * @param sceneId - scene id.
     */
    void AddScanTrustSceneId(int sceneId);
    /**
     * @Description Clear trust list.
     *
     */
    void ClearScanTrustSceneIds();
    /**
     * @Description Is sceneId in trust list.
     *
     * @param sceneId - scene id.
     */
    bool IsInScanTrust(int sceneId) const;
    /**
     * @Description Set the moving freeze status by state.
     *
     * @param state - state value.
     */
    void SetMovingFreezeState(bool state);
    /**
     * @Description Is it the moving freeze state?
     *
    * @return true: success, false: failed
     */
    bool IsMovingFreezeState() const;
    /**
     * @Description Set the moving freeze state to scanned.
     *
     * @param scanned - scanned flag.
     */
    void SetMovingFreezeScaned(bool scanned);
    /**
     * @Description Whether scanned in moving freeze state.?
     *
     */
    bool IsMovingFreezeScaned() const;
    /**
     * @Description Apply trustlists scanning policies.
     *
     * @param scanType - scan type: 0 - Extern; 1 - SystemTimer 2 Pno
     * @return true: success, false: failed
     */
    ErrCode ApplyTrustListPolicy(ScanType scanType);
    /* *
     * @Description Apply trustlists and moving freeze scanning policies.
     *
     * @param scanType - type: 0 - Extern; 1 - SystemTimer 2 Pno
     * @return WIFI_OPT_SUCCESS: success,
     * WIFI_OPT_FAILED: general scan control fail,
     * WIFI_OPT_MOVING_FREEZE_CTRL: moving freeze scan control fail.
     */
    ErrCode ApplyScanPolices(ScanType type);
    /* *
     * @Description  Obtains the current screen.
     *
     * @return success: ScanScene, failed: SCAN_SCENE_MAX
     */
    int GetStaScene();
    /**
     * @Description Determine whether scanning is allowed and scan the control policy through forbidMap.
     *
     * @param staScene scan scene
     * @param scanMode scan mode
     * @return true - allow extern scan
     * @return false - not allow extern scan
     */
    bool AllowExternScanByForbid(int staScene, ScanMode scanMode);
    /**
     * @Description Determine whether scanning is allowed and scan the control policy through intervalMode.
     *
     * @param appId ID of the app to be scanned.
     * @param staScene scan scene
     * @param scanMode scan mode
     * @return true - allow extern scan
     * @return false - not allow extern scan
     */
    bool AllowExternScanByInterval(int appId, int staScene, ScanMode scanMode);
    /**
     * @Description Determines whether externally initiated scanning is being processed.
     *
     * @return success: true, failed: false
     */
    bool IsExternScanning() const;
    /**
     * @Description Indicates whether scanning with parameter.
     *
     * @return success: true, failed: false
     */
    bool IsScanningWithParam();
    /**
     * @Description Adjust the frequency band and frequency based on the scanning policy.
     *
     * @param scanBand - scan band[in]
     * @param freqs - scan frequency[in]
     */
    void GetAllowBandFreqsControlInfo(ScanBandType &scanBand, std::vector<int> &freqs);
    /**
     * @Description Do not adjust the frequency band when the 2.4 GHz frequency band is used.
     *
     * @param scanBand - scan band[in]
     * @return ScanBandType - scan band
     */
    ScanBandType ConvertBandNotAllow24G(ScanBandType scanBand);
    /**
     * @Description Do not adjust the frequency band when the 5 GHz frequency band is used.
     *
     * @param scanBand - scan band[in]
     * @return ScanBandType - scan band
     */
    ScanBandType ConvertBandNotAllow5G(ScanBandType scanBand);
    /**
     * @Description Delete the 2.4 GHz frequency from the frequency list.
     *
     * @param freqs - frequency[in]
     */
    void Delete24GhzFreqs(std::vector<int> &freqs);
    /**
     * @Description Delete the 5 GHz frequency from the frequency list.
     *
     * @param freqs - frequency list[in]
     */
    void Delete5GhzFreqs(std::vector<int> &freqs);
    /**
     * @Description Get the ssid of saved networks.
     *
     * @param savedNetworkSsid - ssid of saved networks[out]
     * @return success: true, failed: false
     */
    bool GetSavedNetworkSsidList(std::vector<std::string> &savedNetworkSsid);
    /**
     * @Description Get the ssid of saved hidden networks.
     *
     * @param hiddenNetworkSsid - ssid of hidden networks[out]
     * @return success: true, failed: false
     */
    bool GetHiddenNetworkSsidList(std::vector<std::string> &hiddenNetworkSsid);
    /**
     * @Description ScanForbidMap control mode specific implementation for specific scanning mode.
     *
     * @param isStaScene - Indicates whether the STA scenario is used.[in]
     * @param scanScene - scan scene[in][in]
     * @param scanMode - scan mode[in][in]
     * @return success: true, failed: false
     */
    bool AllowScanByForbidMap(int scanScene, ScanMode scanMode, time_t currentTime);
    /**
     * @Description Check whether the scan mode can be used during scanning under the forbid mode control.
     *
     * @param scanMode scan mode[in]
     * @return true - success
     * @return false  - failed
     */
    bool AllowScanDuringScanning(ScanMode scanMode) const;
    /**
     * @Description Check whether the scan mode can be used during screen off under the forbid mode control.
     *
     * @param scanMode [in]
     * @return true - success
     * @return false  - failed
     */
    bool AllowScanDuringScreenOff(ScanMode scanMode) const;
    /**
     * @Description
     *
     * @param staScene sta scan scene[in]
     * @param scanMode scan mode[in]
     * @return true - success
     * @return false  - failed
     */
    bool AllowScanDuringStaScene(int staScene, ScanMode scanMode);
    /**
     * @Description Check whether the scan mode can be used during custom scene off under the forbid mode control.
     *
     * @param scanMode [in]
     * @return true - success
     * @return false  - failed
     */
    bool AllowScanDuringCustomScene(ScanMode scanMode);
    /**
     * @Description Check whether the scan mode can be used under the interval mode control.
     *
     * @param appId App type for external requests to scan.[in]
     * @param scanScene scan scene[in]
     * @param scanMode scna mode[in]
     * @return true - success
     * @return false  - failed
     */
    bool AllowExternScanByIntervalMode(int appId, int scanScene, ScanMode scanMode);
    /**
     * @Description Check whether the scan mode can be used during custom scene under the interval mode control.
     *
     * @param appId App type for external requests to scan.[in]
     * @param scanMode scna mode[in]
     * @return true - success
     * @return false  - failed
     */
    bool AllowExternScanByCustomScene(int appId, ScanMode scanMode);
    /**
     * @Description Determines whether to allow system scan based on scanInterval control mode.
     *
     * @param expScanCount - Number of scan.[in]
     * @param interval - system scan interval[in]
     * @param count - Total number of times that the scanning interval is multiplied by 2[in]
     * @return success: true, failed: false
     */
    bool SystemScanByInterval(int &expScanCount, int &interval, int &count);
    /**
     * @Description Determines whether to allow pno scan based on scanInterval control mode.
     *
     * @param fixedScanCount pno scan count[in]
     * @param fixedScanTime pno scan time[in]
     * @param interval pno scan interval[in]
     * @param count pno scan max count[in]
     * @return true - success
     * @return false  - failed
     */
    bool PnoScanByInterval(int &fixedScanCount, time_t &fixedScanTime, int interval, int count);
    /**
     * @Description Determines whether to allow extern scan based on scanInterval control mode.
     *
     * @param appId ID of the app to be scanned[in]
     * @param singleAppForbid Stored External Scan Parameters[in]
     * @return true
     * @return false
     */
    bool ExternScanByInterval(int appId, SingleAppForbid &singleAppForbid);
    /**
     * @Description Determine whether external scanning of a single application can be distinguished.
     *
     * @param appId ID of the app to be scanned[in]
     * @param scanIntervalMode scan control policy parameters[in]
     * @return true
     * @return false
     */
    bool AllowSingleAppScanByInterval(int appId, ScanIntervalMode scanIntervalMode);
    /**
     * @Description Check whether external scanning is allowed regardless of a single application.
     *
     * @param appId ID of the app to be scanned[in]
     * @param scanIntervalMode intervalMode scan control policy parameters[in]
     * @return true
     * @return false
     */
    bool AllowFullAppScanByInterval(int appId, ScanIntervalMode scanIntervalMode);
    /**
     * @Description Determines whether external scanning is allowed in scanning control in INTERVAL_FIXED mode.
     *
     * @param fixedScanCount INTERVAL_FIXED intervalMode scan time[in]
     * @param fixedScanTime INTERVAL_FIXED intervalMode scan time[in]
     * @param interval scan interval[in]
     * @param count scan count[in]
     * @return true
     * @return false
     */
    bool AllowScanByIntervalFixed(int &fixedScanCount, time_t &fixedScanTime, int &interval, int &count);
    /**
     * @Description Determines whether external scanning is allowed in scanning control in INTERVAL_EXP mode.
     *
     * @param expScanCount INTERVAL_EXP intervalMode scan count[in]
     * @param interval scan interval[in]
     * @param count scan count[in]
     * @return true
     * @return false
     */
    bool AllowScanByIntervalExp(int &expScanCount, int &interval, int &count);
    /**
     * @Description Determines whether external scanning is allowed in scanning control in INTERVAL_CONTINUE mode.
     *
     * @param continueScanTime INTERVAL_CONTINUE intervalMode scan time[in]
     * @param lessThanIntervalCount INTERVAL_CONTINUE intervalMode scan count[in]
     * @param interval scan interval[in]
     * @param count scan count[in]
     * @return true
     * @return false
     */
    bool AllowScanByIntervalContinue(time_t &continueScanTime, int &lessThanIntervalCount, int &interval, int &count);
    /**
     * @Description Determines whether external scanning is allowed in scanning control in INTERVAL_BLOCKLIST mode.
     *
     * @param appId ID of the app to be scanned[in]
     * @param blockListScanTime INTERVAL_BLOCKLIST intervalMode scan time[in]
     * @param lessThanIntervalCount INTERVAL_BLOCKLIST intervalMode scan count[in]
     * @param interval scan interval[in]
     * @param count scan count[in]
     * @return true
     * @return false
     */
    bool AllowScanByIntervalBlocklist(
        int appId, time_t &blockListScanTime, int &lessThanIntervalCount, int &interval, int &count);

        /**
     * @Description Determines whether scanning is allowed by disable scan control.
     *
     * @return true: allow, false: not allowed.
     */
    bool AllowScanByDisableScanCtrl();

    /**
     * @Description Determines whether scanning is allowed in movingfreeze mode.
     *
     * @return true: allow, false: not allowed.
     */
    bool AllowScanByMovingFreeze();

    /**
     * @Description Is the app in the trustlist?
     *
     * @param trustList trustlist[in]
     * @param sceneId scene id[in]
     * @param appPackageName app package name[in]
     * @return true: in the trustlist, false: not in the trustlist.
     */
    bool IsPackageInTrustList(const std::string& trustList, int sceneId, const std::string &appPackageName) const;
    /* *
     * @Description all scan check at custom check.
     *
     * @param customIter custom iterator[in]
     * @param scanMode scene mode[in]
     * @return true: allow, false: not allowed.
     */
    bool AllowCustomSceneCheck(const std::map<int, time_t>::const_iterator &customIter, ScanMode scanMode);
    /* *
     * @Description all extern scan check at custom check.
     *
     * @param customIter custom iterator[in]
     * @param appId app id[in]
     * @param scanMode scene mode[in]
     * @return true: allow, false: not allowed.
     */
    bool AllowExternCustomSceneCheck(const std::map<int, time_t>::const_iterator &customIter, int appId,
        ScanMode scanMode);
};
}  // namespace Wifi
}  // namespace OHOS

#endif