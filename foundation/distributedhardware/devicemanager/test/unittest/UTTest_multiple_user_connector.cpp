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

#include "dm_constants.h"
#include "multiple_user_connector.h"
#include "UTTest_multiple_user_connector.h"

namespace OHOS {
namespace DistributedHardware {
void MultipleUserConnectorTest::SetUp()
{
}
void MultipleUserConnectorTest::TearDown()
{
}
void MultipleUserConnectorTest::SetUpTestCase()
{
}
void MultipleUserConnectorTest::TearDownTestCase()
{
}

namespace {
/**
 * @tc.name: OnDecisionFilterResult_001
 * @tc.desc:  Returns a new pointer to the constructor DmDeviceInfoManager new
 * to construct an environment where the device has been discovered, and stop discovering
 * the device. Its return value is DM_INPUT_PARA_EMPTY
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(MultipleUserConnectorTest, GetCurrentAccountUserID_001, testing::ext::TestSize.Level0)
{
    int32_t ret = DM_OK;
    ret = MultipleUserConnector::GetCurrentAccountUserID();
    ASSERT_TRUE(ret > 0);
}

/**
 * @tc.name: SetSwitchOldUserId_001
 * @tc.desc:  Returns a new pointer to the constructor DmDeviceInfoManager new
 * to construct an environment where the device has been discovered, and stop discovering
 * the device. Its return value is DM_INPUT_PARA_EMPTY
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(MultipleUserConnectorTest, SetSwitchOldUserId_001, testing::ext::TestSize.Level0)
{
    int32_t userIdIn = 1234;
    int32_t userIdOut = -1;
    MultipleUserConnector::SetSwitchOldUserId(userIdIn);
    userIdOut  = MultipleUserConnector::GetSwitchOldUserId();
    ASSERT_EQ(userIdIn, userIdOut);
}

/**
 * @tc.name: GetSwitchOldUserId_001
 * @tc.desc:  Returns a new pointer to the constructor DmDeviceInfoManager new
 * to construct an environment where the device has been discovered, and stop discovering
 * the device. Its return value is DM_INPUT_PARA_EMPTY
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(MultipleUserConnectorTest, GetSwitchOldUserId_001, testing::ext::TestSize.Level0)
{
    int32_t userIdIn = 1234;
    int32_t userIdOut = -1;
    MultipleUserConnector::SetSwitchOldUserId(userIdIn);
    userIdOut  = MultipleUserConnector::GetSwitchOldUserId();
    ASSERT_EQ(userIdIn, userIdOut);
}
}
} // namespace DistributedHardware
} // namespace OHOS
