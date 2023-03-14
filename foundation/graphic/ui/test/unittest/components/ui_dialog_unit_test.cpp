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

#include "components/root_view.h"
#include "components/ui_dialog.h"

#include <climits>
#include <gtest/gtest.h>

#if ENABLE_WINDOW
#if ENABLE_DEBUG
using namespace testing::ext;
namespace OHOS {
class UIDialogTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    static UIDialog* dialog_;
    void SetUp(void);
    void TearDown(void);
};

UIDialog* UIDialogTest::dialog_ = nullptr;
UIView::OnClickListener* listener_ = nullptr;

void UIDialogTest::SetUpTestCase(void)
{
    listener_ = new UIView::OnClickListener();
}

void UIDialogTest::SetUp(void)
{
    if (dialog_ != nullptr) {
        delete dialog_;
    }
    dialog_ = new UIDialog();
}


void UIDialogTest::TearDown(void)
{
    if (dialog_ != nullptr) {
        delete dialog_;
        dialog_ = nullptr;
    }
}

void UIDialogTest::TearDownTestCase(void)
{
    if (listener_ != nullptr) {
        delete listener_;
        listener_ = nullptr;
    }
}

/**
 * @tc.name: UIDialogSetTitle_001
 * @tc.desc: Verify SetTitle function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000F4E5F
 */
HWTEST_F(UIDialogTest, UIDialogSetTitle_001, TestSize.Level0)
{
    const char* title1 = "title1";
    dialog_->SetTitle(title1);
    EXPECT_EQ(dialog_->GetTitle(), title1);
    const char* title2 = "title2";
    dialog_->SetTitle(title2);
    EXPECT_EQ(dialog_->GetTitle(), title2);
}

/**
 * @tc.name: UIDialogSetTitle_002
 * @tc.desc: Verify SetTitle function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000F4E5F
 */
HWTEST_F(UIDialogTest, UIDialogSetTitle_002, TestSize.Level1)
{
    dialog_->SetTitle(nullptr);
    EXPECT_EQ(dialog_->GetTitle(), nullptr);
    const char* title = "title";
    dialog_->SetTitle(title);
    dialog_->SetTitle(nullptr);
    EXPECT_EQ(dialog_->GetTitle(), title);
}

/**
 * @tc.name: UIDialogSetText_001
 * @tc.desc: Verify SetText function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000F4E5F
 */
HWTEST_F(UIDialogTest, UIDialogSetText_001, TestSize.Level0)
{
    const char* text1 = "text1";
    dialog_->SetText(text1);
    EXPECT_EQ(dialog_->GetText(), text1);
    const char* text2 = "text2";
    dialog_->SetText(text2);
    EXPECT_EQ(dialog_->GetText(), text2);
}

/**
 * @tc.name: UIDialogSetText_002
 * @tc.desc: Verify SetText function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000F4E5F
 */
HWTEST_F(UIDialogTest, UIDialogSetText_002, TestSize.Level1)
{
    dialog_->SetText(nullptr);
    EXPECT_EQ(dialog_->GetText(), nullptr);
    const char* text = "text";
    dialog_->SetText(text);
    dialog_->SetText(nullptr);
    EXPECT_EQ(dialog_->GetText(), text);
}

/**
 * @tc.name: UIDialogSetButton_001
 * @tc.desc: Verify SetButton function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000F4E5F
 */
HWTEST_F(UIDialogTest, UIDialogSetButton_001, TestSize.Level0)
{
    const char* buttonText = "button";
    dialog_->SetButton(UIDialog::DialogButtonType::BUTTON_LEFT, buttonText, listener_);
    const char* buttonText0 = dialog_->GetButtonText(UIDialog::DialogButtonType::BUTTON_LEFT);
    ASSERT_TRUE(buttonText0);
    if (strcmp(buttonText0, buttonText) != 0) {
        EXPECT_EQ(1, 0);
    }
    EXPECT_EQ(dialog_->GetButtonListener(UIDialog::DialogButtonType::BUTTON_LEFT), listener_);
}

/**
 * @tc.name: UIDialogSetButtonColor_001
 * @tc.desc: Verify SetButtonColor function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000F4E5F
 */
HWTEST_F(UIDialogTest, UIDialogSetButtonColor_001, TestSize.Level1)
{
    dialog_->SetButton(UIDialog::DialogButtonType::BUTTON_LEFT, "button", listener_);
    ColorType color = Color::Red();
    dialog_->SetButtonColor(UIDialog::DialogButtonType::BUTTON_LEFT, color);
    EXPECT_EQ(dialog_->GetButtonColor(UIDialog::DialogButtonType::BUTTON_LEFT).full, color.full);
}

/**
 * @tc.name: UIDialogSetOnCancelListener_001
 * @tc.desc: Verify SetOnCancelListener function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000F4E5F
 */
HWTEST_F(UIDialogTest, UIDialogSetOnCancelListener_001, TestSize.Level1)
{
    dialog_->SetOnCancelListener(nullptr);
    EXPECT_EQ(dialog_->GetOnCancelListener(), nullptr);
    dialog_->SetOnCancelListener(listener_);
    EXPECT_EQ(dialog_->GetOnCancelListener(), listener_);
}

/**
 * @tc.name: UIDialogEnableAutoCancel_001
 * @tc.desc: Verify EnableAutoCancel function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000F4E5F
 */
HWTEST_F(UIDialogTest, UIDialogEnableAutoCancel_001, TestSize.Level1)
{
    dialog_->EnableAutoCancel(true);
    EXPECT_EQ(dialog_->GetEnableAutoCancel(), true);
    dialog_->EnableAutoCancel(false);
    EXPECT_EQ(dialog_->GetEnableAutoCancel(), false);
}

/**
 * @tc.name: UIDialog_001
 * @tc.desc: Verify UIDialog function, equal.
 * @tc.type: FUNC
 * @tc.require: SR000F3PED
 */
HWTEST_F(UIDialogTest, UIDialog_001, TestSize.Level0)
{
    const char* title = "title";
    dialog_->SetTitle(title);
    EXPECT_EQ(dialog_->GetTitle(), title);
    const char* text = "text";
    dialog_->SetText(text);
    EXPECT_EQ(dialog_->GetText(), text);
    const char* buttonText = "button";
    dialog_->SetButton(UIDialog::DialogButtonType::BUTTON_LEFT, buttonText, listener_);
    const char* buttonText0 = dialog_->GetButtonText(UIDialog::DialogButtonType::BUTTON_LEFT);
    ASSERT_TRUE(buttonText0);
    if (strcmp(buttonText0, buttonText) != 0) {
        EXPECT_EQ(1, 0);
    }
    EXPECT_EQ(dialog_->GetButtonListener(UIDialog::DialogButtonType::BUTTON_LEFT), listener_);
    dialog_->SetOnCancelListener(listener_);
    EXPECT_EQ(dialog_->GetOnCancelListener(), listener_);
    dialog_->EnableAutoCancel(true);
    EXPECT_EQ(dialog_->GetEnableAutoCancel(), true);
}
} // namespace OHOS
#endif // ENABLE_DEBUG
#endif // ENABLE_WINDOW
