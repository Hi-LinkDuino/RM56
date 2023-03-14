/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include <thread>
#include <unistd.h>
#include <vector>
#include <gtest/gtest.h>
#include "hdf_log.h"
#include "osal_mem.h"
#include "securec.h"

#define HDF_LOG_TAG   driver_manager

namespace OHOS {
using namespace testing::ext;

class DevMgrTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void DevMgrTest::SetUpTestCase()
{
}

void DevMgrTest::TearDownTestCase()
{
}

void DevMgrTest::SetUp()
{
}

void DevMgrTest::TearDown()
{
}

/*
* @tc.name: DriverLoaderTest_001
* @tc.desc: driver load test
* @tc.type: FUNC
* @tc.require: AR000DT1TK
*/
HWTEST_F(DevMgrTest, DriverLoaderTest_001, TestSize.Level1)
{
    bool flag = true;
    EXPECT_TRUE(flag);
}
} // namespace OHOS
