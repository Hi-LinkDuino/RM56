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

#include "components/ui_radio_button.h"

#include <climits>
#include <gtest/gtest.h>
#include "components/root_view.h"
using namespace testing::ext;
namespace OHOS {
class UIRadioButtonTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    static UIRadioButton* radioBtn_;
};

UIRadioButton* UIRadioButtonTest::radioBtn_ = nullptr;

void UIRadioButtonTest::SetUpTestCase()
{
    if (radioBtn_ == nullptr) {
        radioBtn_ = new UIRadioButton();
    }
}

void UIRadioButtonTest::TearDownTestCase()
{
    if (radioBtn_ != nullptr) {
        delete radioBtn_;
        radioBtn_ = nullptr;
    }
}

/**
 * @tc.name: UIRadioButtonGetViewType_001
 * @tc.desc: Verify GetViewType function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQB
 */
HWTEST_F(UIRadioButtonTest, UIRadioButtonGetViewType_001, TestSize.Level1)
{
    if (radioBtn_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    EXPECT_EQ(radioBtn_->GetViewType(), UI_RADIO_BUTTON);
}

/**
 * @tc.name: UIRadioButtonGetName_001
 * @tc.desc: Verify GetName function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000F4E5I
 */
HWTEST_F(UIRadioButtonTest, UIRadioButtonGetName_001, TestSize.Level1)
{
    if (radioBtn_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    const char* name = "group";
    radioBtn_->SetName(name);
    if (radioBtn_->GetName() == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    EXPECT_EQ(strcmp(radioBtn_->GetName(), name), 0);
}

/**
 * @tc.name: UIRadioButtonSetName_001
 * @tc.desc: Verify SetName function, equal.
 * @tc.type: FUNC
 * @tc.require: SR000F3PEE
 */
HWTEST_F(UIRadioButtonTest, UIRadioButtonSetName_001, TestSize.Level0)
{
    if (radioBtn_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    const char* name1 = "group1";
    const char* name2 = "group2";
    UIRadioButton* radioBtn1 = new UIRadioButton(name1);
    UIRadioButton* radioBtn2 = new UIRadioButton();
    radioBtn2->SetName(name1);
    UIRadioButton* radioBtn3 = new UIRadioButton(name1);
    UIRadioButton* radioBtn4 = new UIRadioButton(name2);
    UIRadioButton* radioBtn5 = new UIRadioButton(name2);
    UIViewGroup* view = static_cast<UIViewGroup*>(RootView::GetInstance());
    view->Add(radioBtn1);
    view->Add(radioBtn2);
    view->Add(radioBtn3);
    view->Add(radioBtn4);
    view->Add(radioBtn5);

    ClickEvent event({0, 0});
    radioBtn2->OnClickEvent(event);
    EXPECT_EQ(radioBtn1->GetState(), UICheckBox::UICheckBoxState::UNSELECTED);
    EXPECT_EQ(radioBtn2->GetState(), UICheckBox::UICheckBoxState::SELECTED);
    EXPECT_EQ(radioBtn3->GetState(), UICheckBox::UICheckBoxState::UNSELECTED);
    EXPECT_EQ(radioBtn4->GetState(), UICheckBox::UICheckBoxState::UNSELECTED);
    EXPECT_EQ(radioBtn5->GetState(), UICheckBox::UICheckBoxState::UNSELECTED);

    radioBtn4->OnClickEvent(event);
    EXPECT_EQ(radioBtn1->GetState(), UICheckBox::UICheckBoxState::UNSELECTED);
    EXPECT_EQ(radioBtn2->GetState(), UICheckBox::UICheckBoxState::SELECTED);
    EXPECT_EQ(radioBtn3->GetState(), UICheckBox::UICheckBoxState::UNSELECTED);
    EXPECT_EQ(radioBtn4->GetState(), UICheckBox::UICheckBoxState::SELECTED);
    EXPECT_EQ(radioBtn5->GetState(), UICheckBox::UICheckBoxState::UNSELECTED);

    radioBtn1->OnClickEvent(event);
    EXPECT_EQ(radioBtn1->GetState(), UICheckBox::UICheckBoxState::SELECTED);
    EXPECT_EQ(radioBtn2->GetState(), UICheckBox::UICheckBoxState::UNSELECTED);
    EXPECT_EQ(radioBtn3->GetState(), UICheckBox::UICheckBoxState::UNSELECTED);
    EXPECT_EQ(radioBtn4->GetState(), UICheckBox::UICheckBoxState::SELECTED);
    EXPECT_EQ(radioBtn5->GetState(), UICheckBox::UICheckBoxState::UNSELECTED);

    view->Remove(radioBtn1);
    view->Remove(radioBtn2);
    view->Remove(radioBtn3);
    view->Remove(radioBtn4);
    view->Remove(radioBtn5);
    delete radioBtn1;
    delete radioBtn2;
    delete radioBtn3;
    delete radioBtn4;
    delete radioBtn5;
}
} // namespace OHOS
