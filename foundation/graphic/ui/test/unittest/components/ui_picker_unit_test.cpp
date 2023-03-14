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

#include "components/ui_picker.h"
#include <climits>
#include <gtest/gtest.h>
#include "gfx_utils/style.h"

using namespace testing::ext;
namespace OHOS {
class UIPickerTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    static UIPicker* picker_;
};

UIPicker* UIPickerTest::picker_ = nullptr;

void UIPickerTest::SetUpTestCase(void)
{
    if (picker_ == nullptr) {
        picker_ = new UIPicker();
    }
}

void UIPickerTest::TearDownTestCase(void)
{
    if (picker_ != nullptr) {
        delete picker_;
        picker_ = nullptr;
    }
}

/**
 * @tc.name: UIPickerGetViewType_001
 * @tc.desc: Verify GetViewType function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ6
 */
HWTEST_F(UIPickerTest, UIPickerGetViewType_001, TestSize.Level1)
{
    if (picker_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    EXPECT_EQ(picker_->GetViewType(), UI_PICKER);
}

/**
 * @tc.name: UIPickerGetHighlightFontId_001
 * @tc.desc: Verify GetHighlightFontId function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ6
 */
HWTEST_F(UIPickerTest, UIPickerGetHighlightFontId_001, TestSize.Level1)
{
    if (picker_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    uint8_t highlightFontId = StyleDefault::GetPickerHighlightStyle().font_;
    EXPECT_EQ(picker_->GetHighlightFontId(), highlightFontId);
}

/**
 * @tc.name: UIPickerSetFontId_001
 * @tc.desc: Verify SetFontId function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ6
 */
HWTEST_F(UIPickerTest, UIPickerSetFontId_001, TestSize.Level1)
{
    if (picker_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    const uint8_t backgroundFontId = 16;
    const uint8_t highlightFontId = 18;

    picker_->SetFontId(backgroundFontId, highlightFontId);
    EXPECT_EQ(picker_->GetBackgroundFontId(), backgroundFontId);
    EXPECT_EQ(picker_->GetHighlightFontId(), highlightFontId);
}

/**
 * @tc.name: UIPickerSetTextColor_001
 * @tc.desc: Verify SetTextColor function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ6
 */
HWTEST_F(UIPickerTest, UIPickerSetTextColor_001, TestSize.Level1)
{
    if (picker_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    ColorType backgroundColor;
    ColorType highlightColor;
    backgroundColor.alpha = OPA_OPAQUE;
    highlightColor.alpha = OPA_OPAQUE;

    picker_->SetTextColor(backgroundColor, highlightColor);
    EXPECT_EQ(picker_->GetBackgroundTextColor().alpha, backgroundColor.alpha);
    EXPECT_EQ(picker_->GetHighlightTextColor().alpha, highlightColor.alpha);
}

/**
 * @tc.name: UIPickerSetWidth_001
 * @tc.desc: Verify SetWidth function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ6
 */
HWTEST_F(UIPickerTest, UIPickerSetWidth_001, TestSize.Level1)
{
    if (picker_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    const int16_t width = 10;
    const int16_t height = 20;

    picker_->SetWidth(width);
    picker_->SetHeight(height);
    EXPECT_EQ(picker_->GetWidth(), width);
    EXPECT_EQ(picker_->GetHeight(), height);
}


/**
 * @tc.name: UIPickerSetSelected_001
 * @tc.desc: Verify SetSelected function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ6
 */
HWTEST_F(UIPickerTest, UIPickerSetSelected_001, TestSize.Level0)
{
    if (picker_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    const uint16_t index = 30;
    const int16_t itemHeight = 10;
    const int16_t width = 100;
    const int16_t height = 100;
    const int16_t startValue= 0;
    const int16_t endValue = 100;
    picker_->SetPosition(0, 0, width, height);
    picker_->SetItemHeight(itemHeight);
    picker_->SetValues(startValue, endValue);
    const char* value[1];

    value[0] = "abc";
    picker_->SetValues(value, 1);
    picker_->SetValues(startValue, endValue);
    picker_->SetSelected(index);
    EXPECT_EQ(picker_->GetSelected(), index);
}
}
