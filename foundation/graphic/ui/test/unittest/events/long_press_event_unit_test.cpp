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

#include "events/long_press_event.h"

#include <climits>
#include <gtest/gtest.h>

using namespace testing::ext;
namespace OHOS {
namespace {
    const Point INIT_POS = {100, 200};
    const TimeType TIME_STAMP = 10;
}

class LongPressEventTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    static LongPressEvent* longPressEvent_;
};

LongPressEvent* LongPressEventTest::longPressEvent_ = nullptr;

void LongPressEventTest::SetUpTestCase(void)
{
    if (longPressEvent_ == nullptr) {
        longPressEvent_ = new LongPressEvent(INIT_POS, TIME_STAMP);
    }
}

void LongPressEventTest::TearDownTestCase(void)
{
    if (longPressEvent_ != nullptr) {
        delete longPressEvent_;
        longPressEvent_ = nullptr;
    }
}
/**
 * @tc.name: LongPressEventConstructor_001
 * @tc.desc: Verify GetCurrentPos function, equal.
 * @tc.type: FUNC
 * @tc.require: SR000DRSH4
 */
HWTEST_F(LongPressEventTest, LongPressEventConstructor_001, TestSize.Level0)
{
    if (longPressEvent_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    EXPECT_EQ(longPressEvent_->GetCurrentPos().x, INIT_POS.x);
    EXPECT_EQ(longPressEvent_->GetCurrentPos().y, INIT_POS.y);
}

/**
 * @tc.name: LongPressEventConstructor_002
 * @tc.desc: Verify GetCurrentPos function, equal.
 * @tc.type: FUNC
 * @tc.require: SR000DRSH4
 */
HWTEST_F(LongPressEventTest, LongPressEventConstructor_002, TestSize.Level0)
{
    if (longPressEvent_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    EXPECT_EQ(longPressEvent_->GetTimeStamp(), TIME_STAMP);
}
} // namespace OHOS
