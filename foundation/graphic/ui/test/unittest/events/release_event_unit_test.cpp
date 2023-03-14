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

#include "events/release_event.h"

#include <climits>
#include <gtest/gtest.h>

using namespace testing::ext;
namespace OHOS {
namespace {
    const Point INIT_POS = { 100, 200 };
}
class ReleaseEventTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    static ReleaseEvent* releaseEvent_;
};

ReleaseEvent* ReleaseEventTest::releaseEvent_ = nullptr;

void ReleaseEventTest::SetUpTestCase(void)
{
    if (releaseEvent_ == nullptr) {
        releaseEvent_ = new ReleaseEvent(INIT_POS);
    }
}

void ReleaseEventTest::TearDownTestCase(void)
{
    if (releaseEvent_ != nullptr) {
        delete releaseEvent_;
        releaseEvent_ = nullptr;
    }
}

/**
 * @tc.name: ReleaseEventConstructor_001
 * @tc.desc: Verify GetCurrentPos function, equal.
 * @tc.type: FUNC
 * @tc.require: SR000DRSH4
 */
HWTEST_F(ReleaseEventTest, ReleaseEventConstructor_001, TestSize.Level0)
{
    EXPECT_EQ(releaseEvent_->GetCurrentPos().x, INIT_POS.x);
    EXPECT_EQ(releaseEvent_->GetCurrentPos().y, INIT_POS.y);
}
} // namespace OHOS
