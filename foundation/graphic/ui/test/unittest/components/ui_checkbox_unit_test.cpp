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

#include "components/ui_checkbox.h"

#include <climits>
#include <gtest/gtest.h>

using namespace testing::ext;

namespace OHOS {
class UICheckBoxTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    static UICheckBox* checkBox_;
};

UICheckBox* UICheckBoxTest::checkBox_ = nullptr;

void UICheckBoxTest::SetUpTestCase()
{
    if (checkBox_ == nullptr) {
        checkBox_ = new UICheckBox();
    }
}

void UICheckBoxTest::TearDownTestCase()
{
    if (checkBox_ != nullptr) {
        delete checkBox_;
        checkBox_ = nullptr;
    }
}

/**
 * @tc.name: UICheckBoxGetViewType_001
 * @tc.desc: Verify SetState function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQC
 */
HWTEST_F(UICheckBoxTest, UICheckBoxGetViewType_001, TestSize.Level0)
{
    if (checkBox_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    EXPECT_EQ(checkBox_->GetViewType(), UI_CHECK_BOX);
}

/**
 * @tc.name: UICheckBoxOnClickEvent_001
 * @tc.desc: Verify OnClickEvent function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ8
 */
HWTEST_F(UICheckBoxTest, UICheckBoxOnClickEvent_001, TestSize.Level1)
{
    if (checkBox_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    ClickEvent event({0, 0});
    checkBox_->OnClickEvent(event);
    EXPECT_EQ(checkBox_->GetState(), UICheckBox::SELECTED);
}

/**
 * @tc.name: UICheckBoxSetState_001
 * @tc.desc: Verify SetState function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ8
 */
HWTEST_F(UICheckBoxTest, UICheckBoxSetState_001, TestSize.Level0)
{
    if (checkBox_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    checkBox_->SetState(UICheckBox::UICheckBoxState::SELECTED);
    EXPECT_EQ(checkBox_->GetState(), UICheckBox::UICheckBoxState::SELECTED);
    checkBox_->SetState(UICheckBox::UICheckBoxState::UNSELECTED);
    EXPECT_EQ(checkBox_->GetState(), UICheckBox::UICheckBoxState::UNSELECTED);
}
} // namespace OHOS
