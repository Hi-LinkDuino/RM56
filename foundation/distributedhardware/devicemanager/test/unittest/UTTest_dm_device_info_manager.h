/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef OHOS_UTTEST_DM_DEVICEINFO_MANAGER_H
#define OHOS_UTTEST_DM_DEVICEINFO_MANAGER_H

#include <gtest/gtest.h>
#include <refbase.h>

#include <string>
#include <vector>

#include "device_manager_service_listener.h"
#include "dm_adapter_manager.h"
#include "dm_device_info.h"
#include "dm_device_info_manager.h"
#include "softbus_connector.h"
#include "softbus_session_callback.h"

namespace OHOS {
namespace DistributedHardware {
class DeviceManagerImplTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif
