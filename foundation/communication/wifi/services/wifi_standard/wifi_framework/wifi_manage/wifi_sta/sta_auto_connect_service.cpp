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
#include "wifi_logger.h"
#include "wifi_sta_hal_interface.h"
#include "wifi_settings.h"

DEFINE_WIFILOG_LABEL("StaAutoConnectService");

namespace OHOS {
namespace Wifi {
StaAutoConnectService::StaAutoConnectService(StaStateMachine *staStateMachine)
    : pStaStateMachine(staStateMachine),
      pSavedDeviceAppraisal(nullptr),
      firmwareRoamFlag(false),
      maxBlockedBssidNum(BLOCKLIST_INVALID_SIZE),
      selectDeviceLastTime(0),
      pAppraisals {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}
{}

StaAutoConnectService::~StaAutoConnectService()
{
    WIFI_LOGI("Enter StaAutoConnectService::~StaAutoConnectService.\n");
    if (pSavedDeviceAppraisal != nullptr) {
        delete pSavedDeviceAppraisal;
        pSavedDeviceAppraisal = nullptr;
    }
}

ErrCode StaAutoConnectService::InitAutoConnectService()
{
    WIFI_LOGI("Enter StaAutoConnectService::InitAutoConnectService.\n");

    if (ObtainRoamCapFromFirmware()) {
        WIFI_LOGI("Succeeded in obtaining firmware roaming information.\n");
    }
    SyncBlockedSsidFirmware();

    pSavedDeviceAppraisal = new (std::nothrow) StaSavedDeviceAppraisal(firmwareRoamFlag);
    if (pSavedDeviceAppraisal == nullptr) {
        WIFI_LOGE("savedDeviceAppraisal is null\n");
        return WIFI_OPT_FAILED;
    }

    int savedPriority = WifiSettings::GetInstance().GetSavedDeviceAppraisalPriority();
    if (RegisterDeviceAppraisal(pSavedDeviceAppraisal, savedPriority)) {
        WIFI_LOGI("RegisterSavedDeviceAppraisal succeeded.\n");
    }
    return WIFI_OPT_SUCCESS;
}

void StaAutoConnectService::OnScanInfosReadyHandler(const std::vector<InterScanInfo> &scanInfos)
{
    WIFI_LOGI("Enter StaAutoConnectService::OnScanInfosReadyHandler.\n");
    ClearOvertimeBlockedBssid(); /* Refreshing the BSSID Blocklist */

    WifiLinkedInfo info;
    WifiSettings::GetInstance().GetLinkedInfo(info);
    if (info.supplicantState == SupplicantState::ASSOCIATING ||
        info.supplicantState == SupplicantState::AUTHENTICATING ||
        info.supplicantState == SupplicantState::FOUR_WAY_HANDSHAKE ||
        info.supplicantState == SupplicantState::GROUP_HANDSHAKE) {
        WIFI_LOGE("Supplicant is under transient state.\n");
        return;
    }

    std::vector<std::string> blockedBssids;
    GetBlockedBssids(blockedBssids);

    WifiDeviceConfig electedDevice;
    if (AutoSelectDevice(electedDevice, scanInfos, blockedBssids, info) == WIFI_OPT_SUCCESS) {
        WIFI_LOGI("AutoSelectDevice succeed.\n");
        ConnectElectedDevice(electedDevice);
    } else {
        WIFI_LOGI("Exit network selection.\n");
        return;
    }
}

bool StaAutoConnectService::EnableOrDisableBssid(std::string bssid, bool enable, int reason)
{
    WIFI_LOGI("Enter StaAutoConnectService::EnableOrDisableBssid.\n");
    if (bssid.empty()) {
        WIFI_LOGI("bssid is empty.\n");
        return false;
    }

    /* Updating the BSSID Blocklist */
    if (!AddOrDelBlockedBssids(bssid, enable, reason)) {
        WIFI_LOGI("The blocklist is not updated.\n");
        return false;
    }

    /* The blocklist has been updated, so update the firmware roaming */
    /* configuration */
    SyncBlockedSsidFirmware();
    return true;
}

bool StaAutoConnectService::AddOrDelBlockedBssids(std::string bssid, bool enable, int reason)
{
    WIFI_LOGI("Enter StaAutoConnectService::AddOrDelBlockedBssids.\n");
    if (enable) {
        if (blockedBssidMap.count(bssid) != 0) {
            /* Removed the BSSID from the blocklist When the BSSID is enabled. */
            blockedBssidMap.erase(bssid);
            return true;
        }
        return false;
    }

    BlockedBssidInfo status;
    auto iter = blockedBssidMap.find(bssid);
    if (iter == blockedBssidMap.end()) {
        blockedBssidMap.emplace(bssid, status);
    }
    auto iterator = blockedBssidMap.find(bssid);
    if (iterator == blockedBssidMap.end()) {
        return false;
    }
    iterator->second.count++;
    time_t now = time(0);
    iterator->second.blockedTime = (int)now;
    if (!iterator->second.blockedFlag) {
        if (iterator->second.count >= MAX_BSSID_BLOCKLIST_COUNT ||
            reason == AP_CANNOT_HANDLE_NEW_STA) {
            iterator->second.blockedFlag = true;
            return true;
        }
    }
    return false;
}

void StaAutoConnectService::GetBlockedBssids(std::vector<std::string> &blockedBssids)
{
    WIFI_LOGI("Enter StaAutoConnectService::GetBlockedBssids.\n");

    for (auto iter = blockedBssidMap.begin(); iter != blockedBssidMap.end(); ++iter) {
        blockedBssids.push_back(iter->first);
    }
    return;
}

void StaAutoConnectService::ClearOvertimeBlockedBssid()
{
    WIFI_LOGI("Enter StaAutoConnectService::ClearOvertimeBlockedBssid.\n");
    if (blockedBssidMap.empty()) {
        return;
    }
    bool updated = false;
    auto iter = blockedBssidMap.begin();
    while (iter != blockedBssidMap.end()) {
        BlockedBssidInfo status = iter->second;
        time_t now = time(0);
        int currentTimeStap = (int)now;
        if (status.blockedFlag && ((currentTimeStap - status.blockedTime) >= MAX_BSSID_BLOCKLIST_TIME)) {
            blockedBssidMap.erase(iter++);
            updated = true;
        } else {
            ++iter;
        }
    }
    if (updated) {
        SyncBlockedSsidFirmware();
    }
    return;
}

void StaAutoConnectService::ConnectElectedDevice(WifiDeviceConfig &electedDevice)
{
    WIFI_LOGI("Enter StaAutoConnectService::ConnectElectedDevice.\n");
    if (electedDevice.bssid.empty()) {
        WIFI_LOGE("electedDevice is null.\n");
        return;
    }

    WifiLinkedInfo currentConnectedNetwork;
    WifiSettings::GetInstance().GetLinkedInfo(currentConnectedNetwork);
    if (currentConnectedNetwork.connState == ConnState::CONNECTED && electedDevice.networkId == INVALID_NETWORK_ID &&
        currentConnectedNetwork.ssid == electedDevice.ssid && currentConnectedNetwork.bssid != electedDevice.bssid) {
        /* Frameworks start roaming only when firmware is not supported */
        if (!firmwareRoamFlag) {
            WIFI_LOGI("Roaming connectTo.\n");
            pStaStateMachine->StartRoamToNetwork(electedDevice.bssid);
            WIFI_LOGI("connecTo network bssid is %s", electedDevice.bssid.c_str());
        }
    } else if (currentConnectedNetwork.detailedState == DetailedState::DISCONNECTED ||
        currentConnectedNetwork.detailedState == DetailedState::CONNECTION_TIMEOUT) {
        WIFI_LOGI("connecTo save network.\n");
        pStaStateMachine->SendMessage(WIFI_SVR_CMD_STA_CONNECT_SAVED_NETWORK,
            electedDevice.networkId,
            NETWORK_SELECTED_FOR_CONNECTION_MANAGEMENT);
        WIFI_LOGI("connecTo networkId is %{public}d", electedDevice.networkId);
        WIFI_LOGI("connecTo bssid is %s", electedDevice.bssid.c_str());
        WIFI_LOGI("connecTo preShareKey is %s", electedDevice.preSharedKey.c_str());
    } else {
        WIFI_LOGE("The current connection status is %{public}d.\n", currentConnectedNetwork.detailedState);
    }
    return;
}

void StaAutoConnectService::SyncBlockedSsidFirmware()
{
    WIFI_LOGI("Enter StaAutoConnectService::SyncBlockedSsidFirmware.\n");
    if (!firmwareRoamFlag) {
        return;
    }
    if (maxBlockedBssidNum <= 0) {
        return;
    }
    std::vector<std::string> blockedBssids;
    GetBlockedBssids(blockedBssids);

    if (static_cast<int>(blockedBssids.size()) > maxBlockedBssidNum) {
        blockedBssids.resize(maxBlockedBssidNum);
    }

    if (SetRoamBlockedBssidFirmware(blockedBssids)) {
        WIFI_LOGE("Set firmware roaming configuration succeeded.\n");
    } else {
        WIFI_LOGI("Set firmware roaming configuration failed.\n");
    }
    return;
}

bool StaAutoConnectService::ObtainRoamCapFromFirmware()
{
    WIFI_LOGI("Enter StaAutoConnectService::ObtainRoamCapFromFirmware.\n");

    unsigned int capabilities;
    if (WifiStaHalInterface::GetInstance().GetStaCapabilities(capabilities) == WIFI_IDL_OPT_OK) {
        if ((capabilities & STA_CAP_ROAMING) == 0) {
            WIFI_LOGE("Firmware roaming is not supported.\n");
            return false;
        }
    }

    WifiIdlRoamCapability capability;
    if (WifiStaHalInterface::GetInstance().GetRoamingCapabilities(capability) == WIFI_IDL_OPT_OK) {
        if (capability.maxBlocklistSize > 0) {
            firmwareRoamFlag = true;
            maxBlockedBssidNum = capability.maxBlocklistSize;
            WIFI_LOGI("Get firmware roaming capabilities succeeded.\n");
            return true;
        }
        WIFI_LOGE("Invalid firmware roaming capabilities.\n");
    }

    WIFI_LOGE("Get firmware roaming capabilities failed.\n");
    return false;
}

bool StaAutoConnectService::SetRoamBlockedBssidFirmware(const std::vector<std::string> &blocklistBssids) const
{
    WIFI_LOGI("Enter StaAutoConnectService::SetRoamBlockedBssidFirmware.\n");
    if (!firmwareRoamFlag) {
        return false;
    }

    if (blocklistBssids.empty()) {
        return false;
    }

    if (static_cast<int>(blocklistBssids.size()) > maxBlockedBssidNum) {
        return false;
    }

    WifiIdlRoamConfig capability;
    capability.blocklistBssids = blocklistBssids;
    if (WifiStaHalInterface::GetInstance().SetRoamConfig(capability) == WIFI_IDL_OPT_OK) {
        return true;
    }
    return false;
}

bool StaAutoConnectService::RegisterDeviceAppraisal(StaDeviceAppraisal *appraisal, int priority)
{
    WIFI_LOGI("Enter StaAutoConnectService::RegisterDeviceAppraisal.\n");
    if (priority < 0 || priority >= MIN_APPRAISAL_PRIORITY) {
        WIFI_LOGE("Out of array range.\n");
        return false;
    }
    if (pAppraisals[priority] != nullptr) {
        WIFI_LOGE("Appraisals is not empty.\n");
        return false;
    }
    pAppraisals[priority] = appraisal;
    return true;
}

ErrCode StaAutoConnectService::AutoSelectDevice(WifiDeviceConfig &electedDevice,
    const std::vector<InterScanInfo> &scanInfos, std::vector<std::string> &blockedBssids, WifiLinkedInfo &info)
{
    WIFI_LOGI("Enter StaAutoConnectService::SelectNetwork.\n");
    if (scanInfos.empty()) {
        WIFI_LOGE("No scanInfo.\n");
        return WIFI_OPT_FAILED;
    }

    /* Whether network selection handover is required */
    if (!AllowAutoSelectDevice(scanInfos, info)) {
        WIFI_LOGE("Network switching is not required.\n");
        return WIFI_OPT_FAILED;
    }

    std::vector<InterScanInfo> availableScanInfos;
    /* Filter out unnecessary networks. */
    GetAvailableScanInfos(availableScanInfos, scanInfos, blockedBssids, info);
    if (availableScanInfos.empty()) {
        WIFI_LOGE("No scanInfo available.\n");
        return WIFI_OPT_FAILED;
    }
    /*
     * Check the registered network appraisal from highest priority to lowest
     * priority until the selected network
     */
    for (auto registeredAppraisal : pAppraisals) {
        if (registeredAppraisal != nullptr) {
            ErrCode code = registeredAppraisal->DeviceAppraisals(electedDevice, availableScanInfos, info);
            if (code == WIFI_OPT_SUCCESS) {
                time_t now = time(0);
                selectDeviceLastTime = static_cast<int>(now);
                WIFI_LOGI("electedDevice generation.\n");
                return WIFI_OPT_SUCCESS;
            }
        }
    }

    if (RoamingSelection(electedDevice, availableScanInfos, info)) {
        WIFI_LOGI("Roaming network generation.\n");
        return WIFI_OPT_SUCCESS;
    }
    WIFI_LOGE("No electedDevice.\n");
    return WIFI_OPT_FAILED;
}

bool StaAutoConnectService::RoamingSelection(
    WifiDeviceConfig &electedDevice, std::vector<InterScanInfo> &availableScanInfos, WifiLinkedInfo &info)
{
    for (auto scanInfo : availableScanInfos) {
        if (info.connState == ConnState::CONNECTED && scanInfo.ssid == info.ssid && scanInfo.bssid != info.bssid) {
            WIFI_LOGI("Discover roaming networks.\n");
            if (RoamingEncryptionModeCheck(electedDevice, scanInfo, info)) {
                return true;
            }
        }
    }
    return false;
}

bool StaAutoConnectService::RoamingEncryptionModeCheck(
    WifiDeviceConfig &electedDevice, InterScanInfo scanInfo, WifiLinkedInfo &info)
{
    WifiDeviceConfig network;
    if (WifiSettings::GetInstance().GetDeviceConfig(scanInfo.ssid, DEVICE_CONFIG_INDEX_SSID, network) == 0) {
        std::string mgmt = scanInfo.capabilities;
        if (mgmt.find("WPA-PSK") != std::string::npos || mgmt.find("WPA2-PSK") != std::string::npos) {
            mgmt = "WPA-PSK";
        } else if (mgmt.find("EAP") != std::string::npos) {
            mgmt = "WPA-EAP";
        } else if (mgmt.find("SAE") != std::string::npos) {
            mgmt = "SAE";
        } else {
            if (mgmt.find("WEP") != std::string::npos && network.wepTxKeyIndex == 0) {
                WIFI_LOGE("The roaming network is a WEP network, but the connected network is not a WEP network.\n");
                return false;
            } else if (mgmt.find("WEP") == std::string::npos && network.wepTxKeyIndex != 0) {
                WIFI_LOGE("The connected network is a WEP network, but the roaming network is not a WEP network.\n");
                return false;
            }
            mgmt = "NONE";
        }
        if (mgmt == network.keyMgmt) {
            WIFI_LOGI("The Current network bssid %s signal strength is %{public}d", info.bssid.c_str(), info.rssi);
            WIFI_LOGI(
                "The Roaming network bssid %s signal strength is %{public}d", scanInfo.bssid.c_str(), scanInfo.rssi);
            int rssi = scanInfo.rssi - info.rssi;
            if (rssi > MIN_ROAM_RSSI_DIFF) {
                WIFI_LOGI("Roming network rssi - Current network rssi > 6.");
                electedDevice.ssid = scanInfo.ssid;
                electedDevice.bssid = scanInfo.bssid;
                return true;
            } else {
                WIFI_LOGE("Roming network rssi - Current network rssi < 6.");
            }
        } else {
            WIFI_LOGE("The encryption mode does not match.\n");
        }
    }
    return false;
}

bool StaAutoConnectService::AllowAutoSelectDevice(const std::vector<InterScanInfo> &scanInfos, WifiLinkedInfo &info)
{
    WIFI_LOGI("Allow auto select device, detailed status is %{public}d\n", info.detailedState);
    if (scanInfos.empty()) {
        WIFI_LOGE("No network,skip network selection.\n");
        return false;
    }

    switch (info.detailedState) {
        case DetailedState::WORKING:
            /* Configure whether to automatically switch the network. */
            if (!WifiSettings::GetInstance().GetWhetherToAllowNetworkSwitchover()) {
                WIFI_LOGE("Automatic network switching is not allowed in user configuration.\n");
                return false;
            }
            /* Indicates whether the minimum interval is the minimum interval since the last network selection. */
            if (selectDeviceLastTime != 0) {
                int gap = static_cast<int>(time(0)) - selectDeviceLastTime;
                if (gap < MIN_SELECT_NETWORK_TIME) {
                    WIFI_LOGE("%ds time before we selected the network(30s).\n", gap);
                    return false;
                }
            }

            if (!CurrentDeviceGoodEnough(scanInfos, info)) {
                WIFI_LOGI("The current network is insuffice.\n");
                return true;
            }
            return false;

        case DetailedState::DISCONNECTED:
        case DetailedState::CONNECTION_TIMEOUT:
            WIFI_LOGI("The current connection status is disconnected or timeout.\n");
            return true;

        case DetailedState::NOTWORKING:
            WIFI_LOGI("The current network cannot access the Internet.\n");
            /* Configure whether to automatically switch the network. */
            if (!WifiSettings::GetInstance().GetWhetherToAllowNetworkSwitchover()) {
                WIFI_LOGE("Automatic network switching is not allowed in user configuration.\n");
                return false;
            }
            return true;

        default:
            return false;
    }
    return false;
}

bool StaAutoConnectService::CurrentDeviceGoodEnough(const std::vector<InterScanInfo> &scanInfos, WifiLinkedInfo &info)
{
    WIFI_LOGI("Enter StaAutoConnectService::CurrentDeviceGoodEnough.\n");

    WifiDeviceConfig network;

    /* The network is deleted */
    if (WifiSettings::GetInstance().GetDeviceConfig(info.networkId, network) == -1) {
        WIFI_LOGE("The network is deleted.\n");
        return false;
    }

    int userLastSelectedNetworkId = WifiSettings::GetInstance().GetUserLastSelectedNetworkId();
    if (userLastSelectedNetworkId != INVALID_NETWORK_ID && userLastSelectedNetworkId == network.networkId) {
        time_t userLastSelectedNetworkTimeVal = WifiSettings::GetInstance().GetUserLastSelectedNetworkTimeVal();
        time_t now = time(0);
        int interval = static_cast<int>(now - userLastSelectedNetworkTimeVal);
        if (interval <= TIME_FROM_LAST_SELECTION) {
            WIFI_LOGI("(60s)Current user recent selections time is %ds.\n", interval);
            return true;
        }
    }

    /* Temporary network unqualified */
    if (network.isEphemeral) {
        WIFI_LOGE("The network is isEphemeral.\n");
        return false;
    }

    if (network.keyMgmt == "NONE" || network.keyMgmt.size() == 0) {
        WIFI_LOGE("This network No keyMgmt.\n");
        return false;
    }

    /* The signal strength on the live network does not meet requirements. */
    if (info.rssi < RSSI_DELIMITING_VALUE) {
        WIFI_LOGE("Signal strength insuffice %{public}d < -65.\n", info.rssi);
        return false;
    }
    /*
     * The network is a 2.4 GHz network and is not qualified when the 5G network
     * is available.
     */
    if (Whether24GDevice(info.frequency)) {
        if (WhetherDevice5GAvailable(scanInfos)) {
            WIFI_LOGE("5 GHz is available when the current frequency band is 2.4 GHz.\n");
            return false;
        }
    }
    return true;
}

bool StaAutoConnectService::WhetherDevice5GAvailable(const std::vector<InterScanInfo> &scanInfos)
{
    WIFI_LOGI("Enter StaAutoConnectService::WhetherDevice5GAvailable.\n");
    for (auto scaninfo : scanInfos) {
        if (Whether5GDevice(scaninfo.frequency)) {
            return true;
        }
    }
    return false;
}

bool StaAutoConnectService::Whether24GDevice(int frequency)
{
    if (frequency > MIN_24_FREQUENCY && frequency < MAX_24_FREQUENCY) {
        return true;
    } else {
        return false;
    }
}

bool StaAutoConnectService::Whether5GDevice(int frequency)
{
    if (frequency > MIN_5_FREQUENCY && frequency < MAX_5_FREQUENCY) {
        return true;
    } else {
        return false;
    }
}

void StaAutoConnectService::GetAvailableScanInfos(std::vector<InterScanInfo> &availableScanInfos,
    const std::vector<InterScanInfo> &scanInfos, std::vector<std::string> &blockedBssids, WifiLinkedInfo &info)
{
    WIFI_LOGI("Enter StaAutoConnectService::GetAvailableScanInfos.\n");
    if (scanInfos.empty()) {
        return;
    }
    bool scanInfosContainCurrentBssid = false;

    for (auto scanInfo : scanInfos) {
        if (scanInfo.ssid.size() == 0) {
            continue;
        }

        /* Check whether the scanning result contains the current BSSID. */
        if (info.connState == ConnState::CONNECTED && scanInfo.bssid == info.bssid) {
            scanInfosContainCurrentBssid = true;
        }

        auto itr = find(blockedBssids.begin(), blockedBssids.end(), scanInfo.bssid);
        if (itr != blockedBssids.end()) { /* Skip Blocklist Network */
            WIFI_LOGI("Skip blocklistedBssid network, ssid: %{public}s.\n", scanInfo.ssid.c_str());
            continue;
        }

        /* Skipping networks with weak signals */
        if (scanInfo.frequency < MIN_5GHZ_BAND_FREQUENCY) {
            if (scanInfo.rssi <= MIN_RSSI_VALUE_24G) {
                WIFI_LOGI("Skip network %{public}s with low 2.4G signals %{public}d.\n",
                    scanInfo.ssid.c_str(), scanInfo.rssi);
                continue;
            }
        } else {
            if (scanInfo.rssi <= MIN_RSSI_VALUE_5G) {
                WIFI_LOGI("Skip network %{public}s with low 5G signals %{public}d.\n",
                    scanInfo.ssid.c_str(), scanInfo.rssi);
                continue;
            }
        }
        availableScanInfos.push_back(scanInfo);
    }
    /*
     * Some scan requests may not include channels for the currently connected
     * network, so the currently connected network will not appear in the scan
     * results. We will not act on these scans to avoid network switching that may
     * trigger disconnections.
     */
    if (info.connState == ConnState::CONNECTED && !scanInfosContainCurrentBssid) {
        WIFI_LOGI("scanInfo is be cleared.\n");
        availableScanInfos.clear();
    }
    return;
}
}  // namespace Wifi
}  // namespace OHOS