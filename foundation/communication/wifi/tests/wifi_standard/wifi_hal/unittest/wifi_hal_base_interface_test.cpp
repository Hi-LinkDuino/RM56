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
#include "wifi_hal_base_interface_test.h"
#include "wifi_hal_base_interface.h"

using namespace testing::ext;

namespace OHOS {
namespace Wifi {
HWTEST_F(WifiHalBaseInterfaceTest, GetNameTest, TestSize.Level1)
{
    EXPECT_TRUE(GetName(NULL, 0) == WIFI_HAL_SUCCESS);
    char ifname[] = "wlan0";
    int size = strlen(ifname);
    EXPECT_TRUE(GetName(ifname, size) == WIFI_HAL_SUCCESS);
}

HWTEST_F(WifiHalBaseInterfaceTest, GetTypeTest, TestSize.Level1)
{
    EXPECT_TRUE(GetType(NULL) == WIFI_HAL_SUCCESS);
    int32_t type = 0;
    EXPECT_TRUE(GetType(&type) == WIFI_HAL_SUCCESS);
}
}  // namespace Wifi
}  // namespace OHOS