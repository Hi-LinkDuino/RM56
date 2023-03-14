/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0wifi_p2p_device_manager_test.cpp
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <gtest/gtest.h>

#include "wifi_p2p_device_manager.h"
#include "wifi_logger.h"

using ::testing::ext::TestSize;

namespace OHOS {
namespace Wifi {
class WifiP2pDeviceManagerTest : public testing::Test {
public:
    static void SetUpTestCase(){}
    static void TearDownTestCase(){}
    virtual void SetUp()
    {
        pWifiP2pDeviceManager.reset(new WifiP2pDeviceManager());
        int decCapa = 1;
        int groCapa = 1;

        device1.SetDeviceAddress(std::string("aa:bb:cc:dd:ee:11"));
        device1.SetDeviceName(std::string("Test1"));
        device1.SetDeviceCapabilitys(decCapa);
        device1.SetGroupCapabilitys(groCapa);
        device1.SetP2pDeviceStatus(P2pDeviceStatus::PDS_UNAVAILABLE);
        device1.SetPrimaryDeviceType("Test1 Type");
        device1.SetWpsConfigMethod(static_cast<int>(WpsConfigMethod::WPS_CFG_INVALID));

        device2.SetDeviceAddress(std::string("aa:bb:cc:dd:ee:22"));
        device2.SetDeviceName(std::string("Test2"));
        device2.SetDeviceCapabilitys(decCapa);
        device2.SetGroupCapabilitys(groCapa);
        device2.SetP2pDeviceStatus(P2pDeviceStatus::PDS_UNAVAILABLE);
        device2.SetPrimaryDeviceType("Test2 Type");
        device2.SetWpsConfigMethod(static_cast<int>(WpsConfigMethod::WPS_CFG_INVALID));
    };
    virtual void TearDown()
    {
        pWifiP2pDeviceManager.reset();
    }

public:
    WifiP2pDevice device1;
    WifiP2pDevice device2;
    std::unique_ptr<WifiP2pDeviceManager> pWifiP2pDeviceManager;
};

HWTEST_F(WifiP2pDeviceManagerTest, Initialize_SUCCESS, TestSize.Level1)
{
    pWifiP2pDeviceManager->Initialize();
}

HWTEST_F(WifiP2pDeviceManagerTest, AddDevice_SUCCESS, TestSize.Level1)
{
    WifiP2pDevice device;
    EXPECT_FALSE(pWifiP2pDeviceManager->AddDevice(device));
    EXPECT_TRUE(pWifiP2pDeviceManager->AddDevice(device1));
    EXPECT_FALSE(pWifiP2pDeviceManager->AddDevice(device1));
}

HWTEST_F(WifiP2pDeviceManagerTest, RemoveDevice_SUCCESS, TestSize.Level1)
{
    EXPECT_TRUE(pWifiP2pDeviceManager->AddDevice(device1));
    EXPECT_FALSE(pWifiP2pDeviceManager->AddDevice(device1));
    EXPECT_TRUE(pWifiP2pDeviceManager->RemoveDevice(device1));
    EXPECT_FALSE(pWifiP2pDeviceManager->RemoveDevice(device1));
}

HWTEST_F(WifiP2pDeviceManagerTest, ClearAll_SUCCESS, TestSize.Level1)
{
    EXPECT_TRUE(pWifiP2pDeviceManager->AddDevice(device1));
    EXPECT_EQ(pWifiP2pDeviceManager->ClearAll(), 1);
}

HWTEST_F(WifiP2pDeviceManagerTest, GetDevicesList_SUCCESS, TestSize.Level1)
{
    std::vector<WifiP2pDevice> decVec;
    EXPECT_TRUE(pWifiP2pDeviceManager->AddDevice(device1));
    EXPECT_EQ(pWifiP2pDeviceManager->GetDevicesList(decVec), 1);
    EXPECT_EQ(decVec[0], device1);
}

HWTEST_F(WifiP2pDeviceManagerTest, UpdateDevice_SUCCESS, TestSize.Level1)
{
    WifiP2pDevice device;
    EXPECT_FALSE(pWifiP2pDeviceManager->UpdateDevice(device));

    EXPECT_TRUE(pWifiP2pDeviceManager->AddDevice(device1));
    EXPECT_TRUE(pWifiP2pDeviceManager->UpdateDevice(device1));
    EXPECT_TRUE(pWifiP2pDeviceManager->UpdateDevice(device2));
}

HWTEST_F(WifiP2pDeviceManagerTest, UpdateDeviceSupplicantInf_SUCCESS, TestSize.Level1)
{
    WifiP2pDevice device;
    EXPECT_FALSE(pWifiP2pDeviceManager->UpdateDeviceSupplicantInf(device));

    EXPECT_TRUE(pWifiP2pDeviceManager->AddDevice(device1));
    EXPECT_TRUE(pWifiP2pDeviceManager->UpdateDeviceSupplicantInf(device1));
    EXPECT_TRUE(pWifiP2pDeviceManager->UpdateDeviceSupplicantInf(device2));
}

HWTEST_F(WifiP2pDeviceManagerTest, UpdateGroupCapability_SUCCESS, TestSize.Level1)
{
    EXPECT_FALSE(pWifiP2pDeviceManager->UpdateDeviceGroupCap("", 1));

    EXPECT_TRUE(pWifiP2pDeviceManager->AddDevice(device1));
    EXPECT_TRUE(pWifiP2pDeviceManager->UpdateDeviceGroupCap("aa:bb:cc:dd:ee:11", 1));
    EXPECT_FALSE(pWifiP2pDeviceManager->UpdateDeviceGroupCap("aa:bb:cc:ad:ee:11", 1));
    WifiP2pDevice device;
    EXPECT_FALSE(pWifiP2pDeviceManager->UpdateDeviceGroupCap(device));
    EXPECT_FALSE(pWifiP2pDeviceManager->UpdateDeviceGroupCap(device2));
}

HWTEST_F(WifiP2pDeviceManagerTest, UpdateDeviceStatus_SUCCESS, TestSize.Level1)
{
    WifiP2pDevice device;
    EXPECT_FALSE(pWifiP2pDeviceManager->UpdateDeviceStatus(device));
    EXPECT_FALSE(pWifiP2pDeviceManager->UpdateDeviceStatus(device1));

    EXPECT_FALSE(pWifiP2pDeviceManager->UpdateDeviceStatus("", P2pDeviceStatus::PDS_CONNECTED));
    EXPECT_TRUE(pWifiP2pDeviceManager->AddDevice(device1));
    EXPECT_TRUE(pWifiP2pDeviceManager->UpdateDeviceStatus("aa:bb:cc:dd:ee:11", P2pDeviceStatus::PDS_CONNECTED));
    EXPECT_FALSE(pWifiP2pDeviceManager->UpdateDeviceStatus("aa:bb:cc:dd:ea:11", P2pDeviceStatus::PDS_CONNECTED));
}

HWTEST_F(WifiP2pDeviceManagerTest, GetDevices_SUCCESS, TestSize.Level1)
{
    WifiP2pDevice ret;
    EXPECT_EQ(ret, pWifiP2pDeviceManager->GetDevices(""));

    EXPECT_TRUE(pWifiP2pDeviceManager->AddDevice(device1));
    EXPECT_EQ(device1, pWifiP2pDeviceManager->GetDevices("aa:bb:cc:dd:ee:11"));

    EXPECT_EQ("Test1", pWifiP2pDeviceManager->GetDeviceName("aa:bb:cc:dd:ee:11"));
    EXPECT_EQ("aa:ba:cc:dd:ee:11", pWifiP2pDeviceManager->GetDeviceName("aa:ba:cc:dd:ee:11"));
}
}  // namespace Wifi
}  // namespace OHOS