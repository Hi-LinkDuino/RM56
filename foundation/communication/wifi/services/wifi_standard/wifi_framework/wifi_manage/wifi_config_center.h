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

#ifndef OHOS_WIFI_CONFIG_CENTER_H
#define OHOS_WIFI_CONFIG_CENTER_H

#include <atomic>
#include <chrono>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "wifi_internal_msg.h"
#include "wifi_logger.h"
#include "wifi_settings.h"
#include "wifi_msg.h"

namespace OHOS {
namespace Wifi {
class WifiConfigCenter {
public:
    WifiConfigCenter();
    ~WifiConfigCenter();
    static WifiConfigCenter &GetInstance();

    /**
     * @Description Init WifiConfigCenter object
     *
     * @return int - init result, when 0 means success, other means some fails happened
     */
    int Init();

    /**
     * @Description Get current wifi middle state
     *
     * @return WifiOprMidState - which can be a CLOSED/CLOSING/OPENING/RUNNING/UNKNOWN
     */
    WifiOprMidState GetWifiMidState();

    /**
     * @Description Set current wifi middle state
     *
     * @param expState - expect the original state
     * @param state - want to set state
     * @return true - set the state success
     * @return false - set state failed, current mid sate is not equal to the expState
     */
    bool SetWifiMidState(WifiOprMidState expState, WifiOprMidState state);

    /**
     * @Description Force to set current wifi middle state
     *
     * @param state - want to set state
     */
    void SetWifiMidState(WifiOprMidState state);

    /**
     * @Description Set the wifi close time, just current timestamp
     *
     */
    void SetWifiStaCloseTime();

    /**
     * @Description Get the interval since last wifi close time
     *
     * @return double - the interval, millisecond
     */
    double GetWifiStaInterval();

    /**
     * @Description Get current wifi state
     *
     * @return int - the wifi state, DISABLING/DISABLED/ENABLING/ENABLED/UNKNOWN
     */
    int GetWifiState();

    /**
     * @Description Get the ScanAlways switch state
     *
     * @return true - the ScanAlways switch is on
     * @return false - the ScanAlways switch is off
     */
    bool IsScanAlwaysActive();

    /**
     * @Description Get scan results
     *
     * @param results - output scan results
     * @return int - 0 success
     */
    int GetScanInfoList(std::vector<WifiScanInfo> &results);

    /**
     * @Description Get the scan control policy info
     *
     * @param info - output scan control policy info struct
     * @return int - 0 success
     */
    int GetScanControlInfo(ScanControlInfo &info);

    /**
     * @Description Save the scan control policy info
     *
     * @param info - input scan control policy info struct
     * @return int - 0 success
     */
    int SetScanControlInfo(const ScanControlInfo &info);

    /**
     * @Description Save a wifi device config
     *
     * @param config - input wifi device config
     * @return int - network id
     */
    int AddDeviceConfig(const WifiDeviceConfig &config);

    /**
     * @Description Remove a wifi device config who's networkId equals input networkId
     *
     * @param networkId - want to remove a networkId
     * @return int - 0 success
     */
    int RemoveDevice(int networkId);

    /**
     * @Description Get all saved wifi device config
     *
     * @param results - output wifi device config results
     * @return int - 0 success
     */
    int GetDeviceConfig(std::vector<WifiDeviceConfig> &results);

    /**
     * @Description Set a wifi device's state who's networkId equals input networkId;
     * when the param bSetOther is true and the state is ENABLED, that means we need
     * to set other wifi device DISABLED
     * @param networkId - the wifi device's id
     * @param state - WifiDeviceConfigStatus DISABLED/ENABLED/UNKNOWN
     * @param bSetOther - whether set other device config disabled
     * @return int - when 0 means success, other means some fails happened,
     *               Input state invalid or not find the wifi device config
     */
    int SetDeviceState(int networkId, int state, bool bSetOther = false);

    /**
     * @Description Get the dhcp info
     *
     * @param info - output IpInfo struct
     * @return int - 0 success
     */
    int GetIpInfo(IpInfo &info);

    /**
     * @Description Get current link info
     *
     * @param info - output WifiLinkedInfo struct
     * @return int - 0 success
     */
    int GetLinkedInfo(WifiLinkedInfo &info);

    /**
     * @Description Get the mac address
     *
     * @param macAddress - output mac address info
     * @return int - 0 success
     */
    int GetMacAddress(std::string &macAddress);

