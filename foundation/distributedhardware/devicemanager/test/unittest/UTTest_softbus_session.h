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
#ifndef OHOS_UTTest_DM_SOFTBUS_SESSION_H
#define OHOS_UTTest_DM_SOFTBUS_SESSION_H

#include <gtest/gtest.h>
#include <refbase.h>

#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "device_manager_service_listener.h"
#include "dm_device_state_manager.h"
#include "dm_discovery_manager.h"
#include "inner_session.h"
#include "session.h"
#include "softbus_session.h"
#include "softbus_session_callback.h"
#include "dm_auth_manager.h"
#include "auth_response_state.h"

namespace OHOS {
namespace DistributedHardware {
class SoftbusSessionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif
