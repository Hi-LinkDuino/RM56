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

#include "events/cancel_event.h"

#include <climits>
#include <gtest/gtest.h>

using namespace testing::ext;
namespace OHOS {
namespace {
    const Point INIT_POS = { 100, 200 };
}
class CancelEventTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    static CancelEvent* cancelEvent_;
};

CancelEvent* CancelEventTest::cancelEvent_ = nullptr;

void CancelEventTest::SetUpTestCase(void)
{
    if (cancelEvent_ == nullptr) {
        cancelEvent_ = new CancelEvent(INIT_POS);
    }
}

void CancelEventTest::TearDownTestCase(void)
{
    if (cancelEvent_ != nullptr) {
        delete cancelEvent_;
        cancelEvent_ = nullptr;
    }
}

/**
 * @tc.name: CancelEventConstructor_001
 * @tc.desc: Verify Constructor function, equal.
 * @tc.type: FUNC
 * @tc.require: SR000DRSH4
 */
HWTEST_F(CancelEventTest, CancelEventConstructor_001, TestSize.Level0)
{
    if (cancelEvent_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    EXPECT_EQ(cancelEvent_->GetCurrentPos().x, INIT_POS.x);
    EXPECT_EQ(cancelEvent_->GetCurrentPos().y, INIT_POS.y);
}
} // namespace OHOS
