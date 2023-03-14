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

#include "events/press_event.h"

#include <climits>
#include <gtest/gtest.h>

using namespace testing::ext;
namespace OHOS {
namespace {
    const Point INIT_POS = { 100, 200 };
}
class PressEventTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    static PressEvent* pressEvent_;
};

PressEvent* PressEventTest::pressEvent_ = nullptr;

void PressEventTest::SetUpTestCase(void)
{
    if (pressEvent_ == nullptr) {
        pressEvent_ = new PressEvent(INIT_POS);
    }
}

void PressEventTest::TearDownTestCase(void)
{
    if (pressEvent_ != nullptr) {
        delete pressEvent_;
        pressEvent_ = nullptr;
    }
}

/**
 * @tc.name: PressEventConstructor_001
 * @tc.desc: Verify Constructor function, equal.
 * @tc.type: FUNC
 * @tc.require: SR000DRSH4
 */
HWTEST_F(PressEventTest, PressEventConstructor_001, TestSize.Level0)
{
    if (pressEvent_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    EXPECT_EQ(pressEvent_->GetCurrentPos().x, INIT_POS.x);
    EXPECT_EQ(pressEvent_->GetCurrentPos().y, INIT_POS.y);
}
} // namespace OHOS
