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

#include "wifi_config_center.h"
#include "wifi_ap_hal_interface.h"
#include "wifi_logger.h"

DEFINE_WIFILOG_LABEL("WifiConfigCenter");

namespace OHOS {
namespace Wifi {
WifiConfigCenter &WifiConfigCenter::GetInstance()
{
    static WifiConfigCenter gWifiConfigCenter;
    return gWifiConfigCenter;
}

WifiConfigCenter::WifiConfigCenter()
{
    mStaMidState = WifiOprMidState::CLOSED;
    mApMidState = WifiOprMidState::CLOSED;
    mP2pMidState = WifiOprMidState::CLOSED;
    mScanMidState = WifiOprMidState::CLOSED;
    mWifiOpenedWhenAirplane = false;
}

WifiConfigCenter::~WifiConfigCenter()
{}

int WifiConfigCenter::Init()
{
    if (WifiSettings::GetInstance().Init() < 0) {
        WIFI_LOGE("Init wifi settings failed!");
        return -1;
    }
    return 0;
}

WifiOprMidState WifiConfigCenter::GetWifiMidState()
{
    return mStaMidState.load();
}

bool WifiConfigCenter::SetWifiMidState(WifiOprMidState expState, WifiOprMidState state)
{
    return mStaMidState.compare_exchange_strong(expState, state);
}

void WifiConfigCenter::SetWifiMidState(WifiOprMidState state)
{
    mStaMidState = state;
}

void WifiConfigCenter::SetWifiStaCloseTime()
{
    mWifiCloseTime = std::chrono::steady_clock::now();
}

double WifiConfigCenter::GetWifiStaInterval()
{
    std::chrono::steady_clock::time_point curr = std::chrono::steady_clock::now();
    double drMs = std::chrono::duration<double, std::milli>(curr - mWifiCloseTime).count();
    return drMs;
}

int WifiConfigCenter::GetWifiState()
{
    return WifiSettings::GetInstance().GetWifiState();
}

bool WifiConfigCenter::IsScanAlwaysActive()
{
    return WifiSettings::GetInstance().GetScanAlwaysState();
}

int WifiConfigCenter::GetScanInfoList(std::vector<WifiScanInfo> &results)
{
    return WifiSettings::GetInstance().GetScanInfoList(results);
}

int WifiConfigCenter::GetScanControlInfo(ScanControlInfo &info)
{
    return WifiSettings::GetInstance().GetScanControlInfo(info);
}

int WifiConfigCenter::SetScanControlInfo(const ScanControlInfo &info)
{
    return WifiSettings::GetInstance().SetScanControlInfo(info);
}

int WifiConfigCenter::AddDeviceConfig(const WifiDeviceConfig &config)
{
    return WifiSettings::GetInstance().AddDeviceConfig(config);
}

int WifiConfigCenter::RemoveDevice(int networkId)
{
    return WifiSettings::GetInstance().RemoveDevice(networkId);
}

int WifiConfigCenter::GetDeviceConfig(std::vector<WifiDeviceConfig> &results)
{
    return WifiSettings::GetInstance().GetDeviceConfig(results);
}

int WifiConfigCenter::SetDeviceState(int networkId, int state, bool bSetOther)
{
    return WifiSettings::GetInstance().SetDeviceState(networkId, state, bSetOther);
}

int WifiConfigCenter::GetIpInfo(IpInfo &info)
{
    return WifiSettings::GetInstance().GetIpInfo(info);
}

int WifiConfigCenter::GetLinkedInfo(WifiLinkedInfo &info)
{
    return WifiSettings::GetInstance().GetLinkedInfo(info);
}

int WifiConfigCenter::GetMacAddress(std::string &macAddress)
{
    return WifiSettings::GetInstance().GetMacAddress(macAddress);
}

int WifiConfigCenter::SetCountryCode(const std::string &countryCode)
{
    return WifiSettings::GetInstance().SetCountryCode(countryCode);
}

int WifiConfigCenter::GetCountryCode(std::string &countryCode)
{
    return WifiSettings::GetInstance().GetCountryCode(countryCode);
}

bool WifiConfigCenter::IsLoadStabak()
{
    return WifiSettings::GetInstance().IsLoadStabak();
}

WifiOprMidState WifiConfigCenter::GetApMidState()
{
    return mApMidState.load();
}

bool WifiConfigCenter::SetApMidState(WifiOprMidState expState, WifiOprMidState state)
{
    return mApMidState.compare_exchange_strong(expState, state);
}

void WifiConfigCenter::SetApMidState(WifiOprMidState state)
{
    mApMidState = state;
}

int WifiConfigCenter::GetHotspotState()
{
    return WifiSettings::GetInstance().GetHotspotState();
}

int WifiConfigCenter::SetHotspotConfig(const HotspotConfig &config)
{
    return WifiSettings::GetInstance().SetHotspotConfig(config);
}

int WifiConfigCenter::GetHotspotConfig(HotspotConfig &config)
{
    return WifiSettings::GetInstance().GetHotspotConfig(config);
}

int WifiConfigCenter::GetStationList(std::vector<StationInfo> &results)
{
    return WifiSettings::GetInstance().GetStationList(results);
}

int WifiConfigCenter::FindConnStation(const StationInfo &info)
{
    return WifiSettings::GetInstance().FindConnStation(info);
}

int WifiConfigCenter::GetBlockLists(std::vector<StationInfo> &infos)
{
    return WifiSettings::GetInstance().GetBlockList(infos);
}

int WifiConfigCenter::AddBlockList(const StationInfo &info)
{
    return WifiSettings::GetInstance().ManageBlockList(info, MODE_ADD);
}

int WifiConfigCenter::DelBlockList(const StationInfo &info)
{
    return WifiSettings::GetInstance().ManageBlockList(info, MODE_DEL);
}

int WifiConfigCenter::GetValidBands(std::vector<BandType> &bands)
{
    return WifiSettings::GetInstance().GetValidBands(bands);
}

int WifiConfigCenter::GetValidChannels(ChannelsTable &channelsInfo)
{
    return WifiSettings::GetInstance().GetValidChannels(channelsInfo);
}

bool WifiConfigCenter::GetSupportedBandChannel()
{
    std::vector<int> allowed5GFreq, allowed2GFreq;
    std::vector<int> allowed5GChan, allowed2GChan;
    if (WifiApHalInterface::GetInstance().GetFrequenciesByBand(static_cast<int>(BandType::BAND_2GHZ), allowed2GFreq)) {
        WIFI_LOGW("fail to get 2.4G channel");
    }
    if (WifiApHalInterface::GetInstance().GetFrequenciesByBand(static_cast<int>(BandType::BAND_5GHZ), allowed5GFreq)) {
        WIFI_LOGW("fail to get 5G channel");
    }

    TransformFrequencyIntoChannel(allowed5GFreq, allowed5GChan);
    TransformFrequencyIntoChannel(allowed2GFreq, allowed2GChan);

    ChannelsTable ChanTbs;
    ChanTbs[BandType::BAND_2GHZ] = allowed2GChan;
    ChanTbs[BandType::BAND_5GHZ] = allowed5GChan;

    if (WifiSettings::GetInstance().SetValidChannels(ChanTbs)) {
        WIFI_LOGE("fail to SetValidChannels");
        return false;
    }
    return true;
}

WifiOprMidState WifiConfigCenter::GetScanMidState()
{
    return mScanMidState.load();
}

bool WifiConfigCenter::SetScanMidState(WifiOprMidState expState, WifiOprMidState state)
{
    return mScanMidState.compare_exchange_strong(expState, state);
}

void WifiConfigCenter::SetScanMidState(WifiOprMidState state)
{
    mScanMidState = state;
}

int WifiConfigCenter::GetSignalLevel(const int &rssi, const int &band)
{
    return WifiSettings::GetInstance().GetSignalLevel(rssi, band);
}

WifiOprMidState WifiConfigCenter::GetP2pMidState()
{
    return mP2pMidState.load();
}

bool WifiConfigCenter::SetP2pMidState(WifiOprMidState expState, WifiOprMidState state)
{
    return mP2pMidState.compare_exchange_strong(expState, state);
}

void WifiConfigCenter::SetP2pMidState(WifiOprMidState state)
{
    mP2pMidState = state;
}

int WifiConfigCenter::GetP2pState()
{
    return WifiSettings::GetInstance().GetP2pState();
}

bool WifiConfigCenter::GetCanUseStaWhenAirplaneMode()
{
    return WifiSettings::GetInstance().GetCanUseStaWhenAirplaneMode();
}

bool WifiConfigCenter::GetCanOpenStaWhenAirplaneMode()
{
    return WifiSettings::GetInstance().GetCanOpenStaWhenAirplaneMode();
}

bool WifiConfigCenter::GetWifiStateWhenAirplaneMode()
{
    return mWifiOpenedWhenAirplane;
}

void WifiConfigCenter::SetWifiStateWhenAirplaneMode(bool bState)
{
    mWifiOpenedWhenAirplane = bState;
}

bool WifiConfigCenter::GetStaLastRunState()
{
    return WifiSettings::GetInstance().GetStaLastRunState();
}

int WifiConfigCenter::SetStaLastRunState(bool bRun)
{
    return WifiSettings::GetInstance().SetStaLastRunState(bRun);
}

void WifiConfigCenter::SetScreenState(const int &state)
{
    WifiSettings::GetInstance().SetScreenState(state);
}

int WifiConfigCenter::GetScreenState() const
{
    return WifiSettings::GetInstance().GetScreenState();
}

void WifiConfigCenter::SetAirplaneModeState(const int &state)
{
    WifiSettings::GetInstance().SetAirplaneModeState(state);
}

int WifiConfigCenter::GetAirplaneModeState() const
{
    return WifiSettings::GetInstance().GetAirplaneModeState();
}

void WifiConfigCenter::SetAppRunningState(ScanMode appRunMode)
{
    WifiSettings::GetInstance().SetAppRunningState(appRunMode);
}

ScanMode WifiConfigCenter::GetAppRunningState() const
{
    return WifiSettings::GetInstance().GetAppRunningState();
}

void WifiConfigCenter::SetPowerSavingModeState(const int &state)
{
    WifiSettings::GetInstance().SetPowerSavingModeState(state);
}

int WifiConfigCenter::GetPowerSavingModeState() const
{
    return WifiSettings::GetInstance().GetPowerSavingModeState();
}

void WifiConfigCenter::SetAppPackageName(const std::string &appPackageName)
{
    WifiSettings::GetInstance().SetAppPackageName(appPackageName);
}

const std::string& WifiConfigCenter::GetAppPackageName() const
{
    return WifiSettings::GetInstance().GetAppPackageName();
}

void WifiConfigCenter::SetFreezeModeState(int state)
{
    WifiSettings::GetInstance().SetFreezeModeState(state);
}

int WifiConfigCenter::GetFreezeModeState() const
{
    return WifiSettings::GetInstance().GetFreezeModeState();
}

void WifiConfigCenter::SetNoChargerPlugModeState(int state)
{
    WifiSettings::GetInstance().SetNoChargerPlugModeState(state);
}

int WifiConfigCenter::GetNoChargerPlugModeState() const
{
    return WifiSettings::GetInstance().GetNoChargerPlugModeState();
}

int WifiConfigCenter::SetP2pDeviceName(const std::string &deviceName)
{
    return WifiSettings::GetInstance().SetP2pDeviceName(deviceName);
}
}  // namespace Wifi
}  // namespace OHOS