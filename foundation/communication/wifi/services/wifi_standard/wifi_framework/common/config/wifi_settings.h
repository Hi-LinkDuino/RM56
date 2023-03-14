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
#ifndef OHOS_WIFI_SETTINGS_H
#define OHOS_WIFI_SETTINGS_H

#include <string>
#include <vector>
#include <map>
#include <atomic>
#include <memory>
#include <mutex>
#include <algorithm>
#include "wifi_config_file_impl.h"

constexpr int RANDOM_STR_LEN = 6;
constexpr int MSEC = 1000;
constexpr int FOREGROUND_SCAN_CONTROL_TIMES = 4;
constexpr int FOREGROUND_SCAN_CONTROL_INTERVAL = 2 * 60;
constexpr int BACKGROUND_SCAN_CONTROL_TIMES = 1;
constexpr int BACKGROUND_SCAN_CONTROL_INTERVAL = 30 * 60;
constexpr int PNO_SCAN_CONTROL_TIMES = 1;
constexpr int PNO_SCAN_CONTROL_INTERVAL = 20;
constexpr int SYSTEM_TIMER_SCAN_CONTROL_TIMES = 4;
constexpr int SYSTEM_TIMER_SCAN_CONTROL_INTERVAL = 20;
constexpr int MODE_ADD = 0;
constexpr int MODE_DEL = 1;
constexpr int MODE_UPDATE = 2;
/* Obtain the scanning result that is valid within 180s. */
constexpr int WIFI_GET_SCAN_INFO_VALID_TIMESTAMP = 180;

constexpr char DEVICE_CONFIG_FILE_PATH[] = "/data/misc/wifi/device_config.conf";
constexpr char HOTSPOT_CONFIG_FILE_PATH[] = "/data/misc/wifi/hotspot_config.conf";
constexpr char BLOCK_LIST_FILE_PATH[] = "/data/misc/wifi/block_list.conf";
constexpr char WIFI_CONFIG_FILE_PATH[] = "/data/misc/wifi/wifi_config.conf";
constexpr char WIFI_P2P_GROUP_INFO_FILE_PATH[] = "/data/misc/wifi/p2p_groups.conf";
constexpr char WIFI_P2P_VENDOR_CONFIG_FILE_PATH[] = "/data/misc/wifi/p2p_vendor_config.conf";
const std::string WIFI_TRUST_LIST_POLICY_FILE_PATH = "/data/misc/wifi/trust_list_polices.conf";
const std::string WIFI_MOVING_FREEZE_POLICY_FILE_PATH = "/data/misc/wifi/moving_freeze_policy.conf";
constexpr char WIFI_STA_RANDOM_MAC_FILE_PATH[] = "/data/misc/wifi/sta_randomMac.conf";

namespace OHOS {
namespace Wifi {
using ChannelsTable = std::map<BandType, std::vector<int32_t>>;

class WifiSettings {
public:
    ~WifiSettings();
    static WifiSettings &GetInstance();

    /**
     * @Description Init the WifiSettings object
     *
     * @return int - init result, when 0 means success, other means some fails happened
     */
    int Init();

    /**
     * @Description Get the Wifi Sta Capabilities
     *
     * @return int - mWifiStaCapabilities
     */
    int GetWifiStaCapabilities() const;

    /**
     * @Description Save the Wifi Sta Capabilities
     *
     * @param capabilities - input capability
     * @return int - 0 success
     */
    int SetWifiStaCapabilities(int capabilities);

    /**
     * @Description Get current STA service state
     *
     * @return int - the wifi state, DISABLING/DISABLED/ENABLING/ENABLED/UNKNOWN
     */
    int GetWifiState() const;

    /**
     * @Description Save STA service state
     *
     * @param state - the wifi state
     * @return int - 0 success
     */
    int SetWifiState(int state);

    /**
     * @Description Get the ScanAlways switch state
     *
     * @return true - ScanAlways on, false - ScanAlways off
     */
    bool GetScanAlwaysState() const;

    /**
     * @Description Set the ScanAlways switch state
     *
     * @param isActive - ScanAlways on/off
     * @return int - 0 success
     */
    int SetScanAlwaysState(bool isActive);

    /**
     * @Description Save scan results
     *
     * @param results - vector scan results
     * @return int - 0 success
     */
    int SaveScanInfoList(const std::vector<WifiScanInfo> &results);

    /**
     * @Description Get scan results
     *
     * @param results - output vector of scan results
     * @return int - 0 success
     */
    int GetScanInfoList(std::vector<WifiScanInfo> &results);

