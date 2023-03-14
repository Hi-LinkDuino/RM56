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

#ifndef OHOS_WIFI_AUTO_CONNECT_SERVICE_H
#define OHOS_WIFI_AUTO_CONNECT_SERVICE_H

#include <ctime>
#include <string>
#include <unordered_map>
#include <vector>
#include "wifi_log.h"
#include "wifi_errcode.h"
#include "wifi_msg.h"
#include "log_helper.h"
#include "sta_define.h"
#include "sta_state_machine.h"
#include "sta_saved_device_appraisal.h"

namespace OHOS {
namespace Wifi {
static const int MAX_BSSID_BLOCKLIST_COUNT = 3;
static const int AP_CANNOT_HANDLE_NEW_STA = 17;
static const int MAX_BSSID_BLOCKLIST_TIME = 60; // 60s
static const int BLOCKLIST_INVALID_SIZE = -1;
static const int STA_CAP_ROAMING = 0x800000;
static const int MIN_APPRAISAL_PRIORITY = 6;
static const int MAX_APPRAISAL_NUM = 6;
static const int MIN_SELECT_NETWORK_TIME = 30;
static const int MIN_5GHZ_BAND_FREQUENCY = 5000;
static const int RSSI_DELIMITING_VALUE = -65;
static const int MIN_RSSI_VALUE_24G = -77;
static const int MIN_RSSI_VALUE_5G = -80;
static const int TIME_FROM_LAST_SELECTION = 60;
static const int MIN_ROAM_RSSI_DIFF = 6;
class StaAutoConnectService {
    FRIEND_GTEST(StaAutoConnectService);
public:
    explicit StaAutoConnectService(StaStateMachine *staStateMachine);
    virtual ~StaAutoConnectService();
    /**
     * @Description  Initialize StaAutoConnectService
     *
     */
    virtual ErrCode InitAutoConnectService();
    /**
     * @Description  Processing scan results
     *
     * @param scanInfos - The list of scanning results(in)
     */
    virtual void OnScanInfosReadyHandler(const std::vector<InterScanInfo> &scanInfos);
    /**
     * @Description  Whether tracking should enable or disable scanned BSSIDs
     *
     * @param bssid - BSSID to be enabled/disabled(in)
     * @param enable - true: Enable the BSSID. false: disable the BSSID.(in)
     * @param reason - Enable/Disable reason code.(in)
     * @Return success: true. failed： false.
     */
    virtual bool EnableOrDisableBssid(std::string bssid, bool enable, int reason);
    /**
     * @Description  Select the best device from the range.
     *
     * @param scanInfos - WifiScanInfo list of all APs in the range(in)
     * @param blockedBssids - Blocklisted BSSID List(in)
     * @param info - Current Connected Device(in)
     * @param electedDevice - Elected Device(out)
     * @Return success : WIFI_OPT_SUCCESS  failed : WIFI_OPT_FAILED
     */
    virtual ErrCode AutoSelectDevice(WifiDeviceConfig &electedDevice, const std::vector<InterScanInfo> &scanInfos,
        std::vector<std::string> &blockedBssids, WifiLinkedInfo &info);
    /**
     * @Description  Registering the Device Appraisal
     *
     * @param appraisal - Device appraisal to be registered(in)
     * @param priority - Value between 0 and (SCORER_MIN_PRIORITY – 1)(in)
     * @Return success : true  failed : false
     */
    virtual bool RegisterDeviceAppraisal(StaDeviceAppraisal *appraisal, int priority);

private:
    StaStateMachine *pStaStateMachine;
    StaDeviceAppraisal *pSavedDeviceAppraisal;
    bool firmwareRoamFlag;
    int maxBlockedBssidNum;
    int selectDeviceLastTime;
    StaDeviceAppraisal *pAppraisals[MAX_APPRAISAL_NUM];
    struct BlockedBssidInfo {
        int count; /* Number of times the BSSID is rejected. */
        bool blockedFlag;
        int blockedTime;
        BlockedBssidInfo()
        {
            count = 0;
            blockedFlag = false;
            blockedTime = 0;
        }
        ~BlockedBssidInfo(){}
    };
    std::unordered_map<std::string, BlockedBssidInfo> blockedBssidMap;
    /**
     * @Description  Refreshing the BSSID Blocklist
     *
     */
    void ClearOvertimeBlockedBssid();
    /**
     * @Description  Compiles and returns the hash set of the blocklist BSSID.
     *
     * @param blockedBssids - Blocklisted BSSID List(out)
     */
    void GetBlockedBssids(std::vector<std::string> &blockedBssids);
    /**
     * @Description  Update the BSSID blocklist when the BSSID is enabled or disabled.
     *
     * @param bssid - BSSID to be enabled/disabled(in)
     * @param enable - true: Enable the BSSID. false: disable the BSSID.(in)
     * @param reasonCode - Enable/Disable reason code.(in)
     * @Return: If the blocklist is updated, The value is true. Otherwise, the value is false.
     */
    bool AddOrDelBlockedBssids(std::string bssid, bool enable, int reason);
    /**
     * @Description  If the firmware roaming function is supported,
                     update the firmware roaming config.
     *
     */
    void SyncBlockedSsidFirmware();
    /**
     * @Description  Querying Firmware Information
     *
     * @Return: If the operation is successful, true is returned.
                If firmware roaming is supported but the valid roaming
                capability cannot be obtained, false is returned.
     */
    bool ObtainRoamCapFromFirmware();
    /**
     * @Description  Write Firmware Roaming Configuration to Firmware
     *
     * @param blocklistBssids - List of BSSIDs to Be Added to the Blocklist(in)
     * @Return: True if successful, false otherwise
     */
    bool SetRoamBlockedBssidFirmware(const std::vector<std::string> &blocklistBssids) const;
    /**
     * @Description  Connect to an elected device
     *
     * @param electedDevice - Elected Device(in)
     */
    void ConnectElectedDevice(WifiDeviceConfig &electedDevice);
    /**
     * @Description  Get available device
     *
     * @param scanInfos - WifiScanInfo list of all APs in the range(in)
     * @param blockedBssids - Blocklisted BSSID List(in)
     * @param info - Current Connected Device(in)
     * @param availableScanInfos - Available Scan Info(out)
     */
    void GetAvailableScanInfos(std::vector<InterScanInfo> &availableScanInfos,
        const std::vector<InterScanInfo> &scanInfos, std::vector<std::string> &blockedBssids, WifiLinkedInfo &info);
    /**
     * @Description  Whether the device needs to be switched.
     *
     * @param scanInfos - WifiScanInfo list of all APs in the range(in)
     * @param info - Current Connected Device(in)
     * @Return success : true  failed : false
     */
    bool AllowAutoSelectDevice(const std::vector<InterScanInfo> &scanInfos, WifiLinkedInfo &info);
    /**
     * @Description  Whether the device strength is sufficient.
     *
     * @param scanInfos - WifiScanInfo list of all APs in the range(in)
     * @param info - Current Connected Device(in)
     * @Return success : true  failed : false
     */
    bool CurrentDeviceGoodEnough(const std::vector<InterScanInfo> &scanInfos, WifiLinkedInfo &info);
    /**
     * @Description  Whether 5G devices are available.
     *
     * @param scanInfos - WifiScanInfo list of all APs in the range(in)
     * @Return success : true  failed : false
     */
    bool WhetherDevice5GAvailable(const std::vector<InterScanInfo> &scanInfos);
    /**
     * @Description  Select Roaming Device.
     *
     * @param availableScanInfos - Available device(in)
     * @param scanInfos - WifiScanInfo list of all APs in the range(in)
     * @param info - Current Connected Device(in)
     * @param electedDevice - Elected Device(out)
     * @Return success : true  failed : false
     */
    bool RoamingSelection(
        WifiDeviceConfig &electedDevice, std::vector<InterScanInfo> &availableScanInfos, WifiLinkedInfo &info);
    /**
     * @Description  Select Roaming Device.
     *
     * @param WifiScanInfo - A scan result(in)
     * @param info - Current Connected Device(in)
     * @param electedDevice - Elected Device(out)
     * @Return success : true  failed : false
     */
    bool RoamingEncryptionModeCheck(WifiDeviceConfig &electedDevice, InterScanInfo scanInfo, WifiLinkedInfo &info);
    /**
     * @Description  Whether the device is a 2.4G device.
     *
     * @param frequency(in)
     * @Return success : true  failed : false
     */
    bool Whether24GDevice(int frequency);
    /**
     * @Description  Whether the device is a 5G device.
     *
     * @param frequency(in)
     * @Return success : true  failed : false
     */
    bool Whether5GDevice(int frequency);
};
}  // namespace Wifi
}  // namespace OHOS
#endif