/*
 * Copyright (c) 2020-2022 Huawei Device Co., Ltd.
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

#include <climits>
#include <codecvt>
#include <gtest/gtest.h>
#include <locale>
#include <string>
#include "common/text.h"
#if ENABLE_VECTOR_FONT
#include "common/spannable_string.h"
#else
#include "common/ui_text_language.h"
#endif
#include "gfx_utils/color.h"
#include "gfx_utils/list.h"
#include "gfx_utils/vector.h"

using namespace testing::ext;
namespace OHOS {
class TextTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    static Text* text_;
};

Text* TextTest::text_ = nullptr;

void TextTest::SetUpTestCase(void)
{
    if (text_ == nullptr) {
        text_ = new Text();
    }
}

void TextTest::TearDownTestCase(void)
{
    if (text_ != nullptr) {
        delete text_;
        text_ = nullptr;
    }
}

/**
 * @tc.name: TextSetText_001
 * @tc.desc: Verify SetText function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQ1
 */
HWTEST_F(TextTest, TextSetText_001, TestSize.Level0)
{
    if (text_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    const char* text = "unit test text";
    text_->SetText(text);
    EXPECT_EQ(strcmp(text_->GetText(), text), 0);
}

/**
 * @tc.name: TextSetDirect_001
 * @tc.desc: Verify SetDirect function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQ1
 */
HWTEST_F(TextTest, TextSetDirect_001, TestSize.Level0)
{
    if (text_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    UITextLanguageDirect direct = UITextLanguageDirect::TEXT_DIRECT_LTR;
    text_->SetDirect(direct);
    EXPECT_EQ(text_->GetDirect(), direct);
    direct = UITextLanguageDirect::TEXT_DIRECT_RTL;
    text_->SetDirect(direct);
    EXPECT_EQ(text_->GetDirect(), direct);
}

/**
 * @tc.name: TextSetAlign_001
 * @tc.desc: Verify SetAlign function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQ1
 */
HWTEST_F(TextTest, TextSetAlign_001, TestSize.Level1)
{
    if (text_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    text_->SetAlign(TEXT_ALIGNMENT_LEFT, TEXT_ALIGNMENT_TOP);
    EXPECT_EQ(text_->IsNeedRefresh(), true);
    EXPECT_EQ(text_->GetHorAlign(), TEXT_ALIGNMENT_LEFT);
    EXPECT_EQ(text_->GetVerAlign(), TEXT_ALIGNMENT_TOP);
}

/**
 * @tc.name: TextSetExpand_001
 * @tc.desc: Verify SetExpand function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQ1
 */
HWTEST_F(TextTest, TextSetExpand_001, TestSize.Level1)
{
    if (text_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    EXPECT_EQ(text_->IsExpandWidth(), false);
    text_->SetExpandWidth(true);
    EXPECT_EQ(text_->IsExpandWidth(), true);

    EXPECT_EQ(text_->IsExpandHeight(), false);
    text_->SetExpandHeight(true);
    EXPECT_EQ(text_->IsExpandHeight(), true);
}

HWTEST_F(TextTest, TextSetBackgroundColorSpan_001, TestSize.Level1)
{
    if (text_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    EXPECT_EQ(text_->GetBackgroundColorSpan().Size(), 0);
    text_->SetBackgroundColorSpan(Color::Red(), 0, 2);
    EXPECT_EQ(text_->GetBackgroundColorSpan().Size(), 1);
}

HWTEST_F(TextTest, TextSetForegroundColorSpan_001, TestSize.Level1)
{
    if (text_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    EXPECT_EQ(text_->GetForegroundColorSpan().Size(), 0);
    text_->SetForegroundColorSpan(Color::Blue(), 1, 3);
    EXPECT_EQ(text_->GetForegroundColorSpan().Size(), 1);
}

HWTEST_F(TextTest, TextSetLineBackgroundSpan_001, TestSize.Level1)
{
    if (text_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    EXPECT_EQ(text_->GetLineBackgroundSpan().Size(), 0);
    text_->SetLineBackgroundSpan(Color::Blue(), 1, 3);
    EXPECT_EQ(text_->GetLineBackgroundSpan().Size(), 1);
}

HWTEST_F(TextTest, TextSetAbsoluteSizeSpan_001, TestSize.Level1)
{
    Text* text = new Text();
#if ENABLE_VECTOR_FONT
#else
    text->SetFontId(16);
#endif
    text->SetAbsoluteSizeSpan(1, 2, 38);
    EXPECT_EQ(text->GetSizeSpan(), 0);
    delete text;
    text = nullptr;
}

HWTEST_F(TextTest, TextSetRelativeSpan_001, TestSize.Level1)
{
    Text* text = new Text();
    text->SetRelativeSizeSpan(1, 2, 1.9f);
    EXPECT_EQ(text->GetSizeSpan(), 0);
    delete text;
    text = nullptr;
}
#if ENABLE_VECTOR_FONT
HWTEST_F(TextTest, TextSetStyleSpan_001, TestSize.Level1)
{
    SpannableString spannableString("图形子系统测试正常粗体斜体粗斜体");
    spannableString.SetTextStyle(TEXT_STYLE_ITALIC, 11, 13);
    spannableString.SetTextStyle(TEXT_STYLE_BOLD, 9, 11);
    spannableString.SetTextStyle(TEXT_STYLE_BOLD_ITALIC, 13, 16);
    EXPECT_EQ(spannableString.spanList_.Size(), 3);
}
#endif

#if ENABLE_VECTOR_FONT
HWTEST_F(TextTest, TextSetText_002, TestSize.Level0)
{
    if (text_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    const char* text = "abcd";
    text_->SetText(text);
    EXPECT_EQ(strcmp(text_->GetText(), text), 0);
}
#else
HWTEST_F(TextTest, TextSetText_002, TestSize.Level0)
{
    if (text_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }

    const char* text = "\xEF\x80\x80\xEF\x80\x81\xEF\x80\x82";
    text_->SetText(text);
    EXPECT_EQ(strcmp(text_->GetText(), text), 0);
}
#endif

#if ENABLE_VECTOR_FONT
HWTEST_F(TextTest, TextSetText_003, TestSize.Level0)
{
    if (text_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    const char* text = "鸿蒙操作系统";
    text_->SetText(text);
    EXPECT_EQ(strcmp(text_->GetText(), text), 0);
}
#else
HWTEST_F(TextTest, TextSetText_003, TestSize.Level0)
{
    if (text_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }

    const char* text = "轻量图形子系统\xEF\x80\x80\xEF\x80\x81\xEF\x80\x82鸿蒙操作系統";
    text_->SetText(text);
    EXPECT_EQ(strcmp(text_->GetText(), text), 0);
}
#endif

#if ENABLE_VECTOR_FONT
HWTEST_F(TextTest, TextSetText_004, TestSize.Level1)
{
    if (text_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    const char* text = "鸿蒙操作系统abcd";
    text_->SetText(text);
    EXPECT_EQ(strcmp(text_->GetText(), text), 0);
}
#else
HWTEST_F(TextTest, TextSetText_004, TestSize.Level1)
{
    if (text_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }

    const char* text = "鸿蒙操作系統轻量图形子系统TDD测试用例\xEF\x80\x80\xEF\x80\x81\xEF\x80\x82";
    text_->SetText(text);
    EXPECT_EQ(strcmp(text_->GetText(), text), 0);
}
#endif


#if ENABLE_VECTOR_FONT
HWTEST_F(TextTest, TextSetText_005, TestSize.Level1)
{
    if (text_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    const char* text = "鸿蒙abcd操作系统";
    text_->SetText(text);
    EXPECT_EQ(strcmp(text_->GetText(), text), 0);
}
#else
HWTEST_F(TextTest, TextSetText_005, TestSize.Level1)
{
    if (text_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }

    const char* text = "\xEF\x80\x80鸿蒙操作系統\xEF\x80\x81\xEF\x80\x82轻量图形子系统TDD测试用例";
    text_->SetText(text);
    EXPECT_EQ(strcmp(text_->GetText(), text), 0);
}
#endif

#if ENABLE_VECTOR_FONT
HWTEST_F(TextTest, TextSetText_006, TestSize.Level1)
{
    if (text_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    const char* text = "鸿蒙ab轻量图形子系统cd操作系统";
    text_->SetText(text);
    EXPECT_EQ(strcmp(text_->GetText(), text), 0);
}
#else
HWTEST_F(TextTest, TextSetText_006, TestSize.Level1)
{
    if (text_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }

    const char* text = "轻量图形子系统TDD测试用例\xEF\x80\x80鸿蒙操作系統\xEF\x80\x81测试符号\xEF\x80\x82";
    text_->SetText(text);
    EXPECT_EQ(strcmp(text_->GetText(), text), 0);
}
#endif
} // namespace OHOS