    /**
     * @Description save the p2p connected info
     *
     * @param linkedInfo - WifiP2pLinkedInfo object
     * @return int - 0 success
     */
    int SaveP2pInfo(WifiP2pLinkedInfo &linkedInfo);

    /**
     * @Description Get the p2p connected info
     *
     * @param linkedInfo - output the p2p connected info
     * @return int - 0 success
     */
    int GetP2pInfo(WifiP2pLinkedInfo &linkedInfo);

    /**
     * @Description Get the scan control policy info
     *
     * @param info - output ScanControlInfo struct
     * @return int - 0 success
     */
    int GetScanControlInfo(ScanControlInfo &info);

    /**
     * @Description Save the scan control policy info
     *
     * @param info - input ScanControlInfo struct
     * @return int - 0 success
     */
    int SetScanControlInfo(const ScanControlInfo &info);

    /**
     * @Description Add Device Configuration
     *
     * @param config - WifiDeviceConfig object
     * @return int - network id
     */
    int AddDeviceConfig(const WifiDeviceConfig &config);

    /**
     * @Description Remove a wifi device config who's networkId equals input networkId
     *
     * @param networkId - a networkId that is to be removed
     * @return int - 0 success ,other is failed
     */
    int RemoveDevice(int networkId);

    /**
     * @Description Remove all saved wifi device config
     *
     */
    void ClearDeviceConfig(void);

    /**
     * @Description Get all saved wifi device config
     *
     * @param results - output wifi device config results
     * @return int - 0 success
     */
    int GetDeviceConfig(std::vector<WifiDeviceConfig> &results);

    /**
     * @Description Get the specify networkId's wifi device config
     *
     * @param networkId - network id
     * @param config - output WifiDeviceConfig struct
     * @return int - 0 success; -1 not find the device config
     */
    int GetDeviceConfig(const int &networkId, WifiDeviceConfig &config);

    /**
     * @brief Get the specify wifi device config which bssid is equal to input bssid
     *
     * @param index - bssid string or ssid string
     * @param indexType - index type 0:ssid 1:bssid
     * @param config - output WifiDeviceConfig struct
     * @return int - 0 success; -1 not find the device config
     */
    int GetDeviceConfig(const std::string &index, const int &indexType, WifiDeviceConfig &config);

    /**
     * @Description Get the specify wifi device config which ssid is equal to input ssid and keymgmt is equal to input
     * keymgmt
     *
     * @param ssid - ssid string
     * @param keymgmt - keymgmt string
     * @param config - output WifiDeviceConfig struct
     * @return int - 0 success; -1 not find the device config
     */
    int GetDeviceConfig(const std::string &ssid, const std::string &keymgmt, WifiDeviceConfig &config);
    /**
     * @Description Get the wifi device configs which hiddenSSID is true
     *
     * @param results - output WifiDeviceConfig structs
     * @return int - 0 success
     */
    int GetHiddenDeviceConfig(std::vector<WifiDeviceConfig> &results);

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
     * @Description Synchronizing saved the wifi device config into config file
     *
     * @return int - 0 success; -1 save file failed
     */
    int SyncDeviceConfig();

    /**
     * @Description Reload wifi device config from config file
     *
     * @return int - 0 success; -1 read config file failed
     */
    int ReloadDeviceConfig();

    /**
     * @Description Synchronizing saved the wifi WifiP2pGroupInfo config into config file
     *
     * @return int - 0 success; -1 save file failed
     */
    int SyncWifiP2pGroupInfoConfig();

    /**
     * @Description Reload wifi WifiP2pGroupInfo config from config file
     *
     * @return int - 0 success; -1 read config file failed
     */
    int ReloadWifiP2pGroupInfoConfig();

    /**
     * @Description Save WifiP2pGroupInfo
     *
     * @param groups - input wifi p2p groups config results
     * @return int - 0 success
     */
    int SetWifiP2pGroupInfo(const std::vector<WifiP2pGroupInfo> &groups);

    /**
     * @Description Delete a WifiP2pGroupInfo node
     *
     * @return int
     */
    int RemoveWifiP2pGroupInfo();

    /**
     * @Description Get all saved wifi p2p groups config
     *
     * @param results - output wifi p2p groups config results
     * @return int - 0 success
     */
    int GetWifiP2pGroupInfo(std::vector<WifiP2pGroupInfo> &groups);

