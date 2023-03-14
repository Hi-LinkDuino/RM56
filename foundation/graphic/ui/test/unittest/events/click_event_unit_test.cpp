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

#include "events/click_event.h"

#include <climits>
#include <gtest/gtest.h>

using namespace testing::ext;
namespace OHOS {
namespace {
    const Point INIT_POS = { 100, 200 };
}
class ClickEventTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    static ClickEvent* clickEvent_;
};

ClickEvent* ClickEventTest::clickEvent_ = nullptr;

void ClickEventTest::SetUpTestCase(void)
{
    if (clickEvent_ == nullptr) {
        clickEvent_ = new ClickEvent(INIT_POS);
    }
}

void ClickEventTest::TearDownTestCase(void)
{
    if (clickEvent_ != nullptr) {
        delete clickEvent_;
        clickEvent_ = nullptr;
    }
}
/**
 * @tc.name: ClickEventConstructor_001
 * @tc.desc: Verify Constructor function, equal.
 * @tc.type: FUNC
 * @tc.require: SR000DRSH4
 */
HWTEST_F(ClickEventTest, ClickEventConstructor_001, TestSize.Level0)
{
    if (clickEvent_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    EXPECT_EQ(clickEvent_->GetCurrentPos().x, INIT_POS.x);
    EXPECT_EQ(clickEvent_->GetCurrentPos().y, INIT_POS.y);
}
} // namespace OHOS
