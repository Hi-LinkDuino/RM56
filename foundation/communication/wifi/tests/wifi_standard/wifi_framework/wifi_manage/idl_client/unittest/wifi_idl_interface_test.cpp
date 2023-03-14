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
#include "wifi_idl_interface_test.h"
#include "i_wifi.h"
#include "i_wifi_chip.h"
#include "i_wifi_iface.h"
#include "i_wifi_sta_iface.h"

using namespace testing::ext;

namespace OHOS {
namespace Wifi {
HWTEST_F(WifiIdlInterfaceTest, GetWifiChipTest, TestSize.Level1)
{
    uint8_t id = 0;
    IWifiChip chip;
    EXPECT_EQ(WifiErrorNo::WIFI_IDL_OPT_OK, GetWifiChip(id, &chip));
}

HWTEST_F(WifiIdlInterfaceTest, GetWifiChipIdsTest, TestSize.Level1)
{
    uint8_t chipIds[8] = {0};
    int32_t size = 8;
    EXPECT_EQ(WifiErrorNo::WIFI_IDL_OPT_OK, GetWifiChipIds(chipIds, &size));
}

HWTEST_F(WifiIdlInterfaceTest, GetChipIdTest, TestSize.Level1)
{
    int32_t id = 0;
    EXPECT_EQ(WifiErrorNo::WIFI_IDL_OPT_OK, GetChipId(&id));
}

HWTEST_F(WifiIdlInterfaceTest, CreateIfaceTest, TestSize.Level1)
{
    int32_t type = 0;
    IWifiIface iface;
    EXPECT_EQ(WifiErrorNo::WIFI_IDL_OPT_OK, CreateIface(type, &iface));
}

HWTEST_F(WifiIdlInterfaceTest, GetIfaceTest, TestSize.Level1)
{
    char ifname[] = "wlan0";
    IWifiIface iface;
    EXPECT_EQ(WifiErrorNo::WIFI_IDL_OPT_OK, GetIface(ifname, &iface));
}

HWTEST_F(WifiIdlInterfaceTest, GetIfaceNamesTest, TestSize.Level1)
{
    char iface[32] = {0};
    int size = 32;
    int32_t type = 0;
    EXPECT_EQ(WifiErrorNo::WIFI_IDL_OPT_OK, GetIfaceNames(type, iface, size));
}

HWTEST_F(WifiIdlInterfaceTest, RemoveIfaceTest, TestSize.Level1)
{
    char ifname[] = "wlan0";
    EXPECT_EQ(WifiErrorNo::WIFI_IDL_OPT_OK, RemoveIface(ifname));
}

HWTEST_F(WifiIdlInterfaceTest, ConfigComboModesTest, TestSize.Level1)
{
    int32_t mode = 0;
    EXPECT_EQ(WifiErrorNo::WIFI_IDL_OPT_OK, ConfigComboModes(mode));
}

HWTEST_F(WifiIdlInterfaceTest, GetComboModesTest, TestSize.Level1)
{
    int32_t mode = 0;
    EXPECT_EQ(WifiErrorNo::WIFI_IDL_OPT_OK, GetComboModes(&mode));
}

HWTEST_F(WifiIdlInterfaceTest, RequestFirmwareDebugDumpTest, TestSize.Level1)
{
    unsigned char buff[128] = {0};
    int size = 128;
    EXPECT_EQ(WifiErrorNo::WIFI_IDL_OPT_OK, RequestFirmwareDebugDump(buff, &size));
}

HWTEST_F(WifiIdlInterfaceTest, SetPowerModeTest, TestSize.Level1)
{
    uint8_t mode = 0;
    EXPECT_EQ(WifiErrorNo::WIFI_IDL_OPT_OK, SetPowerMode(mode));
}

HWTEST_F(WifiIdlInterfaceTest, GetNameTest, TestSize.Level1)
{
    char ifname[32] = {0};
    int size = 32;
    EXPECT_EQ(WifiErrorNo::WIFI_IDL_OPT_OK, GetName(ifname, size));
}

HWTEST_F(WifiIdlInterfaceTest, GetTypeTest, TestSize.Level1)
{
    int32_t type = 0;
    EXPECT_EQ(WifiErrorNo::WIFI_IDL_OPT_OK, GetType(&type));
}

HWTEST_F(WifiIdlInterfaceTest, GetStaCapabilitiesTest, TestSize.Level1)
{
    int32_t capabilities = 0;
    EXPECT_EQ(WifiErrorNo::WIFI_IDL_OPT_OK, GetStaCapabilities(&capabilities));
}

HWTEST_F(WifiIdlInterfaceTest, RunCmdTest, TestSize.Level1)
{
    char ifname[] = "wlan0";
    int32_t cmdId = 0;
    unsigned char buff[] = "run cmd buffer";
    int32_t bufSize = strlen((const char *)buff);
    EXPECT_EQ(WifiErrorNo::WIFI_IDL_OPT_OK, RunCmd(ifname, cmdId, buff, bufSize));
}
}  // namespace Wifi
}  // namespace OHOS