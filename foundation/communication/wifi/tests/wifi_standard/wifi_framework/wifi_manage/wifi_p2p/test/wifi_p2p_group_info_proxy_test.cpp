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
#include <gtest/gtest.h>

#include "wifi_p2p_group_info_proxy.h"

using namespace OHOS;
using namespace Wifi;
using ::testing::ext::TestSize;

class WifiP2pGroupInfoProxyTest : public testing::Test {
public:
    static void SetUpTestCase(){}
    static void TearDownTestCase(){}
    virtual void SetUp()
    {
        pWifiP2pGroupInfoProxy.reset(new WifiP2pGroupInfoProxy());
    }
    virtual void TearDown()
    {
        pWifiP2pGroupInfoProxy.reset();
    }

public:
    std::unique_ptr<WifiP2pGroupInfoProxy> pWifiP2pGroupInfoProxy;
};

HWTEST_F(WifiP2pGroupInfoProxyTest, OperatorSuccess, TestSize.Level1)
{
    WifiP2pGroupInfo wifiP2pGroupInfo;

    EXPECT_TRUE((*pWifiP2pGroupInfoProxy) == wifiP2pGroupInfo);
    wifiP2pGroupInfo.SetNetworkId(1);
    pWifiP2pGroupInfoProxy->SetNetworkId(2);
    EXPECT_TRUE((*pWifiP2pGroupInfoProxy) != wifiP2pGroupInfo);
}

HWTEST_F(WifiP2pGroupInfoProxyTest, CompareSuccess, TestSize.Level1)
{
    pWifiP2pGroupInfoProxy->SetIsGroupOwner(true);
    EXPECT_EQ(pWifiP2pGroupInfoProxy->IsGroupOwner(), true);
    WifiP2pDevice device, device1, device2;
    device.SetDeviceName("device");
    pWifiP2pGroupInfoProxy->SetOwner(device);
    EXPECT_EQ(pWifiP2pGroupInfoProxy->GetOwner(), device);
    pWifiP2pGroupInfoProxy->SetPassphrase("12345678");
    EXPECT_EQ(pWifiP2pGroupInfoProxy->GetPassphrase(), "12345678");
    pWifiP2pGroupInfoProxy->SetInterface("iface");
    EXPECT_EQ(pWifiP2pGroupInfoProxy->GetInterface(), "iface");
    pWifiP2pGroupInfoProxy->SetGroupName("groupName");
    EXPECT_EQ(pWifiP2pGroupInfoProxy->GetGroupName(), "groupName");
    pWifiP2pGroupInfoProxy->SetGoIpAddress("ip");
    EXPECT_EQ(pWifiP2pGroupInfoProxy->GetGoIpAddress(), "ip");
    pWifiP2pGroupInfoProxy->SetFrequency(2020);
    EXPECT_EQ(pWifiP2pGroupInfoProxy->GetFrequency(), 2020);
    pWifiP2pGroupInfoProxy->SetIsPersistent(true);
    EXPECT_EQ(pWifiP2pGroupInfoProxy->IsPersistent(), true);
    pWifiP2pGroupInfoProxy->SetP2pGroupStatus(P2pGroupStatus::GS_CREATING);
    EXPECT_EQ(pWifiP2pGroupInfoProxy->GetP2pGroupStatus(), P2pGroupStatus::GS_CREATING);
    pWifiP2pGroupInfoProxy->SetNetworkId(1);
    EXPECT_EQ(pWifiP2pGroupInfoProxy->GetNetworkId(), 1);
    pWifiP2pGroupInfoProxy->AddClientDevice(device1);
    EXPECT_TRUE(pWifiP2pGroupInfoProxy->IsContainsDevice(device1));
    pWifiP2pGroupInfoProxy->RemoveClientDevice(device1);
    std::vector<WifiP2pDevice> devices;
    devices.push_back(device1);
    devices.push_back(device2);
    pWifiP2pGroupInfoProxy->SetClientDevices(devices);
    std::vector<WifiP2pDevice> clientDevices = pWifiP2pGroupInfoProxy->GetClientDevices();
    EXPECT_FALSE(pWifiP2pGroupInfoProxy->IsClientDevicesEmpty());
    pWifiP2pGroupInfoProxy->ClearClientDevices();
    EXPECT_TRUE(pWifiP2pGroupInfoProxy->IsClientDevicesEmpty());
}