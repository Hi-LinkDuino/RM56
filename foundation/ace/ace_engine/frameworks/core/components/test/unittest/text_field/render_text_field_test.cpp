/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "gtest/gtest.h"

#include "core/components/text_field/render_text_field.h"
#include "core/components/text_field/text_field_component.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

constexpr int32_t TEXT_FIELD_MAXLINE_THREE = 3;
constexpr size_t TEXT_FIELD_MAXLENGTH = 100;
const std::string TEXT_FIELD_PLACEHOLDER_VALUE = "please input";
const TextInputType TEXT_FIELD_TEXTINPUTTYPE_EMAIL = TextInputType::EMAIL_ADDRESS;
const TextInputAction TEXT_FIELD_TEXTINPUTACTION_SEARCH = TextInputAction::SEARCH;
const TextAlign TEXT_FIELD_TEXTALIGN_CENTER = TextAlign::CENTER;
constexpr bool TEXT_FIELD_ISENABLE_FALSE = false;

} // namespace

class MockRenderTextField final : public RenderTextField {
    DECLARE_ACE_TYPE(MockRenderTextField, RenderTextField);

public:
    MockRenderTextField() = default;
    ~MockRenderTextField() override = default;

    int32_t GetTextMaxLines() const
    {
        return maxLines_;
    }

    size_t GetMaxLength() const
    {
        return maxLength_;
    }

    bool ShowCursor() const
    {
        return showCursor_;
    }

    const Color& GetCursorColor()
    {
        return cursorColor_;
    }

    const std::string& GetPlaceholder() const
    {
        return placeholder_;
    }

    const Color& GetPlaceholderColor() const
    {
        return placeholderColor_;
    }

    TextInputType GetTextInputType() const
    {
        return keyboard_;
    }

    TextInputAction GetAction() const
    {
        return action_;
    }

    TextAlign GetTextAlign() const
    {
        return textAlign_;
    }

    bool IsEnabled() const
    {
        return enabled_;
    }

    bool IsExtend() const
    {
        return extend_;
    }

    int32_t GetCursorPositionForMoveUp() override
    {
        return 0;
    }

    int32_t GetCursorPositionForMoveDown() override
    {
        return 0;
    }

    int32_t GetCursorPositionForClick(const Offset& offset) override
    {
        return 0;
    }

    double PreferredLineHeight() override
    {
        return 0.0;
    }

    int32_t AdjustCursorAndSelection(int32_t currentCursorPosition) override
    {
        return 0;
    }

    DirectionStatus GetDirectionStatusOfPosition(int32_t position) const override
    {
        return DirectionStatus::LEFT_LEFT;
    }

    bool GetCaretRect(int32_t extent, Rect& caretRect, double caretHeightOffset = 0.0) const override
    {
        return false;
    }

    Offset GetHandleOffset(int32_t extend) override
    {
        return Offset();
    }

    Size ComputeDeflateSizeOfErrorAndCountText() const override
    {
        return Size();
    }
};

class RenderTextFieldTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void RenderTextFieldTest::SetUpTestCase() {}
void RenderTextFieldTest::TearDownTestCase() {}
void RenderTextFieldTest::SetUp() {}
void RenderTextFieldTest::TearDown() {}

/**
 * @tc.name: RenderTextFieldUpdate001
 * @tc.desc: Verify the Update Interface of RenderTextField work correctly with show cursor.
 * @tc.type: FUNC
 * @tc.require: AR000DBI7U
 * @tc.author: hushilong
 */
HWTEST_F(RenderTextFieldTest, RenderTextFieldUpdate001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct TextFieldComponent and RenderTextField.
     */
    RefPtr<TextFieldComponent> textField = AceType::MakeRefPtr<TextFieldComponent>();
    textField->SetShowCursor(true);
    RefPtr<MockRenderTextField> renderTextField = AceType::MakeRefPtr<MockRenderTextField>();

    /**
     * @tc.steps: step2. call the Update interface of RenderTextField.
     * @tc.expected: step2. RenderTextField is set as needLayout,  showCursor_ is set as true
     */
    renderTextField->Update(textField);
    EXPECT_TRUE(renderTextField->NeedLayout());
    EXPECT_EQ(renderTextField->ShowCursor(), true);
}

/**
 * @tc.name: RenderTextFieldUpdate002
 * @tc.desc: Verify the Update Interface of RenderTextField work correctly with textfield component.
 * @tc.type: FUNC
 * @tc.require: AR000DBI7U
 * @tc.author: hushilong
 */
