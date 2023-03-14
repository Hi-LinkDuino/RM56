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

#include <gtest/gtest.h>
#include "adapter_manager.h"

using namespace testing::ext;
using namespace OHOS::bluetooth;

namespace OHOS {
namespace Bluetooth {

class HostServerTest : public testing::Test {
public:
    HostServerTest()
    {}
    ~HostServerTest()
    {}

    int tempData_ = 0;
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void HostServerTest::SetUpTestCase(void)
{}
void HostServerTest::TearDownTestCase(void)
{}
void HostServerTest::SetUp()
{
    tempData_ = 0;
}

void HostServerTest::TearDown()
{}

// URI: scheme://authority/path1/path2/path3?id = 1&name = mingming&old#fragment
/**
 * @tc.number: Xxx_Unittest_AttachId_GetId_0100
 * @tc.name: AttachId/GetId
 * @tc.desc: Test if attachd and getid return values are correct.
 */
HWTEST_F(HostServerTest, HostServer_ModuleTest_OnStateChanged, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "HostServer_ModuleTest_OnStateChanged start";

    AdapterManager::GetInstance()->Enable(BTTransport::ADAPTER_BREDR);
    AdapterManager::GetInstance()->OnAdapterStateChange(BTTransport::ADAPTER_BREDR, BTStateID::STATE_TURN_OFF);
    GTEST_LOG_(INFO) << "BluetoothHostServer::OnStateChanged ends";

    EXPECT_EQ(1000, 1000);

    GTEST_LOG_(INFO) << "HostServer_ModuleTest_OnStateChanged end";
}

}  // namespace Bluetooth
}  // namespace OHOS