    /**
     * @Description Save the country code
     *
     * @param countryCode - input country code
     * @return int - 0 success
     */
    int SetCountryCode(const std::string &countryCode);

    /**
     * @Description Get the country code
     *
     * @param countryCode - output country code
     * @return int - 0 success
     */
    int GetCountryCode(std::string &countryCode);

    /**
     * @Description Obtaining Whether to Load the Configuration of the Standby STA.
     *
     * @return bool - Indicates whether to load the configuration of the standby STA.
     */
    bool IsLoadStabak();

    /**
     * @Description Get current hotspot middle state
     *
     * @return WifiOprMidState - which can be a CLOSED/CLOSING/OPENING/RUNNING/UNKNOWN
     */
    WifiOprMidState GetApMidState();

    /**
     * @Description Set current hotspot middle state
     *
     * @param expState - expect the original state
     * @param state - want to set state
     * @return true - set the state success
     * @return false - set state failed, current mid sate is not equal to the expState
     */
    bool SetApMidState(WifiOprMidState expState, WifiOprMidState state);

    /**
     * @Description Force to set current hotspot middle state
     *
     * @param state - want to set state
     */
    void SetApMidState(WifiOprMidState state);

    /**
     * @Description Get current hotspot state
     *
     * @return int - the hotspot state, IDLE/STARTING/STARTED/CLOSING/CLOSED
     */
    int GetHotspotState();

    /**
     * @Description Set the hotspot config
     *
     * @param config - input HotspotConfig struct
     * @return int - 0 success
     */
    int SetHotspotConfig(const HotspotConfig &config);

    /**
     * @Description Get the hotspot config
     *
     * @param config - output HotspotConfig struct
     * @return int - 0 success
     */
    int GetHotspotConfig(HotspotConfig &config);

    /**
     * @Description Get current hotspot accept linked stations
     *
     * @param results - output StationInfo results
     * @return int - 0 success
     */
    int GetStationList(std::vector<StationInfo> &results);

    /**
     * @Description Judge whether the station is in current linked station list
     *
     * @param info - input StationInfo struct
     * @return int - 0 find the station, exist; -1 not find, not exist
     */
    int FindConnStation(const StationInfo &info);

    /**
     * @Description Get the block list
     *
     * @param infos - output StationInfo results
     * @return int - 0 success
     */
    int GetBlockLists(std::vector<StationInfo> &infos);

    /**
     * @Description Add the station info into the block list
     *
     * @param info - input StationInfo struct
     * @return int - 0 success
     */
    int AddBlockList(const StationInfo &info);

    /**
     * @Description Remove the station info from the block list
     *
     * @param info - input StationInfo struct
     * @return int - 0 success
     */
    int DelBlockList(const StationInfo &info);

    /**
     * @Description Get the valid bands info
     *
     * @param bands - output band results
     * @return int - 0 success
     */
    int GetValidBands(std::vector<BandType> &bands);

    /**
     * @Description Get current valid channels
     *
     * @param channelsInfo - output valid channel info
     * @return int - 0 success
     */
    int GetValidChannels(ChannelsTable &channelsInfo);

    /**
     * @Description request to chip for initation current vaild bands and channels
     */
    bool GetSupportedBandChannel();

    /**
     * @Description Get current scan service middle state
     *
     * @return WifiOprMidState - which can be a CLOSED/CLOSING/OPENING/RUNNING/UNKNOWN
     */
    WifiOprMidState GetScanMidState();

    /**
     * @Description Set current scan service middle state
     *
     * @param expState - expect the original state
     * @param state - want to set state
     * @return true - set the state success
     * @return false - set state failed, current mid sate is not equal to the expState
     */
    bool SetScanMidState(WifiOprMidState expState, WifiOprMidState state);

    /**
     * @Description Force to set the scan service middle state
     *
     * @param state - want to set state
     */
    void SetScanMidState(WifiOprMidState state);

    /**
     * @Description Get signal level about given rssi and band
     *
     * @param rssi - rssi info
     * @param band - band info
     * @return int - 0 or the level
     */
    int GetSignalLevel(const int &rssi, const int &band);

    /**
     * @Description Get current p2p middle state
     *
     * @return WifiOprMidState
     */
    WifiOprMidState GetP2pMidState();

    /**
     * @Description Set current hotspot middle state.
     *
     * @param expState - expect the original state
     * @param state - want to set state
     * @return true - set the state success
     * @return false - set state failed, current mid sate is not equal to the expState
     */
    bool SetP2pMidState(WifiOprMidState expState, WifiOprMidState state);