    /**
     * @Description Get the dhcp info
     *
     * @param info - output IpInfo struct
     * @return int - 0 success
     */
    int GetIpInfo(IpInfo &info);

    /**
     * @Description Save dhcp info
     *
     * @param info - input IpInfo struct
     * @return int - 0 success
     */
    int SaveIpInfo(const IpInfo &info);

    /**
     * @Description Get current link info
     *
     * @param info - output WifiLinkedInfo struct
     * @return int - 0 success
     */
    int GetLinkedInfo(WifiLinkedInfo &info);

    /**
     * @Description Save link info
     *
     * @param info - input WifiLinkedInfo struct
     * @return int - 0 success
     */
    int SaveLinkedInfo(const WifiLinkedInfo &info);

    /**
     * @Description Save mac address
     *
     * @param macAddress - mac address info
     * @return int - 0 success
     */
    int SetMacAddress(const std::string &macAddress);

    /**
     * @Description Get the mac address
     *
     * @param macAddress - output mac address info
     * @return int - 0 success
     */
    int GetMacAddress(std::string &macAddress);

    /**
     * @Description reload mac address
     *
     * @return int - 0 success
     */
    int ReloadStaRandomMac();

    /**
     * @Description add random mac address
     *
     * @param randomMacInfo - randmon mac address info
     * @return int - 0 success
     */
    bool AddRandomMac(WifiStoreRandomMac &randomMacInfo);

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
     * @Description Get current hotspot state
     *
     * @return int - the hotspot state, IDLE/STARTING/STARTED/CLOSING/CLOSED
     */
    int GetHotspotState();

    /**
     * @Description Save current hotspot state
     *
     * @param state - hotspot state
     * @return int - 0 success
     */
    int SetHotspotState(int state);

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
     * @Description Synchronizing saved the Hotspot config into config file
     *
     * @return int - 0 success; -1 save file failed
     */
    int SyncHotspotConfig();

    /**
     * @Description Set the p2p vendor config
     *
     * @param config - input P2pVendorConfig struct
     * @return int - 0 success
     */
    int SetP2pVendorConfig(const P2pVendorConfig &config);

    /**
     * @Description Get the p2p vendor config
     *
     * @param config - output P2pVendorConfig struct
     * @return int - 0 success
     */
    int GetP2pVendorConfig(P2pVendorConfig &config);

    /**
     * @Description Synchronizing saved the P2p Vendor config into config file
     *
     * @return int - 0 success; -1 save file failed
     */
    int SyncP2pVendorConfig();

    /**
     * @Description Get current hotspot accept linked stations
     *
     * @param results - output StationInfo results
     * @return int - 0 success
     */
    int GetStationList(std::vector<StationInfo> &results);

    /**
     * @Description Management (add/update/delete) connected station list
     *
     * @param info - input StationInfo struct
     * @param mode - mode of MODE_ADD MODE_UPDATE MODE_DEL
     * @return int - 0 success; -1 mode not correct
     */
    int ManageStation(const StationInfo &info, int mode); /* add / update / remove */

    /**
     * @Description Clear connected station list
     *
     * @return int - 0 success
     */
    int ClearStationList();

    /**
     * @Description Get the block list
     *
     * @param results - output StationInfo results
     * @return int - 0 success
     */
    int GetBlockList(std::vector<StationInfo> &results);

    /**
     * @Description Manager (add/update/delete) station connect Blocklist
     *
     * @param info - input StationInfo struct
     * @param mode - mode of MODE_ADD MODE_DEL MODE_UPDATE
     * @return int - 0 success; -1 mode not correct
     */
    int ManageBlockList(const StationInfo &info, int mode); /* add / remove */

    /**
     * @Description Judge whether the station is in current linked station list
     *
     * @param info - input StationInfo struct
     * @return int - 0 find the station, exist; -1 not find, not exist
     */
    int FindConnStation(const StationInfo &info);

    /**
     * @Description Synchronizing saved the block list config into config file
     *
     * @return int - 0 success; -1 save file failed
     */
    int SyncBlockList();

    /**
     * @Description Get the Valid Bands object
     *
     * @param bands - output vector for BandType
     * @return int - 0 success
     */
    int GetValidBands(std::vector<BandType> &bands);

    /**
     * @Description Set the Valid Channels object
     *
     * @param channelsInfo - input ChannelsTable struct
     * @return int - 0 success
     */
    int SetValidChannels(const ChannelsTable &channelsInfo);

