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

#include "events/event.h"

#include <climits>
#include <gtest/gtest.h>

using namespace testing::ext;
namespace OHOS {
namespace {
    const Point INIT_POS = { 100, 200 };
    const TimeType TIME_STAMP = 10;
}

class EventTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    static Event* event_;
};

Event* EventTest::event_ = nullptr;

void EventTest::SetUpTestCase(void)
{
    if (event_ == nullptr) {
        event_ = new Event(INIT_POS);
    }
}

void EventTest::TearDownTestCase(void)
{
    if (event_ != nullptr) {
        delete event_;
        event_ = nullptr;
    }
}
/**
 * @tc.name: EventConstructor_001
 * @tc.desc: Verify Constructor function, equal.
 * @tc.type: FUNC
 * @tc.require: SR000DRSH4
 */
HWTEST_F(EventTest, EventConstructor_001, TestSize.Level1)
{
    if (event_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    EXPECT_EQ(event_->GetCurrentPos().x, INIT_POS.x);
    EXPECT_EQ(event_->GetCurrentPos().y, INIT_POS.y);
}

/**
 * @tc.name: EventSetTimeStamp_001
 * @tc.desc: Verify SetTimeStamp function, equal.
 * @tc.type: FUNC
 * @tc.require: SR000DRSH4
 */
HWTEST_F(EventTest, EventSetTimeStamp_001, TestSize.Level0)
{
    if (event_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    event_->SetTimeStamp(TIME_STAMP);
    EXPECT_EQ(event_->GetTimeStamp(), TIME_STAMP);
}
} // namespace OHOS
