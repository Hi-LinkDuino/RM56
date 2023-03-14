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
#include "wifi_hal_chip_interface_test.h"
#include "securec.h"
#include "wifi_hal_chip_interface.h"

using namespace testing::ext;

namespace OHOS {
namespace Wifi {
HWTEST_F(WifiHalChipInterfaceTest, GetWifiChipTest, TestSize.Level1)
{
    uint8_t id = 0;
    WifiChip chip;
    EXPECT_TRUE(GetWifiChip(id, NULL) == WIFI_HAL_SUCCESS);
    EXPECT_TRUE(GetWifiChip(id, &chip) == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalChipInterfaceTest, GetWifiChipIdsTest, TestSize.Level1)
{
    EXPECT_TRUE(GetWifiChipIds(NULL, NULL) == WIFI_HAL_SUCCESS);
    uint8_t ids[32] = {0};
    int size = 32;
    EXPECT_TRUE(GetWifiChipIds(ids, &size) == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalChipInterfaceTest, GetChipIdTest, TestSize.Level1)
{
    EXPECT_TRUE(GetChipId(NULL) == WIFI_HAL_SUCCESS);
    int32_t id = 0;
    EXPECT_TRUE(GetChipId(&id) == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalChipInterfaceTest, CreateRemoveIfaceTest, TestSize.Level1)
{
    EXPECT_TRUE(CreateIface(0, NULL) == WIFI_HAL_FAILED);
    WifiIface iface;
    ASSERT_TRUE(memset_s(&iface, sizeof(iface), 0, sizeof(iface)) == EOK);
    EXPECT_TRUE(CreateIface(0, &iface) == WIFI_HAL_SUCCESS);
    EXPECT_TRUE(iface.type == 0);
    // test RemoveIface
    EXPECT_TRUE(RemoveIface(NULL) == WIFI_HAL_FAILED);
}

HWTEST_F(WifiHalChipInterfaceTest, GetIfaceNamesTest, TestSize.Level1)
{
    EXPECT_TRUE(GetIfaceNames(0, NULL, 0) == WIFI_HAL_SUCCESS);
    char ifaces[128] = {0};
    int size = 128;
    EXPECT_TRUE(GetIfaceNames(0, ifaces, size) == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalChipInterfaceTest, GetCapabilitiesTest, TestSize.Level1)
{
    EXPECT_TRUE(GetCapabilities(NULL) == WIFI_HAL_SUCCESS);
    uint32_t capability = 0;
    EXPECT_TRUE(GetCapabilities(&capability) == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalChipInterfaceTest, GetSupportedComboModesTest, TestSize.Level1)
{
    EXPECT_TRUE(GetSupportedComboModes(NULL, NULL) == WIFI_HAL_FAILED);
    int32_t modes[32] = {0};
    int size = 32;
    WifiErrorNo ret = GetSupportedComboModes(modes, &size);
    EXPECT_TRUE(ret == WIFI_HAL_SUCCESS || ret == WIFI_HAL_NOT_SUPPORT);
}

HWTEST_F(WifiHalChipInterfaceTest, ConfigComboModesTest, TestSize.Level1)
{
    EXPECT_TRUE(ConfigComboModes(0) == WIFI_HAL_NOT_SUPPORT);
}

HWTEST_F(WifiHalChipInterfaceTest, GetComboModesTest, TestSize.Level1)
{
    EXPECT_TRUE(GetComboModes(NULL) == WIFI_HAL_FAILED);
    int mode = 0;
    EXPECT_TRUE(GetComboModes(&mode) == WIFI_HAL_NOT_SUPPORT);
}

HWTEST_F(WifiHalChipInterfaceTest, RequestFirmwareDebugDumpTest, TestSize.Level1)
{
    EXPECT_TRUE(RequestFirmwareDebugDump(NULL, NULL) == WIFI_HAL_FAILED);
    unsigned char bytes[32] = {0};
    int size = 32;
    EXPECT_TRUE(RequestFirmwareDebugDump(bytes, &size) == WIFI_HAL_NOT_SUPPORT);
}

HWTEST_F(WifiHalChipInterfaceTest, SetPowerModeTest, TestSize.Level1)
{
    EXPECT_TRUE(SetPowerMode(0) == WIFI_HAL_SUCCESS);
}
}  // namespace Wifi
}  // namespace OHOS