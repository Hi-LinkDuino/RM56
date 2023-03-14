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

#include "components/ui_label_button.h"

#include <climits>
#include <gtest/gtest.h>
#include "font/ui_font.h"

using namespace testing::ext;
namespace OHOS {
class UILabelButtonTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    static UILabelButton* labelBtn_;
};

UILabelButton* UILabelButtonTest::labelBtn_ = nullptr;

void UILabelButtonTest::SetUpTestCase()
{
    if (labelBtn_ == nullptr) {
        labelBtn_ = new UILabelButton();
    }
}

void UILabelButtonTest::TearDownTestCase()
{
    if (labelBtn_ != nullptr) {
        delete labelBtn_;
        labelBtn_ = nullptr;
    }
}

/**
 * @tc.name: UILabelButtonGetViewType_001
 * @tc.desc: Verify GetViewType function.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ5
 */
HWTEST_F(UILabelButtonTest, UILabelButtonGetViewType_001, TestSize.Level1)
{
    if (labelBtn_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    EXPECT_EQ(labelBtn_->GetViewType(), UI_LABEL_BUTTON);
}

/**
 * @tc.name: UILabelButtonSetText_001
 * @tc.desc: Verify SetText function.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ5
 */
HWTEST_F(UILabelButtonTest, UILabelButtonSetText_001, TestSize.Level1)
{
    if (labelBtn_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    const char* text = "abc";

    labelBtn_->SetText(text);
    if (labelBtn_->GetText() == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    EXPECT_EQ(strcmp(labelBtn_->GetText(), text), 0);
}
/**
 * @tc.name: UILabelButtonSetLablePosition_001
 * @tc.desc: Verify SetLabelPosition function.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ5
 */
HWTEST_F(UILabelButtonTest, UILabelButtonSetLablePosition_001, TestSize.Level1)
{
    if (labelBtn_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    const int16_t posX = 10;
    const int16_t posY = 20;

    labelBtn_->SetLabelPosition(posX, posY);
    EXPECT_EQ(labelBtn_->GetLabelPosition().x, posX);
    EXPECT_EQ(labelBtn_->GetLabelPosition().y, posY);
}

/**
 * @tc.name: UILabelButtonSetAlign_001
 * @tc.desc: Verify SetAlign function.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ5
 */
HWTEST_F(UILabelButtonTest, UILabelButtonSetAlign_001, TestSize.Level0)
{
    if (labelBtn_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    labelBtn_->SetAlign(UITextLanguageAlignment::TEXT_ALIGNMENT_RIGHT);
    EXPECT_EQ(labelBtn_->GetAlign(), UITextLanguageAlignment::TEXT_ALIGNMENT_RIGHT);
}

/**
 * @tc.name: UILabelButtonSetDirect_001
 * @tc.desc: Verify SetDirect function.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ5
 */
HWTEST_F(UILabelButtonTest, UILabelButtonSetDirect_001, TestSize.Level0)
{
    if (labelBtn_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    labelBtn_->SetDirect(UITextLanguageDirect::TEXT_DIRECT_RTL);
    EXPECT_EQ(labelBtn_->GetDirect(), UITextLanguageDirect::TEXT_DIRECT_RTL);
}

/**
 * @tc.name: UILabelButtonSetLabelStyle_001
 * @tc.desc: Verify SetLabelStyle function.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ5
 */
HWTEST_F(UILabelButtonTest, UILabelButtonSetLabelStyle_001, TestSize.Level1)
{
    if (labelBtn_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    Style style;
    style.borderWidth_ = 1;

    labelBtn_->SetLabelStyle(style);
    EXPECT_EQ(labelBtn_->GetLabelStyle().borderWidth_, style.borderWidth_);
}

/**
 * @tc.name: UILabelButtonSetLabelStyle_002
 * @tc.desc: Verify SetLabelStyle function.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ5
 */
HWTEST_F(UILabelButtonTest, UILabelButtonSetLabelStyle_002, TestSize.Level1)
{
    if (labelBtn_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    labelBtn_->SetLabelStyle(STYLE_BORDER_OPA, OPA_TRANSPARENT);
    EXPECT_EQ(labelBtn_->GetLabelStyle(STYLE_BORDER_OPA), OPA_TRANSPARENT);
}

/**
 * @tc.name: UILabelButtonSetTextColor_001
 * @tc.desc: Verify SetTextColor function.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ5
 */
HWTEST_F(UILabelButtonTest, UILabelButtonSetTextColor_001, TestSize.Level1)
{
    if (labelBtn_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    labelBtn_->SetTextColor(Color::Gray());
    EXPECT_EQ(labelBtn_->GetLabelStyle().textColor_.full, Color::Gray().full);
}

/**
 * @tc.name: UILabelButtonSetFont_001
 * @tc.desc: Verify SetFontId function.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ5
 */
HWTEST_F(UILabelButtonTest, UILabelButtonSetFontId_001, TestSize.Level0)
{
    if (labelBtn_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    const uint8_t fontId = 16;
    labelBtn_->SetFontId(fontId);
    if (!UIFont::GetInstance()->IsVectorFont()) {
        EXPECT_EQ(labelBtn_->GetFontId(), fontId);
    } else {
        EXPECT_EQ(labelBtn_->GetFontId(), 0);
    }
}
} // namespace OHOS
