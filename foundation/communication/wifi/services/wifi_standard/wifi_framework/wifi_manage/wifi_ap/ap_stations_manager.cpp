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
#include "ap_stations_manager.h"
#include <unistd.h>
#include "ap_service.h"
#include "wifi_log.h"
#include "wifi_settings.h"
#include "wifi_ap_hal_interface.h"
#include "ap_state_machine.h"
#include "wifi_logger.h"

DEFINE_WIFILOG_HOTSPOT_LABEL("WifiApStationsManager");

namespace OHOS {
namespace Wifi {
ApStationsManager::ApStationsManager()
{}

ApStationsManager::~ApStationsManager()
{}

bool ApStationsManager::AddBlockList(const StationInfo &staInfo) const
{
    if (WifiApHalInterface::GetInstance().AddBlockByMac(staInfo.bssid) != WifiErrorNo::WIFI_IDL_OPT_OK) {
        return false;
    }
    return true;
}

bool ApStationsManager::DelBlockList(const StationInfo &staInfo) const
{
    if (WifiApHalInterface::GetInstance().DelBlockByMac(staInfo.bssid) != WifiErrorNo::WIFI_IDL_OPT_OK) {
        return false;
    }
    return true;
}

bool ApStationsManager::AddAssociationStation(const StationInfo &staInfo) const
{
    if (WifiSettings::GetInstance().ManageStation(staInfo, MODE_ADD)) {
        return false;
    }
    return true;
}

bool ApStationsManager::DelAssociationStation(const StationInfo &staInfo) const
{
    if (WifiSettings::GetInstance().ManageStation(staInfo, MODE_DEL)) {
        return false;
    }
    return true;
}

bool ApStationsManager::EnableAllBlockList() const
{
    std::vector<StationInfo> results;
    if (WifiSettings::GetInstance().GetBlockList(results)) {
        WIFI_LOGE("failed to get blocklist.");
        return false;
    }
    std::string mac;
    bool ret = true;
    for (std::vector<StationInfo>::iterator iter = results.begin(); iter != results.end(); iter++) {
        if (WifiApHalInterface::GetInstance().AddBlockByMac(iter->bssid) != WifiErrorNo::WIFI_IDL_OPT_OK) {
            WIFI_LOGE("error:Failed to add block bssid is:%{private}s.", iter->bssid.c_str());
            ret = false;
        }
    }
    return ret;
}

void ApStationsManager::StationLeave(const std::string &mac) const
{
    StationInfo staInfo;
    std::vector<StationInfo> results;
    if (WifiSettings::GetInstance().GetStationList(results)) {
        WIFI_LOGE("failed to GetStationList.");
        return;
    }
    auto it = results.begin();
    for (; it != results.end(); ++it) {
        if (it->bssid == mac) {
            staInfo = *it;
            if (!DelAssociationStation(staInfo)) {
                WIFI_LOGE("DelAssociationStation failed.");
                return;
            }
            break;
        }
    }
    if (m_stationChangeCallback) {
        m_stationChangeCallback(staInfo, ApStatemachineEvent::CMD_STATION_LEAVE);
    }
    return;
}

void ApStationsManager::StationJoin(const StationInfo &staInfo) const
{
    StationInfo staInfoTemp = staInfo;
    std::vector<StationInfo> results;
    if (WifiSettings::GetInstance().GetStationList(results)) {
        WIFI_LOGE("failed to GetStationList.");
        return;
    }
    auto it = results.begin();
    for (; it != results.end(); ++it) {
        if (it->bssid == staInfo.bssid) {
            if (staInfo.deviceName == OHOS::Wifi::GETTING_INFO && staInfo.ipAddr == OHOS::Wifi::GETTING_INFO) {
                staInfoTemp = *it;
            }
            break;
        }
    }

    if (!AddAssociationStation(staInfoTemp)) {
        WIFI_LOGE("AddAssociationStation failed.");
        return;
    }

    if (it == results.end() || it->ipAddr != staInfo.ipAddr) {
        if (m_stationChangeCallback) {
            m_stationChangeCallback(staInfoTemp, ApStatemachineEvent::CMD_STATION_JOIN);
        }
    }
    return;
}

bool ApStationsManager::DisConnectStation(const StationInfo &staInfo) const
{
    std::string mac = staInfo.bssid;
    int ret = static_cast<int>(WifiApHalInterface::GetInstance().DisconnectStaByMac(mac));
    if (ret != WifiErrorNo::WIFI_IDL_OPT_OK) {
        WIFI_LOGE("failed to DisConnectStation staInfo bssid:%{private}s, address:%{private}s, name:%{private}s.",
            staInfo.bssid.c_str(),
            staInfo.ipAddr.c_str(),
            staInfo.deviceName.c_str());
        return false;
    }
    return true;
}

std::vector<std::string> ApStationsManager::GetAllConnectedStations() const
{
    std::vector<std::string> staMacList;
    if (WifiApHalInterface::GetInstance().GetStationList(staMacList) == WifiErrorNo::WIFI_IDL_OPT_OK) {
        for (size_t i = 0; i < staMacList.size(); ++i) {
            WIFI_LOGD("staMacList[%{public}zu]:%{private}s.", i, staMacList[i].c_str());
        }
    }
    return staMacList;
}

void ApStationsManager::RegisterEventHandler(std::function<void(const StationInfo &, ApStatemachineEvent)> callback)
{
    m_stationChangeCallback = callback;
}
}  // namespace Wifi
}  // namespace OHOS