HWTEST_F(RenderTextFieldTest, RenderTextFieldUpdate002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct TextFieldComponent and RenderTextField.
     */
    RefPtr<TextFieldComponent> textField = AceType::MakeRefPtr<TextFieldComponent>();
    textField->SetShowCursor(true);
    textField->SetCursorColor(Color::RED);
    RefPtr<MockRenderTextField> renderTextField = AceType::MakeRefPtr<MockRenderTextField>();

    /**
     * @tc.steps: step2. call the Update interface of RenderTextField.
     * @tc.expected: step2. renderTextField is set as needLayout, cursorColor_ is set as RED.
     */
    renderTextField->Update(textField);
    EXPECT_TRUE(renderTextField->NeedLayout());
    EXPECT_EQ(renderTextField->GetCursorColor(), Color::RED);
}

/**
 * @tc.name: RenderTextFieldUpdate003
 * @tc.desc: Verify the Update Interface of RenderTextField work correctly with maxLength.
 * @tc.type: FUNC
 * @tc.require: AR000DBI7U
 * @tc.author: hushilong
 */
HWTEST_F(RenderTextFieldTest, RenderTextFieldUpdate003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct TextFieldComponent and RenderTextField.
     */
    RefPtr<TextFieldComponent> textField = AceType::MakeRefPtr<TextFieldComponent>();
    textField->SetMaxLength(TEXT_FIELD_MAXLENGTH);
    RefPtr<MockRenderTextField> renderTextField = AceType::MakeRefPtr<MockRenderTextField>();

    /**
     * @tc.steps: step2. call the Update interface of RenderTextField.
     * @tc.expected: step2. RenderTextField is set as needLayout,  maxLength_ is set as 100
     */
    renderTextField->Update(textField);
    EXPECT_TRUE(renderTextField->NeedLayout());
    EXPECT_EQ(renderTextField->GetMaxLength(), TEXT_FIELD_MAXLENGTH);
}

/**
 * @tc.name: RenderTextFieldUpdate004
 * @tc.desc: Verify the Update Interface of RenderTextField work correctly with placeholder.
 * @tc.type: FUNC
 * @tc.require: AR000DBI7U
 * @tc.author: hushilong
 */
HWTEST_F(RenderTextFieldTest, RenderTextFieldUpdate004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct TextFieldComponent and RenderTextField.
     */
    RefPtr<TextFieldComponent> textField = AceType::MakeRefPtr<TextFieldComponent>();
    textField->SetPlaceholderColor(Color::RED);
    textField->SetPlaceholder(TEXT_FIELD_PLACEHOLDER_VALUE);

    RefPtr<MockRenderTextField> renderTextField = AceType::MakeRefPtr<MockRenderTextField>();

    /**
     * @tc.steps: step2. call the Update interface of RenderTextField.
     * @tc.expected: step2. RenderTextField is set as needLayout,  showCursor_ is set as false
     */
    renderTextField->Update(textField);
    EXPECT_TRUE(renderTextField->NeedLayout());
    EXPECT_EQ(renderTextField->GetPlaceholderColor(), Color::RED);
    EXPECT_EQ(renderTextField->GetPlaceholder(), TEXT_FIELD_PLACEHOLDER_VALUE);
}

/**
 * @tc.name: RenderTextFieldUpdate005
 * @tc.desc: Verify the Update Interface of RenderTextField work correctly with maxLines.
 * @tc.type: FUNC
 * @tc.require: AR000DBI7U
 * @tc.author: hushilong
 */
HWTEST_F(RenderTextFieldTest, RenderTextFieldUpdate005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct TextFieldComponent and RenderTextField.
     */
    RefPtr<TextFieldComponent> textField = AceType::MakeRefPtr<TextFieldComponent>();
    textField->SetTextMaxLines(TEXT_FIELD_MAXLINE_THREE);
    RefPtr<MockRenderTextField> renderTextField = AceType::MakeRefPtr<MockRenderTextField>();

    /**
     * @tc.steps: step2. call the Update interface of RenderTextField.
     * @tc.expected: step2. RenderTextField is set as needLayout,  maxLines_ is set as 3
     */
    renderTextField->Update(textField);
    EXPECT_TRUE(renderTextField->NeedLayout());
    EXPECT_EQ(renderTextField->GetTextMaxLines(), TEXT_FIELD_MAXLINE_THREE);
}

/**
 * @tc.name: RenderTextFieldUpdate006
 * @tc.desc: Verify the Update Interface of RenderTextField work correctly with keyboard_.
 * @tc.type: FUNC
 * @tc.require: AR000DBI7U
 * @tc.author: hushilong
 */
HWTEST_F(RenderTextFieldTest, RenderTextFieldUpdate006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct TextFieldComponent and RenderTextField.
     */
    RefPtr<TextFieldComponent> textField = AceType::MakeRefPtr<TextFieldComponent>();
    textField->SetTextInputType(TEXT_FIELD_TEXTINPUTTYPE_EMAIL);
    RefPtr<MockRenderTextField> renderTextField = AceType::MakeRefPtr<MockRenderTextField>();

    /**
     * @tc.steps: step2. call the Update interface of RenderTextField.
     * @tc.expected: step2. RenderTextField is set as needLayout,
     *               keyboard_ is set as TextInputType::EMAIL_ADDRESS
     */
    renderTextField->Update(textField);
    EXPECT_TRUE(renderTextField->NeedLayout());
    EXPECT_EQ(renderTextField->GetTextInputType(), TEXT_FIELD_TEXTINPUTTYPE_EMAIL);
}

