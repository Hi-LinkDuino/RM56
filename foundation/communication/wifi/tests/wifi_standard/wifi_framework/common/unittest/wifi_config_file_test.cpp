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
#include "wifi_config_file_test.h"

using namespace testing::ext;

namespace OHOS {
namespace Wifi {
HWTEST_F(WifiConfigFileTest, LoadConfig, TestSize.Level1)
{
    int ret = mWifiCfg.LoadConfig();
    if (ret < 0) {
        mWifiCfg.SaveConfig();
        ret = mWifiCfg.LoadConfig();
    }
    EXPECT_EQ(ret, 0);
}

HWTEST_F(WifiConfigFileTest, SaveConfig, TestSize.Level1)
{
    EXPECT_EQ(mWifiCfg.SaveConfig(), 0);
}

HWTEST_F(WifiConfigFileTest, SaveValueCheck, TestSize.Level1)
{
    WifiConfig tmp;
    tmp.defaultWifiInterface = "abcd";
    tmp.scanAlwaysSwitch = true;
    tmp.staAirplaneMode = true;
    tmp.staLastState = true;
    tmp.savedDeviceAppraisalPriority = PRIORITY_1;
    tmp.scoretacticsScoreSlope = SCORE_SLOPE;
    tmp.scoretacticsInitScore = INIT_SCORE;
    tmp.scoretacticsSameBssidScore = SAME_BSSID_SCORE;
    tmp.scoretacticsSameNetworkScore = SAME_NETWORK_SCORE;
    tmp.scoretacticsFrequency5GHzScore = FREQUENCY_5_GHZ_SCORE;
    tmp.scoretacticsLastSelectionScore = LAST_SELECTION_SCORE;
    tmp.scoretacticsSecurityScore = SECURITY_SCORE;
    tmp.dhcpIpType = static_cast<int>(DhcpIpType::DHCP_IPTYPE_IPV4);
    std::vector<WifiConfig> vec;
    vec.push_back(tmp);
    mWifiCfg.SetValue(vec);
    mWifiCfg.SaveConfig();
    mWifiCfg.LoadConfig();
    vec.clear();
    mWifiCfg.GetValue(vec);
    EXPECT_TRUE(vec.size() == 1);
    WifiConfig ldCfg = vec[0];
    EXPECT_EQ(tmp.defaultWifiInterface, ldCfg.defaultWifiInterface);
    EXPECT_EQ(tmp.scanAlwaysSwitch, ldCfg.scanAlwaysSwitch);
    EXPECT_EQ(tmp.staAirplaneMode, ldCfg.staAirplaneMode);
    EXPECT_EQ(tmp.staLastState, ldCfg.staLastState);
    EXPECT_EQ(tmp.savedDeviceAppraisalPriority, ldCfg.savedDeviceAppraisalPriority);
    EXPECT_EQ(tmp.scoretacticsScoreSlope, ldCfg.scoretacticsScoreSlope);
    EXPECT_EQ(tmp.scoretacticsInitScore, ldCfg.scoretacticsInitScore);
    EXPECT_EQ(tmp.scoretacticsSameBssidScore, ldCfg.scoretacticsSameBssidScore);
    EXPECT_EQ(tmp.scoretacticsSameNetworkScore, ldCfg.scoretacticsSameNetworkScore);
    EXPECT_EQ(tmp.scoretacticsFrequency5GHzScore, ldCfg.scoretacticsFrequency5GHzScore);
    EXPECT_EQ(tmp.scoretacticsLastSelectionScore, ldCfg.scoretacticsLastSelectionScore);
    EXPECT_EQ(tmp.scoretacticsSecurityScore, ldCfg.scoretacticsSecurityScore);
    EXPECT_EQ(tmp.dhcpIpType, ldCfg.dhcpIpType);
}

HWTEST_F(WifiConfigFileTest, LoadDeviceConfig, TestSize.Level1)
{
    int ret = mDeviceCfg.LoadConfig();
    if (ret < 0) {
        mDeviceCfg.SaveConfig();
        ret = mDeviceCfg.LoadConfig();
    }
    EXPECT_EQ(ret, 0);
}

HWTEST_F(WifiConfigFileTest, SaveDeviceConfig, TestSize.Level1)
{
    EXPECT_EQ(mDeviceCfg.SaveConfig(), 0);
}

HWTEST_F(WifiConfigFileTest, SaveDeviceValueCheck, TestSize.Level1)
{
    WifiDeviceConfig tmp;
    tmp.status = static_cast<int>(WifiDeviceConfigStatus::ENABLED);
    tmp.bssid = "01:xx:xx:xx:xx:06";
    tmp.ssid = "my wifi";
    tmp.band = static_cast<int>(BandType::BAND_2GHZ);
    tmp.channel = 1;
    tmp.isPasspoint = true;
    tmp.isEphemeral = true;
    tmp.preSharedKey = "12345678";
    tmp.keyMgmt = "NONE";
    tmp.wepKeys[0] = "12345678";
    tmp.wepKeys[1] = "12345679";
    tmp.wepKeys[1 + 1] = "abcdefgh";
    tmp.wepKeys[1 + 1 + 1] = "0000abcd";
    tmp.wepTxKeyIndex = 1;
    tmp.priority = 1;
    tmp.hiddenSSID = true;
    std::vector<WifiDeviceConfig> vec;
    vec.push_back(tmp);
    mDeviceCfg.SetValue(vec);
    mDeviceCfg.SaveConfig();
    mDeviceCfg.LoadConfig();
    vec.clear();
    mDeviceCfg.GetValue(vec);
    EXPECT_TRUE(vec.size() == 1);
    WifiDeviceConfig ldCfg = vec[0];
    EXPECT_TRUE(tmp.status == ldCfg.status);
    EXPECT_TRUE(tmp.bssid == ldCfg.bssid);
    EXPECT_TRUE(tmp.ssid == ldCfg.ssid);
    EXPECT_TRUE(tmp.band == ldCfg.band);
    EXPECT_TRUE(tmp.channel == ldCfg.channel);
    EXPECT_TRUE(tmp.isPasspoint == ldCfg.isPasspoint);
    EXPECT_TRUE(tmp.isEphemeral == ldCfg.isEphemeral);
    EXPECT_TRUE(tmp.preSharedKey == ldCfg.preSharedKey);
    EXPECT_TRUE(tmp.keyMgmt == ldCfg.keyMgmt);
    EXPECT_TRUE(tmp.wepTxKeyIndex == ldCfg.wepTxKeyIndex);
    EXPECT_TRUE(tmp.priority == ldCfg.priority);
    EXPECT_TRUE(tmp.hiddenSSID == ldCfg.hiddenSSID);
    for (int i = 0; i < WEPKEYS_SIZE; ++i) {
        EXPECT_TRUE(tmp.wepKeys[i] == ldCfg.wepKeys[i]);
    }
}

HWTEST_F(WifiConfigFileTest, LoadHotspotConfig, TestSize.Level1)
{
    int ret = mHotspotCfg.LoadConfig();
    if (ret < 0) {
        mHotspotCfg.SaveConfig();
        ret = mHotspotCfg.LoadConfig();
    }
    EXPECT_EQ(ret, 0);
}

HWTEST_F(WifiConfigFileTest, SaveHotspotConfig, TestSize.Level1)
{
    EXPECT_EQ(mHotspotCfg.SaveConfig(), 0);
}

HWTEST_F(WifiConfigFileTest, SaveHotspotValueCheck, TestSize.Level1)
{
    HotspotConfig tmp;
    tmp.SetSsid("my wifi");
    tmp.SetPreSharedKey("12345678");
    tmp.SetSecurityType(KeyMgmt::WPA_PSK);
    tmp.SetBand(BandType::BAND_5GHZ);
    tmp.SetChannel(1);
    tmp.SetMaxConn(1);
    std::vector<HotspotConfig> vec;
    vec.push_back(tmp);
    mHotspotCfg.SetValue(vec);
    mHotspotCfg.SaveConfig();
    mHotspotCfg.LoadConfig();
    vec.clear();
    mHotspotCfg.GetValue(vec);
    EXPECT_TRUE(vec.size() == 1);
    HotspotConfig ldCfg = vec[0];
    EXPECT_TRUE(tmp.GetSsid() == ldCfg.GetSsid());
    EXPECT_TRUE(tmp.GetPreSharedKey() == ldCfg.GetPreSharedKey());
    EXPECT_TRUE(tmp.GetSecurityType() == ldCfg.GetSecurityType());
    EXPECT_TRUE(tmp.GetBand() == ldCfg.GetBand());
    EXPECT_TRUE(tmp.GetChannel() == ldCfg.GetChannel());
    EXPECT_TRUE(tmp.GetMaxConn() == ldCfg.GetMaxConn());
}

HWTEST_F(WifiConfigFileTest, LoadBlockConfig, TestSize.Level1)
{
    int ret = mBlockCfg.LoadConfig();
    if (ret < 0) {
        mBlockCfg.SaveConfig();
        ret = mBlockCfg.LoadConfig();
    }
    EXPECT_EQ(ret, 0);
}

HWTEST_F(WifiConfigFileTest, SaveBlockConfig, TestSize.Level1)
{
    EXPECT_EQ(mBlockCfg.SaveConfig(), 0);
}

HWTEST_F(WifiConfigFileTest, SaveBlockValueCheck, TestSize.Level1)
{
    StationInfo tmp;
    tmp.deviceName = "my wifi";
    tmp.bssid = "01:xx:xx:xx:xx:06";
    tmp.ipAddr = "192.xxx.x.1";
    std::vector<StationInfo> vec;
    vec.push_back(tmp);
    tmp.deviceName = "my wifi 2";
    tmp.bssid = "01:xx:xx:xx:xx:07";
    tmp.ipAddr = "192.xxx.x.2";
    vec.push_back(tmp);
    mBlockCfg.SetValue(vec);
    mBlockCfg.SaveConfig();
    mBlockCfg.LoadConfig();
    vec.clear();
    mBlockCfg.GetValue(vec);
    EXPECT_TRUE(vec.size() == 2);
    StationInfo ldCfg = vec[1];
    EXPECT_TRUE(tmp.deviceName == ldCfg.deviceName);
    EXPECT_TRUE(tmp.bssid == ldCfg.bssid);
    EXPECT_TRUE(tmp.ipAddr == ldCfg.ipAddr);
}

static void AddWifiP2pGroupInfoTestFirst(std::vector<WifiP2pGroupInfo> &vec)
{
    WifiP2pGroupInfo item;
    item.SetGroupName("net_1");
    item.SetIsGroupOwner(true);
    WifiP2pDevice device;
    device.SetDeviceName("device_1");
    device.SetDeviceAddress("devAddr_1");
    device.SetPrimaryDeviceType("priDevType_1");
    device.SetSecondaryDeviceType("secDevType_1");
    device.SetP2pDeviceStatus(P2pDeviceStatus::PDS_CONNECTED);
    device.SetWpsConfigMethod((int)WpsConfigMethod::WPS_CFG_PUSHBUTTON);
    device.SetDeviceCapabilitys((int)P2pDeviceCapability::PDC_CLIENT_DISCOVERABILITY);
    device.SetGroupCapabilitys((int)P2pGroupCapability::PGC_INTRA_BSS_DIST);
    item.SetOwner(device);
    item.SetPassphrase("passphrase_1");
    item.SetInterface("interface_1");
    item.SetGroupName("name_1");
    item.SetFrequency(0);
    item.SetIsPersistent(true);
    item.SetP2pGroupStatus(P2pGroupStatus::GS_CREATED);
    item.SetNetworkId(0);
    WifiP2pDevice client;
    client.SetDeviceName("device_2");
    client.SetDeviceAddress("devAddr_2");
    client.SetPrimaryDeviceType("priDevType_2");
    client.SetSecondaryDeviceType("secDevType_2");
    client.SetP2pDeviceStatus(P2pDeviceStatus::PDS_CONNECTED);
    client.SetWpsConfigMethod((int)WpsConfigMethod::WPS_CFG_KEYPAD);
    client.SetDeviceCapabilitys((int)P2pDeviceCapability::PDC_CLIENT_DISCOVERABILITY);
    client.SetGroupCapabilitys((int)P2pGroupCapability::PGC_GROUP_OWNER);
    WifiP2pDevice clientSecond;
    clientSecond.SetDeviceName("device_3");
    clientSecond.SetDeviceAddress("devAddr_3");
    clientSecond.SetPrimaryDeviceType("priDevType_3");
    clientSecond.SetSecondaryDeviceType("secDevType_3");
    clientSecond.SetP2pDeviceStatus(P2pDeviceStatus::PDS_CONNECTED);
    clientSecond.SetWpsConfigMethod((int)WpsConfigMethod::WPS_CFG_INVALID);
    clientSecond.SetDeviceCapabilitys((int)P2pDeviceCapability::PDC_STARTED_CONCURRENT_OPER);
    clientSecond.SetGroupCapabilitys((int)P2pGroupCapability::PGC_GROUP_OWNER);
    item.AddClientDevice(client);
    item.AddClientDevice(clientSecond);
    vec.push_back(item);
    return;
}

static void AddWifiP2pGroupInfoTestSecond(std::vector<WifiP2pGroupInfo> &vec)
{
    WifiP2pGroupInfo item;
    item.SetGroupName("net_2");
    item.SetIsGroupOwner(false);
    WifiP2pDevice device;
    device.SetDeviceName("device_4");
    device.SetDeviceAddress("devAddr_4");
    device.SetPrimaryDeviceType("priDevType_4");
    device.SetSecondaryDeviceType("secDevType_4");
    device.SetP2pDeviceStatus(P2pDeviceStatus::PDS_INVITED);
    device.SetWpsConfigMethod((int)WpsConfigMethod::WPS_CFG_DISPLAY);
    device.SetDeviceCapabilitys((int)P2pDeviceCapability::PDC_CLIENT_DISCOVERABILITY);
    device.SetGroupCapabilitys((int)P2pGroupCapability::PGC_INTRA_BSS_DIST);
    item.SetOwner(device);
    item.SetPassphrase("passphrase_2");
    item.SetInterface("interface_2");
    item.SetGroupName("name_2");
    item.SetFrequency(1);
    item.SetIsPersistent(false);
    item.SetP2pGroupStatus(P2pGroupStatus::GS_STARTED);
    item.SetNetworkId(1);
    WifiP2pDevice client;
    client.SetDeviceName("device_5");
    client.SetDeviceAddress("devAddr_5");
    client.SetPrimaryDeviceType("priDevType_5");
    client.SetSecondaryDeviceType("secDevType_5");
    client.SetP2pDeviceStatus(P2pDeviceStatus::PDS_CONNECTED);
    client.SetWpsConfigMethod((int)WpsConfigMethod::WPS_CFG_KEYPAD);
    client.SetDeviceCapabilitys((int)P2pDeviceCapability::PDC_SERVICE_DISCOVERY);
    client.SetGroupCapabilitys((int)P2pGroupCapability::PGC_INTRA_BSS_DIST);
    WifiP2pDevice clientSecond;
    clientSecond.SetDeviceName("device_6");
    clientSecond.SetDeviceAddress("devAddr_6");
    clientSecond.SetPrimaryDeviceType("priDevType_6");
    clientSecond.SetSecondaryDeviceType("secDevType_6");
    clientSecond.SetP2pDeviceStatus(P2pDeviceStatus::PDS_CONNECTED);
    clientSecond.SetWpsConfigMethod((int)WpsConfigMethod::WPS_CFG_INVALID);
    clientSecond.SetDeviceCapabilitys((int)P2pDeviceCapability::PDC_REMOVING_INFRA_MANAGED);
    clientSecond.SetGroupCapabilitys((int)P2pGroupCapability::PGC_GROUP_OWNER);
    item.AddClientDevice(client);
    item.AddClientDevice(clientSecond);
    vec.push_back(item);
    return;
}

HWTEST_F(WifiConfigFileTest, SaveLoadWifiP2pGroupInfo_SUCCESS, TestSize.Level1)
{
    std::vector<WifiP2pGroupInfo> vec;
    AddWifiP2pGroupInfoTestFirst(vec);
    AddWifiP2pGroupInfoTestSecond(vec);
    mWifiP2pGroupInfo.SetValue(vec);
    mWifiP2pGroupInfo.SaveConfig();
    mWifiP2pGroupInfo.LoadConfig();
    unsigned orgSize = vec.size();
    vec.clear();
    mWifiP2pGroupInfo.GetValue(vec);
    EXPECT_TRUE(vec.size() == orgSize);
    EXPECT_EQ(true, (strcmp(vec.at(1).GetClientDevices().at(1).GetDeviceName().c_str(), "device_6") == 0));
    EXPECT_EQ(P2pGroupCapability::PGC_GROUP_OWNER,
        static_cast<P2pGroupCapability>(vec.at(1).GetClientDevices().at(1).GetGroupCapabilitys()));
}
}  // namespace Wifi
}  // namespace OHOS