    /**
     * @Description Get the Valid Channels object
     *
     * @param channelsInfo - output ChannelsTable struct
     * @return int - 0 success
     */
    int GetValidChannels(ChannelsTable &channelsInfo);

    /**
     * @Description Clear the number of valid channels
     *
     * @return int - 0 success
     */
    int ClearValidChannels();

    /**
     * @Description set the p2p state
     *
     * @param state - the p2p state
     * @return int - 0 success
     */
    int SetP2pState(int state);

    /**
     * @Description Get current p2p state
     *
     * @return int - the p2p state, NONE/IDLE/STARTING/STARTED/CLOSING/CLOSED
     */
    int GetP2pState();

    /**
     * @Description set the p2p discover state
     *
     * @param state - the p2p discover state
     * @return int - 0 success
     */
    int SetP2pDiscoverState(int state);

    /**
     * @Description Get current p2p discover state
     *
     * @return int -the p2p discover state, P2P_DISCOVER_NONE/P2P_DISCOVER_STARTING/P2P_DISCOVER_CLOSED
     */
    int GetP2pDiscoverState();

    /**
     * @Description set the p2p connected state
     *
     * @param state - the p2p connected state
     * @return int - 0 success
     */
    int SetP2pConnectedState(int state);

    /**
     * @Description Get current p2p connected state
     *
     * @return int - the connected state, P2P_CONNECTED_NONE/P2P_CONNECTED_STARTING/P2P_CONNECTED_CLOSED
     */
    int GetP2pConnectedState();

    /**
     * @Description Get signal level about given rssi and band
     *
     * @param rssi - rssi info
     * @param band - band info
     * @return int - level
     */
    int GetSignalLevel(const int &rssi, const int &band);

    /**
     * @Description Get the Ap Max Conn Num
     *
     * @return int - number
     */
    int GetApMaxConnNum();

    /**
     * @Description Enable Network
     *
     * @param networkId - enable network id
     * @param disableOthers - when set, save this network id, and can use next time
     * @return true
     * @return false
     */
    bool EnableNetwork(int networkId, bool disableOthers);

    /**
     * @Description Set the User Last Selected Network Id
     *
     * @param networkId - network id
     */
    void SetUserLastSelectedNetworkId(int networkId);

    /**
     * @Description Get the User Last Selected Network Id
     *
     * @return int - network id
     */
    int GetUserLastSelectedNetworkId();

    /**
     * @Description Get the User Last Selected Network time
     *
     * @return time_t - timestamp
     */
    time_t GetUserLastSelectedNetworkTimeVal();

    /**
     * @Description Synchronizing saved the WifiConfig into config file
     *
     * @return int - 0 success; -1 save file failed
     */
    int SyncWifiConfig();

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
     * @Description Get the STA service last running state
     *
     * @return true - running
     * @return false - not running
     */
    bool GetStaLastRunState();

    /**
     * @Description Set the STA service running state
     *
     * @param bRun - running or not
     * @return int - 0 success
     */
    int SetStaLastRunState(bool bRun);

    /**
     * @Description Get the Dhcp Ip Type
     *
     * @return int - dhcp ip type, ipv4/ipv6/double
     */
    int GetDhcpIpType();

    /**
     * @Description Set the Dhcp Ip Type
     *
     * @param dhcpIpType - ipv4/ipv6/double
     * @return int - 0 success
     */
    int SetDhcpIpType(int dhcpIpType);

    /**
     * @Description Get the Default Wifi Interface
     *
     * @return std::string - interface name
     */
    std::string GetDefaultWifiInterface();

    /**
     * @Description Set the Screen State
     *
     * @param state - 1 on; 2 off
     */
    void SetScreenState(const int &state);

    /**
     * @Description Get the Screen State
     *
     * @return int - 1 on; 2 off
     */
    int GetScreenState() const;

    /**
     * @Description Set the Airplane Mode State
     *
     * @param state - 1 open; 2 close
     */
    void SetAirplaneModeState(const int &state);

    /**
     * @Description Get the Airplane Mode State
     *
     * @return int - 1 open; 2 close
     */
    int GetAirplaneModeState() const;

    /**
     * @Description Set the App Running State
     *
     * @param appRunMode - app run mode
     */
    void SetAppRunningState(ScanMode appRunMode);

    /**
     * @Description Get the App Running State
     *
     * @return ScanMode
     */
    ScanMode GetAppRunningState() const;