/**
 * @tc.name: RenderTextFieldUpdate007
 * @tc.desc: Verify the Update Interface of RenderTextField work correctly with action_.
 * @tc.type: FUNC
 * @tc.require: AR000DBI7U
 * @tc.author: hushilong
 */
HWTEST_F(RenderTextFieldTest, RenderTextFieldUpdate007, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct TextFieldComponent and RenderTextField.
     */
    RefPtr<TextFieldComponent> textField = AceType::MakeRefPtr<TextFieldComponent>();
    textField->SetAction(TEXT_FIELD_TEXTINPUTACTION_SEARCH);
    RefPtr<MockRenderTextField> renderTextField = AceType::MakeRefPtr<MockRenderTextField>();

    /**
     * @tc.steps: step2. call the Update interface of RenderTextField.
     * @tc.expected: step2. RenderTextField is set as needLayout,  action_ is set as TextInputAction::SEARCH
     */
    renderTextField->Update(textField);
    EXPECT_TRUE(renderTextField->NeedLayout());
    EXPECT_EQ(renderTextField->GetAction(), TEXT_FIELD_TEXTINPUTACTION_SEARCH);
}

/**
 * @tc.name: RenderTextFieldUpdate008
 * @tc.desc: Verify the Update Interface of RenderTextField work correctly with textAlign_.
 * @tc.type: FUNC
 * @tc.require: AR000DBI7U
 * @tc.author: hushilong
 */
HWTEST_F(RenderTextFieldTest, RenderTextFieldUpdate008, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct TextFieldComponent and RenderTextField.
     */
    RefPtr<TextFieldComponent> textField = AceType::MakeRefPtr<TextFieldComponent>();
    textField->SetTextAlign(TEXT_FIELD_TEXTALIGN_CENTER);
    RefPtr<MockRenderTextField> renderTextField = AceType::MakeRefPtr<MockRenderTextField>();

    /**
     * @tc.steps: step2. call the Update interface of RenderTextField.
     * @tc.expected: step2. RenderTextField is set as needLayout,  textAlign_ is set as TextAlign::CENTER
     */
    renderTextField->Update(textField);
    EXPECT_TRUE(renderTextField->NeedLayout());
    EXPECT_EQ(renderTextField->GetTextAlign(), TEXT_FIELD_TEXTALIGN_CENTER);
}

/**
 * @tc.name: RenderTextFieldUpdate009
 * @tc.desc: Verify the Update Interface of RenderTextField work correctly with enabled_.
 * @tc.type: FUNC
 * @tc.require: AR000DBI7U
 * @tc.author: hushilong
 */
HWTEST_F(RenderTextFieldTest, RenderTextFieldUpdate009, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct TextFieldComponent and RenderTextField.
     */
    RefPtr<TextFieldComponent> textField = AceType::MakeRefPtr<TextFieldComponent>();
    textField->SetEnabled(TEXT_FIELD_ISENABLE_FALSE);
    RefPtr<MockRenderTextField> renderTextField = AceType::MakeRefPtr<MockRenderTextField>();

    /**
     * @tc.steps: step2. call the Update interface of RenderTextField.
     * @tc.expected: step2. RenderTextField is set as needLayout,  enabled_ is set as false
     */
    renderTextField->Update(textField);
    EXPECT_TRUE(renderTextField->NeedLayout());
    EXPECT_TRUE(!renderTextField->IsEnabled());
}

/**
 * @tc.name: RenderTextFieldUpdate010
 * @tc.desc: Verify the Update Interface of RenderTextField work correctly with extend_.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1Q2
 * @tc.author: caocan
 */
HWTEST_F(RenderTextFieldTest, RenderTextFieldUpdate010, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct TextFieldComponent and RenderTextField.
     */
    RefPtr<TextFieldComponent> textField = AceType::MakeRefPtr<TextFieldComponent>();
    textField->SetExtend(true);
    RefPtr<MockRenderTextField> renderTextField = AceType::MakeRefPtr<MockRenderTextField>();

    /**
     * @tc.steps: step2. call the Update interface of RenderTextField.
     * @tc.expected: step2. RenderTextField is set as needLayout,  extend_ is set as true.
     */
    renderTextField->Update(textField);
    EXPECT_TRUE(renderTextField->NeedLayout());
    EXPECT_TRUE(renderTextField->IsExtend());
}

} // namespace OHOS::Ace