    /**
     * @Description Force to set current hotspot middle state
     *
     * @param state - want to set state
     */
    void SetP2pMidState(WifiOprMidState state);

    /**
     * @Description Get current p2p state
     *
     * @return int - the p2p state,NONE/IDLE/STARTING/STARTED/CLOSING/CLOSED
     */
    int GetP2pState();

    /**
     * @Description Get the config whether permit to use wifi when airplane mode opened
     *
     * @return true - can use
     * @return false - cannot use
     */
    bool GetCanUseStaWhenAirplaneMode();

    /**
     * @Description Set the config whether permit to use wifi when airplane mode opened
     *
     * @param bCan - true / false
     * @return int - 0 success
     */
    int SetCanUseStaWhenAirplaneMode(bool bCan);

    /**
     * @Description Get the config whether can open sta when airplane mode opened
     *
     * @return true - can open
     * @return false - can't open
     */
    bool GetCanOpenStaWhenAirplaneMode();

    /**
     * @Description when last airplane mode, get sta state, open or close
     *
     * @return true - when in airplane mode, we open sta
     * @return false - when in airplane mode, we close sta
     */
    bool GetWifiStateWhenAirplaneMode();

    /**
     * @Description when in airplane mode, set sta state
     *
     * @param bState - open or close
     */
    void SetWifiStateWhenAirplaneMode(bool bState);

    /**
     * @Description Get the STA service last running state
     *
     * @return true - sta is running
     * @return false - sta not running
     */
    bool GetStaLastRunState();

    /**
     * @Description Set the STA service running state
     *
     * @param bRun - true / false
     * @return int - 0 success
     */
    int SetStaLastRunState(bool bRun);

    /**
     * @Description Set current phone screen state
     *
     * @param state - 1 open; 2 close
     */
    void SetScreenState(const int &state);

    /**
     * @Description Get current phone screen state
     *
     * @return int - 1 open; 2 close
     */
    int GetScreenState() const;

    /**
     * @Description Set current airplane mode state
     *
     * @param state - 1 open; 2 close
     */
    void SetAirplaneModeState(const int &state);

    /**
     * @Description Get current airplane mode state
     *
     * @return int - 1 open; 2 close
     */
    int GetAirplaneModeState() const;

    /**
     * @Description Set current app running mode
     *
     * @param appRunMode - app run mode
     */
    void SetAppRunningState(ScanMode appRunMode);

    /**
     * @Description Get current app running mode
     *
     * @param ScanMode
     */
    ScanMode GetAppRunningState() const;

    /**
     * @Description Set current power saving mode
     *
     * @param state - 1 saving mode; 2 not saving mode
     */
    void SetPowerSavingModeState(const int &state);

    /**
     * @Description Get current power saving mode
     *
     * @return int - 1 saving mode; 2 not saving mode
     */
    int GetPowerSavingModeState() const;

    /**
     * @Description Set app package name.
     *
     * @param appPackageName - app package name
     */
    void SetAppPackageName(const std::string &appPackageName);

    /**
     * @Description Get app package name.
     *
     * @return const std::string& - app package name.
     */
    const std::string& GetAppPackageName() const;

    /**
     * @Description Set freeze mode state.
     *
     * @param state - 1 freeze mode; 2 moving mode
     */
    void SetFreezeModeState(int state);

    /**
     * @Description Get freeze mode state.
     *
     * @return freeze mode.
     */
    int GetFreezeModeState() const;
    
    /**
     * @Description Set no charger plugged in mode.
     *
     * @param state - 1 no charger plugged in mode; 2 charger plugged in mode
     */
    void SetNoChargerPlugModeState(int state);

    /**
     * @Description Get no charger plugged in mode.
     *
     * @return no charger plugged in mode.
     */
    int GetNoChargerPlugModeState() const;

    /**
     * @Description set the device name
     *
     * @param deviceName - device name
     * @return int - 0 success
     */
    int SetP2pDeviceName(const std::string &deviceName);

private:
    std::atomic<WifiOprMidState> mStaMidState;
    std::atomic<WifiOprMidState> mApMidState;
    std::atomic<WifiOprMidState> mP2pMidState;
    std::atomic<WifiOprMidState> mScanMidState;
    /* Time interval for disabling and re-enabling the STA */
    std::chrono::steady_clock::time_point mWifiCloseTime;
    bool mWifiOpenedWhenAirplane;
};
} // namespace Wifi
} // namespace OHOS
#endif
