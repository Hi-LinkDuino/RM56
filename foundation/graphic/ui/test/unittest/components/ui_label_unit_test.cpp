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

#include "components/ui_label.h"

#include <climits>
#include <gtest/gtest.h>

using namespace testing::ext;
namespace OHOS {
namespace {
    const int16_t INIT_WIDTH = 100;
    const int16_t INIT_HEIGHT = 150;
}

class UILabelTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    static UILabel* label_;
};

UILabel* UILabelTest::label_ = nullptr;

void UILabelTest::SetUpTestCase(void)
{
    if (label_ == nullptr) {
        label_ = new UILabel();
    }
}

void UILabelTest::TearDownTestCase(void)
{
    if (label_ != nullptr) {
        delete label_;
        label_ = nullptr;
    }
}

/**
 * @tc.name: UILabelGetViewType_001
 * @tc.desc: Verify GetViewType and GetHeight function.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQ1
 */
HWTEST_F(UILabelTest, UILabelGetViewType_001, TestSize.Level1)
{
    if (label_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    EXPECT_EQ(label_->GetViewType(), UI_LABEL);
}

/**
 * @tc.name: UILabelResize_001
 * @tc.desc: Verify Resize function.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQ1
 */
HWTEST_F(UILabelTest, UILabelResize_001, TestSize.Level1)
{
    if (label_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    label_->Resize(INIT_WIDTH, INIT_HEIGHT);
    EXPECT_EQ(label_->GetWidth(), INIT_WIDTH);
    EXPECT_EQ(label_->GetHeight(), INIT_HEIGHT);
}

/**
 * @tc.name: UILabelSetText_001
 * @tc.desc: Verify SetText function.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQ1
 */
HWTEST_F(UILabelTest, UILabelSetText_001, TestSize.Level1)
{
    if (label_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    const char* text = "abc";
    label_->Resize(INIT_WIDTH, INIT_HEIGHT);
    label_->SetText(text);

    const char* text0 = label_->GetText();
    ASSERT_TRUE(text0);
    EXPECT_EQ(strcmp(text0, text), 0);
}

/**
 * @tc.name: UILabelSetLineBreakMode_001
 * @tc.desc: Verify SetLineBreakMode function.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQ1
 */
HWTEST_F(UILabelTest, UILabelSetLineBreakMode_001, TestSize.Level0)
{
    if (label_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    const uint8_t lineBreakMode = UILabel::LINE_BREAK_ELLIPSIS;

    label_->SetLineBreakMode(lineBreakMode);
    EXPECT_EQ(label_->GetLineBreakMode(), lineBreakMode);
}

/**
 * @tc.name: UILabelSetTextColor_001
 * @tc.desc: Verify SetTextColor function.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQ1
 */
HWTEST_F(UILabelTest, UILabelSetTextColor_001, TestSize.Level1)
{
    if (label_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    ColorType color = Color::White();

    label_->SetTextColor(color);
    EXPECT_EQ(label_->GetTextColor().full, color.full);
}

/**
 * @tc.name: UILabelSetLongMode_001
 * @tc.desc: Verify SetLongMode function.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQ1
 */
HWTEST_F(UILabelTest, UILabelSetLongMode_001, TestSize.Level0)
{
    if (label_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    label_->SetLineBreakMode(UILabel::LINE_BREAK_ADAPT);
    EXPECT_EQ(label_->GetLineBreakMode(), UILabel::LINE_BREAK_ADAPT);
}

/**
 * @tc.name: UILabelSetAlign_001
 * @tc.desc: Verify SetAlign function.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQ1
 */
HWTEST_F(UILabelTest, UILabelSetAlign_001, TestSize.Level0)
{
    if (label_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    label_->SetAlign(UITextLanguageAlignment::TEXT_ALIGNMENT_RIGHT, UITextLanguageAlignment::TEXT_ALIGNMENT_BOTTOM);
    EXPECT_EQ(label_->GetHorAlign(), UITextLanguageAlignment::TEXT_ALIGNMENT_RIGHT);
    EXPECT_EQ(label_->GetVerAlign(), UITextLanguageAlignment::TEXT_ALIGNMENT_BOTTOM);
}

/**
 * @tc.name: UILabelSetDirect_001
 * @tc.desc: Verify SetDirect function.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQ1
 */
HWTEST_F(UILabelTest, UILabelSetDirect_001, TestSize.Level1)
{
    if (label_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    label_->SetDirect(UITextLanguageDirect::TEXT_DIRECT_RTL);
    EXPECT_EQ(label_->GetDirect(), UITextLanguageDirect::TEXT_DIRECT_RTL);
}

/**
 * @tc.name: UILabelSetRollStartPos_001
 * @tc.desc: Verify SetRollStartPos function.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQ1
 */
HWTEST_F(UILabelTest, UILabelSetRollStartPos_001, TestSize.Level0)
{
    if (label_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    const int16_t rollStartPos = 50;

    label_->SetRollStartPos(rollStartPos);
    EXPECT_EQ(label_->GetRollStartPos(), rollStartPos);
}

/**
 * @tc.name: UILabelSetFont_001
 * @tc.desc: Verify SetFont function.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQ1
 */
HWTEST_F(UILabelTest, UILabelSetFont_001, TestSize.Level1)
{
    if (label_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    uint8_t fontId = label_->GetFontId();

    const uint8_t fontSize = 20;  // 20: font size for test
    label_->SetFont("error_font_name", fontSize);

    EXPECT_EQ(label_->GetFontId(), fontId);
}
} // namespace OHOS