    /**
     * @Description Set the Power Saving Mode State
     *
     * @param state - 1 open; 2 close
     */
    void SetPowerSavingModeState(const int &state);

    /**
     * @Description Get the Power Saving Mode State
     *
     * @return int - 1 open; 2 close
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
     * @Description Set enable/disable Whether to allow network switchover
     *
     * @param bSwitch - enable/disable
     * @return int - 0 success
     */
    int SetWhetherToAllowNetworkSwitchover(bool bSwitch);

    /**
     * @Description Check whether enable network switchover
     *
     * @return true - enable
     * @return false - disable
     */
    bool GetWhetherToAllowNetworkSwitchover();

    /**
     * @Description Set the policy score slope
     *
     * @param score - score
     * @return int - 0 success
     */
    int SetScoretacticsScoreSlope(const int &score);

    /**
     * @Description Get the policy score slope
     *
     * @return int - score
     */
    int GetScoretacticsScoreSlope();

    /**
     * @Description Initial score of the set strategy
     *
     * @param score - score
     * @return int - 0 success
     */
    int SetScoretacticsInitScore(const int &score);

    /**
     * @Description Obtain the initial score of the tactic
     *
     * @return int - score
     */
    int GetScoretacticsInitScore();

    /**
     * @Description Set the scoring policy to the same BSSID score
     *
     * @param score - score
     * @return int - 0 success
     */
    int SetScoretacticsSameBssidScore(const int &score);

    /**
     * @Description Get the scoring policy to the same BSSID score
     *
     * @return int - score
     */
    int GetScoretacticsSameBssidScore();

    /**
     * @Description Set the score policy for the same network
     *
     * @param score - score
     * @return int - 0 success
     */
    int SetScoretacticsSameNetworkScore(const int &score);

    /**
     * @Description Get the score policy for the same network
     *
     * @return int - score
     */
    int GetScoretacticsSameNetworkScore();

    /**
     * @Description Set the 5 GHz score of the policy frequency
     *
     * @param score - score
     * @return int - 0 success
     */
    int SetScoretacticsFrequency5GHzScore(const int &score);

    /**
     * @Description Get the 5 GHz score of the policy frequency
     *
     * @return int - score
     */
    int GetScoretacticsFrequency5GHzScore();

    /**
     * @Description Set the score policy. last select score
     *
     * @param score - score
     * @return int - 0 success
     */
    int SetScoretacticsLastSelectionScore(const int &score);

    /**
     * @Description Get the score policy, last select score
     *
     * @return int - score
     */
    int GetScoretacticsLastSelectionScore();

    /**
     * @Description Setting the Score Policy Security Score
     *
     * @param score - score
     * @return int - 0 success
     */
    int SetScoretacticsSecurityScore(const int &score);

    /**
     * @Description Get the Score Policy Security Score
     *
     * @return int - priority
     */
    int GetScoretacticsSecurityScore();

    /**
     * @Description Set the saved device appraisal priority
     *
     * @param priority - priority
     * @return int - 0 success
     */
    int SetSavedDeviceAppraisalPriority(const int &priority);

    /**
     * @Description Get the saved device appraisal priority
     *
     * @return int - priority
     */
    int GetSavedDeviceAppraisalPriority();

    /**
     * @Description Judge the Module need preloaded or not
     *
     * @param name - module name
     * @return true - need preload
     * @return false - no need preload
     */
    bool IsModulePreLoad(const std::string &name);

    /**
     * @Description Save wps connection device config
     *
     * @param config - input WifiDeviceConfig struct
     * @return int - 0 success; -1 load the device config file failed
     */
    int AddWpsDeviceConfig(const WifiDeviceConfig &config);
    /**
     * @Description Get the Support HwPno Flag object
     *
     * @return true - support HwPno scan
     * @return false - not support HwPno scan
     */
    bool GetSupportHwPnoFlag();
    /**
     * @Description Get the Min 2.4G strength object
     *
     * @return int Min 2.4G strength
     */
    int GetMinRssi2Dot4Ghz();
    /**
     * @Description Get the Min 5G strength object
     *
     * @return int Min 5G strength
     */
    int GetMinRssi5Ghz();

    /**
     * @Description Get the Alternate dns.
     *
     * @return string - dns
     */
    std::string GetStrDnsBak() const;
    /**
     * @Description Obtaining Whether to Load the Configuration of the Standby STA.
     *
     * @return bool - Indicates whether to load the configuration of the standby STA.
     */
    bool IsLoadStabak() const;

