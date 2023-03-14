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

#include "wifi_chip_hal_interface_test.h"
#include "wifi_chip_hal_interface.h"
#include "wifi_log.h"
#undef LOG_TAG
#define LOG_TAG "OHWIFI_IDLCLIENT_CHIP_TEST"

using namespace testing::ext;

namespace OHOS {
namespace Wifi {
void OnIfaceAdded(int32_t type, char *ifname)
{
    LOGD("OnIfaceAdded type %d, ifname: %s", type, ((ifname == nullptr) ? "" : ifname));
}

void OnIfaceRemoved(int32_t type, char *ifname)
{
    LOGD("OnIfaceRemoved type %d, ifname: %s", type, ((ifname == nullptr) ? "" : ifname));
}

void WifiChipHalInterfaceTest::SetUpTestCase()
{
    WifiChipEventCallback callback;
    callback.onIfaceAdded = OnIfaceAdded;
    callback.onIfaceRemoved = OnIfaceRemoved;
    WifiChipHalInterface::GetInstance().RegisterChipEventCallback(callback);
}

void WifiChipHalInterfaceTest::TearDownTestCase()
{
    WifiChipEventCallback callback;
    WifiChipHalInterface::GetInstance().RegisterChipEventCallback(callback);
}

HWTEST_F(WifiChipHalInterfaceTest, GetWifiChipObjectTest, TestSize.Level1)
{
    IWifiChip chip;
    WifiErrorNo ret = WifiChipHalInterface::GetInstance().GetWifiChipObject(1, chip);
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiChipHalInterfaceTest, GetChipIdsTest, TestSize.Level1)
{
    std::vector<int> vec;
    WifiErrorNo ret = WifiChipHalInterface::GetInstance().GetChipIds(vec);
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiChipHalInterfaceTest, GetUsedChipIdTest, TestSize.Level1)
{
    int id = 0;
    WifiErrorNo ret = WifiChipHalInterface::GetInstance().GetUsedChipId(id);
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiChipHalInterfaceTest, GetChipCapabilitiesTest, TestSize.Level1)
{
    int capabilities = 0;
    WifiErrorNo ret = WifiChipHalInterface::GetInstance().GetChipCapabilities(capabilities);
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiChipHalInterfaceTest, GetSupportedModesTest, TestSize.Level1)
{
    std::vector<int> vec;
    WifiErrorNo ret = WifiChipHalInterface::GetInstance().GetSupportedModes(vec);
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiChipHalInterfaceTest, ConfigRunModesTest, TestSize.Level1)
{
    WifiErrorNo ret = WifiChipHalInterface::GetInstance().ConfigRunModes(0);
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiChipHalInterfaceTest, GetCurrentModeTest, TestSize.Level1)
{
    int mode = 0;
    WifiErrorNo ret = WifiChipHalInterface::GetInstance().GetCurrentMode(mode);
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiChipHalInterfaceTest, RequestFirmwareDebugInfoTest, TestSize.Level1)
{
    std::string info;
    WifiErrorNo ret = WifiChipHalInterface::GetInstance().RequestFirmwareDebugInfo(info);
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
}

HWTEST_F(WifiChipHalInterfaceTest, SetWifiPowerModeTest, TestSize.Level1)
{
    WifiErrorNo ret = WifiChipHalInterface::GetInstance().SetWifiPowerMode(0);
    EXPECT_TRUE(ret == WIFI_IDL_OPT_OK);
}
}  // namespace Wifi
}  // namespace OHOS