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

#include "UTTest_pin_auth_ui.h"

#include <unistd.h>

#include <memory>

#include "dm_ability_manager.h"
#include "dm_constants.h"
#include "dm_log.h"
#include "nlohmann/json.hpp"
#include "ui_service_mgr_client.h"

namespace OHOS {
namespace DistributedHardware {
void PinAuthUiTest::SetUp()
{
}

void PinAuthUiTest::TearDown()
{
}

void PinAuthUiTest::SetUpTestCase()
{
}

void PinAuthUiTest::TearDownTestCase()
{
}

namespace {
std::shared_ptr<SoftbusConnector> softbusConnector = std::make_shared<SoftbusConnector>();
std::shared_ptr<DeviceManagerServiceListener> listener = std::make_shared<DeviceManagerServiceListener>();
std::shared_ptr<HiChainConnector> hiChainConnector = std::make_shared<HiChainConnector>();
std::shared_ptr<DmAuthManager> authManager =
        std::make_shared<DmAuthManager>(softbusConnector, listener, hiChainConnector);
/**
 * @tc.name: PinAuthUi::ShowPinDialog_001
 * @tc.desc: Call ShowPinDialog to check whether the return value is DM_POINT_NULL
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(PinAuthUiTest, ShowPinDialog_001, testing::ext::TestSize.Level0)
{
    std::shared_ptr<PinAuthUi> pinAuthUi = std::make_shared<PinAuthUi>();
    int32_t code = 123456;
    int32_t ret = pinAuthUi->ShowPinDialog(code, nullptr);
    ASSERT_EQ(ret, DM_OK);
}

/**
 * @tc.name: PinAuthUi::ShowPinDialog_002
 * @tc.desc: Call ShowPinDialog to check whether the return value is DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(PinAuthUiTest, ShowPinDialog_002, testing::ext::TestSize.Level0)
{
    std::shared_ptr<PinAuthUi> pinAuthUi = std::make_shared<PinAuthUi>();
    int32_t code = 123456;
    int32_t ret = pinAuthUi->ShowPinDialog(code, authManager);
    ASSERT_EQ(ret, DM_OK);
}

/**
 * @tc.name: PinAuthUi::ShowPinDialog_001
 * @tc.desc: Call InputPinDialog to check whether the return value is DM_POINT_NULL
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(PinAuthUiTest, InputPinDialog_001, testing::ext::TestSize.Level0)
{
    std::shared_ptr<PinAuthUi> pinAuthUi = std::make_shared<PinAuthUi>();
    int32_t code = 123456;
    int32_t ret = pinAuthUi->InputPinDialog(code, nullptr);
    ASSERT_EQ(ret, DM_OK);
}

/**
 * @tc.name: PinAuthUi::ShowPinDialog_001
 * @tc.desc: Call InputPinDialog to check whether the return value is DM_OK
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(PinAuthUiTest, InputPinDialog_002, testing::ext::TestSize.Level0)
{
    std::shared_ptr<PinAuthUi> pinAuthUi = std::make_shared<PinAuthUi>();
    int32_t code = 123456;
    int32_t ret = pinAuthUi->InputPinDialog(code, authManager);
    ASSERT_EQ(ret, DM_OK);
}
}
} // namespace DistributedHardware
} // namespace OHOS