    /**
     * @Description set the device name
     *
     * @param deviceName - device name
     * @return int - result
     */
    int SetP2pDeviceName(const std::string &deviceName);

    /**
     * @Description get trustlist policies.
     *
     * @return const std::vector<TrustListPolicy>& - trustlist policies.
     */
    const std::vector<TrustListPolicy>& ReloadTrustListPolicies();

    /**
     * @Description get moving freeze state trustlist.
     *
     * @return const MovingFreezePolicy& - moving freeze policy.
     */
    const MovingFreezePolicy& ReloadMovingFreezePolicy();

    /**
     * @Description get bssid of connection timeout for last time.
     *
     * @return bssid.
     */
    std::string GetConnectTimeoutBssid();

    /**
     * @Description set bssid of connection timeout for last time.
     *
     * @return int - result
     */
    int SetConnectTimeoutBssid(std::string &bssid);

private:
    WifiSettings();
    void InitWifiConfig();
    void InitDefaultHotspotConfig();
    void InitHotspotConfig();
    void InitDefaultP2pVendorConfig();
    void InitP2pVendorConfig();
    void InitGetApMaxConnNum();
    void InitScanControlForbidList();
    void InitScanControlIntervalList();
    void InitScanControlInfo();

private:
    int mWifiStaCapabilities;            /* Sta capability */
    std::atomic<int> mWifiState;         /* Sta service state */
    std::atomic<bool> mScanAlwaysActive; /* if scan always */
    std::vector<WifiScanInfo> mWifiScanInfoList;
    std::vector<WifiP2pGroupInfo> mGroupInfoList;
    std::vector<WifiStoreRandomMac> mWifiStoreRandomMac;
    ScanControlInfo mScanControlInfo;
    WifiP2pLinkedInfo mWifiP2pInfo;
    std::map<int, WifiDeviceConfig> mWifiDeviceConfig;
    IpInfo mWifiIpInfo;
    WifiLinkedInfo mWifiLinkedInfo;
    std::string mMacAddress;
    std::string mCountryCode;
    std::atomic<int> mHotspotState;
    HotspotConfig mHotspotConfig;
    P2pVendorConfig mP2pVendorConfig;
    std::map<std::string, StationInfo> mConnectStationInfo;
    std::map<std::string, StationInfo> mBlockListInfo;
    ChannelsTable mValidChannels;
    std::atomic<int> mP2pState;
    std::atomic<int> mP2pDiscoverState;
    std::atomic<int> mP2pConnectState;
    int mApMaxConnNum;           /* ap support max sta numbers */
    int mLastSelectedNetworkId;  /* last selected networkid */
    time_t mLastSelectedTimeVal; /* last selected time */
    int mScreenState;            /* 1 on 2 off */
    int mAirplaneModeState;      /* 1 on 2 off */
    ScanMode mAppRunningModeState; /* 0 app for 1 app back 2 sys for 3 sys back */
    int mPowerSavingModeState;   /* 1 on 2 off */
    std::string mAppPackageName;
    int mFreezeModeState;        /* 1 on 2 off */
    int mNoChargerPlugModeState;  /* 1 on 2 off */
    WifiConfig mWifiConfig;
    std::pair<std::string, int> mBssidToTimeoutTime;

    std::mutex mStaMutex;
    std::mutex mApMutex;
    std::mutex mConfigMutex;
    std::mutex mInfoMutex;
    std::mutex mP2pMutex;

    std::atomic_flag deviceConfigLoadFlag = ATOMIC_FLAG_INIT;

    WifiConfigFileImpl<WifiDeviceConfig> mSavedDeviceConfig; /* Persistence device config */
    WifiConfigFileImpl<HotspotConfig> mSavedHotspotConfig;
    WifiConfigFileImpl<StationInfo> mSavedBlockInfo;
    WifiConfigFileImpl<WifiConfig> mSavedWifiConfig;
    WifiConfigFileImpl<WifiP2pGroupInfo> mSavedWifiP2pGroupInfo;
    WifiConfigFileImpl<P2pVendorConfig> mSavedWifiP2pVendorConfig;
    WifiConfigFileImpl<TrustListPolicy> mTrustListPolicies;
    WifiConfigFileImpl<MovingFreezePolicy> mMovingFreezePolicy;
    MovingFreezePolicy mFPolicy;
    WifiConfigFileImpl<WifiStoreRandomMac> mSavedWifiStoreRandomMac;
};
}  // namespace Wifi
}  // namespace OHOS
#endif