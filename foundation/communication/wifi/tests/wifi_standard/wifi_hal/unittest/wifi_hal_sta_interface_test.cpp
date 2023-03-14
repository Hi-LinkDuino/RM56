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
#include "wifi_hal_sta_interface_test.h"
#include "securec.h"
#include "wifi_log.h"
#include "wifi_hal_sta_interface.h"
#include "wifi_supplicant_hal.h"
#include "mock_wpa_ctrl.h"
#include "wifi_hal_common_func.h"

using namespace testing::ext;

namespace OHOS {
namespace Wifi {
void WifiHalStaInterfaceTest::SetUpTestCase()
{
    MockInitGlobalCmd();
    MockInitStaSupportedCmd();
    Start();
}

void WifiHalStaInterfaceTest::TearDownTestCase()
{
    Stop();
}

HWTEST_F(WifiHalStaInterfaceTest, StartTest, TestSize.Level1)
{
    EXPECT_TRUE(Start() == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalStaInterfaceTest, StartSupplicantTest, TestSize.Level1)
{
    EXPECT_TRUE(StartSupplicant() == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalStaInterfaceTest, ConnectSupplicantTest, TestSize.Level1)
{
    EXPECT_TRUE(ConnectSupplicant() == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalStaInterfaceTest, SetPowerSaveTest, TestSize.Level1)
{
    bool enable = false;
    EXPECT_TRUE(SetPowerSave(enable) == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalStaInterfaceTest, RemoveNetworkTest, TestSize.Level1)
{
    EXPECT_TRUE(RemoveNetwork(0) == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalStaInterfaceTest, AddNetworkTest, TestSize.Level1)
{
    EXPECT_TRUE(AddNetwork(nullptr) == WIFI_HAL_FAILED);
    int networkId = 0;
    EXPECT_TRUE(AddNetwork(&networkId) == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalStaInterfaceTest, EnableNetworkTest, TestSize.Level1)
{
    EXPECT_TRUE(EnableNetwork(0) == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalStaInterfaceTest, DisableNetworkTest, TestSize.Level1)
{
    EXPECT_TRUE(DisableNetwork(0) == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalStaInterfaceTest, SetNetworkTest, TestSize.Level1)
{
    EXPECT_TRUE(SetNetwork(0, nullptr, 0) == WIFI_HAL_FAILED);
    SetNetworkConfig conf;
    conf.cfgParam = DEVICE_CONFIG_SSID;
    StrSafeCopy(conf.cfgValue, sizeof(conf.cfgValue), "network_ssid");
    EXPECT_TRUE(SetNetwork(0, &conf, 1) == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalStaInterfaceTest, SaveNetworkConfigTest, TestSize.Level1)
{
    EXPECT_TRUE(SaveNetworkConfig() == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalStaInterfaceTest, ConnectTest, TestSize.Level1)
{
    EXPECT_TRUE(Connect(0) == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalStaInterfaceTest, ReconnectTest, TestSize.Level1)
{
    EXPECT_TRUE(Reconnect() == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalStaInterfaceTest, ReassociateTest, TestSize.Level1)
{
    EXPECT_TRUE(Reassociate() == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalStaInterfaceTest, DisconnectTest, TestSize.Level1)
{
    EXPECT_TRUE(Disconnect() == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalStaInterfaceTest, StartWpsPbcModeTest, TestSize.Level1)
{
    EXPECT_TRUE(StartWpsPbcMode(nullptr) == WIFI_HAL_SUCCESS);
    WifiWpsParam conf;
    ASSERT_TRUE(memset_s(&conf, sizeof(conf), 0, sizeof(conf)) == EOK);
    conf.anyFlag = 1;
    conf.multiAp = 0;
    EXPECT_TRUE(StartWpsPbcMode(&conf) == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalStaInterfaceTest, StartWpsPinModeTest, TestSize.Level1)
{
    EXPECT_TRUE(StartWpsPinMode(nullptr, nullptr) == WIFI_HAL_FAILED);
    WifiWpsParam conf;
    ASSERT_TRUE(memset_s(&conf, sizeof(conf), 0, sizeof(conf)) == EOK);
    conf.anyFlag = 1;
    conf.multiAp = 0;
    int pincode = 0;
    EXPECT_TRUE(StartWpsPinMode(&conf, &pincode) == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalStaInterfaceTest, StopWpsTest, TestSize.Level1)
{
    EXPECT_TRUE(StopWps() == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalStaInterfaceTest, WpaSetCountryCodeTest, TestSize.Level1)
{
    EXPECT_TRUE(WpaSetCountryCode(nullptr) == WIFI_HAL_FAILED);
    EXPECT_TRUE(WpaSetCountryCode("CN") == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalStaInterfaceTest, WpaGetCountryCodeTest, TestSize.Level1)
{
    EXPECT_TRUE(WpaGetCountryCode(nullptr, 0) == WIFI_HAL_FAILED);
    char countryCode[3] = {0};
    int size = 3;
    EXPECT_TRUE(WpaGetCountryCode(countryCode, size) == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalStaInterfaceTest, WpaGetNetWorkTest, TestSize.Level1)
{
    EXPECT_TRUE(WpaGetNetWork(nullptr) == WIFI_HAL_FAILED);
    GetNetworkConfig conf;
    ASSERT_TRUE(memset_s(&conf, sizeof(conf), 0, sizeof(conf)) == EOK);
    conf.networkId = 0;
    StrSafeCopy(conf.param, sizeof(conf.param), "ssid");
    EXPECT_TRUE(WpaGetNetWork(&conf) == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalStaInterfaceTest, WpaAutoConnectTest, TestSize.Level1)
{
    EXPECT_TRUE(WpaAutoConnect(0) == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalStaInterfaceTest, GetNetworkListTest, TestSize.Level1)
{
    EXPECT_TRUE(GetNetworkList(nullptr, nullptr) == WIFI_HAL_FAILED);
    WifiNetworkInfo infos[20];
    ASSERT_TRUE(memset_s(infos, sizeof(infos), 0, sizeof(infos)) == EOK);
    int maxSize = 20;
    EXPECT_TRUE(GetNetworkList(infos, &maxSize) == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalStaInterfaceTest, DisconnectSupplicantTest, TestSize.Level1)
{
    EXPECT_TRUE(DisconnectSupplicant() == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalStaInterfaceTest, StopSupplicantTest, TestSize.Level1)
{
    EXPECT_TRUE(StopSupplicant() == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalStaInterfaceTest, WpaBlocklistClearTest, TestSize.Level1)
{
    EXPECT_TRUE(WpaBlocklistClear() == WIFI_HAL_FAILED);
}

HWTEST_F(WifiHalStaInterfaceTest, GetConnectSignalInfoTest, TestSize.Level1)
{
    EXPECT_TRUE(GetConnectSignalInfo(nullptr, nullptr) == WIFI_HAL_FAILED);
    WpaSignalInfo info;
    ASSERT_TRUE(memset_s(&info, sizeof(info), 0, sizeof(info)) == EOK);
    WifiErrorNo ret = GetConnectSignalInfo("00:00:00:00:00:00", &info);
    EXPECT_TRUE(ret != WIFI_HAL_SUPPLICANT_NOT_INIT && ret != WIFI_HAL_GET_WIFI_COND_FAILED);
}

HWTEST_F(WifiHalStaInterfaceTest, StopTest, TestSize.Level1)
{
    EXPECT_TRUE(Stop() == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalStaInterfaceTest, ForceStopTest, TestSize.Level1)
{
    EXPECT_TRUE(ForceStop() == WIFI_HAL_SUCCESS);
}
}  // namespace Wifi
}  // namespace OHOS