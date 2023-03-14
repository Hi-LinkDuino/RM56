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
#include "wifi_base_hal_interface_test.h"
#include "wifi_base_hal_interface.h"

using namespace testing::ext;

namespace OHOS {
namespace Wifi {
HWTEST_F(WifiBaseHalInterfaceTest, InitIdlClientTest, TestSize.Level1)
{
    WifiBaseHalInterface iface;
    iface.InitIdlClient();
    ASSERT_TRUE(iface.mIdlClient != nullptr);
}

HWTEST_F(WifiBaseHalInterfaceTest, ExitAllIdlClientTest, TestSize.Level1)
{
    WifiBaseHalInterface iface;
    iface.InitIdlClient();
    ASSERT_TRUE(iface.mIdlClient != nullptr);
    iface.ExitAllIdlClient();
}
}  // namespace Wifi
}  // namespace OHOS