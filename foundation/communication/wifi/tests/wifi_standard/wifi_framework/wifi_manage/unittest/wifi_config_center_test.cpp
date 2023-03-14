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
#include "wifi_config_center_test.h"
#include "wifi_global_func.h"
#include "wifi_internal_msg.h"

using namespace testing::ext;

namespace OHOS {
namespace Wifi {
HWTEST_F(WifiConfigCenterTest, Init_SUCCESS, TestSize.Level1)
{
    EXPECT_EQ(0, WifiConfigCenter::GetInstance().Init());
}

HWTEST_F(WifiConfigCenterTest, SetGetWifiMidState_SUCCESS, TestSize.Level1)
{
    WifiOprMidState state = OHOS::Wifi::WifiOprMidState::RUNNING;
    WifiConfigCenter::GetInstance().SetWifiMidState(state);
    EXPECT_EQ(state, WifiConfigCenter::GetInstance().GetWifiMidState());
}

HWTEST_F(WifiConfigCenterTest, SetWifiMidStateExp_SUCCESS, TestSize.Level1)
{
    WifiOprMidState cloState = OHOS::Wifi::WifiOprMidState::CLOSED;
    WifiOprMidState runState = OHOS::Wifi::WifiOprMidState::RUNNING;
    WifiConfigCenter::GetInstance().SetWifiMidState(cloState);
    EXPECT_EQ(true, WifiConfigCenter::GetInstance().SetWifiMidState(cloState, runState));
}

HWTEST_F(WifiConfigCenterTest, SetWifiMidStateExp_FAILED, TestSize.Level1)
{
    WifiOprMidState cloState = OHOS::Wifi::WifiOprMidState::CLOSED;
    WifiOprMidState runState = OHOS::Wifi::WifiOprMidState::RUNNING;
    WifiConfigCenter::GetInstance().SetWifiMidState(cloState);
    EXPECT_NE(true, WifiConfigCenter::GetInstance().SetWifiMidState(runState, cloState));
}

HWTEST_F(WifiConfigCenterTest, GetWifiStaIntervalTest, TestSize.Level1)
{
    WifiConfigCenter::GetInstance().SetWifiStaCloseTime();
    sleep(1);
    double interval = WifiConfigCenter::GetInstance().GetWifiStaInterval();
    EXPECT_TRUE(interval >= 1000 && interval <= 2000);
}

HWTEST_F(WifiConfigCenterTest, SetGetWifi_SUCCESS, TestSize.Level1)
{
    WifiOprMidState cloState = OHOS::Wifi::WifiOprMidState::CLOSED;
    WifiSettings::GetInstance().SetWifiState(static_cast<int>(cloState));
    EXPECT_EQ((int)cloState, WifiConfigCenter::GetInstance().GetWifiState());
}

HWTEST_F(WifiConfigCenterTest, SetGetWifi_FAILED, TestSize.Level1)
{
    WifiOprMidState cloState = OHOS::Wifi::WifiOprMidState::CLOSED;
    WifiOprMidState runState = OHOS::Wifi::WifiOprMidState::RUNNING;
    WifiSettings::GetInstance().SetWifiState(static_cast<int>(cloState));
    EXPECT_NE((int)runState, WifiConfigCenter::GetInstance().GetWifiState());
}

HWTEST_F(WifiConfigCenterTest, SetGetScanAlwaysState_SUCCESS, TestSize.Level1)
{
    WifiSettings::GetInstance().SetScanAlwaysState(true);
    EXPECT_EQ(true, WifiConfigCenter::GetInstance().IsScanAlwaysActive());
}

HWTEST_F(WifiConfigCenterTest, SetGetScanAlwaysState_FAILED, TestSize.Level1)
{
    WifiSettings::GetInstance().SetScanAlwaysState(false);
    EXPECT_NE(true, WifiConfigCenter::GetInstance().IsScanAlwaysActive());
}

HWTEST_F(WifiConfigCenterTest, GetScanInfoList_SUCCESS, TestSize.Level1)
{
    std::vector<WifiScanInfo> setInfo;
    std::vector<WifiScanInfo> getInfo;
    WifiScanInfo info1, info2;
    info1.bssid = "asdfg";
    info2.bssid = "adsgds";
    setInfo.push_back(info1);
    setInfo.push_back(info2);
    getInfo.push_back(info1);
    getInfo.push_back(info2);
    WifiSettings::GetInstance().SaveScanInfoList(setInfo);
    WifiConfigCenter::GetInstance().GetScanInfoList(getInfo);
    EXPECT_EQ(setInfo.size(), getInfo.size());
    EXPECT_EQ(setInfo.at(0).bssid, getInfo.at(0).bssid);
    EXPECT_EQ(setInfo.at(1).bssid, getInfo.at(1).bssid);
}

HWTEST_F(WifiConfigCenterTest, GetScanInfoList_FAILED, TestSize.Level1)
{
    std::vector<WifiScanInfo> setInfo;
    std::vector<WifiScanInfo> getInfo;
    WifiScanInfo info1, info2;
    info1.bssid = "asdfg";
    info2.bssid = "adsgds";
    setInfo.push_back(info1);
    getInfo.push_back(info2);
    WifiSettings::GetInstance().SaveScanInfoList(setInfo);
    WifiConfigCenter::GetInstance().GetScanInfoList(getInfo);
    EXPECT_EQ(setInfo.size(), getInfo.size());
    EXPECT_NE(setInfo.at(0).bssid, getInfo.at(0).bssid);
}

HWTEST_F(WifiConfigCenterTest, GetScanControlInfo_SUCCESS, TestSize.Level1)
{
    ScanControlInfo setInfo, getInfo;
    ScanIntervalMode scanIntervalMode;
    int setInfoInterval, getInfoInterval;
    scanIntervalMode.scanScene = SCAN_SCENE_ALL;
    scanIntervalMode.scanMode = ScanMode::PNO_SCAN;
    scanIntervalMode.isSingle = false;
    scanIntervalMode.intervalMode = IntervalMode::INTERVAL_FIXED;
    scanIntervalMode.interval = 20;
    scanIntervalMode.count = 1;
    setInfo.scanIntervalList.push_back(scanIntervalMode);
    WifiConfigCenter::GetInstance().SetScanControlInfo(setInfo);
    WifiConfigCenter::GetInstance().GetScanControlInfo(getInfo);
    for (auto iter1 = setInfo.scanIntervalList.begin(); iter1 != setInfo.scanIntervalList.end(); iter1++) {
        if (iter1->scanScene == SCAN_SCENE_ALL && iter1->scanMode == ScanMode::PNO_SCAN) {
            setInfoInterval = iter1->interval;
        }
    }
    for (auto iter2 = getInfo.scanIntervalList.begin(); iter2 != getInfo.scanIntervalList.end(); iter2++) {
        if (iter2->scanScene == SCAN_SCENE_ALL && iter2->scanMode == ScanMode::PNO_SCAN) {
            getInfoInterval = iter2->interval;
        }
    }
    EXPECT_EQ(setInfoInterval, getInfoInterval);
}

HWTEST_F(WifiConfigCenterTest, SetGetScanControlInfo_SUCCESS, TestSize.Level1)
{
    ScanControlInfo setInfo, getInfo;
    ScanIntervalMode scanIntervalMode;
    int setInfoInterval, getInfoInterval;
    scanIntervalMode.scanScene = SCAN_SCENE_ALL;
    scanIntervalMode.scanMode = ScanMode::PNO_SCAN;
    scanIntervalMode.isSingle = false;
    scanIntervalMode.intervalMode = IntervalMode::INTERVAL_FIXED;
    scanIntervalMode.interval = 20;
    scanIntervalMode.count = 1;
    setInfo.scanIntervalList.push_back(scanIntervalMode);
    WifiConfigCenter::GetInstance().SetScanControlInfo(setInfo);
    WifiConfigCenter::GetInstance().GetScanControlInfo(getInfo);
    for (auto iter1 = setInfo.scanIntervalList.begin(); iter1 != setInfo.scanIntervalList.end(); iter1++) {
        if (iter1->scanScene == SCAN_SCENE_ALL && iter1->scanMode == ScanMode::PNO_SCAN) {
            setInfoInterval = iter1->interval;
        }
    }
    for (auto iter2 = getInfo.scanIntervalList.begin(); iter2 != getInfo.scanIntervalList.end(); iter2++) {
        if (iter2->scanScene == SCAN_SCENE_ALL && iter2->scanMode == ScanMode::PNO_SCAN) {
            getInfoInterval = iter2->interval;
        }
    }
    EXPECT_EQ(setInfoInterval, getInfoInterval);
}

HWTEST_F(WifiConfigCenterTest, GetIpInfo_SUCCESS, TestSize.Level1)
{
    IpInfo setInfo, getInfo;
    setInfo.ipAddress = 100;
    WifiSettings::GetInstance().SaveIpInfo(setInfo);
    WifiConfigCenter::GetInstance().GetIpInfo(getInfo);
    EXPECT_EQ(setInfo.ipAddress, getInfo.ipAddress);
}

HWTEST_F(WifiConfigCenterTest, GetLinkedInfo_SUCCESS, TestSize.Level1)
{
    WifiLinkedInfo setInfo, getInfo;
    setInfo.bssid = "sdfghf489375";
    WifiSettings::GetInstance().SaveLinkedInfo(setInfo);
    WifiConfigCenter::GetInstance().GetLinkedInfo(getInfo);
    EXPECT_EQ(setInfo.bssid, getInfo.bssid);
}

HWTEST_F(WifiConfigCenterTest, SetGetMacAddress_SUCCESS, TestSize.Level1)
{
    std::string getMacAddr, setMacAddr;
    setMacAddr = "192.168.1.2";
    WifiSettings::GetInstance().SetMacAddress(setMacAddr);
    WifiConfigCenter::GetInstance().GetMacAddress(getMacAddr);
    EXPECT_EQ(setMacAddr, getMacAddr);
}

HWTEST_F(WifiConfigCenterTest, SetGetApMidState_SUCCESS, TestSize.Level1)
{
    WifiOprMidState setState;
    setState = OHOS::Wifi::WifiOprMidState::OPENING;
    WifiConfigCenter::GetInstance().SetApMidState(setState);
    EXPECT_EQ(setState, WifiConfigCenter::GetInstance().GetApMidState());
}

HWTEST_F(WifiConfigCenterTest, SetApMidStateExp_SUCCESS, TestSize.Level1)
{
    WifiOprMidState setState, getState;
    setState = OHOS::Wifi::WifiOprMidState::OPENING;
    getState = OHOS::Wifi::WifiOprMidState::RUNNING;
    WifiConfigCenter::GetInstance().SetApMidState(setState);
    EXPECT_EQ(true, WifiConfigCenter::GetInstance().SetApMidState(setState, getState));
}

HWTEST_F(WifiConfigCenterTest, SetApMidStateExp_FAILED, TestSize.Level1)
{
    WifiOprMidState setState, getState;
    setState = OHOS::Wifi::WifiOprMidState::OPENING;
    getState = OHOS::Wifi::WifiOprMidState::RUNNING;
    WifiConfigCenter::GetInstance().SetApMidState(setState);
    EXPECT_NE(true, WifiConfigCenter::GetInstance().SetApMidState(getState, setState));
}

HWTEST_F(WifiConfigCenterTest, GetHotspotState_SUCCESS, TestSize.Level1)
{
    int state = 1;
    WifiSettings::GetInstance().SetHotspotState(state);
    EXPECT_EQ((int)state, WifiConfigCenter::GetInstance().GetHotspotState());
}

HWTEST_F(WifiConfigCenterTest, SetGetHotspotConfig_SUCCESS, TestSize.Level1)
{
    HotspotConfig setConfig, getConfig;
    setConfig.SetSsid("asdjhgfdksf");
    WifiConfigCenter::GetInstance().SetHotspotConfig(setConfig);
    WifiConfigCenter::GetInstance().GetHotspotConfig(getConfig);
    EXPECT_EQ(setConfig.GetSsid(), getConfig.GetSsid());
}

HWTEST_F(WifiConfigCenterTest, GetStationList_SUCCESS, TestSize.Level1)
{
    StationInfo info;
    std::vector<StationInfo> thisList, getList;
    info.bssid = "24:6f:xx:xx:xx:11";
    thisList.push_back(info);
    WifiSettings::GetInstance().ManageStation(info, MODE_ADD);
    WifiConfigCenter::GetInstance().GetStationList(getList);
    EXPECT_EQ(thisList.size(), getList.size());
    EXPECT_EQ(thisList.at(0).bssid, getList.at(0).bssid);
}

HWTEST_F(WifiConfigCenterTest, GetBlockLists_SUCCESS, TestSize.Level1)
{
    StationInfo info;
    std::vector<StationInfo> thisList, getList;
    info.bssid = "24:6f:xx:xx:xx:11";
    thisList.push_back(info);
    WifiConfigCenter::GetInstance().AddBlockList(info);
    WifiConfigCenter::GetInstance().GetBlockLists(getList);
    EXPECT_EQ(thisList.size(), getList.size());
    EXPECT_EQ(thisList.at(0).bssid, getList.at(0).bssid);
    EXPECT_EQ(0, WifiConfigCenter::GetInstance().DelBlockList(info));
}

HWTEST_F(WifiConfigCenterTest, AddDelBlockList_SUCCESS, TestSize.Level1)
{
    StationInfo info;
    info.bssid = "24:6f:xx:xx:xx:11";
    EXPECT_EQ(0, WifiConfigCenter::GetInstance().AddBlockList(info));
    EXPECT_EQ(0, WifiConfigCenter::GetInstance().DelBlockList(info));
}

HWTEST_F(WifiConfigCenterTest, GetValidBands_SUCCESS, TestSize.Level1)
{
    std::vector<BandType> bands;
    EXPECT_EQ(0, WifiConfigCenter::GetInstance().GetValidBands(bands));
}

HWTEST_F(WifiConfigCenterTest, GetValidChannels_SUCCESS, TestSize.Level1)
{
    ChannelsTable channelsInfo;
    EXPECT_EQ(0, WifiConfigCenter::GetInstance().GetValidChannels(channelsInfo));
}

HWTEST_F(WifiConfigCenterTest, SetGetScanMidState_SUCCESS, TestSize.Level1)
{
    WifiOprMidState state = OHOS::Wifi::WifiOprMidState::OPENING;
    WifiConfigCenter::GetInstance().SetScanMidState(state);
    EXPECT_EQ(state, WifiConfigCenter::GetInstance().GetScanMidState());
}

HWTEST_F(WifiConfigCenterTest, SetScanMidStateExp_SUCCESS, TestSize.Level1)
{
    WifiOprMidState state = OHOS::Wifi::WifiOprMidState::OPENING;
    WifiConfigCenter::GetInstance().SetScanMidState(state);
    EXPECT_EQ(true, WifiConfigCenter::GetInstance().SetScanMidState(state, OHOS::Wifi::WifiOprMidState::RUNNING));
}

HWTEST_F(WifiConfigCenterTest, GetSignalLevel_SUCCESS, TestSize.Level1)
{
    EXPECT_EQ(
        0, WifiConfigCenter::GetInstance().GetSignalLevel(RSSI_LEVEL_1_2G - 1, static_cast<int>(BandType::BAND_2GHZ)));
    EXPECT_EQ(
        1, WifiConfigCenter::GetInstance().GetSignalLevel(RSSI_LEVEL_1_2G, static_cast<int>(BandType::BAND_2GHZ)));
    EXPECT_EQ(
        1, WifiConfigCenter::GetInstance().GetSignalLevel(RSSI_LEVEL_2_2G - 1, static_cast<int>(BandType::BAND_2GHZ)));
    EXPECT_EQ(
        2, WifiConfigCenter::GetInstance().GetSignalLevel(RSSI_LEVEL_2_2G, static_cast<int>(BandType::BAND_2GHZ)));
    EXPECT_EQ(
        2, WifiConfigCenter::GetInstance().GetSignalLevel(RSSI_LEVEL_3_2G - 1, static_cast<int>(BandType::BAND_2GHZ)));
    EXPECT_EQ(
        3, WifiConfigCenter::GetInstance().GetSignalLevel(RSSI_LEVEL_3_2G, static_cast<int>(BandType::BAND_2GHZ)));
    EXPECT_EQ(
        3, WifiConfigCenter::GetInstance().GetSignalLevel(RSSI_LEVEL_4_2G - 1, static_cast<int>(BandType::BAND_2GHZ)));
    EXPECT_EQ(
        4, WifiConfigCenter::GetInstance().GetSignalLevel(RSSI_LEVEL_4_2G, static_cast<int>(BandType::BAND_2GHZ)));
    EXPECT_EQ(
        0, WifiConfigCenter::GetInstance().GetSignalLevel(RSSI_LEVEL_1_5G - 1, static_cast<int>(BandType::BAND_5GHZ)));
    EXPECT_EQ(
        1, WifiConfigCenter::GetInstance().GetSignalLevel(RSSI_LEVEL_1_5G, static_cast<int>(BandType::BAND_5GHZ)));
    EXPECT_EQ(
        1, WifiConfigCenter::GetInstance().GetSignalLevel(RSSI_LEVEL_2_5G - 1, static_cast<int>(BandType::BAND_5GHZ)));
    EXPECT_EQ(
        2, WifiConfigCenter::GetInstance().GetSignalLevel(RSSI_LEVEL_2_5G, static_cast<int>(BandType::BAND_5GHZ)));
    EXPECT_EQ(
        2, WifiConfigCenter::GetInstance().GetSignalLevel(RSSI_LEVEL_3_5G - 1, static_cast<int>(BandType::BAND_5GHZ)));
    EXPECT_EQ(
        3, WifiConfigCenter::GetInstance().GetSignalLevel(RSSI_LEVEL_3_5G, static_cast<int>(BandType::BAND_5GHZ)));
    EXPECT_EQ(
        3, WifiConfigCenter::GetInstance().GetSignalLevel(RSSI_LEVEL_4_5G - 1, static_cast<int>(BandType::BAND_5GHZ)));
    EXPECT_EQ(
        4, WifiConfigCenter::GetInstance().GetSignalLevel(RSSI_LEVEL_4_5G, static_cast<int>(BandType::BAND_5GHZ)));
}

HWTEST_F(WifiConfigCenterTest, SetGetStaLastRunState_SUCCESS, TestSize.Level1)
{
    bool mode = true;
    WifiConfigCenter::GetInstance().SetStaLastRunState(mode);
    EXPECT_EQ(mode, WifiConfigCenter::GetInstance().GetStaLastRunState());
}

HWTEST_F(WifiConfigCenterTest, SetGetScreenState_SUCCESS, TestSize.Level1)
{
    int mode = 1;
    WifiConfigCenter::GetInstance().SetScreenState(mode);
    EXPECT_EQ(mode, WifiConfigCenter::GetInstance().GetScreenState());
}

HWTEST_F(WifiConfigCenterTest, SetGetAirplaneModeState_SUCCESS, TestSize.Level1)
{
    int mode = 1;
    WifiConfigCenter::GetInstance().SetAirplaneModeState(mode);
    EXPECT_EQ(mode, WifiConfigCenter::GetInstance().GetAirplaneModeState());
}

HWTEST_F(WifiConfigCenterTest, SetGetAppRunningState_SUCCESS, TestSize.Level1)
{
    ScanMode mode = ScanMode::APP_BACKGROUND_SCAN;
    WifiConfigCenter::GetInstance().SetAppRunningState(mode);
    EXPECT_EQ(mode, WifiConfigCenter::GetInstance().GetAppRunningState());
}

HWTEST_F(WifiConfigCenterTest, SetGetPowerSavingModeState_SUCCESS, TestSize.Level1)
{
    int mode = 1;
    WifiConfigCenter::GetInstance().SetPowerSavingModeState(mode);
    EXPECT_EQ(mode, WifiConfigCenter::GetInstance().GetPowerSavingModeState());
}

HWTEST_F(WifiConfigCenterTest, SettingsInit_SUCCESS, TestSize.Level1)
{
    EXPECT_EQ(0, WifiSettings::GetInstance().Init());
}

HWTEST_F(WifiConfigCenterTest, SetGetWifiStaCapabilities_SUCCESS, TestSize.Level1)
{
    int cap = 3;
    EXPECT_EQ(0, WifiSettings::GetInstance().SetWifiStaCapabilities(cap));
    EXPECT_EQ(cap, WifiSettings::GetInstance().GetWifiStaCapabilities());
}

HWTEST_F(WifiConfigCenterTest, GetDeviceConfig_SUCCESS, TestSize.Level1)
{
    std::string bssid = "asdgdsklf";
    WifiDeviceConfig config, getConfig;
    config.networkId = 15;
    config.bssid = "asdgdsklf";
    WifiSettings::GetInstance().AddDeviceConfig(config);
    WifiSettings::GetInstance().GetDeviceConfig(bssid, 1, getConfig);
    EXPECT_EQ(0, WifiSettings::GetInstance().RemoveDevice(config.networkId));
    EXPECT_EQ(config.bssid, getConfig.bssid);
    EXPECT_EQ(config.networkId, getConfig.networkId);
}

HWTEST_F(WifiConfigCenterTest, GetHiddenDeviceConfig_SUCCESS, TestSize.Level1)
{
    WifiDeviceConfig config1, config2;
    config1.networkId = 15;
    config1.bssid = "asdgdsklf";
    config1.hiddenSSID = true;
    config2.networkId = 16;
    config2.bssid = "asdgdssfklf";
    config2.hiddenSSID = false;
    std::vector<WifiDeviceConfig> configArr;
    WifiSettings::GetInstance().AddDeviceConfig(config1);
    WifiSettings::GetInstance().AddDeviceConfig(config2);
    WifiSettings::GetInstance().GetHiddenDeviceConfig(configArr);
    EXPECT_EQ(0, WifiSettings::GetInstance().RemoveDevice(config1.networkId));
    EXPECT_EQ(0, WifiSettings::GetInstance().RemoveDevice(config2.networkId));
    EXPECT_TRUE(1 == configArr.size());
    EXPECT_EQ(config1.networkId, configArr.at(0).networkId);
    EXPECT_EQ(config1.bssid, configArr.at(0).bssid);
    EXPECT_EQ(config1.hiddenSSID, configArr.at(0).hiddenSSID);
}

HWTEST_F(WifiConfigCenterTest, SyncDeviceConfig_SUCCESS, TestSize.Level1)
{
    EXPECT_EQ(0, WifiSettings::GetInstance().SyncDeviceConfig());
}

HWTEST_F(WifiConfigCenterTest, ReloadDeviceConfig_SUCCESS, TestSize.Level1)
{
    EXPECT_EQ(0, WifiSettings::GetInstance().ReloadDeviceConfig());
}

HWTEST_F(WifiConfigCenterTest, SyncHotspotConfig_SUCCESS, TestSize.Level1)
{
    EXPECT_EQ(0, WifiSettings::GetInstance().SyncHotspotConfig());
}

HWTEST_F(WifiConfigCenterTest, ClearStationList_SUCCESS, TestSize.Level1)
{
    EXPECT_EQ(0, WifiSettings::GetInstance().ClearStationList());
}

HWTEST_F(WifiConfigCenterTest, SyncBlockList_SUCCESS, TestSize.Level1)
{
    EXPECT_EQ(0, WifiSettings::GetInstance().SyncBlockList());
}

HWTEST_F(WifiConfigCenterTest, SetGetValidChannels_SUCCESS, TestSize.Level1)
{
    ChannelsTable setInfos, getInfos;
    std::vector<int> chan2G = {1324, 32, 324, 423, 43214};
    std::vector<int> chan5G = {5435, 4353, 5432, 5432};
    chan2G.push_back(2145);
    setInfos[BandType::BAND_2GHZ] = chan2G;
    setInfos[BandType::BAND_5GHZ] = chan5G;
    EXPECT_EQ(0, WifiSettings::GetInstance().SetValidChannels(setInfos));
    EXPECT_EQ(0, WifiSettings::GetInstance().GetValidChannels(getInfos));
    EXPECT_EQ(setInfos.size(), getInfos.size());
    EXPECT_EQ(setInfos.at(BandType::BAND_2GHZ).size(), getInfos.at(BandType::BAND_2GHZ).size());
    EXPECT_EQ(setInfos.at(BandType::BAND_5GHZ).size(), getInfos.at(BandType::BAND_5GHZ).size());
}

HWTEST_F(WifiConfigCenterTest, ClearValidChannels_SUCCESS, TestSize.Level1)
{
    EXPECT_EQ(0, WifiSettings::GetInstance().ClearValidChannels());
}

HWTEST_F(WifiConfigCenterTest, GetApMaxConnNum_SUCCESS, TestSize.Level1)
{
    WifiSettings::GetInstance().Init();
    EXPECT_EQ(MAX_AP_CONN, WifiSettings::GetInstance().GetApMaxConnNum());
}

HWTEST_F(WifiConfigCenterTest, EnableNetwork_SUCCESS, TestSize.Level1)
{
    int networkId = 15;
    bool disableOthers = true;
    EXPECT_EQ(true, WifiSettings::GetInstance().EnableNetwork(networkId, disableOthers));
}

HWTEST_F(WifiConfigCenterTest, SetGetUserLastSelectedNetworkId_SUCCESS, TestSize.Level1)
{
    int networkId = 15;
    WifiSettings::GetInstance().SetUserLastSelectedNetworkId(networkId);
    EXPECT_EQ(networkId, WifiSettings::GetInstance().GetUserLastSelectedNetworkId());
}

HWTEST_F(WifiConfigCenterTest, SyncWifiConfig_SUCCESS, TestSize.Level1)
{
    EXPECT_EQ(0, WifiSettings::GetInstance().SyncWifiConfig());
}

HWTEST_F(WifiConfigCenterTest, SetGetDhcpIpType_SUCCESS, TestSize.Level1)
{
    int stat = 1234;
    EXPECT_EQ(0, WifiSettings::GetInstance().SetDhcpIpType(stat));
    EXPECT_EQ(stat, WifiSettings::GetInstance().GetDhcpIpType());
}

HWTEST_F(WifiConfigCenterTest, SetGetScoretacticsScoreSlope_SUCCESS, TestSize.Level1)
{
    int stat = 134;
    EXPECT_EQ(0, WifiSettings::GetInstance().SetScoretacticsScoreSlope(stat));
    EXPECT_EQ(stat, WifiSettings::GetInstance().GetScoretacticsScoreSlope());
}

HWTEST_F(WifiConfigCenterTest, SetGetScoretacticsSameBssidScore_SUCCESS, TestSize.Level1)
{
    bool stat = true;
    EXPECT_EQ(0, WifiSettings::GetInstance().SetScoretacticsSameBssidScore(stat));
    EXPECT_EQ(stat, WifiSettings::GetInstance().GetScoretacticsSameBssidScore());
}

HWTEST_F(WifiConfigCenterTest, SetGetScoretacticsSameNetworkScore_SUCCESS, TestSize.Level1)
{
    int stat = 1234;
    EXPECT_EQ(0, WifiSettings::GetInstance().SetScoretacticsSameNetworkScore(stat));
    EXPECT_EQ(stat, WifiSettings::GetInstance().GetScoretacticsSameNetworkScore());
}

HWTEST_F(WifiConfigCenterTest, SetGetScoretacticsFrequency5GHzScore_SUCCESS, TestSize.Level1)
{
    int stat = 234;
    EXPECT_EQ(0, WifiSettings::GetInstance().SetScoretacticsFrequency5GHzScore(stat));
    EXPECT_EQ(stat, WifiSettings::GetInstance().GetScoretacticsFrequency5GHzScore());
}

HWTEST_F(WifiConfigCenterTest, SetGetScoretacticsLastSelectionScore_SUCCESS, TestSize.Level1)
{
    int stat = 1234;
    EXPECT_EQ(0, WifiSettings::GetInstance().SetScoretacticsLastSelectionScore(stat));
    EXPECT_EQ(stat, WifiSettings::GetInstance().GetScoretacticsLastSelectionScore());
}

HWTEST_F(WifiConfigCenterTest, SetGetScoretacticsSecurityScore_SUCCESS, TestSize.Level1)
{
    int stat = 123;
    EXPECT_EQ(0, WifiSettings::GetInstance().SetScoretacticsSecurityScore(stat));
    EXPECT_EQ(stat, WifiSettings::GetInstance().GetScoretacticsSecurityScore());
}

HWTEST_F(WifiConfigCenterTest, SetGetSavedDeviceAppraisalPriority_SUCCESS, TestSize.Level1)
{
    int stat = 532;
    EXPECT_EQ(0, WifiSettings::GetInstance().SetSavedDeviceAppraisalPriority(stat));
    EXPECT_EQ(stat, WifiSettings::GetInstance().GetSavedDeviceAppraisalPriority());
}

HWTEST_F(WifiConfigCenterTest, GetP2pStateTest, TestSize.Level1)
{
    WifiSettings::GetInstance().SetP2pState(1);
    EXPECT_TRUE(WifiConfigCenter::GetInstance().GetP2pState() == 1);
}

HWTEST_F(WifiConfigCenterTest, SetGetP2pMidState_SUCCESS, TestSize.Level1)
{
    WifiOprMidState state = WifiOprMidState::RUNNING;
    WifiConfigCenter::GetInstance().SetP2pMidState(state);
    EXPECT_EQ(state, WifiConfigCenter::GetInstance().GetP2pMidState());
}

HWTEST_F(WifiConfigCenterTest, SetP2pMidStateExp_SUCCESS, TestSize.Level1)
{
    WifiOprMidState cloState = WifiOprMidState::CLOSED;
    WifiOprMidState runState = WifiOprMidState::RUNNING;
    WifiConfigCenter::GetInstance().SetP2pMidState(cloState);
    EXPECT_EQ(true, WifiConfigCenter::GetInstance().SetP2pMidState(cloState, runState));
}

HWTEST_F(WifiConfigCenterTest, SetP2pMidStateExp_FAILED, TestSize.Level1)
{
    WifiOprMidState cloState = WifiOprMidState::CLOSED;
    WifiOprMidState runState = WifiOprMidState::RUNNING;
    WifiConfigCenter::GetInstance().SetP2pMidState(cloState);
    EXPECT_NE(true, WifiConfigCenter::GetInstance().SetP2pMidState(runState, cloState));
}

HWTEST_F(WifiConfigCenterTest, GetCanOpenStaWhenAirplaneModeTest, TestSize.Level1)
{
    EXPECT_TRUE(WifiConfigCenter::GetInstance().GetCanOpenStaWhenAirplaneMode() == false);
}

HWTEST_F(WifiConfigCenterTest, SetWifiStateWhenAirplaneModeTest, TestSize.Level1)
{
    WifiConfigCenter::GetInstance().SetWifiStateWhenAirplaneMode(false);
    EXPECT_TRUE(WifiConfigCenter::GetInstance().GetWifiStateWhenAirplaneMode() == false);
    WifiConfigCenter::GetInstance().SetWifiStateWhenAirplaneMode(true);
    EXPECT_TRUE(WifiConfigCenter::GetInstance().GetWifiStateWhenAirplaneMode() == true);
}
}  // namespace Wifi
}  // namespace OHOS