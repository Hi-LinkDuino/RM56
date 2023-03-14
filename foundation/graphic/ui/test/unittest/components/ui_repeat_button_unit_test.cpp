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

#include "components/ui_repeat_button.h"

#include <climits>
#include <gtest/gtest.h>

using namespace testing::ext;
namespace OHOS {
class UIRepeatButtonTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    static UIRepeatButton* repeatBtn_;
};

UIRepeatButton* UIRepeatButtonTest::repeatBtn_ = nullptr;

void UIRepeatButtonTest::SetUpTestCase()
{
    if (repeatBtn_ == nullptr) {
        repeatBtn_ = new UIRepeatButton();
    }
}

void UIRepeatButtonTest::TearDownTestCase()
{
    if (repeatBtn_ != nullptr) {
        delete repeatBtn_;
        repeatBtn_ = nullptr;
    }
}

/**
 * @tc.name: UIRepeatButtonGetViewType_001
 * @tc.desc: Verify GetViewType function, equal.
 * @tc.type: FUNC
 * @tc.require: SR000DRSH1
 */
HWTEST_F(UIRepeatButtonTest, UIRepeatButtonTestGetViewType_001, TestSize.Level1)
{
    if (repeatBtn_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    EXPECT_EQ(repeatBtn_->GetViewType(), UI_REPEAT_BUTTON);
}

/**
 * @tc.name: UIRepeatButtonSetInterval_001
 * @tc.desc: Verify SetInterval function, equal.
 * @tc.type: FUNC
 * @tc.require: SR000DRSH1
 */
HWTEST_F(UIRepeatButtonTest, UIRepeatButtonSetInterval_001, TestSize.Level0)
{
    if (repeatBtn_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    const uint16_t interval = 10;
    repeatBtn_->SetInterval(interval);
    EXPECT_EQ(repeatBtn_->GetInterval(), interval);
}
} // namespace OHOS
