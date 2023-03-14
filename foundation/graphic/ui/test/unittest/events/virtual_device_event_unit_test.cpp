/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "events/virtual_device_event.h"

#include <climits>
#include <gtest/gtest.h>

using namespace testing::ext;
namespace OHOS {
namespace {
    const uint16_t TYPE = 10;
    const uint16_t VALUE = 20;
}

class VirtualDeviceEventTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    static VirtualDeviceEvent* virtualDeviceEvent_;
};

VirtualDeviceEvent* VirtualDeviceEventTest::virtualDeviceEvent_ = nullptr;

void VirtualDeviceEventTest::SetUpTestCase(void)
{
    if (virtualDeviceEvent_ == nullptr) {
        virtualDeviceEvent_ = new VirtualDeviceEvent(TYPE, VALUE);
    }
}

void VirtualDeviceEventTest::TearDownTestCase(void)
{
    if (virtualDeviceEvent_ != nullptr) {
        delete virtualDeviceEvent_;
        virtualDeviceEvent_ = nullptr;
    }
}
/**
 * @tc.name: VirtualDeviceEventConstructor_001
 * @tc.desc: Verify Constructor function, equal.
 * @tc.type: FUNC
 * @tc.require: SR000DRSH4
 */
HWTEST_F(VirtualDeviceEventTest, VirtualDeviceEventConstructor_001, TestSize.Level0)
{
    if (virtualDeviceEvent_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    EXPECT_EQ(virtualDeviceEvent_->GetType(), TYPE);
}

/**
 * @tc.name: VirtualDeviceEventConstructor_002
 * @tc.desc: Verify Constructor function, equal.
 * @tc.type: FUNC
 * @tc.require: SR000DRSH4
 */
HWTEST_F(VirtualDeviceEventTest, VirtualDeviceEventConstructor_002, TestSize.Level0)
{
    if (virtualDeviceEvent_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    EXPECT_EQ(virtualDeviceEvent_->GetState(), VALUE);
}
} // namespace OHOS
