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
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "wifi_p2p_group_manager.h"
#include "mock_wifi_settings.h"

using ::testing::_;
using ::testing::Return;
using ::testing::ext::TestSize;

namespace OHOS {
namespace Wifi {
const int SET_FREQUENCY = 6;
const int SET_NETWORKID = 10;

class WifiP2pGroupManagerTest : public testing::Test {
public:
    static void SetUpTestCase(){}
    static void TearDownTestCase(){}
    virtual void SetUp()
    {
        pWifiP2pGroupManager.reset(new WifiP2pGroupManager());
        device.SetDeviceName(std::string("UnitTestDevice"));
        device.SetDeviceAddress(std::string("12:34:56:78:9a:bc"));
        device.SetPrimaryDeviceType(std::string("UnitTestPrimaryDeviceType"));
        device.SetP2pDeviceStatus(P2pDeviceStatus::PDS_UNAVAILABLE);
        device.SetWpsConfigMethod(static_cast<int>(WpsConfigMethod::WPS_CFG_INVALID));

        groupInfo.SetIsGroupOwner(true);
        groupInfo.SetOwner(device);
        groupInfo.SetPassphrase(std::string("UnitTestPassphrase"));
        groupInfo.SetInterface(std::string("UnitTestWlan"));
        groupInfo.SetGroupName(std::string("UnitTestGroupName"));
        groupInfo.SetFrequency(SET_FREQUENCY);
        groupInfo.SetIsPersistent(true);
        groupInfo.SetP2pGroupStatus(P2pGroupStatus::GS_INVALID);
        groupInfo.SetNetworkId(SET_NETWORKID);
        groupInfo.AddClientDevice(device);
    }
    virtual void TearDown()
    {
        pWifiP2pGroupManager.reset();
    }
    WifiP2pGroupInfo groupInfo;
    WifiP2pDevice device;
    std::unique_ptr<WifiP2pGroupManager> pWifiP2pGroupManager;
    void WarpRefreshGroupsFromCurrentGroup()
    {
        pWifiP2pGroupManager->RefreshGroupsFromCurrentGroup();
    }
    void WarpRefreshCurrentGroupFromGroups()
    {
        pWifiP2pGroupManager->RefreshCurrentGroupFromGroups();
    }
};

HWTEST_F(WifiP2pGroupManagerTest, Initialize_SUCCESS, TestSize.Level1)
{
    pWifiP2pGroupManager->Initialize();
}

HWTEST_F(WifiP2pGroupManagerTest, StashGroups_SUCCESS, TestSize.Level1)
{
    pWifiP2pGroupManager->StashGroups();
}

HWTEST_F(WifiP2pGroupManagerTest, AddGroup_SUCCESS, TestSize.Level1)
{
    EXPECT_TRUE(pWifiP2pGroupManager->AddGroup(groupInfo));
    EXPECT_TRUE(pWifiP2pGroupManager->AddGroup(groupInfo));
}

HWTEST_F(WifiP2pGroupManagerTest, RemoveGroup, TestSize.Level1)
{
    EXPECT_FALSE(pWifiP2pGroupManager->RemoveGroup(groupInfo));
    EXPECT_TRUE(pWifiP2pGroupManager->AddGroup(groupInfo));
    EXPECT_TRUE(pWifiP2pGroupManager->RemoveGroup(groupInfo));
}

HWTEST_F(WifiP2pGroupManagerTest, ClearAll_SUCCESS, TestSize.Level1)
{
    pWifiP2pGroupManager->AddGroup(groupInfo);
    EXPECT_EQ(1, pWifiP2pGroupManager->ClearAll());
}

HWTEST_F(WifiP2pGroupManagerTest, UpdateWpaGroup_SUCCESS, TestSize.Level1)
{
    pWifiP2pGroupManager->UpdateWpaGroup(groupInfo);
    pWifiP2pGroupManager->UpdateWpaGroup(groupInfo);
}

HWTEST_F(WifiP2pGroupManagerTest, RemoveClientFromGroup_SUCCESS, TestSize.Level1)
{
    EXPECT_EQ(-1, pWifiP2pGroupManager->RemoveClientFromGroup(10, "AA:BB:CC:DD:EE:FF"));

    pWifiP2pGroupManager->AddGroup(groupInfo);
    pWifiP2pGroupManager->RemoveClientFromGroup(10, "12:34:56:78:9a:bc");
}

HWTEST_F(WifiP2pGroupManagerTest, GetGroups_SUCCESS, TestSize.Level1)
{
    std::vector<WifiP2pGroupInfo> groups, groups1;
    pWifiP2pGroupManager->AddGroup(groupInfo);
    groups1.push_back(groupInfo);
    EXPECT_EQ(groups1, pWifiP2pGroupManager->GetGroups());
}

HWTEST_F(WifiP2pGroupManagerTest, GetNetworkIdFromClients_SUCCESS, TestSize.Level1)
{
    EXPECT_EQ(-1, pWifiP2pGroupManager->GetNetworkIdFromClients(device));
    pWifiP2pGroupManager->AddGroup(groupInfo);
    EXPECT_EQ(10, pWifiP2pGroupManager->GetNetworkIdFromClients(device));
}

HWTEST_F(WifiP2pGroupManagerTest, GetNetworkId1_SUCCESS, TestSize.Level1)
{
    EXPECT_EQ(-1, pWifiP2pGroupManager->GetGroupNetworkId(device));
    pWifiP2pGroupManager->AddGroup(groupInfo);
    EXPECT_EQ(10, pWifiP2pGroupManager->GetGroupNetworkId(device));
}

HWTEST_F(WifiP2pGroupManagerTest, GetNetworkId2_SUCCESS, TestSize.Level1)
{
    EXPECT_EQ(-1, pWifiP2pGroupManager->GetGroupNetworkId(device, "name"));
    pWifiP2pGroupManager->AddGroup(groupInfo);
    EXPECT_EQ(10, pWifiP2pGroupManager->GetGroupNetworkId(device, "UnitTestGroupName"));
}

HWTEST_F(WifiP2pGroupManagerTest, GetOwnerAddr_SUCCESS, TestSize.Level1)
{
    EXPECT_EQ("", pWifiP2pGroupManager->GetGroupOwnerAddr(10));
    pWifiP2pGroupManager->AddGroup(groupInfo);
    EXPECT_EQ("12:34:56:78:9a:bc", pWifiP2pGroupManager->GetGroupOwnerAddr(10));
}

HWTEST_F(WifiP2pGroupManagerTest, Contains_SUCCESS, TestSize.Level1)
{
    WifiP2pLinkedInfo linkedInfo;
    EXPECT_FALSE(pWifiP2pGroupManager->IsInclude(10));
    pWifiP2pGroupManager->AddGroup(groupInfo);
    EXPECT_TRUE(pWifiP2pGroupManager->IsInclude(10));
}

HWTEST_F(WifiP2pGroupManagerTest, RefreshGroupsFromCurrentGroup_SUCCESS, TestSize.Level1)
{
    WarpRefreshGroupsFromCurrentGroup();
    WarpRefreshCurrentGroupFromGroups();
}

HWTEST_F(WifiP2pGroupManagerTest, SaveP2pInfo_SUCCESS, TestSize.Level1)
{
    WifiP2pLinkedInfo linkedInfo;
    pWifiP2pGroupManager->SaveP2pInfo(linkedInfo);
    pWifiP2pGroupManager->GetP2pInfo();
}

HWTEST_F(WifiP2pGroupManagerTest, UpdateGroupsNetwork, TestSize.Level1)
{
    EXPECT_TRUE(pWifiP2pGroupManager->AddGroup(groupInfo));
    std::map<int, WifiP2pGroupInfo> wpaGroups;
    pWifiP2pGroupManager->UpdateGroupsNetwork(wpaGroups);
    wpaGroups[1] = groupInfo;
    EXPECT_TRUE(pWifiP2pGroupManager->AddGroup(groupInfo));
    pWifiP2pGroupManager->UpdateGroupsNetwork(wpaGroups);
}
}  // namespace Wifi
}  // namespace OHOS