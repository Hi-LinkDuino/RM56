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
#include "wifi_logger.h"
#include "wifi_settings.h"

DEFINE_WIFILOG_LABEL("StaSavedDeviceAppraisal");

#define BINARY_CODE 0001
#define LAST_SELECTION_SCORE_DECAY_S 300
#define MAX(A, B) (((A) >= (B)) ? (A) : (B))
namespace OHOS {
namespace Wifi {
StaSavedDeviceAppraisal::StaSavedDeviceAppraisal(bool supportFmRoamingFlag)
    : signalScorePerLevel(WifiSettings::GetInstance().GetScoretacticsScoreSlope()),
      signalBaseScore(WifiSettings::GetInstance().GetScoretacticsInitScore()),
      sameBssidScore(WifiSettings::GetInstance().GetScoretacticsSameBssidScore()),
      sameDeviceScore(WifiSettings::GetInstance().GetScoretacticsSameNetworkScore()),
      frequency5GHzScore(WifiSettings::GetInstance().GetScoretacticsFrequency5GHzScore()),
      userSelectedDeviceScore(WifiSettings::GetInstance().GetScoretacticsLastSelectionScore()),
      safetyDeviceScore(WifiSettings::GetInstance().GetScoretacticsSecurityScore()),
      firmwareRoamFlag(supportFmRoamingFlag)
{}
StaSavedDeviceAppraisal::~StaSavedDeviceAppraisal()
{
    WIFI_LOGI("Enter StaSavedDeviceAppraisal::~StaSavedDeviceAppraisal.\n");
}

ErrCode StaSavedDeviceAppraisal::DeviceAppraisals(
    WifiDeviceConfig &electedDevice, std::vector<InterScanInfo> &scanInfos, WifiLinkedInfo &info)
{
    WIFI_LOGI("Enter StaSavedDeviceAppraisal::DeviceAppraisals.\n");
    int highestScore = 0;
    int sign = 0;
    InterScanInfo scanInfoElected;
    scanInfoElected.rssi = VALUE_LIMIT_MIN_RSSI;

    for (auto scanInfo : scanInfos) {
        if (scanInfo.bssid.size() == 0) {
            continue;
        }

        WifiDeviceConfig device;
        if (WifiSettings::GetInstance().GetDeviceConfig(scanInfo.bssid, DEVICE_CONFIG_INDEX_BSSID, device) == -1) {
            WIFI_LOGI("Skip unsaved Network %{public}s.", scanInfo.ssid.c_str()); /* Skipping Unsaved Networks */
            continue;
        }

        if (WhetherSkipDevice(device)) {
            continue;
        }

        int score = 0;
        AppraiseDeviceQuality(score, scanInfo, device, info);
        WIFI_LOGI("The device %{public}s score is %{public}d.rssi is %{public}d.\n",
            scanInfo.ssid.c_str(), score, scanInfo.rssi);

        if (score > highestScore || (score == highestScore && scanInfo.rssi > scanInfoElected.rssi)) {
            highestScore = score;
            scanInfoElected.rssi = scanInfo.rssi;
            electedDevice = device;
            sign = 1;
        } else {
            WIFI_LOGI("The config %{public}s is ignored!\n", scanInfo.ssid.c_str());
        }
    }
    if (sign == 1) {
        if (info.connState == ConnState::CONNECTED && electedDevice.networkId == info.networkId) {
            WifiDeviceConfig networkInfo;
            electedDevice = networkInfo;
            WIFI_LOGI("The electedDevice is the current connected device. Skip the device selection.");
            return WIFI_OPT_FAILED;
        } else {
            WIFI_LOGI("The device is selected successfully.\n");
            return WIFI_OPT_SUCCESS;
        }
    } else {
        WIFI_LOGI("Skip all scan results.\n");
        return WIFI_OPT_FAILED;
    }
}

bool StaSavedDeviceAppraisal::WhetherSkipDevice(WifiDeviceConfig &device)
{
    /* Skip this type of device and evaluate it by other appraisals */
    if (device.isPasspoint || device.isEphemeral) {
        WIFI_LOGI("Skip isPasspoint or isEphemeral Network %{public}s.", device.ssid.c_str());
        return true;
    }

    if (device.status == static_cast<int>(WifiDeviceConfigStatus::DISABLED)) {
        WIFI_LOGI("Skip disabled Network %{public}s.", device.ssid.c_str());
        return true;
    }
    std::string bssid = WifiSettings::GetInstance().GetConnectTimeoutBssid();
    if (!bssid.empty() && bssid == device.bssid) {
        WIFI_LOGI("Skip the connect timeout Network %{public}s.", device.ssid.c_str());
        return true;
    }
    return false;
}

void StaSavedDeviceAppraisal::AppraiseDeviceQuality(
    int &score, InterScanInfo &scanInfo, WifiDeviceConfig &device, WifiLinkedInfo &info)
{
    WIFI_LOGI("Enter StaSavedDeviceAppraisal::AppraiseDeviceQuality.\n");
    int rssi = scanInfo.rssi;
    /* Converts a signal to a grid number */
    int signalStrength = CalculateSignalBars(rssi, MAX_SIGNAL_BAR_NUM);
    /* Signal strength score */
    score += signalBaseScore + signalStrength * signalScorePerLevel;
    WIFI_LOGI("signalstrength score is %{public}d.\n", score);

    /* 5 GHz frequency band: bonus point */
    if (Whether5GDevice(scanInfo.frequency)) {
        score += frequency5GHzScore;
        WIFI_LOGI("5G score is %{public}d.\n", frequency5GHzScore);
    }

    /* Bonus points for last user selection */
    int userLastSelectedNetworkId = WifiSettings::GetInstance().GetUserLastSelectedNetworkId();
    if (userLastSelectedNetworkId != INVALID_NETWORK_ID && userLastSelectedNetworkId == device.networkId) {
        time_t userLastSelectedNetworkTimeVal = WifiSettings::GetInstance().GetUserLastSelectedNetworkTimeVal();
        time_t now = time(0);
        time_t timeDifference = now - userLastSelectedNetworkTimeVal;
        /*
         * Basic score of the device selected by the user: 120.
         * One point is deducted from every 5 points since the time when the user
         * selects the device.
         */
        if (timeDifference > 0) {
            int decay = static_cast<int>(timeDifference / LAST_SELECTION_SCORE_DECAY_S);
            int bonus = MAX((userSelectedDeviceScore - decay), (0));
            score += bonus;
            WIFI_LOGI("lastselected score is %{public}d.\n", bonus);
        }
    }
    /*
     * If the current device is the same as the elected device, bonus points
     * are added.
     */
    if (info.detailedState == DetailedState::WORKING && scanInfo.ssid == info.ssid) {
        score += sameDeviceScore;
        WIFI_LOGI("samenetwork score is %{public}d.\n", sameDeviceScore);
        /*
         * When firmware roaming is supported, the same BSSID is added to different
         * BSSIDs.
         */
        if (firmwareRoamFlag && scanInfo.bssid != info.bssid) {
            score += sameBssidScore;
            WIFI_LOGI("roamingsupport score is %{public}d.\n", sameBssidScore);
        }
    }

    if (info.detailedState == DetailedState::WORKING && info.bssid == scanInfo.bssid) {
        score += sameBssidScore;
        WIFI_LOGI("SameBssid score is %{public}d.\n", sameBssidScore);
    }

    if (device.keyMgmt != "NONE" && device.keyMgmt.size() != 0) {
        score += safetyDeviceScore;
        WIFI_LOGI("security score is %{public}d.\n", safetyDeviceScore);
    }
}

bool StaSavedDeviceAppraisal::Whether5GDevice(int frequency)
{
    if (frequency > MIN_5_FREQUENCY && frequency < MAX_5_FREQUENCY) {
        return true;
    } else {
        return false;
    }
}

int StaSavedDeviceAppraisal::CalculateSignalBars(int rssi, int signalBars)
{
    WIFI_LOGI("Enter StaSavedDeviceAppraisal CalculateSignalBars");
    if (rssi <= VALUE_LIMIT_MIN_RSSI) {
        return 0;
    } else if (rssi >= VALUE_LIMIT_MAX_RSSI) {
        return signalBars - 1;
    } else {
        float inputRange = (VALUE_LIMIT_MAX_RSSI - VALUE_LIMIT_MIN_RSSI);
        float outputRange = (signalBars - 1);
        return static_cast<int>(static_cast<float>(rssi - VALUE_LIMIT_MIN_RSSI) * outputRange / inputRange);
    }
}
}  // namespace Wifi
}  // namespace OHOS