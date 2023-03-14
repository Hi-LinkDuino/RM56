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

#include "components/ui_toggle_button.h"

#include <climits>
#include <gtest/gtest.h>

using namespace testing::ext;
namespace OHOS {
class UIToggleButtonTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    static UIToggleButton* toggleBtn_;
};

UIToggleButton* UIToggleButtonTest::toggleBtn_ = nullptr;

void UIToggleButtonTest::SetUpTestCase(void)
{
    if (toggleBtn_ == nullptr) {
        toggleBtn_ = new UIToggleButton();
    }
}

void UIToggleButtonTest::TearDownTestCase(void)
{
    if (toggleBtn_ != nullptr) {
        delete toggleBtn_;
        toggleBtn_ = nullptr;
    }
}

/**
 * @tc.name: UIToggleButtonGetViewType_001
 * @tc.desc: Verify GetViewType function.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQA
 */
HWTEST_F(UIToggleButtonTest, UIToggleButtonGetViewType_001, TestSize.Level1)
{
    if (toggleBtn_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    EXPECT_EQ(toggleBtn_->GetViewType(), UI_TOGGLE_BUTTON);
}

/**
 * @tc.name: UIToggleButtonSetState_001
 * @tc.desc: Verify SetState function.
 * @tc.type: FUNC
 * @tc.require: AR000F4E5H
 */
HWTEST_F(UIToggleButtonTest, UIToggleButtonSetState_001, TestSize.Level0)
{
    if (toggleBtn_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    toggleBtn_->SetState(true);
    EXPECT_EQ(toggleBtn_->GetState(), true);

    toggleBtn_->SetState(false);
    EXPECT_EQ(toggleBtn_->GetState(), false);
}
} // namespace OHOS
